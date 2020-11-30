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

#ifndef __IPTH_HAL_TIME_H__
#define __IPTH_HAL_TIME_H__

#include <ipth_hal_config.h>

IP_START_EXTERN_C

/**
 * \file
 *
 * \brief This file includes the time interfaces available to the device management (DM) client.
 *
 * The operating system platform-dependent header file ipth_hal_config.h supplies the
 * definitions for the __int8, __int16, and __int32 platform-specific types.
 *
 * \par API Name
 *      Device Manufacturer: Time Management
 *
 */


/**
 * \brief Specifies type of time.
 */
typedef enum {
    TT_LOCAL, /**< Local time */
    TT_UTC    /**< UTC time */
} ipth_TIME_TYPE;

/**
 * \brief Specifies the status of a time operation.
 */
typedef enum {
    TS_OK,             /**< OK */
    TS_ERROR,          /**< Error */
    TS_UINT32_OVERFLOW /**< Unsigned integer overflow */
} ipth_TIME_STATUS;

/**
 * \brief Defines values used to display time.
 */
typedef struct ipth_TimeItem {
    __int8 hour;    /**< Number of hours */
    __int8 minute;  /**< Number of minutes  */
    __int8 second;  /**< Number of seconds  */
} ipth_TimeItem;

/**
 * \brief Defines values used to display date.
 */
typedef struct ipth_DateItem {
    __int16 year;  /**< Number of years  */
    __int8 month;  /**< Number of months  */
    __int8 day;    /**< Number of days  */
} ipth_DateItem;

/**
 * \brief Defines values used to display date and time.
 */
typedef struct ipth_DateTimeItem {
    ipth_DateItem date;   /**< Date */
    ipth_TimeItem time;   /**< Time */
} ipth_DateTimeItem;

/**
 * \brief Gets the difference in seconds between two time structures and
 * returns the result and a status parameter that can indicate integer overflow.
 *
 * \param[in] time1 Pointer to a time structure
 * \param[in] time2 Pointer to a time structure
 * \param[out] timeDiffResult Returned pointer to the time difference in seconds
 * \param[out] status Returned pointer to an ipth_TIME_STATUS variable for status
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALGetTimeDiffInSeconds(const ipth_DateTimeItem *const time1,
                                 const ipth_DateTimeItem *const time2,
                                 unsigned __int32 *timeDiffResult,
                                 ipth_TIME_STATUS *status);

/**
 * \brief Compares two time structures and returns the comparison result.
 *
 * \param[in] time1 Pointer to a time structure
 * \param[in] time2 Pointer to a time structure
 * \param[out] compareResult Returned pointer to the comparison result.
 * The value is set as follows: -1 (time1 < time2);
 *     0 (time1 == time2);
 *     1 (time1 > time2).
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALCompareTimes(const ipth_DateTimeItem *const time1,
                         const ipth_DateTimeItem *const time2,
                         __int32 *compareResult);

/**
 * \brief Compares two time structures and returns whether
 * time1 is less than or equal to time2.
 *
 * \param[in] time1 Pointer to a time structure
 * \param[in] time2 Pointer to a time structure
 * \param[out] isLessOrEqual Returned pointer to the result as a boolean variable.
 * \retval IPTH_OK
 * \retval IPTH_ERR
 *
 */
IP_DLL_EXPORT
int ipth_HALIsTimeLessOrEqual(const ipth_DateTimeItem *const time1,
                              const ipth_DateTimeItem *const time2,
                              bool *isLessOrEqual);

/**
 * \brief Gets a time stamp. Note: This function is required for feature phone deployment.
 *
 * \return Number of seconds that have passed since January 1, 1970.
 */
IP_DLL_EXPORT
unsigned long ipth_HALGetTimestamp( void);

/**
 * \brief Gets system time based on specified time type.
 * Note: This function is required for feature phone deployment.
 *
 * \param[in] timeType Specifies the time type
 * \param[out] sysTime Returned pointer to the returned time structure
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALGetSystemTime(const ipth_TIME_TYPE timeType, ipth_DateTimeItem *sysTime);

/**
 * \brief Allocates and returns a string that holds the current time in the
 * POSIX strftime format "%Y%m%dT%H%M%S%Z".
 * Use ipth_HALStringFree to release the memory of the returned string.
 * Note: This function is required for feature phone deployment.
 *
 * \param[in] timeType Specifies the time type
 * \return String that holds the current time in the format "%Y%m%dT%H%M%S%Z"
 */
IP_DLL_EXPORT
char* ipth_HALGetSystemTimeAsString(const ipth_TIME_TYPE timeType);

/**
 * \brief Adds time (in milliseconds) to an ipth_DateTimeItem structure.
 *
 * \param[in] timeType Specifies the time type
 * \param[in] timeToAdvance Specifies the number of milliseconds to add
 * \param[in,out] time Returned pointer to the time structure to be advanced
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALAdvanceTimeWithMilliseconds(const ipth_TIME_TYPE timeType,
                                        unsigned __int32 timeToAdvance,
                                        ipth_DateTimeItem *time);

/**
 * \brief Calculates the difference between system time and an ipth_DateTimeItem structure.
 *
 * \param[in] localOrGTC Specifies the time type
 * \param[in] time Pointer to the time structure
 * \param[out] timeDiff Returned pointer to time difference in milliseconds
 * \param[out] status Returned pointer to an ipth_TIME_STATUS variable for status
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALDiffToSystemTimeInMilliSeconds(const ipth_TIME_TYPE localOrGTC,
        const ipth_DateTimeItem *const time,
        unsigned __int32 *timeDiff,
        ipth_TIME_STATUS *status);

/**
 * \brief Gets the day of the week from an ipth_DateItem structure.
 *
 * \param[in] time Pointer to the time structure
 * \param[out] dayOfWeek Returned pointer to the day of the week, where Sunday is 0, Monday is 1, etc.
 * \retval IPTH_OK
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALGetDayOfWeek(const ipth_DateItem *const time, unsigned __int8 *dayOfWeek);

/**
 * \brief Gets the internal tick count in milliseconds
 * OEM is only required to implement this function for Windows Mobile devices.
 *
 * \param[out] tickCount Returned pointer to tick count in milliseconds
 * \retval IPTH_OK
 * \retval IPTH_ERR_NOT_IMPLEMENTED  Target platform is not for Windows Mobile.
 * \retval IPTH_ERR
 */
IP_DLL_EXPORT
int ipth_HALGetTickCount(unsigned __int32 *tickCount);

IP_END_EXTERN_C

#endif /*__IPTH_HAL_TIME_H__*/
