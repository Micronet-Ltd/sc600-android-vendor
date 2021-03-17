/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "omadm_service_api.h"
#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#include "./pal_internal.h"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

 /**
 * @breif It creates a dialog for interaction with user.
 * @param [in] pmd, it is an information for user's dialog manipulating
 * @return On success, it returns 0; on error, it returns an error number.
 */
int pal_omadm_controller_gui_create_message(pal_omadm_controller_gui_message_descriptor* pmd)
{
    int status = NO_ERROR;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalOmadmControllerLock);

    status = pal_register_omadm_callback(OMADM_CONTROLLER_USER_REPLY, pmd->user_reaction);
    if (status != NO_ERROR)
    {
        ALOGE("pal_register_omadm_callback() type->OMADM_CONTROLLER_USER_REPLY, error: %d", status);
        return status;
    }

    return IPAL_S_THIS->rpc(PAL_OMADM_CONTROLLER_SHOW_UI,
            pmd->type, pmd->min_disp, pmd->max_disp, pmd->max_resp_len,
            pmd->input_type, pmd->echo_type, pmd->disp_msg, pmd->dflt_resp,
            pmd->choices);
}

/**
 * @breif It remove user's dialog.
 * @param [in] type, it is type of user's dialog which should be removed
 * @return On success, it returns 0; on error, it returns an error number.
 */
int pal_omadm_controller_gui_destroy_message(int32_t type)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalOmadmControllerLock);
    return IPAL_S_THIS->rpc(PAL_OMADM_CONTROLLER_REMOVE_UI, type);
}


__END_DECLS
