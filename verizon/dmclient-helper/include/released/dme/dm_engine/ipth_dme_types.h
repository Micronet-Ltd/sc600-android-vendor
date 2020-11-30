/******************************************************************************
 * Copyright (c) 2003-2010 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_DME_TYPES_H__
#define __IPTH_DME_TYPES_H__

/**
 * \file
 *
 * \brief This file includes standard data types used by the Device Management (DM) client.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */

/**
 * \brief Defines a handle that represents the Device Management Engine (DME) environment
 */
typedef struct ipth_DmeEnv * ipth_DmeHandle; /**< Handle that hides the details of the DME environment from the user */

/**
 * \brief Specifies the data formats defined in the OMA DM standard.
 */
typedef enum ipth_FormatEnum {
    IPTH_FORMAT_UNKNOWN = 0, /**< Unknown format */
    IPTH_FORMAT_B64, /**< B64 encoded */
    IPTH_FORMAT_BIN, /**< Binary */
    IPTH_FORMAT_BOOL,/**< Boolean */
    IPTH_FORMAT_CHR, /**< Character */
    IPTH_FORMAT_INT, /**< Integer */
    IPTH_FORMAT_NODE, /**< Node */
    IPTH_FORMAT_NULL, /**< Null data */
    IPTH_FORMAT_XML, /**< XML encoded */
    IPTH_FORMAT_DATE, /**< Date */
    IPTH_FORMAT_TIME, /**< Time */
    IPTH_FORMAT_FLOAT /**<Float */
} ipth_Format;

/**
 * \brief Specifies the meta mark levels defined in the OMA DM standard.
 */
typedef enum {
    IPTH_DM_META_MARK_UNDEF = 0, /**< Undefined */
    IPTH_DM_META_MARK_FATAL,  /**< Fatal */
    IPTH_DM_META_MARK_CRITICAL, /**< Critical */
    IPTH_DM_META_MARK_MINOR, /**< Minor */
    IPTH_DM_META_MARK_WARNING, /**< Warning */
    IPTH_DM_META_MARK_INFORMATIONAL, /**< Informational */
    IPTH_DM_META_MARK_HARMLESS, /**< Harmless */
    IPTH_DM_META_MARK_INDETERMINATE /**< Indeterminate */
} ipth_MetaMark;

/**
 * Specifies UI alert control widget return codes.
 *
 * \par Constant Table
 *      UI alert control widget return codes
 * End Description
 */
#define IPTH_UI_RETURN_FAILED  -1	/**< UI operation failed. */
#define IPTH_UI_RETURN_PENDING  0	/**< UI operation is incomplete. */
#define IPTH_UI_RETURN_OK       1	/**< Confirming response from user. */
#define IPTH_UI_RETURN_NEGATIVE 2	/**< Negative response from user. */
#define IPTH_UI_RETURN_CANCEL   3	/**< User cancelled interaction. */
#define IPTH_UI_RETURN_TIMEOUT  4	/**< Operation timeout. */
#define IPTH_UI_RETURN_DEFER	5	/**< User deferred update. */
#define IPTH_UI_RETURN_CANCEL_PKG_CORRUPTED	6	/**< Client cancelled because package is corrupted. */
#define IPTH_UI_RETURN_CANCEL_PKG_INVALID	7	/**< Client cancelled because packgage is invalid. */
#define IPTH_UI_RETURN_SU_CANCEL_552  10

#define IPTH_UI_RETURN_SU_CANCEL_553  11
#define IPTH_UI_RETURN_SU_CANCEL_554  12

/**
 * Forward declaration
 * \par InnoPath Internal
 */
typedef struct ipth_ListNode ipth_AlertList;

/**
 * Forward declaration
 * \par InnoPath Internal
 */
typedef struct ipth_Command ipth_Alert;

/**
 * Forward declaration
 * \par InnoPath Internal
 */
typedef struct ipth_Item ipth_Item;

/**
 * Specifies the DME working modes.
 *
 * \par Constant Table
 *      DME working mode definitions
 * End Description
 */
typedef enum ipth_DmeOperatingMode {
    IPTH_DME_MODE_PREEMPTIVE = 0, /**< DME spawns threads to execute workflows, DM sessions, or module jobs */
    IPTH_DME_MODE_COOPERATIVE     /**< Shares the same thread for all tasks, yielding execution back to the caller to avoid blocking */
} ipth_DmeOperatingMode_e;

/**
 * Specifies the progress bar modes.
 *
 * \par Constant Table
 *      DME the progress bar mode definitions
 * End Description
 */
typedef enum ipth_ProgressBarMode {
    IPTH_PROGRESSBAR_MODE_DEFAULT = 0, /**< Default mode */
    IPTH_PROGRESSBAR_MODE_FUMO         /**< FUMO mode */
} ipth_ProgressBarMode_e;

