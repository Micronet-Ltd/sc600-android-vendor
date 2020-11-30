/****************************************************************************************
 * The file (ipth_adapt_layer.h) provides the definitions for using the adaptation      *
 * layer.                                                                               *
 *                                                                                      *
 * Copyright (c) 2003-2008 InnoPath Software, Inc. All rights reserved.                 *
 *                                                                                      *
 * Legal Statements                                                                     *
 *                                                                                      *
 * THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS IS" BASIS *
 * WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND, WHETHER EXPRESS,  *
 * IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE, TRADE OR OTHERWISE INCLUDING*
 * ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, MERCHANTABLE QUALITY,       *
 * FITNESS FOR ANY PARTICULAR PURPOSE OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES *
 * NOT REPRESENT OR WARRANT THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS, *
 * UNINTERRUPTED, ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR         *
 * AVAILABLE AT ANY TIME.                                                               *
 *                                                                                      *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE FOR ANY *
 * DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE, PUNITIVE OR OTHER    *
 * DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED TO ANY LOSS OF PROFITS OR *
 * REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST BUSINESS, LOST BUSINESS            *
 * OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM WORK STOPPAGE, COSTS OF OVERHEAD, *
 * COSTS OF COVER, LOSS OF ANTICIPATED BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO *
 * THIS AGREEMENT, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS  *
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF  *
 * THE ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.                           *
 *                                                                                      *
 ***************************************************************************************/

#ifndef IPTH_AL_DEVICEAPI_H
#define IPTH_AL_DEVICEAPI_H

#ifdef _WINDLL
#ifdef AL_DEVICEAPI_EXPORTS
#define AL_DEVICEAPI_API __declspec(dllexport)
#else
#define AL_DEVICEAPI_API __declspec(dllimport)
#endif
#else
#define AL_DEVICEAPI_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ipth_type.h"

    /*
     * Initializes the API interfaces.
     *
     * The device manufacturer MUST call this API BEFORE calling any DUC APIs. This
     * function will check the capabilities of the device, then each DUC module determines
     * which APIs to export according to the device capability.
     *
     * NOTE: A minimum of 10 KB working memory needs to be passed to this function for its
     * temporary use.
     *
     * @param  aHandle  [OUTPUT] Handle provided to the calling API
     *                           The returned handle must kept by the caller and
     *                           passed to the other adaptation layer APIs.
     *
     * @param  ram_buff [INPUT] The pointer to the ram buffer that the caller provides
     *                          for the temporary use
     *
     * @param  ram_size [INPUT] the size of the ram buffer
     *
     * @return Returns IP_OK or error code if error happens.
     *
     * @version 1.0
     */
    AL_DEVICEAPI_API
    ip_int16 IPTH_AL_InitIpthApiInterfaces(void **aHandle,
    ip_byte *ram_buff,
    ip_uint32 ram_size);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_AL_DEVICEAPI_H */

/* End of File */
