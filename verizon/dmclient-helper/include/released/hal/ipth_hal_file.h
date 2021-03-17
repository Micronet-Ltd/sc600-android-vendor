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

#ifndef __IPTH_HAL_FILE_H__
#define __IPTH_HAL_FILE_H__

/**
 * \file
 *
 * \brief This file includes the interfaces for file system access.
 *
 * The OS-platform dependent header file ipth_hal_config.h must supply definitions for:
 *	  FILE, a platform specific representation for a file; and
 *    ipth_HALFileSearchID, a platform specific representation to search for files.
 *
 * \par API Name
 *      Device Manufacturer: File System Management
 */

#include <ipth_hal_config.h>

IP_START_EXTERN_C

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Opens or creates a file for reading or writing.
 * Note: This function is required for feature phone development.
 *
 * The mode parameter options are as follows:
 *
 * '"r"' Opens a file for reading. The file must exist.
 * '"w"' Creates an empty file for writing. If a file with the same name already
 *     exists its content is erased and the file is treated as a new empty file.
 * '"a"' Appends to a file. Writing operations append data at the end of the file.
 *     The file is created if it does not exist.
 * '"r+"' Opens a file for update both reading and writing. The file must exist.
 * '"w+"' Creates an empty file for both reading and writing. If a file with the
 *      same name already exists, its content is erased and the file is treated
 *      as a new empty file.
 * '"a+"' Opens a file for reading and appending. All write operations are
 *      performed at the end of the file, protecting the previous content from being
 *      overwritten. You can reposition the internal pointer to
 *      anywhere in the file for reading, but writing operations will move it
 *      back to the end of file. The file is created if it does not exist.
 *
 * \param[in] filename   Name of the file to open.
 * \param[in] mode       The way to open the file (see Description).
 * \return FILE pointer or NULL if the operation fails.
 *
 *****************************************************************************/
IP_DLL_EXPORT
FILE* ipth_HALFileOpen(const char* filename, const char *mode);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Closes a file.
 * Note: This function is required for feature phone development.
 *
 * \param[in] file     A pointer to the file to close.
 * \return Zero if successful or non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileClose(FILE *file);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Returns the number of full items actually read, which may be
 * less than the value of the count parameter if an error occurs or if the end of the file is encountered
 * before reaching count items.
 * Use function ipth_HALFileHasError and
 * ipth_HALFileIsEndOfFile to distinguish a read error or invalid parameter error
 * from an end of file condition.
 *
 * If the size parameter or count parameter is zero, the function returns zero and the
 * contents of the buffer parameter are unchanged.
 * Note: This function is required for feature phone development.
 *
 * \param[in,out] buffer	Storage location for data.
 * \param[in] size	      Item size in bytes.
 * \param[in] count       Maximum number of items to read.
 * \param[in] stream	    Pointer to a file.
 * \return	Number of full items actually read.
 *
 *****************************************************************************/
IP_DLL_EXPORT
unsigned int ipth_HALFileRead(void *buffer, unsigned int size, unsigned int count, FILE *stream);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Writes an array of count parameter elements, each one with a size of size parameter bytes, from
 * the block of memory pointed by the buffer parameter to the current position in the stream.
 * The position indicator of the stream is advanced by the total number of bytes
 * written. The total amount of bytes written is (size * count).
 * Note: This function is required for feature phone development.
 *
 * \param[in] buffer	Pointer to the array of elements to write.
 * \param[in] size	  Item size in bytes.
 * \param[in] count   Number of items to write.
 * \param[in] stream	Pointer to a file.
 * \return	Number of full items actually written. If this number
 *          differs from the count parameter, it indicates an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
unsigned int ipth_HALFileWrite(const void *buffer, unsigned int size, unsigned int count, FILE *stream);

/**
 * Reads a line from the stream into the buffer. Reading stops when
 * end of line or end of the stream is reached, or the buffer is full
 * (one less than the buffer size).
 * A terminating zero is stored after the last character in the buffer.
 *
 * \param[out] buffer Storage location for data.
 * \param[in] size	  Maximum number of characters to read.
 * \param[in] stream  Pointer to a file.
 * \return Pointer to the buffer parameter if successful; otherwise, NULL if the operation failed.
 *
 *****************************************************************************/
IP_DLL_EXPORT
char *ipth_HALFileGetStr(char *buffer, int size, FILE *stream);

/**
 * Writes output in the specified format to the file stream.
 *
 * \param[in] stream	Pointer to a file.
 * \param[in] format	Format string.
 * \return	Number of characters written to the file stream (not including the trailing NULL character);
 *          otherwise, a negative value if the operation failed.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFilePrint( FILE *stream, const char *format, ...);

/**
 * Sets the position of an internal pointer in the file.
 *
 * \param[in] stream	Pointer to a file.
 * \param[in] pos		The new position of the internal file pointer.
 * \return	Zero if successful or non-zero if an error occurred.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileSetPosition(FILE *stream, long pos);

/**
 * Gets the current position of the internal file pointer.
 *
 * \param[in] stream	Pointer to a file.
 * \return	Current position if successful; otherwise, -1 if function failed.
 *
 *****************************************************************************/
