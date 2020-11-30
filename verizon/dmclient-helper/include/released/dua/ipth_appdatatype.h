/****************************************************************************************
 * The file (dt_appdatatype.h) defines the application specific data types used by      *
 * InnoPath client software products.                                                   *
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

#ifndef IPTH_APPDATATYPE_H
#define IPTH_APPDATATYPE_H

#include "ipth_type.h"

#include "ipth_signatures.h"

/* Host Info data type */
typedef struct ip_hostInfoType {
    ip_uint8     IpAddr[4];
    ip_uint16    PortNum;
    ip_char      FileLocation[100];
} ip_hostInfoType_s;

/* the Max size of EBSC Version */
/* To change this size limitation, the libraries have to be rebuild
   with the new values and tested with the other components of the
   DU products. This constant is 4-byte aligment */
#ifndef IP_VERSION_LEN
#define IP_VERSION_LEN					(255)
#endif

#define IP_COMP_NAME_SIZE        (32)    /* Size of the EBSC Component Name */
#define MAX_LEN_REL_NOTE         (160)   /* Max length of the release Notes */
#define IP_MAP_VERSION_LEN       (32)    /* Length of the Map Version String */

/* Defined for OMA DM Compliant Client */

#define	MAX_SERVER_IDENTIFIER_LEN	(100)  /* Maximum length of OMA server id */
#define	MAX_VENDOR_SPECIFIC_LEN		(100)  /* Maximum length of vendor specific info */
#define	MAX_SERVER_NAME_LEN			(128)  /* Maximum length of OMA server name */
#define	MAX_OMA_SERVER_NUMBER		(2)   /* Maximum number of OMA servers */

/* No operation */
#define	IP_OPERATION_NONE						(100)
/* Descriptor query operation */
#define	IP_OPERATION_QUERY						(IP_OPERATION_NONE + 1)
/* Package download operation */
#define	IP_OPERATION_DOWNLOAD					(IP_OPERATION_NONE + 2)
/* Firmware update operation */
#define	IP_OPERATION_UPDATE						(IP_OPERATION_NONE + 3)
/* Download status registration operation */
#define	IP_OPERATION_REGISTER_DOWNLOAD_STATUS	(IP_OPERATION_NONE + 4)
/* Update status registration operation   */
#define	IP_OPERATION_REGISTER_UPDATE_STATUS		(IP_OPERATION_NONE + 5)
/* Device binding operation               */
#define	IP_OPERATION_REGISTER_DEVICE			(IP_OPERATION_NONE + 6)

typedef	enum ip_omaProgress {
    IP_OMA_NONE				= 0,	/* None                     */
    IP_OMA_START_QUERY		= 1,	/* Query started            */
    IP_OMA_DOWNLOAD_SUCCESS	= 2		/* OMA DM download success  */
} ip_omaProgress_e;


/* Data Type definition for the EBSC Version */
typedef ip_uint8 ip_Version[IP_VERSION_LEN];

typedef enum ip_updtProgress {
    IP_NO_UPDATE                  = 0x00, /* No diff package for update */
    IP_START_UPDATE               = 0x01, /* Diff package downloaded */
    IP_PREVIOUS_UPDATE_IN_PROGRESS= 0x02, /* Previous update was in progress */
    IP_START_COPY_COMPONENT       = 0x03, /* New component saved in reserved ROM space */
    IP_PREVIOUS_UPDATE_SUCCESSFUL = 0x04, /* Previous update was a success */
    IP_PREVIOUS_UPDATE_FAILED     = 0x05,  /* Previous update failed */
    IP_PREVIOUS_ROLLBACK_SUCCESSFUL = 0x06 /*Previous rollback successful*/
} ip_updtProgress_e;

typedef enum ip_dnldProgress {
    IP_NO_DOWNLOAD                   = 0x00, /* No diff package for download      */
    IP_START_DOWNLOAD                = 0x31, /* Ready to start download           */
    IP_PREVIOUS_DOWNLOAD_IN_PROGRESS = 0x32, /* Previous download was in progress */
    IP_PREVIOUS_DOWNLOAD_SUCCESSFUL  = 0x33, /* Previous download was a success   */
    IP_PREVIOUS_DOWNLOAD_FAILED      = 0x34, /* Previous download failed          */
    IP_DOWNLOAD_OBJECT_CONTINUE      = 0x35, /* More Objects to download and Udpate */
    IP_USER_CANCEL_DOWNLOAD			 = 0x36, /* Download was canceled by user        */
    IP_CLIENT_ERROR					 = 0x37  /* Download was abored for device error */
} ip_dnldProgress_e;

