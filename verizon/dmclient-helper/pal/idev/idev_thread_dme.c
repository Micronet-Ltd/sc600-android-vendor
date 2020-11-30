#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "ipth_type.h"

#include <dm_engine/ipth_dme.h>

#include "idev_shared_api.h"
#include "idev_thread_dme.h"
#include "idev_dmapp.h"


bool fumo_user_confirmed = 0;
static int fumo_update = 1;


/* A generic queue item structure */
typedef struct ip_queueMsg {
    IPTH_DMEHandleEvent msg;
    ip_long param;
    struct ip_queueMsg *next; /* Next item */
} *ip_queueMsgPtr_s, ip_queueMsg_s;

/* Queue handle definition */
typedef struct ip_queue_sag {
    ip_queueMsgPtr_s root; /* root of a queue */
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} *ip_queuePtr_s, ip_queue_s;

typedef struct ip_securityAttributes {
    ip_uint32 nLength;
    void *lpSecurityDescriptor;
    ip_bool bInheritHandle;
} ip_securityAttributes_s, *ip_securityAttributesPtr_s;


char const *const gDMEHandleEvent_msg[] = {
    "IP_EVT_INIT_APP", // 0
    "IP_EVT_INIT_DME", // 1
    "IP_EVT_RECOVER_DME", // 2
    "IP_EVT_START_NI_DM_SESS", // 3
    "IP_EVT_START_UI_FOTA", // 4
    "IP_EVT_START_CI_FOTA", // 5
    "IP_EVT_RUN_DME", // 6
    "IP_EVT_TERM_DMS", // 7
    "IP_EVT_REPORT_DM_CANCEL", // 8
    "IP_EVT_DLDD_HANDLE", // 9
    "IP_EVT_PROGRESS_HANDLE", // 10
    "IP_EVT_UTS_HANDLE", // 11
    "IP_EVT_NETERR_HANDLE", // 12
    "IP_EVT_DMALERT_HANDLE", // 13
    "IP_EVT_REBOOT_DME", // 14
    "IP_EVT_EXIT_DME", // 15
    "IP_EVT_START_SEND_FOTA_RESULT", // 16
    "IP_EVT_PROGRESS_RESUME", // 17
    "IP_EVT_DME_NULL", // 18
};  // IPTH_DMEHandleEvent

char const *const gUIHandleEvent_msg[] = {
    "IP_EVT_UI_INITDM", // Initial DM UI. From beginning of DM session to before 'Download Confirm Screen'
    "IP_EVT_UI_DLDD", // Download description. To display 'Download Confirm Screen'
    "IP_EVT_UI_UTS", // Update Timer Screen. To display 'Update Confirm Screen'
    "IP_EVT_UI_PRORGESS", // Download Progress bar. To display download progress bar
    "IP_EVT_UI_UCS", // Update Complete Screen. To display FOTA(download/update) result
    "IP_EVT_UI_NETERR", // DM session Network error. To display Network Error message. Used in DM session only.
    "IP_EVT_UI_DT_RESUME_DT", // In case of 'Download Confirm Screen' interruption. To return back to 'Download Confirm Screen'
    "IP_EVT_UI_DL_RESUME_DL", // In case of ongoing download interruption. To return back to download progress bar screen.
    "IP_EVT_UI_UT_RESUME_UT", // In case of 'Update Confirm Screen' interruption. To return back to 'Update Confirm Screen'
    "IP_EVT_UI_UCS_RESUME_UCS", // In case of 'Update Complete Screen' interruption. To return back to 'Update Complete Screen'
    "IP_EVT_UI_DMALERT", // DM session UI Alert. To display DM UI Alert screen.
    "IP_EVT_UI_EXIT", // Exit.
    "IP_EVT_UI_NULL", // End of event index.
};  //IPTH_UIHandleEvent

//extern char gfumoCorrelator[64];


int check_snprintf(char *const str, size_t const size, const char *const format, ...)
{
    va_list ap;
    va_start(ap, format);
    int const n = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (n < 0) { /* error */
        return size;  /* remaining space is 0 */
    }
    if (size<=(unsigned)n) { /* overflow even for equality: terminating '\0' */
        return size;  /* remaining space is 0 */
    }
    return n;  /* append is: check_snidev_log(n + str, size - n, ...) */
}


