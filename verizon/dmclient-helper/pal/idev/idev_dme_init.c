#include <stdio.h>
#include <stdlib.h>

// Qualcomm
#include <qmi.h>
#include <qmi_client.h>
#include <device_management_service_v01.h>
#include <user_identity_module_v01.h>
#include <qmi_idl_lib_internal.h>
#include <qmi_cci_target_ext.h>
#include <qmi_idl_lib.h>
#include <dm_engine/ipth_dme.h>
#include <dm_engine/ipth_device_support.h>
#include <dm_engine/ipth_mtree_node.h>
#include <cutils/log.h>

#include <ipth_hal_dll.h>
#include <ipth_hal_file.h>
#include "pal_common.h"

#include "idev_shared_api.h"
#include "idev_dmapp.h"
#include "md5sum.h"
#include <unistd.h>

#define LOG_TAG "QMI_CCI"

ipth_DmeConfig_s g_DmeCfg;
DMApplication_s *g_pDmApp;
ipth_DmEngineDeviceSupport devSupport;

extern int dum_getversion_dfs(char** ver);
extern void* ipth_GetFn_DfsClient( const char *const fnName);
extern void* ipth_GetFn_ConnMO( const char *const fnName);
extern void* ipth_GetFn_DcMO( const char *const fnName);
int idev_QMI_uim_open(void);
int idev_QMI_dms_open(void);

static long qmi_handle = QMI_INVALID_CLIENT_HANDLE;

#define IPTH_DME_CM_QMI_TIMEOUT_VALUE 15000
#define IPTH_DME_QMI_CLIENT_DEV_ID "rmnet0"

static qmi_cci_os_signal_type qmi_dme_notifier_os_params;
static qmi_cci_os_signal_type qmi_dme_os_params;
static qmi_client_type dms_user_handle;
static qmi_client_type qmi_dme_notifier;

static qmi_cci_os_signal_type qmi_uim_notifier_os_params;
static qmi_cci_os_signal_type qmi_uim_os_params;
static qmi_client_type uim_user_handle;
static qmi_client_type qmi_uim_notifier;


void idev_QMI_dms_release()
{
    if (qmi_dme_notifier != NULL) {
        qmi_client_release(qmi_dme_notifier);
    }
}


void idev_QMI_uim_release()
{
    if (qmi_uim_notifier != NULL) {
        qmi_client_release(qmi_uim_notifier);
    }
}

int idev_QMI_open(void)
{
    if (qmi_handle == QMI_INVALID_CLIENT_HANDLE) {
		ALOGE("abing qmi_init\n");
        qmi_handle = qmi_init(NULL,NULL);
    	idev_QMI_dms_open();
    	idev_QMI_uim_open();
        if (qmi_handle < 0) {
            return -1;
        }
    }

	ALOGE("abing idev_QMI_open qmi_handle = 0x%x\n", qmi_handle);
    return 0;
}


void idev_QMI_release(void)
{
    if (qmi_handle >= 0) {
		ALOGE("abing qmi_release\n");
    	idev_QMI_dms_release();
    	idev_QMI_uim_release();
        qmi_release(qmi_handle);
		qmi_handle = QMI_INVALID_CLIENT_HANDLE;
    } else {
    }
}


