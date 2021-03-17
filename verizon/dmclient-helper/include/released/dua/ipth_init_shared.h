/****************************************************************************************
 * The file (ipth_init_shared.h) defines an API to expose shared library module         *
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

#ifndef IPTH_INIT_SHARED_H
#define IPTH_INIT_SHARED_H

#include "ipth_type.h"
#include "ipth_appdatatype.h"

#ifdef _WINDLL
#ifdef SHARED_EXPORTS
#define SHARED_API __declspec(dllexport)
#else
#define SHARED_API __declspec(dllimport)
#endif
#else
#define SHARED_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Description: This API is called only during adaptation layer initialization. Upon being called, the API
     * configures itself for proper operation, and exports all suitable shared APIs to the device
     * application (MMI).
     *
     * @param aHandle [INPUT] Pointer for the adaptation layer state
     *
     * @return Returns IP_OK if Success or IP_ERR if error happens.
     *
     * @version 1.0
     */
    SHARED_API
    ip_int16 IPTH_InitShared(void *aHandle);

    /*
     * Description: This API is called to verify the package signature or integrity.
     *
     * @param level   [INPUT] level of verification (check signature or integrity)
     * @param pkgType [INPUT] package type (FOTA, DFS or COMBO)
     *
     * @return Returns IP_OK if Success or else if error happens.
     *
     * Remark: For integrity check, it only verifies the FOTA package currently.
     *
     * @version 1.0
     */
    SHARED_API
    ip_int16 ipth_dua_verifyPkg(ip_pkgVerifyLevel_e level, ip_piUpdateType_e pkgType);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_INIT_SHARED_H */

/* End of file */

