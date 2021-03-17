/******************************************************************************
 * DUA OTAFF interfaces.
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_DUA_OTAFF_H__
#define __IPTH_DUA_OTAFF_H__

/**
 * \file
 *
 * \brief Programming interface for Delta Upgrade Agent.
 */

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * The state DUM goes through a cycle as defined below.
     */
#define	IPTH_DUM_READY			0
#define	IPTH_DUM_NEED_REBOOT		1
#define	IPTH_DUM_IN_PROGRESS		2
#define IPTH_DUM_DONE			3

    typedef struct ipth_DumInterface ipth_DumInterface;

    /**
     * Initialize the DUM for installing the update package.
     *
     * \param[in] dum Pointer back to DUM itself.
     *
     * \return	IPTH error code.
     */
    int ipth_DumInitialize(ipth_DumInterface *dum);

    /**
     * Finalize the DUM.
     *
     * \param[in] dum Pointer back to DUM itself.
     */
    typedef int (*ipth_DumFinalize)(ipth_DumInterface *dum);

    /**
     * Perform the update or setup for update. FOTA client still owns the file
     * and is responsible to deleting it later after resetting DUM.
     *
     * \param[in] dum Pointer back to DUM itself.
     * \param[in] pkg The path to the update package file.
     *
     * \return IPTH error code.
     */
    typedef int (*ipth_DumUpdate)(ipth_DumInterface *dum, const char *pkg);

    /**
     * Get the update execution progress. Only meaningful if a preceeding
     * DumUpdate() call was successful.
     *
     * \param[in] dum Pointer back to DUM itself.
     *
     * \return One of the DUM states defined (READY, NEED_REBOOT, IN_PROGRESS, DONE)
     */
    typedef int (*ipth_DumGetState)(ipth_DumInterface *dum);

    /**
     * If update is done, get the update result. Only meaningful if DUM is in
     * DONE state.
     *
     * \param[in] dum Pointer back to DUM itself.
     *
     * \return Proper result code defined in ipth_fumo.h
     */
    typedef int (*ipth_DumGetResult)(ipth_DumInterface *dum);

    /**
     * Get the new version. Caller owns the malloc'ed string afterwards. Caller
     * must free the string later.
     *
     * \param[in] dum Pointer back to DUM itself.
     * \param[out] *version The new version string.
     */
    typedef int (*ipth_DumGetVersion)(ipth_DumInterface *dum, char **version);

    /**
     * Reset DUM to prepare for next update. It will clear intermediate states
     * and reset state back to READY.
     *
     * Caller can delete the pkgFile only after this call.
     *
     * \param[in] dum Pointer back to DUM itself.
     */
    typedef int (*ipth_DumReset)(ipth_DumInterface *dum);


    /**
     * The structure of DUM interface.
     */
    struct ipth_DumInterface {
        ipth_DumFinalize finalize;
        ipth_DumUpdate update;
        ipth_DumGetState getState;
        ipth_DumGetResult getResult;
        ipth_DumGetVersion getVersion;
        ipth_DumReset reset;
        int state;
    };

    int dum_getresult_dfs(char *resultString);
    int dum_getversion_dfs(char **version);

#ifdef	__cplusplus
}
#endif

#endif  /* __IPTH_DUA_OTAFF_H__ */
