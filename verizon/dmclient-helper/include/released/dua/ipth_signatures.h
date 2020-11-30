/****************************************************************************************
 * TODO                                                                                 *
 *                                                                                      *
 * Copyright (c) 2008 InnoPath Software, Inc. All rights reserved.                      *
 *                                                                                      *
 * Legal Statements                                                                     *
 *                                                                                      *
 * THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS IS" BASIS *
 * WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND, WHETHER EXPRESS,  *
 * IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE, TRADE OR OTHERWISE INCLUDING*
 * ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, MERCHANTABLE QUALITY,       *
 * FITNESS FOR ANY PARTICULAR PURPOSE OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES *
 * NOT REPRESENT OR WARRANT THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS, *
 * UNINTERRUPTED, ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR         *
 * AVAILABLE AT ANY TIME.                                                               *
 *                                                                                      *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE FOR ANY *
 * DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE, PUNITIVE OR OTHER    *
 * DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED TO ANY LOSS OF PROFITS OR *
 * REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST BUSINESS, LOST BUSINESS            *
 * OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM WORK STOPPAGE, COSTS OF OVERHEAD, *
 * COSTS OF COVER, LOSS OF ANTICIPATED BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO *
 * THIS AGREEMENT, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS  *
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF  *
 * THE ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.                           *
 *                                                                                      *
 ***************************************************************************************/

/*
 * CURRENT CVS CHECKIN ID:
 * -----------------------
 * @(#) $Id:  $
 */

#ifndef IPTH_SIGNATURES_H
#define IPTH_SIGNATURES_H

/* DMA signatures */
#define IDEV_MEMORY_CARD_READ_PARAM ("ip_int32 idev_mc_read(ip_int32 fd, void * buffer, ip_uint32 count)")
#define IDEV_MEMORY_CARD_OPEN_PARAM ("ip_int32 idev_mc_open(const ip_char * filename, ip_int32 oflag, ip_int32 mode)")
#define IDEV_MEMORY_CARD_CLOSE_PARAM ("ip_int16 idev_mc_close(ip_int32 fd)")
#define IDEV_MEMORY_CARD_GET_FILE_SIZE_PARAM ("ip_long idev_mc_getFileSize(ip_int32 fd)")
#define IDEV_USER_INTERFACE_MESSAGE_PARAM ("ip_int16 idev_userInterfaceMessage (const ip_char* name, ip_uiMessageType_e msgType, void* msgInfo)")
#define IDEV_GET_USER_AGENT_PARAM ("ip_int16 idev_getUserAgent(ip_char*AgentBuf, ip_uint32* len, ip_uint32 maxlen)")
#define IDEV_LOCK_DEVICE_PARAM ("ip_int16 idev_lockDevice(ip_ilawmo_s *item)")
#define IDEV_WIPE_DEVICE_PARAM ("ip_int16 idev_wipeDevice(ip_ilawmo_s *item)")
#define IDEV_UNLOCK_DEVICE_PARAM ("ip_int16 idev_unlockDevice(ip_ilawmo_s *item)")
#define IDEV_USER_INTERACT_PARAM ("ip_int16 idev_userInteract(ip_int32 dmSessionNum, ip_uiAlertCmd_s* cmd)")
#define IDEV_COMM_OPEN_PARAM ("ip_uint32 idev_comm_open( void )")
#define IDEV_COMM_CLOSE_PARAM ("void idev_comm_close(ip_uint32 h)")
#define IDEV_COMM_CLOSE_QUERY_STATUS_PARAM ("ip_commStatus_e idev_comm_queryCloseStatus(ip_uint32 h)")
#define IDEV_COMM_CONNECT_PARAM ("void idev_comm_connect(ip_uint32 h, ip_char* addr, ip_uint16 PortNum, ip_int32 TimeOut)")
#define IDEV_COMM_QUERY_CONNECT_STATUS_PARAM ("ip_commStatus_e idev_comm_queryConnectStatus(ip_uint32 h)")
#define IDEV_COMM_SEND_PARAM ("void idev_comm_send(ip_uint32 h, ip_char* SendBuf, ip_uint32 SendLen, ip_int32 TimeOut)")
#define IDEV_COMM_QUERY_SEND_STATUS_PARAM ("ip_commStatus_e idev_comm_querySendStatus(ip_uint32 h, ip_uint32* SentLen)")
#define IDEV_COMM_RECV_PARAM ("void idev_comm_recv(ip_uint32 h, ip_char* RecvBuf, ip_uint32 RecvLen, ip_int32 Timeout)")
#define IDEV_COMM_QUERY_RECV_STATUS_PARAM ("ip_commStatus_e idev_comm_queryRecvStatus(ip_uint32 h, ip_uint32* RecvLen)")

