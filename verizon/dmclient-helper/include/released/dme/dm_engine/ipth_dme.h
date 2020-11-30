/******************************************************************************
 * Copyright (c) 2010 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_DME_H__
#define __IPTH_DME_H__

#include "dm_engine/ipth_device_support.h"
#include "dm_plugin/ipth_plugin_alert.h"

/**
 * \file
 *
 * This file includes the Device Management Engine (DME) interfaces, and
 * includes header files that provide full DME functionality.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Framework Initialization +
 *      ActiveCare Client Foundation: Session +
 *      ActiveCare Client Foundation: Server Notification Processing
 */
#define SUPPORT_MULTI_SESSION_ENABLED

/**
 * \brief Specifies Device Management Engine (DME) version and build information.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */
typedef struct ipth_DmeVersionInfo {
    unsigned short major;			/**< DME major version number */
    unsigned short minor;			/**< DME minor version number */
    const char *build;			    /**< DME build number */
} ipth_DmeVersionInfo_s;

/**
 * \brief Specifies Device Management Engine (DME) configuration information.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */
typedef struct ipth_DmeConfig {
    ipth_DmeOperatingMode_e mode;  /**< DME operating mode. */
    const char *configFile;        /**< Start-time configuration file or NULL. */
    const char *workingDir;        /**< DME working directory. */
    const char *pluginDir;         /**< Plug-in directory. */
    const char *treeStorageId;     /**< Default Management Tree storage ID. */
    const char *recoveryStorageId; /**< Recovery data storage ID. */
    const char *pluginOemDll;	   /**< OEM DLL library. */
    ipth_DmEngineDeviceSupport *devSupport; /**< UI callbacks. */
} ipth_DmeConfig_s;

/**
 * Device Management Engine (DME) session initiator flags for creating a new alert list.
 *
 * \par Constant Table
 *      Session initiator constant definitions
 * \par API Name
 *      ActiveCare Client Foundation: Common
 * End Description
 */
#define	IPTH_SESSION_INITIATOR_NONE		0  /**< Device Management session not initiated */
#define	IPTH_SESSION_INITIATOR_CLIENT	1  /**< Client initiates the DME session */
#define	IPTH_SESSION_INITIATOR_SERVER	2  /**< Server initiates the DME session */

/**
 * \brief Session attributes.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */
typedef struct ipth_SessionAttr {
    char               serverId[256]; /**< Server identifier */
    unsigned short     sessionId;     /**< Session identifier */
    unsigned short     initiator;     /**< One of IPTH_SESSION_INITIATOR_NONE,
									              IPTH_SESSION_INITIATOR_CLIENT,
									           or IPTH_SESSION_INITIATOR_SERVER */
} ipth_SessionAttr_s;

/**
 * \brief UI interaction modes for Package 0 notification.
 */
typedef enum {
    IPTH_UI_MODE_UNSPECIFIED=0,	/**< No recommendation from server */
    IPTH_UI_MODE_SILENT=1,		/**< Do not prompt user */
    IPTH_UI_MODE_INFORM=2,		/**< Inform user with a display */
    IPTH_UI_MODE_PROMPT=3		/**< Ask for user acceptance */
} ipth_UiMode;

/**
 * \brief Notification attributes.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */
typedef struct ipth_NotificationAttr {
    unsigned short       version;       /**< Notification message version. */
    ipth_SessionAttr_s   attr;          /**< Session attributes. */
    ipth_UiMode          uiMode;        /**< One of "not specified", "background",
										"informative" or "interaction". */
    unsigned short       vendorInfoLen; /**< Length of vendor-specific information. */
    unsigned char*       vendorInfo;	/**< Vendor information */
} ipth_NotificationAttr_s;

/**
 * \brief Specifies Device Management Engine (DME) operation types.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */
