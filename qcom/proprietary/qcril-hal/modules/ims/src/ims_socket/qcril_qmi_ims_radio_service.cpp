/******************************************************************************
  @file    qcril_qmi_ims_radio_service.c
  @brief   qcril qmi - ims_radio_service

  DESCRIPTION
    Implements the server side of the IImsradio interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <string.h>
#include "qcril_qmi_ims_radio_utils.h"
#include "qcril_qmi_ims_radio_service.h"

#include "qcril_qmi_ims_radio_agent.h"
#include "qcril_qmi_ims_sms.h"
#include "qcril_qmi_sms.h"
#include "qcril_qmi_ims_sms_util.h"

extern "C" {
    #include "qcril_log.h"
    #include "qcril_reqlist.h"
    #include "qcril_qmi_ims_flow_control.h"
    #include "qcril_qmi_ims_msg_meta.h"
}

#ifdef QMI_RIL_UTF
#include "unistd.h"
extern "C" uint32 qcril_get_time_milliseconds();
#endif

#if defined(FEATURE_TARGET_GLIBC_x86)  || defined(QMI_RIL_UTF)
    extern "C" size_t strlcat(char *, const char *, size_t);
    extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

using namespace vendor::qti::hardware::radio::ims::V1_0::implementation;
using namespace vendor::qti::hardware::radio::ims::V1_0::utils;

using ::android::hardware::Void;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_0 {
namespace implementation {

static sp<ImsRadioImpl> mImsRadio;

/*
 *   @brief
 *   Registers the callback for IImsRadio using the IImsRadioCallback object
 *   being passed in by the client as a parameter
 *
 */
Return<void> ImsRadioImpl::setCallback(
  const sp<IImsRadioResponse>& mImsRadioResponseCbParam,
  const sp<IImsRadioIndication>& mImsRadioIndicationCbParam) {
    QCRIL_LOG_DEBUG("ImsRadioImpl::setCallback");
    {
      RWLock::AutoWLock lock(mCallbackLock);
      if (mImsRadioResponseCb != nullptr) {
        mImsRadioResponseCb->unlinkToDeath(this);
      }
      mImsRadioResponseCb = mImsRadioResponseCbParam;
      mImsRadioIndicationCb = mImsRadioIndicationCbParam;
      mImsRadioIndicationCbV1_1 =
              V1_1::IImsRadioIndication::castFrom(mImsRadioIndicationCbParam).withDefault(nullptr);

      if (mImsRadioResponseCb != nullptr) {
        mImsRadioResponseCb->linkToDeath(this, 0);
      }

      mImsRadioResponseCbV1_2 =
              V1_2::IImsRadioResponse::castFrom(mImsRadioResponseCb).withDefault(nullptr);
      mImsRadioIndicationCbV1_2 =
              V1_2::IImsRadioIndication::castFrom(mImsRadioIndicationCb).withDefault(nullptr);
      if (mImsRadioResponseCbV1_2 == nullptr || mImsRadioIndicationCbV1_2 == nullptr)
      {
        mImsRadioResponseCbV1_2 = nullptr;
        mImsRadioIndicationCbV1_2 = nullptr;
      }

      mImsRadioResponseCbV1_3 =
              V1_3::IImsRadioResponse::castFrom(mImsRadioResponseCb).withDefault(nullptr);
      mImsRadioIndicationCbV1_3 =
              V1_3::IImsRadioIndication::castFrom(mImsRadioIndicationCb).withDefault(nullptr);
      if (mImsRadioResponseCbV1_3 == nullptr || mImsRadioIndicationCbV1_3 == nullptr)
      {
        mImsRadioResponseCbV1_3 = nullptr;
        mImsRadioIndicationCbV1_3 = nullptr;
      }

      mImsRadioResponseCbV1_4 =
              V1_4::IImsRadioResponse::castFrom(mImsRadioResponseCb).withDefault(nullptr);
      mImsRadioIndicationCbV1_4 =
              V1_4::IImsRadioIndication::castFrom(mImsRadioIndicationCb).withDefault(nullptr);
      if (mImsRadioResponseCbV1_4 == nullptr || mImsRadioIndicationCbV1_4 == nullptr)
      {
        mImsRadioResponseCbV1_4 = nullptr;
        mImsRadioIndicationCbV1_4 = nullptr;
      }
    }

    qcril_qmi_voice_ims_client_connected();

    return Void();
}

sp<IImsRadioResponse> ImsRadioImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioResponseCb;
}

sp<IImsRadioIndication> ImsRadioImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioIndicationCb;
}

sp<V1_1::IImsRadioIndication> ImsRadioImpl::getIndicationCallbackV1_1() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioIndicationCbV1_1;
}

sp<V1_2::IImsRadioResponse> ImsRadioImpl::getResponseCallbackV1_2() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioResponseCbV1_2;
}

sp<V1_2::IImsRadioIndication> ImsRadioImpl::getIndicationCallbackV1_2() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioIndicationCbV1_2;
}

sp<V1_3::IImsRadioResponse> ImsRadioImpl::getResponseCallbackV1_3() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioResponseCbV1_3;
}

sp<V1_3::IImsRadioIndication> ImsRadioImpl::getIndicationCallbackV1_3() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioIndicationCbV1_3;
}

sp<V1_4::IImsRadioResponse> ImsRadioImpl::getResponseCallbackV1_4() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioResponseCbV1_4;
}

sp<V1_4::IImsRadioIndication> ImsRadioImpl::getIndicationCallbackV1_4() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mImsRadioIndicationCbV1_4;
}

void ImsRadioImpl::serviceDied(uint64_t ,
        const ::android::wp<::android::hidl::base::V1_0::IBase>&) {
    QCRIL_LOG_DEBUG("ImsRadioImpl::serviceDied: Client died. Cleaning up callbacks");
    clearCallbacks();
}

ImsRadioImpl::ImsRadioImpl() {}

ImsRadioImpl::~ImsRadioImpl() {}

void ImsRadioImpl::clearCallbacks() {
    RWLock::AutoWLock lock(mCallbackLock);
    mImsRadioResponseCb = nullptr;
    mImsRadioIndicationCb = nullptr;
    mImsRadioIndicationCbV1_1 = nullptr;
    mImsRadioResponseCbV1_2 = nullptr;
    mImsRadioIndicationCbV1_2 = nullptr;
    mImsRadioIndicationCbV1_3 = nullptr;
    mImsRadioResponseCbV1_3 = nullptr;
    mImsRadioIndicationCbV1_4 = nullptr;
    mImsRadioResponseCbV1_4 = nullptr;
}


Return<void> ImsRadioImpl::dial(int32_t token, const DialRequest& dialRequest) {
    V1_4::DialRequest v14dialRequest;
    migrateDialRequest(dialRequest, v14dialRequest);
    return dial_1_4(token, v14dialRequest);
}

/*
 *  @brief
 *  Server side implementtaion of the IImsradio service's dial API
 *
 */
Return<void> ImsRadioImpl::dial_1_4(int32_t token, const V1_4::DialRequest& dialRequest) {
    ims_Dial* dial = (ims_Dial*) qcril_malloc(sizeof(ims_Dial));

    if (dial == nullptr) {
        QCRIL_LOG_ERROR("Failed to allocate memmory.");
        return Void();
    }

    ims_Error ret = convertHidlToProtoDial(dialRequest, *dial);

    if (ret == ims_Error_E_SUCCESS) {
        qcril_qmi_voice_process_in_main_thread(QCRIL_EVT_IMS_SOCKET_REQ_DIAL,
                dial, sizeof(*dial),
                qcril_qmi_ims_convert_ims_token_to_ril_token(token));
    } else {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_DIAL, ret);
        QCRIL_LOG_ERROR("Dial failure.");
    }

    return Void();
}

/*
 *   @brief
 *   Delegates the response from QCRIL to the appropriate response callback
 *   registered for the incoming request
 */
