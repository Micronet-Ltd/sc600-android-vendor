/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <private/android_filesystem_config.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include "NetdClient.h"
#include "resolv_netid.h"

#include "../pal/pal_internal.h"
#include "omadm_service_api.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "OmadmServiceAPI"

using namespace android;

    /*
     * PAL proxy to omadm_service in java framework.
     * Keep in sync with android.omadm.IOmadmManager.
     *
     */

namespace omadm_service_api {

    /*
     * Helpers
     */

    // Internal method unsynchronized
    int BpOmadmServiceAPI::initBinder(bool wait_forever) {
        sp<IBinder> binder = NULL;

        //get service manager
        sp<IServiceManager> sm = defaultServiceManager();
        do{
            //Search service by SERVICE_NAME
            binder = sm->getService(String16(SERVICE_NAME));
            if((binder != NULL)||(!wait_forever))
                break;
            ALOGE("Service %s is not found. Waiting...", SERVICE_NAME);
            sleep(1);
        } while(1);

        if (binder == NULL)
            return ERROR_SYSCALL;

        if (mDeathNotifier == NULL) {
            mDeathNotifier = new DeathNotifier();
        }
        binder->linkToDeath(mDeathNotifier);

        mBindrToService = interface_cast<IOmadmServiceAPI>(binder);
        if (mBindrToService == NULL) {
            ALOGE("Interface cast failed %s", SERVICE_NAME);
            return ERROR_SYSCALL;
        }

        // Start a Binder thread pool to receive Death notification callbacks
        sp<ProcessState> proc(ProcessState::self());
        ProcessState::self()->startThreadPool();

        ALOGI("PAL has started");
        mInitialized = true;

        return NO_ERROR;
    }

    int BpOmadmServiceAPI::isPalInitialized() {
        if(mInitialized) {
            return true;
        } else {
            ALOGW("PAL is not initialized");
            int reply = BpOmadmServiceAPI::initBinder(true);
            return (NO_ERROR == reply) ? true : false;
        }
    }

    BpOmadmServiceAPI::BpOmadmServiceAPI(const sp<IBinder>& impl ):BpInterface<IOmadmServiceAPI>(impl) {
        if (mOmadmCallbackData == NULL) {
            mOmadmCallbackData = new KeyedVector<omadmCallback_id, omadmCallback>();
        }
    }

    /**
     * Death Notifier implementation. (Callback)
     * Catch Binder Died notification in cases of system server
     * crash. This is unlikely event but we will need to
     * re-initialise PAL in this case.
     * param@ who is died
     */
    void DeathNotifier::binderDied(const wp<IBinder>& who) {
        ALOGW("Omadm Service: Binder Died");
        UNUSED(who);
        BpOmadmServiceAPI::mInitialized = false;
    }

