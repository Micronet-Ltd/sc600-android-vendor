/******************************************************************************
 * DM plugin instance defenition.
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
* \file ipth_plugin_instance.h
* \brief DM plug-in instance defenitions.
*/

#ifndef __IPTH_DMPLUGININSTANCE_H__
#define __IPTH_DMPLUGININSTANCE_H__

/*=================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include "dm_engine/ipth_device_support.h"
#include "dm_plugin/ipth_plugin_interface.h"
#include "dm_plugin/ipth_plugin_interface_ex.h"

#include <ipth_hal_dll.h>


#ifdef	__cplusplus
extern "C" {
#endif


    /*==================================================================================================
                                     STRUCTURES AND OTHER TYPEDEFS
    ==================================================================================================*/

#define dim(x) sizeof(x) / sizeof(x[0])


    /**
     * \brief plugin instance structure used by plug-ins.
     *
     */
    typedef struct {
        ipth_MOCallbacks * dme;  /**< DME interface */
        ipth_CallbackLog log; /**< Logger interface */
        ipth_DLACallbacks * dla;  /**< DLA interface */
        ipth_DmEngineDeviceSupport * ui; /**< UI interfaces */
        ipth_PluginInterfaces plugin; /**< plugin interface */
        void * handle;
        char *mountUri; //Mount point for plugins. Root uri of a plugin sub-tree.
        ipth_HALDllHandle oemHandle;
    } ipth_PluginInstance;

    /* Reference to a STR_DATA, not Malloced */
    typedef struct STR_PTR {
        char* ptr;
        int   len;
    } STR_PTR_s;

    typedef struct DYN_URL {
        STR_PTR_s dyn;
        STR_PTR_s tail;
    } DYN_URL_s;

    typedef struct URI {
        STR_PTR_s full;     /* Malloced and Freed */
        STR_PTR_s mount;	/* Reference to the 'full' param */
        DYN_URL_s *dyns;    /* Reference list to the 'full' param, List is Malloced */
        int       dyns_len;
    } URI_s;

    int ipth_str_suffix_cmp ( const STR_PTR_s *s0_p, const STR_PTR_s *s1_p);

    /* management object access table used by plugin */

    typedef enum { GET_ATYPE=0x1, REPLACE_ATYPE=0x2, EXEC_ATYPE=0x4 } Action_Type;

    typedef struct MO_Access_Record_t {
        const char*		uri;
        //const ipth_Format	type;
        unsigned int	action;
    } MO_Access_Record;

    typedef MO_Access_Record*	MO_Access_Table;


    typedef int (*HAL_LOCAL_GET_FN) (const char* mount_uri, const char* rel_uri,
                                     idev_HALGetDeviceMO fn_get, char* data, int* data_size);
    typedef int (*HAL_LOCAL_REPLACE_FN) (const char* mount_uri, const char* rel_uri,
                                         idev_HALReplaceDeviceMO fn_replace, const char *data, const int data_size);
    typedef int (*HAL_LOCAL_EXEC_FN) (const char* mount_uri, const char* rel_uri,
                                      idev_HALExecDeviceMO fn_exec);

    typedef struct MO_Access_Params_t {
        MO_Access_Table*	tbl;
        int					tbl_size;
        bool				has_dyn_node;
        const char**		dyn_node_name_list;
        int					list_size;
        Action_Type			atype;
        const char*			uri;
        const char*			mount_uri;
        const char*			mount_uri_postfix;
        HAL_LOCAL_GET_FN		fn_get;
        HAL_LOCAL_REPLACE_FN	fn_replace;
        HAL_LOCAL_EXEC_FN		fn_exec;
        idev_HALGetDeviceMO		fn_idev_get;
        idev_HALReplaceDeviceMO	fn_idev_replace;
        idev_HALExecDeviceMO	fn_idev_exec;
    } MO_Access_Params;


#ifdef	__cplusplus
}
#endif

#endif
