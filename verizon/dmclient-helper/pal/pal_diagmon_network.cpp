/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omadm_service_api.h"
#include "pal.h"
#include "pal_internal.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

/**
 * Sets preferred network mode
 * @param[in] nw_mode
 * @parblock
 *   preferred network mode
 *   0 : ./ManagedObjects/DiagMon/Network/Mode/Operations/GlobalMode
 *   1 : ./ManagedObjects/DiagMon/Network/Mode/Operations/LTE_CDMA_Mode
 *   2 : ./ManagedObjects/DiagMon/Network/Mode/Operations/GSM_UMTS_Mode
 * @endparblock
 * @see ./ManagedObjects/DiagMon/Network/Mode/Operations/GlobalMode
 * @see ./ManagedObjects/DiagMon/Network/Mode/Operations/LTE_CDMA_Mode
 * @see ./ManagedObjects/DiagMon/Network/Mode/Operations/GSM_UMTS_Mode
 * @return @see enum result_states multiplied by -1
 */
int pal_network_preferred_network_mode_set(int nw_mode)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((nw_mode < 0) || (nw_mode > 2))
        return RESULT_ERROR_INVALID_ARGS;

    bool result = BpOmadmServiceAPI::mBindrToService->setPreferredNetworkMode(nw_mode);

#ifdef DEBUG
    if(result)
            if(nw_mode == 0)
                ALOGD("GlobalMode was set");
            else if(nw_mode == 1)
                ALOGD("LTE_CDMA was set");
            else
                ALOGD("GSM_UMTS was set");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
 * Enables or disables mobile data connection.
 * @param mobile_data_enable
 * @parblock
 *   0 : disable mobile data
 * @see ./ManagedObjects/DiagMon/Network/MobileData/Operations/Disable
 *   1 : enable mobile data
 * @see ./ManagedObjects/DiagMon/Network/MobileData/Operations/Enable
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_mobiledata_state_set(int mobile_data_enable)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((mobile_data_enable < 0) || (mobile_data_enable > 1))
        return RESULT_ERROR_INVALID_ARGS;

    bool result = BpOmadmServiceAPI::mBindrToService->setMobileData(mobile_data_enable);

#ifdef DEBUG
    if(result)
        if(enable == 1)
            ALOGD("Mobile data was enabled");
        else
            ALOGD("Mobile data was disabled");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
 * Reads base station identification number
 * @see ./ManagedObjects/DiagMon/Network/BaseStationId
 * @param[out] p_value *p_value: base station identification number
 * @return @see enum result_states multiplied by -1
 */
int pal_network_basestationid_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (p_value == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *p_value = BpOmadmServiceAPI::mBindrToService->getNetworkBaseStationId();
    DBGPRINT("(& %d)", *p_value);

    if (*p_value < 0)
        return RESULT_ERROR;

    return RESULT_SUCCESS;
}

/**
 * Reads CDMA System ID to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/SystemID
 * @param[out] p_value *p_value: CDMA System ID
 * @return @see enum result_states multiplied by -1
 */
int pal_network_systemid_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (p_value == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *p_value = BpOmadmServiceAPI::mBindrToService->getNetworkSystemId();

    DBGPRINT("(& %d)", *p_value);

    if (*p_value < 0)
        return RESULT_ERROR;

    return RESULT_SUCCESS;
}

/**
 * Reads the operator MCC to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/MCC
 * @param[out] p_value *p_value: the operator MCC value
 * @return @see enum result_states multiplied by -1
 */