#define IDEV_COMM_SET_SECURE_INFO_PARAM ("const ip_int16 idev_comm_setSecureInfo( ip_int32 protocolID, ip_uint32 cipherSuites[], \        ip_int32 cipherSuitesSize, ip_char  certificate[], ip_int32 certificateSize)")

#define IDEV_COMM_SECURE_INIT_CONTEXT_PARAM ("const ip_int16 idev_comm_secureInitContext( void )")
#define IDEV_COMM_SECURE_DELETE_CONTEXT_PARAM ("void idev_comm_secureDeleteContext( void )")
#define IDEV_LOG_STRING_PARAM ("void idev_logString(ip_char* string)")
#define IDEV_HTTP_INITIALIZE_PARAM ("ip_httpInterface_s *idev_http_initialize( void )")
#define IDEV_HTTP_TERMINATE_PARAM ("void idev_http_terminate( ip_httpInterface_s *httpInterface )")
#define IDEV_HTTP_TERMINATE_ASYN_PARAM ("ip_commStatus_e idev_httpTerminateAsync( ip_httpInterface_s *httpInterface )")
#define IDEV_ACCESS_PARAM ("ip_int32 idev_access(ip_char * path, ip_int32 mode)")
#define IDEV_CHMOD_PARAM ("ip_int32 idev_chmod(ip_char * filename, ip_int32 pmode)")
#define IDEV_UNLINK_PARAM ("ip_int32 idev_unlink(ip_char * filename)")
#define IDEV_RENAME_PARAM ("ip_int32 idev_rename(ip_char * oldname, ip_char * newname)")
#define IDEV_OPEN_PARAM ("ip_int32 idev_open(ip_char * filename, ip_int32 oflag, ip_int32 pmode)")
#define IDEV_CLOSE_PARAM ("ip_int32 idev_close(ip_int32 fd)")
#define IDEV_READ_PARAM ("ip_int32 idev_read(ip_int32 fd, void * buffer, ip_uint32 count)")
#define IDEV_WRITE_PARAM ("ip_int32 idev_write(ip_int32 fd, void * buffer, ip_uint32 count)")
#define IDEV_LSEEK_PARAM ("ip_long idev_lseek(ip_int32 fd, ip_long offset, ip_int32 origin)")
#define IDEV_CHOWN_PARAM ("ip_int32 idev_chown(const ip_char *filename, ip_int32 owner, ip_int32 group)")
#define IDEV_MKDIR_PARAM ("ip_int32 idev_mkdir(const ip_char *dirname, ip_int32 mode)")
#define IDEV_RMDIR_PARAM ("ip_int32 idev_rmdir(const ip_char *dirname)")
#define IDEV_SYMLINK_PARAM ("ip_int32 idev_symlink(const ip_char *oldpath, const ip_char *newpath)")
#define IDEV_LINK_PARAM ("ip_int32 idev_link(const ip_char *oldpath, const ip_char *newpath)")
#define IDEV_LCHOWN_PARAM ("ip_int32 idev_lchown(const ip_char *filename, ip_int32 owner, ip_int32 group)")
#define IDEV_GET_EUID_PARAM ("ip_uint32 idev_geteuid(void)")
#define IDEV_GET_ERRNO_PARAM ("ip_int32 idev_getErrno(void)")
#define IDEV_GET_DISK_FREE_SPACE_RTOS_PARAM ("ip_int32 idev_getDiskFreeSpaceRTOS(const ip_char *rootname, ip_uint32 *size)")
#define IDEV_REWRITE_PROGRESS_NOTICE_RTOS_PARAM ("void idev_rewriteProgressNoticeRTOS(ip_int32 rate)")
#define IDEV_DOWNLOAD_PROGRESS_NOTICE_PARAM ("void idev_downloadProgressNotice (ip_int32 rate)")
#define IDEV_READ_DELTA_DATA_PARAM ("ip_int16 idev_readDeltaData(ip_char *buffPtr, ip_uint32 buffSize, ip_uint32 offset)")
#define IDEV_WRITE_DELTA_DATA_PARAM ("ip_int16 idev_writeDeltaData(ip_char* deltaPtr, ip_uint32 diff_size, ip_uint32 offset, ip_uint32 total_size)")
#define IDEV_GET_PACKAGE_SIZE_PARAM ("ip_uint32 idev_getMaxPackageSize(void)")
#define IDEV_GET_DEVICE_USER_INFO_PARAM ("ip_int16 idev_getDeviceUserInfo(ip_deviceInfo_s *deviceInfo, ip_userInfo_s *userInfo)")
#define IDEV_GET_CURRENT_TIME_PARAM ("ip_uint32 idev_getCurrentTime(void)")
#define IDEV_RAND_PARAM ("void idev_rand(void)")
#define IDEV_ATTACH_TIMER_HANDLER_PARAM ("ip_int32 idev_attachTimerHandler(ip_int32 time, void (*timer_handler) (void))")
#define IDEV_DETACH_TIMER_HANDLER_PARAM ("ip_int32 idev_detachTimerHandler(ip_int32 timerID)")
#define IDEV_REGISTER_EVENTS_PARAM ("ip_int16 idev_registerEvents(ip_int32 ID,ip_uint32* eventList,(void) (*ipth_eventsHandler) (ip_diagEvent_s *outputEvent), ip_int32 debugLevel,ip_char* classIds)")
#define IDEV_UNREGISTER_EVENTS_PARAM ("ip_int16 idev_unregisterEvents(ip_int32 ID,ip_uint32 *eventList,ip_int32 flag)")
#define IDEV_GET_DEVICE_INFO_PARAM ("ip_int16 idev_getDeviceInfo(ip_int32 device_ID, ip_int32 devIndex, ip_int32 *devDiagDataSize)")
#define IDEV_GET_DEVICE_DATA_PARAM ("ip_int16 idev_getDeviceData(ip_int32 device_ID, ip_int32 devIndex, ip_char *diagData)")
#define IDEV_GET_BATTERY_INFO_PARAM ("ip_int16 idev_getBatteryInfo(ip_battery_s *battDiagData)")
#define IDEV_GET_RF_DATA_SIZE_PARAM ("ip_int16 idev_getRFDataSize(ip_uint32 eventID, ip_uint32 *rfDiagDataSize)")
#define IDEV_GET_RF_DATA_PARAM ("ip_int16 idev_getRFData(ip_uint32 eventID, ip_char *rfDiagData)")
#define IDEV_GET_LOCATION_PARAMS_PARAM ("ip_int16 idev_getLocationParams(ip_locParams_s *locParams)")
#define IDEV_IS_GPS_CAPABLE_PARAM ("ip_bool idev_isGPSCapable(void)")
#define IDEV_GET_GPS_LOCATION_PARAMS_PARAM ("ip_int16 idev_getGPSLocationParams(ip_gpsLocParams_s *GPSlocParams)")
#define IDEV_IS_ROAMING_PARAM ("ip_bool idev_isRoaming(void)")
#define IDEV_GET_PHONE_NUMBER_PARAM ("int idev_getPhoneNumber(char * phone_id, char * phone_number, unsigned int * length)")
#define IDEV_GET_MEMORY_INFO_PARAM ("int idev_getMemoryInfo(ip_memoryInfo_s * memory_info)")
#define IDEV_GET_BEARER_INFO_PARAM ("int idev_getBearerInfo(char * bearer_id, ip_bearerInfo_s * bearer_info)")
#define IDEV_REGISTER_RESET_HANDLER_PARAM ("ip_int16 idev_registerResetHandler(ip_int32 (*diag_reset_handler) (void *p_data, ip_int32 datasize)")
#define IDEV_UNREGISTER_RESET_HANDLER_PARAM ("ip_int16 idev_unregisterResetHandler(void)")
#define IDEV_CHECK_RESET_STATUS_PARAM ("ip_int16 idev_checkResetStatus(ip_resetStatus_s *status)")
#define IDEV_GET_EMAIL_SETTING_PARAM ("ip_int16 idev_getEmailSetting(ip_emailSetting_s *emailSetting)")
#define IDEV_REPLACE_EMAIL_SETTING_PARAM ("ip_int16 idev_replaceEmailSetting(ip_emailSetting_s *emailSetting)")
#define IDEV_ADD_EMAIL_SETTING_PARAM ("ip_int16 idev_addEmailSetting(ip_emailSetting_s *emailSetting)")
#define IDEV_DELETE_EMAIL_SETTING_PARAM ("ip_int16 idev_deleteEmailSetting(ip_emailSetting_s *emailSetting)")
#define IDEV_DEBUG_LOG_RTOS_PARAM ("ip_int16 idev_debugLogRTOS(ip_uint16 debugLevel, ip_uint8 * pLogByteStr, ip_uint16 LogByteStrSize)")

