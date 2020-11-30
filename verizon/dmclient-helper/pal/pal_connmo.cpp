/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <cutils/properties.h>
#include <cutils/log.h>

#include "omadm_service_api.h"
#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#include "./pal_internal.h"
#include "idev_dme_ims.h"
#include "idev_dme_wds.h"

#define PAL_STATE_MODEM
//#define PAL_APN_MODEM
//#define PAL_IP_MODEM
#define PAL_SMS_MODEM
using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

static int __pal_nw_apn_id_get(int32_t apn_cls, int32_t* p_id)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_APN_GET_ID, apn_cls, p_id);
}

static int __pal_nw_apn_name_get_with_modem(int32_t apn_cls, data_buffer_t *p_buffer)
{
	if (NULL == p_buffer || NULL == p_buffer->data)
        return RESULT_BUFFER_NOT_DEFINED;	
	char apn_name[100] = {0};
    idev_get_lte_pdp_profile(apn_cls, apn_name, NULL, NULL);
    DBGPRINT("abing pal_network_apn_name_get apn_class = %d, apn_name = %s\n", apn_cls, apn_name);
    strcpy(p_buffer->data,apn_name);
	return 0;
}

static int __pal_nw_apn_name_set_with_modem(int32_t apn_cls, data_buffer_t *p_buffer)
{
    idev_modify_lte_pdp_profile(apn_cls, p_buffer->data, NULL);
	return 0;
}

static int __pal_nw_apn_name_get(int32_t apn_cls, data_buffer_t *p_buffer)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(
        PAL_NW_APN_GET_NAME, apn_cls, p_buffer);
}

static int __pal_nw_apn_name_set(int32_t apn_cls, const char* apn_name)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
	int ret = -1;
    ret = IPAL_S_THIS->rpc(
        PAL_NW_APN_SET_NAME, apn_cls, apn_name);
	if (apn_cls == 2)
		sleep(2);
	return ret;
}

static int __pal_nw_apn_ip_get(int32_t apn_cls, int32_t* p_ipvX)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NW_APN_GET_IP, apn_cls, p_ipvX);
}

const static char* APN_PROTO_STR_IPV4 = "IPV4";
const static char* APN_PROTO_STR_IPV6 = "IPV6";
const static char* APN_PROTO_STR_IPV4P6 = "IPV4 and IPV6";

#define APN_PROTO_IPV4 0x00000001
#define APN_PROTO_IPV6 0x00000002
#define APN_PROTO_IPV4P6 0x00000003

static int __pal_nw_apn_ip_get_str_with_modem(int32_t apn_cls, data_buffer_t *obuf)
{
	char ip_type[100] = {0};
    idev_get_lte_pdp_profile(apn_cls, NULL, ip_type, NULL);
    if (strcmp(ip_type, "IPV4V6"))
    {
        strcpy(obuf->data,"IPv4 and IPV6");
    } else if (strcmp(ip_type, "IP"))
    {
        strcpy(obuf->data,"ipv4");
    } else if (strcmp(ip_type, "IPV6"))
    {
        strcpy(obuf->data,"ipv6");
    }

	return RESULT_SUCCESS;
}

static int __pal_nw_apn_ip_set_str_with_modem(int32_t apn_cls, data_buffer_t* ibuf) {
	idev_modify_lte_pdp_profile(apn_cls, NULL, ibuf->data);

	return RESULT_SUCCESS;
}

