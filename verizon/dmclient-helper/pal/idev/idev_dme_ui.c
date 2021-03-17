#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "idev_shared_api.h"
#include "idev_dmapp.h"
#include "idev_thread_dme.h"

#define SetProgressBar(app,pb) ((DMApplication_s *)(app))->progBar=pb

typedef struct {
    /* User callback function when cancel button is called */
    ipth_DmeCompletionCallback_s *cbCancel;
    /* Application private data */
    void *appData;
    /* Current progress */
    int nPercent;
} ProgressBar;

int g_respUI=IPTH_UI_RETURN_FAILED;
char g_uiInput[100];


void quec_odm_interaction_init(void)
{
    if (access(IPTH_DME_CMD_PIPE_NAME, F_OK) == -1)  {
        mkfifo(IPTH_DME_CMD_PIPE_NAME, 0777);
    }
}


bool quec_odm_interaction_post(ql_odm_urc_type urc_type, char *msg)
{
    int pipefd;
    int msg_len;
    char uimsg[1024] = {0};
   
    if (access(IPTH_DME_URC_PIPE_NAME, F_OK) == -1) {
        return false;
    }
    pipefd = open(IPTH_DME_URC_PIPE_NAME, O_WRONLY);

    snprintf(uimsg, sizeof(uimsg), "%d %s", urc_type, msg);
    msg_len = write(pipefd, uimsg, strlen(uimsg));
    close(pipefd);
    if (msg_len < 0) {
        return false;
    }  

    return true;
}


int quec_odm_interaction_get(char *uimsg, int size)
{
    int read_len, data_len = 0;
    int pipe_fd;
    char userinput[128];

    if (uimsg == NULL) {
        return -1;
    }
  
    pipe_fd = open(IPTH_DME_CMD_PIPE_NAME, O_RDONLY);
    read_len = read(pipe_fd, userinput, sizeof(userinput) - 1);
    close(pipe_fd);

    if (read_len <= 0) {
        return -1;
    }

    snprintf(uimsg, size -1, "%s", userinput);
    return read_len;
}


int quec_odm_interaction_dme_post(char *msg)
{
    return quec_odm_interaction_post(QL_ODM_URC_DME, msg);
}


int quec_odm_interaction_fumo_post(char *msg)
{
    return quec_odm_interaction_post(QL_ODM_URC_FUMO, msg);
}


int quec_odm_interaction_dmacc_get(struct quec_ip_dmaccParas *pdmacc)
{
    int urclen;
    char urcmsg[512] = {0};

    snprintf(urcmsg, sizeof(urcmsg),
             "<serId>%s</servId>,\r\n<serURI>%s:%s</serURI>,\r\n<authType>%s</authType>,\r\n"
             "<sauthName>%s</sauthName>,\r\n<sauthSec>%s</sauthSec>,\r\n"
             "<cauthName>%s</cauthName>,\r\n<cauthSec>%s</cauthSec>",
             pdmacc->servId, pdmacc->addr, pdmacc->port, pdmacc->authType,
             pdmacc->sauthname, pdmacc->sauthsec, pdmacc->cauthname, pdmacc->cauthsec);

    quec_odm_interaction_post(QL_ODM_URC_DMACC, urcmsg);
    return 0;
}


int quec_odm_interaction_hostdevice_get(struct quec_ip_hostDevice* hostDev)
{
    int urclen;
    char urcmsg[512] = {0};

    snprintf(urcmsg, sizeof(urcmsg),
             "Manu: %s\r\nModel: %s\r\nSwV: %s\r\nFwV: %s\r\nHwV: %s\r\n"
             "DateStamp: %s\r\nDeviceID: %s",
             hostDev->manu, hostDev->model, hostDev->swv, hostDev->fwv,
             hostDev->hwv, hostDev->datestamp, hostDev->deviceid);

    quec_odm_interaction_post(QL_ODM_URC_HOSTDEVICE, urcmsg);
    return 0;
}


int quec_odm_interaction_ui_post(ipth_UIAlertType alter, char *msg)
{
    int urclen, msglen, i;
    char urcmsg[1024] = {0};

    msglen = strlen(msg);
    
    switch (alter) {
    case IPTH_UIALERT_DISPLAY:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<DSP>%s</DSP>", msg);
        break;

    case IPTH_UIALERT_CONFIRM:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<CON>%s</CON>", msg);
        break;

    case IPTH_UIALERT_TEXTINPUT:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<TEXI>%s</TEXI>", msg);
        break;

    case IPTH_UIALERT_SINGLECHOICE:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<SIGC>%s</SIGC>", msg);
        break;

    case IPTH_UIALERT_MULTICHOICE:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<MULC>%s</MULC>", msg);
        break;

    case IPTH_UIMESSAGE_NETERR:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<NETE>%s</NETE>", msg);
        break;

    default:
        urclen = snprintf(urcmsg, sizeof(urcmsg), "<USER>%s</USER>", msg);
        break;
    }

    quec_odm_interaction_post(QL_ODM_URC_UI, urcmsg);

    return urclen;
}


