#ifndef __IDEV_THREAD_EVT_H__
#define __IDEV_THREAD_EVT_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef	enum
    {
        IP_EVT_INIT_APP, //0
        IP_EVT_INIT_DME, //
        IP_EVT_RECOVER_DME, //
        IP_EVT_START_NI_DM_SESS, //
        IP_EVT_START_UI_FOTA, //4
        IP_EVT_START_CI_FOTA, //
        IP_EVT_RUN_DME, //6
        IP_EVT_TERM_DMS, //
        IP_EVT_REPORT_DM_CANCEL, //8
        IP_EVT_DLDD_HANDLE, //
        IP_EVT_PROGRESS_HANDLE, //
        IP_EVT_UTS_HANDLE, //
        IP_EVT_NETERR_HANDLE, //12
        IP_EVT_DMALERT_HANDLE, //
        IP_EVT_REBOOT_DME, //
        IP_EVT_EXIT_DME, //
        IP_EVT_START_SEND_FOTA_RESULT, //16
        IP_EVT_PROGRESS_RESUME,   //
        IP_EVT_DME_NULL //18
    } IPTH_DMEHandleEvent;

    typedef enum
    {
        IP_EVT_UI_INITDM, // Initial DM UI. From beginning of DM session to before 'Download Confirm Screen'
        IP_EVT_UI_DLDD, // Download description. To display 'Download Confirm Screen'
        IP_EVT_UI_UTS, // Update Timer Screen. To display 'Update Confirm Screen'
        IP_EVT_UI_PRORGESS, // Download Progress bar. To display download progress bar
        IP_EVT_UI_UCS, // Update Complete Screen. To display FOTA(download/update) result
        IP_EVT_UI_NETERR, // DM session Network error. To display Network Error message. Used in DM session only.
        IP_EVT_UI_DT_RESUME_DT, // In case of 'Download Confirm Screen' interruption. To return back to 'Download Confirm Screen'
        IP_EVT_UI_DL_RESUME_DL, // In case of ongoing download interruption. To return back to download progress bar screen.
        IP_EVT_UI_UT_RESUME_UT, // In case of 'Update Confirm Screen' interruption. To return back to 'Update Confirm Screen'
        IP_EVT_UI_UCS_RESUME_UCS, // In case of 'Update Complete Screen' interruption. To return back to 'Update Complete Screen'
        IP_EVT_UI_DMALERT, // DM session UI Alert. To display DM UI Alert screen.
        IP_EVT_UI_EXIT, // Exit.
        IP_EVT_UI_NULL // End of event index.

    } IPTH_UIHandleEvent;

    extern int ghDMEEvent;
    extern int ghUIEvent;
    extern char const *const gdme_status_msg[];


    extern int CreateThreadAndEvent(void);

    extern void *IPTH_DME_Task_Process();
    extern void IPTH_Send_DME_Event(IPTH_DMEHandleEvent Event);

    extern void *IPTH_UI_Task_Process();
    extern void IPTH_Send_UI_Event(IPTH_UIHandleEvent Event);

    extern void *IDEV_Pkg0_Listener_Process();
    extern int idev_is_pkg0_q_exist();
    extern int idev_send_pkg0_q(idev_Pkg0_MsgType *msg);
#ifdef __cplusplus
}
#endif


#endif /* __IDEV_THREAD_EVT_H__ */
