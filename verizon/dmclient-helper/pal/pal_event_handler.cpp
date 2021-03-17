/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/Errors.h>
#include <binder/Parcel.h>

#include "pal_event_handler.h"
#include "pal_event_listener.h"
#include "pal_internal.h"

namespace android {
#undef LOG_TAG
#define LOG_TAG "pal_event_handler"

#define WPUSH_MSG_HDR_SIZE 24
#define SU_CANCEL_DATA_SIZE 13
#define SU_CANCEL_TRIGGER_STR "1010101010101"
#define SU_CANCEL_TRIGGER_STR_UTF16 u"1010101010101"

/**
 * Omadm WAP PUSH message handler
 *
 * @param[in] trans_id transaction Id, shall be used for response to server
 * @param[in] pdu_content - syncml message
 * @return success or error code
 */
status_t OmadmEventHandler::WapPushSysUpdate(int trans_id, String16& pdu)
{
    UNUSED(trans_id);
    int ret = RESULT_MEMORY_ERROR;

    int pkg0_size = pdu.size();
    pdu_t * pkg0 = (pdu_t*)malloc(sizeof(pdu_t));
    pkg0->length = pkg0_size;
    pkg0->data = (char*)malloc(pkg0_size);
//abing
    //char * data = (char*)pdu.sharedBuffer()->data();
    char * data = (char*)pdu.string();
    if (pkg0->data) {
        for (int i = 0; i < pkg0_size; i++) {
            pkg0->data[i] = data[i<<1];
            ALOGI("ptr[%d] = %x\n", i, pkg0->data[i] );
        }
        int srv_id_length = pkg0->data[WPUSH_MSG_HDR_SIZE-1];
        int vnd_data_length = pkg0_size - srv_id_length - WPUSH_MSG_HDR_SIZE;
        if (vnd_data_length == SU_CANCEL_DATA_SIZE) {
            /* got SU CANCEL command in vendor specific field */
            int vnd_data_offset = WPUSH_MSG_HDR_SIZE + srv_id_length;
            if (!strncmp(&pkg0->data[vnd_data_offset], SU_CANCEL_TRIGGER_STR,
                    vnd_data_length)) {
                ret = mEventNotifyCallback(SU_CANCEL, pkg0);
            }
        } else if (vnd_data_length != 0) {
            ALOGE("OmadmEventHandler::WapPushSysUpdate() msg format fault,"
                " pkg0 size %d, srv_id_length %d, vendor data size %d, ",
                pkg0_size, srv_id_length, vnd_data_length);
            ret = RESULT_PARSE_ERROR;
        } else if (mEventNotifyCallback) {
            ALOGI("OmadmEventHandler::WapPushSysUpdate() bytes = %d, server name length = %d",
                pkg0_size, srv_id_length);
            ret = mEventNotifyCallback(PACKAGE0, pkg0);
            ALOGI("OmadmEventHandler::WapPushSysUpdate() package0 sent");
        }
    }

    if (ret != RESULT_SUCCESS) {
        if (pkg0->data != NULL) free(pkg0->data);
        if (pkg0 != NULL) free(pkg0);
    }
    /// \todo: to figure out who is responsible to free data
    return ret;
};

/**
 * User system update request handler
 *
 * @param[in] enable - true enables update, false disables one.
 * @return success or error code
 */
status_t OmadmEventHandler::UserEnableSysUpdate(bool enabled)
{
    UNUSED(enabled);
    int ret = RESULT_MEMORY_ERROR;
    ALOGD("OmadmEventHandler::UserEnableSysUpdate(");
    if (mEventNotifyCallback)
        ret = mEventNotifyCallback(USER_SYS_UPDATE, NULL);
    return ret;
};

/**
 * Omadm SMS Cancel SU message handler
 *
 * @param[in] sms_user_data - vendor specific data, supposed to be 1010101010101
 * @return success or error code
 */
status_t OmadmEventHandler::SmsCancelSysUpdate(String16& sms_user_data)
{
    int cbytes = sms_user_data.size();
    int ret = RESULT_MEMORY_ERROR;
    ALOGI("OmadmEventHandler::SmsCancelSysUpdate");
    if (!memcmp(sms_user_data.string(),SU_CANCEL_TRIGGER_STR_UTF16, cbytes << 1)) {
        if (mEventNotifyCallback) {
            ret = mEventNotifyCallback(SU_CANCEL, NULL);
            ALOGI("EH:SU cancel event sent");
        }
    } else {
        ALOGI("EH:SU cancel trigger didn't match");
        ret = RESULT_PARSE_ERROR;
    }
    return ret;
};

/**
 * IPC entry point - event data unmarshalling.
 * @param[in] code type of the method to be called
 * @param[in] p parcel object having marshalled data
 * @param[out] reply returns success or error to caller
 * @param[in] flags auxiliary data
 */
 status_t BnOmadmEventHandler::onTransact(uint32_t code, const Parcel& p,
    Parcel* reply, uint32_t flags)
{
    UNUSED(flags);
    switch (code) {
    case OMADM_WAP_PUSH_PCKG0:
    {
        CHECK_INTERFACE(IEventHandler, p, reply);
        int trans_id = p.readInt32();
        String16 pdu_content = p.readString16();
        reply->writeInt32(WapPushSysUpdate(trans_id, pdu_content));
        break;
    }
    case OMADM_USER_SYSTEM_UPDATE:
    {
        CHECK_INTERFACE(IEventHandler, p, reply);
        reply->writeInt32(UserEnableSysUpdate(p.readInt32()));
        break;
    }
    case OMADM_SMS_CANCEL_SYS_UPDATE:
    {
        CHECK_INTERFACE(IEventHandler, p, reply);
        String16 sms_user_data = p.readString16();
        reply->writeInt32(SmsCancelSysUpdate(sms_user_data));
        break;
    }
    default: break;
    }
    return true;
}

}