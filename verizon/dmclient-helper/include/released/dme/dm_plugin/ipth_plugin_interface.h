/******************************************************************************
 * Plug-in interface.
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
* \file ipth_plugin_interface.h
* \brief DM plugin interfaces.
*/

#ifndef __IPTH_DMPLUGININTERFACE_H__
#define __IPTH_DMPLUGININTERFACE_H__

/*=================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <ipth_hal_config.h>
#include <ipth_config.h>
#include <dm_plugin/ipth_plugin_callback.h>


#ifdef	__cplusplus
extern "C" {
#endif


    /*==================================================================================================
                                     STRUCTURES AND OTHER TYPEDEFS
    ==================================================================================================*/
#ifdef IPTH_CONFIG_VERSION_PLUGIN
#define IPTH_PLUGIN_API_VERSION IPTH_CONFIG_VERSION_PLUGIN
#else
#define IPTH_PLUGIN_API_VERSION (1)
#endif

    /********************************************************
     * Plugin related return codes
     *
     * Pugin return code               GET ADD REPLACE DELETE
     * ======================================================
     * IPTH_PLUGIN_OK	                X 	X	  X		X
     * IPTH_PLUGIN_ERR	                X	X	  X		X
     * IPTH_PLUGIN_MEM_ALLOC_FAILED	    X	X	  X		X
     * IPTH_PLUGIN_NODE_NOT_MANAGED	    X	X	  X		X
     * IPTH_PLUGIN_NODE_ALREADY_EXIST		X
     * IPTH_PLUGIN_NODE_NOT_FOUND	    X		  X		X
     * IPTH_PLUGIN_INVALID_VALUE		    X	  X
     * IPTH_PLUGIN_DEVICE_FULL              X
     *********************************************************/
    typedef enum
    {
        IPTH_PLUGIN_NODE_NOT_MANAGED = -1,		/* Plugin returns this if node is not managed by plugin. Default action will then be taken by DME */
        IPTH_PLUGIN_OK = IPTH_OK,				/* Operation completed successfully */
        IPTH_PLUGIN_ERR = IPTH_ERR,				/* Some unspecified error has occured in the plugin */
        IPTH_PLUGIN_MEM_ALLOC_FAILED = IPTH_ERR_MEM_ALLOC,			/* Device has run out of memory */
        IPTH_PLUGIN_NODE_ALREADY_EXIST = IPTH_ERR_ALREADY_EXIST,	/* Returned if node being added already exist */
        IPTH_PLUGIN_NODE_NOT_FOUND = IPTH_ERR_NOT_EXIST,			/* Plugin could not find a node at the given uri */
        IPTH_PLUGIN_INVALID_VALUE = IPTH_ERR_INVALID_ARGUMENT,      /* Plugin might validate actual value for add and replace */
        IPTH_PLUGIN_DEVICE_FULL = IPTH_OMA_STATUS_DEVICE_FULL		/* Device has run out of storage space */
    } ipth_PluginReturnCode;

    /**
     * \brief Define a plug-in states/events.
     */

    typedef enum {
        IPTH_PLUGIN_EVENT_START,
        IPTH_PLUGIN_EVENT_CONTINUE,
        IPTH_PLUGIN_EVENT_RECOVER,
        IPTH_PLUGIN_EVENT_CANCEL,
        IPTH_PLUGIN_EVENT_PAUSE,
        IPTH_PLUGIN_EVENT_RESUME,
        IPTH_PLUGIN_EVENT_SHUTDOWN
    } ipth_PluginEvent;




    /**
     * \brief DM engine uses this function to fetch node from plug-in.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[out] node  Retrieved node information.
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef ipth_PluginReturnCode (*ipth_PluginGetNode)(const char * serverId,
            const char * uri,
            ipth_NodeInfo * node,
            void * blob);


    /**
     * \brief DM engine uses this function to Add node into DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[out] node  Node information.
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef ipth_PluginReturnCode (*ipth_PluginAddNode)(const char * serverId,
            const char * uri,
            ipth_NodeInfo * node,
            void * blob);



    /**
     * \brief DM engine uses this function to Replace node in the DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[in] node  Node information.
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef ipth_PluginReturnCode (*ipth_PluginReplaceNode)(const char * serverId,
            const char * uri,
            ipth_NodeInfo * node,
            void * blob);



    /**
     * \brief DM engine uses this function to Delete node or sub-tree from DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef ipth_PluginReturnCode (*ipth_PluginDeleteNode)(const char * serverId,
            const char * uri,
            void * blob);



    /**
     * \brief DM engine uses this function to Delete node or sub-tree from DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] serverId  Server Id  or an application Id, may be NULL.
     * \param[in] uri  Node path.
     * \param[in] arg  Arguments for exec command.
     * \param[in] correlator  Correlator provided by server.
     * \param[in] blob Plug-in private data.
     *
     * \return 200 if synchronious job completed , alert supplied via callback,
     * \return 202 if asynchronious job is cheduled or started
     * \return status code in case of failure
     */

    typedef  int (*ipth_PluginExecNode)(const char * serverId,
                                        const char * uri,
                                        const char *arg,
                                        const char *correlator,
                                        void * blob);


    /**
     * \brief DM engine uses this function to serialize plug-in DM tree.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_PluginSerialize)(void * blob);





    /**
     * \brief DM engine use this function to notify plug-in about an rollback.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_PluginRollback)(void * blob);




    /**
     * \brief DME uses this function to start recovery on power-up or to start/continue pending
     *          asynchronous job. This function is used to drive plug-in execution in single threaded
     *          environment where plug-in is implemented as state machine.
     *
     * \par Sync (or) Async
     * This is synchronous API
     *
     * \param[in] event Event to pass into async job.
     * \param[in] blob Plug-in private data.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_PluginRun)(ipth_PluginEvent event,
                                  void * blob);





    /**
     * \brief MO interface to DME.
     *
     */
    typedef struct {
        ipth_PluginAddNode Add; /**< Callback to add node */
        ipth_PluginDeleteNode Delete; /**< Callback to delete node */
        ipth_PluginGetNode Get; /**< Callback to get node */
        ipth_PluginReplaceNode Replace; /**< Callback to replace node */
        ipth_PluginExecNode Exec; /**< Pointer on function to execute a plug-in node.*/
        ipth_PluginSerialize Serialize; /**< Callback to serialize plug-in tree */
        ipth_PluginRollback Rollback; /**< Pointer on function to notify plug-in about need to rollback changes. */
        ipth_PluginRun Run; /**< Pointer on function to notify plug-in to start or continue asynchronous job. */
        ipth_ReleaseNodeInfo ReleaseNode; /**< Pointer on function to release node struct */
        ipth_ExtendedInterface ExInterface; /**< Callback to get extended interface */
        void * blob;
    } ipth_PluginInterfaces;





    /**
     * \brief DME uses this function to retrive supported plug-in API version.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     *
     * \return numeric API version (currently 1)
     */
    typedef int (*ipth_FuncGetVersion)();




    /**
     * \brief DME uses this function to retrive registration information.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     *
     * \param[out] type plug-in type.
     * \param[out] uri  mounting point.
     * \param[out] tree Name of a tree file associated with MO (can be null).
     * \param[out] loadType specifies if plug-in should be loaded on power-up.
     *
     * \return IPTH_OK if success, other status if otherwise
     */
    typedef int (*ipth_FuncRegister)(ipth_PluginType * type,
                                     IPTH_PLUGIN_PATH uri,
                                     IPTH_PLUGIN_PATH tree,
                                     bool * loadType);



    /**
     * DME uses this function to un-register the plugin.
     * This function is invoked when the plugin is  un-installed.
     * Here the inter-mediate files created by the plugins can be deleted and
     * all further clean up activities can be performed.
     *
     */
    typedef void (*ipth_FuncUnRegister)();



    /**
     * \brief Initialize global shared library resources and request plug-in interface from DLL.
     *
     * \par Sync (or) Async
     * This is a synchronous API
     * \param[in] mountUri: Mounting point of the  plug-in.This is the root URI of the subtree
     *to which the plugin is registered for.
     * \param[in] callbacks: DM engine callbacks to use by plug-in.
     * \param[in] handle :to be used in DM engine callbacks.
     *
     * \return pointer on interface if success, otherwise NULL
     */
    typedef void * (*ipth_FuncInit)(const char *mountUri,
                                    const ipth_MOCallbacks * callbacks,
                                    void * handle);




    /**
     * \brief Notify shared library to release all resources associated with plug-in.  Shared library is about to be closed.
     *
     * \par Sync (or) Async
     * \param[in]  PlugInInterface : data that represents plugin environment
     * This is a synchronous API
     *
     */
    typedef void (*ipth_FuncFinalize)(void *PlugInInterface);




#ifdef	__cplusplus
}
#endif
#endif /* __IPTH_DMPLUGININTERFACE_H__ */

