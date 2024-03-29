/******************************************************************************
  @file    qcril_qmi_ims_radio_utils.c
  @brief   qcril qmi - ims_radio_utils

  DESCRIPTION
    Implements the utils to be used by the ImsRadioImpl class

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define TAG "RILQ"

#include "qcril_qmi_ims_radio_utils.h"
#include <string.h>
#include <framework/Log.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_0 {
namespace utils {

ims_Error convertHidlToProtoDial(const V1_4::DialRequest& inDial, ims_Dial& outDial) {
    ims_Error ret = ims_Error_E_SUCCESS;

    do {
        if (!inDial.address.empty()) {
            int addressLen = inDial.address.size();
            size_t destSize = sizeof(char) * (addressLen + 1);
            outDial.address.arg = (char *) qcril_malloc(destSize);

            if (outDial.address.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for dial address.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            strlcpy((char*)outDial.address.arg, inDial.address.c_str(), destSize);
        } else {
            QCRIL_LOG_INFO("Dial address is null.");
            // Empty dial string is allowed for conference uri call
            if (!(inDial.hasIsConferenceUri && inDial.isConferenceUri)) {
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
        }

        if (inDial.clirMode != INT32_MAX) {
            outDial.has_clir = true;
            outDial.clir = inDial.clirMode;
        }

        if (inDial.hasCallDetails) {
            ret = convertHidlToProtoCallDetails(inDial.callDetails, outDial.callDetails);

            if (isError(ret)) {
                break;
            }

            outDial.has_callDetails = true;

        } else {
            QCRIL_LOG_INFO("Call details is not present.");
        }

        if (inDial.presentation != IpPresentation::IP_PRESENTATION_INVALID) {
            outDial.presentation = convertHidlToProtoPresentation(inDial.presentation);
            outDial.has_presentation = true;
        }

        if (inDial.hasIsConferenceUri) {
            outDial.isConferenceUri = inDial.isConferenceUri;
            outDial.has_isConferenceUri = true;
        }

        if (inDial.hasIsCallPull) {
            outDial.isCallPull = inDial.isCallPull;
            outDial.has_isCallPull = true;
        }

        if (inDial.hasIsEncrypted) {
            outDial.isEncrypted = inDial.isEncrypted;
            outDial.has_isEncrypted = true;
        }

        //Originating Number
        auto& info = inDial.multiLineInfo;
        if (!info.msisdn.empty()) {
            outDial.originatingNumber.arg = (char *)qcril_malloc(
                    info.msisdn.size() + 1);
            if (outDial.originatingNumber.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for originating Number.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
            strlcpy((char*)outDial.originatingNumber.arg, info.msisdn.c_str(),
                    info.msisdn.size() + 1);
            outDial.isSecondary =
                info.lineType == V1_4::MultiIdentityLineType::LINE_TYPE_SECONDARY;
        }
    } while(0);

    if (isError(ret)) {
        qcril_free(outDial.address.arg);
        outDial.address.arg = nullptr;
    }
    return ret;
}

ims_Error convertHidlToProtoCallDetails(const CallDetails& inCallDetails,
    ims_CallDetails& outCallDetails) {
    if (inCallDetails.callType == CallType::CALL_TYPE_INVALID ||
            inCallDetails.callDomain == CallDomain::CALL_DOMAIN_INVALID) {
        QCRIL_LOG_ERROR("Call type or domain is invalid.");
        return ims_Error_E_GENERIC_FAILURE;
    }

    ims_Error ret = ims_Error_E_SUCCESS;
    outCallDetails.callType = convertHidlToProtoCallType(inCallDetails.callType);
    outCallDetails.has_callType = true;
    outCallDetails.callDomain = convertHidlToProtoCallDomain(inCallDetails.callDomain);
    outCallDetails.has_callDomain = true;

    uint32_t extrasLen = inCallDetails.extras.size();
    do {
        if (extrasLen > 0) {
            outCallDetails.extras.arg = qcril_malloc((extrasLen + 1) * sizeof(char*));
            if (outCallDetails.extras.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for call detail extras.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            if ((ret = convertHidlToProtoCallDetailsExtras(inCallDetails.extras,
                    (char**)outCallDetails.extras.arg)) != ims_Error_E_SUCCESS) {
                break;
            }
        }

        if ((ret = convertHidlToProtoServiceStatusInfoList(inCallDetails.localAbility,
                (ims_Info**)(outCallDetails.localAbility.SrvStatusInfo.arg))) !=
                ims_Error_E_SUCCESS) {
            break;
        }

        if ((ret = convertHidlToProtoServiceStatusInfoList(inCallDetails.peerAbility,
                (ims_Info**)(outCallDetails.peerAbility.SrvStatusInfo.arg))) !=
                ims_Error_E_SUCCESS) {
            break;
        }

        if (inCallDetails.callSubstate != INT32_MAX) {
            outCallDetails.has_callSubstate = true;
            outCallDetails.callSubstate = (ims_CallSubstate)inCallDetails.callSubstate;
        }

        if (inCallDetails.mediaId != INT32_MAX) {
            outCallDetails.has_mediaId = true;
            outCallDetails.mediaId = inCallDetails.mediaId;
        }

        if (inCallDetails.causeCode != INT32_MAX) {
            outCallDetails.has_causeCode = true;
            outCallDetails.causeCode = (ims_CauseCodes)inCallDetails.causeCode;
        }

        if(inCallDetails.rttMode !=  RttMode::RTT_MODE_INVALID){
            outCallDetails.has_rttMode = true;
            outCallDetails.rttMode = convertHidlToProtoRttMode(inCallDetails.rttMode);
        }

        const uint32_t sipUriStrLen = inCallDetails.sipAlternateUri.size();
        if (sipUriStrLen > 0) {
            size_t destSize = sizeof(char) * (sipUriStrLen + 1);
            outCallDetails.sipAlternateUri.arg = qcril_malloc(destSize);

            if (outCallDetails.sipAlternateUri.arg != nullptr) {
                strlcpy((char*)outCallDetails.sipAlternateUri.arg,
                            inCallDetails.sipAlternateUri.c_str(),
                            destSize);
            } else {
                QCRIL_LOG_ERROR("Failed to allocate memory for call detail sipAlternateUri.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
        }
    } while(0);

    if (isError(ret)) {
        qcril_free(outCallDetails.extras.arg);
        outCallDetails.extras.arg = nullptr;
    }

    return ret;
}

ims_Error convertHidlToProtoCallDetailsExtras(
        const android::hardware::hidl_vec<android::hardware::hidl_string>& inCallDetailsExtras,
        char** outCallDetailsExtras) {
    for (uint32_t i = 0; i < inCallDetailsExtras.size(); i++) {
        size_t destSize = sizeof(char) * (inCallDetailsExtras[i].size() + 1);
        outCallDetailsExtras[i] =  (char *) qcril_malloc(destSize);

        if (outCallDetailsExtras[i] == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory for call details extras.");
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy(outCallDetailsExtras[i], inCallDetailsExtras[i].c_str(), destSize);
        QCRIL_LOG_INFO("ims_radio_utils: dial extras[%d] %s", i, outCallDetailsExtras[i]);
    }

    return ims_Error_E_SUCCESS;
}

ims_Error convertHidlToProtoConfig(const ConfigInfo& inConfig, ims_ConfigMsg& outConfig) {
    outConfig.has_item = (convertHidlToProtoConfigItem(inConfig.item, outConfig.item) ==
            ims_Error_E_SUCCESS) ? true : false;

    outConfig.has_boolValue = inConfig.hasBoolValue;
    if (inConfig.hasBoolValue) {
        outConfig.boolValue = inConfig.boolValue;
    }

    if (inConfig.intValue != INT32_MAX) {
        outConfig.has_intValue = true;
        outConfig.intValue = inConfig.intValue;
    }

    if (convertHidlToProtoString(inConfig.stringValue, (char*&)outConfig.stringValue.arg) !=
            ims_Error_E_SUCCESS) {
        return ims_Error_E_GENERIC_FAILURE;
    }

    outConfig.has_errorCause = (convertHidlToProtoErrorCause(inConfig.errorCause,
            outConfig.errorCause) == ims_Error_E_SUCCESS) ? true : false;

    return ims_Error_E_SUCCESS;
}

int convertHidlToProtoFacilityType(FacilityType facilityType) {
     switch (facilityType) {
        case FacilityType::FACILITY_CLIP:
            return ims_SuppSvcFacilityType_FACILITY_CLIP;
        case FacilityType::FACILITY_COLP:
            return ims_SuppSvcFacilityType_FACILITY_COLP;
        case FacilityType::FACILITY_BAOC:
            return ims_SuppSvcFacilityType_FACILITY_BAOC;
        case FacilityType::FACILITY_BAOIC:
            return ims_SuppSvcFacilityType_FACILITY_BAOIC;
        case FacilityType::FACILITY_BAOICxH:
            return ims_SuppSvcFacilityType_FACILITY_BAOICxH;
        case FacilityType::FACILITY_BAIC:
            return ims_SuppSvcFacilityType_FACILITY_BAIC;
        case FacilityType::FACILITY_BAICr:
            return ims_SuppSvcFacilityType_FACILITY_BAICr;
        case FacilityType::FACILITY_BA_ALL:
            return ims_SuppSvcFacilityType_FACILITY_BA_ALL;
        case FacilityType::FACILITY_BA_MO:
            return ims_SuppSvcFacilityType_FACILITY_BA_MO;
        case FacilityType::FACILITY_BA_MT:
            return ims_SuppSvcFacilityType_FACILITY_BA_MT;
        case FacilityType::FACILITY_BS_MT:
            return ims_SuppSvcFacilityType_FACILITY_BS_MT;
        case FacilityType::FACILITY_BAICa:
            return ims_SuppSvcFacilityType_FACILITY_BAICa;
        default:
            return INT32_MAX;
    }
}

ims_Error convertHidlToProtoConfigItem(const ConfigItem& inConfigItem, ims_ConfigItem& outConfigItem) {
    ims_Error ret = ims_Error_E_SUCCESS;
    switch (inConfigItem) {
        case ConfigItem::CONFIG_ITEM_NONE:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_NONE;
            break;
        case ConfigItem::CONFIG_ITEM_VOCODER_AMRMODESET:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET;
            break;
        case ConfigItem::CONFIG_ITEM_VOCODER_AMRWBMODESET:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_SESSION_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_MIN_SESSION_EXPIRY:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY;
            break;
        case ConfigItem::CONFIG_ITEM_CANCELLATION_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_T_DELAY:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_T_DELAY;
            break;
        case ConfigItem::CONFIG_ITEM_SILENT_REDIAL_ENABLE:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_T1_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_T2_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_TF_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_VLT_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_LVC_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_DOMAIN_NAME:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME;
            break;
        case ConfigItem::CONFIG_ITEM_SMS_FORMAT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT;
            break;
        case ConfigItem::CONFIG_ITEM_SMS_OVER_IP:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP;
            break;
        case ConfigItem::CONFIG_ITEM_PUBLISH_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
            break;
        case ConfigItem::CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
            break;
        case ConfigItem::CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
            break;
        case ConfigItem::CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
            break;
        case ConfigItem::CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
            break;
        case ConfigItem::CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
            break;
        case ConfigItem::CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
            break;
        case ConfigItem::CONFIG_ITEM_GZIP_FLAG:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG;
            break;
        case ConfigItem::CONFIG_ITEM_EAB_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_MOBILE_DATA_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
            break;
        case ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
            break;
        case ConfigItem::CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_EMERGENCY_CALL_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
            break;
        case ConfigItem::CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
            break;
        case ConfigItem::CONFIG_ITEM_LBO_PCSCF_ADDRESS:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS;
            break;
        case ConfigItem::CONFIG_ITEM_KEEP_ALIVE_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
            break;
        case ConfigItem::CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
            break;
        case ConfigItem::CONFIG_ITEM_SPEECH_START_PORT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT;
            break;
        case ConfigItem::CONFIG_ITEM_SPEECH_END_PORT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            break;
        case ConfigItem::CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
            break;
        case ConfigItem::CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
            break;
        case ConfigItem::CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
            break;
        case ConfigItem::CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
            break;
        case ConfigItem::CONFIG_ITEM_DTMF_WB_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT;
            break;
        case ConfigItem::CONFIG_ITEM_DTMF_NB_PT:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT;
            break;
        case ConfigItem::CONFIG_ITEM_AMR_DEFAULT_MODE:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE;
            break;
        case ConfigItem::CONFIG_ITEM_SMS_PSI:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SMS_PSI;
            break;
        case ConfigItem::CONFIG_ITEM_VIDEO_QUALITY:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_LTE1:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_LTE2:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_LTE3:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_1x:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_WIFI_A:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A;
            break;
        case ConfigItem::CONFIG_ITEM_THRESHOLD_WIFI_B:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B;
            break;
        case ConfigItem::CONFIG_ITEM_T_EPDG_LTE:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE;
            break;
        case ConfigItem::CONFIG_ITEM_T_EPDG_WIFI:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI;
            break;
        case ConfigItem::CONFIG_ITEM_T_EPDG_1x:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x;
            break;
        case ConfigItem::CONFIG_ITEM_VWF_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_VCE_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_RTT_SETTING_ENABLED:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED;
            break;
        case ConfigItem::CONFIG_ITEM_SMS_APP:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_SMS_APP;
            break;
        case ConfigItem::CONFIG_ITEM_VVM_APP:
            outConfigItem = ims_ConfigItem_CONFIG_ITEM_VVM_APP;
            break;
        default:
            ret = ims_Error_E_GENERIC_FAILURE;
    }

    return ret;
}

ims_Error convertHidlToProtoErrorCause(const ConfigFailureCause& inErrorCause,
        ims_ConfigFailureCause& outErrorCause) {
    ims_Error ret = ims_Error_E_SUCCESS;
    switch (inErrorCause) {
    case ConfigFailureCause::CONFIG_NO_ERR:
        outErrorCause = ims_ConfigFailureCause_CONFIG_NO_ERR;
        break;
    case ConfigFailureCause::CONFIG_IMS_NOT_READY:
        outErrorCause = ims_ConfigFailureCause_CONFIG_IMS_NOT_READY;
        break;
    case ConfigFailureCause::CONFIG_FILE_NOT_AVAILABLE:
        outErrorCause = ims_ConfigFailureCause_CONFIG_FILE_NOT_AVAILABLE;
        break;
    case ConfigFailureCause::CONFIG_READ_FAILED:
        outErrorCause = ims_ConfigFailureCause_CONFIG_READ_FAILED;
        break;
    case ConfigFailureCause::CONFIG_WRITE_FAILED:
        outErrorCause = ims_ConfigFailureCause_CONFIG_WRITE_FAILED;
        break;
    case ConfigFailureCause::CONFIG_OTHER_INTERNAL_ERR:
        outErrorCause = ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR;
        break;
    default:
        ret = ims_Error_E_GENERIC_FAILURE;
    }

    return ret;
}

void convertProtoToHidlRegistration(const ims_Registration& inReg, RegistrationInfo& outReg) {
    if (inReg.has_state) {
        outReg.state = convertProtoToHidlRegState(inReg.state);
    } else {
        outReg.state = RegState::INVALID;
    }

    if (inReg.has_errorCode) {
        outReg.errorCode = inReg.errorCode;
    } else {
        outReg.errorCode = INT32_MAX;
    }

    convertProtoToHidlString((const char*)inReg.errorMessage.arg, outReg.errorMessage);

    if (inReg.has_radioTech) {
        outReg.radioTech = convertProtoToHidlRadioTech(inReg.radioTech);
    } else {
        outReg.radioTech = RadioTechType::RADIO_TECH_INVALID;
    }

    const char* ptr = (const char*)inReg.pAssociatedUris.arg;
    if (ptr != nullptr) {
        outReg.pAssociatedUris.setToExternal(ptr, strlen(ptr));
    }
}

RegState convertProtoToHidlRegState(const ims_Registration_RegState& state) {
    switch (state) {
        case ims_Registration_RegState_REGISTERED:
            return RegState::REGISTERED;
        case ims_Registration_RegState_NOT_REGISTERED:
            return RegState::NOT_REGISTERED;
        case ims_Registration_RegState_REGISTERING:
            return RegState::REGISTERING;
        default:
            return RegState::INVALID;
    }
}

ims_Registration_RegState convertHidlToProtoRegState(const RegState& state) {
    switch (state) {
        case RegState::REGISTERED:
            return ims_Registration_RegState_REGISTERED;
        case RegState::NOT_REGISTERED:
            return ims_Registration_RegState_NOT_REGISTERED;
        case RegState::REGISTERING:
            return ims_Registration_RegState_REGISTERING;
        default:
            return ims_Registration_RegState_NOT_REGISTERED;
    }
}

RadioTechType convertProtoToHidlRadioTech(const ims_RadioTechType& radioTechType) {
    switch (radioTechType) {
        case ims_RadioTechType_RADIO_TECH_ANY:
            return RadioTechType::RADIO_TECH_ANY;
        case ims_RadioTechType_RADIO_TECH_UNKNOWN:
            return RadioTechType::RADIO_TECH_UNKNOWN;
        case ims_RadioTechType_RADIO_TECH_GPRS:
            return RadioTechType::RADIO_TECH_GPRS;
        case ims_RadioTechType_RADIO_TECH_EDGE:
            return RadioTechType::RADIO_TECH_EDGE;
        case ims_RadioTechType_RADIO_TECH_UMTS:
            return RadioTechType::RADIO_TECH_UMTS;
        case ims_RadioTechType_RADIO_TECH_IS95A:
            return RadioTechType::RADIO_TECH_IS95A;
        case ims_RadioTechType_RADIO_TECH_IS95B:
            return RadioTechType::RADIO_TECH_IS95B;
        case ims_RadioTechType_RADIO_TECH_1xRTT:
            return RadioTechType::RADIO_TECH_1xRTT;
        case ims_RadioTechType_RADIO_TECH_EVDO_0:
            return RadioTechType::RADIO_TECH_EVDO_0;
        case ims_RadioTechType_RADIO_TECH_EVDO_A:
            return RadioTechType::RADIO_TECH_EVDO_A;
        case ims_RadioTechType_RADIO_TECH_HSDPA:
            return RadioTechType::RADIO_TECH_HSDPA;
        case ims_RadioTechType_RADIO_TECH_HSUPA:
            return RadioTechType::RADIO_TECH_HSUPA;
        case ims_RadioTechType_RADIO_TECH_HSPA:
            return RadioTechType::RADIO_TECH_HSPA;
        case ims_RadioTechType_RADIO_TECH_EVDO_B:
            return RadioTechType::RADIO_TECH_EVDO_B;
        case ims_RadioTechType_RADIO_TECH_EHRPD:
            return RadioTechType::RADIO_TECH_EHRPD;
        case ims_RadioTechType_RADIO_TECH_LTE:
            return RadioTechType::RADIO_TECH_LTE;
        case ims_RadioTechType_RADIO_TECH_HSPAP:
            return RadioTechType::RADIO_TECH_HSPAP;
        case ims_RadioTechType_RADIO_TECH_GSM:
            return RadioTechType::RADIO_TECH_GSM;
        case ims_RadioTechType_RADIO_TECH_TD_SCDMA:
            return RadioTechType::RADIO_TECH_TD_SCDMA;
        case ims_RadioTechType_RADIO_TECH_WIFI:
            return RadioTechType::RADIO_TECH_WIFI;
        case ims_RadioTechType_RADIO_TECH_IWLAN:
            return RadioTechType::RADIO_TECH_IWLAN;
        default:
            return RadioTechType::RADIO_TECH_INVALID;
    }
}

ims_RadioTechType convertHidlToProtoRadioTech(const RadioTechType& radioTechType) {
    switch (radioTechType) {
        case RadioTechType::RADIO_TECH_ANY:
            return ims_RadioTechType_RADIO_TECH_ANY;
        case RadioTechType::RADIO_TECH_UNKNOWN:
            return ims_RadioTechType_RADIO_TECH_UNKNOWN;
        case RadioTechType::RADIO_TECH_GPRS:
            return ims_RadioTechType_RADIO_TECH_GPRS;
        case RadioTechType::RADIO_TECH_EDGE:
            return ims_RadioTechType_RADIO_TECH_EDGE;
        case RadioTechType::RADIO_TECH_UMTS:
            return ims_RadioTechType_RADIO_TECH_UMTS;
        case RadioTechType::RADIO_TECH_IS95A:
            return ims_RadioTechType_RADIO_TECH_IS95A;
        case RadioTechType::RADIO_TECH_IS95B:
            return ims_RadioTechType_RADIO_TECH_IS95B;
        case RadioTechType::RADIO_TECH_1xRTT:
            return ims_RadioTechType_RADIO_TECH_1xRTT;
        case RadioTechType::RADIO_TECH_EVDO_0:
            return ims_RadioTechType_RADIO_TECH_EVDO_0;
        case RadioTechType::RADIO_TECH_EVDO_A:
            return ims_RadioTechType_RADIO_TECH_EVDO_A;
        case RadioTechType::RADIO_TECH_HSDPA:
            return ims_RadioTechType_RADIO_TECH_HSDPA;
        case RadioTechType::RADIO_TECH_HSUPA:
            return ims_RadioTechType_RADIO_TECH_HSUPA;
        case RadioTechType::RADIO_TECH_HSPA:
            return ims_RadioTechType_RADIO_TECH_HSPA;
        case RadioTechType::RADIO_TECH_EVDO_B:
            return ims_RadioTechType_RADIO_TECH_EVDO_B;
        case RadioTechType::RADIO_TECH_EHRPD:
            return ims_RadioTechType_RADIO_TECH_EHRPD;
        case RadioTechType::RADIO_TECH_LTE:
            return ims_RadioTechType_RADIO_TECH_LTE;
        case RadioTechType::RADIO_TECH_HSPAP:
            return ims_RadioTechType_RADIO_TECH_HSPAP;
        case RadioTechType::RADIO_TECH_GSM:
            return ims_RadioTechType_RADIO_TECH_GSM;
        case RadioTechType::RADIO_TECH_TD_SCDMA:
            return ims_RadioTechType_RADIO_TECH_TD_SCDMA;
        case RadioTechType::RADIO_TECH_WIFI:
            return ims_RadioTechType_RADIO_TECH_WIFI;
        case RadioTechType::RADIO_TECH_IWLAN:
            return ims_RadioTechType_RADIO_TECH_IWLAN;
        default:
            return ims_RadioTechType_RADIO_TECH_UNKNOWN;
    }
}

ims_CallType convertHidlToProtoCallType(const CallType& callType) {
    switch (callType) {
        case CallType::CALL_TYPE_VOICE:
            return ims_CallType_CALL_TYPE_VOICE;
        case CallType::CALL_TYPE_VT_TX:
            return ims_CallType_CALL_TYPE_VT_TX;
        case CallType::CALL_TYPE_VT_RX:
            return ims_CallType_CALL_TYPE_VT_RX;
        case CallType::CALL_TYPE_VT:
            return ims_CallType_CALL_TYPE_VT;
        case CallType::CALL_TYPE_VT_NODIR:
            return ims_CallType_CALL_TYPE_VT_NODIR;
        case CallType::CALL_TYPE_CS_VS_TX:
            return ims_CallType_CALL_TYPE_CS_VS_TX;
        case CallType::CALL_TYPE_CS_VS_RX:
            return ims_CallType_CALL_TYPE_CS_VS_RX;
        case CallType::CALL_TYPE_PS_VS_TX:
            return ims_CallType_CALL_TYPE_PS_VS_TX;
        case CallType::CALL_TYPE_PS_VS_RX:
            return ims_CallType_CALL_TYPE_PS_VS_RX;
        case CallType::CALL_TYPE_SMS:
            return ims_CallType_CALL_TYPE_SMS;
        case CallType::CALL_TYPE_UT:
            return ims_CallType_CALL_TYPE_UT;
        case CallType::CALL_TYPE_UNKNOWN:
        default:
            return ims_CallType_CALL_TYPE_UNKNOWN;
    }
}

ims_IpPresentation convertHidlToProtoPresentation(const IpPresentation& inPresentation) {
    switch (inPresentation) {
        case IpPresentation::IP_PRESENTATION_NUM_ALLOWED:
            return ims_IpPresentation_IP_PRESENTATION_NUM_ALLOWED;
        case IpPresentation::IP_PRESENTATION_NUM_RESTRICTED:
            return ims_IpPresentation_IP_PRESENTATION_NUM_RESTRICTED;
        case IpPresentation::IP_PRESENTATION_NUM_DEFAULT:
        default:
            return ims_IpPresentation_IP_PRESENTATION_NUM_DEFAULT;
    }
}

ims_RttMode convertHidlToProtoRttMode(const RttMode& inRttMode){
    switch (inRttMode) {
        case RttMode::RTT_MODE_DISABLED:
            return ims_RttMode_RTT_MODE_DISABLED;
        case RttMode::RTT_MODE_FULL:
            return ims_RttMode_RTT_MODE_FULL;
        default:
            return ims_RttMode_RTT_MODE_DISABLED;
    }
}

RttMode convertProtoToHidlRttMode(const ims_RttMode& inRttMode) {
    switch (inRttMode) {
        case ims_RttMode_RTT_MODE_DISABLED:
            return RttMode::RTT_MODE_DISABLED;
        case ims_RttMode_RTT_MODE_FULL:
            return RttMode::RTT_MODE_FULL;
        default:
            return RttMode::RTT_MODE_INVALID;
    }
}

IpPresentation convertProtoToHidlPresentation(ims_IpPresentation presentation) {
    switch (presentation) {
        case ims_IpPresentation_IP_PRESENTATION_NUM_ALLOWED:
            return IpPresentation::IP_PRESENTATION_NUM_ALLOWED;
        case ims_IpPresentation_IP_PRESENTATION_NUM_RESTRICTED:
            return IpPresentation::IP_PRESENTATION_NUM_RESTRICTED;
        case ims_IpPresentation_IP_PRESENTATION_NUM_DEFAULT:
            return IpPresentation::IP_PRESENTATION_NUM_DEFAULT;
        default:
            return IpPresentation::IP_PRESENTATION_INVALID;
    }
}

ims_Error convertProtoToHidlSipErrorInfo(const ims_SipErrorInfo& inErrorInfo,
        SipErrorInfo& outErrorInfo) {

    if (inErrorInfo.has_sipErrorCode) {
        outErrorInfo.errorCode = inErrorInfo.sipErrorCode;
    }

    const char* ptr = (const char*)inErrorInfo.sipErrorString.arg;
    if (ptr == nullptr) {
        return ims_Error_E_GENERIC_FAILURE;
    }
    outErrorInfo.errorString.setToExternal(ptr, strlen(ptr));
    return ims_Error_E_SUCCESS;
}

ims_Error convertHidlToProtoHangup(const V1_3::HangupRequestInfo& hangup, ims_Hangup& ims_hangup) {
    if (hangup.connIndex != INT32_MAX) {
        ims_hangup.has_conn_index = true;
        ims_hangup.conn_index = hangup.connIndex;
    }
    if (hangup.hasMultiParty) {
        ims_hangup.has_multi_party = true;
        ims_hangup.multi_party = hangup.multiParty;
    }

    const uint32_t connUriLen = hangup.connUri.size();
    if (connUriLen > 0) {
        size_t destSize = sizeof(char) * (connUriLen + 1);
        ims_hangup.conn_uri.arg = qcril_malloc(destSize);

        if (ims_hangup.conn_uri.arg == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory for connection URI.");
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)ims_hangup.conn_uri.arg, hangup.connUri.c_str(), destSize);
    }
    if (hangup.hasFailCauseResponse) {
        if (convertHidlToProtoCallFailCauseResponse(hangup.failCauseResponse,
                ims_hangup.failCauseResponse) != ims_Error_E_SUCCESS) {
            if (ims_hangup.conn_uri.arg != nullptr) {
                qcril_free(ims_hangup.conn_uri.arg);
                ims_hangup.conn_uri.arg = nullptr;
            }
            return ims_Error_E_GENERIC_FAILURE;
        }
        ims_hangup.has_failCauseResponse = true;
    }

    return ims_Error_E_SUCCESS;
}

ims_Error convertHidlToProtoCallFailCauseResponse(const V1_3::CallFailCauseResponse& inFailCause,
        ims_CallFailCauseResponse& outFailCause) {
    qcril_binary_data_type* ptr = nullptr;
    ims_Error ret = ims_Error_E_SUCCESS;

    do {
        if (inFailCause.failCause != V1_3::CallFailCause::CALL_FAIL_INVALID) {
            outFailCause.has_failcause = true;
            outFailCause.failcause = convertHidlToProtoCallFailCause(inFailCause.failCause);
        }
        const uint32_t errorInfoLen = inFailCause.errorinfo.size();
        if (errorInfoLen > 0) {
            outFailCause.errorinfo.arg = qcril_malloc(sizeof(qcril_binary_data_type));

            if (outFailCause.errorinfo.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for error infg.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            ptr = (qcril_binary_data_type*)outFailCause.errorinfo.arg;
            ptr->len = errorInfoLen;
            ptr->data = (uint8_t*)qcril_malloc(errorInfoLen);

            if (ptr->data == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for error infg.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            memcpy(ptr->data, inFailCause.errorinfo.data(), errorInfoLen);
        }
        const uint32_t errorStringLen = inFailCause.networkErrorString.size();
        if (errorStringLen > 0) {
            size_t destSize = sizeof(char) * (errorStringLen + 1);
            outFailCause.networkErrorString.arg = qcril_malloc(destSize);

            if (outFailCause.networkErrorString.arg == nullptr) {
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            strlcpy((char*)outFailCause.networkErrorString.arg,
                    inFailCause.networkErrorString.c_str(), destSize);
        }
        if (inFailCause.hasErrorDetails) {
            outFailCause.has_errorDetails = true;
            ret = convertHidlToProtoSipErrorInfo(inFailCause.errorDetails,
                    outFailCause.errorDetails);
        }
    } while(0);

    if (isError(ret)) {
        if (ptr != nullptr) {
            if (ptr->data != nullptr) {
                qcril_free(ptr->data);
                ptr->data = nullptr;
            }
            qcril_free(ptr);
            ptr = nullptr;
        }

        if (outFailCause.networkErrorString.arg != nullptr) {
            qcril_free(outFailCause.networkErrorString.arg);
            outFailCause.networkErrorString.arg = nullptr;
        }
    }

    return ret;
}

ims_Error convertHidlToProtoSipErrorInfo(const SipErrorInfo& inErrorInfo,
        ims_SipErrorInfo& outErrorInfo) {

    if (inErrorInfo.errorCode != INT32_MAX) {
        outErrorInfo.has_sipErrorCode = TRUE;
        outErrorInfo.sipErrorCode = inErrorInfo.errorCode;
    }
    const uint32_t errorStrLen = inErrorInfo.errorString.size();
    if (errorStrLen > 0) {
        size_t destSize = sizeof(char) * (errorStrLen + 1);
        outErrorInfo.sipErrorString.arg = qcril_malloc(destSize);

        if (outErrorInfo.sipErrorString.arg == nullptr) {
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)outErrorInfo.sipErrorString.arg, inErrorInfo.errorString.c_str(), destSize);
    }
    return ims_Error_E_SUCCESS;
}

ims_Error convertHidlToProtoServiceStatusInfo(const ServiceStatusInfo& inServiceInfo,
        ims_Info& outServiceInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    do {
        if (inServiceInfo.hasIsValid) {
            outServiceInfo.has_isValid = true;
            outServiceInfo.isValid = inServiceInfo.isValid;
        }
        if (inServiceInfo.type != ServiceType::INVALID) {
            outServiceInfo.has_type = true;
            outServiceInfo.type = convertHidlToProtoServiceType(inServiceInfo.type);
        }
        if (inServiceInfo.callType != CallType::CALL_TYPE_INVALID) {
            outServiceInfo.has_callType = true;
            outServiceInfo.callType = convertHidlToProtoCallType(inServiceInfo.callType);
        }
        if (inServiceInfo.status != StatusType::STATUS_INVALID) {
            outServiceInfo.has_status = true;
            outServiceInfo.status = convertHidlToProtoStatusType(inServiceInfo.status);
        }
        const uint32_t userdataLen = inServiceInfo.userdata.size();
        if (userdataLen > 0) {
            outServiceInfo.userdata.arg = qcril_malloc(sizeof(qcril_binary_data_type));

            if (outServiceInfo.userdata.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for userdata.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            qcril_binary_data_type* ptr = (qcril_binary_data_type*)outServiceInfo.userdata.arg;
            ptr->len = userdataLen;
            ptr->data = (uint8_t*)qcril_malloc(userdataLen);

            if (ptr->data == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for userdata.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            memcpy(ptr->data, inServiceInfo.userdata.data(), userdataLen);
        }
        if (inServiceInfo.restrictCause != INT32_MAX) {
            outServiceInfo.has_restrictCause = true;
            outServiceInfo.restrictCause = inServiceInfo.restrictCause;
        }
        const uint32_t accTechStatusLen = inServiceInfo.accTechStatus.size();
        if (accTechStatusLen > 0) {
            outServiceInfo.accTechStatus.arg = qcril_malloc(sizeof(ims_StatusForAccessTech*) *
                    (accTechStatusLen + 1));

            if (outServiceInfo.accTechStatus.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for access tech status.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for(uint32_t i = 0; i < accTechStatusLen; ++i) {
                ((ims_StatusForAccessTech**)outServiceInfo.accTechStatus.arg)[i] =
                        (ims_StatusForAccessTech*)qcril_malloc(sizeof(ims_StatusForAccessTech));

                if (((ims_StatusForAccessTech**)(outServiceInfo.accTechStatus.arg))[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for status access tech status.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }

                ims_StatusForAccessTech* ptr =
                        ((ims_StatusForAccessTech**)outServiceInfo.accTechStatus.arg)[i];
                if ((ret = convertHidlToProtoStatusForAccessTech(inServiceInfo.accTechStatus[i],
                        *ptr)) != ims_Error_E_SUCCESS) {
                    break;
                }
            }

            if(inServiceInfo.rttMode != RttMode::RTT_MODE_INVALID){
                outServiceInfo.has_rttMode = true;
                outServiceInfo.rttMode = convertHidlToProtoRttMode(inServiceInfo.rttMode);
            }
        }
    } while(0);

    if (isError(ret)) {
        if (outServiceInfo.userdata.arg != nullptr) {
            qcril_binary_data_type* ptr = (qcril_binary_data_type*)outServiceInfo.userdata.arg;

            qcril_free(ptr->data);
            ptr->data = nullptr;

            qcril_free(ptr);
            ptr = nullptr;
        }

        if (outServiceInfo.accTechStatus.arg != nullptr) {
            StatusForAccessTech** ptr = (StatusForAccessTech**)outServiceInfo.accTechStatus.arg;

            while(*ptr) {
                qcril_free(*ptr);
                *ptr = nullptr;
                ptr++;
            }

            qcril_free(outServiceInfo.accTechStatus.arg);
            outServiceInfo.accTechStatus.arg = nullptr;
        }
    }

    return ret;
}

ims_Error convertHidlToProtoStatusForAccessTech(const StatusForAccessTech& inAccTechStatus,
        ims_StatusForAccessTech& outAccTechStatus) {
    ims_Error ret = ims_Error_E_SUCCESS;
    if (inAccTechStatus.networkMode != RadioTechType::RADIO_TECH_INVALID) {
        outAccTechStatus.has_networkMode = true;
        outAccTechStatus.networkMode = convertHidlToProtoRadioTech(inAccTechStatus.networkMode);
    }
    if (inAccTechStatus.status != StatusType::STATUS_INVALID) {
        outAccTechStatus.has_status = true;
        outAccTechStatus.status = convertHidlToProtoStatusType(inAccTechStatus.status);
    }
    if (inAccTechStatus.restrictCause != INT32_MAX) {
        outAccTechStatus.has_restrictionCause = true;
        outAccTechStatus.restrictionCause = inAccTechStatus.restrictCause;
    }
    if (inAccTechStatus.hasRegistration) {
        if ((ret = convertHidlToProtoRegistrationInfo(inAccTechStatus.registration,
                outAccTechStatus.registered)) != ims_Error_E_SUCCESS) {
            return ret;
        }
        outAccTechStatus.has_registered = true;
    }

    return ret;
}

ims_Error convertHidlToProtoRegistrationInfo(const RegistrationInfo& inRegistration,
        ims_Registration& outRegistration) {
    if (inRegistration.state != RegState::INVALID) {
        outRegistration.has_state = true;
        outRegistration.state = convertHidlToProtoRegState(inRegistration.state);
    }
    if (inRegistration.errorCode !=  INT32_MAX) {
        outRegistration.has_errorCode = true;
        outRegistration.errorCode = inRegistration.errorCode;
    }
    const uint32_t errorMessageLen = inRegistration.errorMessage.size();
    if (errorMessageLen > 0) {
        size_t destSize = sizeof(char) * (errorMessageLen + 1);
        outRegistration.errorMessage.arg = qcril_malloc(destSize);

        if (outRegistration.errorMessage.arg == nullptr) {
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)outRegistration.errorMessage.arg, inRegistration.errorMessage.c_str(),
                destSize);
    }
    if (inRegistration.radioTech != RadioTechType::RADIO_TECH_INVALID) {
        outRegistration.has_radioTech = true;
        outRegistration.radioTech = convertHidlToProtoRadioTech(inRegistration.radioTech);
    }
    const uint32_t pAssociatedUrisLen = inRegistration.pAssociatedUris.size();
    if (pAssociatedUrisLen > 0) {
        size_t destSize = sizeof(char) * (pAssociatedUrisLen + 1);
        outRegistration.pAssociatedUris.arg = qcril_malloc(destSize);

        if (outRegistration.pAssociatedUris.arg == nullptr) {
            if (outRegistration.errorMessage.arg != nullptr) {
                qcril_free(outRegistration.errorMessage.arg);
                outRegistration.errorMessage.arg = nullptr;
            }
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy((char*)outRegistration.pAssociatedUris.arg, inRegistration.pAssociatedUris.c_str(),
                destSize);
    }

    return ims_Error_E_SUCCESS;
}

ims_StatusType convertHidlToProtoStatusType(const StatusType& status) {
    switch (status) {
        case StatusType::STATUS_DISABLED:
            return ims_StatusType_STATUS_DISABLED;
        case StatusType::STATUS_PARTIALLY_ENABLED:
            return ims_StatusType_STATUS_PARTIALLY_ENABLED;
        case StatusType::STATUS_ENABLED:
            return ims_StatusType_STATUS_ENABLED;
        case StatusType::STATUS_NOT_SUPPORTED:
            return ims_StatusType_STATUS_NOT_SUPPORTED;
        default:
            return ims_StatusType_STATUS_DISABLED;
    }
}

ims_SrvType convertHidlToProtoServiceType(const ServiceType& type) {
    switch (type) {
        case ServiceType::SMS:
            return ims_SrvType_SMS;
        case ServiceType::VOIP:
            return ims_SrvType_VOIP;
        case ServiceType::VT:
            return ims_SrvType_VT;
        default:
            return ims_SrvType_VOIP;
    }
}

ims_Error convertHidlToProtoServiceStatusInfoList(const hidl_vec<ServiceStatusInfo>& inList,
        ims_Info** outList) {
    ims_Error ret = ims_Error_E_SUCCESS;
    uint32_t inListLen = inList.size();
    do {
        if (inListLen > 0) {
            outList = (ims_Info**)qcril_malloc((inListLen + 1) * sizeof(ims_Info*));

            if (outList == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for service status info list.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for (uint32_t i = 0; i < inListLen; ++i) {
                outList[i] = (ims_Info*)qcril_malloc(sizeof(ims_Info));

                if (outList[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for service status info.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }

                if ((ret = convertHidlToProtoServiceStatusInfo(inList[i], *(outList[i]))) !=
                        ims_Error_E_SUCCESS) {
                    break;
                }
            }
        }
    } while(0);

    if (isError(ret)) {
        if (outList != nullptr) {
            while (*outList) {
                qcril_free(*outList);
                *outList = nullptr;
                outList++;
            }
            qcril_free(outList);
            outList = nullptr;
        }
    }

    return ret;
}

ims_Error convertHidlToProtoCallForwardInfo(const CallForwardInfo& inCallForwardInfo,
        ims_CallForwardInfoList_CallForwardInfo& outCallForwardInfo) {

    if (convertHidlToProtoString(inCallForwardInfo.number, (char*&)(outCallForwardInfo.number.arg))
            != ims_Error_E_SUCCESS) {
        return ims_Error_E_GENERIC_FAILURE;
    }

    if (inCallForwardInfo.status != INT32_MAX) {
        outCallForwardInfo.has_status = true;
        outCallForwardInfo.status = inCallForwardInfo.status;
    }

    if (inCallForwardInfo.reason != INT32_MAX) {
        outCallForwardInfo.has_reason = true;
        outCallForwardInfo.reason = inCallForwardInfo.reason;
    }

    if (inCallForwardInfo.serviceClass != INT32_MAX) {
        outCallForwardInfo.has_service_class = true;
        outCallForwardInfo.service_class = inCallForwardInfo.serviceClass;
    }

    if (inCallForwardInfo.toa != INT32_MAX) {
        outCallForwardInfo.has_toa = true;
        outCallForwardInfo.toa = inCallForwardInfo.toa;
    }

    if (inCallForwardInfo.timeSeconds != INT32_MAX) {
        outCallForwardInfo.has_time_seconds = true;
        outCallForwardInfo.time_seconds = inCallForwardInfo.timeSeconds;
    }

    if (inCallForwardInfo.hasCallFwdTimerStart) {
        outCallForwardInfo.has_callFwdTimerStart = true;
        convertHidlToProtoCallFwdTimerInfo(inCallForwardInfo.callFwdTimerStart,
                outCallForwardInfo.callFwdTimerStart);
    }

    if (inCallForwardInfo.hasCallFwdTimerEnd) {
        outCallForwardInfo.has_callFwdTimerEnd = true;
        convertHidlToProtoCallFwdTimerInfo(inCallForwardInfo.callFwdTimerEnd,
                outCallForwardInfo.callFwdTimerEnd);
    }

    return ims_Error_E_SUCCESS;
}

void convertHidlToProtoCallFwdTimerInfo(const CallFwdTimerInfo& inCallFwdTimerInfo,
         ims_CallFwdTimerInfo& outCallFwdTimerInfo) {
    if (inCallFwdTimerInfo.year != INT32_MAX) {
         outCallFwdTimerInfo.has_year = true;
         outCallFwdTimerInfo.year = inCallFwdTimerInfo.year;
    }

    if (inCallFwdTimerInfo.month != INT32_MAX) {
         outCallFwdTimerInfo.has_month = true;
         outCallFwdTimerInfo.month = inCallFwdTimerInfo.month;
    }

    if (inCallFwdTimerInfo.day != INT32_MAX) {
         outCallFwdTimerInfo.has_day = true;
         outCallFwdTimerInfo.day = inCallFwdTimerInfo.day;
    }

    if (inCallFwdTimerInfo.hour != INT32_MAX) {
         outCallFwdTimerInfo.has_hour = true;
         outCallFwdTimerInfo.hour = inCallFwdTimerInfo.hour;
    }

    if (inCallFwdTimerInfo.minute != INT32_MAX) {
         outCallFwdTimerInfo.has_minute = true;
         outCallFwdTimerInfo.minute = inCallFwdTimerInfo.minute;
    }

    if (inCallFwdTimerInfo.second != INT32_MAX) {
         outCallFwdTimerInfo.has_second = true;
         outCallFwdTimerInfo.second = inCallFwdTimerInfo.second;
    }

    if (inCallFwdTimerInfo.timezone != INT32_MAX) {
         outCallFwdTimerInfo.has_timezone = true;
         outCallFwdTimerInfo.timezone = inCallFwdTimerInfo.timezone;
    }
}

ims_Error convertHidlToProtoString(const hidl_string& inStr, char*& outStr) {
    if (!inStr.empty()) {
        size_t destSize = sizeof(char) * (inStr.size() + 1);
        outStr = (char*)qcril_malloc(destSize);

        if (outStr == NULL) {
            QCRIL_LOG_ERROR("Failed to allocate memory to outStr.");
            return ims_Error_E_GENERIC_FAILURE;
        }

        strlcpy(outStr, inStr.c_str(), destSize);
    }

    return ims_Error_E_SUCCESS;
}

ims_Error convertHidlToProtoNumListInfo(const CbNumListInfo& inCbNumListInfo,
        ims_CbNumListType& outCbNumListInfo) {
    if (inCbNumListInfo.serviceClass != INT32_MAX) {
        outCbNumListInfo.has_serviceClass = true;
        outCbNumListInfo.serviceClass.has_service_Class = true;
        outCbNumListInfo.serviceClass.service_Class = inCbNumListInfo.serviceClass;
    }

    ims_Error ret = ims_Error_E_SUCCESS;
    uint32_t inListLen = inCbNumListInfo.cbNumInfo.size();
    ims_CbNumList** outList = nullptr;
    outCbNumListInfo.cb_num_list.arg = nullptr;
    do {
        if (inListLen > 0) {
            outList = (ims_CbNumList**)qcril_malloc((inListLen + 1) * sizeof(ims_CbNumList*));

            if (outList == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for service status info list.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            for(uint32_t i = 0; i < inListLen; ++i) {
                outList[i] = (ims_CbNumList*)qcril_malloc(sizeof(ims_CbNumList));

                if (outList[i] == nullptr) {
                    QCRIL_LOG_ERROR("Failed to allocate memory for service status info.");
                    ret = ims_Error_E_GENERIC_FAILURE;
                    break;
                }

                if ((ret = convertHidlToProtoNumInfo(inCbNumListInfo.cbNumInfo[i],
                        *(outList[i]))) != ims_Error_E_SUCCESS) {
                    break;
                }
            }
        }
        outCbNumListInfo.cb_num_list.arg = (void *)outList;
    } while(0);

    if (isError(ret)) {
        if (outList != nullptr) {
            while(*outList) {
                qcril_free(*outList);
                *outList = nullptr;
                outList++;
            }
            qcril_free(outList);
            outList = nullptr;
        }
    }

    return ret;
}

ims_Error convertHidlToProtoNumInfo(const CbNumInfo& inCbNumInfo, ims_CbNumList& outCbNumInfo) {
    if (inCbNumInfo.status != ServiceClassStatus::INVALID) {
        outCbNumInfo.has_status = true;
        outCbNumInfo.status = convertHidlToProtoServiceClassStatus(inCbNumInfo.status);
    }

    return convertHidlToProtoString(inCbNumInfo.number, (char*&)(outCbNumInfo.number.arg));

}

ims_CallDomain convertHidlToProtoCallDomain(const CallDomain& inCallDomain) {
    switch (inCallDomain) {
        case CallDomain::CALL_DOMAIN_UNKNOWN:
            return ims_CallDomain_CALL_DOMAIN_UNKNOWN;
        case CallDomain::CALL_DOMAIN_CS:
            return ims_CallDomain_CALL_DOMAIN_CS;
        case CallDomain::CALL_DOMAIN_PS:
            return ims_CallDomain_CALL_DOMAIN_PS;
        case CallDomain::CALL_DOMAIN_AUTOMATIC:
            return ims_CallDomain_CALL_DOMAIN_AUTOMATIC;
        case CallDomain::CALL_DOMAIN_NOT_SET:
        default:
            return ims_CallDomain_CALL_DOMAIN_NOT_SET;
    }
}

ims_CallFailCause convertHidlToProtoCallFailCause(const V1_3::CallFailCause& failCause) {
    switch(failCause) {
        case V1_3::CallFailCause::CALL_FAIL_SRV_NOT_REGISTERED:
            return ims_CallFailCause_CALL_FAIL_SRV_NOT_REGISTERED;
        case V1_3::CallFailCause::CALL_FAIL_CALL_TYPE_NOT_ALLOWED:
            return ims_CallFailCause_CALL_FAIL_CALL_TYPE_NOT_ALLOWED;
        case V1_3::CallFailCause::CALL_FAIL_EMRG_CALL_ONGOING:
            return ims_CallFailCause_CALL_FAIL_EMRG_CALL_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_CALL_SETUP_ONGOING:
            return ims_CallFailCause_CALL_FAIL_CALL_SETUP_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_MAX_CALL_LIMIT_REACHED:
            return ims_CallFailCause_CALL_FAIL_MAX_CALL_LIMIT_REACHED;
        case V1_3::CallFailCause::CALL_FAIL_UNSUPPORTED_SIP_HDRS:
            return ims_CallFailCause_CALL_FAIL_UNSUPPORTED_SIP_HDRS;
        case V1_3::CallFailCause::CALL_FAIL_CALL_TRANSFER_ONGOING:
            return ims_CallFailCause_CALL_FAIL_CALL_TRANSFER_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_PRACK_TIMEOUT:
            return ims_CallFailCause_CALL_FAIL_PRACK_TIMEOUT;
        case V1_3::CallFailCause::CALL_FAIL_QOS_FAILURE:
            return ims_CallFailCause_CALL_FAIL_QOS_FAILURE;
        case V1_3::CallFailCause::CALL_FAIL_ONGOING_HANDOVER:
            return ims_CallFailCause_CALL_FAIL_ONGOING_HANDOVER;
        case V1_3::CallFailCause::CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED:
            return ims_CallFailCause_CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED;
        case V1_3::CallFailCause::CALL_FAIL_CALL_UPGRADE_ONGOING:
            return ims_CallFailCause_CALL_FAIL_CALL_UPGRADE_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED:
            return ims_CallFailCause_CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED;
        case V1_3::CallFailCause::CALL_FAIL_CALL_CONFERENCE_ONGOING:
            return ims_CallFailCause_CALL_FAIL_CALL_CONFERENCE_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED:
            return ims_CallFailCause_CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED;
        case V1_3::CallFailCause::CALL_FAIL_ENCRYPTION_CALL_ONGOING:
            return ims_CallFailCause_CALL_FAIL_ENCRYPTION_CALL_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_CALL_ONGOING_CW_DISABLED:
            return ims_CallFailCause_CALL_FAIL_CALL_ONGOING_CW_DISABLED;
        case V1_3::CallFailCause::CALL_FAIL_CALL_ON_OTHER_SUB:
            return ims_CallFailCause_CALL_FAIL_CALL_ON_OTHER_SUB;
        case V1_3::CallFailCause::CALL_FAIL_1X_COLLISION:
            return ims_CallFailCause_CALL_FAIL_1X_COLLISION;
        case V1_3::CallFailCause::CALL_FAIL_UI_NOT_READY:
            return ims_CallFailCause_CALL_FAIL_UI_NOT_READY;
        case V1_3::CallFailCause::CALL_FAIL_CS_CALL_ONGOING:
            return ims_CallFailCause_CALL_FAIL_CS_CALL_ONGOING;
        case V1_3::CallFailCause::CALL_FAIL_SIP_USER_MARKED_UNWANTED:
            return ims_CallFailCause_CALL_FAIL_SIP_USER_MARKED_UNWANTED;
        default:
            return convertHidlToProtoCallFailCauseV1_2((V1_2::CallFailCause)failCause);
    }
}

ims_CallFailCause convertHidlToProtoCallFailCauseV1_2(const V1_2::CallFailCause& failCause) {
    switch (failCause) {
        case V1_2::CallFailCause::CALL_FAIL_RADIO_OFF:
            return ims_CallFailCause_CALL_FAIL_RADIO_OFF;
        case V1_2::CallFailCause::CALL_FAIL_OUT_OF_SERVICE:
            return ims_CallFailCause_CALL_FAIL_OUT_OF_SERVICE;
        case V1_2::CallFailCause::CALL_FAIL_NO_VALID_SIM:
            return ims_CallFailCause_CALL_FAIL_NO_VALID_SIM;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_INTERNAL_ERROR:
            return ims_CallFailCause_CALL_FAIL_RADIO_INTERNAL_ERROR;
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_RESP_TIMEOUT:
            return ims_CallFailCause_CALL_FAIL_NETWORK_RESP_TIMEOUT;
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_REJECT:
            return ims_CallFailCause_CALL_FAIL_NETWORK_REJECT;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_ACCESS_FAILURE:
            return ims_CallFailCause_CALL_FAIL_RADIO_ACCESS_FAILURE;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_LINK_FAILURE:
            return ims_CallFailCause_CALL_FAIL_RADIO_LINK_FAILURE;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_LINK_LOST:
            return ims_CallFailCause_CALL_FAIL_RADIO_LINK_LOST;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_UPLINK_FAILURE:
            return ims_CallFailCause_CALL_FAIL_RADIO_UPLINK_FAILURE;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_SETUP_FAILURE:
            return ims_CallFailCause_CALL_FAIL_RADIO_SETUP_FAILURE;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_RELEASE_NORMAL:
            return ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_NORMAL;
        case V1_2::CallFailCause::CALL_FAIL_RADIO_RELEASE_ABNORMAL:
            return ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_ABNORMAL;
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_DETACH:
            return ims_CallFailCause_CALL_FAIL_NETWORK_DETACH;
        case V1_2::CallFailCause::CALL_FAIL_NO_CSFB_IN_CS_ROAM:
            return ims_CallFailCause_CALL_FAIL_NO_CSFB_IN_CS_ROAM;
        default:
            return convertHidlToProtoCallFailCauseV1_1((V1_1::CallFailCause)failCause);
    }
}

ims_CallFailCause convertHidlToProtoCallFailCauseV1_1(const V1_1::CallFailCause& failCause) {
    switch (failCause) {
        case V1_1::CallFailCause::CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL:
            return ims_CallFailCause_CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL;
        default:
            return convertHidlToProtoCallFailCauseV1_0((V1_0::CallFailCause)failCause);
    }
}

ims_CallFailCause convertHidlToProtoCallFailCauseV1_0(const CallFailCause& failCause) {
    switch (failCause) {
        case CallFailCause::CALL_FAIL_UNOBTAINABLE_NUMBER:
            return ims_CallFailCause_CALL_FAIL_UNOBTAINABLE_NUMBER;
        case CallFailCause::CALL_FAIL_NORMAL:
            return ims_CallFailCause_CALL_FAIL_NORMAL;
        case CallFailCause::CALL_FAIL_BUSY:
            return ims_CallFailCause_CALL_FAIL_BUSY;
        case CallFailCause::CALL_FAIL_NORMAL_UNSPECIFIED:
            return ims_CallFailCause_CALL_FAIL_NORMAL_UNSPECIFIED;
        case CallFailCause::CALL_FAIL_CONGESTION:
            return ims_CallFailCause_CALL_FAIL_CONGESTION;
        case CallFailCause::CALL_FAIL_INCOMPATIBILITY_DESTINATION:
            return ims_CallFailCause_CALL_FAIL_INCOMPATIBILITY_DESTINATION;
        case CallFailCause::CALL_FAIL_CALL_BARRED:
            return ims_CallFailCause_CALL_FAIL_CALL_BARRED;
        case CallFailCause::CALL_FAIL_FDN_BLOCKED:
            return ims_CallFailCause_CALL_FAIL_FDN_BLOCKED;
        case CallFailCause::CALL_FAIL_IMEI_NOT_ACCEPTED:
            return ims_CallFailCause_CALL_FAIL_IMEI_NOT_ACCEPTED;
        case CallFailCause::CALL_FAIL_DIAL_MODIFIED_TO_USSD:
            return ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_USSD;
        case CallFailCause::CALL_FAIL_DIAL_MODIFIED_TO_SS:
            return ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_SS;
        case CallFailCause::CALL_FAIL_DIAL_MODIFIED_TO_DIAL:
            return ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL;
        case CallFailCause::CALL_FAIL_EMERGENCY_TEMP_FAILURE:
            return ims_CallFailCause_CALL_FAIL_EMERGENCY_TEMP_FAILURE;
        case CallFailCause::CALL_FAIL_EMERGENCY_PERM_FAILURE:
            return ims_CallFailCause_CALL_FAIL_EMERGENCY_PERM_FAILURE;
        case CallFailCause::CALL_FAIL_HO_NOT_FEASIBLE:
            return ims_CallFailCause_CALL_FAIL_HO_NOT_FEASIBLE;
        case CallFailCause::CALL_FAIL_USER_BUSY:
            return ims_CallFailCause_CALL_FAIL_USER_BUSY;
        case CallFailCause::CALL_FAIL_USER_REJECT:
            return ims_CallFailCause_CALL_FAIL_USER_REJECT;
        case CallFailCause::CALL_FAIL_LOW_BATTERY:
            return ims_CallFailCause_CALL_FAIL_LOW_BATTERY;
        case CallFailCause::CALL_FAIL_BLACKLISTED_CALL_ID:
            return ims_CallFailCause_CALL_FAIL_BLACKLISTED_CALL_ID;
        case CallFailCause::CALL_FAIL_CS_RETRY_REQUIRED:
            return ims_CallFailCause_CALL_FAIL_CS_RETRY_REQUIRED;
        case CallFailCause::CALL_FAIL_NETWORK_UNAVAILABLE:
            return ims_CallFailCause_CALL_FAIL_NETWORK_UNAVAILABLE;
        case CallFailCause::CALL_FAIL_FEATURE_UNAVAILABLE:
            return ims_CallFailCause_CALL_FAIL_FEATURE_UNAVAILABLE;
        case CallFailCause::CALL_FAIL_SIP_ERROR:
            return ims_CallFailCause_CALL_FAIL_SIP_Error;
        case CallFailCause::CALL_FAIL_MISC:
            return ims_CallFailCause_CALL_FAIL_MISC;
        case CallFailCause::CALL_FAIL_ANSWERED_ELSEWHERE:
            return ims_CallFailCause_CALL_FAIL_ANSWERED_ELSEWHERE;
        case CallFailCause::CALL_FAIL_PULL_OUT_OF_SYNC:
            return ims_CallFailCause_CALL_FAIL_PULL_OUT_OF_SYNC;
        case CallFailCause::CALL_FAIL_CAUSE_CALL_PULLED:
            return ims_CallFailCause_CALL_FAIL_CAUSE_CALL_PULLED;
        case CallFailCause::CALL_FAIL_SIP_REDIRECTED:
            return ims_CallFailCause_CALL_FAIL_SIP_REDIRECTED;
        case CallFailCause::CALL_FAIL_SIP_BAD_REQUEST:
            return ims_CallFailCause_CALL_FAIL_SIP_BAD_REQUEST;
        case CallFailCause::CALL_FAIL_SIP_FORBIDDEN:
            return ims_CallFailCause_CALL_FAIL_SIP_FORBIDDEN;
        case CallFailCause::CALL_FAIL_SIP_NOT_FOUND:
            return ims_CallFailCause_CALL_FAIL_SIP_NOT_FOUND;
        case CallFailCause::CALL_FAIL_SIP_NOT_SUPPORTED:
            return ims_CallFailCause_CALL_FAIL_SIP_NOT_SUPPORTED;
        case CallFailCause::CALL_FAIL_SIP_REQUEST_TIMEOUT:
            return ims_CallFailCause_CALL_FAIL_SIP_REQUEST_TIMEOUT;
        case CallFailCause::CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE:
            return ims_CallFailCause_CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE;
        case CallFailCause::CALL_FAIL_SIP_BAD_ADDRESS:
            return ims_CallFailCause_CALL_FAIL_SIP_BAD_ADDRESS;
        case CallFailCause::CALL_FAIL_SIP_BUSY:
            return ims_CallFailCause_CALL_FAIL_SIP_BUSY;
        case CallFailCause::CALL_FAIL_SIP_REQUEST_CANCELLED:
            return ims_CallFailCause_CALL_FAIL_SIP_REQUEST_CANCELLED;
        case CallFailCause::CALL_FAIL_SIP_NOT_ACCEPTABLE:
            return ims_CallFailCause_CALL_FAIL_SIP_NOT_ACCEPTABLE;
        case CallFailCause::CALL_FAIL_SIP_NOT_REACHABLE:
            return ims_CallFailCause_CALL_FAIL_SIP_NOT_REACHABLE;
        case CallFailCause::CALL_FAIL_SIP_SERVER_INTERNAL_ERROR:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_INTERNAL_ERROR;
        case CallFailCause::CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED;
        case CallFailCause::CALL_FAIL_SIP_SERVER_BAD_GATEWAY:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_BAD_GATEWAY;
        case CallFailCause::CALL_FAIL_SIP_SERVICE_UNAVAILABLE:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVICE_UNAVAILABLE;
        case CallFailCause::CALL_FAIL_SIP_SERVER_TIMEOUT:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_TIMEOUT;
        case CallFailCause::CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED;
        case CallFailCause::CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE;
        case CallFailCause::CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE:
            return ims_CallFailCause_CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE;
        case CallFailCause::CALL_FAIL_SIP_USER_REJECTED:
            return ims_CallFailCause_CALL_FAIL_SIP_USER_REJECTED;
        case CallFailCause::CALL_FAIL_SIP_GLOBAL_ERROR:
            return ims_CallFailCause_CALL_FAIL_SIP_GLOBAL_ERROR;
        case CallFailCause::CALL_FAIL_MEDIA_INIT_FAILED:
            return ims_CallFailCause_CALL_FAIL_MEDIA_INIT_FAILED;
        case CallFailCause::CALL_FAIL_MEDIA_NO_DATA:
            return ims_CallFailCause_CALL_FAIL_MEDIA_NO_DATA;
        case CallFailCause::CALL_FAIL_MEDIA_NOT_ACCEPTABLE:
            return ims_CallFailCause_CALL_FAIL_MEDIA_NOT_ACCEPTABLE;
        case CallFailCause::CALL_FAIL_MEDIA_UNSPECIFIED_ERROR:
            return ims_CallFailCause_CALL_FAIL_MEDIA_UNSPECIFIED_ERROR;
        case CallFailCause::CALL_FAIL_HOLD_RESUME_FAILED:
            return ims_CallFailCause_CALL_FAIL_HOLD_RESUME_FAILED;
        case CallFailCause::CALL_FAIL_HOLD_RESUME_CANCELED:
            return ims_CallFailCause_CALL_FAIL_HOLD_RESUME_CANCELED;
        case CallFailCause::CALL_FAIL_HOLD_REINVITE_COLLISION:
            return ims_CallFailCause_CALL_FAIL_HOLD_REINVITE_COLLISION;
        case CallFailCause::CALL_FAIL_ACCESS_CLASS_BLOCKED:
            return ims_CallFailCause_CALL_FAIL_ACCESS_CLASS_BLOCKED;
        case CallFailCause::CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO:
            return ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO;
        case CallFailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL:
            return ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL;
        case CallFailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
            return ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
        case CallFailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS:
            return ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS;
        case CallFailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD:
            return ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD;
        case CallFailCause::CALL_FAIL_ERROR_UNSPECIFIED:
        case CallFailCause::CALL_FAIL_INVALID:
        default:
            return ims_CallFailCause_CALL_FAIL_ERROR_UNSPECIFIED;
    }
}

ims_ServiceClassStatus convertHidlToProtoServiceClassStatus(ServiceClassStatus status) {
    switch (status) {
        case ServiceClassStatus::ENABLED:
            return ims_ServiceClassStatus_ENABLED;
        case ServiceClassStatus::DISABLED:
        default:
            return ims_ServiceClassStatus_DISABLED;
    }
}

ims_ServiceClassProvisionStatus convertHidlToProtoServiceClassProvisionStatus(
        V1_3::ServiceClassProvisionStatus status) {
    switch (status) {
        case V1_3::ServiceClassProvisionStatus::PROVISIONED:
            return ims_ServiceClassProvisionStatus_PROVISIONED;
        case V1_3::ServiceClassProvisionStatus::NOT_PROVISIONED:
        default:
            return ims_ServiceClassProvisionStatus_NOT_PROVISIONED;
    }
}

ims_Error convertHidlToProtoGeolocation(double lat, double lon,
        const V1_2::AddressInfo& inAddress, ims_GeoLocationInfo& outAddress) {
    ims_Error ret = ims_Error_E_SUCCESS;
    memset(&outAddress, 0, sizeof(outAddress));

    outAddress.has_lat = true;
    outAddress.lat = lat;
    outAddress.has_lon = true;
    outAddress.lon = lon;

    outAddress.has_addressInfo = true;
    bool success = true;
    success &= copy(inAddress.city, (char*&) outAddress.addressInfo.city.arg, "city");
    success &= copy(inAddress.state, (char*&) outAddress.addressInfo.state.arg, "state");
    success &= copy(inAddress.country,
            (char*&) outAddress.addressInfo.country.arg, "country");
    success &= copy(inAddress.postalCode,
            (char*&) outAddress.addressInfo.postalCode.arg, "postalCode");
    success &= copy(inAddress.countryCode,
            (char*&) outAddress.addressInfo.countryCode.arg, "countryCode");
    success &= copy(inAddress.street,
            (char*&) outAddress.addressInfo.street.arg, "street");
    success &= copy(inAddress.houseNumber,
            (char*&) outAddress.addressInfo.houseNumber.arg, "houseNumber");

    if (!success) {
        ret = ims_Error_E_GENERIC_FAILURE;
    }

    if (isError(ret)) {
        freeAddressInfo(outAddress.addressInfo);
    }

    return ret;
}

bool copy(const hidl_string& in, char*& out, const char* log_tag) {
    if (in.empty()) {
        QCRIL_LOG_INFO("Empty %s info", log_tag);
    } else {
        const size_t arglen = in.size();
        const size_t destSize = sizeof(char) * (arglen + 1);
        out = (char*)qcril_malloc(destSize);
        if (out == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory for %s", log_tag);
            return false;
        }
        strlcpy(out, in.c_str(), destSize);
    }
    return true;
}

void freeGeolocationInfo(ims_GeoLocationInfo& geoInfo) {
    freeAddressInfo(geoInfo.addressInfo);
}

void freeAddressInfo(ims_AddressInfo& addressInfo) {
    qcril_free(addressInfo.city.arg); addressInfo.city.arg = nullptr;
    qcril_free(addressInfo.state.arg); addressInfo.state.arg = nullptr;
    qcril_free(addressInfo.country.arg); addressInfo.country.arg = nullptr;
    qcril_free(addressInfo.postalCode.arg); addressInfo.postalCode.arg = nullptr;
    qcril_free(addressInfo.countryCode.arg); addressInfo.countryCode.arg = nullptr;
    qcril_free(addressInfo.street.arg); addressInfo.street.arg = nullptr;
    qcril_free(addressInfo.houseNumber.arg); addressInfo.houseNumber.arg = nullptr;
}

void convertCallListV14ToV13(hidl_vec<V1_4::CallInfo>& callListV14,
        hidl_vec<V1_3::CallInfo>& callListV13) {
    uint32_t count = callListV14.size();
    callListV13.resize(count);
    for (uint32_t i = 0; i < count; i++) {
        callListV13[i] = {};
        convertCallV14ToV13(callListV14[i], callListV13[i]);
    }
}

/*
 *   @brief
 *   Utility to convert CallInfo from Vesion 1.4
 *   to Version 1.3 in case of Radio HAL fallback
 *
 *   This conversion is based on the assumption that callInfoV14 does
 *   not go out of scope before callInfoV13
 */
