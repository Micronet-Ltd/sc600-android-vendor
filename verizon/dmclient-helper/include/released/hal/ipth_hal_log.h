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

#ifndef __IPTH_HAL_LOG_H__
#define __IPTH_HAL_LOG_H__

#include <ipth_hal_config.h>

/**
 * \file
 *
 * This file includes the log interfaces that are optionally provided by the device manufacturer.
 * When the log interface is defined, the InnoPath ActiveCare Client outputs
 * messages for integration, debugging, and troubleshooting. For example, the
 * device manufacturer can redirect the log messages to a serial port or
 * its own log system instead of writing to a file.
 *
 * \par API Name
 *      Device Manufacturer: Logging
 */

/**
 * \brief Specifies log levels, their level of importance, and their expected usage.
 */
typedef enum {
    IPTH_HAL_LOG_PANIC = 0,
    /**< Panic: This level indicates that a fatal error occurred.
         The program is terminated abnormally and, depending on the operating system, a
    	   core dump may be triggered. The panic error level should not occur in a stable
    	   and fully tested module. */
    IPTH_HAL_LOG_CRITICAL,
    /**< Critical: This level indicates that a critical error occurred.
         This error is expected and handled, but the program should be terminated
         gracefully if a critical error happens. */
    IPTH_HAL_LOG_WARNING,
    /**< Warning: This level indicates that a non-critical error occurred. A warning error level
         is expected and is properly handled by the program so that the program can continue running
         correctly. */
    IPTH_HAL_LOG_INFO,
    /**< Info: This level indicates that no error occurred. The logged message contains
    	 information such as context or event data. Each module is expected to provide informative
    	 messages that can be used in the case of an error to help reproduce the conditions
    	 that led to the error */
    IPTH_HAL_LOG_VERBOSE
    /**< Verbose: This level indicates that no error occurred. The logged message contains
         detailed event information that can be used to verify proper program execution
         (though the quantity of information my not help in debugging errors). */
} ipth_HALLogLevel;

IP_START_EXTERN_C

/**
 * Initializes logging. Use this function to initialize I/O and other resources.
 *
 * \par Sync (or) Async
 * This is a synchronous API
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_ERR
 * \retval IPTH_HAL_NOT_IMPLEMENTED
 * \return IPTH_HAL_NOT_IMPLEMENTED if Logging is not supported by OEM
 */
IP_DLL_EXPORT
int ipth_HALLogInit();

/**
 * Logs a fixed message.
 *
 * \par Sync (or) Async
 * This is a synchronous API.
 *
 * PARAMS:
 * \param [in] logLevel Log level
 * \param [in] debugTrace Constant message string.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_ERR
 * \retval IPTH_HAL_NOT_IMPLEMENTED
 * \return IPTH_HAL_NOT_IMPLEMENTED if Logging is not supported by OEM
 */
IP_DLL_EXPORT
int ipth_HALLog(ipth_HALLogLevel logLevel, const char *debugTrace);

/**
 * Finalizes logging. Use this function to close I/O and other resources.
 *
 * \par Sync (or) Async
 * This is a synchronous API.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_ERR
 */
IP_DLL_EXPORT
int ipth_HALLogClose();


IP_END_EXTERN_C

#endif /*__IPTH_HAL_LOG_H__*/