idev_TaskDesc* GetTaskDesc()
{
	return NULL;
}


static char const *str_queueID(void* queueID)
{
    idev_TaskDesc* pTaskDesc;

    pTaskDesc = GetTaskDesc();

    if (queueID==pTaskDesc->DMMsgQueueID) return "DM";
    if (queueID==pTaskDesc->UIMsgQueueID) return "UI";
//	if (queueID==pTaskDesc->uiDatQueueID) return "UIdata";
    return "?Q?";
}

char *str_msg(void* queueID, int msg)
{
    switch (msg) {
    case IP_EVT_INIT_APP:
        return "IP_EVT_INIT_APP";
    case IP_EVT_INIT_DME:
        return "IP_EVT_INIT_DME";
    case IP_EVT_RECOVER_DME:
        return "IP_EVT_RECOVER_DME";
    case IP_EVT_START_NI_DM_SESS:
        return "IP_EVT_START_NI_DM_SESS";
    case IP_EVT_START_UI_FOTA:
        return "IP_EVT_START_UI_FOTA";
    case IP_EVT_START_CI_FOTA:
        return "IP_EVT_START_CI_FOTA";
    case IP_EVT_RUN_DME:
        return "IP_EVT_RUN_DME";
    case IP_EVT_TERM_DMS:
        return "IP_EVT_TERM_DMS";
    case IP_EVT_REPORT_DM_CANCEL:
        return "IP_EVT_REPORT_DM_CANCEL";
    case IP_EVT_DLDD_HANDLE:
        return "IP_EVT_DLDD_HANDLE";
    case IP_EVT_PROGRESS_HANDLE:
        return "IP_EVT_PRORGESS_HANDLE";
    case IP_EVT_UTS_HANDLE:
        return "IP_EVT_UTS_HANDLE";
    case IP_EVT_NETERR_HANDLE:
        return "IP_EVT_NETERR_HANDLE";
    case IP_EVT_DMALERT_HANDLE:
        return "IP_EVT_DMALERT_HANDLE";
    case IP_EVT_REBOOT_DME:
        return "IP_EVT_REBOOT_DME";
    case IP_EVT_EXIT_DME:
        return "IP_EVT_EXIT_DME";
    case IP_EVT_START_SEND_FOTA_RESULT:
        return "IP_EVT_START_SEND_FOTA_RESULT";
    case IP_EVT_PROGRESS_RESUME:
        return "IP_EVT_PROGRESS_RESUME";
    case IP_EVT_DME_NULL:
        return "IP_EVT_DME_NULL";
    default:
        return "";
    }
}


char *get_DME_Status_Msg(int code)
{
    switch (code) {
    case IPTH_OK:
        return "IPTH_OK";
        break;
    case IPTH_ERR:
        return "IPTH_ERR";
        break;
    case IPTH_ERR_INVALID_ARGUMENT:
        return "IPTH_ERR_INVALID_ARGUMENT";
        break;
    case IPTH_ERR_OPERATION_DISALLOWED:
        return "IPTH_ERR_OPERATION_DISALLOWED";
        break;
    case IPTH_ERR_MEM_ALLOC:
        return "IPTH_ERR_MEM_ALLOC";
        break;
    case IPTH_ERR_NOT_EXIST:
        return "IPTH_ERR_NOT_EXIST";
        break;
    case IPTH_ERR_NOT_INITIALIZED:
        return "IPTH_ERR_NOT_INITIALIZED";
        break;
    case IPTH_ERR_ALREADY_INITIALIZED:
        return "IPTH_ERR_ALREADY_INITIALIZED";
        break;
    case IPTH_ERR_OPEN:
        return "IPTH_ERR_OPEN";
        break;
    case IPTH_ERR_ALREADY_OPEN:
        return "IPTH_ERR_ALREADY_OPEN";
        break;
    case IPTH_ERR_READ:
        return "IPTH_ERR_READ";
        break;
    case IPTH_ERR_WRITE:
        return "IPTH_ERR_WRITE";
        break;
    case IPTH_ERR_THREAD:
        return "IPTH_ERR_THREAD";
        break;
    case IPTH_ERR_SERVER_NACK:
        return "IPTH_ERR_SERVER_NACK";
        break;
    case IPTH_ERR_UNSUPPORTED:
        return "IPTH_ERR_UNSUPPORTED";
        break;
    case IPTH_ERR_COMMUNICATION:
        return "IPTH_ERR_COMMUNICATION";
        break;
    case IPTH_ERR_CANCEL:
        return "IPTH_ERR_CANCEL";
        break;
    case IPTH_ERR_DMACC:
        return "IPTH_ERR_DMACC Check (InnoPathDMConf.ddf/InnoPath/Custom/Configuration/IMDMC/DMSettings/PreferredServer) node value has matched with your server.";
        break;
    case IPTH_ERR_AUTH:
        return "IPTH_ERR_AUTH";
        break;
    case IPTH_ERR_NOT_IMPLEMENTED:
        return "IPTH_ERR_NOT_IMPLEMENTED";
        break;
    case IPTH_ERR_IO_TIMEOUT:
        return "IPTH_ERR_IO_TIMEOUT";
        break;
    case IPTH_ERR_IO_PENDING:
        return "IPTH_ERR_IO_PENDING";
        break;
    case IPTH_ERR_UNREGISTERED_DEVICE:
        return "IPTH_ERR_UNREGISTERED_DEVICE";
        break;
    case IPTH_ERR_IO_CONNECTION_REFUSED:
        return "IPTH_ERR_IO_CONNECTION_REFUSED";
        break;
    default:
        return ".";
    }
}