static int __pal_nw_apn_ip_get_str(int32_t apn_cls, data_buffer_t *obuf)
{
    int proto_mask = 0;
    char* proto_str = NULL;
    int rc = __pal_nw_apn_ip_get(apn_cls, &proto_mask);
    DBGPRINT("apn_class = %d proto_mask = %d", apn_cls, proto_mask);
    if (RESULT_SUCCESS != rc) {
        return rc;
    }
    switch(proto_mask) {
        case APN_PROTO_IPV4:
            proto_str = (char*)APN_PROTO_STR_IPV4;
            break;
        case APN_PROTO_IPV6:
            proto_str = (char*)APN_PROTO_STR_IPV6;
            break;
        case APN_PROTO_IPV4P6:
            proto_str = (char*)APN_PROTO_STR_IPV4P6;
            break;
    }
    if (NULL == proto_str) {
        return RESULT_ERROR_UNDEFINED;
    }
    CHECK_AND_SET_BUFFER_STR2STR(obuf, proto_str);
    return RESULT_SUCCESS;
}

static int pal_nw_apn_ip_set(int32_t apn_cls, int32_t ipvX)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
	int ret = -1;
    ret = IPAL_S_THIS->rpc(PAL_NW_APN_SET_IP, apn_cls, ipvX);
	if (apn_cls == 2)
		sleep(3);
	return ret;
}

static int __pal_nw_apn_ip_set_str(int32_t apn_cls, data_buffer_t* ibuf) {
    CHECK_BUFFER(ibuf);
    int ipvX = 0;
    if (0 == strcasecmp(ibuf->data, APN_PROTO_STR_IPV4)) {
        ipvX = APN_PROTO_IPV4;
    } else
    if (0 == strcasecmp(ibuf->data, APN_PROTO_STR_IPV6)) {
        ipvX = APN_PROTO_IPV6;
    } else
    if (0 == strcasecmp(ibuf->data, APN_PROTO_STR_IPV4P6)) {
        ipvX = APN_PROTO_IPV4P6;
    }
    DBGPRINT("%s => %d", ibuf->data, ipvX);
    if (0 == ipvX) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    return pal_nw_apn_ip_set(apn_cls, ipvX);
}

static int __pal_nw_apn_state_get(int32_t apn_cls, int32_t* p_is_enabled)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(
        PAL_NW_APN_IS_ENABLED, apn_cls, p_is_enabled);
}

static const char* STR_TRUE = "TRUE";
static const char* STR_FALSE = "FALSE";

