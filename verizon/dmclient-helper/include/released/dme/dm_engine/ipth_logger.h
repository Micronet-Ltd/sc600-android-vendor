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

#ifndef __IPTH_LOGGER_H__
#define __IPTH_LOGGER_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "ipth_hal_log.h"

    /**
     * \file
     *
     * \brief Logging utility for DM engine and DM applications.
     *
     * \par API Name
     *      ActiveCare Client Foundation
     * \par InnoPath Internal
     */

    /**
     * \brief Log level enumeration.
     * Each log message has level of importance associated with it.
     *
     * \par InnoPath Internal
     */
    typedef enum {
        IPTH_LOG_PANIC = IPTH_HAL_LOG_PANIC,
        /**< Panic: This level indicates that a fatal error occurred.
             The program is terminated abnormally and, depending on the operating system, a
        	   core dump may be triggered. The panic error level should not occur in a stable
        	   and fully tested module. */
        IPTH_LOG_CRITICAL = IPTH_HAL_LOG_CRITICAL,
        /**< Critical: This level indicates that a critical error occurred.
             This error is expected and handled, but the program should be terminated
             gracefully if a critical error happens. */
        IPTH_LOG_WARNING = IPTH_HAL_LOG_WARNING,
        /**< Warning: This level indicates that a non-critical error occurred. A warning error level
             is expected and is properly handled by the program so that the program can continue running
             correctly. */
        IPTH_LOG_INFO = IPTH_HAL_LOG_INFO,
        /**< Info: This level indicates that no error occurred. The logged message contains
        	   information such as context or event data. Each module is expected to provide informative
        	   messages that can be used in the case of an error to help reproduce the conditions
        	   that led to the error */
        IPTH_LOG_VERBOSE = IPTH_HAL_LOG_VERBOSE
        /**< Verbose: This level indicates that no error occurred. The logged message contains
             detailed event information that can be used to verify proper program execution
             (though the quantity of information my not help in debugging errors). */
    } ipth_LogLevel;

    /**
     * \brief Log a message.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \par InnoPath Internal
     *
     * \param [in] level log level
     * \param [in] format log message format control string
     * \param [in] ... optional arguments
     */
    int ipth_LogV(ipth_LogLevel level, const char *format, ...);

    /**
     * \brief Log a message with a handle.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \par InnoPath Internal
     *
     * \param [in] handle Void pointer
     * \param [in] level log level
     * \param [in] format log message format control string
     * \param [in] ... optional arguments
     */
    int ipth_LogH(void * handle, ipth_LogLevel level, const char *format, ...);

    /**
     * \brief Log a fixed message (no optional parameter).
     *
     * \par InnoPath Internal
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] level Log level.
     * \param [in] debugTrace Fixed strings with no variable list of arguments.
     */
    int ipth_LogF(ipth_LogLevel level, const char *debugTrace);


    /**
     * Defines the constants for the logging capability: uri, maximum size, log level, filter size
     * log data.
     *
     * \par InnoPath Internal
     *
     * \par Constant Table
     *      Logging parameter definitions
     * End Description
     */
#define IPTH_CONFIG_LOGGER_URI			"./InnoPath/Custom/Configuration/Logger"
#define IPTH_LOGGER_MAXLOGSIZE_URI		IPTH_CONFIG_LOGGER_URI"/maxLogSize"
#define IPTH_LOGGER_LOGGERLEVEL_URI		IPTH_CONFIG_LOGGER_URI"/loggerLevel"
#define IPTH_LOGGER_SIZEFILTER_URI		IPTH_CONFIG_LOGGER_URI"/sizeFilter"
#define IPTH_LOGGER_LOGDATA_URI			IPTH_CONFIG_LOGGER_URI"/logData"

#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_LOGGER_H__ */
