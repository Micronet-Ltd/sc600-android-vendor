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

#ifndef __IPTH_MTREE_NODE_H__
#define __IPTH_MTREE_NODE_H__

#include "ipth_hal_config.h"
#include "dm_engine/ipth_error.h"
#include "dm_engine/ipth_dme_types.h"

/**
 * \file
 *
 * \brief This file includes Device Management (DM) tree node interfaces.
 *
 * \par API Name
 *      ActiveCare Client Foundation: Management Tree
 */

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Device Management (DM) tree node property flag definitions.
     *
     * \par Constant Table
     *      Device Management (DM) tree node property flag definitions
     * End Description
     */
#define IPTH_PROPERTY_FLAG_NONE		0x0000  /**< Tree node has no properties flag */
#define IPTH_PROPERTY_FLAG_NAME		0x0001  /**< Tree node has a name property flag */
#define IPTH_PROPERTY_FLAG_ACL		0x0002  /**< Tree node has an access control list (ACL) property flag */
#define IPTH_PROPERTY_FLAG_FORMAT	0x0004  /**< Tree node has a format property flag */
#define IPTH_PROPERTY_FLAG_VALUE	0x0008  /**< Tree node has a value property flag */
#define IPTH_PROPERTY_FLAG_SIZE		0x0010  /**< Tree node has a size property flag */
#define IPTH_PROPERTY_FLAG_TITLE	0x0020  /**< Tree node has a title property flag */
#define IPTH_PROPERTY_FLAG_TSTAMP	0x0040  /**< Tree node has time stamp property flag */
#define IPTH_PROPERTY_FLAG_TYPE		0x0080  /**< Tree node has a MIME type property flag */
#define IPTH_PROPERTY_FLAG_VERNO	0x0100  /**< Tree node has a version number property flag */
#define IPTH_PROPERTY_FLAG_ALL		0x01ff  /**< Tree node has all properties flag */

    /**
     * \brief Defines tree node properties and values.
     * If this data structure is used as an input argument, subroutines do not
     * inherit the ownership of pointer data.
     *
     * If this data structure is used as an output argument, the caller owns
     * the pointer data.
     *
     * In either case, it is the caller's responsibility to free the node
     * by calling ipth_FreeNodeInfo when the node is no longer needed.
     */
    typedef struct ipth_NodeInfo {
        int            propMask; /**< Mask of property flags for the node */
        char *         acl;      /**< Access control list (ACL) properties */
        ipth_Format    format;   /**< Format */
        char *         name;     /**< Name */
        size_t         size;     /**< Size in bytes of value member */
        char *         title;    /**< Title */
        unsigned long  timeStamp;/**< Time stamp */
        char *         type;     /**< MIME type */
        unsigned short verNo;    /**< Version */
        void *         value;    /**< Value of the node */
    } ipth_NodeInfo;

    /**
     * \brief Initializes an ipth_NodeInfo structure with valid initial values.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] nInfo A pointer to the ipth_NodeInfo structure to be initialized.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     */
    int ipth_InitializeNodeInfo(ipth_NodeInfo *nInfo);

    /**
     * \brief Frees the storage area pointed to by member pointer fields.
     * The software only frees data fields marked by nInfo->propMask.
     * Also, the software does not free the ipth_NodeInfo structure itself. Use this function
     * to free the memory allocated in calls to ipth_mtree_getNode.
     * Do not call this function after ipth_mtree_replaceNode or
     * ipth_mtree_addNode is called because the fields of the node have already been
     * provided by the caller.
     * After the call, the node is re-initialized and ready for new usage.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param [in] nInfo A pointer to the ipth_NodeInfo structure to be released.
     */
    void ipth_FreeNodeInfo(ipth_NodeInfo *nInfo);

#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_MTREE_NODE_H__ */