/* Initialize a queue handle */

void* idev_queueInit()
{
    ip_queuePtr_s const queue = (ip_queuePtr_s)malloc(sizeof(ip_queue_s));
    char const *errmsg;
    int rv;

    if (queue == IP_NULL) {
        return IP_NULL;
    }
    queue->root = IP_NULL;
    queue->count = 0;

    if (0!=(rv = pthread_mutex_init(&queue->mutex, NULL))) {
        errmsg = "pthread_mutex_init";
        goto error;
    }
    if (0!=(rv = pthread_cond_init(&queue->cond, NULL))) {
        errmsg = "pthread_cond_init";
        goto error;
    }
    return (void*)queue;

error:
    free(queue);
    return NULL;
}


/* Add an item to a queue */
ip_int16 idev_postMessage(	void* const queueID,	int const msg,	void const *param )
{
    ip_queue_s *const queue = (ip_queue_s *)queueID;
    char const *errmsg;
    int rv;

#ifdef DEBUG_QUEUE  /*{*/
              str_queueID(queueID), str_msg(queueID, msg), (void *)param);
#endif  /*}*/

    if (queueID == IP_NULL) {
        return IP_ERR;
    }
    ip_queueMsg_s *const le = (ip_queueMsg_s *)malloc(sizeof(ip_queueMsg_s));
    if (le == IP_NULL) {
        return IP_ERR;
    }
    le->msg = msg;
    le->param = (int)param;
    le->next = NULL;

    if (0!=(rv = pthread_mutex_lock(&queue->mutex))) {
        errmsg = "PostMessage lock";
        goto error;
    }
    if (queue->root == IP_NULL) {/* empty */
        queue->root = le; /* add first item to the queue */
    } else {
        ip_queueMsg_s *ptr = queue->root;
        /* Find the last item */
        while (ptr->next != IP_NULL) {
            ptr = ptr->next;
        }
        /* Add item to the end of the queue */
        ptr->next = le;
    }
    if (queue->count<=0) { /* UI waiters (<0) or possible waiters (==0) */
        if (queue->count==0) {
            queue->count += 1;
        }
        /* In the case of UIDatQueue there maybe more than one threads blocked on input,
           so we have to awake them all. That is because pthread_cond_signal would awake
           them based on OS scheduling principals and there is no guarantee that the thread
           that needs the input will be awaken. So we use pthread_cond_broadcase() instead */
        /*if (0!=(rv = pthread_cond_signal(&queue->cond))) {*/
        if (0!=(rv = pthread_cond_broadcast(&queue->cond))) {
            errmsg = "PostMessage signal";
            goto error;
        }
    } else {
        queue->count += 1;
    }
    if (0!=(rv = pthread_mutex_unlock(&queue->mutex))) {
        errmsg = "PostMessage unlock";
        goto error;
    }
    return IP_OK;

error:
    return IP_NULL;
}