/* DUA signatures */

#define IDEV_GET_DELTA_DATA_PTR_PARAM ("ip_int16 idev_getDeltaDataPtr(ip_char **file_data_ptr, ip_uint32* max_buff_len)")

#define IDEV_GET_WORKING_ROM_INFO_PARAM ("ip_int16 idev_getWorkingROMInfo(ip_uint16 *romNum, ip_uint16 *blockNum, ip_uint16 *numBlock)")

#define IDEV_GET_WORKING_RAM_AREA_PARAM ("void* idev_getWorkingRAMArea(ip_uint32 size)")

#define IDEV_FREE_WORKING_RAM_AREA_PARAM ("void idev_freeWorkingRAMArea(void* ramPtr)")

#define IDEV_READ_ROM_BLOCK_PARAM ("ip_int16 idev_readROMBlock (ip_uint16 romNo, ip_uint16 blockNo, void *blockData, ip_int32 *bytesRead)")

#define IDEV_ERASE_ROM_BLOCK_PARAM ("ip_int16 idev_eraseROMBlock (ip_uint16 romNo, ip_uint16 blockNo)")

#define IDEV_WRITE_ROM_BLOCK_PARAM ("ip_int16 idev_writeROMBlock(ip_uint16 romNo, ip_uint16 blockNo, void *blockData)")