void ImsRadioImpl::processResponse(ims_MsgTag tag, void* data) {
    sp<IImsRadioResponse> respCb = getResponseCallback();
    sp<IImsRadioIndication> indCb = getIndicationCallback();
    sp<V1_2::IImsRadioResponse> respCbV1_2 = getResponseCallbackV1_2();
    sp<V1_2::IImsRadioIndication> indCbV1_2 = getIndicationCallbackV1_2();
    sp<V1_3::IImsRadioResponse> respCbV1_3 = getResponseCallbackV1_3();
    sp<V1_3::IImsRadioIndication> indCbV1_3 = getIndicationCallbackV1_3();
    sp<V1_4::IImsRadioResponse> respCbV1_4 = getResponseCallbackV1_4();
    sp<V1_4::IImsRadioIndication> indCbV1_4 = getIndicationCallbackV1_4();
    Return<void> retStatus;

    if (respCb == nullptr || indCb == nullptr) {
        QCRIL_LOG_ERROR("mImsRadioResponseCb/mImsRadioIndicationCb is nullptr");
        return;
    }

    switch(tag.id) {
        case ims_MsgId_REQUEST_DIAL:
            {
                retStatus = respCb->dialResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_IMS_REGISTRATION_STATE:
            {
                RegistrationInfo reg = {};
                if (data != nullptr) {
                    convertProtoToHidlRegistration(*((const ims_Registration*)data), reg);
                }
                retStatus = respCb->getRegistrationResponse(tag.token, tag.error,
                        reg);
                break;
            }
        case ims_MsgId_REQUEST_ANSWER:
            {
                retStatus = respCb->answerResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_HOLD:
            {
                SipErrorInfo sipErrorInfo = {.errorCode = INT32_MAX};
                if (data != nullptr) {
                    convertProtoToHidlSipErrorInfo(*((const ims_SipErrorInfo*)data), sipErrorInfo);
                }

                retStatus = respCb->holdResponse(tag.token, tag.error, sipErrorInfo);
                break;
            }
        case ims_MsgId_REQUEST_RESUME:
            {
                SipErrorInfo sipErrorInfo = {.errorCode = INT32_MAX};
                if (data != nullptr) {
                    convertProtoToHidlSipErrorInfo(*((const ims_SipErrorInfo*)data), sipErrorInfo);
                }
                retStatus = respCb->resumeResponse(tag.token, tag.error,
                        sipErrorInfo);
                break;
            }
        case ims_MsgId_REQUEST_HANGUP:
            {
                retStatus = respCb->hangupResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE:
            {
                retStatus = respCb->requestRegistrationChangeResponse(tag.token,
                        tag.error);
                break;
            }
        case ims_MsgId_REQUEST_QUERY_SERVICE_STATUS:
            {
                hidl_vec<ServiceStatusInfo> serviceStatusInfoList;
                if (data != nullptr) {
                    const ims_Info** ptr = (const ims_Info**)
                            (((ims_SrvStatusList*)data)->SrvStatusInfo.arg);
                    convertProtoToHidlServiceInfoList(ptr, serviceStatusInfoList);
                }
                retStatus = respCb->queryServiceStatusResponse(tag.token, tag.error,
                        serviceStatusInfoList);
                break;
            }
        case ims_MsgId_REQUEST_SET_SERVICE_STATUS:
            {
                QCRIL_LOG_INFO("Sending set service status response token - %d, error - %d.",
                        tag.token, tag.error);
                retStatus = respCb->setServiceStatusResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS:
            {
                hidl_vec<CallForwardInfo> outCfInfoList;
                SipErrorInfo errorDetails = {.errorCode = INT32_MAX};
                if (data != nullptr) {
                    const ims_CallForwardInfoList* ptr = (const ims_CallForwardInfoList*)data;
                    const ims_CallForwardInfoList_CallForwardInfo** const inCfInfoList =
                            (const ims_CallForwardInfoList_CallForwardInfo** const)(ptr->info.arg);

                    convertProtoToHidlCallForwardInfoList(inCfInfoList, outCfInfoList);
                    if (ptr->has_errorDetails) {
                        convertProtoToHidlSipErrorInfo(ptr->errorDetails, errorDetails);
                    }
                }

                retStatus = respCb->queryCallForwardStatusResponse(tag.token,
                        tag.error, outCfInfoList, errorDetails);
                break;
            }
        case ims_MsgId_REQUEST_QUERY_CALL_WAITING:
            {
                SipErrorInfo errorDetails = {.errorCode = INT32_MAX};
                ServiceClassStatus status = ServiceClassStatus::INVALID;
                uint32_t serviceClass = INT32_MAX;
                if (data != nullptr) {
                    const ims_CallWaitingInfo* callWaitingInfoPtr =
                            (const ims_CallWaitingInfo*)data;

                    status = (callWaitingInfoPtr->has_service_status) ?
                            convertProtoToHidlServiceClassStatus(
                            callWaitingInfoPtr->service_status) :
                            ServiceClassStatus::INVALID;

                    serviceClass = (callWaitingInfoPtr->has_service_class) ?
                            ((callWaitingInfoPtr->service_class.has_service_Class)?
                            callWaitingInfoPtr->service_class.service_Class : INT32_MAX) :
                            INT32_MAX;

                    if (callWaitingInfoPtr->has_errorDetails) {
                        convertProtoToHidlSipErrorInfo(callWaitingInfoPtr->errorDetails,
                                errorDetails);
                    }
                }

                retStatus = respCb->getCallWaitingResponse(tag.token, tag.error,
                        status, serviceClass, errorDetails);
                break;
            }
        case ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION:
            {
                ServiceClassStatus status = ServiceClassStatus::INVALID;
                if (data != nullptr) {
                    const ims_SuppSvcStatus* suppSvcStatusPtr = (const ims_SuppSvcStatus*)data;
                    status = (suppSvcStatusPtr->has_status) ?
                            convertProtoToHidlServiceClassStatus(suppSvcStatusPtr->status) :
                            ServiceClassStatus::INVALID;
                }

                retStatus = respCb->setSuppServiceNotificationResponse(tag.token,
                        tag.error, status);
                break;
            }
        case ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS:
        case ims_MsgId_REQUEST_SET_CALL_WAITING:
        case ims_MsgId_REQUEST_SET_COLR:
        case ims_MsgId_REQUEST_SUPP_SVC_STATUS:
            {
                V1_3::SuppServiceStatus suppServiceStatusV1_3 = V1_3::SuppServiceStatus{};
                if (data != nullptr) {
                    convertProtoToHidlSuppServiceStatus(*((ims_SuppSvcResponse*) data),
                        suppServiceStatusV1_3);
                }
                if (respCbV1_3 != nullptr) {
                    QCRIL_LOG_DEBUG("Invoking respCbv1_3->suppServiceStatusResponse_1_3()");
                    retStatus = respCbV1_3->suppServiceStatusResponse_1_3(tag.token, tag.error,
                        suppServiceStatusV1_3);
                    break;
                }
                //respCbV1_3 is a nullptr. Trying to use respCb
                QCRIL_LOG_DEBUG("respCbV1_3 is nullptr. Falling back to V1.0");
                SuppServiceStatus suppServiceStatusV1_0 = SuppServiceStatus{};
                convertSuppServiceStatusV13ToV10(suppServiceStatusV1_3, suppServiceStatusV1_0);
                QCRIL_LOG_DEBUG("Invoking respCb->suppServiceStatusResponse()");
                retStatus = respCb->suppServiceStatusResponse(tag.token, tag.error,
                    suppServiceStatusV1_0);
                break;
            }
        case ims_MsgId_REQUEST_SET_IMS_CONFIG:
            {
                ConfigInfo config = ConfigInfo{};
                if (data != nullptr) {
                    convertProtoToHidlConfig(*((ims_ConfigMsg*)data), config);
                }
                retStatus = respCb->setConfigResponse(tag.token, tag.error, config);
                break;
            }
        case ims_MsgId_REQUEST_GET_IMS_CONFIG:
            {
                ConfigInfo config = ConfigInfo{};
                if (data != nullptr) {
                    convertProtoToHidlConfig(*((ims_ConfigMsg*)data), config);
                }
                retStatus = respCb->getConfigResponse(tag.token, tag.error, config);
                break;
            }
        case ims_MsgId_REQUEST_CONFERENCE:
            {
                SipErrorInfo sipErrorInfo = {.errorCode = INT32_MAX};
                if (data != nullptr) {
                    convertProtoToHidlSipErrorInfo(*((const ims_SipErrorInfo*)data), sipErrorInfo);
                }
                retStatus = respCb->conferenceResponse(tag.token, tag.error,
                        sipErrorInfo);
                break;
            }
        case ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
            {
                retStatus = respCb->exitEmergencyCallbackModeResponse(tag.token,
                        tag.error);
                break;
            }
        case ims_MsgId_REQUEST_SEND_UI_TTY_MODE:
            {
                retStatus = respCb->setUiTTYModeResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_QUERY_CLIP:
            {
                ClipProvisionStatus clipProvisionStatus = {};
                if (data != nullptr) {
                    convertProtoToHidlClipProvisionStatus(*((ims_ClipProvisionStatus*) data),
                        clipProvisionStatus);
                }
                retStatus = respCb->getClipResponse(tag.token, tag.error,
                        clipProvisionStatus);
                break;
            }
        case ims_MsgId_REQUEST_GET_CLIR:
            {
                ClirInfo clirInfo = {};
                bool hasClirInfo = false;

                if (data != nullptr) {
                    hasClirInfo = convertProtoToHidlClirInfo(*((ims_Clir*) data), clirInfo);
                }
                retStatus = respCb->getClirResponse(tag.token, tag.error, clirInfo,
                        hasClirInfo);
                break;
            }
        case ims_MsgId_REQUEST_SET_CLIR:
            {
                retStatus = respCb->setClirResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_GET_COLR:
            {
                V1_3::ColrInfo colrInfoV1_3 = V1_3::ColrInfo{};
                if (data != nullptr) {
                    convertProtoToHidlColrInfo(*((ims_Colr*) data), colrInfoV1_3);
                }
                if (respCbV1_3 != nullptr) {
                    QCRIL_LOG_DEBUG("Invoking respCbv1_3->getColrResponse_1_3()");
                    retStatus = respCbV1_3->getColrResponse_1_3(tag.token,
                            tag.error, colrInfoV1_3);
                    break;
                }
                //respCbV1_2 is a nullptr. Trying to use respCb
                QCRIL_LOG_DEBUG("respCbV1_3 is nullptr. Falling back to V1.0");
                ColrInfo colrInfoV1_0 = ColrInfo{};
                convertColrInfoV13ToV10(colrInfoV1_3, colrInfoV1_0);
                QCRIL_LOG_DEBUG("Invoking respCb->getColrResponse()");
                retStatus = respCb->getColrResponse(tag.token, tag.error, colrInfoV1_0);
                break;
            }
        case ims_MsgId_REQUEST_DTMF:
            {
                retStatus = respCb->sendDtmfResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_DTMF_START:
            {
                retStatus = respCb->startDtmfResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_DTMF_STOP:
            {
                retStatus = respCb->stopDtmfResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER:
            {
                retStatus = respCb->explicitCallTransferResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_MODIFY_CALL_INITIATE:
            {
                retStatus = respCb->modifyCallInitiateResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_CANCEL_MODIFY_CALL:
            {
                retStatus = respCb->cancelModifyCallResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM:
            {
                retStatus = respCb->modifyCallConfirmResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_GET_RTP_STATISTICS:
            {
                uint64_t count = 0;
                if (data != nullptr) {
                    count = ((const ims_RtpStatisticsData*)data)->count;
                }

                retStatus = respCb->getRtpStatisticsResponse(tag.token, tag.error,
                        count);
                break;
            }
         case ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS:
            {
                uint64_t count = 0;
                if (data != nullptr) {
                    count = ((const ims_RtpStatisticsData*)data)->count;
                }

                retStatus = respCb->getRtpErrorStatisticsResponse(tag.token, tag.error,
                        count);
                break;
            }
        case ims_MsgId_REQUEST_DEFLECT_CALL:
            {
                retStatus = respCb->deflectCallResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO:
            {
                retStatus = respCb->sendGeolocationInfoResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_SEND_RTT_MSG:
            {
                retStatus = respCb->sendRttMessageResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_REQUEST_SEND_SMS:
            {
                if (respCbV1_2 != nullptr)
                {
                  if (data) {
                    auto resp = static_cast<QcrilImsSmsResponseData*>(data);
                    retStatus = respCbV1_2->sendImsSmsResponse(tag.token, resp->messageRef,
                        resp->smsStatusResult, resp->reason);
                  } else {
                    retStatus = respCbV1_2->sendImsSmsResponse(tag.token, -1,
                        ImsSmsSendStatusResult::SEND_STATUS_ERROR,
                        ImsSmsSendFailureReason::RESULT_ERROR_GENERIC_FAILURE);
                  }
                }
                else
                {
                  QCRIL_LOG_ERROR("Ims Radio Response Cb for V1_2 is null");
                }
                break;
            }
        case ims_MsgId_REQUEST_ACK_SMS:
            QCRIL_LOG_INFO("no definition for IMS ACK SMS Response cb");
            break;

        case ims_MsgId_REQUEST_REGISTER_MULTI_IDENTITY_LINES:
            if (respCbV1_4 != NULL) {
                retStatus = respCbV1_4->registerMultiIdentityLinesResponse(
                        tag.token, tag.error);
            }
            else {
                QCRIL_LOG_ERROR("Ims Radio Response respCbV1_4 is null");
            }
            break;

        case ims_MsgId_REQUEST_QUERY_VIRTUAL_LINE_INFO:
            if (respCbV1_4 != NULL) {
               hidl_string msisdn;
               hidl_vec<hidl_string> lines;
               if (data) {
                  auto resp = static_cast<VirtualLineInfo*>(data);
                  msisdn = resp->mMsisdn;
                  QCRIL_LOG_INFO("query line msisdn : %s" , msisdn.c_str());
                  convertToHidlVirtualLines(lines, resp->mVirtualLines);
               }
               retStatus = respCbV1_4->queryVirtualLineInfoResponse(
                     tag.token, msisdn, lines);
            }
            else {
                QCRIL_LOG_ERROR("Ims Radio Response respCbV1_4 is null");
            }
            break;

        case ims_MsgId_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
            {
                 retStatus = indCb->onEnterEmergencyCallBackMode();
                 break;
            }
        case ims_MsgId_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
            {
                retStatus = indCb->onExitEmergencyCallBackMode();
                break;
            }
        case ims_MsgId_UNSOL_TTY_NOTIFICATION:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Tty info is null.");
                    break;
                }

                TtyInfo ttyInfo = {};
                ims_TtyNotify* ttyNotify = (ims_TtyNotify*) data;
                ttyInfo.mode = (ttyNotify != nullptr && ttyNotify->has_mode) ?
                    convertProtoToHidlTtyMode(ttyNotify->mode) : TtyMode::TTY_MODE_INVALID;

                qcril_binary_data_type* userdata =
                    (qcril_binary_data_type*)(((ims_TtyNotify*)data)->userData.arg);
                convertProtoToHidlBinaryData(userdata, ttyInfo.userData);
                retStatus = indCb->onTtyNotification(ttyInfo);
                break;
            }
        case ims_MsgId_UNSOL_REFRESH_CONF_INFO:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Conference info is null.");
                    break;
                }

                ConferenceInfo conferenceInfo = {};
                ims_ConfInfo* confInfo = (ims_ConfInfo*) data;
                conferenceInfo.conferenceCallState =
                    (confInfo != nullptr && confInfo->has_confCallState) ?
                    convertProtoToHidlConferenceCallState(confInfo->confCallState) :
                        ConferenceCallState::INVALID;

                qcril_binary_data_type* confUri =
                    (qcril_binary_data_type*) (((ims_ConfInfo*)data)->conf_info_uri.arg);
                convertProtoToHidlBinaryData(confUri, conferenceInfo.confInfoUri);
                retStatus = indCb->onRefreshConferenceInfo(conferenceInfo);
                break;
            }
        case ims_MsgId_UNSOL_REFRESH_VICE_INFO:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Vice info is null.");
                    break;
                }

                ViceInfo viceInfo = {};

                qcril_binary_data_type* viceUri =
                    (qcril_binary_data_type*) (((ims_ViceInfo*)data)->vice_info_uri.arg);
                convertProtoToHidlBinaryData(viceUri, viceInfo.viceInfoUri);
                retStatus = indCb->onRefreshViceInfo(viceInfo);
                break;
            }
        case ims_MsgId_UNSOL_MODIFY_CALL:
            {
                CallModifyInfo modifyInfo = {};
                if (data != nullptr) {
                  convertProtoToHidlCallModifyInfo(*(const ims_CallModify*)data, modifyInfo);
                }
                retStatus = indCb->onModifyCall(modifyInfo);
                break;
            }
        case ims_MsgId_UNSOL_RESPONSE_CALL_STATE_CHANGED:
            {

                if (data == nullptr) {
                    QCRIL_LOG_ERROR("CallList is null.");
                    break;
                }
                hidl_vec<V1_4::CallInfo> callListV1_4;
                convertProtoToHidlCallList(*((const ims_CallList*)data), callListV1_4);
                if (indCbV1_4 != nullptr) {
                    retStatus = indCbV1_4->onCallStateChanged_1_4(callListV1_4);
                    break;
                }

                //indCbV1_4 is a nullptr. Trying to use indCbV1_3
                QCRIL_LOG_DEBUG("indCbV1_4 is nullptr. Falling back to V1.3");
                hidl_vec<V1_3::CallInfo> callListV1_3;
                convertCallListV14ToV13(callListV1_4, callListV1_3);
                if (indCbV1_3 != nullptr) {
                    retStatus = indCbV1_3->onCallStateChanged_1_3(callListV1_3);
                    break;
                }

                //indCbV1_3 is a nullptr. Trying to use indCbV1_2
                QCRIL_LOG_DEBUG("indCbV1_3 is nullptr. Falling back to V1.2");
                hidl_vec<V1_2::CallInfo> callListV1_2;
                convertCallListV13ToV12(callListV1_3, callListV1_2);
                if (indCbV1_2 != nullptr) {
                    retStatus = indCbV1_2->onCallStateChanged_1_2(callListV1_2);
                    break;
                }
                //indCbV1_2 is a nullptr. Trying to use indCbV1_1
                QCRIL_LOG_DEBUG("indCbV1_2 is nullptr. Falling back to V1.1");
                sp<V1_1::IImsRadioIndication> indCbV1_1 = getIndicationCallbackV1_1();
                hidl_vec<V1_1::CallInfo> callListV1_1;
                convertCallListV12ToV11(callListV1_2, callListV1_1);
                if(indCbV1_1 != nullptr) {
                    QCRIL_LOG_DEBUG("Invoking indCbV1_1->onCallStateChanged_1_1()");
                    retStatus = indCbV1_1->onCallStateChanged_1_1(callListV1_1);
                    break;
                }
                //indCbV1_1 is a nullptr. Trying to use indCb
                QCRIL_LOG_DEBUG("indCbV1_1 is nullptr. Falling back to V1.0");
                hidl_vec<V1_0::CallInfo> callListV1_0;
                convertCallListV11ToV10(callListV1_1, callListV1_0);
                QCRIL_LOG_DEBUG("Invoking indCb->onCallStateChanged()");
                retStatus = indCb->onCallStateChanged(callListV1_0);
                break;
            }
        case ims_MsgId_UNSOL_CALL_RING:
            {
                retStatus = indCb->onRing();
                break;
            }
        case ims_MsgId_UNSOL_RINGBACK_TONE:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Ringback tone is null.");
                    break;
                }
                ToneOperation tone = convertProtoToHidlTone(*((const ims_RingBackTone*)data));
                retStatus = indCb->onRingbackTone(tone);
                break;
            }
        case ims_MsgId_UNSOL_AUTO_CALL_REJECTION_IND:
            {
                if (data) {
                    if (indCbV1_3 != nullptr)
                    {
                        V1_3::AutoCallRejectionInfo info = {};
                        convertToHidlAutoCallRejectionInfo(
                            *((ims_AutoCallRejectionInfo*)data), info);
                        retStatus = indCbV1_3->onIncomingCallAutoRejected(info);
                    }
                }
                else
                {
                    QCRIL_LOG_ERROR("indCbV1_3 is null.");
                }
                break;
            }
        case ims_MsgId_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS:
            {
                if (data) {
                    if (indCbV1_4 != nullptr) {
                        hidl_vec<V1_4::MultiIdentityLineInfoHal> hal_info;
                        convertToHidlMultiIdentityRegistrationStatus(
                             *(static_cast<std::vector<QcrilImsMultiIdentityInfo>*>(data)),
                                     hal_info);
                        indCbV1_4->onMultiIdentityRegistrationStatusChange(hal_info);
                    }
                    else {
                        QCRIL_LOG_ERROR("indCbV1_4 is not available.");
                    }
                }
                else {
                    QCRIL_LOG_ERROR("MultiIdentity Registration Info in empty.");
                }
                break;
            }
        case ims_MsgId_UNSOL_MULTI_IDENTITY_PENDING_INFO:
            {
                if (indCbV1_4 != nullptr) {
                    indCbV1_4->onMultiIdentityInfoPending();
                }
                else {
                        QCRIL_LOG_ERROR("indCbV1_4 is not available.");
                }
                break;
            }
        case ims_MsgId_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Registration state is null.");
                    break;
                }
                RegistrationInfo reg = {};
                convertProtoToHidlRegistration(*((const ims_Registration*)data), reg);
                retStatus = indCb->onRegistrationChanged(reg);
                break;
            }
        case ims_MsgId_UNSOL_SRV_STATUS_UPDATE:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Service status is null.");
                    break;
                }
                hidl_vec<ServiceStatusInfo> serviceStatusInfoList;
                const ims_Info** ptr = (const ims_Info**)
                        (((ims_SrvStatusList*)data)->SrvStatusInfo.arg);
                convertProtoToHidlServiceInfoList(ptr, serviceStatusInfoList);
                retStatus = indCb->onServiceStatusChanged(serviceStatusInfoList);
                break;
            }
        case ims_MsgId_UNSOL_RESPONSE_HANDOVER:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Handover is null.");
                    break;
                }
                HandoverInfo handoverInfo = {};
                convertProtoToHidlHandoverInfo(*((ims_Handover*)data), handoverInfo);
                retStatus = indCb->onHandover(handoverInfo);
                break;
            }
        case ims_MsgId_UNSOL_RADIO_STATE_CHANGED:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Radio state is null.");
                    break;
                }
                RadioState state = convertProtoToHidlRadioState(*((ims_RadioStateChanged*)data));
                retStatus = indCb->onRadioStateChanged(state);
                break;
            }
        case ims_MsgId_UNSOL_SUPP_SVC_NOTIFICATION:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Supp service notification is null.");
                    break;
                }
                SuppServiceNotification suppServiceNotification = {};
                convertProtoToHidlSuppServiceNotification(*((ims_SuppSvcNotification*)data),
                        suppServiceNotification);
                retStatus = indCb->onSuppServiceNotification(
                        suppServiceNotification);
                break;
            }
        case ims_MsgId_UNSOL_MWI:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Message waiting indication is null.");
                    break;
                }
                MessageWaitingIndication messageWaitingIndication = {};
                convertProtoToHidlMessageWaitingIndication(*((ims_Mwi*)data),
                        messageWaitingIndication);
                retStatus = indCb->onMessageWaiting(messageWaitingIndication);
                break;
            }
        case ims_MsgId_UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Message waiting indication is null.");
                    break;
                }
                boolean hasBlockStatusOnWwan = false;
                BlockStatus blockStatusOnWwan = {};
                boolean hasBlockStatusOnWlan = false;
                BlockStatus blockStatusOnWlan = {};
                ims_RegistrationBlockStatus *regBlockStatus = (ims_RegistrationBlockStatus*)data;

                if (regBlockStatus->has_imsBlockStatusOnWwan) {
                    hasBlockStatusOnWwan = true;
                    convertProtoToHidlBlockStatus(regBlockStatus->imsBlockStatusOnWwan,
                            blockStatusOnWwan);
                }
                if (regBlockStatus->has_imsBlockStatusOnWlan) {
                    hasBlockStatusOnWlan = true;
                    convertProtoToHidlBlockStatus(regBlockStatus->imsBlockStatusOnWlan,
                            blockStatusOnWlan);
                }
                retStatus = indCb->onRegistrationBlockStatus(
                        hasBlockStatusOnWwan, blockStatusOnWwan,
                        hasBlockStatusOnWlan, blockStatusOnWlan);
                break;
            }
        case ims_MsgId_REQUEST_ADD_PARTICIPANT:
            {
                retStatus = respCb->addParticipantResponse(tag.token, tag.error);
                break;
            }
        case ims_MsgId_UNSOL_REQUEST_GEOLOCATION:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Geolocation indication is null.");
                    break;
                }
                ims_GeoLocationInfo* geo_info = (ims_GeoLocationInfo*)data;
                if (geo_info->has_lat == false || geo_info->has_lon == false) {
                    QCRIL_LOG_ERROR("Geolocation indication has no coordinates!");
                    break;
                }
                retStatus = indCb->onGeolocationInfoRequested(geo_info->lat,
                        geo_info->lon);
                break;
            }
        case ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG:
            {
                ImsSubConfigInfo subConfigInfo = ImsSubConfigInfo{};
                if (data != nullptr) {
                    convertProtoToHidlImsSubConfig(*((ims_ImsSubConfig*)data), subConfigInfo);
                }
                retStatus =  respCb->getImsSubConfigResponse(tag.token, tag.error,
                        subConfigInfo);
                break;
            }
        case ims_MsgId_UNSOL_IMS_SUB_CONFIG_CHANGED:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Ims sub config indication is null.");
                    break;
                }
                ImsSubConfigInfo config = ImsSubConfigInfo{};
                convertProtoToHidlImsSubConfig(*((ims_ImsSubConfig*)data), config);
                retStatus =  indCb->onImsSubConfigChanged(config);
                break;
            }
        case ims_MsgId_UNSOL_PARTICIPANT_STATUS_INFO:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Participant status information is null.");
                    break;
                }
                ParticipantStatusInfo participantStatusInfo = {};
                convertProtoToHidlParticipantStatusInfo(*((ims_ParticipantStatusInfo*)data),
                        participantStatusInfo);
                retStatus = indCb->onParticipantStatusInfo(
                        participantStatusInfo);
                break;
            }
        case ims_MsgId_UNSOL_RESPONSE_RTT_MSG_RECEIVED:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("Message waiting indication is null.");
                    break;
                }
                hidl_string rttMessage;
                if(((ims_RttMessage*)data)->rttMessage.arg != NULL){
                    convertProtoToHidlString((const char*)(((ims_RttMessage*)data)->rttMessage.arg),
                        rttMessage);
                    retStatus = indCb->onRttMessageReceived(rttMessage);
                }
                break;
            }
        case ims_MsgId_UNSOL_VOWIFI_CALL_QUALITY:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("vowifi call quality is null");
                    break;
                }
                VoWiFiCallQualityInfo voWiFiCallQuality = convertProtoToHidlVoWiFiCallQuality(
                        *((ims_VoWiFiCallQuality*)data));
                retStatus = indCb->onVoWiFiCallQuality(voWiFiCallQuality);
                break;
            }
        case ims_MsgId_UNSOL_ON_SS:
            {
                if (data == nullptr) {
                    QCRIL_LOG_ERROR("unsol on ss data is null");
                    break;
                }
                StkCcUnsolSsResult ss = {};
                convertToHidlStkCcUnsolSsResult(*((ims_ril_ss_ind_type*)data), ss);
                retStatus = indCb->onSupplementaryServiceIndication(ss);
                break;
            }
        case ims_MsgId_UNSOL_INCOMING_SMS:
            {
                if (indCbV1_2 != nullptr)
                {
                  if (data) {
                    auto mt_sms = static_cast<IncomingImsSms*>(data);
                    retStatus = indCbV1_2->onIncomingImsSms(*mt_sms);
                  }
                }
                else
                {
                  QCRIL_LOG_ERROR("Ims Radio Indication Cb for V1_2 is null");
                }
                break;
            }
        case ims_MsgId_UNSOL_SMS_STATUS_REPORT:
            {
                if (indCbV1_2 != nullptr)
                {
                  if (data) {
                    auto status_report = static_cast<ImsSmsSendStatusReport*>(data);
                    retStatus = indCbV1_2->onImsSmsStatusReport(*status_report);
                  }
                }
                else
                {
                  QCRIL_LOG_ERROR("Ims Radio Indication Cb for V1_2 is null");
                }
                break;
            }
        case ims_MsgId_UNSOL_VOPS_CHANGED:
            {
                if (indCbV1_2 != nullptr)
                {
                    if (data == nullptr) {
                        QCRIL_LOG_ERROR("Vops indication is null.");
                        break;
                    }
                    ims_VopsInfo* vops_info = (ims_VopsInfo*)data;
                    retStatus = indCbV1_2->onVopsChanged(vops_info->isVopsEnabled);
                }
                else
                {
                  QCRIL_LOG_ERROR("Ims Radio Indication Cb for V1_2 is null");
                }
                break;
            }

        case ims_MsgId_UNSOL_VOICE_INFO:
            {
                if (indCbV1_3 == nullptr) {
                    QCRIL_LOG_ERROR("Ims Radio Indication Cb for V1_3 is null");
                    break;
                }

                if (data != nullptr) {
                    V1_3::VoiceInfo voiceInfo = convertToHidlVoiceInfo(*((ims_VoiceInfo*) data));
                    retStatus = indCbV1_3->onVoiceInfoChanged(voiceInfo);
                }
                break;
            }
        default:
            break;
    }
    checkReturnStatus(retStatus);
}