/**
 * Specifies the status events used in work flow, session management, download, and FUMO.
 *
 * \par Constant Table
 *      DME work flow, session, download, and FUMO status event definitions
 * End Description
 */
/* DME status events */
#define IPTH_EVENT_WORKFLOW_STARTED     1    /**< Workflow started; parameter is a pointer to the server ID */
#define IPTH_EVENT_WORKFLOW_COMPLETED   2    /**< Workflow completed and about to be discarded; parameter is a pointer to the server ID */
#define IPTH_EVENT_IO_COMPLETED         3    /**< Asynchronous I/O request completed; no parameter */
#define IPTH_EVENT_IO_CONNECTION_REFUSED 6	 /**< server unavailable or network error */

/* DM Session status events, parameter is a pointer to ipth_DMSessionEvent_s */
#define IPTH_EVENT_DMSESSION_STARTED     101 /**< DM session started; parameter is a pointer to ipth_DMSessionEvent_s */
#define IPTH_EVENT_DMSESSION_COMPLETED   102 /**< DM session completed; parameter is a pointer to ipth_DMSessionEvent_s */
#define IPTH_EVENT_DMSESSION_ALERTSTATUS 103 /**< Status for an ALERT command received; parameter is a pointer to ipth_DMSessionEvent_s */
#define IPTH_EVENT_DMSESSION_EXEC        104 /**< EXEC command accepted for asynchronous processing; parameter is a pointer to ipth_DMSessionEvent_s */
#define IPTH_EVENT_DMSESSION_GET         105 /**< An GET command accepted for asynchronous processing */
#define IPTH_EVENT_DMSESSION_IO_SENT        106 /**< An DM session event for IO Send success */
#define IPTH_EVENT_DMSESSION_IO_RECEIVED    107 /**< An DM session event for IO receive success  */
/* 150 - 152 reserved*/

/* DLA status events, parameter is a pointer to ipth_DlaStatusEvent_s */
#define IPTH_EVENT_DOWNLOAD_STARTED      201 /**< Download started; parameter is a pointer to ipth_DlaStatusEvent_s */
#define IPTH_EVENT_DOWNLOAD_COMPLETED    202 /**< Download completed; parameter is a pointer to ipth_DlaStatusEvent_s */
#define IPTH_EVENT_DOWNLOAD_ERROR        203 /**< Download error; parameter is a pointer to ipth_DlaStatusEvent_s */

/* FOTA status events */
#define IPTH_EVENT_FUMO_STARTED          801 /**< FUMO started */
#define IPTH_EVENT_FUMO_COMPLETED        802 /**< FUMO completed */

/**
 * \brief Specifies status event specific parameters, if any, for a callback code.
 */
typedef struct ipth_StatusEvent {
    int		code;    /**< Event reported with the callback */
    void	*param; /**< Pointer to the event specific parameters, if any */
} ipth_StatusEvent_s;

/**
 * Performs the status event callback.
 *
 * \param[in] evt Status event structure.
 * \param[out] pData Pointer to user data where the callback function may return a value.
 *
 * \return Callback function may return a value in the pData parameter.
 */
typedef void ipth_StatusCallback_f( ipth_StatusEvent_s evt, void *pData);

/**
 * \brief Specifies status callback function and caller private data.
 */
typedef struct ipth_StatusCallback {
    ipth_StatusCallback_f *pFunc; /**< Pointer to the caller callback function */
    void *pData;                  /**< Pointer to the caller private data */
} ipth_StatusCallback_s;

/**
 * Initializes an ipth_StatusCallback_s or an ipth_DmeCompletionCallback_s structure.
 *
 * \param[in] cb Pointer to callback structure.
 * \param[in] f  Pointer to callback function.
 * \param[in] d  Pointer to caller private data.
 */
#define IPTH_DME_CALLBACK_INIT(cb,f,d) do { (cb)->pFunc = f; (cb)->pData = d; } while(0)

/**
 * \brief Specifies the parameters for the DM session events.
 */
typedef struct ipth_DMSessionEvent {
    const char * serverId; /**< Server ID. */
    const char *corr; /**< IPTH_EVENT_DMSESSION_EXEC  correlator*/
    union {
        int status;      /**< Status of the DM session when the event is
						 IPTH_EVENT_DMSESSION_COMPLETED */
        const char *uri; /**< URI related to the DM session when the event is
						 IPTH_EVENT_DMSESSION_EXEC*/


        struct _ipth_GenericAlertStatus *alertStatus; /**< Alert status of the DM session when the event is
													  IPTH_EVENT_DMSESSION_ALERTSTATUS*/
    } p;	/**< Includes status, uri, and alertStatus members. */
} ipth_DMSessionEvent_s;

/**
 * \brief Specifies the download status event.
 */
typedef struct ipth_DlaStatusEvent {
    const char *url;  /**< download URL */
    int status;       /**< download status */
} ipth_DlaStatusEvent_s;

#endif /* __IPTH_DME_TYPES_H__ */
