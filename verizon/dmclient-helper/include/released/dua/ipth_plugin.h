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

#ifndef IPTH_PLUGIN_H
#define IPTH_PLUGIN_H

#ifdef _WINDLL
#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif
#else
#define PLUGIN_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ipth_appdatatype.h"

#define GET_ENCODE_PLUGIN_API_NAME "ipth_getEncodePlugin"
#define GET_ENCODE_SYMB_PLUGIN_API_NAME "ipth_getSymbEncodePlugin"
#define GET_ENCODE_CRAMFS_PLUGIN_API_NAME "ipth_getCramfsEncodePlugin"

    typedef enum ip_plugInId
    {
        IP_PLUGIN_ID_NONE          = 0x0,
        //Innopath provided DLLs start from 1 to 127
        IP_PLUGIN_ID_ZLIB          = 0x1,
        IP_PLUGIN_ID_GZIP          = 0x2,
        IP_PLUGIN_ID_LZMA          = 0x3,
        IP_PLUGIN_ID_SYMB_9_4	   = 0x4,
        IP_PLUGIN_ID_CRAMFS		   = 0x5,

        IP_PLUGIN_ID_INNO_END      = 0x7F,
        //customer provided DLLs start from 128 to 255


        IP_PLUGIN_ID_END           = 0xFF
    } ip_plugInId_e;

    /* return 0 for ok, else error */
    PLUGIN_API
    ip_int32 ipth_getEncodePlugin(ip_encodePlugin_s *plugin);

    PLUGIN_API
    ip_int32 ipth_getSymbEncodePlugin(ip_encodePlugin_s *plugin);

    PLUGIN_API
    ip_int32 ipth_getCramfsEncodePlugin(ip_encodePlugin_s *plugin);

#ifdef __cplusplus
}
#endif

#endif /* IPTH_PLUGIN_H */
