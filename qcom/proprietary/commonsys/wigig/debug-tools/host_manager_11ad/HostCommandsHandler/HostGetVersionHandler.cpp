/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "Host.h"
#include "HostGetVersionHandler.h"
#include <set>
#include <string>

void HostGetVersionHandler::HandleRequest(const Json::Value& jsonRequest, HostGetVersionResponse& jsonResponse)
{
    LOG_DEBUG << "Host commands, get_version" << std::endl;

    (void)jsonRequest;
    string version = Host::GetHost().GetHostInfo().GetVersion();

    jsonResponse.SetVersion(version);

}

void HostGetVersionResponse::SetVersion(const std::string& version)
{
    m_jsonResponseValue["Version"] = version;
}