/* Get a queue item out of a queue */
ip_int16 idev_getMessage(
    void* const queueID,
    int *const msg,
    void **const param
)
{
    ip_queue_s	*const queue = (ip_queuePtr_s)queueID;
    char const *errmsg;
    int rv;

#ifdef DEBUG_QUEUE
#endif
    if (queue == IP_NULL) {
        *msg = IP_EVT_DME_NULL;
        *param = 0;
        return IP_ERR;
    }
    if (0!=(rv = pthread_mutex_lock(&queue->mutex))) {
        errmsg = "GetMessage lock";
        goto error;
    }
    for (;;) {
        ip_queueMsg_s *const ptr = queue->root;
        if (ptr) {
            *msg = ptr->msg;
            *param = (void *)ptr->param;
            queue->root = ptr->next;
            if (0>=queue->count--) {
                rv = 2;  /* ENOENT: no such file or directory */
                errmsg = "GetMessage count";
                goto error;
            }
            free(ptr);
            if (0!=(rv = pthread_mutex_unlock(&queue->mutex))) {
                errmsg = "GetMessage unlock";
                goto error;
            }
#ifdef DEBUG_QUEUE
#endif
            return IP_OK;
        } else {
            if (0!=queue->count) {
                rv = 2;  /* ENOENT: no such file or directory */
                errmsg = "GetMessage count";
                goto error;
            }
#ifdef DEBUG_QUEUE
#endif
            if (0!=(rv = pthread_cond_wait(&queue->cond, &queue->mutex))) {
                errmsg = "GetMessage wait";
                goto error;
            }
        }
    }

error:
    return IP_ERR;
}
/*
* Basically same with idev_getMessage(),
* but does not block.
*/
ip_int16 idev_readMessage(
    void* const queueID,
    int *const msg,
    void **const param
)
{
    ip_queue_s	*const queue = (ip_queuePtr_s)queueID;
    char const *errmsg;
    int rv;

#ifdef DEBUG_QUEUE
#endif
    if (queue == IP_NULL) {
        *msg = IP_EVT_DME_NULL;
        *param = 0;
        return IP_ERR;
    }
    if (0!=(rv = pthread_mutex_lock(&queue->mutex))) {
        errmsg = "ReadMessage lock";
        goto error;
    }
    //for (;;) {
        ip_queueMsg_s *const ptr = queue->root;
        if (ptr) {
            *msg = ptr->msg;
            *param = (void *)ptr->param;
            queue->root = ptr->next;
            if (0>=queue->count--) {
                rv = 2;  /* ENOENT: no such file or directory */
                errmsg = "GetMessage count";
                goto error;
            }
            free(ptr);
#ifdef DEBUG_QUEUE
                      str_queueID(queueID), str_msg(queueID, *msg), (void *)*param);
#endif
        } else {
            if (0!=queue->count) {
                rv = 2;  /* ENOENT: no such file or directory */
                errmsg = "ReadMessage count";
                goto error;
            }
#ifdef DEBUG_QUEUE
#endif
          *msg = IP_EVT_DME_NULL;
        }
    //}
	    if (0!=(rv = pthread_mutex_unlock(&queue->mutex))) {
	        errmsg = "ReadMessage unlock";
	        goto error;
	    }
		return IP_OK;
error:
    return IP_ERR;
}

void idev_queueDestroy(void* const queueID)
{
    ip_queue_s	*const queue = (ip_queue_s *)queueID;
    ip_queueMsg_s *ptr;
    char const *errmsg = NULL;
    int rv;

    if (NULL==queue) {
        return;
    }
    if (0!=(rv = pthread_mutex_lock(&queue->mutex))) {
        errmsg = "QueueDestroy lock";
        goto error;
    }
    for (ptr=queue->root; ptr != IP_NULL; ptr=queue->root) {
        queue->root = ptr->next;
        free(ptr);
    }
    if (0!=(rv = pthread_mutex_unlock(&queue->mutex))) {
        errmsg = "QueueDestroy unlock";
        goto error;
    }
    if (0!=(rv = pthread_mutex_destroy(&queue->mutex))) {
#ifdef _DEBUG
        errmsg = "QueueDestroy destroy";
#endif
        goto error;
    }
    free(queue);
    return;

error:
    return;
}