typedef enum ip_dfsUpdtProgress {
    IP_DFS_NO_UPDATE                    = 0x00, /* set if no package is available, but DFS update agent is called */
    IP_DFS_UPDATE_SUCCESSFUL            = 0x01, /* set when update is done */
    IP_DFS_UPDATE_FAILED                = 0x02, /* set when unrecoverable error encounters */
    IP_DFS_START_UPDATE                 = 0x03, /* set before really update DFS (dry run is done if asked) */
    IP_DFS_PREVIOUS_UPDATE_IN_PROGRESS  = 0x04, /* set when status is IP_DFS_START_UPDATE and update agent is called again */
    IP_DFS_START_DRY_RUN                = 0x05, /* set before dry run if do_dry_run == IP_TRUE */
    IP_DFS_UPDATE_TODO                  = 0x06, /* set before renaming critical system files */
    IP_DFS_UPDATE_ROLLBACK              = 0x07, /* set before doing roll back failure recovery */
    IP_DFS_UPDATE_CARRYOVER             = 0x08, /* set before doing carry over failure recovery */
    IP_DFS_INVALID_PARAM                = 0x09, /* invalid input parameter */
    IP_DFS_ROOT_USER_REQUIRED           = 0x0A, /* 'root' user is required to run DFS update */
    IP_DFS_BAD_PACKAGE_ID               = 0x0B, /* package is not DFS package */
    IP_DFS_PACKAGE_CORRUPTED            = 0x0C, /* package data corrupted */
    IP_DFS_PACKAGE_VERSION_MISMATCH     = 0x0D, /* package version mismatch */
    IP_DFS_NOT_ENOUGH_MEMORY            = 0x0E, /* not enough working RAM */
    IP_DFS_INTERNAL_ERROR               = 0x0F, /* internal error */
    IP_DFS_SECURITY_NOT_SUPPORTED       = 0x10, /* encryption/decryption not supported */
    IP_DFS_SECURITY_INVALID_KEY         = 0x11  /* invalid decryption key */
} ip_dfsUpdtProgress_e;

typedef enum ip_dnldMode {
    /* OTA download: notify and download in MMI mode; update in IPL */
    IP_DOWNLOAD_MODE_OTA     = 0x00,
    /* download and update thru cable connecting to DUS in IPL mode */
    IP_DOWNLOAD_MODE_CABLE   = 0x61,
    /* download and update from external memory card in IPL mode */
    IP_DOWNLOAD_MODE_MEMCARD = 0x62,
    /* OTA download OMA compliant: download in MMI; update in IPL */
    IP_DOWNLOAD_MODE_OMA     = 0x63,
    /* for future expansion purpose */
    IP_DOWNLOAD_MODE_EXT1    = 0x64
} ip_dnldMode_e;

typedef struct ip_status {
    ip_uint32    updtProgress;      /* enum ip_updtProgress_e */
    ip_uint16    dnldProgress;      /* enum ip_dnldProgress_e */
    ip_uint16    dnldMode;          /* enum ip_dnldMode_e     */
    ip_int16     errorCode;         /* error code stored in the config.bin */
} ip_status_s;

typedef struct ip_versionInfo {
    ip_Version ebscVersion;                     /* EBSC Version */
    ip_char    mapVersion[IP_MAP_VERSION_LEN];  /* Map Version */
} ip_versionInfo_s;

/* Component Information of EBSC */
typedef struct ip_componentInfo {
    ip_char       Name[IP_COMP_NAME_SIZE];
    ip_Version    FromVersion;
    ip_Version    ToVersion;
} ip_componentInfo_s;