void convertCallV14ToV13(V1_4::CallInfo& callInfoV14,
        V1_3::CallInfo& callInfoV13) {
    callInfoV13.state = callInfoV14.state;
    callInfoV13.index = callInfoV14.index;
    callInfoV13.toa = callInfoV14.toa;
    callInfoV13.hasIsMpty = callInfoV14.hasIsMpty;
    callInfoV13.isMpty = callInfoV14.isMpty;
    callInfoV13.hasIsMT = callInfoV14.hasIsMT;
    callInfoV13.isMT = callInfoV14.isMT;
    callInfoV13.als = callInfoV14.als;
    callInfoV13.hasIsVoice = callInfoV14.hasIsVoice;
    callInfoV13.isVoice = callInfoV14.isVoice;
    callInfoV13.hasIsVoicePrivacy = callInfoV14.hasIsVoicePrivacy;
    callInfoV13.isVoicePrivacy = callInfoV14.isVoicePrivacy;
    callInfoV13.number.setToExternal(callInfoV14.number.c_str(),
            callInfoV14.number.size());
    callInfoV13.numberPresentation = callInfoV14.numberPresentation;
    callInfoV13.name.setToExternal(callInfoV14.name.c_str(),
            callInfoV14.name.size());
    callInfoV13.namePresentation = callInfoV14.namePresentation;

    callInfoV13.hasCallDetails = callInfoV14.hasCallDetails;
    CallDetails& callDetailsV14 = callInfoV14.callDetails;
    CallDetails& callDetailsV13 = callInfoV13.callDetails;
    callDetailsV13.callType = callDetailsV14.callType;
    callDetailsV13.callDomain = callDetailsV14.callDomain;
    callDetailsV13.extrasLength = callDetailsV14.extrasLength;
    callDetailsV13.extras.setToExternal(callDetailsV14.extras.data(),
            callDetailsV14.extras.size());
    callDetailsV13.localAbility.setToExternal(
            callDetailsV14.localAbility.data(),
            callDetailsV14.localAbility.size());
    callDetailsV13.peerAbility.setToExternal(
            callDetailsV14.peerAbility.data(),
            callDetailsV14.peerAbility.size());
    callDetailsV13.callSubstate = callDetailsV14.callSubstate;
    callDetailsV13.mediaId = callDetailsV14.mediaId;
    callDetailsV13.causeCode = callDetailsV14.causeCode;
    callDetailsV13.rttMode = callDetailsV14.rttMode;
    callDetailsV13.sipAlternateUri.setToExternal(
           callDetailsV14.sipAlternateUri.c_str(),
           callDetailsV14.sipAlternateUri.size());

    callInfoV13.hasFailCause = callInfoV14.hasFailCause;
    V1_3::CallFailCauseResponse& callFailV14 = callInfoV14.failCause;
    V1_3::CallFailCauseResponse& callFailV13 = callInfoV13.failCause;
    callFailV13.failCause = callFailV14.failCause;
    callFailV13.errorinfo.setToExternal(callFailV14.errorinfo.data(),
            callFailV14.errorinfo.size());
    callFailV13.networkErrorString.setToExternal(callFailV14.networkErrorString.c_str(),
        callFailV14.networkErrorString.size());
    callFailV13.hasErrorDetails = callFailV14.hasErrorDetails;
    callFailV13.errorDetails.errorCode = callFailV14.errorDetails.errorCode;
    callFailV13.errorDetails.errorString.setToExternal(
        callFailV14.errorDetails.errorString.c_str(),
        callFailV14.errorDetails.errorString.size());

    callInfoV13.hasIsEncrypted = callInfoV14.hasIsEncrypted;
    callInfoV13.isEncrypted = callInfoV14.isEncrypted;
    callInfoV13.hasIsCalledPartyRinging = callInfoV14.hasIsCalledPartyRinging;
    callInfoV13.isCalledPartyRinging = callInfoV14.isCalledPartyRinging;
    callInfoV13.hasIsVideoConfSupported = callInfoV14.hasIsVideoConfSupported;
    callInfoV13.isVideoConfSupported = callInfoV14.isVideoConfSupported;
    callInfoV13.historyInfo.setToExternal(callInfoV14.historyInfo.c_str(),
        callInfoV13.historyInfo.size());
}

