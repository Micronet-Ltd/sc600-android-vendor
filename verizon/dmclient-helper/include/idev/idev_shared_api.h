#ifndef __IDEV_SHARED_API_H__
#define __IDEV_SHARED_API_H__

#include <stdio.h>
#include <stdlib.h>

#include "ipth_type.h"
#include "dm_engine/ipth_device_support.h"
#include "ipth_hal_typedef.h"
#include "dm_engine/ipth_dme_types.h"
#include "dm_engine/ipth_error.h"
#include "dm_engine/ipth_dla.h"

#define IP_FUMO_DEVREQ_STR	"org.openmobilealliance.dm.firmwareupdate.devicerequest"
#define IP_FUMO_USRREQ_STR	"org.openmobilealliance.dm.firmwareupdate.userrequest"
#define IP_FUMO_DAU_STR "org.openmobilealliance.dm.firmwareupdate.downloadandupdate"
#define FOTA_ROOT_UIR		"./ManagedObjects/FUMO"

#define DME_WORKING_DIR "/usr/fota_ip_v"
#define DME_TREE_STORAGE_ID "mgmtree"
#define DME_RECOVERY_STORAGE_ID "dme_main.bak"
#define DME_PKG0_LISTENER_Q_UNIQUE_KEY 0x1A2B3D4E
#define DME_PKG0_LISTENER_Q_MSG_LENGTH 40

/* ANDROID
#define UPDATE_CHUNK_SIZE	(64*1024)	//TODO: find out the download chunk size to replace this macro
#define DIFF_PACKAGE_NAME "ipth_package.bin"
#define TASK_DESC_ID "/data/data/com.innopath.activecare/app_dme/fumo.txt"
#define DD_LOCAL_COPY_NAME  "/data/data/com.innopath.activecare/app_dme/download_descriptor.txt" //download descriptor used by update status screen after reboot
#define SESSION_TYPE_NAME "/data/data/com.innopath.activecare/app_dme/session.txt"

//save the update status data to this file for displaying when Check Update Status menu is clicked
#define UPDATE_STATUS_FILE	"/data/data/com.innopath.activecare/app_dme/updateStatus.txt"
#define PKG_DL_DATE_TIME_FILE	"/data/data/com.innopath.activecare/app_dme/downloadTime.txt"
#define UPDATE_STATUS_LEN	1024

#define ROM_INTERFACE_NAMED_PIPE	"/cache/recovery/fota/fota_pipe"
*/

#define UPDATE_CHUNK_SIZE	(64*1024)	//find out the download chunk size to replace this macro
#define DIFF_PACKAGE_NAME "ipth_package.bin"
#define TASK_DESC_ID  DME_WORKING_DIR"/fumo.txt"
#define DD_LOCAL_COPY_NAME DME_WORKING_DIR"/download_descriptor.txt" //download descriptor used by update status screen after reboot
#define SESSION_TYPE_NAME DME_WORKING_DIR"/session.txt"

//save the update status data to this file for displaying when Check Update Status menu is clicked
#define UPDATE_STATUS_FILE	 DME_WORKING_DIR"/updateStatus.txt"
#define PKG_DL_DATE_TIME_FILE	DME_WORKING_DIR"/downloadTime.txt"
#define UPDATE_STATUS_LEN	1024

#define RECOVER_SHUTDOWN_LOCK "/usr/shutdown.omadm.lock"
#define IPTH_DME_TASK_INFO_FILENAME DME_WORKING_DIR"/idev_TaskDesc"


typedef enum {
    IPTH_FOTA_UIALERT_DLDD,
    IPTH_FOTA_UIALERT_UTS,
    IPTH_FOTA_UIALERT_REBOOT,
    IPTH_FOTA_UIALERT_URS,
    IPTH_FOTA_UIALERT_NULL
} ipth_FOTA_UIAlert;


/* Task objective */
typedef enum {
    IP_TASKINIT_UNDEF = 0,
    IP_TASKINIT_SERVER,
    IP_TASKINIT_CLIENT,
    IP_TASKINIT_USER,
    IP_TASKINIT_RECOVER,
    IP_TASKINIT_DMCANCEL
} idev_TaskInitiator;

typedef enum {
    IP_TASK_STATE_IDLE = 0,
    IP_TASK_STATE_STARTUP,
    IP_TASK_STATE_INPROGRESS,
    IP_TASK_STATE_END,
    IP_TASK_STATE_UNDEFINED
} idev_TaskState;

/* Total client types */
typedef enum {
    IP_DMAPP_UNDEF = 0,
    IP_DMAPP_BOOTSTRAP,
    IP_DMAPP_NOTIFICATION,
    IP_DMAPP_DMSESSION,
    IP_DMAPP_DLSCREEN,
    IP_DMAPP_DL_PENDING,
    IP_DMAPP_DLPROGRESS,
    IP_DMAPP_UTSCREEN,
    IP_DMAPP_UT_PENDING,
    IP_DMAPP_UPDATE,
    IP_DMAPP_REDMSESSION,
} idev_DeviceManageApp;