#define IP_NOTIF_CLT_ACTION_SILENT            (IP_FALSE) /* 0 */
#define IP_NOTIF_CLT_ACTION_MMI               (IP_TRUE)  /* 1 */
#define IP_NOTIF_CLT_ACTION_SILENT_QUERY_ONLY (2)        /* 2 */
#define IP_NOTIF_CLT_ACTION_MMI_QUERY_ONLY    (3)        /* 3 */
#define IP_NOTIF_CLT_ACTION_NO_ACTION         (4)        /* 4 */

#define IP_NOTIF_CLT_REL_NOTES_LANG_LEN (2)

/* Notification output from the Process Notification Msg */
typedef struct ip_notificationMsg {
    ip_uint8            ClientAction;
    ip_char             RelNotesLang[IP_NOTIF_CLT_REL_NOTES_LANG_LEN];
    ip_char             RelNotes[MAX_LEN_REL_NOTE];
    ip_int32            NumComponents;
    /* Session identifier */
    ip_uint16			SessionId;
    /* Server identifier */
    ip_char				ServerIdentifier[MAX_SERVER_IDENTIFIER_LEN];
    /* Length of vendor specific information */
    ip_uint16			VendorSpecificLen;
    /* Vendor specific information */
    ip_uint8			VendorSpecific[MAX_VENDOR_SPECIFIC_LEN];
} ip_notificationMsg_s;

/* IMPORTANT: The below two structs are required by customers who donot need the WAMP
   Implementation and does not create config.bin using the ConfigGen. */
typedef enum ip_cfgKeyType {
    IP_CFG_KEY_DEFAULT = 0x01, /* default key type */
    IP_CFG_KEY_MANUFACTURER,   /* OEM key type     */
    IP_CFG_KEY_CARRIER,        /* carrier key type */
    IP_CFG_KEY_USER            /* user key type    */
} ip_cfgKeyType_e;

typedef enum ip_cfgDecryptionType {
    IP_CFG_ENC_TYPE_DES = 0x01, /* DES encryption type         */
    IP_CFG_ENC_TYPE_3DES,       /* 3DES encryption type        */
    IP_CFG_ENC_TYPE_AES_128     /* AES 128 bit encryption type */
} ip_cfgDecryptionType_e;

/* To change this size limitation, the libraries have to be rebuild
   with the new values and tested with the other components of the
   DU products */
#define MAX_VALUE_STR_SIZE				(100)

/* The following define values are to be used in the debug version
   of the libraries.
   NOTE: if needed, Customers can remove the #ifdef DUC_DEBUG */
#define IP_DEBUG_LEVEL_CRITICAL 	    (0)
#define IP_DEBUG_LEVEL_MAJOR		    (1)
#define IP_DEBUG_LEVEL_MINOR	        (2)
#define IP_DEBUG_LEVEL_PERFORMANCE_TEST (5)

/* The structure to hold the device information */
typedef struct ip_deviceInfo {
    ip_char     ManufacturerName[MAX_VALUE_STR_SIZE];/* Manufacturer name */
    ip_char     DeviceModel[MAX_VALUE_STR_SIZE];     /* Device model      */
    ip_char     DeviceIDType[MAX_VALUE_STR_SIZE];    /* Device ID Type    */
    ip_char     DeviceIDValue[MAX_VALUE_STR_SIZE];   /* Device ID Value   */
} ip_deviceInfo_s;

/* The structure to hold the user information */
typedef struct ip_userInfo {
    ip_char     UserIDType[MAX_VALUE_STR_SIZE];     /* User ID Type     */
    ip_char     UserIDValue[MAX_VALUE_STR_SIZE];    /* User ID Value    */
    ip_char     Password[MAX_VALUE_STR_SIZE];       /* Password         */
    ip_char     CountryCode[MAX_VALUE_STR_SIZE];    /* Country code     */
    ip_char     Lang[MAX_VALUE_STR_SIZE];           /* Language         */
} ip_userInfo_s;

/* Enum to determine the mode of operation */
typedef enum ip_operationMode {
    IP_IPL_MODE,    /* The IPL Mode  */
    IP_RTOS_MODE    /* The RTOS Mode */
} ip_operationMode_e;

/* Debug Level list */
typedef enum ip_debugLevelExt {
    IP_DBGLVL_SHOWSTOP,
    IP_DBGLVL_CRITICAL,
    IP_DBGLVL_MAIN,
    IP_DBGLVL_DETAIL
} ip_debugLevelExt_e;