IP_DLL_EXPORT
long ipth_HALFileGetPosition(FILE *stream);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Flushes a stream.
 * Note: This function is required for feature phone development.
 *
 * \param[in] file	Pointer to a file.
 * \return	Zero if successful or non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileFlush(FILE *file);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Tests whether the file pointer is at the end of file.
 * Note: This function is required for feature phone development.
 *
 * \param[in] file	Pointer to a file.
 * \return	Non-zero if at end of file; otherwise, zero if not at end of file.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileIsEndOfFile(FILE *file);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * This function tests whether a file is at an error condition.
 * Note: This function is required for feature phone development.
 *
 * \param[in] file	Pointer to a file.
 * \return	Non-zero if the file is at an error condition; otherwise, zero if not at an error condition.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileHasError(FILE *file);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Gets the size of a file.
 * Note: This function is required for feature phone development.
 *
 * \param[in] file	Pointer to a file.
 * \return	Size of file, in bytes.
 *
 *****************************************************************************/
IP_DLL_EXPORT
long ipth_HALFileGetSize(FILE *file);

/**
 * Copies a file from the src parameter name to the new dest parameter name.
 *
 * \param[in] src	Source file name.
 * \param[in] dest	Destination file name.
 * \return	Zero if successful or non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileCopy(const char* src, const char* dest);

/**
 * Creates a new directory specified by the newDirectoryName parameter at the path
 * specified by the directoryPath parameter.
 *
 * \param[in] directoryPath		Directory path to the location of new directory.
 * \param[in] newDirectoryName	Name of the new directory to create.
 * \return		Zero if successful or non-zero if an error occurred.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileCreateDir(const char * directoryPath, char * newDirectoryName);

/**
 * Checks whether a file exists or not.
 *
 * \param[in] filepath		Name of the file to check.
 * \return	Non-zero if the file exists; otherwise, zero if the file does not exist.
 *
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileExist(const char *filepath);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Renames a file or directory.
 * Note: This function is required for feature phone development.
 *
 * \param[in] oldname	Name of file or directory to rename.
 * \param[in] newname	New name of file or directory.
 * \return		Zero if successful or non-zero if an error occurred.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileRename(const char* oldname, const char* newname);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Removes a file.
 * Note: This function is required for feature phone development.
 *
 * \param[in] path		Name of the file to remove.
 * \return		Zero if successful or non-zero if an error occurred.
 *****************************************************************************/
IP_DLL_EXPORT
int ipth_HALFileRemove(const char *path);

/**
 * Rewinds a file by setting the internal position of the file to the
 * beginning of the file.
 *
 * \param[in] stream	Pointer to a file.
 *****************************************************************************/
IP_DLL_EXPORT
void ipth_HALFileRewind(FILE *stream);

/*###################### FILE SEARCH FUNCTIONS ##############################*/

/**
 * Sets up a search for files in the directory specified by the directory parameter using
 * the filter parameter as search criteria. Returns a search handler for use in
 * the ipth_HALFileSearchNext and ipth_HALFileEndSearch functions.
 * OEM is only required to implement this function for Windows Mobile devices or SCOMO supported client.
 *
 * \param[in] directory   Pointer to directory name.
 * \param[in] filter      Pointer to string specifying what to filter. Wildcard characters are allowed,
 * so to search for all files in a directory, specify "*".

 * \return	Pointer to a search handler.
 *
 *****************************************************************************/
IP_DLL_EXPORT
ipth_HALFileSearchID* ipth_HALFileStartSearch(const char *directory, const char *filter);

/**
 * Returns the next file in the search set up by the ipth_HALFileStartSearch function.
 * OEM is only required to implement this function for Windows Mobile devices or SCOMO supported client.
 *
 * Note: The returned string must be freed using the ipth_HALStringFree function.
 *
 * \param[in,out] searchID	Pointer to a search handler.
 * \return	Next file name that meets the search criteria, or NULL if
 *          no file was found.
 *****************************************************************************/
IP_DLL_EXPORT
char* ipth_HALFileSearchNext(ipth_HALFileSearchID *searchID);

/**
 * Terminates the search set up by the ipth_HALFileStartSearch function and discards the search handler.
 * OEM is only required to implement this function for Windows Mobile devices or SCOMO supported client.
 *
 * \param[in] searchID	Pointer to a search handler.
 *****************************************************************************/
IP_DLL_EXPORT
void ipth_HALFileEndSearch(ipth_HALFileSearchID *searchID);


IP_END_EXTERN_C

#endif /*__IPTH_HAL_FILE_H__*/
