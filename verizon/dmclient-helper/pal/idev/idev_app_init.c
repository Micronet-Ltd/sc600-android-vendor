#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "idev_shared_api.h"
#include "idev_thread_dme.h"

// Qualcomm
#if 0
#include "comdef.h"
#include "customer.h"
#include "ds_util.h"
#include "ps_iface_defs.h"
#include "ds_Utils_DebugMsg.h"
#include "dsnet_lib.h"
#include "dssock_lib.h"
#include "dss_init.h"
#include "dssocket_defs.h"
#include "dss_netpolicy.h"
#include "idev_dss_fota.h"

#define QCMAP_DEFAULT_DSS_INIT_TIME    3

extern dss_fota_prog_args_t Prog_args[];
extern int Prog_args_size;

extern pthread_mutex_t Net_mutx;
extern pthread_cond_t  Net_cond;
extern volatile sint15 Net_mutx_dss_nh;
extern volatile int    Net_ev_pending;

extern pthread_mutex_t Sock_mutx;
extern pthread_cond_t  Sock_cond;
extern volatile sint15 Sock_mutx_dss_nh;
extern volatile int    Sock_ev_pending;

extern int dss_fota_dsnet_get_handle (dss_net_policy_info_type * net_policy, sint15 * dss_nh);

/**************************************************************
*
* idev_network_init() is only for internal test purpose.
* it only works on M9615ACETWMLZD1016.
* After M9615ACETWMLZD1016 version, SoftAp will bring up data call.
*
**************************************************************/

int idev_network_init()
{
    sint15 nh;
    sint15 dss_errno;
    dss_net_policy_info_type net_policy;
    int status;
    int dss_init_time = QCMAP_DEFAULT_DSS_INIT_TIME;
    const dss_fota_prog_args_t * arg_ptr;

    //
    //  BRING UP DSS
    //

    /* Powerup Init */
    dsnet_init();
    dss_powerup();
    dss_init();

    idev_log("Initializing.");
    while (dss_init_time--) {
        sleep(1);
        idev_log(".");
    }
    idev_log("\n");

    // const dss_fota_prog_args_t * arg_ptr;
    dss_init_net_policy_info(&net_policy);
    idev_log("dss_init_net_policy_info \r\n");

    /* First set test specific defaults */
    net_policy.iface.kind = DSS_IFACE_NAME;
    net_policy.iface.info.name = DSS_IFACE_ANY;
    net_policy.family = DSS_AF_INET;
    net_policy.umts.pdp_profile_num = DSS_FOTA_DEFAULT_PDP_PROF;

    idev_log(" net_policy.iface.kind = %d\r\n",net_policy.iface.kind);
    idev_log(" net_policy.iface.info.name= %d\r\n",net_policy.iface.info.name);
    idev_log(" net_policy.policy_flag = %d\r\n",net_policy.policy_flag);
    idev_log(" net_policy.umts.pdp_profile_num = %d\r\n",net_policy.umts.pdp_profile_num);
    idev_log(" net_policy.cdma.data_session_profile_id = %d\r\n",net_policy.cdma.data_session_profile_id);
    idev_log(" net_policy.dss_netpolicy_private.cookie = %x\r\n",net_policy.dss_netpolicy_private.cookie);

//  net_policy.iface.info.name = DSS_IFACE_ANY;
//  net_policy.iface.info.name = DSS_IFACE_UMTS;
//  net_policy.iface.info.name = DSS_IFACE_CDMA_SN;
    dss_fota_dsnet_get_handle (&net_policy, &nh);

    idev_log("dsnet_get_handle %d through DSS_IFACE_ANY. \r\n", nh);

    if (nh == DSS_ERROR) {
        idev_log("dsnet_get_handle error %d \r\n", nh);
        return -1;
    }
    //sleep(1);
    if ((status = dss_fota_dsnet_start_sync(nh, DSS_SUCCESS, 0)) < 0) {
        idev_log("dss_fota_dsnet_start_sync error %d \r\n", status);
        return -1;
    }
    idev_log("Bring up data success(%d).\r\n", status);
    return nh;
}

int idev_network_finalize(sint15 nh)
{
    int status;
    //
    // TEAR DOWN DSS
    //
    if ((status = dss_fota_dsnet_stop_sync(nh, DSS_SUCCESS, 0)) < 0) {
        idev_log("Warning : dss_fota_dsnet_stop_sync error %d\r\n", status);
    }
    if ((status = dss_fota_dsnet_release_handle(nh)) < 0) {
        idev_log("Warning : dss_fota_dsnet_release_handle error %d\r\n", status);
    }

    dss_deinit();
    dss_powerdown();
    dsnet_release();

    idev_log("Tear down data completed.(%d).\r\n", status);

}
#endif

int idev_app_init(void)
{
    int ret;
//  sint15 nh;

    return IPTH_OK;
    /*********************************************
     internal test code.
      nh = idev_network_init();
      if (nh==-1)
    	{
    		gTaskDesc.dss_nh = 0;
    		return IPTH_ERR;
      }
    	else
    	{
    		gTaskDesc.dss_nh = nh;
    		return IPTH_OK;
    	}
    */
}

void idev_app_finalize()
{

    /*********************************************
      internal test code.
      idev_network_finalize(gTaskDesc.dss_nh);
    */
}
