/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#ifndef __OMADM_SERVICE_API__
#define __OMADM_SERVICE_API__ 1

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <utils/KeyedVector.h>
#include <utils/Log.h>
#include <utils/String16.h>

#include "pal.h"
#include "service_listener.h"

#include "pal_event_listener.h"
#include "pal_event_handler.h"

#define NO_ERROR                      0
#define ERROR_ARGS                    1
#define ERROR_SYSCALL                 2
#define ERROR_RETURN                  3
#define ERROR_INIT                    4
#define ERROR_NOMEM                   5
#define ERROR_ADD_OMADM_CALLBACK      6
#define ERROR_REMOVE_OMADM_CALLBACK   7
#define ERROR_RETRIEVE_OMADM_CALLBACK 8

// OMADM service wide listener ID
#define EVENT_HANDLER_LISTENER  1
// Add thy listener ID here

using namespace android;

#define SERVICE_NAME "omadm_service"
#define META_INTERFACE_NAME "android.omadm.IOmadmManager"

namespace omadm_service_api {

    /*
     * API serial code.
     * Keep in sync with android.omadm.IOmadmManager.aidl
     *
     */
    enum OMADM_API_ENUM{
        GET_DEVICE_ID = IBinder::FIRST_CALL_TRANSACTION,
        SEND_NOTIFICATION,

        REGISTER_OMADM_LISTENER,
        UNREGISTER_OMADM_LISTENER,

        CONNECT_LISTENER,
        DISCONNECT_LISTENER,

        /*
         * Interface for pal_device_bluetooth functions
         */
        PAL_DEV_BLUETOOTH_ENABLE,
        PAL_DEV_BLUETOOTH_DISABLE,
        PAL_DEV_BLUETOOTH_DISCOVERABLE_ENABLE,
        PAL_DEV_BLUETOOTH_DISCOVERABLE_DISABLE,
        PAL_DEV_BLUETOOTH_DISCOVERABLE_STATE_GET,
        PAL_DEV_BLUETOOTH_NAME_GET,
        PAL_DEV_BLUETOOTH_STATE_GET,
        PAL_DEV_BLUETOOTH_HDP_STATE_GET,
        PAL_DEV_BLUETOOTH_A2DP_STATE_GET,
        PAL_DEV_BLUETOOTH_HSP_STATE_GET,

        /*
         * Interface for pal_nfc functions
         */
        PAL_NW_NFC_ENABLE,
        PAL_NW_NFC_DISABLE,
        PAL_NW_NFC_STATE_GET,

        /*
         * Interface for pal_network functions
         */
        PAL_NW_PREFERRED_MODE_SET,
        PAL_NW_MOBILE_DATA_ENABLE,
        PAL_NW_BASE_STATION_ID_GET,
        PAL_NW_SYSTEM_ID_GET,
        PAL_NW_MCC_GET,
        PAL_NW_ID_GET,
        PAL_NW_SUPPORTED_MODES_GET,
        PAL_NW_PREFERRED_MODE_GET,
        PAL_NW_CRNT_VOICE_GET,
        PAL_NW_CRNT_DATA_GET,
        PAL_NW_MOBILE_DATA_STATE_GET,
        PAL_NW_RSSI_GET,
        PAL_NW_MNC_GET,
        PAL_NW_CURRENT_NETWORK_GET,
        PAL_NW_CALL_STATE_GET,
        PAL_NW_CONNECT_TYPE_GET,
        PAL_NW_COUNTRY_ISO_GET,
        PAL_NW_SIM_STATE_GET,
        PAL_NW_GLOBAL_DATA_ROAMING_GET,
        PAL_NW_GLOBAL_DATA_ROAMING_SET,

        /*
         * Interface for pal_network mode functions
         */
        PAL_NW_GLOBAL_MODE_ENABLE,
        PAL_NW_LTE_CDMA_MODE_ENABLE,
        PAL_NW_GSM_UMTS_MODE_ENABLE,

