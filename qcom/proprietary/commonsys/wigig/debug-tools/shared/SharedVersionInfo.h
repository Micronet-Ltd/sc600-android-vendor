/*
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _SHARED_VERSION_INFO_H_
#define _SHARED_VERSION_INFO_H_

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// The only place for version definition/editing
#define TOOL_VERSION_MAJOR  1
#define TOOL_VERSION_MINOR  4
#define TOOL_VERSION_MAINT  1
#define TOOL_VERSION_INTERM 0
#define TOOL_DESCRIPTION AUTO_GENERATED_DATE

#define TOOL_VERSION            TOOL_VERSION_MAJOR,TOOL_VERSION_MINOR,TOOL_VERSION_MAINT,TOOL_VERSION_INTERM
#define TOOL_VERSION_STR        STR(TOOL_VERSION_MAJOR.TOOL_VERSION_MINOR.TOOL_VERSION_MAINT.TOOL_VERSION_INTERM)


#endif // _SHARED_VERSION_INFO_H_