void convertCallListV13ToV12(hidl_vec<V1_3::CallInfo>& callListV13,
        hidl_vec<V1_2::CallInfo>& callListV12) {
    uint32_t count = callListV13.size();
    callListV12.resize(count);
    for (uint32_t i = 0; i < count; i++) {
        callListV12[i] = {};
        convertCallV13ToV12(callListV13[i], callListV12[i]);
    }
}

/*
 *   @brief
 *   Utility to convert CallInfo from Vesion 1.1
 *   to Version 1.0 in case of Radio HAL fallback
 *
 *   This conversion is based on the assumption that callInfoV12 does
 *   not go out of scope before callInfoV11
 */
void convertCallV13ToV12(V1_3::CallInfo& callInfoV13,
        V1_2::CallInfo& callInfoV12) {
    callInfoV12.state = callInfoV13.state;
    callInfoV12.index = callInfoV13.index;
    callInfoV12.toa = callInfoV13.toa;
    callInfoV12.hasIsMpty = callInfoV13.hasIsMpty;
    callInfoV12.isMpty = callInfoV13.isMpty;
    callInfoV12.hasIsMT = callInfoV13.hasIsMT;
    callInfoV12.isMT = callInfoV13.isMT;
    callInfoV12.als = callInfoV13.als;
    callInfoV12.hasIsVoice = callInfoV13.hasIsVoice;
    callInfoV12.isVoice = callInfoV13.isVoice;
    callInfoV12.hasIsVoicePrivacy = callInfoV13.hasIsVoicePrivacy;
    callInfoV12.isVoicePrivacy = callInfoV13.isVoicePrivacy;
    callInfoV12.number.setToExternal(callInfoV13.number.c_str(),
            callInfoV13.number.size());
    callInfoV12.numberPresentation = callInfoV13.numberPresentation;
    callInfoV12.name.setToExternal(callInfoV13.name.c_str(),
            callInfoV13.name.size());
    callInfoV12.namePresentation = callInfoV13.namePresentation;

    callInfoV12.hasCallDetails = callInfoV13.hasCallDetails;
    CallDetails& callDetailsV13 = callInfoV13.callDetails;
    CallDetails& callDetailsV12 = callInfoV12.callDetails;
    callDetailsV12.callType = callDetailsV13.callType;
    callDetailsV12.callDomain = callDetailsV13.callDomain;
    callDetailsV12.extrasLength = callDetailsV13.extrasLength;
    callDetailsV12.extras.setToExternal(callDetailsV13.extras.data(),
            callDetailsV13.extras.size());
    callDetailsV12.localAbility.setToExternal(
            callDetailsV13.localAbility.data(),
            callDetailsV13.localAbility.size());
    callDetailsV12.peerAbility.setToExternal(
            callDetailsV13.peerAbility.data(),
            callDetailsV13.peerAbility.size());
    callDetailsV12.callSubstate = callDetailsV13.callSubstate;
    callDetailsV12.mediaId = callDetailsV13.mediaId;
    callDetailsV12.causeCode = callDetailsV13.causeCode;
    callDetailsV12.rttMode = callDetailsV13.rttMode;
    callDetailsV12.sipAlternateUri.setToExternal(
           callDetailsV13.sipAlternateUri.c_str(),
           callDetailsV13.sipAlternateUri.size());

    callInfoV12.hasFailCause = callInfoV13.hasFailCause;
    V1_3::CallFailCauseResponse& callFailV13 = callInfoV13.failCause;
    V1_2::CallFailCauseResponse& callFailV12 = callInfoV12.failCause;
    callFailV12.failCause = convertCallFailCauseV13ToV12(callFailV13.failCause);
    callFailV12.errorinfo.setToExternal(callFailV13.errorinfo.data(),
            callFailV13.errorinfo.size());
    callFailV12.networkErrorString.setToExternal(callFailV13.networkErrorString.c_str(),
        callFailV13.networkErrorString.size());
    callFailV12.hasErrorDetails = callFailV13.hasErrorDetails;
    callFailV12.errorDetails.errorCode = callFailV13.errorDetails.errorCode;
    callFailV12.errorDetails.errorString.setToExternal(
        callFailV13.errorDetails.errorString.c_str(),
        callFailV13.errorDetails.errorString.size());

    callInfoV12.hasIsEncrypted = callInfoV13.hasIsEncrypted;
    callInfoV12.isEncrypted = callInfoV13.isEncrypted;
    callInfoV12.hasIsCalledPartyRinging = callInfoV13.hasIsCalledPartyRinging;
    callInfoV12.isCalledPartyRinging = callInfoV13.isCalledPartyRinging;
    callInfoV12.hasIsVideoConfSupported = callInfoV13.hasIsVideoConfSupported;
    callInfoV12.isVideoConfSupported = callInfoV13.isVideoConfSupported;
    callInfoV12.historyInfo.setToExternal(callInfoV13.historyInfo.c_str(),
        callInfoV12.historyInfo.size());
}

