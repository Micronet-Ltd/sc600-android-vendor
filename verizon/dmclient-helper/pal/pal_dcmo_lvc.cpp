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
 * Get VoLTE LVC status
 * @param[in] Buffer
 * - VoLTE LVC status
 * - Default = False
 * - Enabled = True, Disabled = False
 * @return @see enum result_states
 */
int pal_mobile_lvc_state_get(int* out)
{
    return OMADM_PAL_GET_I4(PAL_NW_IMS_LVC_GET, out);
}

/*
 * Enable lvc
 * @return @see enum result_states
 */
int pal_mobile_lvc_enable()
{
    return OMADM_PAL_SET_I4(PAL_NW_IMS_LVC_SET, 1);
}

/*
 * Disable lvc
 * @return @see enum result_states
 */
int pal_mobile_lvc_disable()
{
    return OMADM_PAL_SET_I4(PAL_NW_IMS_LVC_SET, 0);
}


__END_DECLS