typedef enum ipth_DmeOperation {
    IPTH_DME_SESS_CONTINUE, /**< Continues pending processing after the DME yielded execution (applies to the DME running in consecutive mode only) */
    IPTH_DME_SESS_CANCEL,   /**< Requests the DME to gracefully cancel all running tasks */
    IPTH_DME_SESS_PAUSE,    /**< Requests the DME to pause execution (not supported)*/
    IPTH_DME_SESS_RESUME,   /**< Requests the DME to resume processing (not supported) */
    IPTH_DME_RECOVER,       /**< Requests the DME to recover tasks (if any) from the provided recovery storage */
    IPTH_DME_SHUTDOWN		/**< Requests the DME to shut down and cancels execution of any running task */
} ipth_DmeOperation_e;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Retrieves Device Management Engine (DME) version information. The function can be called at
     * any time, even before ipth_dme_initialize.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Framework Initialization
     *
     * \param [out] ver Pointer to the buffer receiving the DME version information.
     */
    void ipth_dme_getVersionInfo( ipth_DmeVersionInfo_s *ver);

    /**
     * Global initialization of Device Management Engine (DME). The developer must call this function
     * before calling any other DME functions.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Framework Initialization
     *
     * \param [in] cfg Start-time configuration file. It cannot be NULL.
     * \param [out] hDme DME environment handle.
     * \retval IPTH_OK Initialization is successful.
     * \retval IPTH_ERR Initialization failed. Refer to log message for more
     *		detailed failing reasons.
     */
    int ipth_dme_initialize( ipth_DmeConfig_s *cfg, ipth_DmeHandle *hDme);

    /**
     * Performs global finalization of the Device Management Engine (DME). Before calling this function, all
     * blocking callback functions of DME applications should be completed because
     * this call may take some time to wait for the DME applications to be completed.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Framework Initialization
     *
     * \param [in] hDme DME environment handle.
     */
    void ipth_dme_finalize( ipth_DmeHandle hDme);

    /**
     * Initializes Device Management Engine (DME) session attributes with default values.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Session
     *
     * \param [in] hDme DME environment handle.
     * \param [in] attr Session attributes.
     * \retval IPTH_OK Initialization is successful.
     * \retval IPTH_ERR Initialization failed. Refer to the log message details.
     */
    int ipth_dme_initSessionAttr( ipth_DmeHandle hDme, ipth_SessionAttr_s *attr);

    /**
     * Starts a Device Management Engine (DME) session with the server.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Session
     *
     * \param [in] hDme DME environment handle.
     * \param [in] attr Session attributes.
     * \param [in] alert Alert to send or NULL.
     * \param [in] pcb Session status callback or NULL.
     * \retval IPTH_OK Session is started successful.
     * \retval IPTH_ERR Session is started with error. Refer to the log message for details.
     */
    int ipth_dme_startSession( ipth_DmeHandle hDme, ipth_SessionAttr_s *attr, ipth_GenericAlert *alert, ipth_StatusCallback_s *pcb);

    /**
     * Executes asynchronous operations on active Device Management Engine (DME) sessions, if any.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Session
     *
     * \param [in] hDme DME environment handle.
     * \param [in] op Operation to perform.
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_dme_run( ipth_DmeHandle hDme, ipth_DmeOperation_e op);

    /**
     * Returns the total number of Device Management Engine (DME) sessions.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Session
     *
     * \param [in] hDme DME environment handle.
     *
     * \return Number of DME sessions.
     */
    int ipth_dme_numberOfSessions( ipth_DmeHandle hDme);

    /**
     * Blocks execution; waits for completion of all running Device Management Engine (DME) sessions, if any.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Session
     *
     * \param [in] hDme DME environment handle.
     * \retval IPTH_OK Indicates that no session is running.
     * \retval IPTH_ERR
     */
    int ipth_dme_wait( ipth_DmeHandle hDme);

    /**
     * Processes a Device Management Engine (DME) notification that the Client Application received from the server.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Server Notification Processing
     *
     * \param [in] hDme DME environment handle.
     * \param [in] msg Pointer to a buffer that holds a server notification to process.
     * \param [in] msgLen Notification length in bytes.
     * \param [out] attr Pointer to a structure receiving attributes extracted from the notification.
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_dme_processNotification( ipth_DmeHandle hDme, unsigned char *msg, unsigned long msgLen, ipth_NotificationAttr_s *attr);

    /**
     * Processes a bootstrap message received from the server.
     * If the serverId parameter is not NULL and the message contains new account info,
     * then the buffer receives server identifier when found, or an empty string.
     * If the given buffer of serverId is not null, it should be at least 256 bytes.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Server Notification Processing
     *
     * \param [in] hDme DME environment handle.
     * \param [in] msg Pointer to a buffer that holds a bootstrap message.
     * \param [in] msgLen Message length in bytes.
     * \param [out] serverId Pointer to a buffer receiving server identifier, if any, or NULL. If specified, buffer size must be at least 256 bytes.
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_dme_processBootstrap( ipth_DmeHandle hDme, unsigned char *msg, unsigned long msgLen, char *serverId);

#ifdef __cplusplus
}
#endif

#endif /* __IPTH_DME_H__ */