/**
 * Getting state for apn
 *
 * @param apn_class desired class for apn
 * @param[out] : Buffer struct for result with hardcoded value "true"
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int __pal_nw_apn_state_get_str_with_modem(int apn_class, data_buffer_t *Buffer){
    bool enable = true;
    DBGPRINT("abing __pal_nw_apn_state_get_str_with_modem  apn_class = %d\n", apn_class);

	idev_get_lte_pdp_profile(apn_class, NULL,NULL,&enable);

    const char* is_enabled_str = enable ? STR_TRUE : STR_FALSE;
    CHECK_AND_SET_BUFFER_STR2STR(Buffer, is_enabled_str);

    return RESULT_SUCCESS;
}

static int __pal_nw_apn_state_get_str(int32_t apn_cls, data_buffer_t* obuf)
{
    int is_enabled = -1;
	DBGPRINT(" get %d class \n", apn_cls);
    int rc = __pal_nw_apn_state_get(apn_cls, &is_enabled);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    const char* is_enabled_str = is_enabled ? STR_TRUE : STR_FALSE;
    CHECK_AND_SET_BUFFER_STR2STR(obuf, is_enabled_str);
    return RESULT_SUCCESS;
}

static int pal_network_apn_class_disable_with_modem(int32_t apn_cls)
{
	idev_disable_lte_pdp_profile(apn_cls);
	return 0;
}

static int pal_network_apn_class_enable_with_modem(int32_t apn_cls)
{
	idev_enable_lte_pdp_profile(apn_cls);
	return 0;
}

static int pal_network_apn_class_enable(int32_t apn_cls, int32_t enable)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(
        PAL_NW_APN_ENABLE, apn_cls, enable);
}


/**
 * Reads ID of the APN with class 1
 *
 * @param[out] obuf obuf->data contains id for class 1 apn as c-string
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_id_get(data_buffer_t *obuf)
{
    int32_t tmp_val;
    int rc = __pal_nw_apn_id_get(1, &tmp_val);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    CHECK_AND_SET_BUFFER_INT2STR(obuf, tmp_val);
    return RESULT_SUCCESS;
}

/**
 * Reads name for class 1 apn.
 *
 * @param[out] obuf obuf->data contains name value for class 1 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_name_get(data_buffer_t *obuf)
{
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_get(1, obuf);
#else
	return __pal_nw_apn_name_get_with_modem(1, obuf);
#endif
}

/**
 * Sets name for class 1 apn.
 *
 * @param[in] ibuf ibuf->data contains new name value
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_name_set(data_buffer_t *ibuf)
{
    CHECK_BUFFER(ibuf);
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_set(1, ibuf->data);
#else
    return __pal_nw_apn_name_set_with_modem(1, ibuf);
#endif
}

/**
 * Reads ip version for class 1 apn.
 *
 * @param[out] obuf obuf->data contains ip value for class 1 apn
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_ip_get(data_buffer_t *obuf)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_get_str(1, obuf);
#else
	return __pal_nw_apn_ip_get_str_with_modem(1, obuf);
#endif
}

/**
 * Sets ip version for class 1 apn.
 *
 * @param[in] ibuf ibuf->data contains new ip value
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_ip_set(data_buffer_t *ibuf)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_set_str(1, ibuf);
#else
	return __pal_nw_apn_ip_set_str_with_modem(1, ibuf);
#endif
}

/**
 * Reads state for class 1 apn.
 *
 * @param[out] obuf
 * @parblock
 * obuf->data state for class 1 apn as c-string
 * posible values:
 *   "True" : APN is enabled
 *   "False": APN is disabled
 * @endparblock
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_state_get(data_buffer_t *obuf)
{
//#ifndef PAL_STATE_MODEM
    return __pal_nw_apn_state_get_str(1, obuf);
//#else
//	return __pal_nw_apn_state_get_str_with_modem(1, obuf);
//#endif
}

/**
 * Enable class 1 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_enable(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(1, 1);
#else
	return pal_network_apn_class_enable_with_modem(1);
#endif
}
/**
 * Disable class 1 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class1_disabled(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(1, 0);
#else
	return pal_network_apn_class_disable_with_modem(1);
#endif
}


/**
 * Reads ID of the APN with class 2
 *
 * @param[out] obuf obuf->data contains id for class 2 apn as c-string
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_id_get(data_buffer_t *obuf)
{
    int32_t tmp_val;
    int rc = __pal_nw_apn_id_get(2, &tmp_val);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    CHECK_AND_SET_BUFFER_INT2STR(obuf, tmp_val);
    return RESULT_SUCCESS;
}

/**
 * Getting name for class 2 apn.
 *
 * @param[out] obuf obuf->data contains name for class 2 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_name_get(data_buffer_t *obuf)
{
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_get(2, obuf);
#else
	return __pal_nw_apn_name_get_with_modem(2, obuf);
#endif
}

/**
 * Setting name for class 2 apn.
 *
 * @param ibuf ibuf->data contains new name value
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_name_set(data_buffer_t *ibuf)
{
    CHECK_BUFFER(ibuf);
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_set(2, ibuf->data);
#else
	return __pal_nw_apn_name_set_with_modem(2, ibuf);
#endif

}

/**
 * Getting ip for class 2 apn.
 *
 * @param[out] : ip version for class 2 apn
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_ip_get(data_buffer_t *Buffer)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_get_str(2, Buffer);
#else
	return __pal_nw_apn_ip_get_str_with_modem(2, Buffer);
#endif
}

/**
 * Setting ip for class 2 apn.
 *
 * @param new ip value
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_ip_set(data_buffer_t *ibuf)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_set_str(2, ibuf);
#else
	return __pal_nw_apn_ip_set_str_with_modem(2, ibuf);
#endif
}

/**
 * Getting state for class 2 apn.
 *
 * @param[out] : state for class 2 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_state_get(data_buffer_t *obuf)
{
    return __pal_nw_apn_state_get_str(2, obuf);
}

/**
 * Enable class 2 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_enable(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(2, 1);
#else
	return pal_network_apn_class_enable_with_modem(2);
#endif
}
/**
 * Disable class 2 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class2_disabled(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(2, 0);
#else
	return pal_network_apn_class_disable_with_modem(2);
#endif
}

/**
 * Reads ID of the APN with class 3
 *
 * @param[out] obuf obuf->data contains id for class 3 apn as c-string
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_id_get(data_buffer_t *obuf)
{
    int32_t tmp_val;
    int rc = __pal_nw_apn_id_get(3, &tmp_val);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    CHECK_AND_SET_BUFFER_INT2STR(obuf, tmp_val);
    return RESULT_SUCCESS;
}

/**
 * Getting name for class 3 apn.
 *
 * @param[out] obuf obuf->data contains name for class 3 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_name_get(data_buffer_t *obuf)
{
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_get(3, obuf);
#else
	return __pal_nw_apn_name_get_with_modem(3, obuf);
#endif
}

/**
 * Setting name for class 3 apn.
 *
 * @param ibuf ibuf->data contains new name value
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_name_set(data_buffer_t *ibuf)
{
    CHECK_BUFFER(ibuf);
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_set(3, ibuf->data);
#else
	return __pal_nw_apn_name_set_with_modem(3, ibuf);
#endif
}

/**
 * Getting ip for class 3 apn.
 *
 * @param[out] : ip value for class 3 apn
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_ip_get(data_buffer_t *Buffer)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_get_str(3, Buffer);
#else
	return __pal_nw_apn_ip_get_str_with_modem(3, Buffer);
#endif
}

/**
 * Setting ip for class 3 apn.
 *
 * @param new ip value
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_ip_set(data_buffer_t *ibuf)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_set_str(3, ibuf);
#else
	return __pal_nw_apn_ip_set_str_with_modem(3, ibuf);
#endif
}

/**
 * Getting state for class 3 apn.
 *
 * @param[out] : state for class 3 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_state_get(data_buffer_t *obuf)
{
    return __pal_nw_apn_state_get_str(3, obuf);
}

/**
 * Enable class 3 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_enable(data_buffer_t* Buffer){
    UNUSED(Buffer);
    return pal_network_apn_class_enable(3, 1);
}
/**
 * Disable class 3 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class3_disabled(data_buffer_t* Buffer){
    UNUSED(Buffer);
    return pal_network_apn_class_enable(3, 0);
}

/**
 * Reads ID of the APN with class 4
 *
 * @param[out] obuf obuf->data contains id for class 4 apn as c-string
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_id_get(data_buffer_t *obuf)
{
    int32_t tmp_val;
    int rc = __pal_nw_apn_id_get(4, &tmp_val);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    CHECK_AND_SET_BUFFER_INT2STR(obuf, tmp_val);
    return RESULT_SUCCESS;
}

/**
 * Getting name for class 4 apn.
 *
 * @param[out] obuf obuf->data contains name for class 4 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_name_get(data_buffer_t *obuf)
{
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_get(4, obuf);
#else
	return __pal_nw_apn_name_get_with_modem(4, obuf);
#endif
}

/**
 * Setting name for class 4 apn.
 *
 * @param ibuf ibuf->data contains new name value
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_name_set(data_buffer_t *ibuf)
{
    CHECK_BUFFER(ibuf);
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_set(4, ibuf->data);
#else
    return __pal_nw_apn_name_set_with_modem(4, ibuf);
#endif
}

/**
 * Getting ip for class 4 apn.
 *
 * @param[out] : ip value for class 4 apn
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_ip_get(data_buffer_t *Buffer)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_get_str(4, Buffer);
#else
	return __pal_nw_apn_ip_get_str_with_modem(4, Buffer);
#endif
}

/**
 * Setting ip for class 4 apn.
 *
 * @param new ip value
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_ip_set(data_buffer_t *ibuf)
{
#ifndef PAL_IP_MODEM
    return __pal_nw_apn_ip_set_str(4, ibuf);
#else
	return __pal_nw_apn_ip_set_str_with_modem(4, ibuf);
#endif
}

/**
 * Getting state for class 4 apn.
 *
 * @param[out] : state for class 4 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_state_get(data_buffer_t *obuf)
{
    return __pal_nw_apn_state_get_str(4, obuf);
}

/**
 * Enable class 4 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_enable(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
    return pal_network_apn_class_enable(4, 1);
}
/**
 * Disable class 4 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class4_disabled(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
    return pal_network_apn_class_enable(4, 0);
}

/**
 * Reads ID of the APN with class 6
 *
 * @param[out] obuf obuf->data contains id for class 6 apn as c-string
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_id_get(data_buffer_t* obuf)
{
    int32_t tmp_val;
    int rc = __pal_nw_apn_id_get(5, &tmp_val);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    CHECK_AND_SET_BUFFER_INT2STR(obuf, tmp_val);
    return RESULT_SUCCESS;
}

/**
 * Getting name for class 6 apn.
 *
 * @param[out] obuf obuf->data contains name for class 6 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_name_get(data_buffer_t* obuf)
{
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_get(5, obuf);
#else
	return __pal_nw_apn_name_get_with_modem(6, obuf);
#endif
}

/**
 * Setting name for class 6 apn.
 *
 * @param ibuf ibuf->data contains new name value
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_name_set(data_buffer_t *ibuf)
{
    CHECK_BUFFER(ibuf);
#ifndef PAL_APN_MODEM
    return __pal_nw_apn_name_set(5, ibuf->data);
#else
    return __pal_nw_apn_name_set_with_modem(6, ibuf);
#endif
}

/**
 * Getting ip for class 6 apn.
 *
 * @param[out] : ip value for class 6 apn
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_ip_get(data_buffer_t *Buffer)
{
    return __pal_nw_apn_ip_get_str(5, Buffer);
}

/**
 * Setting ip for class 6 apn.
 *
 * @param new ip value
 * For Android device it is : n/a
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_ip_set(data_buffer_t *ibuf)
{
    return __pal_nw_apn_ip_set_str(5, ibuf);
}

/**
 * Getting state for class 6 apn.
 *
 * @param[out] : state for class 6 apn
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_state_get(data_buffer_t *obuf)
{
    return __pal_nw_apn_state_get_str(5, obuf);
}

/**
 * Enable class 6 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_enable(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(5, 1);
#else
	return pal_network_apn_class_enable_with_modem(6);
#endif
}
/**
 * Disable class 6 apn.
 *
 * @param n/a
 * For Android device it is : database with URI Telephony.Carriers.CONTENT_URI
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_network_apn_class6_disabled(data_buffer_t* Buffer)
{
    UNUSED(Buffer);
#ifndef PAL_STATE_MODEM
    return pal_network_apn_class_enable(5, 0);
#else
	return pal_network_apn_class_disable_with_modem(6);
#endif
}

static const char* PROPERTY_SMS_OVER_IMS = "persist.radio.imsallowmtsms";

/**
 * Checks the "ims sms over ip network" feature is enabled.
 * @param iobuf
 * @parblock
 * iobuf->data contains c-string:
 *   "True" : "ims sms over ip network" is enabled
 *   "False": "ims sms over ip network" is disabled
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_ims_sms_over_ip_network_indication_get(data_buffer_t *iobuf)
{
    ALOGE("alpha pal_network_ims_sms_over_ip_network_indication_get");
#ifndef PAL_SMS_MODEM
    int is_enabled = 0; // the same behaviour as in the RadioInfo.java
    int rc = RESULT_ERROR_UNDEFINED;
    char buf[PROPERTY_VALUE_MAX];
    int len = property_get(PROPERTY_SMS_OVER_IMS, buf, "");
    DBGPRINT("property_get(%s, %s, \"\"", PROPERTY_SMS_OVER_IMS, buf);
    if (len == 1) {
        char ch = buf[0];
        if (ch == '0' || ch == 'n') {
            is_enabled = 0;
            rc = RESULT_SUCCESS;
        } else if (ch == '1' || ch == 'y') {
            is_enabled = 1;
            rc = RESULT_SUCCESS;
        } else {
            WRNPRINT("%s: undefined", PROPERTY_SMS_OVER_IMS);
        }
    } else if (len > 1) {
        if (!strcmp(buf, "no") || !strcmp(buf, "false") ||
            !strcmp(buf, "off")) {
            is_enabled = 0;
            rc = RESULT_SUCCESS;
        } else if (!strcmp(buf, "yes") || !strcmp(buf, "true") ||
            !strcmp(buf, "on")) {
            is_enabled = 1;
            rc = RESULT_SUCCESS;
        } else {
            WRNPRINT("%s: undefined", PROPERTY_SMS_OVER_IMS);
        }
    } else {
		is_enabled = 1;
		rc = RESULT_SUCCESS;
        WRNPRINT("%s: default set enable", PROPERTY_SMS_OVER_IMS);
    }
    if (is_enabled == -1) {
        return RESULT_ERROR_UNDEFINED;
    }
    const char* str_value = is_enabled ? STR_TRUE : STR_FALSE;

	//char sms_over_ip[128] = {0};	
	//idev_get_ims_sms_configuration(NULL, sms_over_ip);
	if (is_enabled)
	{
		idev_modify_ims_sms_configuration(NULL, "true");
	} else
	{
		idev_modify_ims_sms_configuration(NULL, "false");
	}
    CHECK_AND_SET_BUFFER_STR2STR(iobuf, str_value);
#else
	char sms_format[50] = {0};
	char sms_over_ip[50] = {0};
    strcpy(sms_over_ip,"true");
    strcpy(sms_format,"3GPP2");
	// idev_get_ims_sms_configuration(sms_format,sms_over_ip);
	CHECK_AND_SET_BUFFER_STR2STR(iobuf, sms_over_ip);
#endif
    //CHECK_AND_SET_BUFFER_STR2STR(iobuf, sms_over_ip);
    return RESULT_SUCCESS;
}

/**
 * Enables or disables the "ims sms over ip network" feature
 * @param ibuf
 * @parblock
 * iobuf->data can contain c-string:
 *   "True" : enable "ims sms over ip network"
 *   "False": disable "ims sms over ip network"
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_ims_sms_over_ip_network_indication_set(data_buffer_t *ibuf){
    ALOGE("alpha pal_network_ims_sms_over_ip_network_indication_set");
    CHECK_BUFFER(ibuf);
    DBGPRINT("(%s)", ibuf->data);
#ifndef PAL_SMS_MODEM
    int enable = -1;
    if (strncasecmp(ibuf->data, STR_TRUE, ibuf->size) == 0) {
        enable = 1;
    } else if (strncasecmp(ibuf->data, STR_FALSE, ibuf->size) == 0) {
        enable = 0;
    }
    if (enable == -1) {
        WRNPRINT("ibuf->data = %s", ibuf->data);
        return RESULT_ERROR_INVALID_ARGS;
    }
    char buf[PROPERTY_VALUE_MAX];
    int len = property_get(PROPERTY_SMS_OVER_IMS, buf, "");
    if (len <= 1) {
        snprintf(buf, 4, "%d", enable);
    } else {
        if (strcmp(buf, "no") == 0 || strcmp(buf, "yes")) {
            snprintf(buf, PROPERTY_VALUE_MAX - 1, enable ? "yes" : "no");
        } else if (strcmp(buf, "true") == 0 || strcmp(buf, "false")){
            snprintf(buf, PROPERTY_VALUE_MAX - 1, enable ? "true" : "false");
        } else if (strcmp(buf, "on") == 0 || strcmp(buf, "off")){
            snprintf(buf, PROPERTY_VALUE_MAX - 1, enable ? "on" : "off");
        } else {
            return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
        }
    }


    if (property_set(PROPERTY_SMS_OVER_IMS, buf) != 0) {
        WRNPRINT("set : %s : %s : FAILED", PROPERTY_SMS_OVER_IMS, buf);
        return RESULT_ERROR;
    } else {
        DBGPRINT("set : %s : %s : OK", PROPERTY_SMS_OVER_IMS, buf);
        return RESULT_SUCCESS;
    }
#else
	idev_modify_ims_sms_configuration(NULL, ibuf->data);
#endif

    return RESULT_SUCCESS;

}

int pal_network_ims_domain_get(data_buffer_t *obuf){
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NETWORK_IMS_DOMAIN, obuf);
}

const static char* const SMS_FORMAT_3GPP_STR = "3GPP";
const static char* const SMS_FORMAT_3GPP2_STR = "3GPP2";
#define SMS_FORMAT_3GPP 1
#define SMS_FORMAT_3GPP2 0

/**
 * Gets the "ims smsformat" value
 * @param obuf
 * @parblock
 * iobuf->data contains c-string:
 * "3GPP" or "3GPP2"
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_ims_smsformat_get(data_buffer_t* obuf)
{
    ALOGE("alpha pal_network_ims_smsformat_get");
    DBGPRINT("(%p)", obuf);
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int32_t smsFormat = 0;
    int errCode = IPAL_S_THIS->rpc(PAL_NETWORK_IMS_SMS_FORMAT_GET, &smsFormat);
    if (errCode != RESULT_SUCCESS) {
        return errCode;
    }
    char* smsFormatStr = NULL;
    switch(smsFormat) {
        case SMS_FORMAT_3GPP:
            smsFormatStr = (char*)SMS_FORMAT_3GPP_STR;
            break;
        case SMS_FORMAT_3GPP2:
            smsFormatStr = (char*)SMS_FORMAT_3GPP2_STR;
            break;
    }
    if (smsFormatStr == NULL) {
        WRNPRINT("smsFormatStr == NULL, smsFormat = %d", smsFormat);
        return RESULT_ERROR_INVALID_STATE;
    }
    DBGPRINT("smsFormat = %d, smsFormatStr = %s", smsFormat, smsFormatStr);
    CHECK_AND_SET_BUFFER_STR2STR(obuf, smsFormatStr);
    return RESULT_SUCCESS;
}

/**
 * Sets the "ims smsformat" feature
 * @param ibuf
 * @parblock
 * iobuf->data can contain c-string:
 *  "3GPP" or "3GPP2"
 * @endparblock
 * @return @see enum result_states
 */
int pal_network_ims_smsformat_set(data_buffer_t* ibuf)
{
    ALOGE("alpha pal_network_ims_smsformat_set");
    CHECK_BUFFER(ibuf);
    int smsFormat = -1;
    if (strcasecmp(ibuf->data, SMS_FORMAT_3GPP_STR) == 0) {
        smsFormat = SMS_FORMAT_3GPP;
    } else if (strcasecmp(ibuf->data, SMS_FORMAT_3GPP2_STR) == 0) {
        smsFormat = SMS_FORMAT_3GPP2;
    }
    if (smsFormat < 0) {
        WRNPRINT("ibuf->data = %s", ibuf->data);
        return RESULT_ERROR_INVALID_ARGS;
    }
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_NETWORK_IMS_SMS_FORMAT_SET, smsFormat);
}

__END_DECLS