        /*
         * Interface for pal_network_apn_class* functions
         */
        PAL_NW_APN_GET_ID,
        PAL_NW_APN_GET_NAME,
        PAL_NW_APN_SET_NAME,
        PAL_NW_APN_GET_IP,
        PAL_NW_APN_SET_IP,
        PAL_NW_APN_IS_ENABLED,
        PAL_NW_APN_ENABLE,

        /*
         * Interface for pal wifi functions
         */
        PAL_NW_GET_WIFI_STATE,
        PAL_NW_GET_WIFI_SPEED,
        PAL_NW_GET_WIFI_STATUS,
        PAL_NW_GET_WIFI_NETWORKS,
        PAL_NW_GET_WIFI_SIGNAL,
        PAL_NW_GET_WIFI_SSID,
        PAL_NW_GET_WIFI_HOTSPOT_STATE,
        PAL_SYS_GET_WIFI_MAC,
        PAL_NW_GET_WIFI_BSSID,
        PAL_NW_SET_WIFI_ENABLE,
        PAL_NW_SET_WIFI_DISABLE,
        PAL_NW_SET_WIFI_HOTSPOT_DISABLE,

        /*
         * Interface for pal_device_volumes_* functions
         */
        PAL_DEV_RINGTONE_GET,
        PAL_DEV_RINGTONE_SET,
        PAL_DEV_NOTIFICATION_GET,
        PAL_DEV_NOTIFICATION_SET,
        PAL_DEV_ALARM_GET,
        PAL_DEV_ALARM_SET,
        PAL_DEV_MEDIA_GET,
        PAL_DEV_MEDIA_SET,
        PAL_DEV_BLUETOOTH_GET,
        PAL_DEV_BLUETOOTH_SET,

        /*
         * Interface for pal_device* security functions
         */
        PAL_DEV_ENCRIPTION_STATE_GET,
        PAL_DEV_VERIFYAPP_STATE_GET,
        PAL_DEV_VERIFYAPP_ENABLE,

        /*
         * Interface for pal_network_ims_sms_over_ip_* functions
         */
        PAL_NW_IMS_SMS_OVER_IP_ENABLED,
        PAL_NW_IMS_SMS_OVER_IP_ENABLE,

        /*
         * Interface for pal_*_ims_* functions
         */
        PAL_NETWORK_IMS_DOMAIN,
        PAL_NETWORK_IMS_SMS_FORMAT_GET,
        PAL_NETWORK_IMS_SMS_FORMAT_SET,
        PAL_NETWORK_IMS_VLT_GET,
        PAL_NETWORK_IMS_VLT_SET,
        PAL_NW_IMS_LVC_GET,
        PAL_NW_IMS_LVC_SET,
        PAL_NW_IMS_VWF_GET,
        PAL_NW_IMS_VWF_SET,
        PAL_NW_IMS_EAB_GET,
        PAL_NW_IMS_EAB_SET,

        /*
         * Interface for pal_devDetail functions
         */
        PAL_SYSTEM_OEM_GET,
        PAL_SYSTEM_FWV_GET,
        PAL_SYSTEM_SWV_GET,
        PAL_SYSTEM_HWV_GET,
        PAL_SYSTEM_SUPPORT_LRGOBJ_GET,
        PAL_SYSTEM_DATE_GET,
        PAL_SYSTEM_TIMEUTC_GET,
        PAL_SYSTEM_HOSTDEVICE_MANU_GET,
        PAL_SYSTEM_HOSTDEVICE_MODEL_GET,
        PAL_SYSTEM_HOSTDEVICE_SWV_GET,
        PAL_SYSTEM_HOSTDEVICE_FWV_GET,
        PAL_SYSTEM_HOSTDEVICE_HWV_GET,
        PAL_SYSTEM_HOSTDEVICE_DATESTAMP_GET,
        PAL_SYSTEM_HOSTDEVICE_DEVICEID_GET,
        PAL_SYSTEM_DEVICETYPE_GET,