/* DFS Input Parameter Struct */
typedef struct ip_dfsInput {
    ip_uint8 *diffPkgBufPtr;    /* pointer to the diff package buffer */
    ip_uint32 diffPkgBufSize;   /* size of the diff package buffer */
    ip_uint8 *workBufPtr;       /* pointer to the RAM buffer */
    ip_uint32 workBufSize;      /* size of the RAM buffer */
    ip_char  *logPath;          /* path where DFSUA logs located */
    ip_byte  *cryptKeyValue;    /* The decryption key value */
    ip_bool   isPreRTOSMode;    /* flag to indicate if DFS update agent running in pre-RTOS mode */
    ip_uint8  cryptKeyValueLen; /* The decryption key length in bytes < 255 */
    ip_uint8  rbkEnable;
    ip_uint8  numOfUpdateFileOps;
} ip_dfsInput_s;

/* DFS Output Parameter Struct */
typedef struct ip_dfsOut {
    ip_uint32 numOfDFSOpExecuted;   /* number of file system operations executed */
    ip_int16  status;               /* update status code */
    ip_bool   isSystemFileUpdated;  /* flag to indicate if system files have been updated */
} ip_dfsOut_s;

/* Otaff result codes */
typedef enum {
    OTAFF_SUCCESSFUL_UPDATE                = 0x0,
    OTAFF_USER_CANCELLED_DOWNLOAD          = 0x1,
    OTAFF_USER_CANCELLED_UPDATE            = 0x2,
    OTAFF_CORRUPTED_FW_UP                  = 0x3,
    OTAFF_PACKAGE_MISMATCH                 = 0x4,
    OTAFF_FAILED_SIGNATURE_AUTHENTICATION  = 0x5,
    OTAFF_UPDATE_FAILED                    = 0x6,
    OTAFF_MEMORY_ALLOC_FAILED              = 0x7,
    /*Errors Before Update*/
    OTAFF_DIFF_PACKAGE_CHECKSUM_FAILED	   = 0x20,
    OTAFF_DIFF_PACKAGE_CORRUPTED		   = 0X21,
    OTAFF_INVALID_PROTOCOL				   = 0x22,
    OTAFF_SECURITY_TYPE_NOT_SUPPORTED      = 0x23,
    OTAFF_NOT_ENOUGH_MEMORY    		       = 0x24,
    OTAFF_CONFIG_CHECKSUM_MISMATCH         = 0x25,
    OTAFF_DIFF_PACKAGE_SIZE_ZERO           = 0x26,
    OTAFF_WORKING_RAM_ALLOCATION_FAILED    = 0x27,
    OTAFF_NOTIFICATION_MSG_CORRUPTED       = 0x28,
    OTAFF_MAP_VERSION_INCORRECT            = 0x29,
    OTAFF_CONFIG_DATA_VERSION_MISMATCH     = 0x30,
    OTAFF_CONFIG_COMP_NOT_FOUND            = 0x31,
    /*Security Errors*/
    OTAFF_SECURITY_CHECKSUM                =0x60,
    OTAFF_SECURITY_RETRIEVE_KEY            =0x61,
    OTAFF_SECURITY_CALC_DIGEST_FAILED      =0x62,
    OTAFF_SECURITY_MD5_DIGEST              =0x63,
    /* Reserve for
       future use */
    OTAFF_UNDEFINED_ERROR                  = 0xFF
} otaff_ResultCode;

/* Mime types */
typedef enum ip_mimeType {
    IP_MIMETYPE_SYNCML_NOTIFICATION = 0,
    IP_MIMETYPE_SYNCML_XML,
    IP_MIMETYPE_SYNCML_WBXML,
    IP_MIMETYPE_OMA_DDXML,
    IP_MIMETYPE_OMA_PKGXML,
    IP_MIMETYPE_OCTET_STREAM,
    IP_MIMETYPE_TEXT_PLAIN,
    IP_MIMETYPE_UNDEF
} ip_mimeType_e;