/* Task structure */
typedef struct {
    idev_TaskInitiator TaskInitiator;
    idev_TaskState TaskState;
    idev_DeviceManageApp CurApp;
    ip_uint16 SessionID;
    ip_int32 FumoResult;
    ip_bool NiQueued;
    ip_bool rebootRequired;
    ip_int32 DMSessionStatus;
    void* DMMsgQueueID; /* device manager requests queue */
    void* UIMsgQueueID; /* async UI requests here, do not block on anything but input queue */
    ip_bool NoNewUpdate;
    ip_int16 dss_nh;
} idev_TaskDesc;

typedef struct {
    long mtype;    /* message type, also sender's pid */
    ip_uint32 mlen;     /* length of mdata */
    ip_byte mdata[DME_PKG0_LISTENER_Q_MSG_LENGTH];  /* message data, acutally DM pkg0 body ex) 0000000000000000000000000000000002D00000001234066D6173746572 */
} idev_Pkg0_MsgType;


struct quec_ip_dmaccParas {
    char *servId;
    char *authType;//BASIC or DIGEST, for client and server
    char *addr;//http://...  or https://...
    char *port;//80 or 443
    char *sauthname;//sever name
    char *sauthsec;
    char *cauthname;//client name
    char *cauthsec;
};

struct quec_ip_hostDevice {
    char *manu;
    char *model;
    char *swv;
    char *fwv;
    char *hwv;
    char *datestamp;
    char *deviceid;
};


#if 1
#define IPTH_DME_CMD_PIPE_NAME  "/data/ipth_dme_cmd"
#define IPTH_DME_URC_PIPE_NAME  "/data/ipth_dme_urc"

typedef enum {
    QL_ODM_URC_DME,
    QL_ODM_URC_DMACC,
    QL_ODM_URC_FUMO,
    QL_ODM_URC_UI,
    QL_ODM_URC_HOSTDEVICE,
    QL_ODM_URC_UNKONWN
} ql_odm_urc_type;


void quec_odm_interaction_init(void);
bool quec_odm_interaction_post(ql_odm_urc_type urc_type, char *msg);
int quec_odm_interaction_get(char *uimsg, int size);
int quec_odm_interaction_dme_post(char *msg);
int quec_odm_interaction_fumo_post(char *msg);
int quec_odm_interaction_dmacc_get(struct quec_ip_dmaccParas *pdmacc);
int quec_odm_interaction_ui_post(ipth_UIAlertType alter, char *msg);
int quec_odm_interaction_ui_get(ipth_UIAlertType alter, char *msg);


#endif


#ifdef __cplusplus
extern "C" {
#endif

extern char g_uiInput[100];
extern int g_respUI;
extern idev_TaskDesc gTaskDesc;

extern int idev_dme_init(void);

extern int idev_uiAlertTypeHandler(ipth_UIAlertData *uiCmdData, void *appData, ipth_DmeCompletionCallback_s *pcb);
extern void* idev_displayProgressBar(const char* name, void *appData, ipth_DmeCompletionCallback_s *pcb);
extern void idev_updateProgressBar(void* bar, int percent);
extern void idev_dmeStatusCallback(ipth_StatusEvent_s evt, void *pData);
extern int idev_checkDmeRecovery(void);

extern int idev_startNiDMSess(void);
extern int idev_startFotaUpdate(idev_TaskInitiator initiator);
extern int idev_app_init(void);
extern int idev_uiAlertFOTAHandler(ipth_UIAlertData *uiCmdData, void *appData, ipth_DmeCompletionCallback_s *pcb, ipth_Dla_DD *Dla_DD, ipth_FOTA_UIAlert screenType);

extern ip_int16 idev_init_TaskDesc(void);
extern ip_int16 idev_update_TaskDesc(void);
extern ip_int16 idev_reset_TaskDesc(void);

extern int idev_update_MtreeAuth(void);
extern char *get_DME_Status_Msg(int code);
extern int quec_updateDMAcc(struct quec_ip_dmaccParas *dmaccParas);
extern int quec_updateTrees(void);
extern int quec_updateFwVerDate(void);
extern int quec_getDMAcc(struct quec_ip_dmaccParas *dmaccParas);
extern void quec_getHostDevice(struct quec_ip_hostDevice *hostDevice);
extern void quec_updateHostDevice(struct quec_ip_hostDevice *hostDevice);


#ifdef __cplusplus
}
#endif

#endif /* __IDEV_SHARED_API_H__ */
