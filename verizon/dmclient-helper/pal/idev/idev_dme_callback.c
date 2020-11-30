#include <stdio.h>
#include <stdlib.h>

#include <dm_engine/ipth_dme_types.h>
#include "idev_shared_api.h"
#include "idev_dmapp.h"
#include "idev_thread_dme.h"
#include <fumo/ipth_fumo.h>

void idev_dmeStatusCallback(ipth_StatusEvent_s evt, void *pData)
{
    DMApplication_s *pApp;
    ipth_DMSessionEvent_s *dm_evt;
    ipth_DlaStatusEvent_s *dl_evt;
    ipth_GenericAlertStatus *alt;
    int sessNum;


    switch ( evt.code) {
    case IPTH_EVENT_WORKFLOW_STARTED: /* new workflow started */
        break;

    case IPTH_EVENT_WORKFLOW_COMPLETED: /* workflow completed, about to be discarded */
        break;
    case IPTH_EVENT_IO_COMPLETED: /* async IO completed */
        pApp = (DMApplication_s *)pData;
        IPTH_Send_DME_Event(IP_EVT_RUN_DME);
        break;

    case IPTH_EVENT_IO_CONNECTION_REFUSED:
        break;

    case IPTH_EVENT_DMSESSION_STARTED:  /* DM session started */
        dm_evt = (ipth_DMSessionEvent_s *)evt.param;
        break;

    case IPTH_EVENT_DMSESSION_COMPLETED: /* DM session completed */
        dm_evt = (ipth_DMSessionEvent_s *)evt.param;
        break;

    case IPTH_EVENT_DMSESSION_ALERTSTATUS:
        dm_evt = (ipth_DMSessionEvent_s *)evt.param;
        alt = (ipth_GenericAlertStatus *) dm_evt->p.alertStatus;
        break;

    case IPTH_EVENT_DMSESSION_EXEC: /* DM session received Exec command */
        dm_evt = (ipth_DMSessionEvent_s *)evt.param;
        break;

    case IPTH_EVENT_DMSESSION_GET: /* DM session received Get command */
        dm_evt = (ipth_DMSessionEvent_s *)evt.param;
        if (strcmp(dm_evt->p.uri, "./DevDetail/FwV")==0) {
        }
        break;

    case IPTH_EVENT_DOWNLOAD_STARTED: /* File download started */
        dl_evt = (ipth_DlaStatusEvent_s *)evt.param;
    #if 1
        quec_odm_interaction_fumo_post("Download start");
    #endif
        break;

    case IPTH_EVENT_DOWNLOAD_COMPLETED: /* Download completed */
        dl_evt = (ipth_DlaStatusEvent_s *)evt.param;
    
        quec_odm_interaction_fumo_post("Download completed");

        break;

    case IPTH_EVENT_DOWNLOAD_ERROR:
        dl_evt = (ipth_DlaStatusEvent_s *)evt.param;
        break;

    case IPTH_EVENT_FUMO_STARTED: /* FUMO task started */
        pApp = (DMApplication_s *)pData;
    
        quec_odm_interaction_fumo_post("Start");

        pApp->cancelCb = (ipth_DmeCompletionCallback_s *)evt.param;
        break;

    case IPTH_EVENT_FUMO_COMPLETED: /* FUMO task completed */
        pApp = (DMApplication_s *)pData;

        quec_odm_interaction_fumo_post("End");

        pApp->cancelCb = NULL;
        break;
    }
}
