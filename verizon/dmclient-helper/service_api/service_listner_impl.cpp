/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "service_listener.h"

using namespace android;

namespace omadm_service_api {

/*
 * API serial code.
 * Keep in sync with android.omadm.IOmadmServiceListener.aidl
 *
 */
enum {
    OMADM_FUMO_USER_REPLY = IBinder::FIRST_CALL_TRANSACTION,
    OMADM_CONTROLLER_USER_REPLY,
    OMADM_ADMIN_NETWORK_STATUS,
    OMADM_NET_MANAGER_REPLY,
    OMADM_SCOMO_GET_BATTERY_STATE,
    OMADM_FUMO_GET_BATTERY_STATE,
    OMADM_FUMO_CHECK_FOR_UPDATE
};

// ----------------------------------------------------------------------------

class BpOmadmServiceListener: public BpInterface<IOmadmServiceListener>
{

public:
    BpOmadmServiceListener(const sp<IBinder>& impl)
        : BpInterface<IOmadmServiceListener>(impl) {}

    virtual void omadmFumoPluginDispachUserReply(
        int state, int64_t defered_update_time, bool wifi_requred,
        bool automatic_update_enable, int button_id)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(state);
        data.writeInt64(defered_update_time);
        data.writeInt32(wifi_requred ? 1 : 0);
        data.writeInt32(automatic_update_enable ? 1 : 0);
        data.writeInt32(button_id);
        remote()->transact(OMADM_FUMO_USER_REPLY, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmControllerDispachUserReply(
        int type, String16 input_text, int selected_choices_count,
        String16 selected_choices, int button_id)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(type);
        data.writeString16(input_text);
        data.writeInt32(selected_choices_count);
        data.writeString16(selected_choices);
        data.writeInt32(button_id);
        remote()->transact(OMADM_CONTROLLER_USER_REPLY, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmControllerDispachAdminNetStatus(int status, int net_id)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(status);
        data.writeInt32(net_id);
        remote()->transact(OMADM_ADMIN_NETWORK_STATUS, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmNetManagerReply(int net_feature, bool enabled)
    {
        Parcel data;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(net_feature);
        data.writeInt32(enabled ? 1 : 0);
        Parcel reply;
        remote()->transact(OMADM_NET_MANAGER_REPLY, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmScomoPluginDispachBatteryState(
        int battery_status, int battery_level)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(battery_status);
        data.writeInt32(battery_level);
        remote()->transact(OMADM_SCOMO_GET_BATTERY_STATE, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmFumoPluginDispachBatteryState(
        int battery_status, int battery_level)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        data.writeInt32(battery_status);
        data.writeInt32(battery_level);
        remote()->transact(OMADM_FUMO_GET_BATTERY_STATE, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }

    virtual void omadmFumoPluginDispachCheckForUpdate()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmServiceListener::getInterfaceDescriptor());
        remote()->transact(OMADM_FUMO_CHECK_FOR_UPDATE, data, &reply, IBinder::FLAG_ONEWAY);
        reply.readExceptionCode();
    }
};

IMPLEMENT_META_INTERFACE(OmadmServiceListener,
                         "android.omadm.IOmadmServiceListener");

// ----------------------------------------------------------------------

status_t BnOmadmServiceListener::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case OMADM_FUMO_USER_REPLY: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);
            int state = data.readInt32();
            int64_t defered_update_time = data.readInt64();
            bool wifi_requred = (0 != data.readInt32());
            bool automatic_update_enable = (0 != data.readInt32());
            int button_id = data.readInt32();
            omadmFumoPluginDispachUserReply(
                state, defered_update_time, wifi_requred,
                automatic_update_enable, button_id);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_CONTROLLER_USER_REPLY: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);
            int type = data.readInt32();
            String16 input_text = data.readString16();
            int selected_choices_count = data.readInt32();
            String16 selected_choices = data.readString16();
            int button_id = data.readInt32();
            omadmControllerDispachUserReply(
                type, input_text, selected_choices_count,
                selected_choices, button_id);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_ADMIN_NETWORK_STATUS: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);

            int status = data.readInt32();
            int net_id = data.readInt32();
            omadmControllerDispachAdminNetStatus(status, net_id);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_NET_MANAGER_REPLY: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);

            int net_feature = data.readInt32();
            bool enabled = (0 != data.readInt32());
            omadmNetManagerReply(net_feature, enabled);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_SCOMO_GET_BATTERY_STATE: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);
            int battery_status = data.readInt32();
            int battery_level = data.readInt32();
             omadmScomoPluginDispachBatteryState(
                battery_status, battery_level);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_FUMO_GET_BATTERY_STATE: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);
            int battery_status = data.readInt32();
            int battery_level = data.readInt32();
             omadmFumoPluginDispachBatteryState(
                battery_status, battery_level);
            reply->writeNoException();
            return NO_ERROR;
        } break;

        case OMADM_FUMO_CHECK_FOR_UPDATE: {
            CHECK_INTERFACE(IOmadmServiceListener, data, reply);
            omadmFumoPluginDispachCheckForUpdate();
            reply->writeNoException();
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; // namespace omadm_service_api