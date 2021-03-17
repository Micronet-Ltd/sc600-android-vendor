/****************************************************************************************
 * Copyright (c) 2003-2009 InnoPath Software, Inc. All rights reserved.                 *
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

#ifndef	__IPTH_STDLIB_H__
#define	__IPTH_STDLIB_H__

/** \file ipth_stdlib.h
 *
 * \Innopath defined standard library function prototypes.
 *
 */

/*======================================================================================
									INCLUDE FILES
========================================================================================*/
#include "ipth_stdlib_config.h"


/**
*
*Definitions If not using Standard C   libraries , use innopath defined libraries.
*
*/
#ifdef NOT_USE_STDC

#ifndef ipth_strlen
#define ipth_strlen		ipth_dme_strlen
#endif
#ifndef ipth_strcpy
#define ipth_strcpy		ipth_dme_strcpy
#endif
#ifndef ipth_strncpy
#define ipth_strncpy	ipth_dme_strncpy
#endif
#ifndef ipth_strdup
#define ipth_strdup		ipth_dme_strdup
#endif
#ifndef ipth_strcat
#define ipth_strcat		ipth_dme_strcat
#endif
#ifndef ipth_strncat
#define ipth_strncat	ipth_dme_strncat
#endif
#ifndef ipth_strcmp
#define ipth_strcmp		ipth_dme_strcmp
#endif
#ifndef ipth_strncmp
#define ipth_strncmp	ipth_dme_strncmp
#endif
#ifndef ipth_stricmp
#define ipth_stricmp    ipth_dme_stricmp
#endif
#ifndef ipth_strnicmp
#define ipth_strnicmp   ipth_dme_strnicmp
#endif
#ifndef ipth_strchr
#define ipth_strchr		ipth_dme_strchr
#endif
#ifndef ipth_strrchr
#define ipth_strrchr	ipth_dme_strrchr
#endif
#ifndef ipth_strstr
#define ipth_strstr		ipth_dme_strstr
#endif
#ifndef ipth_stristr
#define ipth_stristr	ipth_dme_stristr
#endif
#ifndef ipth_strtok_r
#define ipth_strtok_r	ipth_dme_strtok_r
#endif
#ifndef ipth_strtoul
#define ipth_strtoul	ipth_dme_strtoul
#endif
#ifndef ipth_isspace
#define ipth_isspace	ipth_dme_isspace
#endif
#ifndef ipth_isdigit
#define ipth_isdigit	ipth_dme_isdigit
#endif
#ifndef ipth_isalpha
#define ipth_isalpha	ipth_dme_isalpha
#endif
#ifndef ipth_memcpy
#define ipth_memcpy		ipth_dme_memcpy
#endif
#ifndef ipth_memmove
#define ipth_memmove	ipth_dme_memmove
#endif
#ifndef ipth_memset
#define ipth_memset		ipth_dme_memset
#endif
#ifndef ipth_memcmp
#define ipth_memcmp		ipth_dme_memcmp
#endif
#ifndef ipth_memchr
#define ipth_memchr		ipth_dme_memchr
#endif
#ifndef ipth_calloc
#define ipth_calloc		ipth_dme_calloc
#endif
#ifndef ipth_atoi
#define ipth_atoi		ipth_dme_atoi
#endif
#ifndef ipth_malloc
#define ipth_malloc(x)		ipth_HALStdcMalloc((x)==0 ? 1 : (x))
#endif
#ifndef ipth_realloc
#define ipth_realloc(p,x)	ipth_HALStdcRealloc((p),(x)==0 ? 1 : (x))
#endif
#ifndef ipth_free
#define ipth_free		ipth_HALStdcFree
#endif
#ifndef ipth_sprintf
#define ipth_sprintf		ipth_HALStdcSprintf
#endif
#ifndef ipth_snprintf
#define ipth_snprintf		ipth_HALStdcSnprintf
#endif