int pal_network_mcc_get(data_buffer_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (p_value == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    int mcc_int = BpOmadmServiceAPI::mBindrToService->getNetworkMcc();

    DBGPRINT("(& %d)", mcc_int);
    if (mcc_int < 0) {
        WRNPRINT("mcc_int = %d", mcc_int);
        /*
         * According to the requirement:
         * Reqs-OTADM_Diagnostics 2016-07
         * 3.8.15 ./ManagedObjects/DiagMon/Network/MCC VZ_REQ_OTADMDIAG_35250
         * If no value is available, report a value of 0 to the server.
         */
        CHECK_AND_SET_BUFFER_INT2STR(p_value, 0);
        return RESULT_SUCCESS;
    }

    CHECK_AND_SET_BUFFER_INT2STR(p_value, mcc_int);
    return RESULT_SUCCESS;
}

/**
 * Reads CDMA network identification number to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/NetworkID
 * @param[out] p_value *p_value: network Id
 * @return @see enum result_states multiplied by -1
 */
int pal_network_networkid_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (p_value == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *p_value = BpOmadmServiceAPI::mBindrToService->getNetworkId();

    DBGPRINT("(& %d)", *p_value);
    if (*p_value < 0)
        return RESULT_ERROR;

    return RESULT_SUCCESS;
}

/**
 * Reads array of supported preferred network modes to the outbuf->data buffer
 * as array of int32_t values
 * @see ./ManagedObjects/DiagMon/Network/Mode/SupportedModes
 * @param[out] outbuf
 * @parblock
 *  ((int32_t*)(outbuf->data)): array of supported network modes
 *   0 : Global Mode
 *   1 : LTE/CDMA Mode
 *   2 : GSM/UMTS Mode
 *  ((int32_t)(outbuf->size)): of array of supported network modes in bytes
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_supported_modes_get(data_buffer_t* outbuf)
{
    if ((NULL == outbuf) || (NULL == outbuf->data) || (0 == outbuf->size))
        return RESULT_BUFFER_NOT_DEFINED;

    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = BpOmadmServiceAPI::mBindrToService->getSupportedNetworkModes();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > outbuf->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(outbuf->data, 0, outbuf->size);
    strncpy(outbuf->data, String8(*reply).string(), outbuf->size);
    delete reply;

#ifdef DEBUG
    ALOGD("Network supported modes = %s", outbuf->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * Reads current preferred network mode to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/Mode/PreferredNetworkMode
 * @param[out] p_value
 * @parblock
 * pointer to store current preferred network mode:
 *   0 : Global Mode
 *   1 : LTE/CDMA Mode
 *   2 : GSM/UMTS Mode
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_preferred_network_mode_get(int32_t* p_value)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (p_value == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *p_value = BpOmadmServiceAPI::mBindrToService->getNetworkPreferredMode();

    DBGPRINT("Network preferred mode = %d", *p_value);

    if (*p_value < 0)
        return RESULT_ERROR;

    return RESULT_SUCCESS;
}

const static char* __STRS_VOICE[] = {
    "voice not available",
    "1xRTT",
    "LTE",
    "WiFi"
};

/**
 * Reads type of the current system which is used for Voice Traffic
 * to the p_value pointer
 * @see ./ManagedObjects/DiagMon/RF/CurrentSystem/Voice
 * @param[out] p_value
 * @parblock
 * pointer to store type of the current system which is used for Voice Traffic
 *   0 : voice not available
 *   1 : 1xRTT
 *   2 : LTE
 *   3 : Wi-Fi
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_curr_voice_get(data_buffer_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    int32_t intVoice = -1;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_CRNT_VOICE_GET, &intVoice);
    if (rc != RESULT_SUCCESS) {
        WRNPRINT("IPAL_S_THIS->rpc(PAL_NW_CRNT_VOICE_GET, &intVoice) = %d", rc);
        return rc;
    }
    char* str_voice = NULL;
    if (intVoice >= 0 &&
        intVoice < (int32_t)(sizeof(__STRS_VOICE) / sizeof(char*))) {
        str_voice = (char*)__STRS_VOICE[intVoice];
    }
    DBGPRINT("voice[%d] = %s", intVoice, str_voice);
    if (str_voice != NULL) {
        CHECK_AND_SET_BUFFER_STR2STR(p_value, str_voice);
        return RESULT_SUCCESS;
    }
    return RESULT_ERROR_INVALID_STATE;
}

const static char* __STRS_DATA[] = {
    "1xRTT",             // PalConstants.PAL_NW_TYPE_1X
    "eHPRD",             // PalConstants.PAL_NW_TYPE_3G
    "LTE",               // PalConstants.PAL_NW_TYPE_4G
    "data not available" // PalConstants.PAL_NW_TYPE_UNKNOWN
};

/**
 * Reads type of the current system which is used for Data access
 * to the p_value pointer
 * @see ./ManagedObjects/DiagMon/RF/CurrentSystem/Data
 * @param[out] p_value
 * @parblock
 * pointer to store type of the current system which is used for Data access
 *   0 : None
 *   1 : 1xRTT
 *   2 : eHPRD
 *   3 : LTE
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_curr_data_get(data_buffer_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    int32_t intData = -1;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_CRNT_DATA_GET, &intData);
    if (rc != RESULT_SUCCESS) {
        WRNPRINT("IPAL_S_THIS->rpc(PAL_NW_CRNT_DATA_GET, &intData) = %d", rc);
        return rc;
    }
    char* str_data = NULL;
    if (intData >= 0 &&
        intData < (int32_t)(sizeof(__STRS_DATA) / sizeof(char*))) {
        str_data = (char*)__STRS_DATA[intData];
    }
    DBGPRINT("data[%d] = %s", intData, str_data);
    if (str_data != NULL) {
        CHECK_AND_SET_BUFFER_STR2STR(p_value, str_data);
        return RESULT_SUCCESS;
    }
    return RESULT_ERROR_INVALID_STATE;
}

#define Telephony_PROPERTY_OPERATOR_ISROAMING "gsm.operator.isroaming"

const static char* __STRS_HOMEROAM[] = {
    "Home", "Roam"
};

/**
 * Reads flag is device in roaming network
 * to the p_value pointer
 * @see ./ManagedObjects/DiagMon/RF/HomeRoam
 * @param[out] p_value
 * @parblock
 * pointer to store flag is device in roaming network
 *   0 : home
 *   1 : roaming
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_homeroam_get(data_buffer_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    int int_hoamroam = -1;
    int status = RESULT_SUCCESS;
    status = android_get_property_bool(Telephony_PROPERTY_OPERATOR_ISROAMING,
        &int_hoamroam);
    if (status != RESULT_SUCCESS) {
        WRNPRINT(
            "android_get_property_bool("
            "Telephony_PROPERTY_OPERATOR_ISROAMING) = %d", status);
        return status;
    }
    char* str_hoamroam = NULL;
    if(int_hoamroam >= 0 && int_hoamroam <= 1) {
        str_hoamroam = (char*)__STRS_HOMEROAM[int_hoamroam];
    } else {
        WRNPRINT("unknown int_hoamroam = %d", int_hoamroam);
    }
    if (str_hoamroam != NULL) {
        CHECK_AND_SET_BUFFER_STR2STR(p_value, str_hoamroam);
        return RESULT_SUCCESS;
    }
    WRNPRINT("int_hoamroam = %d : return RESULT_ERROR_UNDEFINED;",
        int_hoamroam);
    return RESULT_ERROR_UNDEFINED;
}

#define PAL_NW_TYPE_1X 0
#define PAL_NW_TYPE_3G 1
#define PAL_NW_TYPE_4G 2

/**
 * Reads 1x Signal strength value in dBm to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/1xSignal
 * @param[out] p_value *p_value: 1x Signal strength value in dBm
 * @return @see enum result_states
 */
int pal_network_1xsignal_get(float* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    int32_t rssi32;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_1X, &rssi32);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_1X, &rssi32) = %d", rc);
        return rc;
    }
    *p_value = (float)rssi32;
    return RESULT_SUCCESS;
}