/* Package Installer update types */
typedef enum ip_piUpdateType {
    IP_PI_UNKNOWN_TYPE 		= 0,	// unknown type
    IP_PI_FW_UPDATE_TYPE    = 1,	// firmware update
    IP_PI_DFS_UPDATE_TYPE   = 2,	// DFS update
    IP_PI_COMBO_PKG_TYPE	= 3,	// COMBO package
    IP_PI_FW_R_UPDATE_TYPE  = 4,	// firmware update w/ rollback support
    IP_PI_DFS_R_UPDATE_TYPE = 5,	// DFS update w/ rollback support
    IP_PI_COMBO_R_PKG_TYPE	= 6		// COMBO package w/ rollback support

} ip_piUpdateType_e;

/* Package verification level */
typedef enum ip_pkgVerifyLevel {
    IP_PKG_CHECK_SIGNATURE = 0,	// check package signature
    IP_PKG_CHECK_INTEGRITY = 1	// check package integrity
} ip_pkgVerifyLevel_e;

/* Struct for the memory apis */
typedef struct ip_memApis {
    void *(*allocMem)(void *mem_handle, ip_uint32 size); /* Pointer to the malloc API */
    void (*freeMem)(void *mem_handle, void * ptr);       /* Pointer to the free API */
} ip_memApis_s;

/* Struct for the plugin */
typedef struct ip_encodePlugin {
    char *(*get_name)(void);
    char *(*get_version)(void);
    int  (*init)(void);
    void (*set_mem_apis)(void *mem_handle, ip_memApis_s *mem_apis);
    int  (*encode)(char *out_buff, int *out_len, const char *in_buff, int *in_len, int id);
    int  (*decode)(char *out_buff, int *out_len, const char *in_buff, int *in_len, int id);
    int  (*close)(void);
    char *(*get_error)(void);
} ip_encodePlugin_s;

/* SSL/TLS cipher suites supported */
#define IP_SSL_RSA_RC4_128_SHA                    0x0000
#define IP_SSL_RSA_3DES_EDE_CBC_SHA               0x0001
#define IP_TLS_RSA_AES_128_CBC_SHA1               0x0002
#define IP_TLS_RSA_3DES_EDE_CBC_SHA               0x0003
#define IP_TLS_RSA_RC4_128_CBC_SHA                0x0004

/* Protocol ID */
#define IP_NON_SECURE           0x00
#define IP_TLS_SSL              0x01
#define IP_SSL                  0x02
#define IP_TLS                  0x03

/*Structure of ILAWMO*/
typedef struct ip_ilawmo {
    ip_char*	target;
    ip_char*	type;
    ip_char*	msg;
    ip_int32	pin;
    ip_int32	retry;
    ip_bool		allowcall;
} ip_ilawmo_s;


/* ip_devInfo_s is the sturcture used for the device API: idev_getDevInfo */
/* it contains the mapping of the blocks in the ROMs */
/* block range info */
typedef struct {
    ip_uint32 startAddr;    /* start address of this block range with the same block size */
    ip_uint32 numBlocks;    /* num of blocks in this range */
    ip_uint32 blockSize;    /* the block size */
} ip_blockRangeInfo_s;

/*rom info. one rom may have mutiple block ranges */
typedef struct {
    ip_uint32     romNum;                   /* rom number */
    ip_uint32     startAddr;                /* start address of the rom */
    ip_uint32     size;                     /* the size of the rom */
    ip_uint32     numBlocks;                /* total number of blocks in the rom */
    ip_uint32     numBlockRange;            /* number of block ranges */
    ip_blockRangeInfo_s *blockRangeInfo;    /* block range pointer */
} ip_romBlockInfo_s;

/* device info struct */
typedef struct {
    ip_uint32 numROMs;              /* number of roms */
    ip_romBlockInfo_s *romInfo;     /* pointer to romBlockInfo */
} ip_devInfo_s;

/* Block information */
typedef struct ip_blkInfo {
    ip_uint16 rom_no;    /* The ROM number   */
    ip_uint16 blk_no;    /* The Block number */
    ip_uint32 blk_size;  /* The Block Size   */
} ip_blkInfo_s;

/* A basic structure to hold the information describing a buffer */
typedef struct ip_dataBuff {
    ip_byte     *ptr; /* Buffer of bytes */
    ip_uint32   size; /* Size of buffer */
} ip_dataBuff_s;

#endif /* IPTH_APPDATATYPE_H */

/* End of file */
