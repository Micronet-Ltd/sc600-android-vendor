/****************************************************************************************
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

#ifndef IPTH_INIT_DUA_H
#define IPTH_INIT_DUA_H

#include "ipth_type.h"
#include "ipth_appdatatype.h"

#ifdef _WINDLL
#ifdef IPTH_DUA_EXPORTS
#define IPTH_DUA_API __declspec(dllexport)
#else
#define IPTH_DUA_API __declspec(dllimport)
#endif
#else
#define IPTH_DUA_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /* The structure to hold input settings for the DUA operations */
    typedef struct ip_duaInput {
        ip_bool is_dry_run;         /* If this flag is set to IP_TRUE, the DUA will perform
                                   a dry-run update process: blocks will NOT be
                                   physically rewritten, the package WILL be
                                   processed, and the update logic will be checked if a
                                   real run will succeed. */
        ip_bool check_rom_before;   /* If this flag is set to IP_TRUE, the DUA will perform
                                   a pre-check of the ROM BEFORE starting the
                                   update process. */
        ip_bool check_rom_after;    /* If this flag is set to IP_TRUE, the DUA will perform
                                   a post-check of the ROM AFTER completing the
                                   update process. */
        ip_uint8 prog_step_size;    /* The steps in which the progress bar needs to be
								   shown. This value MUST be > 0 and < 100. Example for
								   prog_step_size of 5, the progress bar is shown in
								   5%, 10%, 15% ... */
        ip_bool disable_recovery;   /* If this flag is set to IP_TRUE, the DUA will disable the
                                   error recovery logic */
        ip_uint8 init_prog_rate;    /* The initial rate of  the progress bar when the update client
                                   starts. The value should be between 0 and 100. The default
                                   is 0 */
        ip_bool is_rollback;		/* If this flag is set to IP_TRUE, the DUA will perform rollback */
        ip_bool disable_rollback;	/*if this flag is set to IP_TRUE, the DUA will disable ROLLBACK logic*/
        ip_bool is_prog_rollback;
    } ip_duaInput_s;

    /* The structure to hold output settings for the DUA operations */
    typedef struct ip_duaOut {
        ip_int16        error_code;            /* Error code value, if any.
                                             If there is none, then the value is zero. */
        ip_uint32       n_blks_to_update;      /* Number of blocks to be updated. */
        ip_uint32       n_blks_updated;        /* Number of blocks that were updated. */
        ip_uint32       n_blks_read;           /* Total number of blocks that were read during
                                              the update process. */
        ip_uint32       n_blks_read_pre;       /* Number of blocks read to perform the pre-check
                                              of the ROM. */
        ip_uint32       n_blks_read_post;      /* Number of blocks read to perform the post-check
                                              of the ROM. */
        ip_bool         is_rom_changed;        /* This flag is IP_TRUE if any of the blocks has
                                              been changed during this update process. */
        ip_updtProgress_e updt_progress;       /* Progress state of the ROM update process. */
        ip_uint32       n_blk_write_api_calls; /* Number of block write calls executed. */
        ip_uint32       n_blk_erase_api_calls; /* Number of block erase calls executed. */
        ip_uint32       n_blk_read_api_calls;  /* Number of block read calls executed.  */
        ip_uint32       n_su_blks_updated;     /* Number of su blocks that were updated. */
        ip_byte         new_img_ver[IP_VERSION_LEN+1]; /* New image version */
        ip_uint32       max_mem_used;          /* max amount of memory used during update */
    } ip_duaOut_s;

    /*
     * Description This API is called only during adaptation layer initialization. Upon being called, the API
     * configures itself for proper operation, and exports all suitable DUA APIs to the device
     * application (MMI).
     *
     * @param aHandle [INPUT] Pointer for the adaptation layer state
     *
     * @return Returns IP_OK if Success or IP_ERR if error happens.
     *
     * @version 1.0
     */
    IPTH_DUA_API
    ip_int16 IPTH_InitDUA(void *aHandle);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_INIT_DUA_H */

/* End of file */