/**
 * Reads 4G Signal strength value in dBm to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/4GSignal
 * @param[out] p_value *p_value: 4G Signal strength value in dBm
 * @return @see enum result_states
 */
int pal_network_4gsignal_get(float* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    int32_t rssi32 = 0;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_4G, &rssi32);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_4G, &rssi32) = %d", rc);
        return rc;
    }
    *p_value = (float)rssi32;
    return RESULT_SUCCESS;
}

/**
 * Reads status of Mobile Data to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/MobileData/Enabled
 * @param[out] p_value
 * @parblock
 * pointer to store status of Mobile Data:
 *   0 : mobile data is disabled
 *   1 : mobile data is enabled
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_mobiledata_state_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_MOBILE_DATA_STATE_GET, p_value);
}

/**
 * Reads the operator MNC to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/MNC
 * @param[out] p_value the operator MNC value
 * @return @see enum result_states multiplied by -1
 */
int pal_network_mnc_get(data_buffer_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    int32_t mnc_int32 = 0;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_MNC_GET, &mnc_int32);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("rpc(PAL_NW_MNC_GET, int32_t*) = %d", rc);
        return rc;
    }
    if (0 >= mnc_int32) {
        /*
         * According to the requirement
         * Reqs-OTADM_Diagnostics 2016-07
         * 3.8.16 ./ManagedObjects/DiagMon/Network/MNC VZ_REQ_OTADMDIAG_35241
         * MNC should be "000" if it is not available
         */
        CHECK_AND_SET_BUFFER_STR2STR(p_value, "000");
    } else {
        CHECK_AND_SET_BUFFER_INT2STR(p_value, mnc_int32);
    }
    return RESULT_SUCCESS;
}

