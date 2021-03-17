/******************************************************************************
 * Plug-in tree access utilities.
 *
 * Copyright (c) 2003-2009 InnoPath Software, Inc. All rights reserved.
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
 * \file ipth_util_plugin.h
 * \brief DM plugin interfaces.
 */

#ifndef __IPTH_UTILPLUGIN_H__
#define __IPTH_UTILPLUGIN_H__

/*=================================================================================================
											  INCLUDE FILES
==================================================================================================*/
#include <ipth_hal_config.h>
#include <dm_plugin/ipth_plugin_callback.h>
#include <dm_plugin/ipth_plugin_instance.h>

#ifdef	__cplusplus
extern "C" {
#endif

    /*==================================================================================================
                                     FUNCTION PROTOTYPES
    ==================================================================================================*/

    int ipth_UtilPlgGetInteriorNode(ipth_MOCallbacks * dme,
    void * handle,
    const char * serverId,
    const char * uri,
    char ** value);


    int ipth_UtilPlgGetCharNode(ipth_MOCallbacks * dme,
                                void * handle,
                                const char * serverId,
                                const char * uri,
                                char ** value);


    int ipth_UtilPlgGetIntNode(ipth_MOCallbacks * dme,
                               void * handle,
                               const char * serverId,
                               const char * uri,
                               int * value);


    int ipth_UtilPlgGetBoolNode(ipth_MOCallbacks * dme,
                                void * handle,
                                const char * serverId,
                                const char * uri,
                                bool * value);


    int ipth_UtilPlgGetChildInteriorNode(ipth_MOCallbacks * dme,
                                         void * handle,
                                         const char * serverId,
                                         const char * parent,
                                         const char * child,
                                         char ** value);


    int ipth_UtilPlgGetChildCharNode(ipth_MOCallbacks * dme,
                                     void * handle,
                                     const char * serverId,
                                     const char * parent,
                                     const char * child,
                                     char ** value);


    int ipth_UtilPlgGetChildIntNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * parent,
                                    const char * child,
                                    int * value);


    int ipth_UtilPlgGetChildBoolNode(ipth_MOCallbacks * dme,
                                     void * handle,
                                     const char * serverId,
                                     const char * parent,
                                     const char * child,
                                     bool * value);



    int ipth_UtilPlgReplaceCharNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * uri,
                                    const char * value);


    int ipth_UtilPlgReplaceIntNode(ipth_MOCallbacks * dme,
                                   void * handle,
                                   const char * serverId,
                                   const char * uri,
                                   int value);


    int ipth_UtilPlgReplaceBoolNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * uri,
                                    bool value);


    int ipth_UtilPlgReplaceChildCharNode(ipth_MOCallbacks * dme,
                                         void * handle,
                                         const char * serverId,
                                         const char * parent,
                                         const char * child,
                                         const char * value);


    int ipth_UtilPlgReplaceChildIntNode(ipth_MOCallbacks * dme,
                                        void * handle,
                                        const char * serverId,
                                        const char * parent,
                                        const char * child,
                                        int value);


    int ipth_UtilPlgReplaceChildBoolNode(ipth_MOCallbacks * dme,
                                         void * handle,
                                         const char * serverId,
                                         const char * parent,
                                         const char * child,
                                         bool value);



    int ipth_UtilPlgAddCharNode(ipth_MOCallbacks * dme,
                                void * handle,
                                const char * serverId,
                                const char * uri,
                                const char * value);


    int ipth_UtilPlgAddIntNode(ipth_MOCallbacks * dme,
                               void * handle,
                               const char * serverId,
                               const char * uri,
                               int value);


    int ipth_UtilPlgAddBoolNode(ipth_MOCallbacks * dme,
                                void * handle,
                                const char * serverId,
                                const char * uri,
                                bool value);


    int ipth_UtilPlgAddChildCharNode(ipth_MOCallbacks * dme,
                                     void * handle,
                                     const char * serverId,
                                     const char * parent,
                                     const char * child,
                                     const char * value);


    int ipth_UtilPlgAddChildIntNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * parent,
                                    const char * child,
                                    int value);


    int ipth_UtilPlgAddChildBoolNode(ipth_MOCallbacks * dme,
                                     void * handle,
                                     const char * serverId,
                                     const char * parent,
                                     const char * child,
                                     bool value);


    int ipth_UtilPlgAddInteriorNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * uri,
                                    bool bIsExec);


    int ipth_UtilPlgAddChildInteriorNode(ipth_MOCallbacks * dme,
                                         void * handle,
                                         const char * serverId,
                                         const char * parent,
                                         const char * child,
                                         bool bIsExec);


    int ipth_UtilPlgDeleteChildNode(ipth_MOCallbacks * dme,
                                    void * handle,
                                    const char * serverId,
                                    const char * parent,
                                    const char * child);


    int ipth_UtilPlgFindNode(ipth_MOCallbacks * dme,
                             void * handle,
                             const char * serverId,
                             const char * profileParent,
                             const char * nameLeaf,
                             const char * valueLeaf,
                             const char ** nameProfile);


    int ipth_UtilPlgIsNodeExist(ipth_MOCallbacks * dme,
                                void * handle,
                                const char * serverId,
                                const char * uri);


    int ipth_UtilPlgIsChildNodeExist(ipth_MOCallbacks * dme,
                                     void * handle,
                                     const char * serverId,
                                     const char * parent,
                                     const char * child);


    ipth_Format ipth_UtilPlgGetFormatForNode(const ipth_MOCallbacks * dme,
            void * handle,
            const char * serverId,
            const char * uri);


    void ipth_PluginUtilLogHALError(ipth_PluginInstance *plgInstance,
                                    const char *pluginName,
                                    const char *errorText,
                                    int errorCode);

#ifdef	__cplusplus
}
#endif


#endif
