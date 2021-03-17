/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */


#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mo_error.h"
#include "omadm_service_api.h"
#include "pal.h"
#include "pal_fumo_cfg.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#include "./pal_internal.h"
#include "omadm_service_api.h"

using namespace android;
using namespace omadm_service_api;

__BEGIN_DECLS

#define PALFU_WORK_PATH MO_WORK_PATH "/data"

/**
 * @breif It creates a dialog for interaction with user.
 * @param [in, out] pmd, it is an information for user's dialog manipulating
 * @return On success, it returns 0; on error, it returns an error number.
 */
int pal_fumo_gui_create_message(pal_fumo_gui_message_descriptor* pmd)
{
    int status = NO_ERROR;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalFumoPluginLock);

    status = pal_register_omadm_callback(OMADM_FUMO_USER_REPLY, pmd->user_reaction);
    if (status != NO_ERROR)
    {
        ALOGE("pal_register_omadm_callback() type->OMADM_FUMO_USER_REPLY, error: %d", status);
        return status;
    }

    return IPAL_S_THIS->rpc(PAL_FUMO_PLUGIN_UPDATE_STATE,
                pmd->state, pmd->message_type, pmd->message_mode, pmd->icon_type,
                pmd->header_text, pmd->message_text, pmd->hyper_link_caption,
                pmd->hyper_link, pmd->progress_bar_caption, pmd->install_param,
                pmd->button_type, pmd->required_space_for_update,
                pmd->required_space_for_delete, pmd->severity);
}

/**
 * @breif It remove user's dialog.
 * @param [in] state, it is an unique value for message's identifing
 * @return On success, it returns 0; on error, it returns an error number.
 */
int pal_fumo_gui_destroy_message(int32_t state)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalFumoPluginLock);
    return IPAL_S_THIS->rpc(PAL_FUMO_PLUGIN_REMOVE_STATE, state);
}

/**
 * Sets fumo plugin download progress.
 * @param[in] percent current percent of download package
 * @return RESULT_SUCCESS if success
 */
int pal_fumo_gui_update_download_progress(int32_t percent)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalFumoPluginLock);
    return IPAL_S_THIS->rpc(PAL_FUMO_PLUGIN_UPDATE_DOWNLOAD_PROGRESS, percent);
}

/**
 * Send system update info from fumo plugin.
 * @param [in] psu, it is an information about latest system update
 * @return On success, it returns 0; on error, it returns an error number.
 */
int pal_fumo_gui_set_system_update_info(pal_fumo_gui_system_update_info* psu)
{
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalFumoPluginLock);
    return IPAL_S_THIS->rpc(PAL_FUMO_PLUGIN_SYSTEM_UPDATE_INFO,
        psu->software_verion, psu->configuration_version,
        psu->latest_system_update_time, psu->status_message,
        psu->hyper_link);
}

/**
 *  @breif signal handler of SIGTERM and SIGUSR1 signals
 *  @param [in] signal number
 */
void fumo_sighandler(int sgnl)
{
    switch(sgnl) {
        case SIGTERM:
            break;
        case SIGUSR1:
            break;
        default:
            break;
    }
};

/**
 * @brief it is called by initFunc
 * @return On success, it returns 0
 */
int pal_fumo_init(void)
{
    if( SIG_ERR == signal(SIGTERM, fumo_sighandler) ||
        SIG_ERR == signal(SIGUSR1, fumo_sighandler) )
        return 1;
    return 0;
};

/**
 * @brief it is called by closeFunc
 * @return On success, it returns 0
 */
int pal_fumo_close(void)
{
    return 0;
};


/**
 *  @brief notification about network changing
 *  @param [in] nc network conditions
 *  @return MO_ERROR_SUCCESS if success
 */
int pal_fumo_network_conditions_changed(network_conditions_t nc)
{
    ALOGE("pal_fumo_network_conditions_changed, \
enabled = %d, net_feature= %d\n", (int)nc.enabled, (int)nc.net_feature);
    return MO_ERROR_SUCCESS;
}
__END_DECLS
