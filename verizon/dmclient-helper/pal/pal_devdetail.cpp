/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#include <cutils/properties.h>
#include "omadm_service_api.h"
#include "pal.h"
#include "pal_internal.h"
#include "pal_fumo_cfg.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#define PAL_LINUX
using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS
/*
 * User accessible interface
 */


//devDetail
int pal_system_oem_get(data_buffer_t *Buffer) {
#ifndef PAL_LINUX
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getDeviceOEM();
    // Can return NULL on failure
    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail:oem: %s", Buffer->data);
#endif
#else
    char *value = "Quectel";
    if ((NULL == Buffer)||(NULL ==  Buffer -> data))
        return RESULT_BUFFER_NOT_DEFINED;
    if (strlen(value) + 1 > Buffer->size)
        return RESULT_BUFFER_OVERFLOW;
    memcpy(Buffer->data, value, strlen(value) + 1);
    return RESULT_SUCCESS;
#endif
    return RESULT_SUCCESS;
}

void convert(char *in,char *out){
    char *str1;
    str1 = strchr(in, '(');
    char *str2;
    str2 = strchr(in, ')');
    if(str1 != NULL){
        memcpy(out, str1 + 1, strlen(str1) - strlen(str2) - 1);
        out[strlen(str1) - strlen(str2) - 1] = '\0';
    }else{
        strcpy(out,in);
    }
    /*int j = 0;
    for(int i = 0;i < strlen(out);i ++){
        if(out[i] >= 48 && out[i] <= 57
                || out[i] >= 97 && out[i] <= 122 ||
                out[i] >= 65 && out[i] <= 90){
            out[j ++] = out[i];
        }
    }
    out[j] = '\0';*/
}

int pal_system_fwv_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
#ifndef PAL_LINUX
    ALOGE("sssssssssssssss get\n");
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getFWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }   
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
#else
    char value[PROPERTY_VALUE_MAX] = {0};
    char out[30];
    property_get("gsm.version.baseband", value, 0); 
    convert(value,out);
    if ((NULL == Buffer)||(NULL ==  Buffer -> data))
        return RESULT_BUFFER_NOT_DEFINED;
    if (strlen(out) + 1 > Buffer->size)
        return RESULT_BUFFER_OVERFLOW;
    memcpy(Buffer->data, out, strlen(out) + 1); 
    ALOGE("pal_system_fwv_get value = %s\n", out);
    return RESULT_SUCCESS;
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Software Version of current device
 * For Android device it is a version of android & build
 * Software Version comes from soft build data or
 * can be get by parcing result of shell command "getProp"
 * or
 * using android.os.Build
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_swv_get(data_buffer_t *Buffer) {


    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getSWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;

}/**
  *
  * @param[out] allocated buffer for return value.
  * Hardware Version of current device
  * For Android device it is a version of android & build
  * Hardware Version comes from firmware build data
  * or
  * can be get by parcing result of shell command "getProp"
  * or
  * using android.os.Build
  *
  * @return
  * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
  * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
  * else returnes  SUCCESS  *
  *
  */
int pal_system_hwv_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getHWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Support of Large Object("true" or "false")
 * Hardcoded
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_support_lrgobj_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->support_lrgobj_get();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

#define __USE_TIME_FROM_SYSTEM_PARTITION 1
const static char* const __s__system_part_path = "/system";

#define __UPDATE_LATEST_TIME(__PATH__, __STV__, __TMV__, __P_TIME__)       \
{                                                                          \
    int __rc = 0;                                                          \
    if ((__rc = stat((__PATH__), &(__STV__))) >= 0) {                        \
        if ((__STV__).st_mtime > *(__P_TIME__)) {                            \
            *(__P_TIME__) = (__STV__).st_mtime;                              \
            gmtime_r((__P_TIME__), &(__TMV__));                              \
            DBGPRINT("%04d-%02d-%02d %02d:%02d:%02d %s",                     \
                    (__TMV__).tm_year + 1900, (__TMV__).tm_mon + 1,                \
                    (__TMV__).tm_mday,                                             \
                    (__TMV__).tm_hour, (__TMV__).tm_min, (__TMV__).tm_sec,         \
                    (__PATH__));                                                   \
        }                                                                    \
    } else {                                                                 \
        WRNPRINT("stat(%s, &stv)= %d", (__PATH__), __rc);                    \
    }                                                                        \
}


