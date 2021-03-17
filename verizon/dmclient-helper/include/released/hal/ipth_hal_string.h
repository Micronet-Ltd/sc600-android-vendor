/******************************************************************************
 * Copyright (c) 2010 InnoPath Software, Inc. All rights reserved.
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

#ifndef __IPTH_HAL_STRING_H__
#define __IPTH_HAL_STRING_H__

/**
 * \file
 *
 * This file includes the string manipulation interfaces.
 *
 * \par API Name
 *      Device Manufacturer: String Manipulation
 */

#include <ipth_hal_config.h>

IP_START_EXTERN_C

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Frees memory allocated by a HAL string function.
 * Note: This function is required for feature phone development.
 *
 * \param[in] str Pointer to the string to be freed.
 */
IP_DLL_EXPORT
void ipth_HALStringFree(void *str);

/**
 * Allocates memory and converts a WCHAR to a char. WCHAR is defined in
 * ipth_hal_config.h, a platform specific file.
 * Use ipth_HALStringFree to release memory for the returned string.
 * OEM is only required to implement this function for Windows Mobile devices.
 *
 * \param[in] wStr Pointer to a wide char string to be converted.
 * \return Pointer to a char string, or NULL if the conversion failed.
 */
IP_DLL_EXPORT
char* ipth_HALStringWCHARToChar(const WCHAR *const wStr);

/**
 * Allocates memory and converts a char to a WCHAR. WCHAR is defined in
 * ipth_hal_config.h, a platform specific file.
 * Use ipth_HALStringFree to release memory for the returned string.
 * OEM is only required to implement this function for Windows Mobile devices.
 *
 * \param[in] str Pointer to the string to be converted.
 * \return	Pointer to a WCHAR string, or NULL if the conversion failed.
 *
 */
IP_DLL_EXPORT
WCHAR* ipth_HALStringCharToWCHAR(const char *const str);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Allocates memory and copies the input string into the returned string.
 * Use ipth_HALStringFree to release memory for the returned string.
 * Note: This function is required for feature phone development.
 *
 * \param[in] stringToCopy	String to copy.
 * \return Pointer to the copied string.
 */
IP_DLL_EXPORT
char* ipth_HALStringCopy(const char *stringToCopy);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Estimates a buffer size that is large enough for the input formatted
 * string.
 * Note: This function is required for feature phone development.
 *
 * \param[in] format Format control string.
 * \param[in] args   Optional arguments.
 * \return Number of bytes for the estimated buffer size or -1 if an error occurs.
 */
IP_DLL_EXPORT
int ipth_HALStringGetFormatSize(const char *format, va_list args);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Allocates a string large enough to hold the formatted output, including
 * the terminating null byte, and returns a pointer to the string in the buf
 * parameter. Use ipth_HALStringFree to release memory for the returned string.
 * Note: This function is required for feature phone development.
 *
 * \param[in] buf    Allocated buffer for the output string
 * \param[in] format Format control string
 * \param[in] ...    Optional arguments
 * \return Number of bytes formatted (like sprintf) if successful.
 *         Returns -1 if memory allocation fails or another error occurs.
 */
IP_DLL_EXPORT
int ipth_HALStringAsprintf(char **buf, const char *format, ...);

/**
 * Creates a string defined by the result of the formatted output
 * and concatenates this string to the end of the string referenced by the buf parameter.
 * The function reallocates memory if the buffer is too small to hold the new string.
 *
 * If the buf parameter is NULL, the string is allocated and returned.
 * If the new memory allocation fails, the previous buffer is returned.
 * It is safe to call this function multiple times even if memory allocation fails because
 * the data up to the point of failure is saved in the buffer.
 * OEM is only required to implement this function for Windows Mobile devices or MMS plugin supported client.
 * If the buffer returned is different from the input buffer,
 * use ipth_HALStringFree to release memory for the returned string.
 *
 * \param[in] buf    Buffer for the output string.
 * \param[in] format Format control string.
 * \param[in] ...    Optional arguments.
 * \return Pointer to buffer, which is either the buffer on input or a new buffer
 *         allocated to accommodate the required additional memory.
 */
