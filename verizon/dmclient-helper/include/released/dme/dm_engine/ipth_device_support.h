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

#ifndef __IPTH_DEVICE_SUPPORT_H__
#define __IPTH_DEVICE_SUPPORT_H__

#include "ipth_hal_typedef.h"
#include <dm_engine/ipth_dme_types.h>

/** \file
 *
 * This file declares the struct ipth_DmEngineDeviceSupport that contains a
 * list of function prototypes that the developer must implement in order to
 * use the Device Management Engine (DME).
 *
 * \par API Name
 *      ActiveCare Client Foundation: Server Notification Processing
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief UI alert control widget types.
     *
     */
    typedef enum {
        IPTH_UIALERT_DISPLAY,		/**< User displayed notification. */
        IPTH_UIALERT_CONFIRM,		/**< User confirm notification. */
        IPTH_UIALERT_TEXTINPUT,		/**< Prompt user to provide input. */
        IPTH_UIALERT_SINGLECHOICE,	/**< Prompt user to select one item. */
        IPTH_UIALERT_MULTICHOICE,	/**< Prompt user to select multiple items. */
        IPTH_UIMESSAGE_NETERR       /**< Communication error. Prompt user to retry. */
    } ipth_UIAlertType;

    /**
     * \brief Text input types.
     *
     */
    typedef enum {
        IPTH_UI_TEXT_INPUT_ALPHA,	/**< Alphanumeric input. */
        IPTH_UI_TEXT_INPUT_NUM,		/**< Numeric input (numerals, decimal point, + or - sign). */
        IPTH_UI_TEXT_INPUT_DATE,	/**< Date in format "DDMMYYYY". */
        IPTH_UI_TEXT_INPUT_TIME,	/**< Time in format "HHMMSS". */
        IPTH_UI_TEXT_INPUT_PHNMBR,  /**< Phone number (numerals, "+", "p", "w", "s"). */
        IPTH_UI_TEXT_INPUT_IPADDR   /**< IP Address in format "xxx.yyy.zzz.www". */
    } ipth_UITextInputType;

    /**
     * \brief UI input echo types.
     */
    typedef enum {
        IPTH_UI_ECHO_TYPE_TEXT,		/**< Normal text input echo. */
        IPTH_UI_ECHO_TYPE_PASSWORD	/**< Password input echo. */
    } ipth_UIEchoType;

    /**
     * \brief DME passes this information in to
     *        ipth_DisplayUIAlert method to display a UI alert.
     */
    typedef struct {
        ipth_UIAlertType alertType;	/**< Type of the UI Alert */
        unsigned int minDispTime;	/**< Minimum type to display the widget or 0. */
        unsigned int maxDispTime;	/**< Maximum time to display the widget or 0. */
        char *prompt;				/**< Text to display to user */
        char **items;				/**< A null terminated list of strings of items. */
        char *rsp;					/**< User response: A string for text input that is an item
								number for single choice such as "1" or "3", or a list of
								item numbers that are separated by "-" for multiple choice
								such as "2-3-5". */
        char *defRsp;				/**< Initially displayed response text. */
        /**< If user interaction is: */
        /**< NOTIFICATION: parameter is ignored; */
        /**< CONFIRMATION: 0 = reject highlighted by default,
         				   1 = accept highlighted by default; */
        /**< USER INPUT: Default text for text box; */
        /**< SINGLE-CHOICE: item to be highlighted by default; */
        /**< MULTI-CHOICE: Highlight multi choice items, e.g., "1-3-5" on display. */
        unsigned int maxInputLen;	/**< Maximum allowed user input length or 0. */
        ipth_UITextInputType tType; /**< Type of text input. */
        ipth_UIEchoType eType;		/**< Type of echo. */
        char* alertDesc;            /**< Description of alert. */
        unsigned int updateTime;    /**< Time needed to perform the update */
        char* updatePkgName;        /**< File name of update package. */
        char* vendorName;           /**< Vendor name. */
        unsigned int pkgSize;       /**< Size of update package in bytes. */
    } ipth_UIAlertData;

    /**
     * Events used to control processing of a long-lasting operation monitored with
     * a progress bar (see ipth_DisplayProgressBar and ipth_UpdateProgressBar).
     *
     * \par API Name
     *      ActiveCare Client Foundation: Common
     *
     * \par Constant Table
     *      Progress bar constant definitions
     * End Description
     */
