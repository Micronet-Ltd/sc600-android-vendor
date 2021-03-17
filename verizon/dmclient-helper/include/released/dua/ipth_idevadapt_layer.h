/****************************************************************************************
 * The file (ipth_idevadapt_layer.h) provides the definitions of funtions implemented by    *
 * OEM for using the adaptation layer.                                                  *
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

#ifndef IDEV_ADAPT_LAYER_H
#define IDEV_ADAPT_LAYER_H

#ifdef _WINDLL
#ifdef OEM_DEVICEAPI_EXPORTS
#define OEM_DEVICEAPI __declspec(dllexport)
#else
#define OEM_DEVICEAPI __declspec(dllimport)
#endif
#else
#define OEM_DEVICEAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ipth_type.h"

    /*
     * Description The OEM team needs to add the signatures of the entire device APIs that available for
     *             DUA to use. If OEM does not provide certain required device APIs then the DUA might
     *             export its APIs, as it cannot work on the crippled environment.
     *             The client initialization APIs will use this API for importing all available device APIs.
     *
     * @param  aHandle [INPUT] The handle to adaptation layer that needs to be passed to the
     *                         IPTH_AL_MatchApi and IPTH_AL_Release.
     *
     * @return Returns IP_OK if Success or Manufacturer error code if Failed .
     *
     * @version 1.0
     */
    OEM_DEVICEAPI
    ip_int16 IDEV_AL_AddOemApiSignatures(void *aHandle);

#ifdef __cplusplus
}
#endif

#endif /* IDEV_ADAPT_LAYER_H */

/* End of File */
