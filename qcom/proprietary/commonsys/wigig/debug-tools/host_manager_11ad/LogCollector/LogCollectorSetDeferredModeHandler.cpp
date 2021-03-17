/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorSetDeferredModeHandler.h"
#include "LogCollector.h"
#include "Host.h"

void LogCollectorSetDeferredModeHandler::HandleRequest(const LogCollectorSetDeferredModeRequest& jsonRequest, LogCollectorSetDeferredModeResponse& jsonResponse)
{
    JsonValueBoxed<bool> setRecordingMode = jsonRequest.GetRecordingMode();

    if (setRecordingMode.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        std::stringstream ss;
        ss << "Couldn't parse 'RecordLogs' field: " << setRecordingMode.GetState();
        jsonResponse.Fail(ss.str());
        return;
    }

   /* JsonValueBoxed<bool> setPublishLogsMode = jsonRequest.GetPublishLogsMode();

    if (setPublishLogsMode.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        std::stringstream ss;
        ss << "Couldn't parse 'PublishLogs' field: " << setPublishLogsMode.GetState();
        jsonResponse.Fail(ss.str());
        return;
    }*/

    //LOG_INFO << "Deferred recording mode was set to " << setRecordingMode.GetValue() << std::endl;
    Host::GetHost().GetDeviceManager().SetLogRecordingDeferredMode(setRecordingMode.GetValue());

    //LOG_INFO << "Deferred publishing mode was set to " << setPublishLogsMode.GetValue() << std::endl;
    //Host::GetHost().GetDeviceManager().SetLogPublishingDeferredMode(setPublishLogsMode.GetValue());
}