V1_2::CallFailCause convertCallFailCauseV13ToV12(const V1_3::CallFailCause& failCause) {
    switch (failCause) {
        //return V1.2 CALL_FAIL_ERROR_UNSPECIFIED for all the new fail causes
        //added in V1.3
        case V1_3::CallFailCause::CALL_FAIL_RADIO_OFF:
        case V1_3::CallFailCause::CALL_FAIL_OUT_OF_SERVICE:
        case V1_3::CallFailCause::CALL_FAIL_NO_VALID_SIM:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_INTERNAL_ERROR:
        case V1_3::CallFailCause::CALL_FAIL_NETWORK_RESP_TIMEOUT:
        case V1_3::CallFailCause::CALL_FAIL_NETWORK_REJECT:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_ACCESS_FAILURE:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_LINK_FAILURE:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_LINK_LOST:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_UPLINK_FAILURE:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_SETUP_FAILURE:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_RELEASE_NORMAL:
        case V1_3::CallFailCause::CALL_FAIL_RADIO_RELEASE_ABNORMAL:
        case V1_3::CallFailCause::CALL_FAIL_NETWORK_DETACH:
            return V1_2::CallFailCause::CALL_FAIL_ERROR_UNSPECIFIED;
        default:
            return (V1_2::CallFailCause)failCause;
    }
}

void convertCallListV12ToV11(hidl_vec<V1_2::CallInfo>& callListV12,
        hidl_vec<V1_1::CallInfo>& callListV11) {
    uint32_t count = callListV12.size();
    callListV11.resize(count);
    for (uint32_t i = 0; i < count; i++) {
        callListV11[i] = {};
        convertCallV12ToV11(callListV12[i], callListV11[i]);
    }
}

/*
 *   @brief
 *   Utility to convert CallInfo from Vesion 1.1
 *   to Version 1.0 in case of Radio HAL fallback
 *
 *   This conversion is based on the assumption that callInfoV12 does
 *   not go out of scope before callInfoV11
 */
void convertCallV12ToV11(V1_2::CallInfo& callInfoV12,
        V1_1::CallInfo& callInfoV11) {
    callInfoV11.state = callInfoV12.state;
    callInfoV11.index = callInfoV12.index;
    callInfoV11.toa = callInfoV12.toa;
    callInfoV11.hasIsMpty = callInfoV12.hasIsMpty;
    callInfoV11.isMpty = callInfoV12.isMpty;
    callInfoV11.hasIsMT = callInfoV12.hasIsMT;
    callInfoV11.isMT = callInfoV12.isMT;
    callInfoV11.als = callInfoV12.als;
    callInfoV11.hasIsVoice = callInfoV12.hasIsVoice;
    callInfoV11.isVoice = callInfoV12.isVoice;
    callInfoV11.hasIsVoicePrivacy = callInfoV12.hasIsVoicePrivacy;
    callInfoV11.isVoicePrivacy = callInfoV12.isVoicePrivacy;
    callInfoV11.number.setToExternal(callInfoV12.number.c_str(),
            callInfoV12.number.size());
    callInfoV11.numberPresentation = callInfoV12.numberPresentation;
    callInfoV11.name.setToExternal(callInfoV12.name.c_str(),
            callInfoV12.name.size());
    callInfoV11.namePresentation = callInfoV12.namePresentation;

    callInfoV11.hasCallDetails = callInfoV12.hasCallDetails;
    CallDetails& callDetailsV12 = callInfoV12.callDetails;
    CallDetails& callDetailsV11 = callInfoV11.callDetails;
    callDetailsV11.callType = callDetailsV12.callType;
    callDetailsV11.callDomain = callDetailsV12.callDomain;
    callDetailsV11.extrasLength = callDetailsV12.extrasLength;
    callDetailsV11.extras.setToExternal(callDetailsV12.extras.data(),
            callDetailsV12.extras.size());
    callDetailsV11.localAbility.setToExternal(
            callDetailsV12.localAbility.data(),
            callDetailsV12.localAbility.size());
    callDetailsV11.peerAbility.setToExternal(
            callDetailsV12.peerAbility.data(),
            callDetailsV12.peerAbility.size());
    callDetailsV11.callSubstate = callDetailsV12.callSubstate;
    callDetailsV11.mediaId = callDetailsV12.mediaId;
    callDetailsV11.causeCode = callDetailsV12.causeCode;
    callDetailsV11.rttMode = callDetailsV12.rttMode;
    callDetailsV11.sipAlternateUri.setToExternal(
           callDetailsV12.sipAlternateUri.c_str(),
           callDetailsV12.sipAlternateUri.size());

    callInfoV11.hasFailCause = callInfoV12.hasFailCause;
    V1_2::CallFailCauseResponse& callFailV12 = callInfoV12.failCause;
    V1_1::CallFailCauseResponse& callFailV11 = callInfoV11.failCause;
    callFailV11.failCause = convertCallFailCauseV12ToV11(callFailV12.failCause);
    callFailV11.errorinfo.setToExternal(callFailV12.errorinfo.data(),
            callFailV12.errorinfo.size());
    callFailV11.networkErrorString.setToExternal(callFailV12.networkErrorString.c_str(),
        callFailV12.networkErrorString.size());
    callFailV11.hasErrorDetails = callFailV12.hasErrorDetails;
    callFailV11.errorDetails.errorCode = callFailV12.errorDetails.errorCode;
    callFailV11.errorDetails.errorString.setToExternal(
        callFailV12.errorDetails.errorString.c_str(),
        callFailV12.errorDetails.errorString.size());

    callInfoV11.hasIsEncrypted = callInfoV12.hasIsEncrypted;
    callInfoV11.isEncrypted = callInfoV12.isEncrypted;
    callInfoV11.hasIsCalledPartyRinging = callInfoV12.hasIsCalledPartyRinging;
    callInfoV11.isCalledPartyRinging = callInfoV12.isCalledPartyRinging;
    callInfoV11.hasIsVideoConfSupported = callInfoV12.hasIsVideoConfSupported;
    callInfoV11.isVideoConfSupported = callInfoV12.isVideoConfSupported;
    callInfoV11.historyInfo.setToExternal(callInfoV12.historyInfo.c_str(),
        callInfoV12.historyInfo.size());
}

V1_1::CallFailCause convertCallFailCauseV12ToV11(const V1_2::CallFailCause& failCause) {
    switch (failCause) {
        //return V1.1 CALL_FAIL_ERROR_UNSPECIFIED for all the new fail causes
        //added in V1.2
        case V1_2::CallFailCause::CALL_FAIL_RADIO_OFF:
        case V1_2::CallFailCause::CALL_FAIL_OUT_OF_SERVICE:
        case V1_2::CallFailCause::CALL_FAIL_NO_VALID_SIM:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_INTERNAL_ERROR:
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_RESP_TIMEOUT:
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_REJECT:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_ACCESS_FAILURE:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_LINK_FAILURE:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_LINK_LOST:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_UPLINK_FAILURE:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_SETUP_FAILURE:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_RELEASE_NORMAL:
        case V1_2::CallFailCause::CALL_FAIL_RADIO_RELEASE_ABNORMAL:
        case V1_2::CallFailCause::CALL_FAIL_NETWORK_DETACH:
        case V1_2::CallFailCause::CALL_FAIL_NO_CSFB_IN_CS_ROAM:
            return V1_1::CallFailCause::CALL_FAIL_ERROR_UNSPECIFIED;
        default:
            return (V1_1::CallFailCause)failCause;
    }
}

void convertCallListV11ToV10(hidl_vec<V1_1::CallInfo>& callListV11,
        hidl_vec<V1_0::CallInfo>& callListV10) {
    uint32_t count = callListV11.size();
    callListV10.resize(count);
    for (uint32_t i = 0; i < count; i++) {
        callListV10[i] = {};
        convertCallV11ToV10(callListV11[i], callListV10[i]);
    }
 }

/*
 *   @brief
 *   Utility to convert CallInfo from Vesion 1.1
 *   to Version 1.0 in case of Radio HAL fallback
 *
 *   This conversion is based on the assumption that callInfoV11 does
 *   not go out of scope before callInfoV10
 */
void convertCallV11ToV10(V1_1::CallInfo& callInfoV11,
        V1_0::CallInfo& callInfoV10) {
    callInfoV10.state = callInfoV11.state;
    callInfoV10.index = callInfoV11.index;
    callInfoV10.toa = callInfoV11.toa;
    callInfoV10.hasIsMpty = callInfoV11.hasIsMpty;
    callInfoV10.isMpty = callInfoV11.isMpty;
    callInfoV10.hasIsMT = callInfoV11.hasIsMT;
    callInfoV10.isMT = callInfoV11.isMT;
    callInfoV10.als = callInfoV11.als;
    callInfoV10.hasIsVoice = callInfoV11.hasIsVoice;
    callInfoV10.isVoice = callInfoV11.isVoice;
    callInfoV10.hasIsVoicePrivacy = callInfoV11.hasIsVoicePrivacy;
    callInfoV10.isVoicePrivacy = callInfoV11.isVoicePrivacy;
    callInfoV10.number.setToExternal(callInfoV11.number.c_str(),
            callInfoV11.number.size());
    callInfoV10.numberPresentation = callInfoV11.numberPresentation;
    callInfoV10.name.setToExternal(callInfoV11.name.c_str(),
            callInfoV11.name.size());
    callInfoV10.namePresentation = callInfoV11.namePresentation;

    callInfoV10.hasCallDetails = callInfoV11.hasCallDetails;
    CallDetails& callDetailsV11 = callInfoV11.callDetails;
    CallDetails& callDetailsV10 = callInfoV10.callDetails;
    callDetailsV10.callType = callDetailsV11.callType;
    callDetailsV10.callDomain = callDetailsV11.callDomain;
    callDetailsV10.extrasLength = callDetailsV11.extrasLength;
    callDetailsV10.extras.setToExternal(callDetailsV11.extras.data(),
            callDetailsV11.extras.size());
    callDetailsV10.localAbility.setToExternal(
            callDetailsV11.localAbility.data(),
            callDetailsV11.localAbility.size());
    callDetailsV10.peerAbility.setToExternal(
            callDetailsV11.peerAbility.data(),
            callDetailsV11.peerAbility.size());
    callDetailsV10.callSubstate = callDetailsV11.callSubstate;
    callDetailsV10.mediaId = callDetailsV11.mediaId;
    callDetailsV10.causeCode = callDetailsV11.causeCode;
    callDetailsV10.rttMode = callDetailsV11.rttMode;
    callDetailsV10.sipAlternateUri.setToExternal(
           callDetailsV11.sipAlternateUri.c_str(),
           callDetailsV11.sipAlternateUri.size());

    callInfoV10.hasFailCause = callInfoV11.hasFailCause;
    V1_1::CallFailCauseResponse& callFailV11 = callInfoV11.failCause;
    V1_0::CallFailCauseResponse& callFailV10 = callInfoV10.failCause;
    callFailV10.failCause = convertCallFailCauseToV10(callFailV11.failCause);
    callFailV10.errorinfo.setToExternal(callFailV11.errorinfo.data(),
            callFailV11.errorinfo.size());
    callFailV10.networkErrorString.setToExternal(callFailV11.networkErrorString.c_str(),
        callFailV11.networkErrorString.size());
    callFailV10.hasErrorDetails = callFailV11.hasErrorDetails;
    callFailV10.errorDetails.errorCode = callFailV11.errorDetails.errorCode;
    callFailV10.errorDetails.errorString.setToExternal(
        callFailV11.errorDetails.errorString.c_str(),
        callFailV11.errorDetails.errorString.size());

    callInfoV10.hasIsEncrypted = callInfoV11.hasIsEncrypted;
    callInfoV10.isEncrypted = callInfoV11.isEncrypted;
    callInfoV10.hasIsCalledPartyRinging = callInfoV11.hasIsCalledPartyRinging;
    callInfoV10.isCalledPartyRinging = callInfoV11.isCalledPartyRinging;
    callInfoV10.hasIsVideoConfSupported = callInfoV11.hasIsVideoConfSupported;
    callInfoV10.isVideoConfSupported = callInfoV11.isVideoConfSupported;
    callInfoV10.historyInfo.setToExternal(callInfoV11.historyInfo.c_str(),
        callInfoV11.historyInfo.size());
}

V1_0::CallFailCause convertCallFailCauseToV10(const V1_1::CallFailCause& failCause) {
    return (failCause == V1_1::CallFailCause::CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL)?
        V1_0::CallFailCause::CALL_FAIL_ERROR_UNSPECIFIED :
            (V1_0::CallFailCause)failCause;
}

void convertProtoToHidlCallList(const ims_CallList& inCallList,
        hidl_vec<V1_4::CallInfo>& outCallList) {
    const ims_CallList_Call** ptr = (const ims_CallList_Call**)(inCallList.callAttributes.arg);
    uint32_t count = getCount(ptr);
    outCallList.resize(count);
    for(uint32_t i = 0; i < count; ++i) {
        outCallList[i] = {};
        convertProtoToHidlCall(*(ptr[i]), outCallList[i]);
    }
 }

void convertProtoToHidlCall(const ims_CallList_Call& inCall, V1_4::CallInfo& outCall) {
    if (inCall.has_state) {
        outCall.state = convertProtoToHidlCallState(inCall.state);
    } else {
        outCall.state = CallState::CALL_STATE_INVALID;
    }

    if (inCall.has_index) {
        outCall.index = inCall.index;
    } else {
        outCall.index = INT32_MAX;
    }

    if (inCall.has_toa) {
        outCall.toa = inCall.toa;
    } else {
        outCall.toa = INT32_MAX;
    }

    outCall.hasIsMpty = inCall.has_isMpty;
    if (inCall.has_isMpty) {
        outCall.isMpty = inCall.isMpty;
    }

    outCall.hasIsMT = inCall.has_isMT;
    if (inCall.has_isMT) {
        outCall.isMT = inCall.isMT;
    }

    if (inCall.has_als) {
        outCall.als = inCall.als;
    } else {
        outCall.als = INT32_MAX;
    }

    outCall.hasIsVoice = inCall.has_isVoice;
    if (inCall.has_isVoice) {
        outCall.isVoice = inCall.isVoice;
    }

    outCall.hasIsVoicePrivacy = inCall.has_isVoicePrivacy;
    if (inCall.has_isVoicePrivacy) {
        outCall.isVoicePrivacy = inCall.isVoicePrivacy;
    }

    const char* ptr = (const char*)(inCall.number.arg);
    if (ptr != nullptr) {
        outCall.number.setToExternal(ptr, strlen(ptr));
    }

    if (inCall.has_numberPresentation) {
        outCall.numberPresentation = inCall.numberPresentation;
    } else {
        outCall.numberPresentation = INT32_MAX;
    }

    ptr = (const char*)(inCall.name.arg);
    if (ptr != nullptr) {
        outCall.name.setToExternal(ptr, strlen(ptr));
    }

    if (inCall.has_namePresentation) {
        outCall.namePresentation = inCall.namePresentation;
    } else {
        outCall.namePresentation = INT32_MAX;
    }

    if (inCall.has_callDetails) {
        outCall.hasCallDetails = true;
        convertProtoToHidlCallDetails(inCall.callDetails, outCall.callDetails);
    }

    if (inCall.has_failCause) {
        outCall.hasFailCause = true;
        convertProtoToHidlCallFailCauseResponse(inCall.failCause, outCall.failCause);
    }

    outCall.hasIsEncrypted = inCall.has_isEncrypted;
    if (inCall.has_isEncrypted) {
        outCall.isEncrypted = inCall.isEncrypted;
    }

    outCall.hasIsCalledPartyRinging = inCall.has_isCalledPartyRinging;
    if (inCall.has_isCalledPartyRinging) {
        outCall.isCalledPartyRinging = inCall.isCalledPartyRinging;
    }

    outCall.hasIsVideoConfSupported = inCall.has_peerConfAbility?
            inCall.peerConfAbility.has_isVideoConfSupported: false;
    if (outCall.hasIsVideoConfSupported) {
        outCall.isVideoConfSupported = inCall.peerConfAbility.isVideoConfSupported;
    }

    ptr = (const char*)(inCall.historyInfo.arg);
    if (ptr != nullptr) {
        outCall.historyInfo.setToExternal(ptr, strlen(ptr));
    }

    //Set the Verstat Information
    if (inCall.hasVerstatInfo) {
        convertToHidlVerstatInfo(inCall.verstatInfo,
                outCall.verstatInfo);
    }
    else {
        setDefaultVerstatInfo(outCall.verstatInfo);
    }
    std::string logStr = toString(outCall.verstatInfo);
    QCRIL_LOG_INFO("Verstat Info = %s", logStr.c_str());

    //Secondary Line Information
    const char* terminating = (const char*)(inCall.terminatingNumber.arg);
    auto& info = outCall.mtMultiLineInfo;
    if (terminating != nullptr) {
        info.msisdn.setToExternal(terminating, strlen(terminating));
        info.lineType = inCall.isSecondary ?
           V1_4::MultiIdentityLineType::LINE_TYPE_SECONDARY :
           V1_4::MultiIdentityLineType::LINE_TYPE_PRIMARY;
    QCRIL_LOG_INFO("Terminating Number = %s  , Is Seconadary = %d",
            info.msisdn.c_str(),
            info.lineType);
    }
}

void convertProtoToHidlCallDetails(const ims_CallDetails& inCallDetails,
        CallDetails& outCallDetails) {

    if (inCallDetails.has_callType) {
        outCallDetails.callType = convertProtoToHidlCallType(inCallDetails.callType);
    } else {
        outCallDetails.callType = CallType::CALL_TYPE_INVALID;
    }

    if (inCallDetails.has_callDomain) {
        outCallDetails.callDomain = convertProtoToHidlCallDomain(inCallDetails.callDomain);
    } else {
        outCallDetails.callDomain = CallDomain::CALL_DOMAIN_INVALID;
    }

    if (inCallDetails.has_extrasLength) {
        outCallDetails.extrasLength = inCallDetails.extrasLength;
    } else {
        outCallDetails.extrasLength = INT32_MAX;
    }

    const char** ptr = (const char**)(inCallDetails.extras.arg);
    uint32_t count = getCount(ptr);
    outCallDetails.extras.resize(count);
    for(uint32_t i = 0; i < count; ++i) {
        outCallDetails.extras[i].setToExternal(ptr[i], strlen(ptr[i]));
    }

    convertProtoToHidlServiceInfoList((const ims_Info**)
            (inCallDetails.localAbility.SrvStatusInfo.arg),
            outCallDetails.localAbility);

    convertProtoToHidlServiceInfoList((const ims_Info**)
            (inCallDetails.peerAbility.SrvStatusInfo.arg),
            outCallDetails.peerAbility);

    if (inCallDetails.has_callSubstate) {
        outCallDetails.callSubstate = inCallDetails.callSubstate;
    } else {
        outCallDetails.callSubstate = INT32_MAX;
    }

    if (inCallDetails.has_mediaId) {
        outCallDetails.mediaId = inCallDetails.mediaId;
    } else {
        outCallDetails.mediaId = INT32_MAX;
    }

    if (inCallDetails.has_causeCode) {
        outCallDetails.causeCode = inCallDetails.causeCode;
    } else {
        outCallDetails.causeCode = INT32_MAX;
    }

    if(inCallDetails.has_rttMode) {
       outCallDetails.rttMode = convertProtoToHidlRttMode(inCallDetails.rttMode);
    } else {
        outCallDetails.rttMode = RttMode::RTT_MODE_INVALID;
    }
    QCRIL_LOG_INFO("Out rrtMode %d ", outCallDetails.rttMode);

    const char* ptr1 = (const char*)inCallDetails.sipAlternateUri.arg;
    if (ptr1 != NULL) {
        outCallDetails.sipAlternateUri.setToExternal(ptr1, strlen(ptr1));
        QCRIL_LOG_INFO("Out Sip Redirect Uri %s ", outCallDetails.sipAlternateUri.c_str());
    }

}

