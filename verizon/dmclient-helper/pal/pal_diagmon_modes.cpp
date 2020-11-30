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
 * Set the Network Mode to Global Mode.
 *
 * @return
 * operation status
 *
 */
int pal_network_globalmode_enable() {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setNetworkGlobalMode();

#ifdef DEBUG
    ALOGD("Global Network mode was set");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
 * Set the Network Mode to LTE/CDMA Mode.
 *
 * @return
 * operation status
 *
 */
int pal_network_lte_cdma_mode_enable() {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setNetworkLteCdmaMode();

#ifdef DEBUG
    ALOGD("LTE/CDMA Network mode was set");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

/**
 * Set the Network Mode to GSM/UMTS Mode.
 *
 * @return
 * operation status
 *
 */
int pal_network_gsm_umts_mode_enable() {
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);

    if(!BpOmadmServiceAPI::isPalInitialized())
        return RESULT_ERROR_PAL_NO_INIT;

    bool result = BpOmadmServiceAPI::mBindrToService->setNetworkGsmUmtsMode();

#ifdef DEBUG
    ALOGD("GSM/UMTS Network mode was set");
#endif
    return result ? RESULT_SUCCESS : RESULT_ERROR;
}

__END_DECLS
