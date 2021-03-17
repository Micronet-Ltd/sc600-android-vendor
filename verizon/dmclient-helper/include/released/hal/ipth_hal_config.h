#ifndef __IPTH_HAL_CONFIG_H__
#define __IPTH_HAL_CONFIG_H__

// Run-time library support
// ------------------------
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <wchar.h>

#include "ipth_hal_typedef.h"
#include "ipth_hal_stdlib.h"

typedef wchar_t WCHAR;

#define __int8 char
#define __int16 short
#define __int32 long

// TODO - check if there is something like a max path for Unix
#define IPTH_HAL_FILE_PATH_MAX	512  // max length of a file path

#define TASK_FILE_PATH "law_task.txt" //Lock&Wipe task file. 

#include <assert.h>

#ifndef ipth_Assert
#define	ipth_Assert(expr)   assert(expr)
#endif

#ifndef ipth_Verify
#define ipth_Verify(expr, test) do { if(!(expr test)) assert(0); } while(0)
#endif
/////////////////////////////////////////////////////////////
// Thread support
#include <pthread.h>
#include <semaphore.h>

#define IPTH_HAL_INVALID_THREAD_ID NULL

#ifdef D_LINUX
typedef pthread_t* ipth_HALThreadId;
#else
typedef pthread_t ipth_HALThreadId;
#endif
typedef pthread_mutex_t ipth_HALMutex;
typedef pthread_cond_t ipth_HALCondVar;
typedef sem_t ipth_HALSemaphore;

////////////////////////////////////////////////////////////////
struct ipth_HALFileSearchID_t {
    //TCHAR searchStr[MAX_PATH];
//   WIN32_FIND_DATA fileData;
//   HANDLE resultList;
    bool finished;
};

typedef struct ipth_HALFileSearchID_t ipth_HALFileSearchID;

////////////////////////////////////////////////////////////////
#define IPTH_HAL_INVALID_DLL_HANDLE NULL
typedef void* ipth_HALDllHandle;

//#define IP_DLL_EXPORT _declspec(dllexport)
#define IP_DLL_EXPORT

#define NODE_MAX_LENGTH 256
#define URI_MAX_PATH	512

// Logging
#define IPTH_HAL_LOG(logLevel, format, args) \
	{ \
        printf(format, args); \
		/*__debugbreak();*/ \
    }

// For debugging
#if _DEBUG
#undef IPTH_TRACE
#define IPTH_TRACE printf
//#define IPTH_BREAK() __debugbreak();
#define IPTH_BREAK()
#define IPTH_RETURN_ERROR(x) \
	if (x) { \
		IPTH_TRACE("Error in file %s, line %d\n",__FILE__, __LINE__); \
		IPTH_BREAK(); \
		return IPTH_HAL_ERR; \
	}
#define IPTH_RETURN_SYS_ERROR(x,s) \
	if (x) { \
        LPVOID msgBuf = NULL;       \
        DWORD dw = GetLastError();  \
                                    \
        if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, 0, (LPTSTR) &msgBuf, 0, NULL )) { \
    		IPTH_TRACE("FormatMessage failed with system error code=%d\n", GetLastError()); \
        } IPTH_TRACE("Error in file %s, line %d, %s: system error code=%d, system message=%s\n",__FILE__, __LINE__, s, dw, (LPTSTR) msgBuf); \
		IPTH_BREAK(); \
		return IPTH_HAL_ERR; \
    }
#define IPTH_GOTO_ERROR(x, errorTag) \
	if (x) { \
		IPTH_TRACE("Error in file %s, line %d\n",__FILE__, __LINE__); \
		IPTH_BREAK(); \
		goto errorTag; \
	}
#define IASSERT(x) if (!(x)) __debugbreak();
#else
#define IPTH_TRACE
#define IPTH_RETURN_ERROR(x) \
	if (x) { \
		return IPTH_HAL_ERR; \
	}
#define IPTH_RETURN_SYS_ERROR(x,s) \
	if (x) { \
		printf("Error in file %s, line %d, %s: system error code=%d\n",__FILE__, __LINE__, s, GetLastError()); \
		return IPTH_HAL_ERR; \
	}
#define IPTH_GOTO_ERROR(x, errorTag)  \
	if (x) { \
		goto errorTag; \
    }
#define IASSERT(x)
#define IPTH_BREAK()
#endif


#endif //__IPTH_HAL_CONFIG_H__