void setDefaultVerstatInfo(V1_3::VerstatInfo& verstatInfo) {
    verstatInfo.canMarkUnwantedCall = false;
    verstatInfo.verificationStatus =
            V1_2::VerificationStatus::STATUS_VALIDATION_NONE;
}

void convertToHidlVerstatInfo(const ims_VerstatInfo& inVerstatInfo,
        V1_3::VerstatInfo& outVerstatInfo) {
    outVerstatInfo.canMarkUnwantedCall = inVerstatInfo.canMarkUnwantedCall;
    outVerstatInfo.verificationStatus = convertToHidlVerstatVerificationStatus(
                inVerstatInfo.verificationStatus);
}

V1_2::VerificationStatus convertToHidlVerstatVerificationStatus(
        ims_VerstatVerificationStatus status){
    switch(status) {
        case ims_VerstatVerificationStatus_PASS:
            return V1_2::VerificationStatus::STATUS_VALIDATION_PASS;
        case ims_VerstatVerificationStatus_FAIL:
            return V1_2::VerificationStatus::STATUS_VALIDATION_FAIL;
        default:
            return V1_2::VerificationStatus::STATUS_VALIDATION_NONE;
    }
}

void convertProtoToHidlServiceInfoList(const ims_Info** const inServiceInfoList,
        hidl_vec<ServiceStatusInfo>& outServiceInfoList) {
    if (inServiceInfoList != nullptr) {
        uint32_t count = getCount(inServiceInfoList);
        outServiceInfoList.resize(count);
        for(uint32_t i = 0; i < count; ++i) {
            outServiceInfoList[i] = {};
            convertProtoToHidlServiceStatusInfo(*(inServiceInfoList[i]), outServiceInfoList[i]);
        }
    }
}

void convertProtoToHidlServiceStatusInfo(const ims_Info& inServiceStatusInfo,
        ServiceStatusInfo& outServiceStatusInfo) {
    outServiceStatusInfo.hasIsValid = inServiceStatusInfo.has_isValid;
    if (inServiceStatusInfo.has_isValid) {
        outServiceStatusInfo.isValid = inServiceStatusInfo.isValid;
    }

    if (inServiceStatusInfo.has_type) {
        outServiceStatusInfo.type = convertProtoToHidlServiceType(inServiceStatusInfo.type);
    } else {
        outServiceStatusInfo.type = ServiceType::INVALID;
    }

    if (inServiceStatusInfo.has_callType) {
        outServiceStatusInfo.callType = convertProtoToHidlCallType(inServiceStatusInfo.callType);
    } else {
        outServiceStatusInfo.callType = CallType::CALL_TYPE_INVALID;
    }

    if (inServiceStatusInfo.has_status) {
        outServiceStatusInfo.status = convertProtoToHidlStatusType(inServiceStatusInfo.status);
    } else {
        outServiceStatusInfo.status = StatusType::STATUS_INVALID;
    }

    qcril_binary_data_type* userdataPtr =
            (qcril_binary_data_type*)inServiceStatusInfo.userdata.arg;
    if (userdataPtr != nullptr && userdataPtr->data != nullptr) {
        outServiceStatusInfo.userdata.setToExternal(userdataPtr->data, userdataPtr->len);
    }

    if (inServiceStatusInfo.has_restrictCause) {
        outServiceStatusInfo.restrictCause = inServiceStatusInfo.restrictCause;
    } else {
        outServiceStatusInfo.restrictCause = INT32_MAX;
    }

    if(inServiceStatusInfo.has_rttMode) {
       outServiceStatusInfo.rttMode = (inServiceStatusInfo.rttMode) ? RttMode::RTT_MODE_FULL : RttMode::RTT_MODE_DISABLED;
    } else {
        outServiceStatusInfo.rttMode = RttMode::RTT_MODE_INVALID;
    }
    QCRIL_LOG_INFO("Out rrtMode %d ", outServiceStatusInfo.rttMode);

    const ims_StatusForAccessTech** ptr =
            (const ims_StatusForAccessTech**)inServiceStatusInfo.accTechStatus.arg;

    if (ptr != nullptr) {
        uint32_t count = getCount(ptr);
        outServiceStatusInfo.accTechStatus.resize(count);
        for(uint32_t i = 0; i < count; ++i) {
            outServiceStatusInfo.accTechStatus[i] = {};
            convertProtoToHidlStatusForAccessTech(*(ptr[i]), outServiceStatusInfo.accTechStatus[i]);
        }
    }

}

void convertProtoToHidlStatusForAccessTech(const ims_StatusForAccessTech& inAccTechStatus,
        StatusForAccessTech& outAccTechStatus) {
    if (inAccTechStatus.has_networkMode) {
        outAccTechStatus.networkMode = convertProtoToHidlRadioTech(inAccTechStatus.networkMode);
    } else {
        outAccTechStatus.networkMode = RadioTechType::RADIO_TECH_INVALID;
    }

    if (inAccTechStatus.has_status) {
        outAccTechStatus.status = convertProtoToHidlStatusType(inAccTechStatus.status);
    } else {
        outAccTechStatus.status = StatusType::STATUS_INVALID;
    }

    if (inAccTechStatus.has_restrictionCause) {
        outAccTechStatus.restrictCause = inAccTechStatus.restrictionCause;
    } else {
        outAccTechStatus.restrictCause = INT32_MAX;
    }

    outAccTechStatus.hasRegistration = inAccTechStatus.has_registered;
    if (inAccTechStatus.has_registered) {
        convertProtoToHidlRegistration(inAccTechStatus.registered,
                outAccTechStatus.registration);
    }
}

void convertProtoToHidlCallFailCauseResponse(const ims_CallFailCauseResponse& inFailCause,
        V1_3::CallFailCauseResponse& outFailCause) {
    if (inFailCause.has_failcause) {
        outFailCause.failCause = convertProtoToHidlCallFailCause(inFailCause.failcause);
    }

    qcril_binary_data_type* ptr = (qcril_binary_data_type*)(inFailCause.errorinfo.arg);
    if (ptr != nullptr && ptr->data != nullptr) {
        outFailCause.errorinfo.setToExternal(ptr->data, ptr->len);
    }

    const char* networkErrorStringPtr = (const char*)(inFailCause.networkErrorString.arg);
    if (networkErrorStringPtr != nullptr) {
        outFailCause.networkErrorString.setToExternal(networkErrorStringPtr,
                strlen(networkErrorStringPtr));
    }

    if (inFailCause.has_errorDetails) {
        outFailCause.hasErrorDetails = (ims_Error_E_SUCCESS ==
                convertProtoToHidlSipErrorInfo(inFailCause.errorDetails,
                outFailCause.errorDetails));
    }
}

void convertProtoToHidlHandoverInfo(const ims_Handover& inHandover, HandoverInfo& outHandover) {
    outHandover.type = HandoverType::INVALID;
    if (inHandover.has_type) {
        outHandover.type = convertProtoToHidlHandoverType(inHandover.type);
    }

    outHandover.srcTech = RadioTechType::RADIO_TECH_INVALID;
    if (inHandover.has_srcTech) {
        outHandover.srcTech = convertProtoToHidlRadioTech(inHandover.srcTech);
    }

    outHandover.targetTech = RadioTechType::RADIO_TECH_INVALID;
    if (inHandover.has_targetTech) {
        outHandover.targetTech = convertProtoToHidlRadioTech(inHandover.targetTech);
    }

    outHandover.hasHoExtra = inHandover.has_hoExtra;
    if (inHandover.has_hoExtra) {
        convertProtoToHidlHandoverExtra(inHandover.hoExtra, outHandover.hoExtra);
    }

    const char* ptr = (const char*)inHandover.errorCode.arg;
    if (ptr != nullptr) {
        outHandover.errorCode.setToExternal(ptr, strlen(ptr));
    }

    ptr = (const char*)inHandover.errorMessage.arg;
    if (ptr != nullptr) {
        outHandover.errorMessage.setToExternal(ptr, strlen(ptr));
    }
}

void convertProtoToHidlHandoverExtra(const ims_Extra& inExtra, Extra& outExtra) {
    qcril_binary_data_type* extraPtr = (qcril_binary_data_type*)inExtra.extraInfo.arg;

    outExtra.type = ExtraType::INVALID;
    if (inExtra.has_type && inExtra.type == ims_Extra_Type_LTE_TO_IWLAN_HO_FAIL) {
        outExtra.type = ExtraType::LTE_TO_IWLAN_HO_FAIL;
    }

    if (extraPtr != nullptr && extraPtr->data != nullptr) {
        outExtra.extraInfo.setToExternal(extraPtr->data, extraPtr->len);
    }
}

void convertProtoToHidlCallForwardInfoList(
        const ims_CallForwardInfoList_CallForwardInfo** inCfInfoList,
        hidl_vec<CallForwardInfo>& outCfInfoList) {
    if (inCfInfoList == NULL) {
        return;
    }

    uint32_t count = getCount(inCfInfoList);
    outCfInfoList.resize(count);

    for (uint32_t i = 0; i < count; ++i) {
        outCfInfoList[i] = {};
        convertProtoToHidlCallForwardInfo(*(inCfInfoList[i]), outCfInfoList[i]);
    }
}

void convertProtoToHidlCallForwardInfo(
        const ims_CallForwardInfoList_CallForwardInfo& inCallForwardInfo,
        CallForwardInfo& outCallForwardInfo) {
    outCallForwardInfo.status = (inCallForwardInfo.has_status) ? inCallForwardInfo.status :
            INT32_MAX;
    outCallForwardInfo.reason = (inCallForwardInfo.has_reason) ? inCallForwardInfo.reason :
            INT32_MAX;
    outCallForwardInfo.serviceClass = (inCallForwardInfo.has_service_class) ?
            inCallForwardInfo.service_class : INT32_MAX;
    outCallForwardInfo.toa = (inCallForwardInfo.has_toa) ? inCallForwardInfo.toa : INT32_MAX;
    convertProtoToHidlString((const char*)inCallForwardInfo.number.arg,
            outCallForwardInfo.number);
    outCallForwardInfo.timeSeconds = (inCallForwardInfo.has_time_seconds) ?
            inCallForwardInfo.time_seconds : INT32_MAX;

    outCallForwardInfo.hasCallFwdTimerStart = false;
    if (inCallForwardInfo.has_callFwdTimerStart) {
        outCallForwardInfo.hasCallFwdTimerStart = true;
        convertProtoToHidlCallForwardTimerInfo(inCallForwardInfo.callFwdTimerStart,
                outCallForwardInfo.callFwdTimerStart);
    }

    outCallForwardInfo.hasCallFwdTimerEnd = false;
    if (inCallForwardInfo.has_callFwdTimerEnd) {
        outCallForwardInfo.hasCallFwdTimerEnd = true;
        convertProtoToHidlCallForwardTimerInfo(inCallForwardInfo.callFwdTimerEnd,
                outCallForwardInfo.callFwdTimerEnd);
    }
}

void convertProtoToHidlCallForwardTimerInfo(const ims_CallFwdTimerInfo& inCallFwdTimerInfo,
        CallFwdTimerInfo& outCallFwdTimerInfo) {
    outCallFwdTimerInfo.year = (inCallFwdTimerInfo.has_year) ? inCallFwdTimerInfo.year : INT32_MAX;
    outCallFwdTimerInfo.month = (inCallFwdTimerInfo.has_month) ? inCallFwdTimerInfo.month :
            INT32_MAX;
    outCallFwdTimerInfo.day = (inCallFwdTimerInfo.has_day) ? inCallFwdTimerInfo.day : INT32_MAX;
    outCallFwdTimerInfo.hour = (inCallFwdTimerInfo.has_hour) ? inCallFwdTimerInfo.hour : INT32_MAX;
    outCallFwdTimerInfo.minute = (inCallFwdTimerInfo.has_minute) ? inCallFwdTimerInfo.minute :
            INT32_MAX;
    outCallFwdTimerInfo.second = (inCallFwdTimerInfo.has_second) ? inCallFwdTimerInfo.second :
            INT32_MAX;
    outCallFwdTimerInfo.timezone = (inCallFwdTimerInfo.has_timezone) ?
            inCallFwdTimerInfo.timezone : INT32_MAX;
}

void convertProtoToHidlString(const char* inStr, hidl_string& outStr) {
    if (inStr == NULL) {
        return;
    }

    uint32_t size = strlen(inStr);
    outStr = hidl_string(inStr, size);
}

void convertProtoToHidlSuppServiceNotification(
        const ims_SuppSvcNotification& inSuppServiceNotification,
        SuppServiceNotification& outSuppServiceNotification) {
    outSuppServiceNotification.notificationType =
            (inSuppServiceNotification.has_notificationType) ?
            convertProtoToHidlNotificationType(inSuppServiceNotification.notificationType) :
            NotificationType::INVALID;
    outSuppServiceNotification.code = (inSuppServiceNotification.has_code) ?
            inSuppServiceNotification.code : INT32_MAX;
    outSuppServiceNotification.index = (inSuppServiceNotification.has_index) ?
            inSuppServiceNotification.index : INT32_MAX;
    outSuppServiceNotification.type = (inSuppServiceNotification.has_type) ?
            inSuppServiceNotification.type : INT32_MAX;
    convertProtoToHidlString((const char*)inSuppServiceNotification.number.arg,
            outSuppServiceNotification.number);
    outSuppServiceNotification.connId = (inSuppServiceNotification.has_connId) ?
            inSuppServiceNotification.connId : INT32_MAX;
    convertProtoToHidlString((const char*)inSuppServiceNotification.history_info.arg,
            outSuppServiceNotification.historyInfo);
    outSuppServiceNotification.hasHoldTone = inSuppServiceNotification.has_hold_tone;
    if (inSuppServiceNotification.has_hold_tone) {
        outSuppServiceNotification.holdTone = inSuppServiceNotification.hold_tone;
    }
}

void convertProtoToHidlConfig(const ims_ConfigMsg& inConfig, ConfigInfo& outConfig) {
    outConfig.item = (inConfig.has_item) ? convertProtoToHidlConfigItem(inConfig.item) :
            ConfigItem::CONFIG_ITEM_INVALID;
    outConfig.hasBoolValue = inConfig.has_boolValue;
    if (inConfig.has_boolValue) {
        outConfig.boolValue = inConfig.boolValue;
    }
    outConfig.intValue = (inConfig.has_intValue) ? inConfig.intValue : INT32_MAX;
    convertProtoToHidlString((const char*)inConfig.stringValue.arg, outConfig.stringValue);

    outConfig.errorCause = (inConfig.has_errorCause) ?
            convertProtoToHidlErrorCause(inConfig.errorCause) :
            ConfigFailureCause::CONFIG_FAILURE_INVALID;
}

ConfigFailureCause convertProtoToHidlErrorCause(const ims_ConfigFailureCause& inErrorCause) {
    switch (inErrorCause) {
    case ims_ConfigFailureCause_CONFIG_NO_ERR:
        return ConfigFailureCause::CONFIG_NO_ERR;
    case ims_ConfigFailureCause_CONFIG_IMS_NOT_READY:
        return ConfigFailureCause::CONFIG_IMS_NOT_READY;
    case ims_ConfigFailureCause_CONFIG_FILE_NOT_AVAILABLE:
        return ConfigFailureCause::CONFIG_FILE_NOT_AVAILABLE;
    case ims_ConfigFailureCause_CONFIG_READ_FAILED:
        return ConfigFailureCause::CONFIG_READ_FAILED;
    case ims_ConfigFailureCause_CONFIG_WRITE_FAILED:
        return ConfigFailureCause::CONFIG_WRITE_FAILED;
    case ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR:
        return ConfigFailureCause::CONFIG_OTHER_INTERNAL_ERR;
    default:
        return ConfigFailureCause::CONFIG_FAILURE_INVALID;
    }
}

ConfigItem convertProtoToHidlConfigItem(const ims_ConfigItem& inConfigItem) {
    switch (inConfigItem) {
        case ims_ConfigItem_CONFIG_ITEM_NONE:
            return ConfigItem::CONFIG_ITEM_NONE;
        case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET:
            return ConfigItem::CONFIG_ITEM_VOCODER_AMRMODESET;
        case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET:
            return ConfigItem::CONFIG_ITEM_VOCODER_AMRWBMODESET;
        case ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER:
            return ConfigItem::CONFIG_ITEM_SIP_SESSION_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY:
            return ConfigItem::CONFIG_ITEM_MIN_SESSION_EXPIRY;
        case ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER:
            return ConfigItem::CONFIG_ITEM_CANCELLATION_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_T_DELAY:
            return ConfigItem::CONFIG_ITEM_T_DELAY;
        case ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE:
            return ConfigItem::CONFIG_ITEM_SILENT_REDIAL_ENABLE;
        case ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER:
            return ConfigItem::CONFIG_ITEM_SIP_T1_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER:
            return ConfigItem::CONFIG_ITEM_SIP_T2_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER:
            return ConfigItem::CONFIG_ITEM_SIP_TF_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_VLT_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_LVC_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME:
            return ConfigItem::CONFIG_ITEM_DOMAIN_NAME;
        case ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT:
            return ConfigItem::CONFIG_ITEM_SMS_FORMAT;
        case ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP:
            return ConfigItem::CONFIG_ITEM_SMS_OVER_IP;
        case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER:
            return ConfigItem::CONFIG_ITEM_PUBLISH_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
            return ConfigItem::CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
            return ConfigItem::CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
        case ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
            return ConfigItem::CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
            return ConfigItem::CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
        case ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
            return ConfigItem::CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
        case ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
            return ConfigItem::CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
        case ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
            return ConfigItem::CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
        case ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG:
            return ConfigItem::CONFIG_ITEM_GZIP_FLAG;
        case ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_EAB_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED:
            return ConfigItem::CONFIG_ITEM_MOBILE_DATA_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
            return ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
            return ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
        case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
            return ConfigItem::CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
        case ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
            return ConfigItem::CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER:
            return ConfigItem::CONFIG_ITEM_EMERGENCY_CALL_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER:
            return ConfigItem::CONFIG_ITEM_SSAC_HYSTERESIS_TIMER;
        case ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
            return ConfigItem::CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
        case ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS:
            return ConfigItem::CONFIG_ITEM_LBO_PCSCF_ADDRESS;
        case ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED:
            return ConfigItem::CONFIG_ITEM_KEEP_ALIVE_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
            return ConfigItem::CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
        case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
            return ConfigItem::CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
        case ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT:
            return ConfigItem::CONFIG_ITEM_SPEECH_START_PORT;
        case ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT:
            return ConfigItem::CONFIG_ITEM_SPEECH_END_PORT;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
            return ConfigItem::CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
        case ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
            return ConfigItem::CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
        case ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
            return ConfigItem::CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
        case ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
            return ConfigItem::CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
        case ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
            return ConfigItem::CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
        case ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT:
            return ConfigItem::CONFIG_ITEM_DTMF_WB_PT;
        case ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT:
            return ConfigItem::CONFIG_ITEM_DTMF_NB_PT;
        case ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE:
            return ConfigItem::CONFIG_ITEM_AMR_DEFAULT_MODE;
        case ims_ConfigItem_CONFIG_ITEM_SMS_PSI:
            return ConfigItem::CONFIG_ITEM_SMS_PSI;
        case ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY:
            return ConfigItem::CONFIG_ITEM_VIDEO_QUALITY;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_LTE1;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_LTE2;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_LTE3;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_1x;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_WIFI_A;
        case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B:
            return ConfigItem::CONFIG_ITEM_THRESHOLD_WIFI_B;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE:
            return ConfigItem::CONFIG_ITEM_T_EPDG_LTE;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI:
            return ConfigItem::CONFIG_ITEM_T_EPDG_WIFI;
        case ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x:
            return ConfigItem::CONFIG_ITEM_T_EPDG_1x;
        case ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_VWF_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_VCE_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED:
            return ConfigItem::CONFIG_ITEM_RTT_SETTING_ENABLED;
        case ims_ConfigItem_CONFIG_ITEM_SMS_APP:
            return ConfigItem::CONFIG_ITEM_SMS_APP;
        case ims_ConfigItem_CONFIG_ITEM_VVM_APP:
            return ConfigItem::CONFIG_ITEM_VVM_APP;
        default:
            return ConfigItem::CONFIG_ITEM_INVALID;
    }
}

NotificationType convertProtoToHidlNotificationType(ims_NotificationType inType) {
    switch (inType) {
        case ims_NotificationType_MO:
            return NotificationType::MO;
        case ims_NotificationType_MT:
            return NotificationType::MT;
        default:
            return NotificationType::INVALID;
    }
}

CallType convertProtoToHidlCallType(const ims_CallType& callType) {
    switch (callType) {
        case ims_CallType_CALL_TYPE_VOICE:
            return CallType::CALL_TYPE_VOICE;
        case ims_CallType_CALL_TYPE_VT_TX:
            return CallType::CALL_TYPE_VT_TX;
        case ims_CallType_CALL_TYPE_VT_RX:
            return CallType::CALL_TYPE_VT_RX;
        case ims_CallType_CALL_TYPE_VT:
            return CallType::CALL_TYPE_VT;
        case ims_CallType_CALL_TYPE_VT_NODIR:
            return CallType::CALL_TYPE_VT_NODIR;
        case ims_CallType_CALL_TYPE_CS_VS_TX:
            return CallType::CALL_TYPE_CS_VS_TX;
        case ims_CallType_CALL_TYPE_CS_VS_RX:
            return CallType::CALL_TYPE_CS_VS_RX;
        case ims_CallType_CALL_TYPE_PS_VS_TX:
            return CallType::CALL_TYPE_PS_VS_TX;
        case ims_CallType_CALL_TYPE_PS_VS_RX:
            return CallType::CALL_TYPE_PS_VS_RX;
        case ims_CallType_CALL_TYPE_SMS:
            return CallType::CALL_TYPE_SMS;
        case ims_CallType_CALL_TYPE_UT:
            return CallType::CALL_TYPE_UT;
        case ims_CallType_CALL_TYPE_UNKNOWN:
            return CallType::CALL_TYPE_UNKNOWN;
        default:
            return CallType::CALL_TYPE_INVALID;
    }
}

ToneOperation convertProtoToHidlTone(const ims_RingBackTone& inTone) {
    ToneOperation outToneOperation = ToneOperation::INVALID;

    if (inTone.has_flag) {
        outToneOperation = convertProtoToHidlToneOperation(inTone.flag);
    }

    return outToneOperation;
}

HandoverType convertProtoToHidlHandoverType(const ims_Handover_Msg_Type& inType) {
    switch (inType) {
    case ims_Handover_Msg_Type_START:
        return HandoverType::START;
    case ims_Handover_Msg_Type_COMPLETE_SUCCESS:
        return HandoverType::COMPLETE_SUCCESS;
    case ims_Handover_Msg_Type_COMPLETE_FAIL:
        return HandoverType::COMPLETE_FAIL;
    case ims_Handover_Msg_Type_CANCEL:
        return HandoverType::CANCEL;
    case ims_Handover_Msg_Type_NOT_TRIGGERED:
        return HandoverType::NOT_TRIGGERED;
    default:
        return HandoverType::INVALID;
    }
}