/*
 * searchs FS object with the latest time of modification
 * recursively and returns this time via p_time
 */
static int __get_latest_dir_time_r(const char* path, time_t* p_time)
{
    char* full_path = NULL;
    struct stat stv;
    struct tm tmv;
    int rc = 0;
    if (p_time == NULL) {
        WRNPRINT("p_time is null");
        return RESULT_ERROR_INVALID_ARGS;
    }
    if (*p_time == 0) {
        __UPDATE_LATEST_TIME(path, stv, tmv, p_time);
    }
    DIR* dir = opendir(path);
    if (dir == NULL) {
        WRNPRINT("cannot open DIR:%s", path);
        return RESULT_ERROR;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if(pal_strcat_path(path, entry->d_name, &full_path) != NULL) {
            __UPDATE_LATEST_TIME(full_path, stv, tmv, p_time);
        }
        FREE_IF(full_path);
        if (pal_is_path_dir(full_path)) {
            rc = __get_latest_dir_time_r(full_path, p_time);
        }
    }

    closedir(dir);
    FREE_IF(full_path);
    return rc;
}

#define __FUMO_UPDATE_TIME_PATH \
    PALFU_WORK_PATH "/" PAL_FILENAME_LAST_UPD_DATETIME

/**
 *
 * @param[out] allocated buffer for return value.
 * Date the Software Update was successfully completed. The format
 shall be "MM:DD:YYYY"
 * from java.util.Date or in c using time.h (library)
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_date_get(data_buffer_t *Buffer)
{
    DBGPRINT("(%p)", Buffer);
#ifdef __USE_TIME_FROM_SYSTEM_PARTITION
    CHECK_BUFFER(Buffer);
    if (Buffer->size < 11) {
        WRNPRINT("Buffer->size = %d", Buffer->size);
        return RESULT_BUFFER_OVERFLOW;
    }
    char* fumo_str_time = pal_getstr_from_file(__FUMO_UPDATE_TIME_PATH);
    if (fumo_str_time != NULL) {
        DBGPRINT("FUMO_TIME:%s", fumo_str_time);
        char* fumo_str_time_d = fumo_str_time;
        char* date_str = strtok_r(fumo_str_time_d, " ", &fumo_str_time_d);
        if (date_str != NULL) {
            strncpy(Buffer->data, date_str, 11);
        }
        FREE_IF(fumo_str_time);
    } else {
        DBGPRINT("could read fumo time from file: " __FUMO_UPDATE_TIME_PATH);
        time_t latest_time = 0;
        if (__get_latest_dir_time_r(__s__system_part_path, &latest_time)) {
            WRNPRINT("get latest time failed: %s", __s__system_part_path);
            return RESULT_ERROR;
        } else {
            struct tm tmv;
            gmtime_r(&latest_time, &tmv);
            snprintf(Buffer->data, 11, "%02d:%02d:%04d",
                    tmv.tm_mon + 1, tmv.tm_mday, tmv.tm_year + 1900);
        }
    }
    DBGPRINT("%s", PAL_FILENAME_LAST_UPD_DATETIME);
#else // __USE_TIME_FROM_SYSTEM_PARTITION
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getDate();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#endif // __USE_TIME_FROM_SYSTEM_PARTITION
    DBGPRINT("(%s):OK", Buffer->data);
    return RESULT_SUCCESS;
}

/**
 *
 * @param[out] allocated buffer for return value.
 * UTC time the Software Update was successfully completed. The UTC
 format shall be "HH:MM"
 * from java.util.Date or in c using time.h (library)
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_timeutc_get(data_buffer_t *Buffer)
{
    DBGPRINT("(%p)", Buffer);
#ifdef __USE_TIME_FROM_SYSTEM_PARTITION
    CHECK_BUFFER(Buffer);
    if (Buffer->size < 6) {
        WRNPRINT("Buffer->size = %d", Buffer->size);
        return RESULT_BUFFER_OVERFLOW;
    }
    char* fumo_str_time = pal_getstr_from_file(__FUMO_UPDATE_TIME_PATH);
    if (fumo_str_time != NULL) {
        DBGPRINT("FUMO_TIME:%s", fumo_str_time);
        char* fumo_str_time_d = fumo_str_time;
        char* date_str = strtok_r(fumo_str_time_d, " ", &fumo_str_time_d);
        date_str = strtok_r(fumo_str_time_d, " ", &fumo_str_time_d);
        if (date_str != NULL) {
            strncpy(Buffer->data, date_str, 5);
        }
        FREE_IF(fumo_str_time);
    } else {
        DBGPRINT("could read fumo time from file: " __FUMO_UPDATE_TIME_PATH);
        time_t latest_time = 0;
        if (__get_latest_dir_time_r(__s__system_part_path, &latest_time)) {
            WRNPRINT("get latest time failed: %s", __s__system_part_path);
            return RESULT_ERROR;
        } else {
            struct tm tmv;
            gmtime_r(&latest_time, &tmv);
            snprintf(Buffer->data, 6, "%02d:%02d",
                    tmv.tm_hour, tmv.tm_min);
        }
    }
#else // __USE_TIME_FROM_SYSTEM_PARTITION
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getTime();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#endif // __USE_TIME_FROM_SYSTEM_PARTITION
    DBGPRINT("(%s):OK", Buffer->data);
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * manufacturer of host device
 * can be get using shell command "lsusb" / "lsusb -v"
 * or
 * from file on host device
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_manu_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getHostDeviceManu();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Model Name of host device
 * can be get using shell command "lsusb" / "lsusb -v"
 * or
 * from file on host device
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_model_get(data_buffer_t *Buffer) {
#ifndef PAL_LINUX
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService-> getHostDeviceModel();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
#else
    char *value = "SC20";
    if ((NULL == Buffer)||(NULL ==  Buffer -> data))
        return RESULT_BUFFER_NOT_DEFINED;
    if (strlen(value) + 1 > Buffer->size)
        return RESULT_BUFFER_OVERFLOW;
    memcpy(Buffer->data, value, strlen(value) + 1);
    return RESULT_SUCCESS;
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Software Version of host device
 *from file on host device
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_swv_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService-> getHostSWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Firmware Version of host device
 * from file on host device
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_fwv_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService-> getHostFWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Hardware Version of host device
 * from file on host device
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_hwv_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService-> getHostHWV();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * Date of the latest successful Host Device Software Update.
 * hardcoded or
 * if host device works under linux we need to get date of "/var/log/dpkg.log
 * " file
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_datestamp_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService-> getHostDateStamp();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}
/**
 *
 * @param[out] allocated buffer for return value.
 * ID of host device
 * It can be IMEI, MAC address or ID of usb device
 * if hostdevice works under android we can read using shell "getprop"
 * from android.provider.Settings.Secure
 * from telephonyManager
 * using shell "service call iphonesubinfo"
 * if host device works under linux we need to parce result of
 * "ifconfig command on device"
 * " file
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS  *
 *
 */
