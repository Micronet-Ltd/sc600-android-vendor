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

#ifndef __IPTH_DME_MTREE_APP_H
#define __IPTH_DME_MTREE_APP_H

#include <dm_engine/ipth_dme_types.h>
#include <dm_engine/ipth_mtree_node.h>

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * \file
     *
     * \brief This file includes the Device Management (DM) Tree public access interfaces.
     *
     * \par API Name
     *      ActiveCare Client Foundation: Management Tree
     */

    /**
     * \brief Creates a node with the supplied properties.
     *
     * The name of the node is the last element of the URI.
     * The format property must be specified.
     *
     * Note: OEM is responsible for data validity because the software does not validate the data.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] hDme DME Environment handle
     * \param[in] appId Application identifier
     * \param[in] uri	URI of the new node.
     * \param[in] node	Contents of the new node as defined by node->propMask.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     * \retval IPTH_ERR_NOT_INITIALIZED
     * \retval IPTH_ERR_INVALID_ARGUMENT
    */
    int ipth_mtree_addNode( ipth_DmeHandle hDme, const char *appId, const char *uri, ipth_NodeInfo *node);

    /**
     * \brief Deletes a node or a subtree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] hDme DME environment handle
     * \param[in] appId Application identifier
     * \param[in] uri	URI of the node to be deleted.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     * \retval IPTH_ERR_NOT_INITIALIZED
     * \retval IPTH_ERR_INVALID_ARGUMENT
     */
    int ipth_mtree_deleteNode( ipth_DmeHandle hDme, const char *appId, const char *uri);

    /**
     * \brief Reads values and properties as indicated by node->propMask.
     *
     * For interior nodes, the value is a list of all the node's child names
     * separated by "/" as defined in the OMA standard.
     *
     * Note: OEM is responsible for data validity because the software does not validate the data.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in]  hDme DME environment handle
     * \param[in]  appId Application identifier
     * \param[in]  uri	The URI of the node.
     * \param[out] node	The contents to read as defined by node->propMask.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     * \retval IPTH_ERR_NOT_INITIALIZED
     * \retval IPTH_ERR_INVALID_ARGUMENT
     */
    int ipth_mtree_getNode( ipth_DmeHandle hDme, const char *appId, const char * uri, ipth_NodeInfo * node);

    /**
     * \brief Writes new values and properties as indicated by the node->propMask
     * field to an existing leaf node. Only valid for leaf nodes.
     *
     * Certain fields of properties, including version, time stamp, and size,
     * are filled in automatically. The input values for those fields that are
     * filled in are ignored.
     *
     * Note: OEM is responsible for data validity because the software does not validate the data.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] hDme  DME environment handle
     * \param[in] appId Application identifier
     * \param[in] uri	  The URI of the existing node.
     * \param[in] node	The contents to be written as defined by node->propMask.
     *
     * \retval IPTH_OK
     * \retval IPTH_ERR
     * \retval IPTH_ERR_NOT_INITIALIZED
     * \retval IPTH_ERR_INVALID_ARGUMENT
     */
    int ipth_mtree_replaceNode( ipth_DmeHandle hDme, const char *appId, const char * uri, ipth_NodeInfo * node);

    /**
     * \brief Saves any changes to the tree to persistent storage.
     * Note: The DME may perform syncing independently of applications
     * that call this function.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] hDme DME environment handle
     * \retval IPTH_OK
     * \retval IPTH_ERR
     * \retval IPTH_ERR_NOT_INITIALIZED
     */
    int ipth_mtree_sync( ipth_DmeHandle hDme);

#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_DME_MTREE_APP_H */