ToneOperation convertProtoToHidlToneOperation(const ims_RingBackTone_ToneFlag& flag) {
    switch (flag) {
        case ims_RingBackTone_ToneFlag_STOP:
            return ToneOperation::STOP;
        case ims_RingBackTone_ToneFlag_START:
            return ToneOperation::START;
        default:
            return ToneOperation::INVALID;
    }
}

CallDomain convertProtoToHidlCallDomain(const ims_CallDomain& inCallDomain) {
    switch (inCallDomain) {
        case ims_CallDomain_CALL_DOMAIN_UNKNOWN:
            return CallDomain::CALL_DOMAIN_UNKNOWN;
        case ims_CallDomain_CALL_DOMAIN_CS:
            return CallDomain::CALL_DOMAIN_CS;
        case ims_CallDomain_CALL_DOMAIN_PS:
            return CallDomain::CALL_DOMAIN_PS;
        case ims_CallDomain_CALL_DOMAIN_AUTOMATIC:
            return CallDomain::CALL_DOMAIN_AUTOMATIC;
        case ims_CallDomain_CALL_DOMAIN_NOT_SET:
            return CallDomain::CALL_DOMAIN_NOT_SET;
        default:
            return CallDomain::CALL_DOMAIN_INVALID;
    }
}

CallState convertProtoToHidlCallState(const ims_CallState& inState) {
    switch (inState) {
        case ims_CallState_CALL_ACTIVE:
            return CallState::CALL_ACTIVE;
        case ims_CallState_CALL_HOLDING:
            return CallState::CALL_HOLDING;
        case ims_CallState_CALL_DIALING:
            return CallState::CALL_DIALING;
        case ims_CallState_CALL_ALERTING:
            return CallState::CALL_ALERTING;
        case ims_CallState_CALL_INCOMING:
            return CallState::CALL_INCOMING;
        case ims_CallState_CALL_WAITING:
            return CallState::CALL_WAITING;
        case ims_CallState_CALL_END:
            return CallState::CALL_END;
        default:
            return CallState::CALL_STATE_INVALID;
    }
}

ServiceType convertProtoToHidlServiceType(const ims_SrvType& type) {
    switch (type) {
        case ims_SrvType_SMS:
            return ServiceType::SMS;
        case ims_SrvType_VOIP:
            return ServiceType::VOIP;
        case ims_SrvType_VT:
            return ServiceType::VT;
        default:
            return ServiceType::INVALID;
    }
}

StatusType convertProtoToHidlStatusType(const ims_StatusType& status) {
    switch (status) {
        case ims_StatusType_STATUS_DISABLED:
            return StatusType::STATUS_DISABLED;
        case ims_StatusType_STATUS_PARTIALLY_ENABLED:
            return StatusType::STATUS_PARTIALLY_ENABLED;
        case ims_StatusType_STATUS_ENABLED:
            return StatusType::STATUS_ENABLED;
        case ims_StatusType_STATUS_NOT_SUPPORTED:
            return StatusType::STATUS_NOT_SUPPORTED;
        default:
            return StatusType::STATUS_INVALID;
    }
}

V1_3::CallFailCause convertProtoToHidlCallFailCause(const ims_CallFailCause& failCause) {
    typedef V1_3::CallFailCause FailCause;

    switch (failCause) {
        case ims_CallFailCause_CALL_FAIL_UNOBTAINABLE_NUMBER:
            return FailCause::CALL_FAIL_UNOBTAINABLE_NUMBER;
        case ims_CallFailCause_CALL_FAIL_NORMAL:
            return FailCause::CALL_FAIL_NORMAL;
        case ims_CallFailCause_CALL_FAIL_BUSY:
            return FailCause::CALL_FAIL_BUSY;
        case ims_CallFailCause_CALL_FAIL_NORMAL_UNSPECIFIED:
            return FailCause::CALL_FAIL_NORMAL_UNSPECIFIED;
        case ims_CallFailCause_CALL_FAIL_CONGESTION:
            return FailCause::CALL_FAIL_CONGESTION;
        case ims_CallFailCause_CALL_FAIL_INCOMPATIBILITY_DESTINATION:
            return FailCause::CALL_FAIL_INCOMPATIBILITY_DESTINATION;
        case ims_CallFailCause_CALL_FAIL_CALL_BARRED:
            return FailCause::CALL_FAIL_CALL_BARRED;
        case ims_CallFailCause_CALL_FAIL_FDN_BLOCKED:
            return FailCause::CALL_FAIL_FDN_BLOCKED;
        case ims_CallFailCause_CALL_FAIL_IMEI_NOT_ACCEPTED:
            return FailCause::CALL_FAIL_IMEI_NOT_ACCEPTED;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_USSD:
            return FailCause::CALL_FAIL_DIAL_MODIFIED_TO_USSD;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_SS:
            return FailCause::CALL_FAIL_DIAL_MODIFIED_TO_SS;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL:
            return FailCause::CALL_FAIL_DIAL_MODIFIED_TO_DIAL;
        case ims_CallFailCause_CALL_FAIL_EMERGENCY_TEMP_FAILURE:
            return FailCause::CALL_FAIL_EMERGENCY_TEMP_FAILURE;
        case ims_CallFailCause_CALL_FAIL_EMERGENCY_PERM_FAILURE:
            return FailCause::CALL_FAIL_EMERGENCY_PERM_FAILURE;
        case ims_CallFailCause_CALL_FAIL_HO_NOT_FEASIBLE:
            return FailCause::CALL_FAIL_HO_NOT_FEASIBLE;
        case ims_CallFailCause_CALL_FAIL_USER_BUSY:
            return FailCause::CALL_FAIL_USER_BUSY;
        case ims_CallFailCause_CALL_FAIL_USER_REJECT:
            return FailCause::CALL_FAIL_USER_REJECT;
        case ims_CallFailCause_CALL_FAIL_LOW_BATTERY:
            return FailCause::CALL_FAIL_LOW_BATTERY;
        case ims_CallFailCause_CALL_FAIL_BLACKLISTED_CALL_ID:
            return FailCause::CALL_FAIL_BLACKLISTED_CALL_ID;
        case ims_CallFailCause_CALL_FAIL_CS_RETRY_REQUIRED:
            return FailCause::CALL_FAIL_CS_RETRY_REQUIRED;
        case ims_CallFailCause_CALL_FAIL_NETWORK_UNAVAILABLE:
            return FailCause::CALL_FAIL_NETWORK_UNAVAILABLE;
        case ims_CallFailCause_CALL_FAIL_FEATURE_UNAVAILABLE:
            return FailCause::CALL_FAIL_FEATURE_UNAVAILABLE;
        case ims_CallFailCause_CALL_FAIL_SIP_Error:
            return FailCause::CALL_FAIL_SIP_ERROR;
        case ims_CallFailCause_CALL_FAIL_MISC:
            return FailCause::CALL_FAIL_MISC;
        case ims_CallFailCause_CALL_FAIL_ANSWERED_ELSEWHERE:
            return FailCause::CALL_FAIL_ANSWERED_ELSEWHERE;
        case ims_CallFailCause_CALL_FAIL_PULL_OUT_OF_SYNC:
            return FailCause::CALL_FAIL_PULL_OUT_OF_SYNC;
        case ims_CallFailCause_CALL_FAIL_CAUSE_CALL_PULLED:
            return FailCause::CALL_FAIL_CAUSE_CALL_PULLED;
        case ims_CallFailCause_CALL_FAIL_SIP_REDIRECTED:
            return FailCause::CALL_FAIL_SIP_REDIRECTED;
        case ims_CallFailCause_CALL_FAIL_SIP_BAD_REQUEST:
            return FailCause::CALL_FAIL_SIP_BAD_REQUEST;
        case ims_CallFailCause_CALL_FAIL_SIP_FORBIDDEN:
            return FailCause::CALL_FAIL_SIP_FORBIDDEN;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_FOUND:
            return FailCause::CALL_FAIL_SIP_NOT_FOUND;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_SUPPORTED:
            return FailCause::CALL_FAIL_SIP_NOT_SUPPORTED;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_TIMEOUT:
            return FailCause::CALL_FAIL_SIP_REQUEST_TIMEOUT;
        case ims_CallFailCause_CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE:
            return FailCause::CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE;
        case ims_CallFailCause_CALL_FAIL_SIP_BAD_ADDRESS:
            return FailCause::CALL_FAIL_SIP_BAD_ADDRESS;
        case ims_CallFailCause_CALL_FAIL_SIP_BUSY:
            return FailCause::CALL_FAIL_SIP_BUSY;
        case ims_CallFailCause_CALL_FAIL_SIP_REQUEST_CANCELLED:
            return FailCause::CALL_FAIL_SIP_REQUEST_CANCELLED;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_ACCEPTABLE:
            return FailCause::CALL_FAIL_SIP_NOT_ACCEPTABLE;
        case ims_CallFailCause_CALL_FAIL_SIP_NOT_REACHABLE:
            return FailCause::CALL_FAIL_SIP_NOT_REACHABLE;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_INTERNAL_ERROR:
            return FailCause::CALL_FAIL_SIP_SERVER_INTERNAL_ERROR;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED:
            return FailCause::CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_BAD_GATEWAY:
            return FailCause::CALL_FAIL_SIP_SERVER_BAD_GATEWAY;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVICE_UNAVAILABLE:
            return FailCause::CALL_FAIL_SIP_SERVICE_UNAVAILABLE;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_TIMEOUT:
            return FailCause::CALL_FAIL_SIP_SERVER_TIMEOUT;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED:
            return FailCause::CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE:
            return FailCause::CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE;
        case ims_CallFailCause_CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE:
            return FailCause::CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE;
        case ims_CallFailCause_CALL_FAIL_SIP_USER_REJECTED:
            return FailCause::CALL_FAIL_SIP_USER_REJECTED;
        case ims_CallFailCause_CALL_FAIL_SIP_GLOBAL_ERROR:
            return FailCause::CALL_FAIL_SIP_GLOBAL_ERROR;
        case ims_CallFailCause_CALL_FAIL_MEDIA_INIT_FAILED:
            return FailCause::CALL_FAIL_MEDIA_INIT_FAILED;
        case ims_CallFailCause_CALL_FAIL_MEDIA_NO_DATA:
            return FailCause::CALL_FAIL_MEDIA_NO_DATA;
        case ims_CallFailCause_CALL_FAIL_MEDIA_NOT_ACCEPTABLE:
            return FailCause::CALL_FAIL_MEDIA_NOT_ACCEPTABLE;
        case ims_CallFailCause_CALL_FAIL_MEDIA_UNSPECIFIED_ERROR:
            return FailCause::CALL_FAIL_MEDIA_UNSPECIFIED_ERROR;
        case ims_CallFailCause_CALL_FAIL_HOLD_RESUME_FAILED:
            return FailCause::CALL_FAIL_HOLD_RESUME_FAILED;
        case ims_CallFailCause_CALL_FAIL_HOLD_RESUME_CANCELED:
            return FailCause::CALL_FAIL_HOLD_RESUME_CANCELED;
        case ims_CallFailCause_CALL_FAIL_HOLD_REINVITE_COLLISION:
            return FailCause::CALL_FAIL_HOLD_REINVITE_COLLISION;
        case ims_CallFailCause_CALL_FAIL_ACCESS_CLASS_BLOCKED:
            return FailCause::CALL_FAIL_ACCESS_CLASS_BLOCKED;
        case ims_CallFailCause_CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO:
            return FailCause::CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL:
            return FailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO:
            return FailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS:
            return FailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS;
        case ims_CallFailCause_CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD:
            return FailCause::CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD;
        case ims_CallFailCause_CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL:
            return FailCause::CALL_FAIL_SIP_ALTERNATE_EMERGENCY_CALL;
        case ims_CallFailCause_CALL_FAIL_RADIO_OFF:
            return FailCause::CALL_FAIL_RADIO_OFF;
        case ims_CallFailCause_CALL_FAIL_OUT_OF_SERVICE:
            return FailCause::CALL_FAIL_OUT_OF_SERVICE;
        case ims_CallFailCause_CALL_FAIL_NO_VALID_SIM:
            return FailCause::CALL_FAIL_NO_VALID_SIM;
        case ims_CallFailCause_CALL_FAIL_RADIO_INTERNAL_ERROR:
            return FailCause::CALL_FAIL_RADIO_INTERNAL_ERROR;
        case ims_CallFailCause_CALL_FAIL_NETWORK_RESP_TIMEOUT:
            return FailCause::CALL_FAIL_NETWORK_RESP_TIMEOUT;
        case ims_CallFailCause_CALL_FAIL_NETWORK_REJECT:
            return FailCause::CALL_FAIL_NETWORK_REJECT;
        case ims_CallFailCause_CALL_FAIL_RADIO_ACCESS_FAILURE:
            return FailCause::CALL_FAIL_RADIO_ACCESS_FAILURE;
        case ims_CallFailCause_CALL_FAIL_RADIO_LINK_FAILURE:
            return FailCause::CALL_FAIL_RADIO_LINK_FAILURE;
        case ims_CallFailCause_CALL_FAIL_RADIO_LINK_LOST:
            return FailCause::CALL_FAIL_RADIO_LINK_LOST;
        case ims_CallFailCause_CALL_FAIL_RADIO_UPLINK_FAILURE:
            return FailCause::CALL_FAIL_RADIO_UPLINK_FAILURE;
        case ims_CallFailCause_CALL_FAIL_RADIO_SETUP_FAILURE:
            return FailCause::CALL_FAIL_RADIO_SETUP_FAILURE;
        case ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_NORMAL:
            return FailCause::CALL_FAIL_RADIO_RELEASE_NORMAL;
        case ims_CallFailCause_CALL_FAIL_RADIO_RELEASE_ABNORMAL:
            return FailCause::CALL_FAIL_RADIO_RELEASE_ABNORMAL;
        case ims_CallFailCause_CALL_FAIL_NETWORK_DETACH:
            return FailCause::CALL_FAIL_NETWORK_DETACH;
        case ims_CallFailCause_CALL_FAIL_ERROR_UNSPECIFIED:
            return FailCause::CALL_FAIL_ERROR_UNSPECIFIED;
        case ims_CallFailCause_CALL_FAIL_NO_CSFB_IN_CS_ROAM:
            return FailCause::CALL_FAIL_NO_CSFB_IN_CS_ROAM;
        case ims_CallFailCause_CALL_FAIL_SRV_NOT_REGISTERED:
            return FailCause::CALL_FAIL_SRV_NOT_REGISTERED;
        case ims_CallFailCause_CALL_FAIL_CALL_TYPE_NOT_ALLOWED:
            return FailCause::CALL_FAIL_CALL_TYPE_NOT_ALLOWED;
        case ims_CallFailCause_CALL_FAIL_EMRG_CALL_ONGOING:
            return FailCause::CALL_FAIL_EMRG_CALL_ONGOING;
        case ims_CallFailCause_CALL_FAIL_CALL_SETUP_ONGOING:
            return FailCause::CALL_FAIL_CALL_SETUP_ONGOING;
        case ims_CallFailCause_CALL_FAIL_MAX_CALL_LIMIT_REACHED:
            return FailCause::CALL_FAIL_MAX_CALL_LIMIT_REACHED;
        case ims_CallFailCause_CALL_FAIL_UNSUPPORTED_SIP_HDRS:
            return FailCause::CALL_FAIL_UNSUPPORTED_SIP_HDRS;
        case ims_CallFailCause_CALL_FAIL_CALL_TRANSFER_ONGOING:
            return FailCause::CALL_FAIL_CALL_TRANSFER_ONGOING;
        case ims_CallFailCause_CALL_FAIL_PRACK_TIMEOUT:
            return FailCause::CALL_FAIL_PRACK_TIMEOUT;
        case ims_CallFailCause_CALL_FAIL_QOS_FAILURE:
            return FailCause::CALL_FAIL_QOS_FAILURE;
        case ims_CallFailCause_CALL_FAIL_ONGOING_HANDOVER:
            return FailCause::CALL_FAIL_ONGOING_HANDOVER;
        case ims_CallFailCause_CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED:
            return FailCause::CALL_FAIL_VT_WITH_TTY_NOT_ALLOWED;
        case ims_CallFailCause_CALL_FAIL_CALL_UPGRADE_ONGOING:
            return FailCause::CALL_FAIL_CALL_UPGRADE_ONGOING;
        case ims_CallFailCause_CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED:
            return FailCause::CALL_FAIL_CONFERENCE_WITH_TTY_NOT_ALLOWED;
        case ims_CallFailCause_CALL_FAIL_CALL_CONFERENCE_ONGOING:
            return FailCause::CALL_FAIL_CALL_CONFERENCE_ONGOING;
        case ims_CallFailCause_CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED:
            return FailCause::CALL_FAIL_VT_WITH_AVPF_NOT_ALLOWED;
        case ims_CallFailCause_CALL_FAIL_ENCRYPTION_CALL_ONGOING:
            return FailCause::CALL_FAIL_ENCRYPTION_CALL_ONGOING;
        case ims_CallFailCause_CALL_FAIL_CALL_ONGOING_CW_DISABLED:
            return FailCause::CALL_FAIL_CALL_ONGOING_CW_DISABLED;
        case ims_CallFailCause_CALL_FAIL_CALL_ON_OTHER_SUB:
            return FailCause::CALL_FAIL_CALL_ON_OTHER_SUB;
        case ims_CallFailCause_CALL_FAIL_1X_COLLISION:
            return FailCause::CALL_FAIL_1X_COLLISION;
        case ims_CallFailCause_CALL_FAIL_UI_NOT_READY:
            return FailCause::CALL_FAIL_UI_NOT_READY;
        case ims_CallFailCause_CALL_FAIL_CS_CALL_ONGOING:
            return FailCause::CALL_FAIL_CS_CALL_ONGOING;
        default:
            return FailCause::CALL_FAIL_INVALID;
    }
}

RadioState convertProtoToHidlRadioState(const ims_RadioStateChanged& inState) {
    if (inState.has_state) {
        switch (inState.state) {
            case ims_RadioState_RADIO_STATE_OFF:
                return RadioState::RADIO_STATE_OFF;
            case ims_RadioState_RADIO_STATE_ON:
                return RadioState::RADIO_STATE_ON;
            case ims_RadioState_RADIO_STATE_UNAVAILABLE:
                return RadioState::RADIO_STATE_UNAVAILABLE;
            default:
                return RadioState::RADIO_STATE_INVALID;
        }
    }

    return RadioState::RADIO_STATE_INVALID;
}

ims_Tty_Mode_Type convertHidlToProtoTtyMode(TtyMode ttyMode) {
    switch (ttyMode) {
        case TtyMode::TTY_MODE_FULL:
            return ims_Tty_Mode_Type_TTY_MODE_FULL;
        case TtyMode::TTY_MODE_HCO:
            return ims_Tty_Mode_Type_TTY_MODE_HCO;
        case TtyMode::TTY_MODE_VCO:
            return ims_Tty_Mode_Type_TTY_MODE_VCO;
        case TtyMode::TTY_MODE_OFF:
        default:
            return ims_Tty_Mode_Type_TTY_MODE_OFF;
    }
}

TtyMode convertProtoToHidlTtyMode(ims_Tty_Mode_Type ttyMode) {
    switch (ttyMode) {
        case ims_Tty_Mode_Type_TTY_MODE_FULL:
            return TtyMode::TTY_MODE_FULL;
        case ims_Tty_Mode_Type_TTY_MODE_HCO:
            return TtyMode::TTY_MODE_HCO;
        case ims_Tty_Mode_Type_TTY_MODE_VCO:
            return TtyMode::TTY_MODE_VCO;
        case ims_Tty_Mode_Type_TTY_MODE_OFF:
            return TtyMode::TTY_MODE_OFF;
        default:
            return TtyMode::TTY_MODE_INVALID;
    }
}

ClipStatus convertProtoToHidlClipStatus(ims_ClipStatus clipStatus) {
    switch (clipStatus) {
        case ims_ClipStatus_NOT_PROVISIONED:
            return ClipStatus::NOT_PROVISIONED;
        case ims_ClipStatus_PROVISIONED:
            return ClipStatus::PROVISIONED;
        case ims_ClipStatus_STATUS_UNKNOWN:
            return ClipStatus::STATUS_UNKNOWN;
        default:
            return ClipStatus::INVALID;
    }
}

ServiceClassStatus convertProtoToHidlServiceClassStatus(ims_ServiceClassStatus status) {
    switch (status) {
        case ims_ServiceClassStatus_ENABLED:
            return ServiceClassStatus::ENABLED;
        case ims_ServiceClassStatus_DISABLED:
            return ServiceClassStatus::DISABLED;
        default:
            return ServiceClassStatus::INVALID;
    }
}

V1_3::ServiceClassProvisionStatus convertProtoToHidlServiceClassProvisionStatus(
        ims_ServiceClassProvisionStatus provisionStatus) {
    switch (provisionStatus) {
        case ims_ServiceClassProvisionStatus_NOT_PROVISIONED:
            return V1_3::ServiceClassProvisionStatus::NOT_PROVISIONED;
        case ims_ServiceClassProvisionStatus_PROVISIONED:
            return V1_3::ServiceClassProvisionStatus::PROVISIONED;
        default:
            return V1_3::ServiceClassProvisionStatus::INVALID;
    }
}


