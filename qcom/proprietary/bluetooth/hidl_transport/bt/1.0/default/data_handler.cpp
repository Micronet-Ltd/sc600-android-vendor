/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <pthread.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <hidl/HidlSupport.h>
#include "data_handler.h"
#include "logger.h"
#include <cutils/properties.h>
#include "bluetooth_address.h"
#include <utils/Log.h>
#include <signal.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.bluetooth@1.0-data_handler"

namespace {

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

DataHandler *data_handler = nullptr;

std::mutex init_mutex_;

std::mutex evt_wait_mutex_;
std::condition_variable event_wait_cv;
bool event_wait;
uint16_t awaited_evt;

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using InitializeCallback = std::function<void(bool success)>;

bool soc_need_reload_patch = true;
bool DataHandler :: caught_signal = false;
unsigned int ClientStatus;
unsigned int RxthreadStatus;

void DataHandler::data_service_sighandler(int signum)
{
  // lock is required incase of multiple binder threads
  std::unique_lock<std::mutex> guard(init_mutex_);
  ALOGW("%s: Caught Signal: %d", __func__, signum);

  if (data_handler) {
    data_handler->SetSignalCaught();
    if (data_handler->Close(TYPE_BT))
        goto cleanup;
    if (data_handler->Close(TYPE_FM))
        goto cleanup;
    if (data_handler->Close(TYPE_ANT))
        goto cleanup;
    return;
cleanup:
    ALOGI("%s: deleting data_handler", __func__);
    delete data_handler;
    data_handler = NULL;
    kill(getpid(), SIGKILL);
  }
}

int DataHandler::data_service_setup_sighandler(void)
{
  struct sigaction sig_act;

  ALOGI("%s: Entry", __func__);
  memset(&sig_act, 0, sizeof(sig_act));
  sig_act.sa_handler = data_handler->data_service_sighandler;
  sigemptyset(&sig_act.sa_mask);

  sigaction(SIGTERM, &sig_act, NULL);

  return 0;
}

bool DataHandler::Init(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  // lock required incase of multiple binder threads
  ALOGW("DataHandler:: Init()");
  std::unique_lock<std::mutex> guard(init_mutex_);
  if (!data_handler) {
    data_handler = new DataHandler();
    data_handler->data_service_setup_sighandler();
  }
  return data_handler->Open(type, init_cb, data_read_cb);
}

void DataHandler::CleanUp(ProtocolType type)
{
  // lock is required incase of multiple binder threads
  std::unique_lock<std::mutex> guard(init_mutex_);

  ALOGW("DataHandler::CleanUp()");
  if (data_handler && data_handler->Close(type)) {
    delete data_handler;
    data_handler = NULL;
  }
}

bool DataHandler::isProtocolAdded(ProtocolType pType)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  bool status = false;
  ALOGD("%s:",__func__);

  it = protocol_info_.find(pType);
  if (it != protocol_info_.end()) {
    status = true;
  }
  else {
    status = false;
  }

  ALOGI("%s: status:%d",__func__, status);
  return status;
}

DataHandler * DataHandler::Get()
{
  return data_handler;
}

DataHandler::DataHandler() {
  soc_type_ = GetSocTypeInt();
  logger_ = Logger::Get();
}

bool DataHandler::IsSocAlwaysOnEnabled()
{
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  property_get("persist.vendor.service.bdroid.soc.alwayson", value, "false");
  return (strcmp(value, "true") == 0) && (soc_type_ == BT_SOC_CHEROKEE);
}

BluetoothSocType DataHandler::GetSocType()
{
  return soc_type_;
}

