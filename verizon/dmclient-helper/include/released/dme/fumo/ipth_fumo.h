/******************************************************************************
 * Interface of FUMO enabler
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_FUMO_ENABLER_H__
#define __IPTH_FUMO_ENABLER_H__

#include "dm_plugin/ipth_plugin_instance.h"
#include "dm_plugin/ipth_plugin_callback.h"

/**
 * \file
 *
 * \brief Programming interface for FUMO enabler library.
 */

#ifdef	__cplusplus
extern "C" {
#endif

    /* FUMO callback function return code */
#define IPTH_FUMO_CALLBACK_INITIAL			(0)
#define IPTH_FUMO_CALLBACK_DONE				(1)
#define IPTH_FUMO_CALLBACK_PENDING			(2)

    /* FUMO callback function EXEC flag */
#define IPTH_FUMO_EXEC_NORMAL				(0)
#define IPTH_FUMO_EXEC_CRASH_RECOVERY		(2)
#define IPTH_FUMO_EXEC_PENDING_RECOVERY		(3)

    /* FUMO result code */
#define IPTH_FUMO_SUCCESS							(200)
#define IPTH_FUMO_CLIENT_ERROR						(400)
#define IPTH_FUMO_USER_CANCEL						(401)
#define IPTH_FUMO_PACKAGE_CORRUPTED					(402)
#define IPTH_FUMO_PACKAGE_MISMATCH					(403)
#define IPTH_FUMO_PACKAGE_INVALID			        (404)
#define IPTH_FUMO_PACKAGE_UNACCEPTABLE		        (405)
#define IPTH_FUMO_DOWNLOAD_AUTHENTICATION_FAILED	(406)
#define IPTH_FUMO_DOWNLOAD_TIMEOUT					(407)
#define IPTH_FUMO_NOT_IMPLEMENTED					(408)
#define IPTH_FUMO_UNDEFINED_ERROR					(409)
#define IPTH_FUMO_INSTALL_FAILED					(410)
#define IPTH_FUMO_BAD_URL					        (411)
#define IPTH_FUMO_DOWNLOAD_SERVER_UNAVAILABLE		(412)
#define IPTH_FUMO_DOWNLOAD_SERVER_ERROR				(500)
#define IPTH_FUMO_DOWNLOAD_OUT_OF_MEMORY			(501)
#define IPTH_FUMO_INSTALL_OUT_OF_MEMORY				(502)
#define IPTH_FUMO_NETWORK_ERR			            (503)
#define IPTH_FUMO_DOWNLOAD_ERR			            (504)
#define IPTH_FUMO_WIFI_CANCEL						(550)
#define IPTH_FUMO_SU_CANCEL_552			            (552)
#define IPTH_FUMO_SU_CANCEL_553			            (553)
#define IPTH_FUMO_SU_CANCEL_554			            (554)

    /* Forward declaration of the FUMO enabler private context */
    typedef struct ipth_FumoContext ipth_FumoContext_s;

    /* Forward declaration of the application private context */
    typedef struct ipth_FotaAppContext ipth_FotaAppContext_s;

    /* Task types */
    typedef enum {
        IPTH_FUMO_TASK_UNDEF = 0,
        IPTH_FUMO_TASK_DOWNLOAD,
        IPTH_FUMO_TASK_UPDATE,
        IPTH_FUMO_TASK_DOWNLOAD_AND_UPDATE
    } ipth_FumoTaskType;

    /* FUMO task states */
    typedef enum {
        IPTH_FUMO_INITIAL=0,
        IPTH_FUMO_NOTDELIVERED,
        IPTH_FUMO_DELIVERED,
        IPTH_FUMO_CONFIRMED,
        IPTH_FUMO_UPDATED,
        IPTH_FUMO_REPORTED
    } ipth_FumoState;

    /**
     * A structure that reports task information and status. It is part of
     * persistent task context.
     */
    typedef struct ipth_FumoTaskStatus {
        ipth_FumoTaskType type;
        ipth_FumoState state;
        int execStatus;			/* DONE or PENDING */
        ipth_DmeCompletionCallback_s *pcb; /* FUMO callback for async operations */
        ipth_FotaAppContext_s *fotaAppData; /* a pointer to the FOTA application private data */
        char * serverID;
        char * correlator;
        char * execUri;

        /* download related */
        char * pkgUri;
        int downloadResult;

        /* update related */
        int updateResult;

    } ipth_FumoTaskStatus;

    /* ====================== Action Handlers FOTA application should provide =================== */

    /**
     * Generic prototype of an action handler.
     *
     * \param[in] fumo	The fumo enabler.
     * \param[in] execFlag  Execution flag for handler. See notes on EXEC flag.
     * \param[in] blob		A private data passed back to the handler during registration.
     * \param[out] *result	If it is synchronous execution, it carries the result of
     *						the action.
     *
     * \retval FUMO_CALLBACK_DONE Download action is done.
     * \retval FUMO_CALLBACK_PENDING Download action is pending and will be resumed later.
     */
    typedef int (*ipth_FumoActionHandler)(ipth_FumoTaskStatus *taskDesc,
                                          int execFlag,
                                          int *result);

    /* ====================== Default DLA Handlers FOTA application should provide =================== */

    /**
     * Download confirmation handler.
     *
     * \param[in] taskDesc	The fumo task.
     * \param[in] ddPtr     Download Descriptor for the package to download.
     *
     * \retval IPTH_UI_RETURN_xxx user response.
     * \retval IPTH_ERR_DLADEV_xxx device related noncompliance code.
     */
    typedef int (*ipth_FumoDownloadConfirm)( ipth_FumoTaskStatus *taskDesc, ipth_DlaDD* ddPtr);

    /**
     * Download progress update handler.
     *
     * \param[in] taskDesc	The fumo task.
     * \param[in] percent   percent of package downloaded.
     *
     * \retval none
     */
    typedef void (*ipth_FumoDownloadProgress)( ipth_FumoTaskStatus *taskDesc, int percent);

    /**
     * Update confirmation handler.
     *
     * \param[in] taskDesc	The fumo task.
     * \param[in] ddPtr     Download Descriptor for the package to download.
     *
     * \retval IPTH_UI_RETURN_xxx user response.
     * \retval IPTH_ERR_DLADEV_xxx device related noncompliance code.
     */
    typedef int (*ipth_FumoUpdateConfirm)( ipth_FumoTaskStatus *taskDesc);

    /**
     * action handler structure that FOTA application needs to pass to
     * FUMO enabler.
     * Note. If the download handler is NULL, then the FUMO enabler uses
     * the DME default downloader and the confrm, progress and install handlers
     * must be provided, otherwise the plug-in implements its own downloader and
     * the handlers are ignored.
     */
    typedef struct ipth_FumoActionHandler_t {
        ipth_FumoDownloadConfirm confirmDownload;	/**< Download confirmation handler, or NULL */
        ipth_FumoDownloadProgress progress;	/**< Download progress handler, or NULL */
        ipth_FumoUpdateConfirm confirmUpdate;	/**< Update confirmation handler, or NULL */
        ipth_FumoActionHandler update;		/**< Update action handler */
        ipth_FotaAppContext_s* fotaAppData;	/**< Appliccation private data */
    } ipth_FumoHandlers;

    /**
     * Initialize FUMO enabler.
     *
     * \param[in] appHandle Application handle of FOTA application. Pass in NULL for now.
     * \param[in] fumoRootURI The target fumo subtree root node.
     * \param[in] persistentContextFile   Tell fumo enabler where to put the intermidia files.
     * \param[in] pkgFile  The path for in-band downloaded package file. Out-of-band
     *			package downloading is handled by download action handler supplied
     *			by FOTA client.
     * \param[in] asyncFlag if not 0, then enabler spawns its own thread
     * \param[in] actionHandler  Fumo action handlers.
     * \return	FUMO enabler context.
     */
    ipth_PluginInstance * ipth_FumoInit(const ipth_MOCallbacks* pCb,
                                        void * handle,
                                        const char* fumoRootURI,
                                        const char* persistentContextFile,
                                        const char* pkgFile,
                                        int asyncFlag,
                                        ipth_FumoHandlers *actionHandlers);

    /**
     * Uninitialize FUMO enabler. If there is any incomplete task, it must be
     * in PENDING state. Otherwise, next time when FUMO enabler is re-initialized,
     * the FUMO will recover as if there is crash.
     *
     * \param[in] fumo FUMO enabler context from the initialize routine.
     * \return	none.
     */
    void ipth_FumoExit( ipth_PluginInstance * plgInst);


    /*
     * This function registers a media storage interface associated with the
     * specified type.
     * If pointer to the storage interface is set to NULL, then the function
     * unregisters storage interface for the mediaTypeString.
     * The DLA downloads and handles different MIME type contents.
     * Data of each MIME type can be placed into a storage required by a
     * particular application registering corresponding functions (storage interfaces)
     * into the DLA.
     *
     * \param[in] fumo		The fumo enabler.
     * \param[in] mediaTypeString Type of media content for which the storage interface
     *                            is registered.
     * \param[in] storage Pointer to the storage access interface connected to the media type
     * \retval IPTH_OK			Registration succeeded.
     * \retval anything else	Registration failed.
     */
    int ipth_FumoRegisterMediaStorage( ipth_FumoContext_s *fumo, const char *mediaTypeString, ipth_MediaStorage_s* storage);

    /* ------------ helper routines ----------------- */

    /**
     * Returns a pointer to the application private data passed to the ipth_FumoInit()
     * as a member of ipth_FumoHandlers structure
     *
     * \param[in] fumo The FUMO context handle returned from ipth_FumoInit().
     * \return	pointer to the application private data.
     */
    ipth_FotaAppContext_s *ipth_FumoGetAppPrivData( ipth_FumoContext_s *fumo);



#ifdef	__cplusplus
}
#endif
#endif  /* __IPTH_FUMO_ENABLER_H__ */

