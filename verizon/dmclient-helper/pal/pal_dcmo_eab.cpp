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

/*
 * Get VoLTE EAB status
 * @param[out] out
 * - VoLTE EAB status
 * - Default = 0
 * - Enabled = 1, Disabled = 0
 * @return @see enum result_states
 */
int pal_mobile_eab_state_get(int* out)
{
    return OMADM_PAL_GET_I4(PAL_NW_IMS_EAB_GET, out);
}

/*
 * Enable eab
 * @return @see enum result_states
 */
int pal_mobile_eab_enable()
{
    return OMADM_PAL_SET_I4(PAL_NW_IMS_EAB_SET, 1);
}


/*
 * Disable eab
 * @return @see enum result_states
 */
int pal_mobile_eab_disable()
{
    return OMADM_PAL_SET_I4(PAL_NW_IMS_EAB_SET, 0);
}

__END_DECLS
