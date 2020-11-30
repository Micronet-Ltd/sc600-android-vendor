/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <memory>
#include <framework/SolicitedMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/message_translator.h>
#include <modules/android/ril_message_factory.h>
#include <translators/android/legacy_translator.h>
#include <modules/sms/RilRequestSendSmsExpectMoreMessage.h>

class send_sms_expect_more_translator: public legacy_translator<RIL_REQUEST_SEND_SMS_EXPECT_MORE> {
  public:
    send_sms_expect_more_translator() = default;
    ~send_sms_expect_more_translator() {};

    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request) {
      return std::make_shared<RilRequestSendSmsExpectMoreMessage>(request);
    }
};
