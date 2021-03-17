/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorSetModeHandler.h"
#include "LogCollector.h"
#include "Host.h"

void LogCollectorSetModeHandler::HandleRequest(const LogCollectorSetModeRequest& jsonRequest, LogCollectorSetModeResponse& jsonResponse)
{
    std::string deviceName = jsonRequest.GetDeviceName();
    if (deviceName.empty()) // key is missing or an empty string provided
    {
        const OperationStatus os = Host::GetHost().GetDeviceManager().GetDefaultDevice(deviceName);
        if (!os)
        {
            jsonResponse.Fail(os.GetStatusMessage());
            return;
        }
    }

    const JsonValueBoxed<CpuType> cpuType = jsonRequest.GetCpuType();
    if (cpuType.GetState() == JsonValueState::JSON_VALUE_MISSING)
    {
        jsonResponse.Fail("CpuType field is missing");
        return;
    }
    if (cpuType.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail("CpuType is wrong, it should be 'fw' or 'ucode'");
        return;
    }

    LOG_DEBUG << "Log Collector set mode request for Device: " << deviceName << " with CPU type: " << cpuType << std::endl;

    JsonValueBoxed<bool> setRecordingMode = jsonRequest.GetRecordingMode();

    if (setRecordingMode.GetState() != JsonValueState::JSON_VALUE_PROVIDED)
    {
        std::stringstream ss;
        ss << "Couldn't parse 'RecordLogs' field: " << setRecordingMode.GetState();
        jsonResponse.Fail(ss.str());
        return;
    }
    LOG_INFO << "Recording mode was set to " << setRecordingMode.GetValue() << std::endl;
    OperationStatus os = Host::GetHost().GetDeviceManager().SetLogRecordingMode(setRecordingMode.GetValue(), deviceName, cpuType);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    JsonValueBoxed<bool> setPublishLogsModeBoxed = jsonRequest.GetPublishLogsMode();
    bool setPublishLogsMode = false;
    if (setPublishLogsModeBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        std::stringstream ss;
        ss << "Couldn't parse 'PublishLogs' field: " << setPublishLogsModeBoxed.GetState();
        jsonResponse.Fail(ss.str());
        return;
    }
    if (setPublishLogsModeBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        setPublishLogsMode = setPublishLogsModeBoxed;
    }
    LOG_INFO << "Collection mode was set to " << setPublishLogsMode << std::endl;
    os = Host::GetHost().GetDeviceManager().SetLogPublishingMode(setPublishLogsMode, deviceName, cpuType);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }
}

