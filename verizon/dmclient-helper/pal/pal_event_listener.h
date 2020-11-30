/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#ifndef __PAL_EVENT_LISTENER__
#define __PAL_EVENT_LISTENER__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include "pal.h"

namespace android {

    enum {
        OMADM_WAP_PUSH_PCKG0 = IBinder::FIRST_CALL_TRANSACTION,
        OMADM_USER_SYSTEM_UPDATE,
        OMADM_SMS_CANCEL_SYS_UPDATE,
    };

    /**
     * This is a Binder interface shared between client and server
     */
    class IOmadmEventHandler: public IInterface
    {
    public:
        DECLARE_META_INTERFACE(OmadmEventHandler);
        virtual status_t WapPushSysUpdate(int tranId, String16& pdu) = 0;
        virtual status_t UserEnableSysUpdate(bool enabled) = 0;
        virtual status_t SmsCancelSysUpdate(String16& sms_user_data) = 0;
    };

    class BnOmadmEventHandler: public BnInterface<IOmadmEventHandler>
    {
    public:
        virtual status_t onTransact( uint32_t code,
                const Parcel& data,
                Parcel* reply,
                uint32_t flags = 0);
    };
}

#endif