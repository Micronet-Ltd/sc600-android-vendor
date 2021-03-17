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

#ifndef IPTH_ADAPT_LAYER_H
#define IPTH_ADAPT_LAYER_H

#ifdef _WINDLL
#ifdef ADAPT_LAYER_EXPORTS
#define IPTH_ADAPT_LAYER_API __declspec(dllexport)
#else
#define IPTH_ADAPT_LAYER_API __declspec(dllimport)
#endif
#else
#define IPTH_ADAPT_LAYER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ipth_type.h"

    /* Version matching strategies */
    typedef enum ip_vStrategy
    {
        IP_AL_EXACT,     /* Exact match       */
        IP_AL_ANY,       /* Match any version */
        IP_AL_LOWEST,    /* Lowest available  */
        IP_AL_HIGHEST,   /* Highest available */
        IP_AL_GE,        /* Greater or equal  */
        IP_AL_LE         /* Less or equal     */
    } ip_vStrategy_e;

    /*
     * Add API Signature
     *
     * Device APIs provided by the device manufacturer MUST be registered with the DUC
     * through this API. This function MUST be called for each device API. If a particular
     * device API is not registered, then DUC considers it not available.
     *
     * If a required device API is NOT available, the DUC will not export the DUC API that
     * uses that device API. If an optional device API is NOT available, then DUC will export
     * the DUC API, but will NOT provide the certain functionality that that requires the
     * particular device API.
     *
     * @param  fDecl   [INPUT] Signature of the device API
     *
     * @param  fPtr    [INPUT] Pointer to the device API function
     *
     * @param  version [INPUT] Version number of the device API that the device supports
     *
     * @param  flags   [INPUT] Reserved parameter (pass 0 for version 1.0)
     *
     * @param  aHandle [INPUT] Handle received by calling IPTH_InitApiInterfaces
     *                         The handle must be passed back to the adaptation layer API.
     *
     * @return Returns IP_OK or error code if error happens.
     *
     * @version 1.0
     */
    IPTH_ADAPT_LAYER_API
    ip_int16 IPTH_AL_AddApiSignature(const ip_char *fDecl,
                                     void (*fPtr)(),
                                     ip_uint32 version,
                                     ip_uint32 flags,
                                     void *aHandle);

    /*
     * Querying API Availability
     *
     * Queries the DUC for available APIs. This function MUST be called to get the reference
     * to DUC APIs. The availability of the DUC APIs is determined by the device APIs that
     * were exported by the OEM application (through IDEV_AddOemApiSignatures).
     *
     * @param  fDecl   [INPUT] Signature of the DUC API that needs to check for the version
     * *
     * @param  version [INPUT/OUTPUT] Version number of the DUC API
     *
     * @param  strategy[INPUT] The strategy of matching the DUC API signature
     *
     * @param  aHandle [INPUT] Handle received by calling IPTH_InitApiInterfaces
     *                         The handle must be passed back to the adaptation layer API.
     *
     * @param  match   [OUTPUT] Results of the match operation
     *                          IP_TRUE: found; IP_FALSE: not found
     *
     * @param  fPtr    [OUTPUT] Pointer to the DUC API function if a match is found
     *
     * @return Returns IP_OK or error code if error happens.
     *
     * @version 1.0
     */
    IPTH_ADAPT_LAYER_API
    ip_int16 IPTH_AL_MatchApi(const ip_char *fDecl,
                              ip_uint32 *version,
                              const ip_vStrategy_e strategy,
                              const void *aHandle,
                              ip_bool *match,
                              void **fPtr);

    /*
     * Releases resources
     *
     * The API release the resources allocated using the dt_InitApiInterfaces API call.
     *
     * @param  aHandle [INPUT] Handle received by calling IPTH_InitApiInterfaces
     *                         The handle must be passed back to the adaptation layer API.
     *
     * @return Returns IP_OK or error code if error happens.
     *
     * @version 1.0
     */
    IPTH_ADAPT_LAYER_API
    ip_int16 IPTH_AL_Release(void *aHandle);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_ADAPT_LAYER_H */

/* End of File */
