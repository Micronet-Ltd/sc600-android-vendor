/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <omadm_service_api.h>
#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_DEVICE_BLUETOOTH"
#define DEBUG

#include "./pal_internal.h"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

/**
  * Sets Bluetooth to Enabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_enable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
            return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setBluetoothEnabled();

    #ifdef DEBUG
        if(result)
            ALOGD("Bluetooth was enabled");
    #endif
        return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Sets Bluetooth to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_disable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
            return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setBluetoothDisabled();

    #ifdef DEBUG
        if(result)
            ALOGD("Bluetooth was disabled");
    #endif
        return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Sets Discoverable mode for Bluetooth to Enabled on the device and automatically enable Bluetooth if Bluetooth was disabled.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_discoverable_enable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
            return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setBluetoothDiscoverableEnabled();

    #ifdef DEBUG
        if(result)
            ALOGD("Bluetooth discovering was enabled");
    #endif
        return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Sets Discoverable mode for Bluetooth to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_discoverable_disable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
            return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setBluetoothDiscoverableDisabled();

    #ifdef DEBUG
        if(result)
            ALOGD("Bluetooth discovering was disabled");
    #endif
        return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Gets the Bluetooth Name of the Device.
  *
  * @param[out] outbuf
  * @parblock
  *
  *  size    size of received data in bytes
  *
  *  data    Bluetooth Name.
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_name_get(data_buffer_t *outbuf)
{
    if ((NULL == outbuf) || (NULL == outbuf->data) || (0 == outbuf->size))
        return RESULT_BUFFER_NOT_DEFINED;

    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    String16* reply = BpOmadmServiceAPI::mBindrToService->getBluetoothName();

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
    ALOGD("Bluetooth name = %s", outbuf->data);
#endif
    return RESULT_SUCCESS;
}

/**
  * Gets the status of the Bluetooth on the Device.
  *
  * @param[out] outdata status of Bluetooth (1 - Bluetooth is Enabled, 0 - Bluetooth is Disabled)
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_state_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getBluetoothState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Bluetooth state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

/**
  * Gets the state of the Health Device Profile (HDP) of the device.
  * Note: HDP - Profile designed to facilitate transmission and reception of Medical Device data
  *
  * @param[out] outdata
  * @parblock
  *  state of HDP of the device, possible values:
  *
  *           0 CONNECTED
  *           1 CONNECTING
  *           2 DISCONNECTED
  *           3 DISCONNECTING
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_hdp_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getBluetoothHdpState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Bluetooth hdp state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

/**
  * Gets the device state of Advanced Audio Distribution profile (A2DP).
  * Note: A2DP - profile defines how multimedia audio can be streamed from one device to another over a Bluetooth connection
  *
  * @param[out] outdata
  * @parblock
  *  A2DP state of the device, possible values:
  ^
  *           0 CONNECTED
  *           1 CONNECTING
  *           2 DISCONNECTED
  *           3 DISCONNECTING
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_a2dpstate_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getBluetoothA2dpState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Bluetooth a2dp state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

/**
  * Gets the Headset profile (HSP) state of the device.
  * Note: HSP profile, providing support for the popular Bluetooth Headsets to be used with mobile phones
  *
  * @param[out] outdata
  * @parblock
  *  HSP state of the device, possible values:
  *
  *           0 CONNECTED
  *           1 CONNECTING
  *           2 DISCONNECTED
  *           3 DISCONNECTING
  * @endparblock
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_hspstate_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getBluetoothHspState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Bluetooth hsp state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

/**
  * Gets the status of Discoverable Mode for Bluetooth on the device.
  *
  * @param[out] outdata status of Discoverable Mode (1 - Discoverable Mode is Discoverable, 0 - Discoverable Mode is Not Discoverable)
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_bluetooth_discoverable_state_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getBluetoothDiscoverableState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Bluetooth discoverable state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

__END_DECLS
