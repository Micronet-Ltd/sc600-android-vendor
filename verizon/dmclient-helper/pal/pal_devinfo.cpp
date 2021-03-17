/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "omadm_service_api.h"
#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#include "./pal_internal.h"
#include <fstream>
#include "mo_error.h"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

#define IMEI_PREFIX "IMEI:"
#define PAL_LINUX

/**
 * Reads device unique ID
 * @return @see enum result_states
 * @param[out] Buffer Buffer->data contains
 * an unique device identifier such as MAC of network card or IMEI
 * For Android device it is IMEI
 * IMEI comes form TelephonyManager service
 */
int pal_system_dev_id_get(data_buffer_t *Buffer) {

    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getDeviceId();

    // Can return NULL on failure
    if (NULL == reply)
        return RESULT_ERROR;

    int prefix_len = strlen(IMEI_PREFIX);
    if ((reply->size() + prefix_len + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    // According to LTE_OTADM_Requirements:
    // 4.1.3.8 DevInfo Subtree - DevId format shall be "IMEI:x",
    // with x being the value of the IMEI.
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, IMEI_PREFIX, prefix_len);
    strncat(Buffer->data, String8(*reply).string(),
           ( Buffer->size - prefix_len) );
    delete reply;
#ifdef DEBUG
    ALOGD("Device ID %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/*
 * @return RESULT_SUCCESS if succes
 *         RESULT_BUFFER_OVERFLOW if input buffer is too shor
 *         RESULT_BUFFER_NOT_DEFINED if argument is not define
 * @param[out]: manufacturer name
 * For Android device it is a brand of this device
 * For example: Verizon Phone (M123) -> Verizo
 * Manufacturer name comes from fimware build dat
 */
int pal_system_man_get(data_buffer_t *Buffer){
#ifndef PAL_LINUX
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getManufacturer();

    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Manufactor ID %s", Buffer->data);
#endif
#else
    if (NULL == Buffer || NULL == Buffer->data)
        return RESULT_BUFFER_NOT_DEFINED;
    char *value = "Quectel";
    if (strlen(value) + 1 > Buffer->size)
        return RESULT_BUFFER_OVERFLOW;
    memcpy(Buffer->data, value, strlen(value) + 1);
    return RESULT_SUCCESS;
#endif
    return RESULT_SUCCESS;
}

/**
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 * @param[out]: model name
 * For Android device it is a name of this device.
 * For example:  Verizon Phone (M123) -> M123
 * Manufacturer name comes from fimware build data
 */
int pal_system_mod_get(data_buffer_t *Buffer){
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getModel();

    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Model ID %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 * @param[out]:  version of OMDMA protocol
 * For Android device it is a version of current OMDMA protocol
 * This is hardcoded value "1.2"
 */
int pal_system_dmv_get(data_buffer_t *Buffer){
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_OVERFLOW;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getDmv();

    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Dmv ID %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 * @param[out]:  current default system locale
 * For Android device it is a language, what used for user's applications
 * and OS translation
 * Comes from Locale
 */
int pal_system_lang_get(data_buffer_t *Buffer){
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getLanguage();

    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Language ID %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 * @param[out]: interior node
 * For Android device it is an avilable interrior node
 * witch can be used by server.
 * This is hardcoded value "./DevInfo/Ext/ConfigurationVer"
 */
int pal_system_ext_get(data_buffer_t *Buffer){
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getExt();

    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Ext %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * @return RESULT_SUCCESS if success
 *         RESULT_BUFFER_OVERFLOW if input buffer is too short
 *         RESULT_BUFFER_NOT_DEFINED if argument is not defined
 * @param[out]: sim serial number
 * For Android device this value is getting from telephony manager.
 */
int pal_system_iccid_get(data_buffer_t *Buffer) {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    if((NULL == Buffer)||(NULL == Buffer->data)||
            (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getIccid();


    // Can return NULL on failure
    if(NULL == reply)
        return RESULT_ERROR_UNDEFINED;

    if((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }

    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("Sim serial number is %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 * Cancel download process if exists
 * @param context
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS if success
 */
int pal_verify_update_image(char* file_path)
{
    ALOGD("pal_verify_update_image");
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;
    bool reply = BpOmadmServiceAPI::mBindrToService->verifyUpdateImage(file_path);
    if(reply){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"pal_verify_update_image: Verified\n");
        return MO_ERROR_SUCCESS;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"pal_verify_update_image: Verification failed\n");
        return MO_ERROR_COMMAND_FAILED;
    }
}

/**
 * Cancel
 * @param context
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS if success
 */
int pal_install_update_image(char* file_path)
{
    ALOGD("pal_install_update_image");
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;
    bool reply = BpOmadmServiceAPI::mBindrToService->installUpdateImage(file_path);
    if(reply){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"pal_install_update_image: ok\n");
        return MO_ERROR_SUCCESS;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"pal_install_update_image: install failed\n");
        return MO_ERROR_COMMAND_FAILED;
    }
}

__END_DECLS
