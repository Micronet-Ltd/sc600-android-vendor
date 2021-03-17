/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#ifndef __PAL_EVENT_HANDLER__
#define __PAL_EVENT_HANDLER__

#include <binder/IBinder.h>
#include "pal.h"
#include "pal_event_listener.h"

namespace android {

typedef struct _pdu_t {
   char * data;
   int length;
} pdu_t;

/**
 * OmadmEventHandler is the interface through which the OMADM
 * java service notifies the event handler of some events.
 *
 * It playes the listener role and listen for 5 events:
 * @method DispatchWapPush() provides WAP PUSH event
 * @method UserEnableUpdate() provides USER UPDATE request event
 *
 * SetNotificationCallback() provides dinamic link with OMADM
 * daemon so that to be able to send the events above further.
 */
class OmadmEventHandler: public BnOmadmEventHandler {
public:
    status_t WapPushSysUpdate(int trans_id, String16& pdu);
    status_t UserEnableSysUpdate(bool enabled);
    status_t SmsCancelSysUpdate(String16& sms_user_data);
    void SetNotificationCallback(process_omadm_event_t callback) {
        mEventNotifyCallback = callback;
    }
private:
    process_omadm_event_t mEventNotifyCallback;
};

    /**
     * initializes event handler object, shall be invoked from .init
     * section of shared library
     * @return omadm_event_handler singleton
     */
    OmadmEventHandler * pal_init_event_handler(void);

    /**
     * finalizes event handler, shall be invoked from .fini
     * section of shared library
     */
    void pal_fini_event_handler(void);

    #ifdef __cplusplus
    extern "C" {
    #endif
    /**
     * Set callback function for WAP push or "Check for update" events
     *
     * @param[in] process_omadm_event Callback function
     * @return Rresult code
     */
    int pal_notify_on_omadm_event(process_omadm_event_t process_omadm_event);
    #ifdef __cplusplus
    }
    #endif
} // namespace

#endif
