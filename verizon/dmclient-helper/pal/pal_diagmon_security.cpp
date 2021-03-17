/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <omadm_service_api.h>
#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DiagMon.Security"

#include "./pal_internal.h"

using namespace android;
using namespace omadm_service_api;

/**
  * Gets the Device Encryption status.
  *
  * @param[out] outdata Device Encryption status (1 - Device Encryption is Enabled, 0 - Device Encryption is Disabled)
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_encryption_state_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_ENCRIPTION_STATE_GET, outdata);
}

/**
  * Gets the App Verification status.
  *
  * @param[out] outdata App Verification status (1 - App Verification is Enabled, 0 - App Verification is Disabled)
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_verifyapp_state_get(int *outdata)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_VERIFYAPP_STATE_GET, outdata);
}

/**
  * Sets App Verification to Enable on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_verifyapp_enable()
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    return IPAL_S_THIS->rpc(PAL_DEV_VERIFYAPP_ENABLE);
}
