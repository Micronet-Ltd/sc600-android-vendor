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
  * Sets NFC status to Enabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_nfc_enable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setNfcEnabled();

#ifdef DEBUG
    if(result)
        ALOGD("Device NFC was enabled");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Sets NFC status to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_nfc_disable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setNfcDisabled();

#ifdef DEBUG
    if(result)
        ALOGD("Device NFC was disabled");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Gets NFC status of the device.
  * @param[out] outdata NFC status (1 - NFC status is Enabled, 0 - NFC status is Disabled)
  * @return an error code @see enum result_states
  *
  */
int pal_device_nfc_state_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getNfcState();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Device NFS state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

__END_DECLS
