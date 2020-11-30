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

#ifndef __IPTH_HAL_THREAD_H__
#define __IPTH_HAL_THREAD_H__

/**
 * \file
 *
 * This file includes the thread and synchronization interfaces for the native OS.
 *
 * The OS-platform dependent header ipth_hal_config.h must supply definitions for:
 *	ipth_HALThreadId,
 *	ipth_HALSemaphore,
 *	ipth_HALMutex,
 *	ipth_HALCondVar.
 *
 * \par API Name
 *      Device Manufacturer: Thread Management
 */

#include <ipth_hal_config.h>

IP_START_EXTERN_C

/*################################  GENERAL  ################################*/
/**
 * Sleeps for the specified number of milliseconds.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 *
 * \param[in] milliseconds Number of milliseconds to sleep.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSleep(unsigned int milliseconds);

/*#################################  THREAD  ################################*/

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Creates a thread.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_PREEMPTIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[out] thread       Holds thread ID upon successful creation. A NULL value is OK.
 * \param[in] start_routine Pointer to the startup function of the new thread.
 * \param[in] arg           Argument passed to the startup function of the new thread.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_ERR_UNSUPPORTED DME client mode is set to IPTH_DME_MODE_COOPERATIVE.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALThreadCreate(ipth_HALThreadId *thread,
                         void *(*start_routine)(void*),
                         void *arg);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Returns the calling thread ID.
 * Note: This function is required for feature phone development.
 *
 * \return Thread ID of the calling thread.
 *
 *****************************************************************************/
IP_DLL_EXPORT
ipth_HALThreadId ipth_HALThreadSelf(void);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Compares whether two thread IDs are equal.
 * Note: This function is required for feature phone development.
 *
 * \param[in] t1 First thread ID.
 * \param[in] t2 Second thread ID.
 * \return Non-zero if the threads are equal; otherwise, zero if the threads are not equal.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALThreadEqual(ipth_HALThreadId t1, ipth_HALThreadId t2);

/*#################################  MUTEX  #################################*/

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Initializes the mutex as a recursive mutex.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] mutex Pointer to a mutex.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALMutexInit(ipth_HALMutex *mutex);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Destroys the objects referenced by a mutex. If the mutex is
 * currently locked by a thread, this function returns an error.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] mutex Pointer to a mutex.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALMutexDestroy(ipth_HALMutex *mutex);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Locks the mutex. Locking a mutex is nestable. For example, the same thread can
 * lock the same mutex multiple times. To free a mutex for use by others, the
 * mutex must be unlocked the same number of times it was locked.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] mutex Pointer to a mutex.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALMutexLock(ipth_HALMutex *mutex);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Unlocks the mutex. To free a mutex for use by others, the
 * mutex must be unlocked the same number of times it was locked.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] mutex Pointer to a mutex.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALMutexUnlock(ipth_HALMutex *mutex);

/*#########################  CONDITIONAL VARIABLES  #########################*/

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Initializes a conditional variable.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] cond Pointer to a conditional variable.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALCondVarInit(ipth_HALCondVar * cond);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Destroys any references in the conditional variable.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] cond Pointer to a conditional variable.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALCondVarDestroy(ipth_HALCondVar *cond);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Wakes up all threads waiting on the conditional variable.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] cond Pointer to a conditional variable.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALCondVarBroadcast(ipth_HALCondVar *cond);

/**
 * Wakes up one thread waiting on the conditional variable.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 *
 * \param[in,out] cond Pointer to a conditional variable.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALCondVarSignal(ipth_HALCondVar *cond);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Waits on the conditional variable.
 * This function must be called with the mutex already acquired.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] cond Pointer to a conditional variable.
 * \param[in] mutex Acquired mutex.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALCondVarWait(ipth_HALCondVar *cond, ipth_HALMutex *mutex);

/*##############################  SEMAPHORE  ################################*/

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Initializes a semaphore.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] sem Pointer to a semaphore.
 * \param[in] value Initial count for the semaphore.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSemaphoreInit(ipth_HALSemaphore *sem, unsigned int value);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Destroys objects referenced by the semaphore.
 * Returns an error if threads are waiting on the semaphore.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] sem Pointer to a semaphore.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSemaphoreDestroy(ipth_HALSemaphore *sem);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Waits on a semaphore.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] sem Pointer to a semaphore.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSemaphoreWait(ipth_HALSemaphore *sem);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Signals a semaphore.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] sem Pointer to a semaphore.
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSemaphorePost(ipth_HALSemaphore *sem);

/**
 * Gets current count in the semaphore. Some implementations may return a
 * negative value to indicate the number of threads waiting on this semaphore.
 * However, this is an optional feature that the caller should not depend on.
 * OEM is only required to implement this function when the DME mode is set to IPTH_DME_MODE_COOPERATIVE.
 *
 * \param[in,out] sem Pointer to a semaphore.
 * \param[out] val Current semaphore count.
 *
 * \return IPTH_HAL_OK returned if successful or not supported; otherwise, non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALSemaphoreGetValue(ipth_HALSemaphore *sem, int *val);

IP_END_EXTERN_C

#endif /* __IPTH_HAL_THREAD_H__ */