    /**
     * Omadm Service Listener implementation.
     * Sends request to fumo plugin about user reply action
     * param@ data data from java client
     */
    void OmadmListener::omadmFumoPluginDispachUserReply(
        int state, int64_t defered_update_time, bool wifi_requred,
        bool automatic_update_enable, int button_id) {
        ALOGD("OmadmListener::omadmFumoPluginDispachUserReply");

        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_FUMO_USER_REPLY);
        if (index < 0) {
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
            // Prepare omadm callback structure
            pal_fumo_gui_user_reply_param* fgurp =
                ( pal_fumo_gui_user_reply_param*)malloc(sizeof(pal_fumo_gui_user_reply_param));
            if (fgurp != NULL) {
                memset(fgurp, 0, sizeof(pal_fumo_gui_user_reply_param));
                fgurp->state = state;
                fgurp->defered_update_time = defered_update_time;
                fgurp->wifi_requred = wifi_requred;
                fgurp->automatic_update_enable = automatic_update_enable;
                fgurp->button_id = button_id;

                omadmCallback((void*)fgurp);
            }
        }
    }

    /**
     * Omadm Service Listener implementation.
     * Sends request to controller about user reply action
     */
    void OmadmListener::omadmControllerDispachUserReply(
        int type, String16 input_text, int selected_choices_count,
        String16 selected_choices, int button_id) {
        ALOGD("OmadmListener::omadmControllerDispachUserReply");
        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_CONTROLLER_USER_REPLY);
        if (index < 0) {
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
            // Prepare omadm callback structure
            pal_omadm_controller_gui_user_reply_param* cgurp =
                ( pal_omadm_controller_gui_user_reply_param*)malloc(sizeof(pal_omadm_controller_gui_user_reply_param));
            if (cgurp != NULL) {
                memset(cgurp, 0, sizeof(pal_omadm_controller_gui_user_reply_param));
                cgurp->type = type;
                if (input_text.size() > 0) {
                    cgurp->input_text = String8(input_text).string();
                } else {
                    cgurp->input_text = String8(" ").string();
                }
                cgurp->selected_choices_count = selected_choices_count;
                if (selected_choices.size() > 0) {
                    cgurp->selected_choices = omadmStringSplit(String8(selected_choices),
                        OMADM_DELIMITER, selected_choices_count);
                } else {
                    cgurp->selected_choices = omadmStringSplit(String8(" "),
                        OMADM_DELIMITER, selected_choices_count);
                }
                cgurp->button_id = button_id;
                omadmCallback((void*)cgurp);
            }
        }
    }

    /**
     * Omadm Listener utility function.
     *
     */
    const char** OmadmListener::omadmStringSplit(const String8& str,
            const char* delimiter, int counter) {
        const char** array = (const char**)malloc((counter + 1)*sizeof(char*));
        const char* p = str.string();
        const char* q;

        int i = 0;
        while (true) {
            q = strstr(p, delimiter);
            if (q == NULL) {
                array[i] = strndup(p, strlen(p));
                return array;
            }

            array[i] = strndup(p, q - p);
            p = q + strlen(delimiter);
           i++;
        }
        return array;
    }

    /**
     * Omadm Service Listener implementation.
     * Sends callback to Network manager about network status changes
     * @param net_feature is defined by network_conditions_type_t
     * @param enabled defines whether proper feature is enabled or not
     * @note Allocates structure network_conditions_t. This is caller
     * responsibility to free it.
     */
    void OmadmListener::omadmNetManagerReply(int net_feature, bool enabled) {
        ALOGD("OmadmListener::omadmNetManagerReply");

        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_NET_MANAGER_REPLY);
        if (index < 0) {
            ALOGD("OmadmListener::omadmNetManagerReply listener for \
            OMADM_NET_MANAGER_REPLY wasn't properly set");
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);    
        if (omadmCallback != NULL) {
            int nc_len = sizeof(network_conditions_t);
            network_conditions_t *nc = (network_conditions_t *)malloc(nc_len);
            if (nc != NULL) {
                memset(nc, 0, nc_len);
                nc->net_feature = static_cast<network_conditions_type_t>(net_feature);
                nc->enabled = enabled;
                omadmCallback(nc);
            }
        }
    }

    /**
     * Omadm Service Listener implementation.
     * Sends request to scomo plugin about battery state
     * @param battery_status current battery status
     * @param battery_level current battery level
     */
    void OmadmListener::omadmScomoPluginDispachBatteryState(
        int battery_status, int battery_level) {
        ALOGD("OmadmListener::omadmScomoPluginDispachBatteryState");

        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_SCOMO_GET_BATTERY_STATE);
        if (index < 0) {
            ALOGD("OmadmListener::omadmScomoPluginDispachBatteryState listener for \
            OMADM_SCOMO_GET_BATTERY_STATE wasn't properly set");
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
            // Prepare omadm callback structure
            pal_battery_state* bsp =
                ( pal_battery_state*)malloc(sizeof(pal_battery_state));
            if (bsp != NULL) {
                memset(bsp, 0, sizeof(pal_battery_state));
                bsp->battery_status = battery_status;
                bsp->battery_level = battery_level;
                omadmCallback((void*)bsp);
            }
        }
    }

    /**
     * Omadm Service Listener implementation.
     * Sends request to fumo plugin about battery state
     * @param battery_status current battery status
     * @param battery_level current battery level
     */
    void OmadmListener::omadmFumoPluginDispachBatteryState(
        int battery_status, int battery_level) {
        ALOGD("OmadmListener::omadmFumoPluginDispachBatteryState");

        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_FUMO_GET_BATTERY_STATE);
        if (index < 0) {
            ALOGD("OmadmListener::omadmFumoPluginDispachBatteryState listener for \
            OMADM_FUMO_GET_BATTERY_STATE wasn't properly set");
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
            // Prepare omadm callback structure
            pal_battery_state* bsp =
                ( pal_battery_state*)malloc(sizeof(pal_battery_state));
            if (bsp != NULL) {
                memset(bsp, 0, sizeof(pal_battery_state));
                bsp->battery_status = battery_status;
                bsp->battery_level = battery_level;
                omadmCallback((void*)bsp);
            }
        }
    }

    /**
     * Omadm Service Listener implementation.
     * Sends request to fumo about pressing check for update button action
     */
    void OmadmListener::omadmFumoPluginDispachCheckForUpdate()
    {
        ALOGD("OmadmListener::omadmFumoPluginDispachCheckForUpdate");
        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_FUMO_CHECK_FOR_UPDATE);
        if (index < 0) {
            return;
        }

        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
                omadmCallback((void*)NULL);
        }
    }

    /**
     * This method returns device IMEI as String16
     * @return string with IMEI on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getDeviceId()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_DEVICE_ID, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getDeviceId() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method sends user notification
     * @param string: notification title
     * @param string: notification body
     * @return status of execution: true on success
     */
    bool BpOmadmServiceAPI::sendNotification(String16* title, String16* message)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeString16(*title);
        data.writeString16(*message);
        remote()->transact(SEND_NOTIFICATION, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("sendNotification() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    /**
     * This method registers omadm callback and listener
     * @param omadmCallbackId omadm callback type
     * @param omadmcallback omadm callback
     * @return status of execution: NO_ERROR on success
     */
    int BpOmadmServiceAPI::registerOmadmCallback(
                    omadmCallback_id omadmCallbackId, omadmCallback omadmcallback)
    {
        ALOGD("BpOmadmServiceAPI::addOmadmCallback");
        int status = NO_ERROR;
        // Add Omadm Callback
        mOmadmCallbackData->add(omadmCallbackId, omadmcallback);

        // Register Omadm Listener for Java Client
        if (OmadmListener::mOmadmListener == NULL) {
            OmadmListener::mOmadmListener = new OmadmListener();
            Parcel data, reply;
            data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
            data.writeStrongBinder(IInterface::asBinder(OmadmListener::mOmadmListener));
            remote()->transact(REGISTER_OMADM_LISTENER, data, &reply);
            int32_t err = reply.readExceptionCode();
            if (err < 0) {
              ALOGE("remote exception: %d", err);
              return err;
            }
            status = reply.readInt32();
        }

        return status;
    }

     /**
     * This method unregisters omadm callback and listener
     * @param omadmCallbackId omadm callback type
     * @return status of execution: NO_ERROR on success
     */
    int BpOmadmServiceAPI::unregisterOmadmCallback(omadmCallback_id omadmCallbackId)
    {
        ALOGD("BpOmadmServiceAPI::removeOmadmCallback");
        int status = NO_ERROR;

        // Remove Omadm Callback
        ssize_t index = mOmadmCallbackData->indexOfKey(omadmCallbackId);
        if (index < 0) {
            return ERROR_REMOVE_OMADM_CALLBACK;
        }
        mOmadmCallbackData->removeItemsAt(index);

        // UnRegister Omadm Listener for Java Client
        if (mOmadmCallbackData->isEmpty())
        {
            Parcel data, reply;
            data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
            data.writeStrongBinder(IInterface::asBinder(OmadmListener::mOmadmListener));
            remote()->transact(UNREGISTER_OMADM_LISTENER, data, &reply);
            int32_t err = reply.readExceptionCode();
            if (err < 0) {
              ALOGE("remote exception: %d", err);
              return err;
            }
            OmadmListener::mOmadmListener = NULL;
            status = reply.readInt32();
        }

        return status;
    }

    /**
     * This method is used to call the "void <method name>(int arg)"
     * remote method which receives arguments (int arg0) and returns
     * status via exception if something error occurs during operation
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param arg0 1st argument for remote method
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0)
    {
        DBGPRINT("code = %d, arg = %d", code, arg0);
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        DBGPRINT("code = %d, arg = %d : OK", code, arg0);
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the "int <method name>()"
     * remote method which gets int value end returns
     * status via exception if something error occurs during operation
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param p_result pointer to store read value
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t* p_result)
    {
        DBGPRINT("code = %d, p_result = %p", code, p_result);
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        result.readInt32(p_result);
        DBGPRINT("code = %d, *p_result = %d", code, *p_result);
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the "int64_t <method name>(int)"
     * remote method which gets int value end returns
     * status via exception if something error occurs during operation
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param p_result pointer to store read value
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0, int64_t* p_result)
    {
        DBGPRINT("(%d, %d, %p)", code, arg0, p_result);
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        result.readInt64(p_result);
        DBGPRINT("(%d, %d, &(%lld))", code, arg0, *p_result);
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the "int <method name>(int arg)"
     * remote method which receives arguments (int arg0) and returns int value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param arg0 1st argument for remote method
     * @param p_result pointer to store read value
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0, int32_t* p_result)
    {
        DBGPRINT("code = %d, arg = %d, p_result = %p", code, arg0, p_result);
        if (p_result == NULL) {
            return RESULT_ERROR_INVALID_ARGS;
        }
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        result.readInt32(p_result);
        DBGPRINT("*p_result = %d", *p_result);
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the "String <method name>(int arg)"
     * remote method which receives arguments (int arg0)
     * and returns String value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param arg 1st argument for remote method
     * @param p_result buffer to store c-string
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg,
        data_buffer_t* p_result)
    {
        DBGPRINT("code = %d, arg = %d, p_result = %p", code, arg, p_result);
        if (p_result == NULL) {
            return RESULT_ERROR_INVALID_ARGS;
        }
        if (p_result->data == NULL) {
            return RESULT_EMPTY_BUFFER;
        }
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        String8 str_result(result.readString16());
        DBGPRINT("str_result:%s", str_result.string());
        if ((str_result.size() + 1) > p_result->size) {
            p_result->size = str_result.size() + 1;
            return RESULT_BUFFER_OVERFLOW;
        }
        strcpy(p_result->data, str_result.string());
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the
     * "int <method name>(int arg0, String arg1)"
     * remote method which receives arguments (int arg0)
     * and returns String value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param arg 1st argument for remote method
     * @param name 2nd argument for remote method
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg, const char* name)
    {
        DBGPRINT("code = %d, arg = %d, name = %s", code, arg, name);
        if (name == NULL) {
            return RESULT_ERROR_INVALID_ARGS;
        }
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg);
        args.writeString16(String16(name));
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        if (result.dataAvail() > 0) {
            int32_t err;
            result.readInt32(&err);
        }
        DBGPRINT("code = %d, arg = %d, name = %s : OK", code, arg, name);
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to call the
     * "int <method name>(int arg0, int arg1)"
     * remote method which receives arguments (int arg0, int arg1)
     * and returns int value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param arg 1st argument for remote method
     * @param name 2nd argument for remote method
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0, int32_t arg1)
    {
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        args.writeInt32(arg1);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        if (result.dataAvail() > 0) {
            int32_t err;
            result.readInt32(&err);
        }
        return RESULT_SUCCESS;
    }

    /**
     * This methos is used to call the "String <method name>()"
     * remote method which returns String value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param p_result 1st argument for remote method to return data from
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, data_buffer_t* p_result)
    {
        DBGPRINT("code = %d, p_result = %p", code, p_result);
        if (p_result == NULL) {
            return RESULT_ERROR_INVALID_ARGS;
        }
        if (p_result->data == NULL) {
            return RESULT_EMPTY_BUFFER;
        }
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        String8 str_result(result.readString16());
        DBGPRINT("str_result:%s", str_result.string());
        if ((str_result.size() + 1) > p_result->size) {
            p_result->size = str_result.size() + 1;
            return RESULT_BUFFER_OVERFLOW;
        }
        strcpy(p_result->data, str_result.string());
        return RESULT_SUCCESS;
    }

    /**
     * This methos is used to call the
     * "int <method name>(String arg)"
     * remote method which receives arguments (String arg)
     * and returns int value
     * @param code number of the remote method @see enum OMADM_API_ENUM
     * @param data 1st argument for remote method
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, const char* data)
    {
        DBGPRINT("code = %d, data = %s", code, data);
        if (data == NULL) {
            return RESULT_ERROR_INVALID_ARGS;
        }
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeString16(String16(data));
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        DBGPRINT("code = %d, data = %s : OK", code, data);
        return RESULT_SUCCESS;
    }

    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3,
            const char* data0, const char* data1, const char* data2, const char* data3,
            const char* data4, const char* data5, int32_t arg4, uint64_t arg5, uint64_t arg6, const char* data6)
    {
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        args.writeInt32(arg1);
        args.writeInt32(arg2);
        args.writeInt32(arg3);
        args.writeString16(String16((data0 != NULL && strlen(data0) > 0) ? data0 : " "));
        args.writeString16(String16((data1 != NULL && strlen(data1) > 0) ? data1 : " "));
        args.writeString16(String16((data2 != NULL && strlen(data2) > 0) ? data2 : " "));
        args.writeString16(String16((data3 != NULL && strlen(data3) > 0) ? data3 : " "));
        args.writeString16(String16((data4 != NULL && strlen(data4) > 0) ? data4 : " "));
        args.writeString16(String16((data5 != NULL && strlen(data5) > 0) ? data5 : " "));
        args.writeInt32(arg4);
        args.writeUint64(arg5);
        args.writeUint64(arg6);
        args.writeString16(String16((data6 != NULL && strlen(data6) > 0) ? data6 : " "));
        remote()->transact(code, args, &result);
        int32_t err = result.readExceptionCode();
        if (err < 0) {
            GET_ERR_CODE(result);
            return RESULT_ERROR_TRANSACTION;
        }
        return RESULT_SUCCESS;
    }

    int BpOmadmServiceAPI::rpc(uint32_t code, const char* data0, const char* data1, int64_t arg0,
            const char* data2, const char* data3)
    {
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeString16(String16((data0 != NULL && strlen(data0) > 0) ? data0 : " "));
        args.writeString16(String16((data1 != NULL && strlen(data1) > 0) ? data1 : " "));
        args.writeInt64(arg0);
        args.writeString16(String16((data2 != NULL && strlen(data2) > 0) ? data2 : " "));
        args.writeString16(String16((data3 != NULL && strlen(data3) > 0) ? data3 : " "));
        remote()->transact(code, args, &result);
        int32_t err = result.readExceptionCode();
        if (err < 0) {
            GET_ERR_CODE(result);
            return RESULT_ERROR_TRANSACTION;
        }
        return RESULT_SUCCESS;
    }

    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t arg0 , int32_t arg1, int32_t arg2,
            int32_t arg3, int32_t arg4, int32_t arg5, const char* data0,
            const char* data1, const char** array_data0)
    {
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        args.writeInt32(arg1);
        args.writeInt32(arg2);
        args.writeInt32(arg3);
        args.writeInt32(arg4);
        args.writeInt32(arg5);
        args.writeString16(String16((data0 != NULL && strlen(data0) > 0) ? data0 : " "));
        args.writeString16(String16((data1 != NULL && strlen(data1) > 0) ? data1 : " "));
        if (!array_data0) {
            args.writeInt32(0);
        } else {
            int i = 0;
            while (array_data0[i])
            {
                i++;
            }
            args.writeInt32(i);

            i = 0;
            while (array_data0[i])
            {
                args.writeString16(String16(strlen(array_data0[i]) > 0 ? array_data0[i] : " "));
                i++;
            }
        }
        remote()->transact(code, args, &result);
        int32_t err = result.readExceptionCode();
        if (err < 0) {
            GET_ERR_CODE(result);
            return RESULT_ERROR_TRANSACTION;
        }
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to get list of strings from OmadmService.
     * It calls the "List<String> <method name>()" method.
     * @param[in] code number of the remote method @see enum OMADM_API_ENUM
     * @param[out] p_argc pointer to store quantity of strings
     * @param[out] p_argv pointer to store pointer to array of strings
     */
    int BpOmadmServiceAPI::rpc(uint32_t code, int32_t* p_argc, char*** p_argv)
    {
        DBGPRINT("code = %d", code);
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        result.readInt32(p_argc);
        if (*p_argc > 0){
            /*
             * *p_argc == -1 means that we receive null List<String>
             */
            *p_argv = (char**)calloc((*p_argc), sizeof(char*));
            for (int idx = 0; idx < *p_argc; idx ++) {
                String8 argv_str8_i(result.readString16());
                (*p_argv)[idx] = strdup(argv_str8_i.string());
            }
        }
        return RESULT_SUCCESS;
    }

    /**
     * This method is used to get list of strings from OmadmService.
     * It calls "List<String> <method name>(int arg0)" or
     * "String[] <method name>(int arg0)" methods.
     * @param[in] code number of the remote method @see enum OMADM_API_ENUM
     * @param[in] arg0 1st argument of the remote method
     * @param[out] p_argc pointer to store quantity of strings
     * @param[out] p_argv pointer to store pointer to array of strings
     */
    int BpOmadmServiceAPI::rpc(uint32_t code,
        int32_t arg0, int32_t* p_argc, char*** p_argv)
    {
        DBGPRINT("code = %d, arg0 = %d", code, arg0);
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(arg0);
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);
        result.readInt32(p_argc);
        if (*p_argc > 0){
            /*
             * *p_argc == -1 means that we receive null List<String>
             */
            *p_argv = (char**)calloc((*p_argc), sizeof(char*));
            for (int idx = 0; idx < *p_argc; idx ++) {
                String8 argv_str8_i(result.readString16());
                (*p_argv)[idx] = strdup(argv_str8_i.string());
            }
        }
        return RESULT_SUCCESS;
    }

    /**
     * This methos is used to call the "int <method name>()"
     * remote method which returns int value
     * @param p_result 1st argument for remote method
     */
    int BpOmadmServiceAPI::rpc(uint32_t code)
    {
        DBGPRINT("code = %d", code);

        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(code, args, &result);
        GET_ERR_CODE(result);

        return RESULT_SUCCESS;
    }

    /**
          * This method returns device OEM as String16
     * @return string with OEM on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getDeviceOEM()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_OEM_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getDeviceOEM() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns FWV as String16
     * @return string with FWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getFWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_FWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getFWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns SWV as String16
     * @return string with SWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getSWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_SWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getSWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns HWV as String16
     * @return string with HWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }


    /**
     * @return support of large objects
     */
    String16* BpOmadmServiceAPI::support_lrgobj_get()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(PAL_SYSTEM_SUPPORT_LRGOBJ_GET, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("support_lrgobj_get() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns date of last update as String16
     * @return string with date of last update on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getDate()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_DATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getDate caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }


    /**
     * This method returns time of last update as String16
     * @return string with time of last update on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getTime()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_TIMEUTC_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getTime caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns host device OEM as String16
     * @return string with OEM on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostDeviceManu()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_MANU_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostDeviceManu caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns host device model as String16
     * @return string with model on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostDeviceModel()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_MODEL_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostDeviceModel() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }


    /**
     * This method returns SWV of host device as String16
     * @return string with SWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostSWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_SWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostSWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns FWV of host device as String16
     * @return string with FWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostFWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_FWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostFWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }


    /**
     * This method returns HWV of host device as String16
     * @return string with HWV on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostHWV()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_HWV_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostHWV caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns date and time of last update of host as String16
     * @return string with date and time of last update of host  on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostDateStamp()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_DATESTAMP_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostDateStamp caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns ID of host as String16
     * @return string with ID of host  on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
    String16* BpOmadmServiceAPI::getHostID()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_SYSTEM_HOSTDEVICE_DEVICEID_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getHostID caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * This method returns ID of host as String16
     * @return string with ID of host  on success or NULL
     * on failure.
     * @note Returns NULL on failure. Please check.
     * @note Allocates String16 object it is caller
     * responsibility to free it.
     */
     String16* BpOmadmServiceAPI::getTyp()
     {
         Parcel data, reply;

         data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
         //By operation code to transact
         remote()->transact(PAL_SYSTEM_DEVICETYPE_GET, data, &reply);
         //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
         int32_t err = reply.readExceptionCode();
         if (err < 0) {
             ALOGE("getTyp caught exception %d", err);
             return NULL;
         }
         String16* s = new String16(reply.readString16());
         return s;
     }
    //////////////////////////////////////////////////////////
    //              EVENT HANDLER SECTION                   //
    //////////////////////////////////////////////////////////

    /**
     * Set listener to omadm service
     * @param[in] listener object
     * @param[in] api
     * @return success or failure
     */
    int BpOmadmServiceAPI::connectListener(const sp<IBinder>& listener, int api)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeStrongBinder(listener);
        data.writeInt32(api);
        remote()->transact(CONNECT_LISTENER, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("connectEventHandler() caught exception %d", err);
            return false;
        }
        return reply.readInt32();
    }

    /**
     * Remove listener from omadm service
     *
     * @param[in] api
     * @return service wide error code
     */
    int BpOmadmServiceAPI::disconnectListener(int api)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeInt32(api);
        remote()->transact(DISCONNECT_LISTENER, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("disconnectEventHandler() caught exception %d", err);
            return false;
        }
        return reply.readInt32();
    }

    /*
     * Add your methods here
     */
    String16* BpOmadmServiceAPI::getManufacturer()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_MANUFACTURER, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getManufacturer() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getModel()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_MODEL, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getModel() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getDmv()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_DMV, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getDmv() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getLanguage()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_LANGUAGE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getLanguage() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getIccid()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        remote()->transact(PAL_ICCID, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getIccid() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getExt()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(GET_EXT, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getExt() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    String16* BpOmadmServiceAPI::getImsDomain()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NETWORK_IMS_DOMAIN, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getImsDomain() caught exception %d", err);
            return NULL;
        }
        String16* s = new String16(reply.readString16());
        return s;
    }

    /**
     * Omadm Service Listener implementation.
     * Sends reply to controller about admin network status
     * param@ network status
     * param@ network ID
     */
    void OmadmListener::omadmControllerDispachAdminNetStatus(int status, int net_id) {
        ALOGD("OmadmListener::omadmControllerDispachAdminNetStatus(%d, %d)", status, net_id);
        if (status == ADMIN_NET_AVAILABLE) {
            int err = setNetworkForProcess(net_id);
            if (err) {
                ALOGE("Could not set network for process: err %d", err);
                return;
            }
        } else if (status == ADMIN_NET_LOST) {
            int err = setNetworkForProcess(NETID_UNSET);
            if(err) {
                ALOGE("Could not unset network for process: err %d", err);
                return;
            }
        }
        ssize_t index = BpOmadmServiceAPI::mOmadmCallbackData->indexOfKey(OMADM_ADMIN_NETWORK_STATUS);
        if (index < 0) {
            return;
        }
        omadmCallback omadmCallback = BpOmadmServiceAPI::mOmadmCallbackData->valueAt(index);
        if (omadmCallback != NULL) {
            omadmCallback((void*)(long)status);
        }
    }

    /**
     * Functions for network section
     */
    int BpOmadmServiceAPI::requestAdminNetwork(bool enable)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeInt32(enable);
        remote()->transact(PAL_REQUEST_ADMIN_NETWORK, data, &reply);
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("requestAdminNetwork() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    bool BpOmadmServiceAPI::setNetworkGlobalMode()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_GLOBAL_MODE_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("enableNetworkGlobalMode() caught exception %d", err);
        return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setNetworkLteCdmaMode()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_LTE_CDMA_MODE_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("enableNetworkGlobalMode() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setNetworkGsmUmtsMode()
    {
        Parcel data, reply;

        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_GSM_UMTS_MODE_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("enableNetworkGlobalMode() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    /**
     * This method set preferred network mdoe
     * @param 0 - Global, 1 - LTE/CDMA, 2 - GSM/UMTS
     * @return status of execution: true on success
     */
    bool BpOmadmServiceAPI::setPreferredNetworkMode(int mode)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeInt32(mode);
        //By operation code to transact
        remote()->transact(PAL_NW_PREFERRED_MODE_SET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setPreferredNetworkMode() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    /**
     * This method turns Mobile Data Connection On or Off
     * @param 0 - disable, 1 - enable
     * @return status of execution: true on success
     */
    bool BpOmadmServiceAPI::setMobileData(int enable)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        data.writeInt32(enable);
        //By operation code to transact
        remote()->transact(PAL_NW_MOBILE_DATA_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setMobileData() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    int BpOmadmServiceAPI::getNetworkBaseStationId()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_BASE_STATION_ID_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNetworkBaseStationId caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getNetworkSystemId()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_SYSTEM_ID_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNetworkSystemId caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getNetworkMcc()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_MCC_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNetworkMcc caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getNetworkId()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_ID_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNetworkId caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    String16* BpOmadmServiceAPI::getSupportedNetworkModes()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_SUPPORTED_MODES_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getSupportedNetworkModes caught exception %d", err);
            return NULL;
        }
        String16* result = new String16(reply.readString16());
        return result;
    }

    int BpOmadmServiceAPI::getNetworkPreferredMode()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_PREFERRED_MODE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNetworkPreferredMode caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    bool BpOmadmServiceAPI::setNfcEnabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_NFC_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setNfcEnabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setNfcDisabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_NFC_DISABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setNfcDisabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    int BpOmadmServiceAPI::getNfcState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_NW_NFC_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getNfcState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    bool BpOmadmServiceAPI::setBluetoothEnabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setBluetoothEnabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setBluetoothDisabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_DISABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setBluetoothDisabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setBluetoothDiscoverableEnabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_DISCOVERABLE_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setBluetoothDiscoverableEnabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::setBluetoothDiscoverableDisabled()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_DISCOVERABLE_DISABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("setBluetoothDiscoverableDisabled() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    int BpOmadmServiceAPI::getBluetoothDiscoverableState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_DISCOVERABLE_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothDiscoverableState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    String16* BpOmadmServiceAPI::getBluetoothName()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_NAME_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothName() caught exception %d", err);
            return NULL;
        }
        String16* result = new String16(reply.readString16());
        return result;
    }

    int BpOmadmServiceAPI::getBluetoothState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getBluetoothHdpState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_HDP_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothHdpState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getBluetoothA2dpState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_A2DP_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothA2dpState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getBluetoothHspState()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DEV_BLUETOOTH_HSP_STATE_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getBluetoothHspState() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    bool BpOmadmServiceAPI::GpsEnable()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DIAGMON_GPS_ENABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("GpsEnable() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    bool BpOmadmServiceAPI::GpsDisable()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DIAGMON_GPS_DISABLE, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("GpsDisable() caught exception %d", err);
            return false;
        }
        return (bool)reply.readInt32();
    }

    int BpOmadmServiceAPI::getGpsStatus()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DIAGMON_GPS_STATUS_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getGpsStatus() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    int BpOmadmServiceAPI::getSatellitesCount()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DIAGMON_GPS_SATELLITES_COUNT_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getSatellitesCount() caught exception %d", err);
            return err;
        }
        return reply.readInt32();
    }

    float BpOmadmServiceAPI::getSatellitesSnr()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        //By operation code to transact
        remote()->transact(PAL_DIAGMON_GPS_SATELLITES_SNR_GET, data, &reply);
        //Exception Code. In Java Level, aidl auto generate codes will process exceptioncode.
        int32_t err = reply.readExceptionCode();
        if (err < 0) {
            ALOGE("getSatellitesSnr() caught exception %d", err);
            return err;
        }
        return (float) reply.readFloat();
    }


    bool BpOmadmServiceAPI::verifyUpdateImage(const char* path)
    {
   	 	ALOGE("verifyUpdateImage() ");
        Parcel args, result;
        args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
        args.writeInt32(12345);
        args.writeString16(String16(path));
        remote()->transact(PAL_NW_APN_SET_NAME, args, &result);

        int32_t err = result.readExceptionCode();
        if (err < 0) {
           	ALOGE("verifyUpdateImage() caught exception %d", err);
            return false;
        }
        return (bool)result.readInt32();
    }

    bool BpOmadmServiceAPI::installUpdateImage(const char* path)
    {
      	 ALOGE("installUpdateImage() ");
           Parcel args, result;
           args.writeInterfaceToken(IOmadmServiceAPI::getInterfaceDescriptor());
           args.writeInt32(12346);
           args.writeString16(String16(path));
           remote()->transact(PAL_NW_APN_SET_NAME, args, &result);

           int32_t err = result.readExceptionCode();
           ALOGE("verifyUpdateImage() ");
           if (err < 0) {
               ALOGE("verifyUpdateImage() caught exception %d", err);
               return false;
           }
           return (bool)result.readInt32();
    }

    IMPLEMENT_META_INTERFACE(OmadmServiceAPI,META_INTERFACE_NAME);
}//end of namespace omadm_service_api