IP_DLL_EXPORT
char * ipth_HALStringCasprintf(char *buf, const char *format, ...);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Splits a string into smaller strings (tokens), with each token
 * delimited by any character from the control parameter. For example, you can
 * tokenize the string "This is an example string" into
 * individual words by using the space character as the delimiter. You can specify multiple tokens
 * in the control parameter. The string will be tokenized when any one of the
 * delimiters is encountered.
 *
 * Note that only the first call to ipth_HALStringStrtok uses the str parameter. Every
 * subsequent call to ipth_HALStringStrtok must set the str parameter to NULL.
 * The delimiter string always needs to be passed.
 * To start over, or to tokenize a new string, call ipth_HALStringStrtok with a
 * string argument to initialize the function.
 *
 * Note: This function is required for feature phone development.

 * \param[in] str         String to tokenize.
 * \param[in] control     String that specifies the delimiter characters.
 * \param[in,out] context Pointer to the environment that's needed to get consecutive
 *                        tokens from a string.
 * \return Pointer to the token if found; otherwise, NULL.
 */
/*
 * Example - tokenize the pangram string into its individual words by using the
 *           space character as the delimiter:
 *
 *	char *pangram = "The quick brown fox jumps over the lazy dog";
 *	const char *control = " ";
 *	char *context = NULL;
 *  // Get first token
 *	char *token = ipth_HALStringStrtok(pangram, control, &context);
 *	while (token)
 *	{
 *      // Gets each consecutive token
 *		token = ipth_HALStringStrtok(NULL, control, &context);
 *	}
 */
IP_DLL_EXPORT
char* ipth_HALStringStrtok(char *str, const char *control, char **context);

/**
 * Compares two strings (case sensitive).
 *
 * \param[in] str1 Pointer to first string.
 * \param[in] str2 Pointer to second string.
 * \return True if the strings are equal; otherwise, false.
 */
IP_DLL_EXPORT
bool ipth_HALStringCompare(const char *str1, const char *str2);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Compares two strings, ignoring case.
 * Note: This function is required for feature phone development.
 *
 * \param[in] str1	Pointer to first string.
 * \param[in] str2	Pointer to second string.
 * \return	True if the strings are equal; otherwise, false.
 */
IP_DLL_EXPORT
bool ipth_HALStringCompareNoCase(const char *str1, const char *str2);

/******************************************************************************
 * REQUIRED FUNCTION FOR FEATURE PHONE DEVELOPMENT
 ******************************************************************************/
/**
 * Compares up to the lengthToCompare parameter number of characters of two strings,
 * first converting the strings to lower case.
 * Note: This function is required for feature phone development.
 *
 * \param[in] str1	          Pointer to first string.
 * \param[in] str2	          Pointer to second string.
 * \param[in] lengthToCompare Number of characters to compare.
 * \return	True if the strings are equal; otherwise, false.
 */
IP_DLL_EXPORT
bool ipth_HALStringCompareNoCaseLen(const char *str1, const char *str2, unsigned int lengthToCompare);

/**
 * Allocates and generates a Universally Unique Identifier (UUID) string.
 * Use ipth_HALStringFree to release memory for the returned string.
 * OEM is only required to implement this function for Windows Mobile devices.
 *
 * \param[in,out] uuidStr	Variable to store the returned UUID string.
 * \retval	IPTH_OK
 * \retval  IPTH_ERR_NOT_IMPLEMENTED  Target platform is not for Windows Mobile.
 */
IP_DLL_EXPORT
int ipth_HALStringGenerateUuid(char **uuidStr);

IP_END_EXTERN_C

#endif /*__IPTH_HAL_STRING_H__*/