        /*
         * Interface for pal_devInfo functions
         */
        GET_MANUFACTURER,
        GET_MODEL,
        GET_DMV,
        GET_LANGUAGE,
        PAL_ICCID,
        GET_EXT,

        /*
        * Interface for Diagmon Gps functions
        */
        PAL_DIAGMON_GPS_ENABLE,
        PAL_DIAGMON_GPS_DISABLE,
        PAL_DIAGMON_GPS_STATUS_GET,
        PAL_DIAGMON_GPS_SATELLITES_COUNT_GET,
        PAL_DIAGMON_GPS_SATELLITES_SNR_GET,

        /*
         * Interface for pal_fumo functions
         */
        PAL_FUMO_PLUGIN_UPDATE_STATE,
        PAL_FUMO_PLUGIN_REMOVE_STATE,
        PAL_FUMO_PLUGIN_UPDATE_DOWNLOAD_PROGRESS,
        PAL_FUMO_PLUGIN_SYSTEM_UPDATE_INFO,

        /*
         * Interface for pal_omadm_controller functions
         */
        PAL_OMADM_CONTROLLER_SHOW_UI,
        PAL_OMADM_CONTROLLER_REMOVE_UI,

        /*
         * Interface for pal_request_network functions
         */
        PAL_REQUEST_ADMIN_NETWORK,

        /*
         * Interface for pal_system_storage_x_used_* functions
         */
        PAL_APPS_PATHS_GET,
        PAL_PICTS_PATHS_GET,
        PAL_VIDEO_PATHS_GET,
        PAL_AUDIO_PATHS_GET,
        PAL_CACHE_PATHS_GET,
        PAL_EXT_STORAGE_PATHS_GET,
        PAL_DOWNLOADS_PATH_GET,
        PAL_GET_DISK_USAGE,

        __PAL_DOES_NOT_USE_notifyOmadmClient,
        __PAL_DOES_NOT_USE_addOmadmChangedListener,
        __PAL_DOES_NOT_USE_removeOmadmChangedListener,

        // Add your serial code here
    };

class DeathNotifier: public IBinder::DeathRecipient {
    public:
        DeathNotifier() {
        }
        virtual void binderDied(const wp<IBinder>& who);
};

/*
 * Binder Shared interface, stub code
 */
class IOmadmServiceAPI: public IInterface {
    public:
        DECLARE_META_INTERFACE (OmadmServiceAPI);

            //Service API
            virtual bool sendNotification(String16* title, String16* message) = 0;

            virtual int registerOmadmCallback(
                    omadmCallback_id omadmCallbackId, omadmCallback omadmcallback) = 0;
            virtual int unregisterOmadmCallback(omadmCallback_id omadmCallbackId) = 0;

            // Event Handler section: listener connect/disconnect
            virtual int connectListener(const sp<IBinder>& listener, int api) = 0;
            virtual int disconnectListener(int api) = 0;

            /**
             * This method is used to call the "void <method name>(int arg)"
             * remote method which receives arguments (int arg0) and returns
             * status via exception if something error occurs during operation
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg0 1st argument for remote method
             */
            virtual int rpc(uint32_t code, int32_t arg) = 0;

            /**
             * This method is used to call the "int <method name>()"
             * remote method which gets int value end returns
             * status via exception if something error occurs during operation
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param p_result pointer to store read value
             */
            virtual int rpc(uint32_t code, int32_t* p_result) = 0;

            /**
             * This method is used to call the "int64_t <method name>(int arg)"
             * remote method which receives arguments (int arg0) and
             * returns int value
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg0 1st argument for remote method
             * @param p_result pointer to store read value
             */
            virtual int rpc(uint32_t code, int32_t arg0, int64_t* p_result) = 0;