#ifndef ipth_fopen
#define ipth_fopen		ipth_HALFileOpen
#endif
#ifndef ipth_fclose
#define ipth_fclose		ipth_HALFileClose
#endif
#ifndef ipth_fread
#define	ipth_fread		ipth_HALFileRead
#endif
#ifndef	ipth_fwrite
#define	ipth_fwrite		ipth_HALFileWrite
#endif
#ifndef ipth_fgets
#define	ipth_fgets		ipth_HALFileGetStr
#endif
#ifndef ipth_ftell
#define ipth_ftell		ipth_HALFileGetPosition	/* Used in LOGGER only */
#endif
#ifndef	ipth_fflush
#define	ipth_fflush		ipth_HALFileFlush
#endif
#ifndef	ipth_feof
#define	ipth_feof		ipth_HALFileIsEndOfFile
#endif
#ifndef	ipth_ferror
#define	ipth_ferror		ipth_HALFileHasError
#endif
#ifndef	ipth_fsize
#define	ipth_fsize		ipth_HALFileGetSize
#endif
#ifndef	ipth_remove
#define	ipth_remove		ipth_HALFileRemove
#endif
#ifndef	ipth_rename
#define	ipth_rename		ipth_HALFileRename
#endif

/**
*
*\use std c libraries
*
*/
#else /* NOT_USE_STDC */

#ifndef ipth_strlen
#define ipth_strlen		strlen
#endif
#ifndef ipth_strcpy
#define ipth_strcpy		strcpy
#endif
#ifndef ipth_strncpy
#define ipth_strncpy	strncpy
#endif
#ifndef ipth_strdup
#define ipth_strdup		strdup
#endif
#ifndef ipth_strcat
#define ipth_strcat		strcat
#endif
#ifndef ipth_strcmp
#define ipth_strcmp		strcmp
#endif
#ifndef ipth_strncmp
#define ipth_strncmp	strncmp
#endif
#ifndef ipth_stricmp
#define ipth_stricmp    stricmp
#endif
#ifndef ipth_strnicmp
#define ipth_strnicmp   strnicmp
#endif
#ifndef ipth_strchr
#define ipth_strchr		strchr
#endif
#ifndef ipth_strrchr
#define ipth_strrchr	strrchr
#endif
#ifndef ipth_strstr
#define ipth_strstr		strstr
#endif
#ifndef ipth_strtok_r
#define ipth_strtok_r	strtok_r
#endif
#ifndef ipth_strtoul
#define ipth_strtoul	strtoul
#endif
#ifndef ipth_isspace
#define ipth_isspace	isspace
#endif
#ifndef ipth_isdigit
#define ipth_isdigit	isdigit
#endif
#ifndef ipth_isalpha
#define ipth_isalpha	isalpha
#endif
#ifndef ipth_memcpy
#define ipth_memcpy		memcpy
#endif
#ifndef ipth_memmove
#define ipth_memmove	memmove
#endif
#ifndef ipth_memset
#define ipth_memset		memset
#endif
#ifndef ipth_memcmp
#define ipth_memcmp		memcmp
#endif
#ifndef ipth_memchr
#define ipth_memchr		memchr
#endif
#ifndef ipth_malloc
#define ipth_malloc		malloc
#endif
#ifndef ipth_realloc
#define ipth_realloc	realloc
#endif
#ifndef ipth_calloc
#define ipth_calloc		calloc
#endif
#ifndef ipth_atoi
#define ipth_atoi		atoi
#endif
#ifndef ipth_free
#define ipth_free		free
#endif
#ifndef ipth_fopen
#define ipth_fopen		fopen
#endif
#ifndef ipth_fclose
#define ipth_fclose		fclose
#endif
#ifndef	ipth_fread
#define	ipth_fread		fread
#endif
#ifndef	ipth_fwrite
#define	ipth_fwrite		fwrite
#endif
#ifndef	ipth_fflush
#define	ipth_fflush		fflush
#endif
#ifndef	ipth_feof
#define	ipth_feof		feof
#endif
#ifndef	ipth_ferror
#define	ipth_ferror		ferror
#endif
#ifndef	ipth_fsize
#define	ipth_fsize		ipth_HALFileGetSize
#endif
#ifndef	ipth_remove
#define	ipth_remove		remove
#endif
#ifndef	ipth_rename
#define	ipth_rename		rename
#endif
#ifndef ipth_fgets
#define	ipth_fgets		fgets
#endif
#ifndef ipth_fprintf
#define ipth_fprintf	fprintf
#endif
#ifndef ipth_ftell
#define ipth_ftell		ftell	/* Used in LOGGER only */
#endif
#ifndef ipth_fscanf
#define ipth_fscanf		fscanf	/* Used in SCOMO enabler only */
#endif
#ifndef ipth_fgetc
#define ipth_fgetc		fgetc	/* Used in SCOMO enabler only */
#endif
#ifndef ipth_sprintf
#define ipth_sprintf	sprintf
#endif
#ifndef ipth_snprintf
#define ipth_snprintf	snprintf
#endif
#ifndef ipth_vfprintf
#define ipth_vfprintf	vfprintf	/* Used in LOGGER only */
#endif
#ifndef ipth_vsnprintf
#define ipth_vsnprintf	vsnprintf	/* Used in LOGGER only */
#endif