int quec_odm_interaction_ui_get(ipth_UIAlertType alter, char *msg)
{
    int urclen;

    return 0;
}


int idev_uiAlertTypeHandler(ipth_UIAlertData*uiCmdData, void *appData, ipth_DmeCompletionCallback_s *pcb)
{
    int i, len;
    char uimsg[256];
    DMApplication_s *pApp = (DMApplication_s *)appData;

    if ( pApp == NULL || uiCmdData == NULL) {
        return IPTH_UI_RETURN_FAILED;
    }

    pApp->pcb = pcb;
    pApp->uiCmdData = uiCmdData;

    if ( uiCmdData->alertType == IPTH_UIMESSAGE_NETERR) {
    } else {
    }

    switch (uiCmdData->alertType) {
    case IPTH_UIALERT_DISPLAY:
        snprintf(uimsg, sizeof(uimsg), "MinT:%d, MaxT:%d %s",
            uiCmdData->minDispTime, uiCmdData->maxDispTime, uiCmdData->prompt);
        quec_odm_interaction_ui_post(IPTH_UIALERT_DISPLAY, uimsg);
        return IPTH_UI_RETURN_OK;

    case IPTH_UIALERT_CONFIRM:

        if (uiCmdData->defRsp != NULL) { // Set up default response value
            if (uiCmdData->defRsp[0] == '0') {
                g_respUI=IPTH_UI_RETURN_CANCEL;
            } else if (uiCmdData->defRsp[0] == '1') {
                g_respUI=IPTH_UI_RETURN_OK;
            }
        }

        snprintf(uimsg, sizeof(uimsg), "%s", uiCmdData->prompt);
        quec_odm_interaction_ui_post(IPTH_UIALERT_CONFIRM, uimsg);        
        break;

    case IPTH_UIALERT_TEXTINPUT:
        quec_odm_interaction_ui_post(IPTH_UIALERT_TEXTINPUT, "");
        break;

    case IPTH_UIALERT_SINGLECHOICE:

        i = 0;
        snprintf(uimsg, sizeof(uimsg), "MinT:%d, MaxT:%d, %s:",
            uiCmdData->minDispTime, uiCmdData->maxDispTime, uiCmdData->prompt);
        while (uiCmdData->items[i] != NULL) {
            //strncat(uimsg, uiCmdData->items[i], sizeof(uimsg) - strlen(uimsg));
            len = strlen(uimsg);
            snprintf(&uimsg[len], sizeof(uimsg) - len, " [%d-%s]", i + 1, uiCmdData->items[i]);
            i++;
        }
        
        quec_odm_interaction_ui_post(IPTH_UIALERT_SINGLECHOICE, uimsg);
        break;

    case IPTH_UIALERT_MULTICHOICE:

        i = 0;
        snprintf(uimsg, sizeof(uimsg), "MinT:%d, MaxT:%d, %s:",
            uiCmdData->minDispTime, uiCmdData->maxDispTime, uiCmdData->prompt);
        while (uiCmdData->items[i] != NULL) {
            //strncat(uimsg, uiCmdData->items[i], sizeof(uimsg) - strlen(uimsg));
            len = strlen(uimsg);
            snprintf(&uimsg[len], sizeof(uimsg) - len, " [%d-%s]", i + 1, uiCmdData->items[i]);
            i++;
        }

        quec_odm_interaction_ui_post(IPTH_UIALERT_MULTICHOICE, uimsg);
        break;

    case IPTH_UIMESSAGE_NETERR:
        //IPTH_Send_UI_Event(IP_EVT_UI_NETERR);
        quec_odm_interaction_ui_post(IPTH_UIMESSAGE_NETERR, "");
        return IPTH_UI_RETURN_OK;

    default:
        return IPTH_UI_RETURN_FAILED;
    }

    return IPTH_UI_RETURN_PENDING;
}

