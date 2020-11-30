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

#ifndef __IPTH_HAL_DLL_H__
#define __IPTH_HAL_DLL_H__

#include <ipth_hal_config.h>

/**
 * \file
 *
 * \brief This file includes the dynamically loaded library (DLL) interfaces.
 *
 * \par API Name
 *      Device Manufacturer: DLL Management
 *
 */

IP_START_EXTERN_C

/**
 * \brief Opens a dynamically loaded library (DLL). Returns ipth_HALDllHandle,
 * a unique identifier to the opened DLL that can be
 * used to get symbols from the library or to close the library.
 *
 * \param[in] dllName Pointer to the name of the file to be opened.
 *
 * \return  Handle ipth_HALDllHandle if successful, or NULL if the operation failed.
 */
/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
ipth_HALDllHandle ipth_HALDllOpenLibrary(const char* dllName);

/**
 * \brief Closes a dynamically loaded library (DLL).
 *
 * \param[in] handleToDLL Handle that is a unique identifier of the opened DLL.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_ERR
 */
/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
int ipth_HALDllCloseLibrary(ipth_HALDllHandle handleToDLL);

/**
 * \brief Gets symbols from a dynamically loaded library (DLL).
 *
 * \param[in] handleToDLL Handle that is a unique identifier to the opened DLL.
 * \param[in] symbolName Name of a symbol in the opened library.
 *
 * \return Function pointer to the requested symbol, or NULL if the symbol was not found.
 */
/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
void* ipth_HALDllGetSymbol(ipth_HALDllHandle handleToDLL, const char* symbolName);

#if IPTH_STATIC_LINKING
/**
 * Defines static function pointer.
 *
 * \param [in] fnName Name of static function.
 *
 * \return Return void function pointer if the function with the specified name "fnName"
 *         is found, otherwise NULL.
 */
typedef void* (*ipth_HALStaticFunction)(const char *const fnName);

/**
 * \brief Registers a callback function if a project needs to statically
 * link some modules rather than dynamically load them. This function provides a
 * method to manage the modules as DLLs even though they are statically linked.
 * The staticFunction variable is used to simulate the ipth_HALDllGetSymbol function,
 * so it must return a pointer to the corresponding DLL function.
 *
 * \param[in] key String that identifies the statically linked library.
 * \param[in] staticFunction Function pointer to symbol resolution function.
 *
 * \retval IPTH_OK
 * \retval IPTH_ERR
 *
 *****************************************************************************/
int ipth_HALDllRegisterStaticFunction(const char *const key, const void *staticFunction);
#endif //IPTH_STATIC_LINKING

/**
 * Defines optional HAL interface that implements the handling of the OMA GET
 * operation in the management object module table.
 *
 * \par InnoPath Internal
 */
typedef int (*idev_HALGetDeviceMO) (const char* mount_uri, const char* rel_uri,
                                    char* data, int* data_size);

/**
 * Defines optional HAL interface that implements the handling of the OMA REPLACE
 * operation in the management object module table.
 *
 * \par InnoPath Internal
 */
typedef int (*idev_HALReplaceDeviceMO) (const char* mount_uri, const char* rel_uri,
                                        const char* data, int data_size);

/**
 * Defines optional HAL interface that implements the handling of the OMA EXEC
 * operation in the management object module table.
 *
 * \par InnoPath Internal
 */
typedef int (*idev_HALExecDeviceMO) (const char* mount_uri, const char* rel_uri);

/**
 * Defines optional HAL interface that implements the handling of the OMA ADD
 * operation in the management object module table.
 *
 * \par InnoPath Internal
 */

typedef int (*idev_HALAddDeviceMO) (const char* mount_uri, const char* rel_uri,
                                    const char* data, int data_size);
/**
 * Defines optional HAL interface that implements the handling of the OMA Delete
 * operation in the management object module table.
 *
 * \par InnoPath Internal
 */

typedef int (*idev_HALDeleteDeviceMO) (const char* mount_uri, const char* rel_uri);


IP_END_EXTERN_C

#endif /* __IPTH_HAL_DLL_H__ */

