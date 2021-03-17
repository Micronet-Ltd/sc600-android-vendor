/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <cutils/sockets.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pal.h"
#include "pal_event_listener.h"
#include "pal_event_handler.h"

namespace android {

#undef LOG_TAG
#define LOG_TAG "pal_event_listener"
#define DMC_MAX_COMMAND_BYTES   1024

static OmadmEventHandler * event_handler = NULL;

/**
 * The stub between the server and client
 */
class BpOmadmEventHandler : public BpInterface<IOmadmEventHandler>
{
public:
    BpOmadmEventHandler(const sp<IBinder>& impl );
    status_t WapPushSysUpdate(int tranId, String16& pdu){
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmEventHandler::getInterfaceDescriptor());
        data.writeInt32(tranId);
        data.writeString16(pdu);
        remote()->transact(OMADM_WAP_PUSH_PCKG0, data, &reply, IBinder::FLAG_ONEWAY);
        return -ENOSYS;
    };
    status_t UserEnableSysUpdate(bool enabled){
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmEventHandler::getInterfaceDescriptor());
        data.writeInt32(enabled);
        remote()->transact(OMADM_USER_SYSTEM_UPDATE, data, &reply, IBinder::FLAG_ONEWAY);
        return -ENOSYS;
    };
    status_t SmsCancelSysUpdate(String16& pdu){
        Parcel data, reply;
        data.writeInterfaceToken(IOmadmEventHandler::getInterfaceDescriptor());
        data.writeString16(pdu);
        remote()->transact(OMADM_SMS_CANCEL_SYS_UPDATE, data, &reply, IBinder::FLAG_ONEWAY);
        return -ENOSYS;
    };
};

BpOmadmEventHandler::BpOmadmEventHandler(const sp<IBinder>& impl ):
   BpInterface<IOmadmEventHandler>(impl) {}

IMPLEMENT_META_INTERFACE(OmadmEventHandler,"android.omadm.IOmadmEventHandler");

/**
 * Initializes event handler object, shall be invoked from .init
 * section of shared library. It should be added to java OMADM service
 * as listener so that to get events.
 */
OmadmEventHandler * pal_init_event_handler(void)
{
    if (event_handler != NULL)
        return event_handler;

        event_handler = new OmadmEventHandler();
    return event_handler;
}

/**
 * Finalizes event handler, shall be invoked from .fini
 * section of shared library
 */
void pal_fini_event_handler(void)
{
    delete event_handler;
    ALOGE ("pal_fini_event_handler()");
}

/**
 * Set callback to omadm event handler function
 * @param[in] process_omadm_event omadm client callback function
 * @return
 */
int pal_notify_on_omadm_event(process_omadm_event_t process_omadm_event)
{
    if (process_omadm_event == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    if (event_handler != NULL) {
        event_handler->SetNotificationCallback(process_omadm_event);
    }

    return RESULT_SUCCESS;
}

}