int idev_QMI_uim_open(void)
{
    qmi_idl_service_object_type uim_get_service_object_p;
    qmi_service_info info;
    qmi_client_error_type rc;
    
    unsigned int num_entries = 1;
    unsigned int num_services;
    int ret = IP_OK;

    if (uim_user_handle != NULL) {
        return ret;
    }

    uim_get_service_object_p = uim_get_service_object_v01();
    if (uim_get_service_object_p==NULL) {
        ret = -2;
        goto RETURN;
    } else {
    }

    rc = qmi_client_notifier_init(
                                  uim_get_service_object_p,
                                  &qmi_uim_notifier_os_params,
                                  &qmi_uim_notifier
                                 );
    if(rc != QMI_NO_ERR)
    {
      ret = -3;
      goto RETURN;
    }
    
    /*Check if the services is up,if not wait on a signal
      Max.wait time = 3sec
    */
    QMI_CCI_OS_SIGNAL_WAIT(&qmi_uim_notifier_os_params,3000);
    if(qmi_uim_notifier_os_params.timed_out)
    {
        ret = -3;
        goto RETURN;
    }
    QMI_CCI_OS_SIGNAL_CLEAR(&qmi_uim_notifier_os_params);
    
    rc = qmi_client_get_service_list(uim_get_service_object_p,
                                     &info,
                                     &num_entries,
                                     &num_services);
    
    if(rc != QMI_NO_ERR)
    {
        ret = -3;
        goto RETURN;
    }

    /* Initialize a connection to first QMI control port */
    rc = qmi_client_init(&info,
                         uim_get_service_object_p,
                         NULL,
                         NULL,
                         &qmi_uim_notifier_os_params, 
                         &uim_user_handle);
    if (rc != QMI_NO_ERR) {
        ret = -3;
        goto RETURN;
    } else {
    }

RETURN:
    if (ret < 0 && qmi_uim_notifier > 0) {
        qmi_client_release(qmi_uim_notifier);
    }

    return ret;
}


int idev_QMI_dms_open(void)
{
    qmi_idl_service_object_type dms_get_service_object_p;
    qmi_service_info info;
    qmi_client_error_type rc;
    
    unsigned int num_entries = 1;
    unsigned int num_services;
    int ret = IP_OK;

    /* Initialize the qmi datastructure(Once per process ) */

    dms_get_service_object_p = dms_get_service_object_v01();
    /*
       extern const struct qmi_idl_service_object dms_qmi_idl_service_object_v01;
       dms_get_service_object_p = (qmi_idl_service_object_type)&dms_qmi_idl_service_object_v01;
    */
    if (dms_get_service_object_p==NULL) {
        ret = -2;
        goto RETURN;
    } else {
    }

    rc = qmi_client_notifier_init(
                                  dms_get_service_object_p,
                                  &qmi_dme_notifier_os_params,
                                  &qmi_dme_notifier
                                 );
    if(rc != QMI_NO_ERR)
    {
      ret = -3;
      goto RETURN;
    }
    
    /*Check if the services is up,if not wait on a signal
      Max.wait time = 3sec
    */
    QMI_CCI_OS_SIGNAL_WAIT(&qmi_dme_notifier_os_params,3000);
    if(qmi_dme_notifier_os_params.timed_out)
    {
        ret = -3;
        goto RETURN;
    }
    QMI_CCI_OS_SIGNAL_CLEAR(&qmi_dme_notifier_os_params);
    
    rc = qmi_client_get_service_list(dms_get_service_object_p,
                                     &info,
                                     &num_entries,
                                     &num_services);
    
    if(rc != QMI_NO_ERR)
    {
        ret = -3;
        goto RETURN;
    }

    /* Initialize a connection to first QMI control port */
    rc = qmi_client_init(&info,
                         dms_get_service_object_p,
                         NULL,
                         NULL,
                         &qmi_dme_os_params,
                         &dms_user_handle);
    if (rc != QMI_NO_ERR) {
        ret = -3;
        goto RETURN;
    } else {
    }

RETURN:
    if (ret < 0 && qmi_dme_notifier > 0) {
        qmi_client_release(qmi_dme_notifier);
    }

	//ALOGE("abing idev_QMI_dms_open rc = %d ret = %d\n", rc, ret);
    return ret;
}