BluetoothSocType DataHandler::GetSocTypeInt()
{
  int ret = 0;
  char soc[PROPERTY_VALUE_MAX];

  ret = property_get("vendor.qcom.bluetooth.soc", soc, NULL);

  if (ret == 0) {
    ALOGW("vendor.qcom.bluetooth.soc prop  not set, so using pronto.\n ");
    soc_type_ = BT_SOC_DEFAULT;
  } else {
    ALOGW("vendor.qcom.bluetooth.soc set to %s\n", soc);
    if (!strncasecmp(soc, "rome", sizeof("rome"))) {
      soc_type_ = BT_SOC_ROME;
    } else if (!strncasecmp(soc, "cherokee", sizeof("cherokee"))) {
      soc_type_ = BT_SOC_CHEROKEE;
    } else if (!strncasecmp(soc, "ath3k", sizeof("ath3k"))) {
      soc_type_ = BT_SOC_AR3K;
    } else if (!strncasecmp(soc, "napier", sizeof("napier"))) {
      soc_type_ = BT_SOC_NAPIER;
    } else if (!strncasecmp(soc, "hastings", sizeof("hastings"))) {
      soc_type_ = BT_SOC_HASTINGS;
    } else {
      ALOGI("vendor.qcom.bluetooth.soc not set, so using pronto.\n");
      soc_type_ = BT_SOC_DEFAULT;
    }
  }
  return soc_type_;
}

// this is used to send the actual packet.
size_t DataHandler::SendData(ProtocolType ptype, HciPacketType packet_type,
                             const uint8_t *data, size_t length)
{
  /* Stop proceeding further when the SSR is in progress, At any point
   * HAL daemon may be killed.
  */
  if (diag_interface_.isSsrTriggered()) {
    ALOGD("<%s: give up Tx since crash dump has started>", __func__);
    return 0;
  }

  if (CheckSignalCaughtStatus()) {
    ALOGD("%s: Return as SIGTERM Signal is caught", __func__);
    return 0;
  }

  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  {
    it = protocol_info_.find(ptype);
    if (it == protocol_info_.end()) {
      ALOGE("%s: NO entry found for the protocol %d \n", __func__, ptype);
      return 0;
    }
    if (init_status_ != INIT_STATUS_SUCCESS) {
      ALOGE("%s: BT Daemon not initialized, ignore packet", __func__);
      return 0;
    }
  }

  UpdateRingBuffer(packet_type, data, length);
  if (controller_ != nullptr)
     return controller_->SendPacket(packet_type, data, length);
  return 0;
}

#ifdef DUMP_RINGBUF_LOG
void DataHandler::AddHciCommandTag(char* dest_tag_str, struct timeval& time_val, char * opcode)
{
  uint32_t w_index = 0;

  memset(dest_tag_str, 0, RX_TAG_STR_LEN);
  add_time_str(dest_tag_str, &time_val);

  w_index += TIME_STAMP_LEN;
  snprintf(dest_tag_str+w_index, strlen(opcode) + 1, "%s", opcode);
}
#endif

void DataHandler::OnPacketReady(ProtocolType ptype, HciPacketType type,
                                const hidl_vec<uint8_t>*hidl_data)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;

  uint16_t len = hidl_data->size();
  const uint8_t* data = hidl_data->data();


  logger_->ProcessRx(type, data, len);

  if (logger_->IsControllerLogPkt(type, data, len)) {
    ALOGV("%s:Received a controller log packet\n", __func__);
    if(!logger_->IsHciFwSnoopEnabled()) {
      delete hidl_data;
      return;
    }
  }

  if (type == HCI_PACKET_TYPE_EVENT && data_handler->isAwaitedEvent(data, len)) {
    ALOGW("%s: Received event for command sent internally: %02x %02x \n",
            __func__, data[3], data[4]);
    delete hidl_data;
    event_wait = true;
    event_wait_cv.notify_all();
    return;
  }

  ProtocolCallbacksType *cb_data = nullptr;
  {
    if (diag_interface_.GetCleanupStatus(ptype)) {
      if (GetRxthreadStatus(ptype)) {
        SetRxthreadStatus(false, ptype);
        ALOGW("Skip sending packet to client as cleanup in process\n");
      }
      delete hidl_data;
      return;
    }
    std::unique_lock<std::mutex> guard(internal_mutex_);
    // update the pending Init cb and other callbacks
    it = protocol_info_.find(ptype);
    if (it != protocol_info_.end()) {
      cb_data = (ProtocolCallbacksType*)it->second;
    }
  }

