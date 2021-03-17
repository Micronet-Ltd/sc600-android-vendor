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
 * Enable vce
 *
 * @param[in] Buffer
 *
 * - Null
 *
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_ERROR if not success
 */
int pal_mobile_vce_enable()
{
    /// \todo: The currect android version does not  provide API to manage this parameter.
    /// This is OEM responcibility to support this functionality.
    return RESULT_ERROR_INVALID_STATE;
}

/*
 * Disable vce
 *
 * @param[in] Buffer
 *
 * - Null
 *
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_ERROR if not success
 */
int pal_mobile_vce_disable()
{
    /// \todo: The currect android version does not  provide API to manage this parameter.
    /// This is OEM responcibility to support this functionality.
    return RESULT_ERROR_INVALID_STATE;
}

/*!
 * Get VICE status
 * @param[in] Buffer
 * - VICE status
 * - Default = false
 * - Enabled = True, Disabled = False
 * @return
 * - RESULT_SUCCESS if success
 * - RESULT_BUFFER_OVERFLOW if input buffer is too short
 * - RESULT_BUFFER_NOT_DEFINED if argument is not defined
 */
int pal_mobile_vce_state_get(int * out)
{
    /// \todo: The currect android version does not  provide API to manage this parameter.
    /// This is OEM responcibility to support this functionality.
    UNUSED(out);
    return RESULT_ERROR_INVALID_STATE;
}

__END_DECLS