int idev_uiAlertProcessUserInput(DMApplication_s *pApp, IPTH_DMEHandleEvent EvtType, int retCode)
{
    ipth_UIAlertData *uiCmdData;
    int resplen;


    uiCmdData = pApp->uiCmdData;
    /* uiCmdData->alertType is set by library */
    if (uiCmdData == NULL) {
        return IP_ERR;
    }

    resplen = strlen(g_uiInput);
    if (resplen > 0) {
        pApp->uiCmdData->rsp = malloc(resplen + 1);
        strcpy(pApp->uiCmdData->rsp, g_uiInput);
        memset(g_uiInput, 0x00, sizeof(g_uiInput));
    }

    switch (uiCmdData->alertType) {
    case IPTH_UIALERT_DISPLAY:
        break;

    case IPTH_UIALERT_CONFIRM:
        break;

    case IPTH_UIMESSAGE_NETERR:
        break;

    case IPTH_UIALERT_TEXTINPUT:
        break;

    case IPTH_UIALERT_SINGLECHOICE:
        break;

    case IPTH_UIALERT_MULTICHOICE:
        break;
    default:
        return IP_ERR;
        break;
    }

    if ( pApp->pcb) {
        /* Notify completion */
        IPTH_DME_NOTIFY(pApp->pcb, retCode);
        //(*pApp->pcb->pFunc)( pApp->pcb->pData, retCode);
        /* done */
        pApp->pcb = NULL;
    } else {
        IPTH_Send_DME_Event(IP_EVT_EXIT_DME);
        return IP_ERR;
    }
    pApp->uiCmdData = NULL;
    return retCode;
}


void* idev_displayProgressBar(const char* name, void *appData, ipth_DmeCompletionCallback_s *pcb)
{
    ProgressBar *progress;


    progress = (ProgressBar *)malloc( sizeof(ProgressBar));
    if ( progress != NULL) {
        memset( progress, 0, sizeof(ProgressBar));

        progress->cbCancel = pcb;
        progress->appData = appData;
        if (pcb) {
            SetProgressBar( appData, progress);
        }
    }
    IPTH_Send_UI_Event(IP_EVT_UI_PRORGESS);
    return progress;
}


void idev_updateProgressBar(void* bar, int percent)
{
}

void idev_uiProcessBarUserInput(DMApplication_s *pApp, IPTH_DMEHandleEvent EvtType, int retCode)
{
    ProgressBar* progress = pApp->progBar;

    IPTH_DME_NOTIFY( progress->cbCancel, retCode);

    if ( retCode == IPTH_PROGBAR_INPUT_EVENT_PAUSE) {
    } else if ( retCode == IPTH_PROGBAR_INPUT_EVENT_RESUME) {
    } else if (retCode == IPTH_PROGBAR_INPUT_EVENT_CANCEL) {
        SetProgressBar( progress->appData, NULL);
        free(progress);
    }
    if ( g_pDmApp->pcb ) IPTH_DME_NOTIFY( pApp->pcb, IPTH_OK);
}

void idev_uiDownloadPause()
{
    g_respUI=IPTH_PROGBAR_INPUT_EVENT_PAUSE;
    IPTH_Send_DME_Event(IP_EVT_PROGRESS_HANDLE);
}

void idev_uiDownloadResume()
{
    g_respUI=IPTH_PROGBAR_INPUT_EVENT_RESUME;
    IPTH_Send_DME_Event(IP_EVT_PROGRESS_HANDLE);
}

void idev_uiDownloadCancel()
{
    g_respUI=IPTH_PROGBAR_INPUT_EVENT_CANCEL;
    IPTH_Send_DME_Event(IP_EVT_PROGRESS_HANDLE);
}
    
    
/***********************************************************************/
/*********           FOTA UI HANDLER FOR           *******************/
/***********************************************************************/

int idev_uiAlertFOTAHandler(ipth_UIAlertData *uiCmdData, void *appData, ipth_DmeCompletionCallback_s *pcb, ipth_Dla_DD *Dla_DD, ipth_FOTA_UIAlert screenType)
{
    DMApplication_s *pApp = (DMApplication_s *)appData;

    if ( pApp == NULL || uiCmdData == NULL) {
        return IPTH_UI_RETURN_FAILED;
    }
    pApp->pcb = pcb;
    pApp->uiCmdData = uiCmdData;

    if (uiCmdData->prompt) {
    } else {
    }

    if ((Dla_DD==NULL) && (screenType==IPTH_FOTA_UIALERT_DLDD)) {
    }

    switch (screenType) {
    case IPTH_FOTA_UIALERT_DLDD:
        IPTH_Send_UI_Event(IP_EVT_UI_DLDD);
        break;
    case IPTH_FOTA_UIALERT_UTS:
	    g_pDmApp->pcb_ut = pcb;
        IPTH_Send_UI_Event(IP_EVT_UI_UTS);
        break;
    default:
        return IPTH_UI_RETURN_FAILED;
    }

    return IPTH_UI_RETURN_PENDING;
}
