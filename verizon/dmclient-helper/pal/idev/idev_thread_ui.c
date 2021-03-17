#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <dm_engine/ipth_dme.h>

#include "idev_shared_api.h"
#include "idev_thread_dme.h"
#include "idev_dmapp.h"

int ghUIEvent;

extern ip_int16 idev_postMessage(	void* const queueID,	int const msg,	void const *param );
extern char const *const gUIHandleEvent_msg[];
extern idev_TaskDesc* GetTaskDesc();
extern ip_int16 idev_getMessage(
            void* const queueID,
                int *const msg,
                    void **const param
        );


/****************************************************************************/
/****************************************************************************/
/******************        DME UI Thread and Event         ******************/
/****************************************************************************/
/****************************************************************************/

void IPTH_Send_UI_Event(IPTH_UIHandleEvent Event)
{
    idev_TaskDesc* pTaskDesc;
    void* UIMsgQueueID;


    pTaskDesc = (idev_TaskDesc *)GetTaskDesc();
    UIMsgQueueID = pTaskDesc->UIMsgQueueID;
    idev_postMessage(UIMsgQueueID, Event, pTaskDesc);
}

IPTH_UIHandleEvent IPTH_Get_UI_Event()
{
    IPTH_UIHandleEvent Event;
    void *param;
    idev_TaskDesc* pTaskDesc;
    void* UIMsgQueueID;

    pTaskDesc = (idev_TaskDesc *)GetTaskDesc();
    UIMsgQueueID = pTaskDesc->UIMsgQueueID;
    idev_getMessage(UIMsgQueueID, (int *)&Event, (void *)&pTaskDesc);

    return Event;
}

/****************************************************************************/
/****************************************************************************/
/*******************          DME UI Task Process       *********************/
/****************************************************************************/
/****************************************************************************/

void *IPTH_UI_Task_Process()
{
    IPTH_UIHandleEvent msg;

    //pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL);

    for (;;) {
        msg = IPTH_Get_UI_Event();

        switch (msg) {
        case IP_EVT_UI_EXIT:
            pthread_exit(0);
            break;
        case IP_EVT_UI_INITDM:
            break;
        case IP_EVT_UI_DLDD:  // Before Download
            g_respUI = IPTH_UI_RETURN_OK;
            //g_respUI = IPTH_UI_RETURN_SU_CANCEL_552;  // <-- Input IPTH_UI_RETURN_SU_CANCEL_552 for SU Cancel case.

            /*** FOTA Download Pending Test
            */
            //sleep(60);

            IPTH_Send_DME_Event(IP_EVT_DLDD_HANDLE);
            break;
        case IP_EVT_UI_PRORGESS:
            // Display a Initial Download Progress Bar Screen
            break;
        case IP_EVT_UI_UTS:
            g_respUI = IPTH_UI_RETURN_OK;
            //g_respUI = IPTH_UI_RETURN_SU_CANCEL_552; // <-- Input IPTH_UI_RETURN_SU_CANCEL_554 for SU Cancel case.

            /*** FOTA Update Pending Test
            sleep(60);
            */

            IPTH_Send_DME_Event(IP_EVT_UTS_HANDLE);
            break;
        case IP_EVT_UI_UCS:
            break;
        case IP_EVT_UI_NETERR:
            g_respUI = IPTH_UI_RETURN_NEGATIVE;
            IPTH_Send_DME_Event(IP_EVT_NETERR_HANDLE);
            break;
        case IP_EVT_UI_DMALERT:
            g_respUI = IPTH_UI_RETURN_OK;
            IPTH_Send_DME_Event(IP_EVT_DMALERT_HANDLE);
            break;
        default:
            break;
        }
        sleep(1);
    }
}

