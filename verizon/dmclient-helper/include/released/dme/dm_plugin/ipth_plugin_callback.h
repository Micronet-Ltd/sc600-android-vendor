/******************************************************************************
 * DM engine calbacks.
 *
 * Copyright (c) 2003-2008 InnoPath Software, Inc. All rights reserved.
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

/**
* \file ipth_plugin_callback.h
* \brief DM engine callbacks.
*/

#ifndef __IPTH_DMPLUGINCALLBACK_H__
#define __IPTH_DMPLUGINCALLBACK_H__

/*=================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include "dm_plugin/ipth_plugin_node.h"
#include "dm_plugin/ipth_plugin_alert.h"


#ifdef	__cplusplus
extern "C" {
#endif


    /*==================================================================================================
                                     STRUCTURES AND OTHER TYPEDEFS
    ==================================================================================================*/


    /**
     * \brief Define a plug-in string.
     */
#define IPTH_PLUGIN_MAX_PATH_SIZE 256
    typedef char IPTH_PLUGIN_PATH[IPTH_PLUGIN_MAX_PATH_SIZE];

    /**
     * \brief Define a plug-in type.
     */

    typedef enum {
        IPTH_PLUGIN_DATA	/**< Data management (MO) plug-in	*/
    } ipth_PluginType;



    /**
     * \brief Define a extended interface Id.
     */

    typedef enum {
        IPTH_INTERFACE_LOGGER,
        IPTH_INTERFACE_DL,
        IPTH_INTERFACE_DEVSUPPORT,
        IPTH_INTERFACE_BINDER,
        IPTH_INTERFACE_GETDATAPATH,
        IPTH_INTERFACE_WFC,
        IPTH_INTERFACE_LOGUPLOAD,
        IPTH_INTERFACE_GETOEMLIBHANDLE,
        IPTH_INTERFACE_GETPLUGINEXTRA
    } ipth_ExtendedInterfaceId;



    /**
     * \brief Plug-in uses this callback function to report job status back to DME
     *          (DM server) or notify DME that asynchronous job is completed.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] alert Generic Alert.
     * \param[in] isDone Flag to indicate that plug-in finished all jobs and ready to be unloaded.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackReport)(void * handle,
                                       const char * serverId,
                                       ipth_GenericAlert * alert,
                                       int isDone);




    /**
     * \brief Plug-in uses this callback function to fetch node from DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[out] node  Retrieved node information.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackGetNode)(void * handle,
                                        const char * serverId,
                                        const char * uri,
                                        ipth_NodeInfo * node);


    /**
     * \brief Plug-in uses this callback function to Add node into DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[out] node  Node information.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackAddNode)(void * handle,
                                        const char * serverId,
                                        const char * uri,
                                        ipth_NodeInfo * node);



    /**
     * \brief Plug-in uses this callback function to Replace node in the DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[in] node  Node information.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackReplaceNode)(void * handle,
                                            const char * serverId,
                                            const char * uri,
                                            ipth_NodeInfo * node);



    /**
     * \brief Plug-in uses this callback function to Delete node or sub-tree from DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackDeleteNode)(void * handle,
                                           const char * serverId,
                                           const char * uri);




    /**
     * \brief Plug-in uses this callback function to serialize plug-in DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_CallbackSerialize)(void * handle);




    /**
     * \brief Plug-in uses this callback to request extended interface from DM engine (DL agent, for example).
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] handle Plug-in handle (provided by framework).
     * \param[in] id Extended interface Id.
     *
     * \return pointer on extended interface if success, otherwise NULL
     */
    typedef void * (*ipth_ExtendedInterface)(void * handle,
            ipth_ExtendedInterfaceId id);



    /**
     * \brief Plug-in uses this callback to release memory allocated within ipth_CallbackGetNode.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] node node information to be freed.
     *
     * \return pointer on extended interface if success, otherwise NULL
     */
    typedef void (*ipth_ReleaseNodeInfo)(ipth_NodeInfo *node);



    /**
     * \brief DME interface.
     *
     */
    typedef struct {
        ipth_CallbackAddNode Add; /**< Callback to add node */
        ipth_CallbackDeleteNode Delete; /**< Callback to delete node */
        ipth_CallbackGetNode Get; /**< Callback to get node */
        ipth_CallbackReplaceNode Replace; /**< Callback to replace node */
        ipth_CallbackSerialize Serialize; /**< Callback to serialize plug-in tree */
        ipth_CallbackReport Report; /**< Callback to add node */
        ipth_ReleaseNodeInfo ReleaseNode; /**< Callback to free node struct */
        ipth_ExtendedInterface ExInterface; /**< Callback to get extended interface */
    } ipth_MOCallbacks;



#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_DMPLUGINCALLBACK_H__ */
