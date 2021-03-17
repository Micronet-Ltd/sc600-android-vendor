/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#ifndef IOMADM_SERVICE_LISTENER_H
#define IOMADM_SERVICE_LISTENER_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

#include "pal.h"

using namespace android;

namespace omadm_service_api {

class OmadmServiceListener : public virtual RefBase
{
public:
    OmadmServiceListener() {}
    virtual ~OmadmServiceListener() {}

    /**
     * Retrieve information for OMADM_FUMO_USER_REPLY
     * omadm callback
     */
    virtual void omadmFumoPluginDispachUserReply(
        int state, int64_t defered_update_time, bool wifi_requred,
        bool automatic_update_enable, int button_id) = 0;

    /**
     * Retrieve information for OMADM_CONTROLLER_USER_REPLY
     * omadm callback
     */
    virtual void omadmControllerDispachUserReply(
        int type, String16 input_text, int selected_choices_count,
        String16 selected_choices, int button_id) = 0;

    /**
     * Retrieve information for OMADM_FUMO_CHECK_FOR_UPDATE
     * omadm callback
     */
    virtual void omadmFumoPluginDispachCheckForUpdate() = 0;

    /**
     * Retrieve information for OMADM_NET_MANAGER_REPLY
     * omadm callback
     */
    virtual void omadmNetManagerReply(int net_feature, bool enabled) = 0;

    /**
     * Retrieve information for OMADM_ADMIN_NETWORK_STATUS
     * omadm callback
     */
    virtual void omadmControllerDispachAdminNetStatus(int status, int net_id) = 0;

    /**
     * Retrieve information for OMADM_SCOMO_GET_BATTERY_STATUS
     * omadm callback
     */
    virtual void omadmScomoPluginDispachBatteryState(
        int battery_status, int battery_level) = 0;
    /**
     * Retrieve information for OMADM_FUMO_GET_BATTERY_STATUS
     * omadm callback
     */
    virtual void omadmFumoPluginDispachBatteryState(
        int battery_status, int battery_level) = 0;
};

class IOmadmServiceListener : public OmadmServiceListener, public IInterface
{
public:
     DECLARE_META_INTERFACE(OmadmServiceListener);
 };

// ----------------------------------------------------------------------------

class BnOmadmServiceListener : public BnInterface<IOmadmServiceListener>
{
public:
    virtual status_t onTransact( uint32_t code, const Parcel& data, Parcel* reply,
            uint32_t flags = 0);
};

class OmadmListener: public BnOmadmServiceListener
{
public:
    static sp<IOmadmServiceListener> mOmadmListener;
    virtual void omadmFumoPluginDispachUserReply(
        int state, int64_t defered_update_time, bool wifi_requred,
        bool automatic_update_enable, int button_id);
    virtual void omadmControllerDispachUserReply(
        int type, String16 input_text, int selected_choices_count,
        String16 selected_choices, int button_id);
    virtual void omadmNetManagerReply(int net_feature, bool enabled);
    virtual void omadmControllerDispachAdminNetStatus(int status, int net_id);
    virtual void omadmScomoPluginDispachBatteryState(
        int battery_status, int battery_level);
    virtual void omadmFumoPluginDispachBatteryState(
        int battery_status, int battery_level);
    virtual void omadmFumoPluginDispachCheckForUpdate();

    // Utility functions
    virtual const char** omadmStringSplit(const String8& str,
        const char* delimiter, int counter);
};

}; // namespace omadm_service_api

#endif
