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

#ifndef __IPTH_HAL_TYPEDEF_H__
#define __IPTH_HAL_TYPEDEF_H__

/**
 * \file
 *
 * \brief This file includes the hardware abstraction layer (HAL) interfaces.
 *
 * \par API Name
 *      Device Manufacturer: Common
 */

/**
 * Calculates the number of entries in a one-dimensional array
 *
 * \par InnoPath Internal
 * For internal use only.
 *
 * \param x Array used for calculation.
 */
#define dim(x) sizeof(x) / sizeof(x[0])

#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

/**
 * Standard return value error codes.
 *
 * \par API Name
 *      Error and Status Codes
 * \par Constant Table
 *      Standard return value error code definitions
 * End Description
 */
#define IPTH_OK                         0 /**< No error */
#define IPTH_ERR                        1 /**< Generic error */
#define IPTH_ERR_INVALID_ARGUMENT       2 /**< Invalid argument */
#define IPTH_ERR_OPERATION_DISALLOWED   3 /**< Operation disallowed due to state constraints or other factors */
#define IPTH_ERR_MEM_ALLOC              4 /**< Memory allocation error */
#define IPTH_ERR_NOT_EXIST              5 /**< Generic error for item not found  */
#define IPTH_ERR_ALREADY_EXIST          6 /**< Generic duplication error */
#define IPTH_ERR_NOT_INITIALIZED        7 /**< Module not initialized */
#define IPTH_ERR_ALREADY_INITIALIZED    8 /**< Module already initialized */
#define IPTH_ERR_OPEN                   9 /**< File not opened */
#define IPTH_ERR_ALREADY_OPEN           10 /**< File is already opened */
#define IPTH_ERR_READ                   11 /**< Read error */
#define IPTH_ERR_WRITE                  12 /**< Write error */
#define IPTH_ERR_THREAD                 13 /**< Error creating thread */
#define IPTH_ERR_SERVER_NACK            14 /**< Server not acknowledging alerts */
#define IPTH_ERR_UNSUPPORTED            15 /**< Unsupported feature or OMA DM version */
#define IPTH_ERR_COMMUNICATION          16 /**< Communication error occurred */
#define IPTH_ERR_CANCEL                 17 /**< The session was cancelled */
#define IPTH_ERR_DMACC                  18 /**< No Device Management account found for server */
#define IPTH_ERR_AUTH                   19 /**< Authentication failure */
#define IPTH_ERR_NOT_IMPLEMENTED        20 /**< Expected operation not yet implemented */
#define IPTH_ERR_IO_TIMEOUT             21 /**< I/O request timeout */
#define IPTH_ERR_IO_PENDING             22 /**< An asynchronous request is in progress */
#define IPTH_ERR_UNREGISTERED_DEVICE    23 /**< Unregistered device */
#define IPTH_ERR_IO_CONNECTION_REFUSED  111 /**< socket error */

/**
 * HAL return value error codes.
 *
 * \par API Name
 *      Error and Status Codes
 * \par Constant Table
 *      HAL return value error code definitions
 * End Description
 */
#define IPTH_HAL_OK					IPTH_OK /**< No error */
#define IPTH_HAL_ERR				IPTH_ERR /**< Generic error */
#define IPTH_HAL_NOT_IMPLEMENTED	IPTH_ERR_NOT_IMPLEMENTED /**< Expected operation not yet implemented */
#define IPTH_HAL_INVALID_PARAMETER	IPTH_ERR_INVALID_ARGUMENT /**< Invalid argument */
#define IPTH_HAL_ERR_MEM_ALLOC		IPTH_ERR_MEM_ALLOC /**< Memory allocation error */
#define IPTH_HAL_BUFFER_TOO_SMALL	-5 /**< Buffer too small to be used */
#define IPTH_HAL_NOT_SUPPORTED		-6 /**< Feature or operation not supported on the platform */

/**
 * \brief Defines the completion callback function and DME private context.
 * The HAL implementation must call pFunc upon completion of the asynchronous API.
 */
typedef struct ipth_DmeCompletionCallback {
    void (*pFunc)( void *pData, int completion_code); /**< Pointer to function that must
													  be called on completion of the asynchronous API */
    void *pData; /**< Pointer to the DME private context */
} ipth_DmeCompletionCallback;
/**
 * \brief Defines DME callback context for asynchronous task processing.
 */
typedef ipth_DmeCompletionCallback ipth_DmeCompletionCallback_s;
/**
 * \brief Defines DME callback context for asynchronous HAL APIs.
 */
typedef ipth_DmeCompletionCallback ipth_HALCallback_s;


/**
 * Notifies DME with a completion code, when an asynchronous task is complete.
 *
 * \param[in] p  Pointer to ipth_DmeCompletionCallback.
 * \param[in] c  Completion code.
 */
#define IPTH_DME_NOTIFY(p, c) (p)->pFunc((p)->pData, c)

// For C++ header support
#ifdef __cplusplus
#define IP_START_EXTERN_C extern "C" {
#define IP_END_EXTERN_C }
#else
#define IP_START_EXTERN_C
#define IP_END_EXTERN_C
#endif /* __cplusplus */

/**
 * Once true, goto an error label.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define IPTH_GOTO_ERROR(x, errorTag)  \
	if (x) { \
		goto errorTag; \
    }


#endif //__IPTH_HAL_CONFIG_H__

