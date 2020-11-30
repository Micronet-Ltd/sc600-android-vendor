/******************************************************************************
 * dmapp.h - Declaration file for UI alert.
 *
 * Copyright (c) 9 InnoPath Software, Inc. All rights reserved.
 *
 * Legal Statements
 *
 * THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS
 * IS" BASIS WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE,
 * TRADE OR OTHERWISE INCLUDING ANY IMPLIED WARRANTIES OR CONDITIONS OF
 * MERCHANTABILITY, MERCHANTABLE QUALITY, FITNESS FOR ANY PARTICULAR PURPOSE
 * OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES NOT REPRESENT OR WARRANT
 * THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS, UNINTERRUPTED,
 * ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR AVAILABLE AT
 * ANY TIME.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE
 * FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE,
 * PUNITIVE OR OTHER DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED
 * TO ANY LOSS OF PROFITS OR REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST
 * BUSINESS, LOST BUSINESS OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM
 * WORK STOPPAGE, COSTS OF OVERHEAD, COSTS OF COVER, LOSS OF ANTICIPATED
 * BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO THIS AGREEMENT, HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF THE
 * ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.
 *****************************************************************************/

#ifndef __IDEV_DMAPP_H__
#define __IDEV_DMAPP_H__

#include <sys/socket.h>

#include <ipth_config.h>
#include <common/ipth_authutils.h>
#include <dm_engine/ipth_dme.h>
#include <dm_engine/ipth_dla.h>

#include "idev_thread_dme.h"


typedef struct DMApplication {
    ipth_DmeHandle hDme;
    int listenSocket;
    size_t borNMsgLen;
    unsigned char * borNMsg;
    ipth_UIAlertData *uiCmdData;
    ipth_DmeCompletionCallback_s *pcb;
	ipth_DmeCompletionCallback_s *pcb_ut; // For multi session of Update Pending
    ipth_StatusCallback_s statusCb;
    void *progBar;
    ipth_DmeCompletionCallback_s *cancelCb;
} DMApplication_s;

#ifdef __cplusplus
extern "C" {
#endif

    extern DMApplication_s *g_pDmApp;

    extern int idev_uiAlertProcessUserInput(DMApplication_s *pApp, IPTH_DMEHandleEvent EvtType, int retCode);
    extern void idev_uiProcessBarUserInput(DMApplication_s *pApp, IPTH_DMEHandleEvent EvtType, int retCode);

#ifdef __cplusplus
}
#endif

#endif /* __IDEV_DMAPP_H__ */