            /**
             * This method is used to call the "int <method name>(int arg)"
             * remote method which receives arguments (int arg0) and returns int value
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg0 1st argument for remote method
             * @param p_result pointer to store read value
             */
            virtual int rpc(uint32_t code, int32_t arg0, int32_t* p_result) = 0;

            /**
             * This method is used to call the
             * "int <method name>(int arg0, int arg1)"
             * remote method which receives arguments (int arg0, int arg1)
             * and returns int value
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg 1st argument for remote method
             * @param name 2nd argument for remote method
             */
            virtual int rpc(uint32_t code, int32_t arg0, int32_t arg1) = 0;

            /**
             * This method is used to call the "String <method name>(int arg)"
             * remote method which receives arguments (int arg0)
             * and returns String value
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg 1st argument for remote method
             * @param p_result buffer to store c-string
             */
            virtual int rpc(uint32_t code, int32_t arg, data_buffer_t* p_result) = 0;

            /**
             * This method is used to call the
             * "int <method name>(int arg0, String arg1)"
             * remote method which receives arguments (int arg0)
             * and returns String value
             * @param code number of the remote method @see enum OMADM_API_ENUM
             * @param arg 1st argument for remote method
             * @param name 2nd argument for remote method
             */
            virtual int rpc(uint32_t code, int32_t arg, const char* name) = 0;

            /**
             * This method is used to get list of strings from OmadmService.
             * It calls "List<String> <method name>()" or
             * "String[] <method name>()" methods.
             * @param[in] code number of the remote method @see enum OMADM_API_ENUM
             * @param[out] p_argc pointer to store quantity of strings
             * @param[out] p_argv pointer to store pointer to array of strings
             */
            virtual int rpc(uint32_t code, int32_t* p_argc, char*** p_argv) = 0;

            /**
             * This method is used to get list of strings from OmadmService.
             * It calls "List<String> <method name>(int arg0)" or
             * "String[] <method name>(int arg0)" methods.
             * @param[in] code number of the remote method @see enum OMADM_API_ENUM
             * @param[in] arg0 1st argument of the remote method
             * @param[out] p_argc pointer to store quantity of strings
             * @param[out] p_argv pointer to store pointer to array of strings
             */
            virtual int rpc(uint32_t code, int32_t arg0,
                int32_t* p_argc, char*** p_argv) = 0;

            virtual int rpc(uint32_t code, int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3,
                    const char* data0, const char* data1, const char* data2, const char* data3,
                    const char* data4, const char* data5, int32_t arg4, uint64_t arg5, uint64_t arg6, const char* data6) = 0;

            virtual int rpc(uint32_t code, const char* data0, const char* data1, int64_t arg0,
                    const char* data2, const char* data3) = 0;

            virtual int rpc(uint32_t code, data_buffer_t* p_result) = 0;
            virtual int rpc(uint32_t code, const char* data) = 0;
            virtual int rpc(uint32_t code) = 0;

            virtual int rpc(uint32_t code, int32_t arg0 , int32_t arg1, int32_t arg2, int32_t arg3,
                    int32_t arg4, int32_t arg5, const char* data0, const char* data1, const char** array_data0) = 0;

            /**
             * DevInfo helper methods
             */
            virtual String16* getDeviceId() = 0;
            virtual String16* getManufacturer() = 0;
            virtual String16* getModel() = 0;
            virtual String16* getDmv() = 0;
            virtual String16* getLanguage() = 0;
            virtual String16* getIccid() = 0;
            virtual String16* getExt() = 0;

            /**
             * DevDetail helper methods
             */
            virtual String16* getDeviceOEM()= 0;
            virtual String16* getFWV()= 0;
            virtual String16* getSWV()= 0;
            virtual String16* getHWV()= 0;
            virtual String16* support_lrgobj_get() = 0;
            virtual String16* getDate() = 0;
            virtual String16* getTime() = 0;
            virtual String16* getHostDeviceManu() = 0;
            virtual String16* getHostDeviceModel() = 0;
            virtual String16* getHostHWV() = 0;
            virtual String16* getHostFWV() = 0;
            virtual String16* getHostSWV() = 0;
            virtual String16* getHostDateStamp() = 0;
            virtual String16* getHostID() = 0;
            virtual String16* getTyp() = 0;