#define	IPTH_PROGBAR_INPUT_EVENT_CANCEL 1	/**< User cancels monitored operation. */
#define	IPTH_PROGBAR_INPUT_EVENT_PAUSE  2	/**< User pauses operation processing. */
#define	IPTH_PROGBAR_INPUT_EVENT_RESUME 3	/**< User resumes operation processing. */

    /**
     * \brief Processes a UI alert.
     * This function, implemented by the developer, is passed as a pointer
     * to the DME during initialization.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] uiCmdData Pointer to ipth_UIAlertData structure to carry the
     *                           input data and response. The caller allocates and
     *                           deallocates the memory for the input data. The callee
     *                           allocates memory for the response, which is deallocated
     *                           by the caller.
     * \param [in] appData Pointer to application data.
     * \param [in] pcb Pointer to ipth_DmeCompletionCallback_s data structure.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR Low-level or native system errors.
     * \retval IPTH_ERR_UNSUPPORTED
     */
    typedef int (*ipth_DisplayUIAlert)( ipth_UIAlertData *uiCmdData, void *appData, ipth_DmeCompletionCallback_s *pcb);

    /**
     * \brief Creates a progress bar.
     * This function, implemented by the developer, is passed as a pointer
     * to the DME during initialization.
     * This function, together with ipth_UpdateProgressBar, is used by DME components
     * to notify the device with the progress status of a long-lasting continuous operation.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] pTitle  A name that will display at the top of progress bar,
     *                     NULL equals default name.
     * \param [in] appData Pointer to application data.
     * \param [in] pcb  Pointer to callback function to control execution. The function
     *                  can be fired by the device by
     *                  providing one of the IPTH_PROGBAR_INPUT_EVENT_CANCEL, etc., codes in order
     *                  to cancel, pause, or resume processing. Do not invoke the function
     *                  after the ipth_UpdateProgressBar function has reported 100% completion.
     *
     * \return Returns pointer to progress bar structure if success; otherwise, NULL.
     */
    typedef void* (*ipth_DisplayProgressBar)( const char* pTitle, void *appData, ipth_DmeCompletionCallback_s *pcb);

    /**
     * \brief Updates a progress bar.
     * This function, implemented by the developer, is passed as a pointer to
     * the DME during initialization.
     * This function is called by the DME to report progress status of a
     * long-lasting continuous operation.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] bar  Pointer on progress bar.
     * \param [in] percent Percentage of completion, value in the range from 0 to 100.
     *
     */
    typedef void (*ipth_UpdateProgressBar)(void* bar, int percent);

    /**
     * \brief Reports changes of the DME working status.
     * This function, implemented by the developer, is
     * passed as a pointer to the command processor.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param status  DME status reported.
     * \param appData Pointer to the application private data.
     *
     */
    typedef ipth_StatusCallback_f *ipth_ReportDmeStatus;


    /**
     * \brief Device support interface for the DME.
     */
    typedef struct ipth_DmEngineDeviceSupport {
        ipth_DisplayUIAlert displayAlert;           /**< UI alert display handler */
        ipth_DisplayProgressBar displayProgressBar; /**< Progress bar display handler */
        ipth_UpdateProgressBar updateProgressBar;   /**< Progress bar update handler */
        ipth_ReportDmeStatus reportDmeStatus;       /**< DME progress reporter */
        void *appData;                              /**< Application private data */
    } ipth_DmEngineDeviceSupport;

#ifdef __cplusplus
}
#endif

#endif /* __IPTH_DEVICE_SUPPORT_H__ */

