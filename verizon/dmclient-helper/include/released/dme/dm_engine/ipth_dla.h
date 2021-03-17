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

#ifndef _IPTH_DLA_H_
#define _IPTH_DLA_H_

#include "ipth_hal_typedef.h"

/**
 * \file
 *
 * \brief This file includes the Download Agent (DLA) interfaces.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Common
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief Specifies the Download Agent (DLA) control commands.
     *
     */
    typedef enum {
        IPTH_DLA_CTL_CMD_UNDEFINED = 0, /**< Undefined */
        IPTH_DLA_CTL_CMD_CANCEL,	    /**< User cancels download */
        IPTH_DLA_CTL_CMD_PAUSE,		    /**< User pauses download */
        IPTH_DLA_CTL_CMD_RESUME,	    /**< User resumes download */
        IPTH_DLA_CTL_CMD_ABORT		    /**< Cancel download with no notification to the server */
    } IPTH_DLACtlCommand_e;

    /**
     * \brief Specifies Download Agent (DLA) notification flags.
     *
     */
    typedef enum
    {
        IPTH_DLANOTIFY_SUCCESS = 900,           /**< Success */
        IPTH_DLANOTIFY_INSUFFICIENT_MEMORY,     /**< Out of memory */
        IPTH_DLANOTIFY_USER_CANCEL,             /**< User canceled */
        IPTH_DLANOTIFY_LOSS_SERVICE,            /**< Loss of service */
        IPTH_DLANOTIFY_ATTRMISMATCH = 905,      /**< Attribute mismatch */
        IPTH_DLANOTIFY_INVALID_DESCR,	        /**< Invalid download descriptor */
        IPTH_DLANOTIFY_INVALID_DDVERSION = 951,	/**< Invalid download descriptor version */
        IPTH_DLANOTIFY_DEVICE_ABORT,	        /**< Device operation is aborted */
        IPTH_DLANOTIFY_NONACCEPT,	            /**< Not accepted */
        IPTH_DLANOTIFY_LOADERROR	            /**< Load error */
    } IPTH_DlaNotifyCode_e;

    /**
     * \brief Download descriptor information.
     * Every field corresponds to a field in the Generic Content Download Over The Air
     * Approved Version 1.0 - 25 Jun 2004 (OMA-Download_OTA-V1_0-20040625-A).
     * See also ipth_Dla_DD, which has the same fields using the character arrays.
     */
    typedef struct {
        char* name; /**< Download descriptor name */
        char* ddVersion; /**< Download descriptor version */
        char* objectURI; /**< URI of the object */
        char* mediaType; /**< Media type */
        char* vendor; /**< Vendor*/
        char* description; /**< Informative description */
        char* installNotifyURI; /**< URI to be notified when installed */
        char* nextURI; /**< Next URI */
        char* infoURI; /**< Info URI */
        char* iconURI; /**< URI for retrieving icon */
        char* installParam; /**< Install parameters */
        char* pPreDownloadMessage;/**< PreDownloadMessage VzW Specific*/
        char* pPostDownloadMessage;/**< PostDownloadMessage VzW Specific*/
        char* pPostUpdateMessage;/**< PostUpdateMessage VzW Specific*/
        char* pPreDownloadURL;/**< PreDownloadURL VzW Specific*/
        char* pPostDownloadURL;/**< PostDownloadURL VzW Specific*/
        char* pPostUpdateURL;/**< PostUpdateURL VzW Specific*/
        long objectSize; /**< Size (in bytes) of the object to be downloaded */
    } ipth_DlaDD;


    /**
     * \brief Download descriptor information. See also ipth_DlaDD, which has the same
     * fields using the character pointers.
     */
    typedef struct {
        char name[32];  /**< Download descriptor name */
        char ddVersion[32]; /**< Download descriptor version */
        char objectURI[128]; /**< URI of the object */
        char mediaType[32]; /**< Media type */
        char vendor[32]; /**< Vendor*/
        char description[128]; /**< Informative description */
        char installNotifyURI[128]; /**< URI to be notified when installed */
        char nextURI[128]; /**< Next URI */
        char infoURI[128]; /**< Info URI */
        char iconURI[128]; /**< URI for retrieving icon */
        char installParam[128]; /**< Install parameters */
        char preDownloadMessage[1025];/**< PreDownloadMessage VzW Specific*/
        char postDownloadMessage[1025];/**< PostDownloadMessage VzW Specific*/
        char postUpdateMessage[1025];/**< PostUpdateMessage VzW Specific*/
        char preDownloadURL[1025];/**< PreDownloadURL VzW Specific. NO DEFINITION FOR URL LENGTH JUST USING 1024 char long*/
        char postDownloadURL[1025];/**< PostDownloadURL VzW Specific*/
        char postUpdateURL[1025];/**< PostUpdateURL VzW Specific*/
        long objectSize; /**< Size (in bytes) of the object to be downloaded */
        int	downloadTime; /**< Estimated download time in seconds */
        int updateTime;  /**< Estimated install time in seconds */
        int severity; /**< Severity of the package to be downloaded */
    } ipth_Dla_DD;


    /**
     * \brief Specifies the download states.
     */
    typedef enum
    {
        IPTH_DOWNLOAD_INPROGRESS = 0, /**< In progress. */
        IPTH_DOWNLOAD_DONE            /**< Download completed. */
    } IPTH_DLAState;


    /**
     * \brief DL task status values.
     */
    typedef struct {
        IPTH_DLAState state; /**< State of download task */
        int percent;         /**< Downloaded percent of the object that is used
						      to update the download progress bar. The range is 0 to 100 */
    } ipth_DlaStatus;


    /**
     * \brief Specifies the function pointers to access media storage, which might
     * not be a file system.
     */
    typedef struct ipth_MediaStorage {
        unsigned long	(*get_available_space)( const char* name); /**< Function pointer to get available space */
        int				(*open_file)( const char* name, unsigned long length); /**< Function pointer to open a file */
        void			(*close_file)( const char* name); /**< Function pointer to close a file */
        int				(*write_file)( const char* name, void* data, unsigned long offset, unsigned long length); /**< Function pointer to write data to a file */
    } ipth_MediaStorage_s;

    /**
     *\brief Type definition for a callback function included in the ipth_DlaCallBack structure.
     *\param[in]  mediaTypeString Media type string.
     *\param[in]  data Void Pointer to data.
     *
     *\return The function pointers to access media storage, such as getting available
     *        space, open, write, and close. The media storage might not be a traditional
     *        file system.
     */
    typedef ipth_MediaStorage_s* (*ipth_DlaResolveMediaType)(const char *mediaTypeString, void* data);

    /**
     *\brief Checks the device capability and confirms the download.
     * Called after downloading the download descriptor, but before downloading the object.
     *\param[in]  ddPtr Pointer to the structure of download descriptor.
     *\param[in]  data Pointer to caller's private data.
     *\param[in]  pcb Pointer to a callback for asynchronous UI, or NULL.
     *\retval IPTH_UI_RETURN_FAILED
     *\retval IPTH_UI_RETURN_PENDING
     *\retval IPTH_UI_RETURN_OK
     *\retval IPTH_UI_RETURN_NEGATIVE
     *\retval IPTH_UI_RETURN_CANCEL
     *\retval IPTH_UI_RETURN_TIMEOUT
     *
     *\retval IPTH_ERR_DLADEV_INSUFFICIENTMEMORY
     *\retval IPTH_ERR_DLADEV_LACKCAPABILITY
     *\retval IPTH_ERR_DLADEV_INSTALLOBJ
     *\retval IPTH_ERR_DLADEV_BADRETV
     */
    typedef int (*ipth_DlaCheckAndConfirm)(ipth_DlaDD* ddPtr, void* data, ipth_DmeCompletionCallback_s *pcb);

    /**
     * \brief After successfully downloading the package, installs the package.
     *\param[in]  pkgFullPath Package full path.
     *\param[in]  data Pointer to the caller's private data.
     *\return Returns 0 if successful, non-zero if failed.
     */
    typedef int (*ipth_DlaInstallPackage)(const char* pkgFullPath, void* data);

    /**
     * \brief Called during object download to update progress bar.
     *\param[in]  percent Specifies downloaded percent. The range is 0 to 100.
     *\param[in]  data Pointer to caller's private data.
     */
    typedef void (*ipth_DlaUpdateProgress)(int percent, void* data);

    /**
     * Handle for capability and media installer checking for each download task. An
     * instance of the ipth_DlaCallBack structure is passed to the Download Agent (DLA)
     * during a download initialization. The fields of ipth_DlaCallBack are callback
     * functions. The DLA uses these callback functions to allow the caller to take
     * care of the content being received, to confirm the download, and to report the
     * status. Note on resolveMedia: If the callback function is not provided (it is
     * set to NULL), the DLA uses the pkgFullPath value in the ipth_DlaStartInfo
     * structure to store the downloaded content to a file. If the callback function is
     * provided, then calling ipth_DlaResolveMediaType returns a pointer to the
     * external, application specific interface (see ipth_MediaStorage_s structure)
     * that the DLA uses for content storing during download. This behavior is very
     * important because some OEMs need to store the update package into a private location
     * on the device that is not accessible through the file system.
     *
     */
    typedef struct {
        ipth_DlaResolveMediaType resolveMedia; /**< Pointer to functions where OEM provides a media content
										   (download package) installer (optional). If not provided,
										   set this value to NULL. */
        ipth_DlaCheckAndConfirm userConfirm; /**< Pointer to a function that checks the device
										 capability and confirms it is ok to start the download. */
        ipth_DlaUpdateProgress updateProgress; /**< Pointer to function which updates the progress bar. */
        void* data; /**< Pointer to caller's private data. */
    } ipth_DlaCallBack;

    /**
     * \brief Values used by the Device Management application to set the running configuration of a download task.
     *
     */
    typedef struct {
        char* url;
        /**< Target URL (required)*/
        char* pkgFullPath;
        /**< Directory and file name (required).
             Caller must ensure that DLA can create a temporary file
        	 that contains partial content in the directory.
        	 After successfully downloading, the temporary file is renamed
        	 and passed to the caller via a callback function.*/
        char* userAgent;
        /**< User agent for filling in header field (optional)*/
        //VZW_REQ_MMOTADM_7948 - "	The HTTP Header used in a FOTA transaction with the Download Server shall include the additional attribute: "x-device-id".
        char* xDeviceId;
        char* userName;
        /**< User name for authentication (optional)*/
        char* password;
        /**< Password for authentication (optional)*/
        unsigned long bufSize;
        /**< Buffer size for storing content
             or a range for partial download (optional). The default value is 64 KB*/
        int downloadMode;
        /**< Download mode (optional). 0 is full download, 1 is partial download. The default value is 0*/
        int retryTime;
        /**< Retry times if a network error occurs (optional). The default value is 3.*/

        int shortRetryInterval;
        int longRetryTime;
        int longRetryInterval;

        ipth_DlaCallBack* callback;
        /**< Pointer to callback function for the download descriptor and object */
    } ipth_DlaStartInfo;

#ifdef __cplusplus
}
#endif
#endif /* _IPTH_DLA_H_ */