            /*
             * Functions for palConnmo ims_domain & ims_sipt1
             */
            virtual String16* getImsDomain() = 0;

            /*
             * Functions for admin network request
             */
            virtual int requestAdminNetwork(bool enable) = 0;

            /**
             * DiagMon Network Mode operations
             */
            virtual bool setNetworkGlobalMode() = 0;
            virtual bool setNetworkLteCdmaMode() = 0;
            virtual bool setNetworkGsmUmtsMode() = 0;

            /**
             *DiagMon Network operations
             */
            virtual bool setPreferredNetworkMode(int mode) = 0;
            virtual bool setMobileData(int enable) = 0;
            virtual int getNetworkBaseStationId() = 0;
            virtual int getNetworkSystemId() = 0;
            virtual int getNetworkMcc() = 0;
            virtual int getNetworkId() = 0;
            virtual String16* getSupportedNetworkModes() = 0;
            virtual int getNetworkPreferredMode() = 0;

            //DiagMon NFC operations
            virtual bool setNfcEnabled() = 0;
            virtual bool setNfcDisabled() = 0;
            virtual int getNfcState() = 0;

            //DiagMon Bluetooth operations
            virtual bool setBluetoothEnabled() = 0;
            virtual bool setBluetoothDisabled() = 0;
            virtual bool setBluetoothDiscoverableEnabled() = 0;
            virtual bool setBluetoothDiscoverableDisabled() = 0;
            virtual int getBluetoothDiscoverableState() = 0;
            virtual String16* getBluetoothName() = 0;
            virtual int getBluetoothState() = 0;
            virtual int getBluetoothHdpState() = 0;
            virtual int getBluetoothA2dpState() = 0;
            virtual int getBluetoothHspState() = 0;

            //Diagmon Gps functions
            virtual bool GpsEnable() = 0;
            virtual bool GpsDisable() = 0;
            virtual int getGpsStatus() = 0;
            virtual int getSatellitesCount() = 0;
            virtual float getSatellitesSnr() = 0;

            // Add your methods here
            virtual bool verifyUpdateImage(const char* path) = 0;
            virtual bool installUpdateImage(const char* path) = 0;
    };

    /*
     * The proxy between the PAL and Service
     */
    class BpOmadmServiceAPI : public BpInterface<IOmadmServiceAPI>
    {
        public:
            static bool mInitialized;
            static sp<IOmadmServiceAPI> mBindrToService;
            static KeyedVector<omadmCallback_id, omadmCallback> *mOmadmCallbackData;

            BpOmadmServiceAPI(const sp<IBinder>& impl );

            // Service API
            virtual bool sendNotification(String16* title, String16* message);
            // Listners
            virtual int registerOmadmCallback(
                    omadmCallback_id omadmCallbackId, omadmCallback omadmcallback);
            virtual int unregisterOmadmCallback(omadmCallback_id omadmCallbackId);

            static int initBinder(bool wait_forever);
            static int isPalInitialized();
            static Mutex mPalLock;
            static Mutex mPalFumoPluginLock;
            static Mutex mPalOmadmControllerLock;
            static sp<IOmadmServiceAPI> gBindrToService;
            static sp<DeathNotifier> mDeathNotifier;
            static bool gInitialized;
            friend void DeathNotifier::binderDied(const wp<IBinder>& who);
            // Event Handler section: listner connect/disconnect
            virtual int connectListener(const sp<IBinder>& listener, int api);
            virtual int disconnectListener(int api);