int idev_QMI_get_DeviceId(char* deviceid, char* urn_deviceid)
{
    int ret = IP_OK;
    qmi_client_error_type rc;
    dms_get_device_serial_numbers_resp_msg_v01 resp;

    rc = qmi_client_send_msg_sync(dms_user_handle,
                                  QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                                  (void*)NULL,
                                  0,
                                  (void*)&resp,
                                  sizeof(resp),
                                  IPTH_DME_CM_QMI_TIMEOUT_VALUE);

	ALOGE("abing idev_QMI_get_DeviceId rc = %d\n", rc);
    if (rc!=QMI_NO_ERR) {
        ret = -4;
        goto RETURN;
    }

	ALOGE("abing idev_QMI_get_DeviceId rc = %d, resp.imei_valid = %d resp.meid_valid = %d, resp.esn_valid = %d resp.imei = %s\n", rc, resp.imei_valid, resp.meid_valid, resp.esn_valid, resp.imei);

    if (resp.imei_valid) {
        strcpy(deviceid, resp.imei);
        strcpy(urn_deviceid, "IMEI:");
        strcat(urn_deviceid, resp.imei);
        ret = 0;
    } else if (resp.meid_valid) {
        strcpy(deviceid, resp.meid);
        strcpy(urn_deviceid, "MEID:");
        strcat(urn_deviceid, resp.meid);
        ret = 0;
    } else if (resp.esn_valid) {
        strcpy(deviceid, resp.esn);
        strcpy(urn_deviceid, "ESN:");
        strcat(urn_deviceid, resp.esn);
        ret = 0;
    } else {
        ret = -5;
    }

RETURN:
    return ret;
}

int idev_QMI_get_SWRevision(char *rev)
{
    int ret = IP_OK;
    qmi_client_error_type rc;
    dms_get_sw_version_resp_msg_v01 resp;

    rc = qmi_client_send_msg_sync(dms_user_handle,
                                  QMI_DMS_GET_SW_VERSION_REQ_V01,
                                  (void*)NULL,
                                  0,
                                  (void*)&resp,
                                  sizeof(resp),
                                  IPTH_DME_CM_QMI_TIMEOUT_VALUE);

    if (rc != QMI_NO_ERR) {
        ret = -4;
        goto RETURN;
    }

    strcpy(rev, resp.sw_version);

RETURN:
    return ret;
}


int idev_QMI_get_ICCID(char *iccid)
{
    int ret = IP_OK;

    if (iccid == NULL) {
        return IP_ERR;
    }
#if 1
#if 0
    qmi_client_error_type rc;
    dms_uim_get_iccid_resp_msg_v01 resp;

    rc = qmi_client_send_msg_sync(uim_user_handle,
                                  QMI_DMS_UIM_GET_ICCID_REQ_V01,
                                  (void*)NULL,
                                  0,
                                  (void*)&resp,
                                  sizeof(resp),
                                  IPTH_DME_CM_QMI_TIMEOUT_VALUE);

    if (rc != QMI_NO_ERR) {
        idev_log("Error: qmi_client_send_msg_sync error return %d\r\n", rc);
        ret = -4;
        goto RETURN;
    }

    idev_log("UIM ICCID: %s\n\r", resp.uim_id);
    strcpy(iccid, resp.uim_id);
#else
    int i, pos, len;
    unsigned char* data;
    char tmp;
	qmi_client_error_type              qmi_err_code = 0;
	uim_read_transparent_req_msg_v01   qmi_read_trans_req;
	uim_read_transparent_resp_msg_v01  read_trans_resp;

	memset(&read_trans_resp, 0, sizeof(uim_read_transparent_resp_msg_v01));
	memset(&qmi_read_trans_req, 0, sizeof(uim_read_transparent_req_msg_v01));

	qmi_read_trans_req.read_transparent.length = 0;
	qmi_read_trans_req.read_transparent.offset = 0;

    qmi_read_trans_req.file_id.file_id = 0x2FE2;//
	qmi_read_trans_req.file_id.path_len = 2;
    qmi_read_trans_req.file_id.path[0] = 0x00;
	qmi_read_trans_req.file_id.path[1] = 0x3F;

    qmi_read_trans_req.session_information.session_type =
				UIM_SESSION_TYPE_PRIMARY_GW_V01;
	qmi_read_trans_req.session_information.aid_len = 0;

	qmi_err_code = qmi_client_send_msg_sync(uim_user_handle,
					QMI_UIM_READ_TRANSPARENT_REQ_V01,
					(void *)&qmi_read_trans_req,
					sizeof(uim_read_transparent_req_msg_v01),
					(void *) &read_trans_resp,
					sizeof(uim_read_transparent_resp_msg_v01),
					IPTH_DME_CM_QMI_TIMEOUT_VALUE);
	ALOGE("abing iccid qmi_err_code = %d\n", qmi_err_code);

    if (QMI_NO_ERR == qmi_err_code) {
		if (read_trans_resp.read_result_valid) {
			len = read_trans_resp.read_result.content_len;
			data = read_trans_resp.read_result.content;
            for (i = 0; i < len; i++) {
                pos = i * 2;
                sprintf(&iccid[pos], "%02X", data[i]);
                tmp = iccid[pos];
                iccid[pos] = iccid[pos + 1];
                iccid[pos + 1] = tmp;
            }
            iccid[i * 2] = 0;
		}
        else {
        }
    }
    else {
    }

#endif
#else
    strcpy(iccid, "89860115830003736256");
#endif

RETURN:
    return ret;
}