#ifndef ipth_Assert
/**
 * Assert definition.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define	ipth_Assert(expr)   assert(expr)
#endif

#ifndef IPTH_LOG
/**
 * Log a message.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define ipth_Log ipth_LogV
#endif

#endif /* NOT_USE_STDC */

#ifndef ipth_Assert
/**
 * Assert definition.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define	ipth_Assert(expr) ((void)0)
#endif

#ifndef ipth_Verify
/**
 * Assert that the given test should pass.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define ipth_Verify(expr, test) do { if(!(expr test)) {ipth_Assert(0);} } while(0)
#endif

/**
 * Free a pointer if not NULL, and then assign NULL to it.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define ipth_free_ptr(p) if (p)  do { ipth_free(p); p = NULL; } while(0);

/**
*
* \brief Specifies output Stream format.
*
*/

typedef enum ipth_fmtstream_type {
    IPTH_FMTSTREAM_MEM,
    IPTH_FMTSTREAM_LOG,
    IPTH_FMTSTREAM_FILE
} ipth_fmtstream_type_e;

#ifndef ipth_fmtstream_m
#define ipth_fmtstream_m(t,p,...) \
		do{ \
			char* buf__[40]; /* the buffer shall start with char* and be at least 16 bytes long */ \
			ipth_fmtstream_init( buf__,sizeof(buf__),t,(void*)p); \
			while ( ipth_fmtstream_check(buf__, ipth_snprintf(buf__[0],ipth_fmtstream_len(buf__),__VA_ARGS__))) {;} \
		} \
		while(0)
#endif

#ifndef ipth_asprintf
#define ipth_asprintf(b,...) ipth_fmtstream_m(IPTH_FMTSTREAM_MEM,b,__VA_ARGS__)
#endif

#ifndef ipth_Log
/**
 * Log a message.
 *
 * \par InnoPath Internal
 * For internal use only.
 */
#define ipth_Log(l,...) ipth_fmtstream_m(IPTH_FMTSTREAM_LOG,l,__VA_ARGS__)
#endif

#ifndef ipth_fprintf
#define ipth_fprintf(fp,...) ipth_fmtstream_m(IPTH_FMTSTREAM_FILE,fp,__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /*======================================================================================
    									FUNCTION PROTOTYPES
    ========================================================================================*/

    /**
     *
     * Initializes the passed in buf ptr to ipth_fmtstream_s based on the type.
     *
     * \param [in]  buf    Pointer to memory location of the format stream.
     * \param [in]  len    size of buf .
     * \param [in]  type  stream type  mem, log or file .
     * \param [in]  parm  format stream is initialized with passed in parm.
     *
     * \retval   void ptr to the initialized and formated stream.
     *
     */


    void *ipth_fmtstream_init( char **buf, unsigned int len, ipth_fmtstream_type_e type, void *parm);


    /**
     *
     * Returns length  of the stream passed in.
     *
     * \param[in] h  Pointer to format stream.
     *
     * \retval len     of the stream passed in.
     *
     */


    unsigned int ipth_fmtstream_len( void *h);


    /**
     *
     * checks stream type . Logs stream if stream type is log , writes to file if type is stream type
     * is file, creates copy if streamtype is memory.
     *
     * \param[in] h  Pointer to format  stream.
     * \param[in] rc remaining count / bytes  to process in the stream.
     *
     * \retval  pointer to format stream buff or NULL.
     *
     */


    int ipth_fmtstream_check( void *h, int rc);

#ifdef __cplusplus
}
#endif

#endif	/* __IPTH_STDLIB_H__ */

