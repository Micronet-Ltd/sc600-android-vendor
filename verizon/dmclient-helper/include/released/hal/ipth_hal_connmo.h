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
#ifndef __IPTH_HAL_CONNMO_H__
#define __IPTH_HAL_CONNMO_H__

#include <ipth_hal_config.h>
#include <ipth_hal_dll.h>

IP_START_EXTERN_C

/**
 * \file
 *
 * This file includes the definition of the interfaces that are required to
 * register the ConnMO module with the Device Management Engine (DME) and
 * manage the corresponding Hardware Abstraction Layer (HAL)
 * APIs. In these HAL APIs, 'mount_uri' and 'rel_uri' together make
 * the full URI of the operation from the OMA server. The 'mount_uri'
 * is the portion of the URI that is provided in the URI value of this module in
 * the Binder.ddf file (located in the DDFs folder of the distributed files).
 *
 * \par API Name
 *      ActiveCare Client Libraries: Connectivity +
 *      ActiveCare Client Foundation: Framework Initialization
 */

/**
 * \brief Function handle that is required for registering the ConnMO module with the DME.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
void* ipth_GetFn_ConnMO(const char *const fnName);

/**
 * This is an OEM overriding API. A stub file is provided on the release CD.
 * \par InnoPath Internal
 */
IP_DLL_EXPORT
int idev_HALGetDeviceConnMO(const char* mount_uri, const char* rel_uri,
                            char* data, int* data_size);

/**
 * This is an OEM overriding API. A stub file is provided on the release CD.
 * \par InnoPath Internal
 */
IP_DLL_EXPORT
int idev_HALReplaceDeviceConnMO(const char* mount_uri, const char* rel_uri,
                                const char* data, const int data_size);

/**
 * This is an OEM overriding API. A stub file is provided on the release CD.
 * \par InnoPath Internal
 */
IP_DLL_EXPORT
int idev_HALExecDeviceConnMO(const char* mount_uri, const char* rel_uri);

/**
 * This is an OEM overriding API. A stub file is provided on the release CD.
 * \par InnoPath Internal
 */
IP_DLL_EXPORT
int idev_HALAddDeviceConnMO(const char* mount_uri, const char* rel_uri,
                            const char* data, const int data_size);

/**
 * This is an OEM overriding API. A stub file is provided on the release CD.
 * \par InnoPath Internal
 */
IP_DLL_EXPORT
int idev_HALDeleteDeviceConnMO(const char* mount_uri, const char* rel_uri);


/**
 * HAL interface that implements the handling of the OMA GET operation
 * in the ConnMO module table.
 *
 * \param [in] mount_uri Prefix portion of the URI that corresponds to the ConnMO module.
 * \param [in] rel_uri Suffix portion of the URI that corresponds to the ConnMO module.
 * \param [out] data Buffer that contains the value to be returned to the server for this request.
 * \param [in,out] data_size On input, specifies the size of the 'data' buffer that is provided. On return, specifies the actual bytes of 'data' that are valid.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_BUFFER_TOO_SMALL
 * \retval IPTH_HAL_INVALID_PARAMETER
 * \retval IPTH_HAL_ERR
 */
IP_DLL_EXPORT
int ipth_HALGetDeviceConnMO(const char* mount_uri, const char* rel_uri,
                            char* data, int* data_size);


/**
 * HAL interface that implements the handling of the OMA REPLACE operation
 * in the ConnMO module table.
 *
 * \param [in] mount_uri Prefix portion of the URI that corresponds to the ConnMO module.
 * \param [in] rel_uri Suffix portion of the URI that corresponds to the ConnMO module.
 * \param [in] data Buffer that contains the value to be replaced on the device.
 * \param [in] data_size Size of the 'data' buffer.
 *
 * \retval IPTH_HAL_OK
 * \retval IPTH_HAL_INVALID_PARAMETER
 * \retval IPTH_HAL_ERR
 */
IP_DLL_EXPORT
int ipth_HALReplaceDeviceConnMO(const char* mount_uri, const char* rel_uri,
                                const char* data, const int data_size);


/**
 * HAL interface that implements the handling of the OMA EXEC operation
 * in the ConnMO module table.
 *
 * \param [in] mount_uri Prefix portion of the URI that corresponds to the ConnMO module.
 * \param [in] rel_uri Suffix portion of the URI that corresponds to the ConnMO module.
 *
 * \return Status code of this EXEC operation.
 */
IP_DLL_EXPORT
int ipth_HALExecDeviceConnMO(const char* mount_uri, const char* rel_uri);

IP_DLL_EXPORT
int ipth_HALAddDeviceConnMO (const char* mount_uri, const char* rel_uri,
                             const char* data, const int data_size);

IP_DLL_EXPORT
int ipth_HALDeleteDeviceConnMO (const char* mount_uri, const char* rel_uri);
IP_END_EXTERN_C

#endif /*__IPTH_HAL_CONNMO_H__*/
