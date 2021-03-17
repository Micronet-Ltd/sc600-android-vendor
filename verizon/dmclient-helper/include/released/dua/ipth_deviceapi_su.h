/****************************************************************************************
 * The file (dt_deviceapi_su.h) provides the reference declarations of all the Device   *
 * APIs that are to be provided by the customer in the BOOT mode                        *
 *                                                                                      *
 * This source file is part of the reference code delivered with the DeltaUpgrade Client*
 * SDKDevice CD. The content of this file is ONLY for reference.                        *
 *                                                                                      *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.                 *
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

#ifndef DEVICEAPI_SU_H
#define DEVICEAPI_SU_H

#include "ipth_type.h"

/* Manufacturer's Device API : Get config API in Boot mode */

/*
 * Description This function reads the DUA status area data from flash memory to RAM during DUA
 *             self-upgrades. There are two copies of the DUA status area data stored in flash
 *             memory, primary and backup.
 *             The parameter whichconfig specifies the copy from which to read, and the
 *             parameters target and size specify the RAM address and number of bytes to read,
 *             respectively. The DUA calls this function in boot-IPL mode during DUA upgrade.
 *
 * @param whichCopy [INPUT] Status area copy type::
 *                            0 = primary copy
 *                            1 = backup copy
 * @param target [OUTPUT] Pointer to the starting RAM address
 * @param size [INPUT] Number of bytes to be read from status area data
 *
 * @return Returns IP_OK if Success or Manufacturer error code if Failed.
 *
 * @version 1.0
 */
ip_int16 dt_ReadDUCConfig_SU(ip_int16 whichCopy, void *target, ip_int16 size);

#endif /* DEVICEAPI_SU_H */

/* End of file */