int idev_QMI_get_IMSI(char *imsi)
{
    int ret = IP_OK;

    if (imsi == NULL) {
        return IP_ERR;
    }
#if 1
#if 0
    qmi_client_error_type rc;
    dms_uim_get_imsi_resp_msg_v01 resp;

    rc = qmi_client_send_msg_sync(uim_user_handle,
                                  QMI_DMS_UIM_GET_IMSI_REQ_V01,
                                  (void*)NULL,
                                  0,
                                  (void*)&resp,
                                  sizeof(resp),
                                  IPTH_DME_CM_QMI_TIMEOUT_VALUE);

    if (rc != QMI_NO_ERR) {
        idev_log("Error: qmi_client_send_msg_sync error return %d\r\n", rc);
        ret = -4;
        goto RETURN;
    }

    idev_log("IMSI: %s\n\r", resp.imsi);
    strcpy(imsi, resp.imsi);
#else
    int i, pos, len;
    unsigned char* data;
    char tmp;
	qmi_client_error_type              qmi_err_code = 0;
	uim_read_transparent_req_msg_v01   qmi_read_trans_req;
	uim_read_transparent_resp_msg_v01  read_trans_resp;

	memset(&read_trans_resp, 0, sizeof(uim_read_transparent_resp_msg_v01));
	memset(&qmi_read_trans_req, 0, sizeof(uim_read_transparent_req_msg_v01));

	qmi_read_trans_req.read_transparent.length = 0;
	qmi_read_trans_req.read_transparent.offset = 0;

    qmi_read_trans_req.file_id.file_id = 0x6F07;//
	qmi_read_trans_req.file_id.path_len = 4;
    qmi_read_trans_req.file_id.path[0] = 0x00;
	qmi_read_trans_req.file_id.path[1] = 0x3F;
    qmi_read_trans_req.file_id.path[2] = 0x20;
	qmi_read_trans_req.file_id.path[3] = 0x7F;

    qmi_read_trans_req.session_information.session_type =
				UIM_SESSION_TYPE_PRIMARY_GW_V01;
	qmi_read_trans_req.session_information.aid_len = 0;

	qmi_err_code = qmi_client_send_msg_sync(uim_user_handle,
					QMI_UIM_READ_TRANSPARENT_REQ_V01,
					(void *)&qmi_read_trans_req,
					sizeof(uim_read_transparent_req_msg_v01),
					(void *) &read_trans_resp,
					sizeof(uim_read_transparent_resp_msg_v01),
					IPTH_DME_CM_QMI_TIMEOUT_VALUE);

    if (QMI_NO_ERR == qmi_err_code) {
		if (read_trans_resp.read_result_valid) {
			len = read_trans_resp.read_result.content_len;
			data = read_trans_resp.read_result.content;
            for (i = 0; i < len; i++) {
                pos = i * 2;
                sprintf(&imsi[pos], "%02X", data[i]);
                tmp = imsi[pos];
                imsi[pos] = imsi[pos + 1];
                imsi[pos + 1] = tmp;
            }
            imsi[i * 2] = 0;

            if (strncmp(imsi, "809", 3) == 0) {
                //memcpy(imsi, &imsi[3], i * 2 - 3);
                //imsi[i * 2 - 3] = 0;
                strcpy(imsi, &imsi[3]);
            }
		}
        else {
        }
    }
    else {
    }

#endif
#else
    strcpy(iccid, "89860115830003736256");
#endif

RETURN:
    return ret;
}