int pal_system_hostdevice_deviceid_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getHostID();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

/**
 *
 * @param[out] allocated buffer for return value.
 * Type of current device
 * It can be "Smart Device" or "Feature Phone"
 *
 * @return
 * if length of buffer <  length of value, then returns BUFFER_OVERFLOW
 * if buffer == NULL or data of buffer == NULL, then returnes BUFFER_NOT_DEFINED,
 * else returnes  SUCCESS
 *
 */
int pal_system_devtype_get(data_buffer_t *Buffer) {
    if ((NULL == Buffer) || (NULL == Buffer->data) || (0 == Buffer->size))
        return RESULT_BUFFER_NOT_DEFINED;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = NULL;
    reply = BpOmadmServiceAPI::mBindrToService->getTyp();

    if (NULL == reply)
        return RESULT_ERROR;

    if ((reply->size() + 1) > Buffer->size) {
        delete reply;
        return RESULT_BUFFER_OVERFLOW;
    }
    memset(Buffer->data, 0, Buffer->size);
    strncpy(Buffer->data, String8(*reply).string(), Buffer->size);
    delete reply;
#ifdef DEBUG
    ALOGD("DevDetail :Result of operation %s", Buffer->data);
#endif
    return RESULT_SUCCESS;
}

__END_DECLS