/**
 * Reads 3G Signal strength value in dBm to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/3GSignal
 * @param[out] p_value 3G Signal strength value in dBm
 * @return @see enum result_states multiplied by -1
 */
int pal_network_3gsignal_get(float* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    int32_t rssi32 = 0;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = IPAL_S_THIS->rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_3G, &rssi32);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("rpc(PAL_NW_RSSI_GET, PAL_NW_TYPE_3G, int32_t*) = %d", rc);
        return rc;
    }
    *p_value = (float)rssi32;
    return RESULT_SUCCESS;
}

/**
 * Reads current mobile network type to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/CurrentNetwork
 * @param[out] p_value
 * @parblock
 * pointer to store current mobile network type:
 *   0 : NETWORK_TYPE_UNKNOWN
 *   1 : NETWORK_TYPE_GPRS
 *   2 : NETWORK_TYPE_EDGE
 *   3 : NETWORK_TYPE_UMTS
 *   4 : NETWORK_TYPE_CDMA
 *   5 : NETWORK_TYPE_EVDO_0
 *   6 : NETWORK_TYPE_EVDO_A
 *   7 : NETWORK_TYPE_1xRTT
 *   8 : NETWORK_TYPE_HSDPA
 *   9 : NETWORK_TYPE_HSUPA
 *  10 : NETWORK_TYPE_HSPA
 *  11 : NETWORK_TYPE_IDEN
 *  12 : NETWORK_TYPE_EVDO_B
 *  13 : NETWORK_TYPE_LTE
 *  14 : NETWORK_TYPE_EHRPD
 *  15 : NETWORK_TYPE_HSPAP
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_currentnetwork_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_CURRENT_NETWORK_GET, p_value);
}

/**
 * Reads current device call state to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/CallState
 * @param[out] p_value
 * @parblock
 * pointer to store current device call state
 *   0 : idle
 *   1 : ringing
 *   2 : offhook
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_callstate_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_CALL_STATE_GET, p_value);
}

/**
 * Reads current active default data network type to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/ConnectType
 * @param[out] p_value
 * @parblock
 * pointer to store active default data network type:
 *   0 : WI-FI
 *   1 : Mobile
 *   2 : Unknown
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_connecttype_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_CONNECT_TYPE_GET, p_value);
}

/**
 * Reads ISO country code to the outbuf->data buffer as C-string (char*)
 * @param[out] outbuf buffer "outbuf->data" contains country ISO code as C-string
 * @see ./ManagedObjects/DiagMon/Network/NetworkCountryIso
 * @return @see enum result_states multiplied by -1
 */
int pal_network_network_countryiso_get(data_buffer_t* outbuf)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_COUNTRY_ISO_GET, outbuf);
}

/**
 * Reads device SIM state to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/SIMState
 * @param[out] p_value
 * @parblock
 * pointer to store device SIM state:
 *   0 : unknown
 *   1 : absent
 *   2 : pin required
 *   3 : puk required
 *   4 : network locked
 *   5 : ready
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_simstate_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_SIM_STATE_GET, p_value);
}

/**
 * Reads flag "is data enabled in roaming" to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Network/GlobalDataRoamingAccess
 * @param[out] p_value
 * @parblock
 * pointer to store flag "is data enabled in roaming":
 *   1 : enabled
 *   0 : disabled
 * @endparblock
 * @return @see enum result_states multiplied by -1
 */
int pal_network_global_data_roaming_access_get(int32_t* p_value)
{
    DBGPRINT("(%p)", p_value);
    CHECK_PVALUE(p_value);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GLOBAL_DATA_ROAMING_GET, p_value);
}

/**
 * Enable or disable data connection in roaming
 * @param[in] enable
 * @parblock
 *   0 : disable data connection in roaming
 *   1 : enable data connection in roaming
 * @endparblock
 * @see ./ManagedObjects/DiagMon/Network/Operations/DenyRoaming
 * @see ./ManagedObjects/DiagMon/Network/Operations/AllowCurrentTripRoaming
 * @return @see enum result_states multiplied by -1
 */
int pal_network_global_data_roaming_access_set(int enable)
{
    DBGPRINT("(%d)", enable);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_GLOBAL_DATA_ROAMING_SET, enable);
}

__END_DECLS
