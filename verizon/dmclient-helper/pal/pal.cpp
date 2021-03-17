/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <private/android_filesystem_config.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/cdefs.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>

#include "../pal/pal_internal.h"

#include "omadm_service_api.h"
#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"
#define DEBUG
using namespace android;
using namespace omadm_service_api;

/// \todo: make onload call of pal_init()
//__attribute__((constructor)) void pal_init(void) {
//__attribute__((destructor)) {}

//int pal_connect_listener(const sp<IOmadmEventHandler>& listener, int listener_id);
int pal_connect_listener(const sp<IBinder>& listener, int listener_id);
int pal_disconnect_listener(int listener_id);

// Shared mutex objects
Mutex BpOmadmServiceAPI::mPalLock;
Mutex BpOmadmServiceAPI::mPalFumoPluginLock;
Mutex BpOmadmServiceAPI::mPalOmadmControllerLock;

// Initialize static fields
sp<IOmadmServiceAPI> BpOmadmServiceAPI::mBindrToService = NULL;
KeyedVector<omadmCallback_id, omadmCallback> * BpOmadmServiceAPI::mOmadmCallbackData = NULL;
sp<IOmadmServiceListener> OmadmListener::mOmadmListener = NULL;

sp<DeathNotifier> BpOmadmServiceAPI::mDeathNotifier = NULL;
bool BpOmadmServiceAPI::mInitialized = false;

    /*******************************************************************************
     *
     * User accessible interface
     */

    /**
     * This method initializes PAL and creates binder to
     * omadm service in framework.
     * @return execution status: 0 on success or error code
     * on failure.
     * @note Sets BpOmadmServiceAPI::mInitialized to true
     * @note Thist method blocks forever if service
     * not found.
     */
int pal_init() {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int ret = BpOmadmServiceAPI::initBinder(true);
    if (ret == NO_ERROR) {
        sp<IOmadmEventHandler> event_handler = pal_init_event_handler();
        ALOGI("event_handler initialized");
        // Add event handler as a listener to OMADM service
        ret = pal_connect_listener(IInterface::asBinder(event_handler), EVENT_HANDLER_LISTENER);
    } 

    if (ret != NO_ERROR)
        ALOGE("pal_init() fault with error %d", ret);

    return ret;
}

void pal_fini() {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    pal_disconnect_listener(EVENT_HANDLER_LISTENER);
    pal_fini_event_handler();
}

    /**
     * This method sends user notification
     * @param buffer with title of message
     * @param buffer with message body
     * @return execution status 0 on success or error code
     * on failure.
     */
int pal_device_volumes_notifications_set(data_buffer_t *title, data_buffer_t *message) {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return ERROR_INIT;

    if((NULL == title)||(NULL == title->data)||
            (0 == title->size)||(NULL == message)||
            (NULL == message->data)||(0 == message->size))
        return ERROR_ARGS;
    String16 ts(title->data);
    String16 ms(message->data);
    bool reply = BpOmadmServiceAPI::mBindrToService->sendNotification(&ts, &ms);
#ifdef DEBUG
    ALOGD("Send Notification return: %d", reply);
#endif
    return reply ? NO_ERROR : ERROR_RETURN;
}

    /**
     * This method adds omadm callback
     * @param omadm_callback  omadm callback function pointer
     * @return execution status 0 on success or error code
     * on failure.
     */
int pal_register_omadm_callback(omadmCallback_id omadmCallbackId, omadmCallback omadmcallback) {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return ERROR_INIT;

    int reply = BpOmadmServiceAPI::mBindrToService->registerOmadmCallback(omadmCallbackId, omadmcallback);
#ifdef DEBUG
    ALOGD("pal_register_omadm_callback return: %d", reply);
#endif
    return reply;
}

    /**
     * This method removes listener from the service
     * @return execution status 0 on success or error code
     * on failure.
     */
int pal_unregister_omadm_callback(omadmCallback_id omadmCallbackId) {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return ERROR_INIT;

    int reply = BpOmadmServiceAPI::mBindrToService->unregisterOmadmCallback(omadmCallbackId);
#ifdef DEBUG
    ALOGD("pal_unregister_omadm_callback return: %d", reply);
#endif
    return reply;
}

    /**
    * connect any listener to omadm service
    *
    * @param[in] listener any listener object
    * @partam[in] listener_id system wide listener object id
    * @return success or false
    */
int pal_connect_listener(const sp<IBinder>& listener, int api) {
    bool reply = false;

    if (listener != NULL) {
        ALOGI("Trying to connect omadm listener");
        reply = BpOmadmServiceAPI::mBindrToService->connectListener(listener, api);
    } else {
        ALOGE("listener is not initialized");
    }
#ifdef DEBUG
    ALOGI("pal_connect_listener() return %d", reply);
#endif
    return reply ? NO_ERROR : ERROR_RETURN;
}

    /**
    * disconnect any listener to omadm service
    * @param listener_id
    * @return success or connect code
    */
int pal_disconnect_listener(int api) {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    bool reply = false;

    if(!BpOmadmServiceAPI::isPalInitialized())
        return ERROR_INIT;

    reply = BpOmadmServiceAPI::mBindrToService->disconnectListener(api);
#ifdef DEBUG
    ALOGI("pal_disconnect_listener() return %d", reply);
#endif
    return reply ? NO_ERROR : ERROR_RETURN;
}

    /*
     * End of user accessible interface
     ***********************************************************************************/
