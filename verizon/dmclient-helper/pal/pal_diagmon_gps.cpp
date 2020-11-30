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
  * Sets GPS status to Enabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_gps_enable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized()) {
      return RESULT_ERROR_PAL_NO_INIT;
    }
      bool result = BpOmadmServiceAPI::mBindrToService->GpsEnable();

#ifdef DEBUG
    if(result)
        ALOGD("Device GPS was enabled");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Sets GPS status to Disabled on the device.
  *
  * @return an error code @see enum result_states
  *
  */
int pal_device_gps_disable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->GpsDisable();

#ifdef DEBUG
    if(result)
        ALOGD("Device GPS was disabled");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
  * Gets GPS status of the device.
  * @param[out] outdata GPS status (1 - GPS status is Enabled, 0 - GPS status is Disabled)
  * @return an error code @see enum result_states
  *
  */
int pal_device_gps_state_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if (outdata == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    *outdata = BpOmadmServiceAPI::mBindrToService->getGpsStatus();

    if (*outdata < 0)
        return RESULT_ERROR;

#ifdef DEBUG
    ALOGD("Device GPS state = %d", outdata);
#endif
    return RESULT_SUCCESS;
}

int pal_device_gps_snr_inviewsatellites_get(float *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

       if (outdata == NULL) {
           return RESULT_ERROR_INVALID_ARGS;
       }

       if(!BpOmadmServiceAPI::isPalInitialized())
           return RESULT_ERROR_PAL_NO_INIT;

        *outdata = (float) BpOmadmServiceAPI::mBindrToService->getSatellitesSnr();

       if (*outdata < 0)
           return RESULT_ERROR;

   #ifdef DEBUG
       ALOGD("Device GPS state = %d", outdata);
   #endif
       return RESULT_SUCCESS;
}

int pal_device_gps_num_of_satellites_get(int *outdata)
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

       if (outdata == NULL) {
           return RESULT_ERROR_INVALID_ARGS;
       }

       if(!BpOmadmServiceAPI::isPalInitialized())
           return RESULT_ERROR_PAL_NO_INIT;

       *outdata = BpOmadmServiceAPI::mBindrToService->getSatellitesCount();

       if (*outdata < 0)
           return RESULT_ERROR;

   #ifdef DEBUG
       ALOGD("Device GPS state = %d", outdata);
   #endif
       return RESULT_SUCCESS;
}

__END_DECLS