ims_Error convertHidlToProtoEctInfo(const ExplicitCallTransferInfo &inEctRequest,
        ims_ExplicitCallTransfer &outEctRequest) {
    ims_Error ret = ims_Error_E_SUCCESS;

    do {
        if (inEctRequest.callId != INT32_MAX) {
            outEctRequest.has_callId = true;
            outEctRequest.callId = inEctRequest.callId;
        }

        outEctRequest.has_ectType = convertHidlToProtoEctType(inEctRequest.ectType,
                outEctRequest.ectType);

        if (!inEctRequest.targetAddress.empty()) {
            const uint32_t targetAddressLen = inEctRequest.targetAddress.size();
            size_t destSize = sizeof(char) * (targetAddressLen + 1);
            outEctRequest.targetAddress.arg = (char *) qcril_malloc(destSize);
            if (outEctRequest.targetAddress.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for ECT targetAddress.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
            strlcpy((char *)outEctRequest.targetAddress.arg,
                    inEctRequest.targetAddress.c_str(), destSize);
            break;
        }
        if (inEctRequest.targetCallId != INT32_MAX) {
            outEctRequest.has_targetCallId = true;
            outEctRequest.targetCallId = inEctRequest.targetCallId;
            break;
        }
        ret = ims_Error_E_GENERIC_FAILURE;
    } while(0);

    if (isError(ret)) {
        qcril_free(outEctRequest.targetAddress.arg);
        outEctRequest.targetAddress.arg = nullptr;
    }

    return ret;
}

bool convertHidlToProtoEctType(const EctType &in, ims_EctType &out) {
    bool ret = true;
    switch(in) {
        case EctType::BLIND_TRANSFER:
            out = ims_EctType_BLIND_TRANSFER;
            break;
        case EctType::ASSURED_TRANSFER:
            out = ims_EctType_ASSURED_TRANSFER;
            break;
        case EctType::CONSULTATIVE_TRANSFER:
            out = ims_EctType_CONSULTATIVE_TRANSFER;
            break;
        case EctType::INVALID:
        default:
            ret = false;
            break;
    }
    return ret;
}

ims_Error convertHidlToProtoCallModifyInfo(const CallModifyInfo &in, ims_CallModify &out)
{
  ims_Error ret = ims_Error_E_SUCCESS;

  do {
    out.has_callIndex = TRUE;
    out.callIndex = in.callIndex;

    if (in.hasCallDetails) {
      if ((ret = convertHidlToProtoCallDetails(in.callDetails, out.callDetails)) !=
        ims_Error_E_SUCCESS) {
        break;
      }
    }
    out.has_callDetails = true;

    out.has_error = convertHidlToProtoCallModifyFailCause(in.failCause, out.error);
  } while(0);

  return ret;
}

bool convertProtoToHidlCallModifyInfo(const ims_CallModify &in, CallModifyInfo &out)
{
  if (in.has_callIndex) {
    out.callIndex = in.callIndex;
  }

  if (in.has_callDetails) {
    out.hasCallDetails = true;
    convertProtoToHidlCallDetails(in.callDetails, out.callDetails);
  }

  if (in.has_error) {
    convertProtoToHidlCallModifyFailCause(in.error, out.failCause);
  } else {
    out.failCause = CallModifyFailCause::E_INVALID;
  }

  return true;
}

bool convertHidlToProtoCallModifyFailCause(const CallModifyFailCause &in, ims_Error &out)
{
  bool ret = true;
  switch(in)
  {
    case CallModifyFailCause::E_SUCCESS:
      out = ims_Error_E_SUCCESS;
      break;
    case CallModifyFailCause::E_RADIO_NOT_AVAILABLE:
      out = ims_Error_E_RADIO_NOT_AVAILABLE;
      break;
    case CallModifyFailCause::E_GENERIC_FAILURE:
      out = ims_Error_E_GENERIC_FAILURE;
      break;
    case CallModifyFailCause::E_REQUEST_NOT_SUPPORTED:
      out = ims_Error_E_REQUEST_NOT_SUPPORTED;
      break;
    case CallModifyFailCause::E_CANCELLED:
      out = ims_Error_E_CANCELLED;
      break;
    case CallModifyFailCause::E_UNUSED:
      out = ims_Error_E_UNUSED;
      break;
    case CallModifyFailCause::E_INVALID_PARAMETER:
      out = ims_Error_E_INVALID_PARAMETER;
      break;
    case CallModifyFailCause::E_REJECTED_BY_REMOTE:
      out = ims_Error_E_REJECTED_BY_REMOTE;
      break;
    case CallModifyFailCause::E_IMS_DEREGISTERED:
      out = ims_Error_E_IMS_DEREGISTERED;
      break;
    case CallModifyFailCause::E_NETWORK_NOT_SUPPORTED:
      out = ims_Error_E_NETWORK_NOT_SUPPORTED;
      break;
    case CallModifyFailCause::E_HOLD_RESUME_FAILED:
      out = ims_Error_E_HOLD_RESUME_FAILED;
      break;
    case CallModifyFailCause::E_HOLD_RESUME_CANCELED:
      out = ims_Error_E_HOLD_RESUME_CANCELED;
      break;
    case CallModifyFailCause::E_REINVITE_COLLISION:
      out = ims_Error_E_REINVITE_COLLISION;
      break;
    case CallModifyFailCause::E_INVALID:
    default:
      ret = false;
      break;
  }
  return ret;
}

bool convertProtoToHidlCallModifyFailCause(const ims_Error &in, CallModifyFailCause &out)
{
  bool ret = true;
  switch(in)
  {
    case ims_Error_E_SUCCESS:
      out = CallModifyFailCause::E_SUCCESS;
      break;
    case ims_Error_E_RADIO_NOT_AVAILABLE:
      out = CallModifyFailCause::E_RADIO_NOT_AVAILABLE;
      break;
    case ims_Error_E_GENERIC_FAILURE:
      out = CallModifyFailCause::E_GENERIC_FAILURE;
      break;
    case ims_Error_E_REQUEST_NOT_SUPPORTED:
      out = CallModifyFailCause::E_REQUEST_NOT_SUPPORTED;
      break;
    case ims_Error_E_CANCELLED:
      out = CallModifyFailCause::E_CANCELLED;
      break;
    case ims_Error_E_UNUSED:
      out = CallModifyFailCause::E_UNUSED;
      break;
    case ims_Error_E_INVALID_PARAMETER:
      out = CallModifyFailCause::E_INVALID_PARAMETER;
      break;
    case ims_Error_E_REJECTED_BY_REMOTE:
      out = CallModifyFailCause::E_REJECTED_BY_REMOTE;
      break;
    case ims_Error_E_IMS_DEREGISTERED:
      out = CallModifyFailCause::E_IMS_DEREGISTERED;
      break;
    case ims_Error_E_NETWORK_NOT_SUPPORTED:
      out = CallModifyFailCause::E_NETWORK_NOT_SUPPORTED;
      break;
    case ims_Error_E_HOLD_RESUME_FAILED:
      out = CallModifyFailCause::E_HOLD_RESUME_FAILED;
      break;
    case ims_Error_E_HOLD_RESUME_CANCELED:
      out = CallModifyFailCause::E_HOLD_RESUME_CANCELED;
      break;
    case ims_Error_E_REINVITE_COLLISION:
      out = CallModifyFailCause::E_REINVITE_COLLISION;
      break;
    default:
      out = CallModifyFailCause::E_INVALID;
      break;
  }
  return ret;
}

ims_Error convertHidlToProtoDeflectRequestInfo(const DeflectRequestInfo &inDeflectRequest,
        ims_DeflectCall &outDeflectRequest) {
    ims_Error ret = ims_Error_E_SUCCESS;

    do {
        if (inDeflectRequest.connIndex != INT32_MAX) {
            outDeflectRequest.has_conn_index = true;
            outDeflectRequest.conn_index = inDeflectRequest.connIndex;
        }

        if (!inDeflectRequest.number.empty()) {
            const uint32_t numberLen = inDeflectRequest.number.size();
            size_t destSize = sizeof(char) * (numberLen + 1);
            outDeflectRequest.number.arg = (char *) qcril_malloc(destSize);
            if (outDeflectRequest.number.arg == nullptr) {
                QCRIL_LOG_ERROR("Failed to allocate memory for deflect call number");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }
            strlcpy((char *)outDeflectRequest.number.arg, inDeflectRequest.number.c_str(),
                    destSize);
        } else {
            QCRIL_LOG_ERROR("Deflect request number is empty");
            ret = ims_Error_E_GENERIC_FAILURE;
        }
    } while(0);

    if (isError(ret)) {
        qcril_free(outDeflectRequest.number.arg);
        outDeflectRequest.number.arg = nullptr;
    }

    return ret;
}

/*
 *   @brief
 *   Utility to create the message Tag based on the token, type, message id
 *   and error code
 *
 *   @return
 *   ims_MsgTag - The tag for this request to be sent back to the client
 */
void imsRadioGetTag(int32_t token, ims_MsgType type, ims_MsgId msgid, ims_Error error,
        ims_MsgTag& msgTag) {
    memset(&msgTag, 0x00, sizeof(ims_MsgTag));
    msgTag.token = token;
    msgTag.type = type;
    msgTag.id = msgid;
    msgTag.error = error;
}

void imsRadioGetTag(int32_t token, ims_MsgId id, ims_MsgTag& msgTag) {
    imsRadioGetTag(token, ims_MsgType_REQUEST, id, ims_Error_E_SUCCESS, msgTag);
}

void convertProtoToHidlClipProvisionStatus(const ims_ClipProvisionStatus& inClipProvisionStatus,
    ClipProvisionStatus& outClipProvisionStatus) {
    outClipProvisionStatus.clipStatus = (inClipProvisionStatus.has_clip_status) ?
            convertProtoToHidlClipStatus(inClipProvisionStatus.clip_status) : ClipStatus::INVALID;
    if (inClipProvisionStatus.has_errorDetails) {
        outClipProvisionStatus.hasErrorDetails = (ims_Error_E_SUCCESS ==
                convertProtoToHidlSipErrorInfo(inClipProvisionStatus.errorDetails,
                outClipProvisionStatus.errorDetails));
    }
}

bool convertProtoToHidlClirInfo(const ims_Clir& inClir, ClirInfo& outClir) {
    outClir.paramM = (inClir.has_param_m) ?
        inClir.param_m : INT32_MAX;
    outClir.paramN = (inClir.has_param_n) ?
        inClir.param_n : INT32_MAX;
    return (inClir.has_param_m || inClir.has_param_n);
}

void convertProtoToHidlColrInfo(const ims_Colr& inColr, V1_3::ColrInfo& outColr) {
    outColr.status = (inColr.has_status) ?
        convertProtoToHidlServiceClassStatus(inColr.status) :
                ServiceClassStatus::INVALID;
    outColr.provisionStatus = (inColr.has_provisionStatus) ?
        convertProtoToHidlServiceClassProvisionStatus(inColr.provisionStatus):
                V1_3::ServiceClassProvisionStatus::INVALID;
    outColr.presentation = (inColr.has_presentation) ?
        convertProtoToHidlPresentation(inColr.presentation) :
                IpPresentation::IP_PRESENTATION_INVALID;
    if (inColr.has_errorDetails) {
        outColr.hasErrorDetails = (ims_Error_E_SUCCESS ==
                convertProtoToHidlSipErrorInfo(inColr.errorDetails,
                outColr.errorDetails));
    }
}

void convertColrInfoV13ToV10(const V1_3::ColrInfo& colrInfoV13,
    V1_0::ColrInfo& colrInfoV10) {
    colrInfoV10.presentation = colrInfoV13.presentation;
    colrInfoV10.hasErrorDetails = colrInfoV13.hasErrorDetails;
    colrInfoV10.errorDetails.errorCode = colrInfoV13.errorDetails.errorCode;
    colrInfoV10.errorDetails.errorString.setToExternal(
            colrInfoV13.errorDetails.errorString.c_str(),
            colrInfoV13.errorDetails.errorString.size());
}

ims_Error convertHidlToProtoDtmf(const DtmfInfo& inDtmf, ims_Dtmf& outDtmf) {
    if (!inDtmf.dtmf.empty()) {
        int dtmfSize = inDtmf.dtmf.size() + 1;
        outDtmf.dtmf.arg = (char *) qcril_malloc(sizeof(char) * dtmfSize);
        if (outDtmf.dtmf.arg == nullptr) {
            return ims_Error_E_GENERIC_FAILURE;
        }
        strlcpy((char *)outDtmf.dtmf.arg, inDtmf.dtmf.c_str(), dtmfSize);
        return ims_Error_E_SUCCESS;
    }
    return ims_Error_E_GENERIC_FAILURE;
}

void convertProtoToHidlBinaryData(const qcril_binary_data_type* inBinaryData,
    hidl_vec<uint8_t>& outBinaryData) {
    if (inBinaryData != nullptr && inBinaryData->data != nullptr) {
        outBinaryData.setToExternal(inBinaryData->data, inBinaryData->len);
    }
}

ims_Error convertHidlToProtoBinaryData(const hidl_vec<uint8_t>& inBinaryData,
        qcril_binary_data_type& outBinaryData) {
    const uint32_t inBinaryDataLen = inBinaryData.size();

    if (inBinaryDataLen > 0) {
        outBinaryData.len = inBinaryDataLen;
        outBinaryData.data = (uint8_t*)qcril_malloc(inBinaryDataLen);

        if (outBinaryData.data == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory for outBinaryData->data.");
            return ims_Error_E_GENERIC_FAILURE;
        }
        memcpy(outBinaryData.data, inBinaryData.data(), inBinaryDataLen);
    }
    return ims_Error_E_SUCCESS;
}

ConferenceCallState convertProtoToHidlConferenceCallState(
    ims_ConfCallState conferenceCallState) {
    switch (conferenceCallState) {
        case ims_ConfCallState_RINGING:
            return ConferenceCallState::RINGING;
        case ims_ConfCallState_FOREGROUND:
            return ConferenceCallState::FOREGROUND;
        case ims_ConfCallState_BACKGROUND:
            return ConferenceCallState::BACKGROUND;
        default:
            return ConferenceCallState::INVALID;
    }
}

/*
 *   @brief
 *   Utility to get the size of an array of type T.
 *
 */
template <typename T>
uint32_t getCount(T** ptr) {


    if (ptr == nullptr) {
        return 0;
    }

    uint32_t count = 0;

    while(*ptr) {
        count++;
        ptr++;
    }

    return count;
}

bool isError(ims_Error ret) {
    return (ret != ims_Error_E_SUCCESS);
}

FacilityType convertProtoToHidlFacilityType(int facilityType) {
     switch (facilityType) {
        case ims_SuppSvcFacilityType_FACILITY_CLIP:
            return FacilityType::FACILITY_CLIP;
        case ims_SuppSvcFacilityType_FACILITY_COLP:
            return FacilityType::FACILITY_COLP;
        case ims_SuppSvcFacilityType_FACILITY_BAOC:
            return FacilityType::FACILITY_BAOC;
        case ims_SuppSvcFacilityType_FACILITY_BAOIC:
            return FacilityType::FACILITY_BAOIC;
        case ims_SuppSvcFacilityType_FACILITY_BAOICxH:
            return FacilityType::FACILITY_BAOICxH;
        case ims_SuppSvcFacilityType_FACILITY_BAIC:
            return FacilityType::FACILITY_BAIC;
        case ims_SuppSvcFacilityType_FACILITY_BAICr:
            return FacilityType::FACILITY_BAICr;
        case ims_SuppSvcFacilityType_FACILITY_BA_ALL:
            return FacilityType::FACILITY_BA_ALL;
        case ims_SuppSvcFacilityType_FACILITY_BA_MO:
            return FacilityType::FACILITY_BA_MO;
        case ims_SuppSvcFacilityType_FACILITY_BA_MT:
            return FacilityType::FACILITY_BA_MT;
        case ims_SuppSvcFacilityType_FACILITY_BS_MT:
            return FacilityType::FACILITY_BS_MT;
        case ims_SuppSvcFacilityType_FACILITY_BAICa:
            return FacilityType::FACILITY_BAICa;
        default:
            return FacilityType::FACILITY_INVALID;
    }
}

void convertProtoToHidlSuppServiceStatus(const ims_SuppSvcResponse& inSuppSvcResponse,
    V1_3::SuppServiceStatus& outSuppServiceStatus) {

    outSuppServiceStatus.status = (inSuppSvcResponse.has_status) ?
            convertProtoToHidlServiceClassStatus(inSuppSvcResponse.status) :
                    ServiceClassStatus::INVALID;
    outSuppServiceStatus.provisionStatus = (inSuppSvcResponse.has_provisionStatus) ?
            convertProtoToHidlServiceClassProvisionStatus(inSuppSvcResponse.provisionStatus) :
                    V1_3::ServiceClassProvisionStatus::INVALID;

    outSuppServiceStatus.facilityType = (inSuppSvcResponse.has_facilityType) ?
            convertProtoToHidlFacilityType(inSuppSvcResponse.facilityType) :
                    FacilityType::FACILITY_INVALID;

    if (inSuppSvcResponse.failureCause.arg != nullptr) {
        outSuppServiceStatus.failureCause = hidl_string(
                (char *) inSuppSvcResponse.failureCause.arg);
    }

    const ims_CbNumListType** ptr = (const ims_CbNumListType**)
            inSuppSvcResponse.cbNumListType.arg;
    if (ptr != nullptr) {
        uint32_t size = getCount(ptr);
        outSuppServiceStatus.cbNumListInfo.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            outSuppServiceStatus.cbNumListInfo[i] = {};
            convertProtoToHidlNumListInfo(*(ptr[i]), outSuppServiceStatus.cbNumListInfo[i]);
        }
    }

    if (inSuppSvcResponse.has_errorDetails) {
        outSuppServiceStatus.hasErrorDetails = (ims_Error_E_SUCCESS ==
                convertProtoToHidlSipErrorInfo(inSuppSvcResponse.errorDetails,
                outSuppServiceStatus.errorDetails));
    }
}

void convertSuppServiceStatusV13ToV10(const V1_3::SuppServiceStatus& suppServiceStatusV13,
    V1_0::SuppServiceStatus& suppServiceStatusV10) {
    suppServiceStatusV10.status = suppServiceStatusV13.status;
    suppServiceStatusV10.facilityType = suppServiceStatusV13.facilityType;
    suppServiceStatusV10.failureCause = suppServiceStatusV13.failureCause;

    uint32_t count = suppServiceStatusV13.cbNumListInfo.size();
    suppServiceStatusV10.cbNumListInfo.resize(count);
    for (uint32_t i = 0; i < count; i++) {
        suppServiceStatusV10.cbNumListInfo[i] = {};
        auto &in = suppServiceStatusV13.cbNumListInfo[i];
        auto &out = suppServiceStatusV10.cbNumListInfo[i];
        out.serviceClass = in.serviceClass;
        uint32_t num_count = in.cbNumInfo.size();
        out.cbNumInfo.resize(num_count);
        for (uint32_t j = 0; j < num_count; j++) {
            out.cbNumInfo[j] = {};
            out.cbNumInfo[j].status = in.cbNumInfo[j].status;
            out.cbNumInfo[j].number = in.cbNumInfo[j].number;
        }
    }

    suppServiceStatusV10.hasErrorDetails = suppServiceStatusV13.hasErrorDetails;
    suppServiceStatusV10.errorDetails.errorCode =
            suppServiceStatusV13.errorDetails.errorCode;
    suppServiceStatusV10.errorDetails.errorString =
            suppServiceStatusV13.errorDetails.errorString;
}

void convertProtoToHidlNumListInfo(const ims_CbNumListType& inCbNumListType,
    CbNumListInfo& outCbNumListInfo) {
    outCbNumListInfo.serviceClass = (inCbNumListType.has_serviceClass
        && inCbNumListType.serviceClass.has_service_Class) ?
            inCbNumListType.serviceClass.service_Class : INT32_MAX;

    const ims_CbNumList** ptr = (const ims_CbNumList**) inCbNumListType.cb_num_list.arg;
    if (ptr != nullptr) {
        uint32_t size = getCount(ptr);
        outCbNumListInfo.cbNumInfo.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            outCbNumListInfo.cbNumInfo[i] = {};
            convertProtoToHidlNumInfo(*(ptr[i]), outCbNumListInfo.cbNumInfo[i]);
        }
    }
}

void convertProtoToHidlNumInfo(const ims_CbNumList& inCbNumList, CbNumInfo& outCbNumInfo) {
    outCbNumInfo.status = inCbNumList.has_status ?
        convertProtoToHidlServiceClassStatus(inCbNumList.status): ServiceClassStatus::INVALID;
    if (inCbNumList.number.arg != nullptr) {
        outCbNumInfo.number = hidl_string((char *) inCbNumList.number.arg);
    }
}

MessageType convertProtoToHidlMwiMessageType(ims_MwiMessageType mwiMessageType) {
     switch (mwiMessageType) {
        case ims_MwiMessageType_MWI_MSG_NONE:
            return MessageType::MWI_MSG_NONE;
        case ims_MwiMessageType_MWI_MSG_VOICE:
            return MessageType::MWI_MSG_VOICE;
        case ims_MwiMessageType_MWI_MSG_VIDEO:
            return MessageType::MWI_MSG_VIDEO;
        case ims_MwiMessageType_MWI_MSG_FAX:
            return MessageType::MWI_MSG_FAX;
        case ims_MwiMessageType_MWI_MSG_PAGER:
            return MessageType::MWI_MSG_PAGER;
        case ims_MwiMessageType_MWI_MSG_MULTIMEDIA:
            return MessageType::MWI_MSG_MULTIMEDIA;
        case ims_MwiMessageType_MWI_MSG_TEXT:
            return MessageType::MWI_MSG_TEXT;
        default:
            return MessageType::MWI_MSG_INVALID;
    }
}

MessagePriority convertProtoToHidlMwiMessagePriority(ims_MwiPriority mwiPriority) {
     switch (mwiPriority) {
        case ims_MwiPriority_MWI_MSG_PRIORITY_UNKNOWN:
            return MessagePriority::MWI_MSG_PRIORITY_UNKNOWN;
        case ims_MwiPriority_MWI_MSG_PRIORITY_LOW:
            return MessagePriority::MWI_MSG_PRIORITY_LOW;
        case ims_MwiPriority_MWI_MSG_PRIORITY_NORMAL:
            return MessagePriority::MWI_MSG_PRIORITY_NORMAL;
        case ims_MwiPriority_MWI_MSG_PRIORITY_URGENT:
            return MessagePriority::MWI_MSG_PRIORITY_URGENT;
        default:
            return MessagePriority::MWI_MSG_PRIORITY_INVALID;
    }
}

void convertProtoToHidlMessageWaitingIndication(const ims_Mwi& mwi,
            MessageWaitingIndication& messageWaitingIndication) {
    const ims_MwiMessageSummary** summary = (const ims_MwiMessageSummary**)
            mwi.mwiMsgSummary.arg;
    if (summary != nullptr) {
        uint32_t size = getCount(summary);
        messageWaitingIndication.messageSummary.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            messageWaitingIndication.messageSummary[i] = {};
            convertProtoToHidlMessageSummary(*summary[i],
                    messageWaitingIndication.messageSummary[i]);
        }
    }

    if (mwi.ueAddress.arg != nullptr) {
        messageWaitingIndication.ueAddress = hidl_string((char *) mwi.ueAddress.arg);
    }

    const ims_MwiMessageDetails** details = (const ims_MwiMessageDetails**) mwi.mwiMsgDetail.arg;
    if (details != nullptr) {
        uint32_t size = getCount(details);
        messageWaitingIndication.messageDetails.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            messageWaitingIndication.messageDetails[i] = {};
            convertProtoToHidlMessageDetails(*details[i],
                    messageWaitingIndication.messageDetails[i]);
        }
    }
}

void convertProtoToHidlMessageSummary(const ims_MwiMessageSummary& mwiMessageSummary,
            MessageSummary& messageSummary) {
    messageSummary.type = (mwiMessageSummary.has_messageType) ?
            convertProtoToHidlMwiMessageType(mwiMessageSummary.messageType) :
            MessageType::MWI_MSG_INVALID;

    messageSummary.newMessageCount = (mwiMessageSummary.has_newMessage) ?
            mwiMessageSummary.newMessage : INT32_MAX;
    messageSummary.oldMessageCount = (mwiMessageSummary.has_oldMessage) ?
            mwiMessageSummary.oldMessage : INT32_MAX;
    messageSummary.newUrgentMessageCount = (mwiMessageSummary.has_newUrgent) ?
            mwiMessageSummary.newUrgent : INT32_MAX;
    messageSummary.oldUrgentMessageCount = (mwiMessageSummary.has_oldUrgent) ?
            mwiMessageSummary.oldUrgent : INT32_MAX;
}

void convertProtoToHidlMessageDetails(const ims_MwiMessageDetails& mwiMessageDetails,
            MessageDetails& messageDetails) {
    if (mwiMessageDetails.toAddress.arg != nullptr) {
        messageDetails.toAddress = hidl_string((char *) mwiMessageDetails.toAddress.arg);
    }

    if (mwiMessageDetails.fromAddress.arg != nullptr) {
        messageDetails.fromAddress = hidl_string((char *) mwiMessageDetails.fromAddress.arg);
    }

    if (mwiMessageDetails.subject.arg != nullptr) {
        messageDetails.subject = hidl_string((char *) mwiMessageDetails.subject.arg);
    }

    if (mwiMessageDetails.date.arg != nullptr) {
        messageDetails.date = hidl_string((char *) mwiMessageDetails.date.arg);
    }

    messageDetails.priority = (mwiMessageDetails.has_priority) ?
            convertProtoToHidlMwiMessagePriority(mwiMessageDetails.priority) :
            MessagePriority::MWI_MSG_PRIORITY_INVALID;

    if (mwiMessageDetails.messageId.arg != nullptr) {
        messageDetails.id = hidl_string((char *) mwiMessageDetails.messageId.arg);
    }

    messageDetails.type = (mwiMessageDetails.has_messageType) ?
            convertProtoToHidlMwiMessageType(mwiMessageDetails.messageType) :
            MessageType::MWI_MSG_INVALID;
}

