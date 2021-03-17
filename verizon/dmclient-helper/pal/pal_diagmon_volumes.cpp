/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <omadm_service_api.h>
#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DiagMon.Volumes"

#include "./pal_internal.h"

using namespace android;
using namespace omadm_service_api;

/**
  * Gets the current Ringtone volume level on the device.
  *
  * @param[out] outdata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_ringtone_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_RINGTONE_GET, outdata);
}

/**
  * Sets the current Ringtone volume level on the device.
  *
  * @param[in] indata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_ringtone_set(int indata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_RINGTONE_SET, indata);
}

/**
  * Gets the current Notification volume level on the device.
  *
  * @param[out] outdata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_notifications_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_NOTIFICATION_GET, outdata);
}

/**
  * Sets Notification volume level on the device.
  *
  * @param[in] indata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_notifications_set(int indata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_NOTIFICATION_SET, indata);
}

/**
  * Gets the current Alarms volume level on the device.
  *
  * @param[out] outdata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_diagmon_sound_volumes_alarms_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_ALARM_GET, outdata);
}

/**
  * Sets Alarms volume level on the device.
  *
  * @param[in] indata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_alarms_set(int indata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_ALARM_SET, indata);
}

/**
  * Gets the current Audio/Video/Media volume level on the device.
  *
  * @param[out] outdata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_media_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_MEDIA_GET, outdata);
}

/**
  * Sets Audio/Video/Media volume level on the device.
  *
  * @param[in] indata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_media_set(int indata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_MEDIA_SET, indata);
}

/**
  * Gets the Bluetooth volume level that is set by the device during a phone call.
  *
  * @param[out] outdata
  * @parblock
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_volumes_bluetooth_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_BLUETOOTH_GET, outdata);
}

/**
  * Sets Bluetooth volume level on the device.
  *
  * @param[in] indata
  * @parblock
  *  value in range 0..4, where:
  *  value in range (0, 25, 50, 75, 100), where:
  *
  *           0   0%(vibrate)
  *           25  25%
  *           50  50%
  *           75  75%
  *           100 100%
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_diagmon_sound_volumes_bluetooth_set(int indata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_BLUETOOTH_SET, indata);
}