#define IDEV_WRITE_ROM_BLOCK_EXTRA_PARAM ("ip_int16 idev_writeROMBlock(ip_blkInfo_s *blk, ip_dataBuff_s *data, ip_dataBuff_s* extra)")

#define IDEV_GET_DEV_INFO_PARAM ("ip_int32 idev_getDevInfo(ip_devInfo_s *devInfo)")

#define IDEV_READ_CONFIG_PARAM ("ip_int16 idev_readConfig(ip_int16 whichCopy, void *target, ip_int16 size)")

#define IDEV_WRITE_CONFIG_PARAM ("ip_int16 idev_writeConfig(ip_int16 whichCopy, void *src, ip_int16 size)")

#define IDEV_REWRITE_PROG_NOTICE_PARAM ("void idev_rewriteProgressNotice(ip_int32 rate)")

#define IDEV_REWRITE_PROG_NOTICE_WITH_UPDATE_DIR_PARAM ("void idev_rewriteProgressNoticeWithUpdateDir(ip_int32 rate, ip_bool isRollback)")

#define IDEV_GET_OPERATION_MODE_PARAM ("ip_int16 idev_getOperationMode(ip_operationMode_e *mode)")

#define IDEV_DEBUG_LOG_PARAM ("ip_int16 idev_debugLog(ip_uint16 debugLevel, ip_uint8 * pLogByteStr, ip_uint16 LogByteStrSize)")

#define IDEV_GET_WDT_PARAM_PARAM ("ip_uint32 idev_getWDTParam(void)")

#define IDEV_RESET_WDT_PARAM ("void idev_ResetWDT(void)")

#define IDEV_GET_DESCRYPTION_KEY_PARAM ("ip_int16 idev_getDecryptionKey(ip_cfgKeyType_e key_type, ip_cfgDecryptionType_e decrypt_type, ip_byte *key, ip_uint16 key_len   )")

#define IDEV_GET_ENCODE_PLUGIN_PARAM ("ip_int16 idev_getEncodePlugin(ip_uint8 plugin_id, ip_encodePlugin_s *plugin)")

#define IDEV_GET_AVAILABLE_RAM_PARAM ("void* idev_getAvailableRAM(ip_uint32 *size)")

#define IDEV_GET_RAM_ALLOC_APIS ("ip_int16 idev_getRamAllocApis(ip_memApis_s *mem_apis)")

#define IDEV_SET_TIMER_PARAM ("ip_int32 idev_setTimer(ip_int32 time, void (*timer_handler) (ip_int32 *ctx),ip_int32 *ctx)")

#define IDEV_CANCEL_TIMER_PARAM ("ip_int32 idev_cancelTimer(ip_int32 *ctx)")

/* InnoPath APIs */
/* DUA APIs */

#define IPTH_DELTA_UPDATE_AGENT_PARAM ("ip_int32 ipth_dua_deltaUpdateAgent(ip_duaInput_s *input, ip_duaOut_s *output)")
#define IPTH_DUA_GET_STATUS_PARAM ("ip_int16 ipth_dua_getStatus(ip_status_s *statusInfo)")
#define IPTH_DUA_GET_VERSION_INFO_PARAM ("ip_int16 ipth_dua_getVersionInfo(ip_versionInfo_s *verInfo)")
#define IPTH_DUA_SET_UPDATE_PROGRESS_PARAM ("ip_int16 ipth_dua_setUpdateProgress(ip_updtProgress_e progress)")
#define IPTH_DUA_SET_DOWNLOAD_MODE_PARAM ("ip_int16 ipth_dua_setDownloadMode(ip_dnldMode_e mode)")
#define IPTH_DUA_VERIFYPKG_PARAM ("ip_int16 ipth_dua_verifyPkg(ip_pkgVerifyLevel_e level, ip_piUpdateType_e pkgType)")
#define IPTH_DUA_TOGGLE_UPDATE_DIRECTION_PARAM ("ip_int32 ipth_dua_toggleUpdateDirection(void)")

#endif /* IPTH_SIGNATURES_H */