#ifdef WCNSS_IBS_ENABLED
  if (controller_ != nullptr && !diag_interface_.isSsrTriggered())
    controller_->StartRxThreadTimer();
#endif

  // execute callbacks here
  if(cb_data != nullptr && !cb_data->is_pending_init_cb) {
    cb_data->data_read_cb(type, hidl_data);
  }

  delete hidl_data;

}

// signal handler
void DataHandler::usr1_handler(int /* s */)
{
  bool status = TRUE;

  ALOGI("%s: exit\n", __func__);
  Wakelock :: UnlockWakelockMutex();
  pthread_exit(&status);
}

unsigned int  DataHandler :: GetRxthreadStatus(ProtocolType Type)
{
  return (RxthreadStatus & (0x01 << Type));
}

void DataHandler :: SetRxthreadStatus(bool status, ProtocolType Type)
{
  if (status)
    RxthreadStatus = (RxthreadStatus | (0x01 << Type));
  else
    RxthreadStatus = (RxthreadStatus & (~(0x01 << Type)));

}

unsigned int  DataHandler :: GetClientStatus(ProtocolType Type)
{
  return (ClientStatus & (0x01 << Type));
}

void DataHandler :: SetClientStatus(bool status, ProtocolType Type)
{
  if (status)
    ClientStatus = (ClientStatus | (0x01 << Type));
  else
    ClientStatus = (ClientStatus & (~(0x01 << Type)));

}

bool DataHandler::Open(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  std::unique_lock<std::mutex> guard(internal_mutex_);

  /* Don't register new client when SSR in progress. This avoids
   * Crash as we will kill daemon after collecting the dump.
   */
  if (diag_interface_.isSsrTriggered()) {
    ALOGE("<%s: Returning as SSR in progress>", __func__);
    return false;
  }

  if (isProtocolAdded(type)) {
    ALOGE("<%s: Returning as protocol already added>", __func__);
    return false;
  }

  ALOGI("Open init_status %d \n", init_status_);
  SetClientStatus(true, type);
  SetRxthreadStatus(true, type);

  // update the pending Init cb and other callbacks
  it = protocol_info_.find(type);
  if (it == protocol_info_.end()) {
    ProtocolCallbacksType *cb_data  = new (ProtocolCallbacksType);
    cb_data->type = type;
    cb_data->is_pending_init_cb = true;
    cb_data->init_cb = init_cb;
    cb_data->data_read_cb = data_read_cb;
    protocol_info_[type] = cb_data;
  }
  switch (init_status_) {
    case INIT_STATUS_INITIALIZING:
      return true;
      break;
    case INIT_STATUS_SUCCESS:
      it = protocol_info_.find(type);
      if (it != protocol_info_.end()) {
        ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it->second;
        cb_data->is_pending_init_cb = false;
        cb_data->init_cb(true);
      }
      return true;
      break;
    case INIT_STATUS_FAILED:
      init_thread_.join();
    // fallback to below case
    case INIT_STATUS_IDLE:
      init_status_ = INIT_STATUS_INITIALIZING;
      if (!IsSocAlwaysOnEnabled()) {
        soc_need_reload_patch = true;
      }
      ALOGI("%s: soc_need_reload_patch = %d",
                  __func__, soc_need_reload_patch);
#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Init();
#endif
      break;
  }

  init_thread_ = std::thread([this]() {
    bool status = false;
    struct sigaction old_sa, sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = usr1_handler;
    sigaction(SIGUSR1, &sa, &old_sa);

    if(soc_type_ == BT_SOC_SMD) {
      controller_ = static_cast<Controller *> (new MctController(soc_type_));
    } else {
      controller_ = static_cast<Controller *> (new UartController(soc_type_));
    }
    if (controller_) {
      status = controller_->Init([this](ProtocolType ptype, HciPacketType type,
                                        const hidl_vec<uint8_t> *hidl_data)   {
                                   OnPacketReady(ptype, type, hidl_data);
                                 });
    }

    std::unique_lock<std::mutex> guard(internal_mutex_);
    if (status) {
      init_status_ = INIT_STATUS_SUCCESS;
      if (!soc_need_reload_patch) {
        guard.unlock();
        sendCommandWait(HCI_RESET_CMD);
        sendCommandWait(HCI_WRITE_BD_ADDRESS);
        guard.lock();
      }
      soc_need_reload_patch = false;
      ALOGI("Firmware download succeded.");
    } else {
      ALOGW("Controller Init failed ");
      init_status_ = INIT_STATUS_FAILED;
      soc_need_reload_patch = true;
      delete controller_;
      controller_ = nullptr;

      logger_->PrepareDumpProcess();
#ifdef DUMP_IPC_LOG
      logger_->DumpUartIpcLogs();
#endif

#ifdef DUMP_RINGBUF_LOG
      logger_->DumpRingBuffers(REASON_PATCH_DLDNG_FAILED_SOCINIT);
#endif

      sync();

      //Reset logger so that we can get correct log file index next time.
      logger_->Cleanup();
    }

    std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
    for (auto& it: protocol_info_) {
      ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it.second;
      cb_data->is_pending_init_cb = false;
      cb_data->init_cb(status);
    }
    // clear the list if the controller open call fails
    if (!status) {
      protocol_info_.clear();
    }
  });
  return true;
}

