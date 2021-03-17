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

#ifndef __IPTH_OTAFF_H__
#define __IPTH_OTAFF_H__

/**
 * \file
 *
 * \brief This file includes the interfaces for access to Delta Update Agent.

 * The OTA Flash Forum (OTAFF) is a group of leading companies within the
 * wireless industry that are involved in the development and deployment
 * of over-the-air (OTA) firmware update solutions for mobile phones.
 *
 * \par API Name
 *      Device Manufacturer: OTAFF
 */

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Determines the maximum size of an update package that can be stored.
     *
     * \param[in] name Update package name.
     *
     * \return Available maximum storage space in bytes.
     */
    unsigned long otaff_get_available_space( const char* name);

    /**
     * Opens persistent storage for access to the update package.
     *
     * \param[in] name      Update package name.
     * \param[in] length    Total length of the update package in bytes.
     *
     * \return Non-zero if succeeded, otherwise failed.
     */
    int otaff_open_update_package( const char* name, unsigned long length);

    /**
     * Closes access to the update package in persistent storage.
     *
     * \param[in] name Update package name.
     *
     */
    void otaff_close_update_package( const char* name);

    /**
     * Stores all or part of the update package data into persistent storage.
     * If the update package is large, this function may be called many times with part of the
     * update package.
     *
     * \param[in] name       Update package name.
     * \param[in] deltaPtr   Pointer to a buffer containing the update package data.
     * \param[in] offset     Offset in bytes from the beginning of the update package.
     * \param[in] diff_size  Number of bytes of this part of the update package.
     *
     * \return Non-zero if succeeded, otherwise failed.
     */
    int otaff_put_update_package_data( const char* name, void* deltaPtr, unsigned long offset, unsigned long diff_size);

#ifdef	__cplusplus
}
#endif

#endif  /* __IPTH_OTAFF_H__ */