void* IPTH_DME_Task_Init()
{
    idev_TaskDesc* pTaskDesc;

    pTaskDesc = GetTaskDesc();


    pTaskDesc->DMMsgQueueID = 0;
    pTaskDesc->UIMsgQueueID = 0;

    if ((pTaskDesc->DMMsgQueueID = idev_queueInit()) == IP_NULL) {
        return IP_NULL;
    } else if ((pTaskDesc->UIMsgQueueID = idev_queueInit()) == IP_NULL) {
        return IP_NULL;
    }


    return pTaskDesc->DMMsgQueueID;
}


/****************************************************************************/
/****************************************************************************/
/*******************        DME Thread and Event        *********************/
/****************************************************************************/
/****************************************************************************/
void IPTH_Send_DME_Event(IPTH_DMEHandleEvent Event)
{
    idev_TaskDesc* pTaskDesc;
    void* DMMsgQueueID;


    pTaskDesc = (idev_TaskDesc *)GetTaskDesc();
    DMMsgQueueID = pTaskDesc->DMMsgQueueID;
    idev_postMessage(DMMsgQueueID, Event, pTaskDesc);
}

IPTH_DMEHandleEvent IPTH_Get_DME_Event()
{
    IPTH_DMEHandleEvent Event;
    idev_TaskDesc* pTaskDesc;
    void* DMMsgQueueID;

    pTaskDesc = (idev_TaskDesc *)GetTaskDesc();
    DMMsgQueueID = pTaskDesc->DMMsgQueueID;
    idev_getMessage(DMMsgQueueID, (int *)&Event, (void *)&pTaskDesc);

    return Event;
}

IPTH_DMEHandleEvent IPTH_Read_DME_Event()
{
    IPTH_DMEHandleEvent Event;
    idev_TaskDesc *pTaskDesc;
    void *DMMsgQueueID;

    pTaskDesc = (idev_TaskDesc *)GetTaskDesc();
    DMMsgQueueID = pTaskDesc->DMMsgQueueID;
    idev_readMessage(DMMsgQueueID, (int *)&Event, (void *)&pTaskDesc);

    return Event;
}
/****************************************************************************/
/****************************************************************************/
/*******************     DME Application Task Process   *********************/
/****************************************************************************/
/****************************************************************************/

/*
* IO Pending has proviced from socket layer during download for every specific size bytes downloaded.
* Check DL_PKG_CHUNK_SIZE_CHECK_UI at ipth_hal_com.c
*
*/
int IPTH_DME_Run_IOPending()
{
    IPTH_DMEHandleEvent msg;

	return 0;
}