bool DataHandler::Close(ProtocolType type)
{
  if (!controller_) {
#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif
    return false;
  }

  controller_->WaitforCrashdumpFinish();

  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  bool status = false;
  bool Cleanup_Status = true;

  ALOGI("DataHandler:: init_status %d", init_status_);
  ALOGD("%s: Signal close to Diag interface", __func__);
  if (!diag_interface_.SignalHALCleanup(type)) {
    ALOGE("%s: Returning as SSR or cleanup in progress", __func__);
    Cleanup_Status = false;
  }

  std::unique_lock<std::mutex> guard(internal_mutex_);
  it = protocol_info_.find(type);
  if (it != protocol_info_.end()) {
    ProtocolCallbacksType *cb_data =
      reinterpret_cast<ProtocolCallbacksType *> (it->second);
    delete (cb_data);
    protocol_info_.erase(it);
  }

  SetClientStatus(false, type);
  /* Stop moving forward if the HAL Cleanup is in process */
  if (!Cleanup_Status)
    return false;

  if (!IsSocAlwaysOnEnabled()) {
    soc_need_reload_patch = true;
  }

  if (!protocol_info_.size()) {
    protocol_info_.clear();

    if(data_handler && data_handler->soc_type_ == BT_SOC_CHEROKEE ) {
      data_handler->sendCommandWait(SOC_PRE_SHUTDOWN_CMD);
    }

    guard.unlock();
    if (init_status_ == INIT_STATUS_SUCCESS) {
      /* Cleanup returns false when dump collected and other thread
       * is in process of doing post dump procedure.
       */
      if (!controller_->Cleanup()) {
        ALOGW("Skip controller cleanup as other thread is in process of cleanup: %s", __func__);
        return false;
      }
      ALOGW("controller Cleanup done");
      if (!data_handler->CheckSignalCaughtStatus()) {
        delete controller_;
        controller_ = nullptr;
      }
    }
    guard.lock();

    if (init_status_ > INIT_STATUS_IDLE ) {
      if ( INIT_STATUS_INITIALIZING == init_status_) {
        pthread_kill(init_thread_.native_handle(), SIGUSR1);

        // Sleep for 50 ms to ensure that thread exit happens
        usleep(50 * 1000);

        // Ensure acquired mutex(s) are released
        logger_->UnlockRingbufferMutex();
        controller_->UnlockControllerMutex();

        logger_->PrepareDumpProcess();

#ifdef DUMP_IPC_LOG
        logger_->DumpUartIpcLogs();
#endif

#ifdef DUMP_RINGBUF_LOG
        logger_->DumpRingBuffers(REASON_CLOSE_RCVD_DURING_INIT);
#endif

        sync();

        // close the transport
        controller_->Disconnect();

#ifdef WAKE_LOCK_ENABLED
        Wakelock :: CleanUp();
#endif

        //Reset logger so that we can get correct log file index next time.
        ALOGD("%s: Doing Logger Cleanup", __func__);
        logger_->Cleanup();

        ALOGE("Killing daemon to recover as firmware download is stuck");
        kill(getpid(), SIGKILL);
      }

      if (init_thread_.joinable()) {
        init_thread_.join();
        ALOGI("DataHandler:: joined Init thread \n");
      }

      init_status_ = INIT_STATUS_IDLE;
    }

#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif

    status = true;
  }
  diag_interface_.SignalEndofCleanup(type);
  return status;
}

