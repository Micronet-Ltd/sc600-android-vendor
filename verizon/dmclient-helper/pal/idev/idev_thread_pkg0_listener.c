#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>

#include "idev_shared_api.h"
#include "idev_thread_dme.h"
#include "idev_dmapp.h"

/***
 * This is for
 * Verizon Telematics OTADM - Requirement (2014-09-22).docx
 * 10.4.1 EXISTING FOTA SESSION VZ_REQ_TELEOTADM_36811
 *
 * IDEV_Pkg0_Listener_Process will receive the pkg0 message
 * sent by "ipth_dme sender".
 */

/****************************************************************************/
/****************************************************************************/
/**************     DME Pkg0 Wap Push Listner Thread      *******************/
/****************************************************************************/
/****************************************************************************/

void *IDEV_Pkg0_Listener_Process()
{
    int que_id = msgget(DME_PKG0_LISTENER_Q_UNIQUE_KEY, IPC_CREAT | 0600);

    idev_Pkg0_MsgType msg;
    ssize_t nbytes = 0;
    int msg_size = 0;
    unsigned char *pkg0_body;
    int byte_index;

    msg_size = sizeof(msg);


    //pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL);

    while (1) {
        nbytes = msgrcv(que_id, &msg, msg_size, 0, 0);

        if (nbytes == msg_size) {

            if ( (gTaskDesc.CurApp == IP_DMAPP_DL_PENDING) ||
                    (gTaskDesc.CurApp == IP_DMAPP_UT_PENDING) ||
                    (gTaskDesc.CurApp == IP_DMAPP_DLPROGRESS)) {
                pkg0_body = malloc(msg.mlen+1);
                memcpy(pkg0_body, msg.mdata, msg.mlen);
                g_pDmApp->borNMsg = pkg0_body;
                g_pDmApp->borNMsgLen = msg.mlen;
                /* log for debugging
                for (byte_index=0; byte_index<msg.mlen; byte_index++) {
                }*/

                IPTH_Send_DME_Event(IP_EVT_START_NI_DM_SESS);
            } else {
            }
        } else {
            // Ignore wrong message
            /* log for debugging
            for (byte_index=0; byte_index<msg.mlen; byte_index++) {
            	idev_log("msg.mdata [%d] : 0x%02X \r\n",byte_index, msg.mdata[byte_index] );
            }*/
        }
        sleep(10);
    }
}

int idev_is_pkg0_q_exist()
{
    int que_id = msgget(DME_PKG0_LISTENER_Q_UNIQUE_KEY, 0600);

    if (que_id!=-1) {
        printf("pkg0 q exist %d\r\n", que_id);
        return IP_OK;
    } else {
        printf("No pkg0 q\r\n");
        return IP_ERR;
    }
}

int idev_remove_pkg0_q()
{
    int que_id = msgget(DME_PKG0_LISTENER_Q_UNIQUE_KEY, 0600);

    if (que_id!=-1) {
        msgctl( que_id, IPC_RMID, 0);
        return IP_OK;
    } else {
        printf("No pkg0 q\r\n");
        return IP_ERR;
    }
}


// Only for ipth_dme sender
int idev_send_pkg0_q(idev_Pkg0_MsgType *msg)
{
    int msg_size = 0;
    int r;
    int que_id = msgget(DME_PKG0_LISTENER_Q_UNIQUE_KEY, 0600);

    if (que_id==-1) {
        return -1 ;
    }

    msg_size = sizeof(*msg);

    // log
    r = msgsnd(que_id, msg, msg_size, 0);

    if (r == -1) {
        return -2;
    }
    return IP_OK;
}