void ImsRadioImpl::sendEmptyErrorResponse(int32_t token, ims_MsgId id, ims_Error error) {
    ims_MsgTag tag;
    imsRadioGetTag(token, ims_MsgType_RESPONSE, id, error, tag);
    processResponse(tag, nullptr);
}

/*
 *   @brief
 *   Process request and queue in RIL event queue.
 */
ims_Error ImsRadioImpl::processRequest(int32_t token, ims_MsgId id, void* data) {
    ims_MsgTag tag;
    imsRadioGetTag(token, id, tag);
    RIL_Errno ret = qcril_qmi_ims_flow_control_event_queue(
            QCRIL_QMI_IMS_FLOW_CONTROL_HANDLE_REQ,
            QCRIL_DATA_NOT_ON_STACK,
            qcril_qmi_ims_map_request_to_event(tag.id),
            data, qcril_qmi_ims_get_msg_size(tag.id, tag.type),
            qcril_qmi_ims_convert_ims_token_to_ril_token(
            tag.token));
    ims_Error error = qcril_qmi_ims_map_ril_error_to_ims_error(ret);
    QCRIL_LOG_ERROR("ImsRadioImpl::processRequest - Error %d", error);
    return error;
}

Return<void> ImsRadioImpl::getImsRegistrationState(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::answer(int32_t token, CallType callType, IpPresentation presentation,
        RttMode rttMode) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Answer* ansReq = (ims_Answer*) qcril_malloc(sizeof(ims_Answer));

    do {
        if (ansReq == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ansReq->has_call_type = true;
        ansReq->call_type = convertHidlToProtoCallType(callType);
        if (presentation != IpPresentation::IP_PRESENTATION_INVALID) {
            ansReq->has_presentation = true;
            ansReq->presentation = convertHidlToProtoPresentation(presentation);
        }

        if (rttMode != RttMode::RTT_MODE_INVALID) {
            ansReq->has_rttMode = true;
            ansReq->rttMode = convertHidlToProtoRttMode(rttMode);
            QCRIL_LOG_INFO("answer rtt mode = %d", ansReq->rttMode);
        }

        ret = processRequest(token, ims_MsgId_REQUEST_ANSWER, ansReq);
    } while(0);

    if (isError(ret)) {
        qcril_free(ansReq);
        ansReq = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_ANSWER, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::hangup(int32_t token, const HangupRequestInfo& hangup) {
    V1_1::HangupRequestInfo v11Hangup;
    migrateHangup(hangup, v11Hangup);
    return hangup_1_1(token, v11Hangup);
}

Return<void> ImsRadioImpl::hangup_1_1(int32_t token, const V1_1::HangupRequestInfo& hangup) {
    V1_2::HangupRequestInfo v12Hangup;
    migrateHangup(hangup, v12Hangup);
    return hangup_1_2(token, v12Hangup);
}

Return<void> ImsRadioImpl::hangup_1_2(int32_t token, const V1_2::HangupRequestInfo& hangup) {
    V1_3::HangupRequestInfo v13Hangup;
    migrateHangup(hangup, v13Hangup);
    return hangup_1_3(token, v13Hangup);
}

Return<void> ImsRadioImpl::hangup_1_3(int32_t token, const V1_3::HangupRequestInfo& hangup) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Hangup* imsHangup = (ims_Hangup*) qcril_malloc(sizeof(ims_Hangup));

    do {
        if (imsHangup == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoHangup(hangup, *imsHangup)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Failed to convert hangup payload.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (ret == ims_Error_E_SUCCESS) {
            qcril_qmi_voice_process_in_main_thread(QCRIL_EVT_IMS_SOCKET_REQ_HANGUP,
                  imsHangup, sizeof(*imsHangup),
                  qcril_qmi_ims_convert_ims_token_to_ril_token(token));
        }
    } while(0);

    if (isError(ret)) {
        qcril_free(imsHangup);
        imsHangup = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_HANGUP, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::requestRegistrationChange(int32_t token, RegState state) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Registration* imsRegistration = (ims_Registration*)qcril_malloc(
            sizeof(ims_Registration));

    do {
        if (imsRegistration == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        imsRegistration->has_state = true;
        imsRegistration->state = convertHidlToProtoRegState(state);

        ret = processRequest(token, ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE, imsRegistration);
    } while(0);

    if (isError(ret)) {
        qcril_free(imsRegistration);
        imsRegistration = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::queryServiceStatus(int32_t token)
{
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_QUERY_SERVICE_STATUS, nullptr);
    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_QUERY_SERVICE_STATUS, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::setServiceStatus(int32_t token,
        const ServiceStatusInfo& srvStatusInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Info* serviceInfo = (ims_Info*)qcril_malloc(sizeof(ims_Info));

    do {

        if (serviceInfo == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoServiceStatusInfo(srvStatusInfo, *serviceInfo)) !=
                ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Failed to convert service status info.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_SERVICE_STATUS, serviceInfo);
    } while(0);

    if (isError(ret)) {
        qcril_free(serviceInfo);
        serviceInfo = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_SERVICE_STATUS, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::hold(int32_t token, uint32_t callId) {
    ims_Error  ret = ims_Error_E_SUCCESS;
    ims_Hold* hold = (ims_Hold*)qcril_malloc(sizeof(ims_Hold));

    do {
        if (hold == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
        }

        hold->has_callId = true;
        hold->callId = callId;

        ret = processRequest(token, ims_MsgId_REQUEST_HOLD, hold);
    } while(0);

    if (isError(ret)) {
        qcril_free(hold);
        hold = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_HOLD, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::resume(int32_t token, uint32_t callId) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Resume* resume = (ims_Resume*)qcril_malloc(sizeof(ims_Resume));

    do {
        if (resume == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        resume->has_callId = true;
        resume->callId = callId;

        ret = processRequest(token, ims_MsgId_REQUEST_RESUME, resume);
    } while(0);

    if (isError(ret)) {
        qcril_free(resume);
        resume = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_RESUME, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setConfig(int32_t token, const ConfigInfo& inConfig) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_ConfigMsg* outConfig = (ims_ConfigMsg*)qcril_malloc(sizeof(ims_ConfigMsg));

    do {
        if (outConfig == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoConfig(inConfig, *outConfig)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Failed to convert config.");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_IMS_CONFIG, outConfig);
    } while(0);

    if (isError(ret)) {
        qcril_free(outConfig);
        outConfig = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_IMS_CONFIG, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::getConfig(int32_t token, const ConfigInfo& inConfig) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_ConfigMsg* outConfig = (ims_ConfigMsg*)qcril_malloc(sizeof(ims_ConfigMsg));

    do {
        if (outConfig == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoConfig(inConfig, *outConfig)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Failed to convert config.");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_GET_IMS_CONFIG, outConfig);
    } while(0);

    if (isError(ret)) {
        qcril_free(outConfig);
        outConfig = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_IMS_CONFIG, ret);
    }
    return Void();
}

void ImsRadioImpl::callForwardStatusInternal(int32_t token,
        const CallForwardInfo& callForwardInfo, ims_MsgId id) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_CallForwardInfoList* callForwardInfoList = (ims_CallForwardInfoList*)qcril_malloc(sizeof(
            ims_CallForwardInfoList));

    do {
        if (callForwardInfoList == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        //List only contains 1 element. The extra space is for null termination.
        callForwardInfoList->info.arg = qcril_malloc(
                sizeof(ims_CallForwardInfoList_CallForwardInfo*) * 2);
        ims_CallForwardInfoList_CallForwardInfo** ptr = (ims_CallForwardInfoList_CallForwardInfo**)
                (callForwardInfoList->info.arg);

        if (ptr == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ptr[0] = (ims_CallForwardInfoList_CallForwardInfo*)qcril_malloc(sizeof(
                ims_CallForwardInfoList_CallForwardInfo));

        if (ptr[0] == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoCallForwardInfo(callForwardInfo, *(ptr[0]))) !=
                ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Call forward info conversion failed.");
            break;
        }

        ret = processRequest(token, id, callForwardInfoList);
    } while(0);

    if (isError(ret)) {
        if (callForwardInfoList != nullptr) {
        ims_CallForwardInfoList_CallForwardInfo** ptr = (ims_CallForwardInfoList_CallForwardInfo**)
                (callForwardInfoList->info.arg);

            if (ptr != nullptr) {
                qcril_free(ptr[0]);
                ptr[0] = nullptr;
            }
            qcril_free(ptr);
            ptr = nullptr;
        }

        qcril_free(callForwardInfoList);
        callForwardInfoList = nullptr;
        sendEmptyErrorResponse(token, id, ret);

    }
}

Return<void> ImsRadioImpl::queryCallForwardStatus(int32_t token,
        const CallForwardInfo& callForwardInfo) {
    callForwardStatusInternal(token, callForwardInfo, ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS);
    return Void();
}

Return<void> ImsRadioImpl::setCallForwardStatus(int32_t token,
        const CallForwardInfo& callForwardInfo) {
    callForwardStatusInternal(token, callForwardInfo, ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS);
    return Void();
}

Return<void> ImsRadioImpl::getCallWaiting(int32_t token, uint32_t inServiceClass) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_ServiceClass* outServiceClass = (ims_ServiceClass*)qcril_malloc(sizeof(ims_ServiceClass));

    do {
        if (outServiceClass == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (inServiceClass != INT32_MAX) {
            outServiceClass->has_service_Class = true;
            outServiceClass->service_Class = inServiceClass;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_QUERY_CALL_WAITING, outServiceClass);
    } while(0);

    if (isError(ret)) {
        qcril_free(outServiceClass);
        outServiceClass = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_QUERY_CALL_WAITING, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setCallWaiting(int32_t token, ServiceClassStatus inStatus,
        uint32_t inServiceClass) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_CallWaitingInfo* outCallWaitingInfo = (ims_CallWaitingInfo*)qcril_malloc(sizeof(
            ims_CallWaitingInfo));

    do {
        if (outCallWaitingInfo == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (inServiceClass != INT32_MAX) {
            outCallWaitingInfo->has_service_class = true;
            outCallWaitingInfo->service_class.has_service_Class = true;
            outCallWaitingInfo->service_class.service_Class = inServiceClass;
        }

        if (inStatus != ServiceClassStatus::INVALID) {
            outCallWaitingInfo->has_service_status = true;
            outCallWaitingInfo->service_status = convertHidlToProtoServiceClassStatus(inStatus);
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_CALL_WAITING, outCallWaitingInfo);
    } while(0);

    if (isError(ret)) {
        qcril_free(outCallWaitingInfo);
        outCallWaitingInfo = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_CALL_WAITING, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::suppServiceStatus(int32_t token, int32_t operationType,
        FacilityType facilityType, const CbNumListInfo& cbNumListInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_SuppSvcRequest* suppServiceRequest = (ims_SuppSvcRequest*)qcril_malloc(sizeof(
            ims_SuppSvcRequest));

    do {
        if (suppServiceRequest == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (operationType != INT32_MAX) {
            suppServiceRequest->has_operationType = true;
            suppServiceRequest->operationType = operationType;
        }

        if (facilityType != FacilityType::FACILITY_INVALID) {
            suppServiceRequest->has_facilityType = true;
            suppServiceRequest->facilityType = convertHidlToProtoFacilityType(facilityType);
        }

        if ((ret = convertHidlToProtoNumListInfo(cbNumListInfo,
                suppServiceRequest->cbNumListType)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Failed to convert num list info.");
            break;
        }

        suppServiceRequest->has_cbNumListType = true;

        ret = processRequest(token, ims_MsgId_REQUEST_SUPP_SVC_STATUS, suppServiceRequest);
    } while(0);

    if (isError(ret)) {
        qcril_free(suppServiceRequest);
        suppServiceRequest = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SUPP_SVC_STATUS, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setSuppServiceNotification(int32_t token, ServiceClassStatus status) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_SuppSvcStatus* outSuppSvcStatus = (ims_SuppSvcStatus*)qcril_malloc(
            sizeof(ims_SuppSvcStatus));

    do {
        if (outSuppSvcStatus == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (status != ServiceClassStatus::INVALID) {
            outSuppSvcStatus->has_status = true;
            outSuppSvcStatus->status = convertHidlToProtoServiceClassStatus(status);
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION, outSuppSvcStatus);
    } while(0);

    if (isError(ret)) {
        qcril_free(outSuppSvcStatus);
        outSuppSvcStatus = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::conference(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_CONFERENCE, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_CONFERENCE, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::exitEmergencyCallbackMode(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::getClip(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_QUERY_CLIP, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_QUERY_CLIP, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::getClir(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_GET_CLIR, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_CLIR, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setClir(int32_t token, const ClirInfo& clirInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Clir* clir = (ims_Clir*) qcril_malloc(sizeof(ims_Clir));

    do {
        if (clir == nullptr) {
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (clirInfo.paramM != INT32_MAX) {
            clir->has_param_m = true;
            clir->param_m = clirInfo.paramM;
        }

        if (clirInfo.paramN != INT32_MAX) {
            clir->has_param_n = true;
            clir->param_n = clirInfo.paramN;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_CLIR , clir);
    } while(0);

    if (isError(ret)) {
        qcril_free(clir);
        clir = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_CLIR, ret);
    }
    return Void();

}

Return<void> ImsRadioImpl::getColr(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_GET_COLR, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_COLR, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setColr(int32_t token, const ColrInfo& colrInfo) {
    V1_3::ColrInfo v13ColrInfo;
    migrateColrInfo(colrInfo, v13ColrInfo);
    return setColr_1_3(token, v13ColrInfo);
}

Return<void> ImsRadioImpl::setColr_1_3(int32_t token, const V1_3::ColrInfo& colrInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Colr* colr = (ims_Colr*) qcril_malloc(sizeof(ims_Colr));

    do {
        if (colr == nullptr) {
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if (colrInfo.status != ServiceClassStatus::INVALID) {
            colr->has_status = true;
            colr->status = convertHidlToProtoServiceClassStatus(colrInfo.status);
        }

        if (colrInfo.provisionStatus != V1_3::ServiceClassProvisionStatus::INVALID) {
            colr->has_provisionStatus = true;
            colr->provisionStatus = convertHidlToProtoServiceClassProvisionStatus(
                    colrInfo.provisionStatus);
        }

        if (colrInfo.presentation != IpPresentation::IP_PRESENTATION_INVALID) {
            colr->has_presentation = true;
            colr->presentation = convertHidlToProtoPresentation(colrInfo.presentation);
        }

        if (colrInfo.hasErrorDetails) {
            if ((ret = convertHidlToProtoSipErrorInfo(colrInfo.errorDetails,
                    colr->errorDetails)) != ims_Error_E_SUCCESS) {
                QCRIL_LOG_ERROR("Failed to convert sip error info.");
                break;
            }
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SET_COLR , colr);
    } while(0);

    if (isError(ret)) {
        qcril_free(colr);
        colr = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SET_COLR, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::sendDtmf(int32_t token, const DtmfInfo& dtmfInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Dtmf* dtmf = (ims_Dtmf*) qcril_malloc(sizeof(ims_Dtmf));

    do {
        if (dtmf == nullptr) {
            QCRIL_LOG_ERROR("dtmf is null.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoDtmf(dtmfInfo, *dtmf)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Hidl to proto dtmf conversion failed.");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_DTMF , dtmf);
    } while(0);

    if (isError(ret)) {
        qcril_free(dtmf);
        dtmf = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_DTMF, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::startDtmf(int32_t token, const DtmfInfo& dtmfInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Dtmf* dtmf = (ims_Dtmf*) qcril_malloc(sizeof(ims_Dtmf));

    do {
        if (dtmf == nullptr) {
            QCRIL_LOG_ERROR("dtmf is null.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        if ((ret = convertHidlToProtoDtmf(dtmfInfo, *dtmf)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Hidl to proto dtmf conversion failed.");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_DTMF_START , dtmf);
    } while(0);

    if (isError(ret)) {
        qcril_free(dtmf);
        dtmf = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_DTMF_START, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::stopDtmf(int32_t token) {
    ims_Error ret = processRequest(token, ims_MsgId_REQUEST_DTMF_STOP, nullptr);

    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_DTMF_STOP, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::setUiTtyMode(int32_t token, const TtyInfo& ttyInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_TtyNotify* ttyNotify = (ims_TtyNotify*) qcril_malloc(sizeof(ims_TtyNotify));

    do {
        if (ttyNotify == nullptr) {
            QCRIL_LOG_ERROR("TtyNotify is null.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }
        ttyNotify->mode = convertHidlToProtoTtyMode(ttyInfo.mode);
        ttyNotify->has_mode = true;

        if (ttyInfo.userData.size() > 0) {
            qcril_binary_data_type* ptr = (qcril_binary_data_type*)ttyNotify->userData.arg;
            ptr = (qcril_binary_data_type*)qcril_malloc(sizeof(qcril_binary_data_type));

            if (ptr == nullptr) {
                QCRIL_LOG_ERROR("TtyNotify->userdata.arg is null.");
                ret = ims_Error_E_GENERIC_FAILURE;
                break;
            }

            // RIL doesn't use this at all so for now treating it as a binary_data_type
            if ((ret = convertHidlToProtoBinaryData(ttyInfo.userData, *ptr)) !=
                    ims_Error_E_SUCCESS) {
                QCRIL_LOG_ERROR("Tty binary data conversion failed.");
                break;
            }
        }

        ret = processRequest(token, ims_MsgId_REQUEST_SEND_UI_TTY_MODE, ttyNotify);
    } while(0);

    if (isError(ret)) {
        if(ttyNotify != nullptr) {
            qcril_free(ttyNotify->userData.arg);
            qcril_free(ttyNotify);
        }
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SEND_UI_TTY_MODE, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::explicitCallTransfer(int32_t token,
        const ExplicitCallTransferInfo& ectInfo) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_ExplicitCallTransfer* imsEctReq = nullptr;

    do {
        imsEctReq = (ims_ExplicitCallTransfer*) qcril_malloc(sizeof(ims_ExplicitCallTransfer));
        if (imsEctReq == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }
        if ((ret = convertHidlToProtoEctInfo(ectInfo, *imsEctReq)) != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Hidl to proto ExplicitCallTransfer request conversion failed");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER, imsEctReq);
    } while (0);

    if (isError(ret)) {
        if (imsEctReq != nullptr) {
            qcril_free(imsEctReq->targetAddress.arg);
            imsEctReq->targetAddress.arg = nullptr;
            qcril_free(imsEctReq);
            imsEctReq = nullptr;
        }
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::modifyCallInitiate(int32_t token, const CallModifyInfo& callModifyInfo)
{
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_CallModify* imsCallModifyReq = nullptr;

    do {
        imsCallModifyReq = (ims_CallModify*)qcril_malloc(sizeof(ims_CallModify));
        if (imsCallModifyReq == nullptr)
        {
            QCRIL_LOG_ERROR("memory allocation failure.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ret = convertHidlToProtoCallModifyInfo(callModifyInfo, *imsCallModifyReq);
        if (isError(ret))
        {
            QCRIL_LOG_ERROR("conversion failure.");
            break;
        }
        ret = processRequest(token, ims_MsgId_REQUEST_MODIFY_CALL_INITIATE, imsCallModifyReq);
    } while (0);

    if (isError(ret)) {
        qcril_free(imsCallModifyReq);
        imsCallModifyReq = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_MODIFY_CALL_INITIATE, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::cancelModifyCall(int32_t token, uint32_t callId) {
    ims_Error  ret = ims_Error_E_SUCCESS;
    ims_CancelModifyCall* cancelModifyCall = nullptr;

    do {
        cancelModifyCall = (ims_CancelModifyCall*)qcril_malloc(sizeof(ims_CancelModifyCall));
        if (cancelModifyCall == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        cancelModifyCall->has_callId = true;
        cancelModifyCall->callId = callId;

        ret = processRequest(token, ims_MsgId_REQUEST_CANCEL_MODIFY_CALL, cancelModifyCall);
    } while(0);

    if (isError(ret)) {
        qcril_free(cancelModifyCall);
        cancelModifyCall = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_CANCEL_MODIFY_CALL, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::modifyCallConfirm(int32_t token, const CallModifyInfo& callModifyInfo)
{
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_CallModify* imsCallModifyReq = nullptr;

    do {
        imsCallModifyReq = (ims_CallModify*)qcril_malloc(sizeof(ims_CallModify));
        if (imsCallModifyReq == nullptr)
        {
            QCRIL_LOG_ERROR("memory allocation failure.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ret = convertHidlToProtoCallModifyInfo(callModifyInfo, *imsCallModifyReq);
        if (isError(ret)) {
            QCRIL_LOG_ERROR("conversion failure.");
            break;
        }
        ret = processRequest(token, ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM, imsCallModifyReq);
    } while (0);

    if (isError(ret)) {
        qcril_free(imsCallModifyReq);
        imsCallModifyReq = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::getRtpStatistics(int32_t token) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ret = processRequest(token, ims_MsgId_REQUEST_GET_RTP_STATISTICS, nullptr);
    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_RTP_STATISTICS, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::getRtpErrorStatistics(int32_t token) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ret = processRequest(token, ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS, nullptr);
    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::addParticipant(int32_t token, const DialRequest& dialRequest) {
    V1_4::DialRequest v14dialRequest;
    migrateDialRequest(dialRequest, v14dialRequest);
    return addParticipant_1_4(token, v14dialRequest);
}

/*
 *  @brief
 *  Server side implementtaion of the IImsradio service's add participant API
 *
 */
Return<void> ImsRadioImpl::addParticipant_1_4(int32_t token, const V1_4::DialRequest& dialRequest) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_Dial* dial = nullptr;

    do {
        dial = (ims_Dial*) qcril_malloc(sizeof(ims_Dial));
        if (dial == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memmory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ret = convertHidlToProtoDial(dialRequest, *dial);
        if (isError(ret)) {
            QCRIL_LOG_ERROR("conversion failure.");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_ADD_PARTICIPANT, dial);
    } while(0);

    if (isError(ret)) {
        if (dial != nullptr) {
            qcril_free(dial->address.arg);
            dial->address.arg = nullptr;
        }
        qcril_free(dial);
        dial = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_ADD_PARTICIPANT, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::sendRttMessage(int32_t token, const hidl_string& rttMessage) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_RttMessage* imsRttMessage = nullptr;

    do {
        imsRttMessage = (ims_RttMessage*)qcril_malloc(sizeof(ims_RttMessage));
        if (imsRttMessage == nullptr)
        {
            QCRIL_LOG_ERROR("memory allocation failure.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }

        ret = convertHidlToProtoString(rttMessage, (char*&)(imsRttMessage->rttMessage.arg));
        if (isError(ret)) {
            QCRIL_LOG_ERROR("conversion failure.");
            break;
        }
        ret = processRequest(token, ims_MsgId_REQUEST_SEND_RTT_MSG, imsRttMessage);
    } while (0);

    if (isError(ret)) {
        if (imsRttMessage != nullptr) {
            qcril_free(imsRttMessage->rttMessage.arg);
        }
        qcril_free(imsRttMessage);
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SEND_RTT_MSG, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::deflectCall(int32_t token, const DeflectRequestInfo& deflectRequestInfo)
{
    ims_Error ret = ims_Error_E_SUCCESS;
    ims_DeflectCall* imsDeflectCall = nullptr;

    do {
        imsDeflectCall = (ims_DeflectCall*) qcril_malloc(sizeof(ims_DeflectCall));
        if (imsDeflectCall == nullptr) {
            QCRIL_LOG_ERROR("Failed to allocate memory.");
            ret = ims_Error_E_GENERIC_FAILURE;
            break;
        }
        if ((ret = convertHidlToProtoDeflectRequestInfo(deflectRequestInfo, *imsDeflectCall))
                != ims_Error_E_SUCCESS) {
            QCRIL_LOG_ERROR("Hidl to proto deflect request conversion failed");
            break;
        }

        ret = processRequest(token, ims_MsgId_REQUEST_DEFLECT_CALL, imsDeflectCall);
    } while (0);

    if (isError(ret)) {
        if (imsDeflectCall != nullptr) {
            qcril_free(imsDeflectCall->number.arg);
            imsDeflectCall->number.arg = nullptr;
            qcril_free(imsDeflectCall);
            imsDeflectCall = nullptr;
        }
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_DEFLECT_CALL, ret);
    }

    return Void();
}


/**
 * @brief
 * Server side implementation of the IImsRadio service's sendGeolocation API.
 *
 */
Return<void> ImsRadioImpl::sendGeolocationInfo(int32_t token, double lat, double lon,
            const AddressInfo& addressInfo) {
    V1_2::AddressInfo v12AddressInfo = {};
    migrateAddressInfo(addressInfo, v12AddressInfo);
    return sendGeolocationInfo_1_2(token, lat, lon, v12AddressInfo);
}

Return<void> ImsRadioImpl::sendGeolocationInfo_1_2(int32_t token, double lat, double lon,
            const V1_2::AddressInfo& addressInfo) {
    QCRIL_LOG_INFO("sendGeolocationInfo_1_2");
    ims_GeoLocationInfo* geo_info =
        static_cast<ims_GeoLocationInfo*>(qcril_malloc(sizeof(ims_GeoLocationInfo)));
    if (geo_info == nullptr) {
        QCRIL_LOG_ERROR("Failed to allocate memmory.");
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO,
         ims_Error_E_GENERIC_FAILURE);
        return  Void();
    }

    ims_Error ret = convertHidlToProtoGeolocation(lat, lon, addressInfo, *geo_info);
    if (! isError(ret)) {
        ret = processRequest(token, ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO, geo_info);
    }

    if (isError(ret)) {
        QCRIL_LOG_ERROR("sendGeolocationInfo_1_2 failure.");
        freeGeolocationInfo(*geo_info);
        qcril_free(geo_info);
        geo_info = nullptr;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO, ret);
    }

    return Void();
}

template<typename T>
void ImsRadioImpl::checkReturnStatus(const Return<T>& ret) {
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("checkReturnStatus: Unable to send response. Exception : %s",
                ret.description().c_str());
    }
}

boolean ImsRadioImpl::hasValidCallbacks() {
    return (mImsRadioResponseCb != nullptr && mImsRadioIndicationCb != nullptr);
}

Return<void> ImsRadioImpl::getImsSubConfig(int32_t token) {
    ims_Error ret = ims_Error_E_SUCCESS;
    ret = processRequest(token, ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, nullptr);
    if (isError(ret)) {
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::registerMultiIdentityLines(int32_t token,
        const hidl_vec<V1_4::MultiIdentityLineInfoHal>& lines) {
    ims_Error ret = ims_Error_E_GENERIC_FAILURE;
    std::vector<QcrilImsMultiIdentityInfo>* out_lines =
            new std::vector<QcrilImsMultiIdentityInfo>();
    if (out_lines) {
        for (auto& in_line : lines) {
            QcrilImsMultiIdentityInfo out_line;
            //from here only activation requests will be sent to modem
            convertToRilMultiIdentityInfo(in_line, out_line);
            out_lines->push_back(out_line);
        }
        ret = processRequest(token,
                ims_MsgId_REQUEST_REGISTER_MULTI_IDENTITY_LINES,
                out_lines);
    }
    if (isError(ret)) {
        sendEmptyErrorResponse(token,
                ims_MsgId_REQUEST_REGISTER_MULTI_IDENTITY_LINES,
                ret);
    }
    return Void();
}

Return<void> ImsRadioImpl::queryVirtualLineInfo(int32_t token, const hidl_string& msisdn) {
    ims_Error ret = ims_Error_E_GENERIC_FAILURE;
    if (!msisdn.empty()) {
        std::string* line_msisdn = new std::string(msisdn);
        ret = processRequest(token,
                ims_MsgId_REQUEST_QUERY_VIRTUAL_LINE_INFO,
                line_msisdn);
    }
    if (isError(ret)) {
        sendEmptyErrorResponse(token,
                ims_MsgId_REQUEST_QUERY_VIRTUAL_LINE_INFO,
                ret);
    }
    return Void();
}


Return<void> ImsRadioImpl::sendImsSms(int32_t token, const V1_2::ImsSmsMessage& imsSms) {
    ims_Error ret = ims_Error_E_GENERIC_FAILURE;

    auto ril_ims_msg = getRILImsSmsFormatPayload(imsSms);
    if (ril_ims_msg) {
        ret = processRequest(token, ims_MsgId_REQUEST_SEND_SMS, ril_ims_msg);
    }

    if (isError(ret)) {
        if (ril_ims_msg)
            freeRILImsSmsFormatPayload(ril_ims_msg);
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_SEND_SMS, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::acknowledgeSms(int32_t token, uint32_t messageRef,
        V1_2::ImsSmsDeliverStatusResult smsdeliverResult) {
    ims_Error ret = ims_Error_E_GENERIC_FAILURE;

    QcrilImsSmsAckData* data = new QcrilImsSmsAckData(messageRef, smsdeliverResult);
    if (data) {
        ret = processRequest(token, ims_MsgId_REQUEST_ACK_SMS, data);
    }

    if (isError(ret)) {
        if (data)
            delete data;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_ACK_SMS, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::acknowledgeSmsReport(int32_t token, uint32_t messageRef,
        V1_2::ImsSmsStatusReportResult smsstatusreport) {
    ims_Error ret = ims_Error_E_GENERIC_FAILURE;

    QcrilImsSmsAckReportData* data = new QcrilImsSmsAckReportData(messageRef, smsstatusreport);
    if (data) {
        ret = processRequest(token, ims_MsgId_REQUEST_ACK_SMS_REPORT, data);
    }

    if (isError(ret)) {
        if (data)
            delete data;
        sendEmptyErrorResponse(token, ims_MsgId_REQUEST_ACK_SMS_REPORT, ret);
    }

    return Void();
}

Return<void> ImsRadioImpl::getSmsFormat(getSmsFormat_cb _hidl_cb) {
    const char* format = "unknown";
    switch (qcril_qmi_sms_get_ims_sms_format()) {
    case QMI_RIL_SMS_FORMAT_UNKNOWN:
        format = "unknown";
        break;
    case QMI_RIL_SMS_FORMAT_3GPP:
        format = "3gpp";
        break;
    case QMI_RIL_SMS_FORMAT_3GPP2:
        format = "3gpp2";
        break;
    default:
        break;
    }

    _hidl_cb(format);

    return Void();
}

//===========================================================================
// ImsRadioImpl::sendMessage
//===========================================================================
boolean ImsRadioImpl::sendMessage(RIL_Token token, ims_MsgType type,
        ims_MsgId message_id, ims_Error error, const void* msg, int msg_len)
{
    QCRIL_NOTUSED(msg_len);
    boolean ret = 0;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO( "msg: %s(type: %d, id: %d), error: %d",
            qcril_qmi_ims_get_msg_log_str(message_id, type), type, message_id, error);

    /**
     * Send the response to the client for messages using the IImsRadio service interface via
     * the response callbacks
     */
    if (isImsRadioConnected()) {
        // Create the ims msg tag
        ims_MsgTag msg_tag;
        imsRadioGetTag(qcril_qmi_ims_free_and_convert_ril_token_to_ims_token(token),
                type, message_id, error, msg_tag);
        QCRIL_LOG_INFO( "Response: Token %d, Type %d, id %d, error %d", msg_tag.token,
                msg_tag.type, msg_tag.id, msg_tag.error);
        processResponse(msg_tag, (void *)msg);
    } else {
        QCRIL_LOG_INFO("Ims Radio service is not connected");
        ret = 1;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
    return ret;
} // ImsRadioImpl::sendMessage


//===========================================================================
// imsRadioInit
//===========================================================================
extern "C" void imsRadioInit()
{
    QCRIL_LOG_FUNC_ENTRY();
    // Register ims radio service
    imsRadioRegisterService();

    QCRIL_LOG_FUNC_RETURN();
} // imsRadioInit

//===========================================================================
// imsRadioSendMessageToFlowControl
//===========================================================================
extern "C" void imsRadioSendMessageToFlowControl(RIL_Token token, ims_MsgType type,
        ims_MsgId message_id, ims_Error error, const void* msg, int msg_len) {
    qcril_reqlist_public_type request_info;
    QCRIL_LOG_FUNC_ENTRY();

    if (E_SUCCESS == qcril_reqlist_query(QCRIL_DEFAULT_INSTANCE_ID, token, &request_info) ||
            (ims_MsgId_REQUEST_DIAL != message_id && ims_MsgId_REQUEST_ANSWER != message_id &&
            ims_MsgId_REQUEST_HANGUP_WAITING_OR_BACKGROUND != message_id)) {
        qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , token );
        mImsRadio->sendMessage(token, type, message_id, error, msg, msg_len);
    }

    QCRIL_LOG_FUNC_RETURN();
} // imsRadioSendMessageToFlowControl

//===========================================================================
// imsRadioSendMessage
//===========================================================================
extern "C" errno_enum_type imsRadioSendMessage(RIL_Token token, ims_MsgType type,
        ims_MsgId message_id, ims_Error error, const void* msg, int msg_len)
{
    errno_enum_type ret = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    imsRadioSendMessageToFlowControl(token, type, message_id, error, msg, msg_len);
    if (type == ims_MsgType_RESPONSE) {
        QCRIL_LOG_INFO("Check for any pending request in flow control %d", message_id);
        ret = qcril_qmi_ims_flow_control_event_queue(QCRIL_QMI_IMS_FLOW_CONTROL_REQ_COMPLETE,
                QCRIL_DATA_NOT_ON_STACK, qcril_qmi_ims_map_request_to_event(message_id),
                NULL, 0, token) ? E_FAILURE : E_SUCCESS;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)ret);
    return ret;
} // imsRadioSendMessage

//===========================================================================
// imsRadioSendEmptyUnsolResponse
//===========================================================================
extern "C" void imsRadioSendEmptyUnsolResponse(ims_MsgId msg_id)
{
    imsRadioSendMessage(0, ims_MsgType_UNSOL_RESPONSE, msg_id, ims_Error_E_SUCCESS,
            nullptr, 0);
} // imsRadioSendEmptyUnsolResponse


//===========================================================================
// imsRadioRegisterService
//
//===========================================================================
/*!
    @brief
    Creates and registers the IImsRadio service with name imsradio0 with
    the system

*/
/*=========================================================================*/
void imsRadioRegisterService() {
    android::status_t ret = android::OK;
    if (mImsRadio == nullptr) {
        mImsRadio = new ImsRadioImpl();

        char QTI_IMS_SERVICE_NAME[20];
        snprintf(QTI_IMS_SERVICE_NAME, sizeof(QTI_IMS_SERVICE_NAME), "%s%d",
                "imsradio", qmi_ril_get_process_instance_id());

        ret = mImsRadio->registerAsService(QTI_IMS_SERVICE_NAME);
        QCRIL_LOG_INFO("registerService: starting ImsRadioImpl %s status %d",
                QTI_IMS_SERVICE_NAME, ret);
    }
}

extern "C" boolean isImsRadioConnected() {
    return (mImsRadio != nullptr && mImsRadio->hasValidCallbacks());
}

} // namespace implementation
} // namespace V1_0
} // namespace ims
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