void DataHandler::UpdateRingBuffer(HciPacketType packet_type, const uint8_t *data, int length)
{
#ifdef DUMP_RINGBUF_LOG
  gettimeofday(&time_hci_cmd_arrived_, NULL);
  snprintf(last_hci_cmd_timestamp_.opcode, OPCODE_LEN, "0x%x%x", data[0], data[1]);
  AddHciCommandTag(last_hci_cmd_timestamp_.hcicmd_timestamp,
                   time_hci_cmd_arrived_, last_hci_cmd_timestamp_.opcode);
  logger_->SaveLastStackHciCommand(last_hci_cmd_timestamp_.hcicmd_timestamp);
#endif
  logger_->ProcessTx(packet_type, data, length);
}

void DataHandler::sendCommandWait(HciCommand cmd)
{
  std::unique_lock<std::mutex> guard(evt_wait_mutex_);
  if(init_status_ != INIT_STATUS_SUCCESS)
    return;

  switch (cmd) {
    case HCI_RESET_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x03, 0x0C, 0x00};
      int length = 3;

      ALOGI("Sending HCI Reset \n");
      UpdateRingBuffer(packet_type, data, length);
      if (!controller_->SendPacket(packet_type, data, length)) {
        ALOGD("Unable to send HCI Reset \n");
        return;
      }
      awaited_evt = *(uint16_t *)(&data);
      break;
    }

    case SOC_PRE_SHUTDOWN_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x08, 0xFC, 0x00};
      int length = 3;

      ALOGI("Sending Pre-shutdown command \n");
      UpdateRingBuffer(packet_type, data, length);
      if (!controller_->SendPacket(packet_type, data, length)) {
        ALOGD("Unable to send Pre-shutdown command \n");
        return;
      }
      awaited_evt = *(uint16_t *)(&data);
      break;
    }

    case HCI_WRITE_BD_ADDRESS:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      uint8_t data[HCI_WRITE_BD_ADDRESS_LENGTH] = {0x14, 0xFC, 0x06};

      BluetoothAddress::GetLocalAddress(&data[HCI_WRITE_BD_ADDRESS_OFFSET]);
      BluetoothAddress::le2bd(&data[HCI_WRITE_BD_ADDRESS_OFFSET]);

      ALOGI("Sending HCI_WRITE_BD_ADDRESS command \n");
      UpdateRingBuffer(packet_type, data, HCI_WRITE_BD_ADDRESS_LENGTH);
      if (!controller_->SendPacket(packet_type, data, HCI_WRITE_BD_ADDRESS_LENGTH)) {
        ALOGD("Unable to send HCI_WRITE_BD_ADDRESS \n");
        return;
      }
      awaited_evt = *(uint16_t *)(&data);
      break;
    }

    default:
      return;
  }

  event_wait = false;
  event_wait_cv.wait_for(guard, std::chrono::milliseconds(500), []{return event_wait;});
  awaited_evt = 0;
}

inline bool DataHandler::isAwaitedEvent(const uint8_t *buff, uint16_t len)
{
  if(len < 5)
    return false;

  if(awaited_evt && (*(uint16_t *)(&buff[3])) == awaited_evt)
    return true;
  else
    return false;
}

void DataHandler::SetSignalCaught()
{
  caught_signal = true;
}

bool DataHandler::CheckSignalCaughtStatus()
{
  return caught_signal;
}
} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
