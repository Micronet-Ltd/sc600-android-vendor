/******************************************************************************
 * DM engine extended interafaces.
 *
 * Copyright (c) 2003-2008 InnoPath Software, Inc. All rights reserved.
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

/**
* \file ipth_plugin_interface_ex.h
* \brief DM engine extended interfaces (callbacks).
*/

#ifndef __IPTH_DMEXTENDEDINTERFACE_H__
#define __IPTH_DMEXTENDEDINTERFACE_H__

/*=================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include "dm_plugin/ipth_plugin_callback.h"
#include "dm_engine/ipth_logger.h"
#include "dm_engine/ipth_dla.h"


#ifdef	__cplusplus
extern "C" {
#endif


    /*==================================================================================================
                                     STRUCTURES AND OTHER TYPEDEFS
    ==================================================================================================*/



    /**
     * \brief Plug-in uses this callback to log a message.
     *
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param [in] level log level
     * \param [in] format log message format control string
     * \param [in] ... optional arguments
     * \return error code.
     */
    typedef int (*ipth_CallbackLog)(void * handle,
    ipth_LogLevel level,
    const char *format, ...);




    /**
     * \brief Plug-in uses this callback to start a download task.
     * Set the related running config by start info.
     * Allocate buffer for the DL task's context and store the context into file.
     * dlID is fixed during the whole lifecycle of the specific download task.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] info configuration information for the download task.
     * \param[out] dlID identifier for the download task.
     * \return IPTH_OK successfully initialize download
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
     * \return IPTH_ERR_INVALID_ARGUMENT invalid argument
     * \return IPTH_ERR_MEM_ALLOC insufficient memory
     * \return IPTH_ERR_OPEN can't open context file
     * \return IPTH_ERR_WRITE failed in writing context file
    */
    typedef int (*ipth_CallbackInitializeDownload)(void * handle,
            ipth_DlaStartInfo* info,
            int* dlID);

    /**
     * \brief Plug-in uses this callback to process the transaction for the download task.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  dlID identifier for the download task.
     * \return IPTH_OK successfully finish download
     * \return more error code referring to error.h
    */
    typedef int (*ipth_CallbackProcessDownload)(void *handle,
            int dlID);

    /**
     * Plug-in uses this callback to finalize the download task. Clean task context.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  dlID identifier for the download task.
     * \return IPTH_OK successfully finalize download
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
    */
    typedef int (*ipth_CallbackFinalizeDownload)(void * handle,
            int dlID);
    /**
     * \brief Plug-in uses this callback to get the status for the download task.
     * It's caller's duty to free the related memory.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  dlID identifier for the download task.
     * \param[out] status the status of the specified download task.
     * \return IPTH_OK successfully get download status
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
     * \return IPTH_ERR_INVALID_ARGUMENT invalid argument
     * \return IPTH_ERR_NOT_EXIST no related download task
    */
    typedef int (*ipth_CallbackGetDownloadStatus)(void * handle,
            int dlID,
            ipth_DlaStatus* status);

    /**
     * \brief Plug-in uses this callback to control the download task.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  dlID identifier for the download task.
     * \param[in]  ctlCmd controll command ( see IPTH_DLACtlCommand_e) or a quit download status code ( see IPTH_DlaNotifyCode_e)
     * \return IPTH_OK succeeded
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
     * \return IPTH_ERR_NOT_EXIST no related download task
     * \return IPTH_ERR_OPEN can't open context file
     * \return IPTH_ERR_WRITE failed in writing context file
    */
    typedef int (*ipth_CallbackControlDownload)(void * handle,
            int dlID,
            int ctlCmd);


    /**
     * \brief Plug-in uses this callback to get task identifier by the specified URL.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  url object url.
     * \param[in]  pid pointer to the  task identifier.
     * \return IPTH_OK successfully get download id
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
     * \return IPTH_ERR_INVALID_ARGUMENT invalid argument
     * \return IPTH_ERR_NOT_EXIST no related download task
    */
    typedef int (*ipth_CallbackGetDownloadIdByUrl)(void * handle,
            const char* url,
            int* pid);

    /**
     * \brief Plug-in uses this callback to restart the aborted download task by the specified task identifier.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in]  dlID identifier for the download task.
     * \param[in]  callback callback function for user confirmation,
     *             object installation and progress bar update.
     * \return IPTH_OK successfully restart download
     * \return IPTH_ERR_NOT_INITIALIZED dl agent is not initialized
     * \return IPTH_ERR_INVALID_ARGUMENT invalid argument
     * \return IPTH_ERR_NOT_EXIST no related download task
     * \return IPTH_ERR_MEM_ALLOC insufficient memory
     * \return IPTH_ERR_READ failed in read dd file
     * \return IPTH_ERR_DLADD_PARSE dd parse error
     * \return IPTH_ERR_DLADD_INVALID invalid dd content
     * \return IPTH_ERR_DLADD_INVALIDVERSION invalid dd version
    */
    typedef int (*ipth_CallbackRestartDownload)(void * handle,
            int dlID,
            ipth_DlaCallBack* callback);





    /**
     * \brief DME extended Logger interface.
     *
     */
    typedef struct {
        ipth_CallbackLog Log; /**< Callback to log a message */
    } ipth_LoggerCallbacks;


    /**
     * \brief DME extended Log Data Upload interface.
     *
     */
    typedef struct ipth_logUploadInterface {
        int (*ipth_getLoggerLevel_fnPtr)(void *ctx);
        void (*ipth_setLoggerLevel_fnPtr)(void *ctx, int level);
        int (*ipth_getMaxLogSize_fnPtr)(void *ctx);
        void (*ipth_setMaxLogSize_fnPtr)(void *ctx, int size);
        int (*ipth_getLogData_fnPtr)(void *ctx, int size, char *buf);
        void *loggerCtxt;
    } ipth_logUploadInterface_s;


    /**
     * \brief DME extended DLA interface.
     *
     */
    typedef struct {
        ipth_CallbackInitializeDownload Init; /**< Callback to start a download task */
        ipth_CallbackProcessDownload Process; /**< Callback to process the transaction for the download task */
        ipth_CallbackFinalizeDownload Finalize; /**< Callback to  finalize the download task */
        ipth_CallbackGetDownloadStatus GetStatus; /**< Callback to get the status for the download task */
        ipth_CallbackControlDownload Control; /**< Callback to to cancel the download task by the specified task identifier */
        ipth_CallbackGetDownloadIdByUrl GetDownloadId; /**< Callback to get task identifier by the specified URL */
        ipth_CallbackRestartDownload Restart; /**< Callback to to restart the aborted download task  */
    } ipth_DLACallbacks;


    /**
     * \brief DME extended WFC interface.
     *
     */
    typedef struct ipth_WFCCallbacks {
        int (*ipth_wfcExecCallback)( const char * serverId, const char *uri, void *wfcPrivData);
        int (*ipth_wfcAlertCallback)( const char * serverId, ipth_GenericAlert * alert, int isDone, void *wfcPrivData);
        void *wfcPrivData;
    } ipth_WFCCallbacks_s;

#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_DMEXTENDEDINTERFACE_H__ */