void convertProtoToHidlImsSubConfig(const ims_ImsSubConfig& subConfig,
        ImsSubConfigInfo& subConfigInfo) {
    if (subConfig.has_simultStackCount) {
        subConfigInfo.simultStackCount = subConfig.simultStackCount;
    } else {
        subConfigInfo.simultStackCount = INT32_MAX;
    }

    const bool** ptr = (const bool**)subConfig.imsStackEnabled.arg;
    uint32_t count = getCount(ptr);
    subConfigInfo.imsStackEnabled.resize(count);
    for(uint32_t i = 0; i < count; ++i) {
      if (ptr[i]) {
        subConfigInfo.imsStackEnabled[i] = *(ptr[i]);
      }
    }
    std::string logStr = toString(subConfigInfo);
    QCRIL_LOG_INFO("subConfigInfo = %s", logStr.c_str());
}

void convertProtoToHidlConfParticipantOperation(const ims_ConfParticipantOperation& in,
        ConfParticipantOperation& out) {
    switch (in) {
        case ims_ConfParticipantOperation_ADD:
            out = ConfParticipantOperation::ADD;
            break;
        case ims_ConfParticipantOperation_REMOVE:
            out = ConfParticipantOperation::REMOVE;
            break;
        default:
            out = ConfParticipantOperation::INVALID;
            break;
    }
}

void convertProtoToHidlParticipantStatusInfo(const ims_ParticipantStatusInfo &in,
        ParticipantStatusInfo &out) {
    if (in.has_callId) {
        out.callId = in.callId;
    } else {
        out.callId = INT32_MAX;
    }

    if (in.has_operation) {
        convertProtoToHidlConfParticipantOperation(in.operation, out.operation);
    } else {
        out.operation = ConfParticipantOperation::INVALID;
    }

    if (in.has_sipStatus) {
        out.sipStatus = in.sipStatus;
    } else {
        out.sipStatus = INT32_MAX;
    }

    convertProtoToHidlString((const char*)in.participantUri.arg,
            out.participantUri);

    out.hasIsEct = in.has_isEct;
    if (in.has_isEct) {
        out.isEct = in.isEct;
    }
}

void convertProtoToHidlBlockStatus(const ims_BlockStatus &inStatus, BlockStatus &outStatus) {
    outStatus.blockReason = BlockReasonType::INVALID;
    if (inStatus.has_blockReason) {
        outStatus.blockReason = convertProtoToHidlBlockReasonType(inStatus.blockReason);
    }
    if (inStatus.has_blockReasonDetails) {
        outStatus.hasBlockReasonDetails = true;
        convertProtoToHidlBlockReasonDetails(inStatus.blockReasonDetails,
                outStatus.blockReasonDetails);
    }
}

void convertProtoToHidlBlockReasonDetails(const ims_BlockReasonDetails &inDetails,
    BlockReasonDetails &outDetails) {
    outDetails.regFailureReasonType = RegFailureReasonType::INVALID;
    if (inDetails.has_regFailureReasonType) {
        outDetails.regFailureReasonType = convertProtoToHidlRegFailureReasonType(
                inDetails.regFailureReasonType);
    }
    outDetails.regFailureReason = inDetails.has_RegFailureReason ?
          inDetails.RegFailureReason : INT32_MAX;
}

BlockReasonType convertProtoToHidlBlockReasonType(ims_BlockReasonType reason) {
     switch (reason) {
        case ims_BlockReasonType_BLOCK_REASON_PDP_FAILURE:
            return BlockReasonType::PDP_FAILURE;
        case ims_BlockReasonType_BLOCK_REASON_REGISTRATION_FAILURE:
            return BlockReasonType::REGISTRATION_FAILURE;
        case ims_BlockReasonType_BLOCK_REASON_HANDOVER_FAILURE:
            return BlockReasonType::HANDOVER_FAILURE;
        case ims_BlockReasonType_BLOCK_REASON_OTHER_FAILURE:
            return BlockReasonType::OTHER_FAILURE;
        default:
            return BlockReasonType::INVALID;
     }
}

RegFailureReasonType convertProtoToHidlRegFailureReasonType(ims_RegFailureReasonType reason) {
     switch (reason) {
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_UNSPECIFIED:
            return RegFailureReasonType::UNSPECIFIED;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_MOBILE_IP:
            return RegFailureReasonType::MOBILE_IP;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_INTERNAL:
            return RegFailureReasonType::INTERNAL;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED:
            return RegFailureReasonType::CALL_MANAGER_DEFINED;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_3GPP_SPEC_DEFINED:
            return RegFailureReasonType::TYPE_3GPP_SPEC_DEFINED;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_PPP:
            return RegFailureReasonType::PPP;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_EHRPD:
            return RegFailureReasonType::EHRPD;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IPV6:
            return RegFailureReasonType::IPV6;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IWLAN:
            return RegFailureReasonType::IWLAN;
        case ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_HANDOFF:
            return RegFailureReasonType::HANDOFF;
        default:
            return RegFailureReasonType::INVALID;
     }
}

VoWiFiCallQualityInfo convertProtoToHidlVoWiFiCallQuality(const ims_VoWiFiCallQuality&
        voWiFiCallQuality) {
    if (!voWiFiCallQuality.has_quality) {
        return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_INVALID;
    }

    switch (voWiFiCallQuality.quality) {
        case ims_voWiFiQuality_VOWIFI_QUALITY_EXCELLENT:
            return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_EXCELLENT;
        case ims_voWiFiQuality_VOWIFI_QUALITY_FAIR:
            return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_FAIR;
        case ims_voWiFiQuality_VOWIFI_QUALITY_BAD:
            return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_BAD;
        case ims_voWiFiQuality_VOWIFI_QUALITY_NONE:
            return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_NONE;
        default:
            return VoWiFiCallQualityInfo::VOWIFI_CALL_QUALITY_INVALID;
    }
}

SsServiceType convertToHidlSsServiceType(const int &serviceType) {
  switch (serviceType) {
    case SS_CFU:
      return SsServiceType::SS_CFU;
    case SS_CF_BUSY:
      return SsServiceType::SS_CF_BUSY;
    case SS_CF_NO_REPLY:
      return SsServiceType::SS_CF_NO_REPLY;
    case SS_CF_NOT_REACHABLE:
      return SsServiceType::SS_CF_NOT_REACHABLE;
    case SS_CF_ALL:
      return SsServiceType::SS_CF_ALL;
    case SS_CF_ALL_CONDITIONAL:
      return SsServiceType::SS_CF_ALL_CONDITIONAL;
    case SS_CLIP:
      return SsServiceType::SS_CLIP;
    case SS_CLIR:
      return SsServiceType::SS_CLIR;
    case SS_COLP:
      return SsServiceType::SS_COLP;
    case SS_COLR:
      return SsServiceType::SS_COLR;
    case SS_WAIT:
      return SsServiceType::SS_WAIT;
    case SS_BAOC:
      return SsServiceType::SS_BAOC;
    case SS_BAOIC:
      return SsServiceType::SS_BAOIC;
    case SS_BAOIC_EXC_HOME:
      return SsServiceType::SS_BAOIC_EXC_HOME;
    case SS_BAIC:
      return SsServiceType::SS_BAIC;
    case SS_BAIC_ROAMING:
      return SsServiceType::SS_BAIC_ROAMING;
    case SS_ALL_BARRING:
      return SsServiceType::SS_ALL_BARRING;
    case SS_OUTGOING_BARRING:
      return SsServiceType::SS_OUTGOING_BARRING;
    case SS_INCOMING_BARRING:
      return SsServiceType::SS_INCOMING_BARRING;
    case QCRIL_SS_INCOMING_BARRING_DN:
      return SsServiceType::SS_INCOMING_BARRING_DN;
    case QCRIL_SS_INCOMING_BARRING_ANONYMOUS:
      return SsServiceType::SS_INCOMING_BARRING_ANONYMOUS;
    default:
      return SsServiceType::SS_INVALID;
  }
}

SsRequestType convertToHidlSsRequestType(const RIL_SsRequestType &type) {
  switch (type) {
    case SS_ACTIVATION:
      return SsRequestType::SS_ACTIVATION;
    case SS_DEACTIVATION:
      return SsRequestType::SS_DEACTIVATION;
    case SS_INTERROGATION:
      return SsRequestType::SS_INTERROGATION;
    case SS_REGISTRATION:
      return SsRequestType::SS_REGISTRATION;
    case SS_ERASURE:
      return SsRequestType::SS_ERASURE;
    default:
      return SsRequestType::SS_INVALID;
  }
}

SsTeleserviceType convertToHidlSsTeleserviceType(const RIL_SsTeleserviceType &type) {
  switch (type) {
    case SS_ALL_TELE_AND_BEARER_SERVICES:
      return SsTeleserviceType::SS_ALL_TELE_AND_BEARER_SERVICES;
    case SS_ALL_TELESEVICES:
      return SsTeleserviceType::SS_ALL_TELESEVICES;
    case SS_TELEPHONY:
      return SsTeleserviceType::SS_TELEPHONY;
    case SS_ALL_DATA_TELESERVICES:
      return SsTeleserviceType::SS_ALL_DATA_TELESERVICES;
    case SS_SMS_SERVICES:
      return SsTeleserviceType::SS_SMS_SERVICES;
    case SS_ALL_TELESERVICES_EXCEPT_SMS:
      return SsTeleserviceType::SS_ALL_TELESERVICES_EXCEPT_SMS;
    default:
      return SsTeleserviceType::SS_INVALID;
  }
}

bool isServiceTypeCfQuery(SsServiceType serType, SsRequestType reqType) {
  if ((reqType == SsRequestType::SS_INTERROGATION) &&
      (serType == SsServiceType::SS_CFU ||
       serType == SsServiceType::SS_CF_BUSY ||
       serType == SsServiceType::SS_CF_NO_REPLY ||
       serType == SsServiceType::SS_CF_NOT_REACHABLE ||
       serType == SsServiceType::SS_CF_ALL ||
       serType == SsServiceType::SS_CF_ALL_CONDITIONAL ||
       serType == SsServiceType::SS_CFUT)) {
    return true;
  }
  return false;
}

bool isServiceTypeIcbQueryWithNumber(SsServiceType serType, SsRequestType reqType) {
  if ((reqType == SsRequestType::SS_INTERROGATION) &&
      (serType == SsServiceType::SS_INCOMING_BARRING_DN ||
       serType == SsServiceType::SS_INCOMING_BARRING_ANONYMOUS)) {
    return true;
  }
  return false;
}

void convertToHidlStkCcUnsolSsResult(ims_ril_ss_ind_type &in, StkCcUnsolSsResult &out) {
  out.serviceType = convertToHidlSsServiceType((int)in.ril_ss_response->serviceType);
  out.requestType = convertToHidlSsRequestType(in.ril_ss_response->requestType);
  out.teleserviceType = convertToHidlSsTeleserviceType(in.ril_ss_response->teleserviceType);
  out.serviceClass = in.ril_ss_response->serviceClass;
  out.result = in.ril_ss_response->result;

  out.ssInfoData.resize(0);
  out.cfData.resize(0);
  out.cbNumInfo.resize(0);

  if (isServiceTypeCfQuery(out.serviceType, out.requestType)) {
    out.cfData.resize(1);
    out.cfData[0].cfInfo.resize(in.ril_ss_response->cfData.numValidIndexes);
    for (int i = 0; i < in.ril_ss_response->cfData.numValidIndexes; i++) {
      RIL_CallForwardInfo &cf = in.ril_ss_response->cfData.cfInfo[i];
      CallForwardInfo &cfInfo = out.cfData[0].cfInfo[i];
      cfInfo = {};
      cfInfo.status = cf.status;
      cfInfo.reason = cf.reason;
      cfInfo.serviceClass = cf.serviceClass;
      cfInfo.toa = cf.toa;
      convertProtoToHidlString(cf.number, cfInfo.number);
      cfInfo.timeSeconds = cf.timeSeconds;
      cfInfo.hasCallFwdTimerStart = false;
      cfInfo.hasCallFwdTimerEnd = false;
    }
  } else if (isServiceTypeIcbQueryWithNumber(out.serviceType, out.requestType) &&
      in.cb_num_list_len > 0 && in.cb_num_list != nullptr) {
    out.cbNumInfo.resize(in.cb_num_list_len);
    for (int i = 0; i < in.cb_num_list_len; i++) {
      convertProtoToHidlNumInfo(in.cb_num_list[i], out.cbNumInfo[i]);
    }
  } else {
    out.ssInfoData.resize(1);
    out.ssInfoData[0].ssInfo.resize(SS_INFO_MAX);
    for (int i = 0; i < SS_INFO_MAX; i++) {
      out.ssInfoData[0].ssInfo[i] = in.ril_ss_response->ssInfo[i];
    }
  }
}

void convertToHidlAutoCallRejectionInfo(const ims_AutoCallRejectionInfo& in,
        V1_3::AutoCallRejectionInfo& out) {
    out.callType = convertProtoToHidlCallType(in.callType);
    out.autoRejectionCause = convertProtoToHidlCallFailCause(in.autoRejectionCause);
    out.sipErrorCode = in.sipErrorCode;
    out.number.clear();
    if(in.hasNumber) {
        const char* number = (const char*)in.number.arg;
        out.number.setToExternal(number, strlen(number));
    }

    std::string logStr = toString(out);
    QCRIL_LOG_INFO("Auto Call Rejection : %s", logStr.c_str());
}

void convertToHidlVirtualLines(hidl_vec<hidl_string>& out,
        const std::vector<std::string>& in) {
    int size = in.size();
    out.resize(size);
    for (int i = 0; i < size; i++) {
        out[i] = hidl_string(in[i]);
        QCRIL_LOG_INFO("Virtual Line[%d] : %s", i, out[i].c_str());
    }
}

void convertToHidlMultiIdentityRegistrationStatus(
        const std::vector<QcrilImsMultiIdentityInfo>& in_lines,
        hidl_vec<V1_4::MultiIdentityLineInfoHal>& out_lines) {
    out_lines.resize(in_lines.size());
    for(unsigned i = 0; i < in_lines.size(); i++) {
        auto& in_line = in_lines[i];
        V1_4::MultiIdentityLineInfoHal out_line = {};
        out_line.msisdn = in_line.msisdn;
        out_line.registrationStatus = convertToHIdlMultiIdentityRegistrationStatus(
                in_line.registrationStatus);
        std::string logStr = toString(out_line);
        QCRIL_LOG_INFO("MultiIdentity Registration Info for line[%d] : %s", i, logStr.c_str());
        out_lines[i] = out_line;
    }
}

V1_4::MultiIdentityRegistrationStatus convertToHIdlMultiIdentityRegistrationStatus(
        int status) {
    switch(status) {
        case QcrilImsMultiIdentityInfo::REGISTRATION_STATUS_ACTIVE:
            return V1_4::MultiIdentityRegistrationStatus::STATUS_ENABLE;
        case QcrilImsMultiIdentityInfo::REGISTRATION_STATUS_INACTIVE:
            return V1_4::MultiIdentityRegistrationStatus::STATUS_DISABLE;
        default:
            return V1_4::MultiIdentityRegistrationStatus::STATUS_UNKNOWN;
    }
}

void convertToRilMultiIdentityInfo(const V1_4::MultiIdentityLineInfoHal& in_line,
            QcrilImsMultiIdentityInfo& out_line) {
    out_line.msisdn = in_line.msisdn;
    out_line.registrationStatus =
            convertToRilMultiIdentityRegistrationStatus(in_line.registrationStatus);
    out_line.lineType = convertToRilMultiIdentityLineType(in_line.lineType);
}

int convertToRilMultiIdentityRegistrationStatus(
        V1_4::MultiIdentityRegistrationStatus status) {
    switch(status) {
        case V1_4::MultiIdentityRegistrationStatus::STATUS_ENABLE:
            return QcrilImsMultiIdentityInfo::REGISTRATION_STATUS_ACTIVE;
        case V1_4::MultiIdentityRegistrationStatus::STATUS_DISABLE:
            return QcrilImsMultiIdentityInfo::REGISTRATION_STATUS_INACTIVE;
        default:
            return QcrilImsMultiIdentityInfo::REGISTRATION_STATUS_UNKNOWN;
    }
}

int convertToRilMultiIdentityLineType(
        V1_4::MultiIdentityLineType type) {
    switch(type) {
        case V1_4::MultiIdentityLineType::LINE_TYPE_PRIMARY:
            return QcrilImsMultiIdentityInfo::LINE_TYPE_PRIMARY;
        case V1_4::MultiIdentityLineType::LINE_TYPE_SECONDARY:
            return QcrilImsMultiIdentityInfo::LINE_TYPE_SECONDARY;
        default:
            return QcrilImsMultiIdentityInfo::LINE_TYPE_UNKNOWN;
    }
}

V1_1::CallFailCause migrateCallFailCauseToV11(CallFailCause failCause) {
    //We need this check to make sure that an invalid call fail cause in
    //V10 gets mapped to an invalid call fail cause in V11. Otherwise it
    //might get mapped to a valid fail cause.
    return (failCause == V1_0::CallFailCause::CALL_FAIL_INVALID) ?
        V1_1::CallFailCause::CALL_FAIL_INVALID :
            static_cast<V1_1::CallFailCause>(failCause);
}

void migrateDialRequest(const V1_0::DialRequest& from,
        V1_4::DialRequest& to) {
    to.address = from.address;
    to.clirMode = from.clirMode;
    to.hasCallDetails = from.hasCallDetails;
    to.presentation = from.presentation;
    //Call details
    CallDetails& callDetailsV14 = to.callDetails;
    const CallDetails& callDetailsV10 = from.callDetails;
    callDetailsV14.callType = callDetailsV10.callType;
    callDetailsV14.callDomain = callDetailsV10.callDomain;
    callDetailsV14.extrasLength = callDetailsV10.extrasLength;
    callDetailsV14.extras = callDetailsV10.extras;
    callDetailsV14.localAbility = callDetailsV10.localAbility;
    callDetailsV14.peerAbility = callDetailsV10.peerAbility;
    callDetailsV14.callSubstate = callDetailsV10.callSubstate;
    callDetailsV14.mediaId = callDetailsV10.mediaId;
    callDetailsV14.causeCode = callDetailsV10.causeCode;
    callDetailsV14.rttMode = callDetailsV10.rttMode;
    callDetailsV14.sipAlternateUri = callDetailsV10.sipAlternateUri;

    to.hasIsConferenceUri = from.hasIsConferenceUri;
    to.isConferenceUri = from.isConferenceUri;
    to.hasIsCallPull = from.hasIsCallPull;
    to.isCallPull = from.isCallPull;
    to.hasIsEncrypted = from.hasIsEncrypted;
    to.isEncrypted = from.isEncrypted;
    to.multiLineInfo.msisdn = NULL;
}

void migrateHangup(const V1_0::HangupRequestInfo& from,
        V1_1::HangupRequestInfo& to) {
    to.connIndex = from.connIndex;
    to.hasMultiParty = from.hasMultiParty;
    to.multiParty = from.multiParty;
    to.connUri = from.connUri;
    to.conf_id = from.conf_id;
    to.hasFailCauseResponse = from.hasFailCauseResponse;
    to.failCauseResponse.failCause =
            migrateCallFailCauseToV11(from.failCauseResponse.failCause);
    to.failCauseResponse.errorinfo = from.failCauseResponse.errorinfo;
    to.failCauseResponse.networkErrorString = from.failCauseResponse.networkErrorString;
    to.failCauseResponse.hasErrorDetails = from.failCauseResponse.hasErrorDetails;
    to.failCauseResponse.errorDetails.errorCode =
            from.failCauseResponse.errorDetails.errorCode;
    to.failCauseResponse.errorDetails.errorString =
            from.failCauseResponse.errorDetails.errorString;
}

V1_2::CallFailCause migrateCallFailCauseToV12(V1_1::CallFailCause failCause) {
    //We need this check to make sure that an invalid call fail cause in
    //V11 gets mapped to an invalid call fail cause in V12. Otherwise it
    //might get mapped to a valid fail cause.
    return (failCause == V1_1::CallFailCause::CALL_FAIL_INVALID) ?
        V1_2::CallFailCause::CALL_FAIL_INVALID :
            static_cast<V1_2::CallFailCause>(failCause);
}

void  migrateHangup(const V1_1::HangupRequestInfo& from,
        V1_2::HangupRequestInfo& to) {
    to.connIndex = from.connIndex;
    to.hasMultiParty = from.hasMultiParty;
    to.multiParty = from.multiParty;
    to.connUri = from.connUri;
    to.conf_id = from.conf_id;
    to.hasFailCauseResponse = from.hasFailCauseResponse;
    to.failCauseResponse.failCause =
            migrateCallFailCauseToV12(from.failCauseResponse.failCause);
    to.failCauseResponse.errorinfo = from.failCauseResponse.errorinfo;
    to.failCauseResponse.networkErrorString = from.failCauseResponse.networkErrorString;
    to.failCauseResponse.hasErrorDetails = from.failCauseResponse.hasErrorDetails;
    to.failCauseResponse.errorDetails.errorCode =
            from.failCauseResponse.errorDetails.errorCode;
    to.failCauseResponse.errorDetails.errorString =
            from.failCauseResponse.errorDetails.errorString;
}

V1_3::CallFailCause migrateCallFailCauseToV13(V1_2::CallFailCause failCause) {
    //We need this check to make sure that an invalid call fail cause in
    //V12 gets mapped to an invalid call fail cause in V13. Otherwise it
    //might get mapped to a valid fail cause.
    return (failCause == V1_2::CallFailCause::CALL_FAIL_INVALID) ?
        V1_3::CallFailCause::CALL_FAIL_INVALID :
            static_cast<V1_3::CallFailCause>(failCause);
}

void  migrateHangup(const V1_2::HangupRequestInfo& from,
        V1_3::HangupRequestInfo& to) {
    to.connIndex = from.connIndex;
    to.hasMultiParty = from.hasMultiParty;
    to.multiParty = from.multiParty;
    to.connUri = from.connUri;
    to.conf_id = from.conf_id;
    to.hasFailCauseResponse = from.hasFailCauseResponse;
    to.failCauseResponse.failCause =
            migrateCallFailCauseToV13(from.failCauseResponse.failCause);
    to.failCauseResponse.errorinfo = from.failCauseResponse.errorinfo;
    to.failCauseResponse.networkErrorString = from.failCauseResponse.networkErrorString;
    to.failCauseResponse.hasErrorDetails = from.failCauseResponse.hasErrorDetails;
    to.failCauseResponse.errorDetails.errorCode =
            from.failCauseResponse.errorDetails.errorCode;
    to.failCauseResponse.errorDetails.errorString =
            from.failCauseResponse.errorDetails.errorString;
}

void  migrateAddressInfo(const V1_0::AddressInfo& from,
        V1_2::AddressInfo& to) {
    to.city = from.city;
    to.state = from.state;
    to.country = from.country;
    to.postalCode = from.postalCode;
    to.countryCode = from.countryCode;
    // not filling street and houseNumber fields so that they contain default values.
}

void migrateColrInfo(const V1_0::ColrInfo& from,
            V1_3::ColrInfo& to) {
    to.status = ServiceClassStatus::INVALID;
    to.provisionStatus = V1_3::ServiceClassProvisionStatus::INVALID;
    to.presentation = from.presentation;
    to.hasErrorDetails = from.hasErrorDetails;
    to.errorDetails.errorCode = from.errorDetails.errorCode;
    to.errorDetails.errorString = from.errorDetails.errorString;
}

V1_3::VoiceInfo convertToHidlVoiceInfo(ims_VoiceInfo voiceInfo) {
    switch(voiceInfo) {
      case ims_Voice_Info_SILENT:
        return V1_3::VoiceInfo::VOICE_INFO_SILENT;
      case ims_Voice_Info_SPEECH:
        return V1_3::VoiceInfo::VOICE_INFO_SPEECH;
      default:
        return V1_3::VoiceInfo::VOICE_INFO_UNKNOWN;
    }
}

} // namespace utils
} // namespace V1_0
} // namespace ims
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
