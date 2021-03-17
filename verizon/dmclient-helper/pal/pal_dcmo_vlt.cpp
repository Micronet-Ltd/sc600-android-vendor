/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "omadm_service_api.h"
#include "pal_internal.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

/*!
 * Get VoLTE VLT status
 * @param[out] Buffer
 * - VoLTE VLT status
 * - Default = False
 * - Enabled = True, Disabled = False
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_BUFFER_OVERFLOW if input buffer is too short
 * - RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_mobile_vlt_state_get(int * out)
{
    DBGPRINT("(%p)", out);
    CHECK_PVALUE(out);
    int32_t out32;
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    OMADM_SERVICE_INITIALIZED();
    int rc = IPAL_S_THIS->rpc(PAL_NETWORK_IMS_VLT_GET, &out32);
    *out = out32;
    DBGPRINT("(&%d) = %d", *out, rc);

    //TODO
    //Not implemented. OEM resposibility.
    return rc;
}

/*
 * Enable vlt
 *
 * @param[in] Buffer
 *
 * - Null
 *
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_ERROR if not success
 */
int pal_mobile_vlt_enable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    OMADM_SERVICE_INITIALIZED();
    int rc = IPAL_S_THIS->rpc(PAL_NETWORK_IMS_VLT_SET, 1);
    DBGPRINT("() = %d", rc);

    //TODO
    //Not implemented. OEM resposibility.
    return rc;
}

/*
 * Disable vlt
 *
 * @param[in] Buffer
 *
 * - Null
 *
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_ERROR if not success
 */
int pal_mobile_vlt_disable()
{
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    OMADM_SERVICE_INITIALIZED();
    int rc = IPAL_S_THIS->rpc(PAL_NETWORK_IMS_VLT_SET, 0);
    DBGPRINT("() = %d", rc);

    //TODO
    //Not implemented. OEM resposibility.
    return rc;
}

__END_DECLS