void *IPTH_DME_Task_Process()
{
    int ret = IPTH_OK;
    int sessNum=0;
	int s;
    IPTH_DMEHandleEvent msg;

    //pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL);

    for (;;) {
        msg = IPTH_Get_DME_Event();

        switch (msg) {
        case IP_EVT_INIT_APP:
            ret = idev_app_init();
            if (ret!=IPTH_OK) {
                IPTH_Send_DME_Event(IP_EVT_EXIT_DME);
            } else {
                IPTH_Send_DME_Event(IP_EVT_RUN_DME);
            }
            break;
        case IP_EVT_RECOVER_DME:
            if ( idev_checkDmeRecovery() ) {
//				IPTH_Send_DME_Event(IP_EVT_RUN_DME);
                IPTH_Send_DME_Event(IP_EVT_INIT_APP);
            } else {
                IPTH_Send_DME_Event(IP_EVT_EXIT_DME);
            }
            break;
        case IP_EVT_START_NI_DM_SESS:
            ret = idev_startNiDMSess();
            if (ret==IPTH_OK) {
                quec_odm_interaction_dme_post("Start NI");
//				IPTH_Send_DME_Event(IP_EVT_RUN_DME);
                IPTH_Send_DME_Event(IP_EVT_INIT_APP);
            } else {
            }
            break;
        case IP_EVT_START_UI_FOTA:
            if ( idev_checkDmeRecovery() ) {
//				IPTH_Send_DME_Event(IP_EVT_RUN_DME);
                IPTH_Send_DME_Event(IP_EVT_INIT_APP);
            } else {
                ret = idev_startFotaUpdate(IP_TASKINIT_USER);
                if (ret==IPTH_OK) {
//					IPTH_Send_DME_Event(IP_EVT_RUN_DME);
                    IPTH_Send_DME_Event(IP_EVT_INIT_APP);
                } else {
                }
            }
            break;
        case IP_EVT_START_SEND_FOTA_RESULT: {
            #if 0
            gTaskDesc.TaskInitiator = IP_TASKINIT_USER;
            ret = idev_sendFotaResult("255");   // SU Cancel Error Code
            idev_log("idev_sendFotaResult(255)\r\n");
            if (ret==IPTH_OK) {
                idev_log("idev_sendFotaResult() returned IPTH_OK\r\n");
                IPTH_Send_DME_Event(IP_EVT_INIT_APP);
            } else {
                idev_log("idev_sendFotaResult() returned IPTH_ERR\r\n");
                IPTH_Send_DME_Event(IP_EVT_EXIT_DME);
            }
            #else
            //idev_sendFotaResult("255", gfumoCorrelator);
            IPTH_Send_DME_Event(IP_EVT_INIT_APP);
            #endif
        }
        break;
        case IP_EVT_START_CI_FOTA:
#if 0
            TaskDesc.TaskInitiator = IP_TASKINIT_CLIENT;
            ret=idev_startFotaUpdate(IP_TASKINIT_CLIENT);
            if (ret==IPTH_OK) {
                idev_log("idev_startFotaUpdate(IP_TASKINIT_CLIENT) returned IPTH_OK\r\n");
                IPTH_Send_DME_Event(IP_EVT_RUN_DME);
                IPTH_Send_UI_Event(IP_EVT_UI_INITDM);
            } else {
                idev_log("idev_startFotaUpdate(IP_TASKINIT_CLIENT) returned IPTH_ERR\r\n");
            }
#endif
            break;
        case IP_EVT_TERM_DMS:
#if 0
            IPTH_Send_DME_Event(IP_EVT_RUN_DME);
#endif
            break;
        case IP_EVT_REPORT_DM_CANCEL:
#if 0
            ret=idev_startFotaUpdate(IP_TASKINIT_DMCANCEL);
            if (ret==IPTH_OK) {
                idev_log("idev_startFotaUpdate(IP_TASKINIT_DMCANCEL) returned IPTH_OK\r\n");
                IPTH_Send_DME_Event(IP_EVT_RUN_DME);
            } else {
                idev_log("idev_startFotaUpdate(IP_TASKINIT_DMCANCEL) returned IPTH_ERR\r\n");
            }
#endif
            break;
        case IP_EVT_RUN_DME:
            break;
        case IP_EVT_DLDD_HANDLE:
            idev_uiAlertProcessUserInput(g_pDmApp, IP_EVT_DLDD_HANDLE, g_respUI);
            break;
        case IP_EVT_UTS_HANDLE:
	        g_pDmApp->pcb = g_pDmApp->pcb_ut; 
            idev_uiAlertProcessUserInput(g_pDmApp, IP_EVT_UTS_HANDLE, g_respUI);
            break;
        case IP_EVT_NETERR_HANDLE:
            idev_uiAlertProcessUserInput(g_pDmApp, IP_EVT_NETERR_HANDLE, g_respUI);
            break;
        case IP_EVT_DMALERT_HANDLE:
            idev_uiAlertProcessUserInput(g_pDmApp, IP_EVT_DMALERT_HANDLE, g_respUI);
            break;
       case IP_EVT_PROGRESS_HANDLE:
            idev_uiProcessBarUserInput(g_pDmApp, IP_EVT_PROGRESS_HANDLE, g_respUI);
            break;

        case IP_EVT_PROGRESS_RESUME:
            idev_uiProcessBarUserInput(g_pDmApp, IP_EVT_PROGRESS_HANDLE, IPTH_PROGBAR_INPUT_EVENT_RESUME);
            break;
        case IP_EVT_REBOOT_DME:
            break;

        case IP_EVT_EXIT_DME:
            
            IPTH_Send_UI_Event(IP_EVT_UI_EXIT);

            if (g_pDmApp->hDme) {
            }
            if ( g_pDmApp) free( g_pDmApp);
            pthread_exit(0);
            break;
        default: {
        }
        break;
        }
        sleep(1);
    }
}