int idev_dme_init(void)
{
    int ret, sessNum;
    ipth_DmeVersionInfo_s ver;


    g_pDmApp = malloc( sizeof(DMApplication_s));
    if (g_pDmApp == NULL) {
        return -2;
    }

    memset(g_pDmApp, 0, sizeof(DMApplication_s));

    devSupport.displayAlert = idev_uiAlertTypeHandler;
    devSupport.displayProgressBar = idev_displayProgressBar;
    devSupport.updateProgressBar = idev_updateProgressBar;
    devSupport.reportDmeStatus = idev_dmeStatusCallback;
    devSupport.appData = g_pDmApp;

    g_DmeCfg.mode = IPTH_DME_MODE_COOPERATIVE;
    g_DmeCfg.configFile = NULL;
    g_DmeCfg.workingDir = DME_WORKING_DIR;
    g_DmeCfg.pluginDir = NULL;
    g_DmeCfg.treeStorageId = DME_TREE_STORAGE_ID;
    g_DmeCfg.recoveryStorageId = DME_RECOVERY_STORAGE_ID;
    g_DmeCfg.devSupport = &devSupport;

    // library name defined from Binder.ddf
    /* Initialize the whole enchilada */
    idev_QMI_open();

    ipth_DmeVersionInfo_s dme_ver;

    return 0;
}


int idev_checkDmeRecovery(void)
{
    char path[256];

    memset(path,0x0,sizeof(path));
    strcpy(path, g_DmeCfg.workingDir);
    strcat(path,"/");
    strcat(path,g_DmeCfg.recoveryStorageId);
    return IPTH_FALSE;
}

int idev_startFotaUpdate(idev_TaskInitiator initiator)
{
    ipth_SessionAttr_s attr;
    ipth_GenericAlert alert = {1226, NULL, NULL};
    int ret, sessNum;


    /* Prepare Generic Alert 1226 */
    return ret;
}

int idev_sendFotaResult( char *err_code, char *correlator)
{
    ipth_SessionAttr_s attr;
    ipth_GenericAlert alert = {1226, NULL, NULL};
    int ret, sessNum;


    if (correlator != NULL) {
        alert.correlator = malloc(strlen(correlator) + 1);
        strcpy(alert.correlator, correlator);
    }
    /* Prepare Generic Alert 1226 */

    return ret;
}


int idev_startNiDMSess(void)
{
    int ret;
    ipth_NotificationAttr_s niAttr;
    int byte_index;


    for (byte_index=0; byte_index<g_pDmApp->borNMsgLen; byte_index++) {
    }

    free( g_pDmApp->borNMsg );
    return IPTH_OK;
}


int quec_updateTrees(void)
{
    idev_QMI_dms_open();
    idev_QMI_uim_open();
    //idev_update_MtreeDevDetail();
    //idev_update_MtreeDMAcc();
//    idev_CheckFwVUpdate();
    idev_QMI_uim_release();
    idev_QMI_dms_release();
    //idev_test_dcmo();

    return 0;
}