            virtual int rpc(uint32_t code, int32_t arg);
            virtual int rpc(uint32_t code, int32_t* p_result);
            virtual int rpc(uint32_t code, int32_t arg0, int64_t* p_result);
            virtual int rpc(uint32_t code, int32_t arg, int32_t* p_result);
            virtual int rpc(uint32_t code, int32_t arg0, int32_t arg1);
            virtual int rpc(uint32_t code, int32_t arg, data_buffer_t* p_result);
            virtual int rpc(uint32_t code, int32_t arg, const char* name);

            virtual int rpc(uint32_t code, data_buffer_t* p_result);
            virtual int rpc(uint32_t code, const char* data);

            virtual int rpc(uint32_t code, int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3,
                    const char* data0, const char* data1, const char* data2, const char* data3,
                    const char* data4, const char* data5, int32_t arg4, uint64_t arg5, uint64_t arg6, const char* data6);

            virtual int rpc(uint32_t code, const char* data0, const char* data1, int64_t arg0,
                    const char* data2, const char* data3);
            virtual int rpc(uint32_t code, int32_t*, char***);
            virtual int rpc(uint32_t code, int32_t, int32_t*, char***);

            virtual int rpc(uint32_t code);

            virtual int rpc(uint32_t code, int32_t arg0 , int32_t arg1, int32_t arg2, int32_t arg3,
                    int32_t arg4, int32_t arg5, const char* data0, const char* data1, const char** array_data0);

            //DevInfo
            virtual String16* getDeviceId();
            virtual String16* getManufacturer();
            virtual String16* getModel();
            virtual String16* getDmv();
            virtual String16* getLanguage();
            virtual String16* getIccid();
            virtual String16* getExt();

            //DevDetail
            virtual String16* getDeviceOEM();
            virtual String16* getFWV();
            virtual String16* getSWV();
            virtual String16* getHWV();
            virtual String16* support_lrgobj_get();
            virtual String16* getDate();
            virtual String16* getTime();
            virtual String16* getHostDeviceManu();
            virtual String16* getHostDeviceModel();
            virtual String16* getHostHWV();
            virtual String16* getHostFWV();
            virtual String16* getHostSWV();
            virtual String16* getHostDateStamp();
            virtual String16* getHostID();
            virtual String16* getTyp();
            //palConnmo ims_domain & ims_sipt1
            virtual String16* getImsDomain();

            /*
             * Functions for admin network request
             */
            virtual int requestAdminNetwork(bool enable);

            //DiagMon Network Mode operations
            virtual bool setNetworkGlobalMode();
            virtual bool setNetworkLteCdmaMode();
            virtual bool setNetworkGsmUmtsMode();

            //DiagMon Network operations
            virtual bool setPreferredNetworkMode(int mode);
            virtual bool setMobileData(int enable);
            virtual int getNetworkBaseStationId();
            virtual int getNetworkSystemId();
            virtual int getNetworkMcc();
            virtual int getNetworkId();
            virtual String16* getSupportedNetworkModes();
            virtual int getNetworkPreferredMode();

            //DiagMon NFC operations
            virtual bool setNfcEnabled();
            virtual bool setNfcDisabled();
            virtual int getNfcState();

            //DiagMon Bluetooth operations
            virtual bool setBluetoothEnabled();
            virtual bool setBluetoothDisabled();
            virtual bool setBluetoothDiscoverableEnabled();
            virtual bool setBluetoothDiscoverableDisabled();
            virtual int getBluetoothDiscoverableState();
            virtual String16* getBluetoothName();
            virtual int getBluetoothState();
            virtual int getBluetoothHdpState();
            virtual int getBluetoothA2dpState();
            virtual int getBluetoothHspState();

             //Diagmon Gps functions
            virtual bool GpsEnable();
            virtual bool GpsDisable();
            virtual int getGpsStatus();
            virtual int getSatellitesCount();
            virtual float getSatellitesSnr();
            // Add your methods here
            virtual bool verifyUpdateImage(const char* path);
            virtual bool installUpdateImage(const char* path);
    };

} //end of namespace omadm_service_api
#endif //end of ifndef __OMADM_SERVICE_API__
