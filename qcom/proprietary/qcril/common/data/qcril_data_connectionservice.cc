/******************************************************************************
@file    qcril_data_connectionservice.cc
@brief   qcril data connection service

DESCRIPTION
Basic implementaton of IDataConnection interface.
Register as data connection HAL service

******************************************************************************/

/*=============================================================================
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include "qcril_data_connectionservice.h"

extern "C" {
  #include "qcril_log.h"
}

using ::android::hardware::Status;

namespace vendor {
namespace qti {
namespace hardware {
namespace data {
namespace connection {
namespace V1_0 {

static sp<DataConnectionServiceImpl> dataConnectionService = NULL;


/*===========================================================================

FUNCTION:  DataConnectionServiceImpl::serviceDied

===========================================================================*/
void DataConnectionServiceImpl::serviceDied
(
  uint64_t,
  const ::android::wp<::android::hidl::base::V1_0::IBase> &
)
{
  QCRIL_LOG_INFO("DataConnectionServiceImpl::serviceDied: Client died.");
} /* DataConnectionServiceImpl::serviceDied */


/*===========================================================================

FUNCTION:  DataConnectionServiceImpl::setInstanceId

===========================================================================*/
void DataConnectionServiceImpl::setInstanceId
(
  int instanceId
)
{
  mInstanceId = instanceId;
} /* DataConnectionServiceImpl::setInstanceId */


/*===========================================================================

FUNCTION:  qcril_dataconnectionservice_register_service

===========================================================================*/
extern "C" uint8_t qcril_dataconnectionservice_register_service
(
  int instanceId
)
{
  android::status_t ret = -1;
  dataConnectionService = new DataConnectionServiceImpl();
  dataConnectionService->setInstanceId(instanceId);
  if (instanceId == 0) {
    ret = dataConnectionService->registerAsService("slot1");
  } else if (instanceId == 1) {
    ret = dataConnectionService->registerAsService("slot2");
  }
  QCRIL_LOG_INFO("qcril_dataconnectionservice_register_service instanceId=%d status=%d", instanceId, ret);

  return ret == android::OK;
} /* qcril_dataconnectionservice_register_service */

} // V1_0
} // connection
} // data
} // hardware
} // qti
} // vendor
