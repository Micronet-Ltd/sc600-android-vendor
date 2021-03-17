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

#ifndef	IPTH_HAL_STDLIB_H
#define	IPTH_HAL_STDLIB_H

/**
 * \file
 *
 * This file includes interfaces for the standard C library.
 *
 * \par API Name
 *      Device Manufacturer: Standard C Library
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
    * Determines the length of the input string.
    *
    * \param [in] str Input string for determining the length.
    *
    * \return Length of the input string.
    *
    */
    unsigned int ipth_dme_strlen(const char *str);

    /**
    * Copies source string to destination string,
    * including the null terminator.
    *
    * \param [in] strDestination   Destination string.
    * \param [in] strSource        Source string.
    *
    * \return Pointer to destination string.
    *
    */
    char *ipth_dme_strcpy(char *strDestination, const char *strSource);

    /**
    *    Copies source string to destination string
    *    up to the number of characters specified by the n parameter or until
    *    the null terminator of the source string is reached.
    *
    * \param [in] strDestination   Destination string.
    * \param [in] strSource        Source string.
    * \param [in] n                Maximum number of characters to copy.
    *
    * \return Pointer to destination string.
    *
    */
    char *ipth_dme_strncpy(char *strDestination, const char *strSource, unsigned int n);

    /**
    * Duplicates the input string. Use ipth_HALStdcFree to release the memory
    * for the duplicated string.
    *
    * \param [in] s1        String to duplicate.
    * \return Pointer to the duplicated string if successful; otherwise, NULL if error occurred.
    *
    */
    char *ipth_dme_strdup(const char *s1);

    /**
    *    Concatenates source string to destination string.
    *
    * \param [in,out] strDestination   Destination string.
    * \param [in] strSource        Source string.
    *
    * \return Pointer to destination string.
    *
    */
    char *ipth_dme_strcat(char *strDestination, const char *strSource);

    /**
    *    Concatenates source string to destination string
    *    up to the number of characters specified by the n parameter or until
    *    the null terminator of the source string is reached.
    *
    * \param [in,out] strDestination   Destination string.
    * \param [in] strSource            Source string.
    * \param [in] n                    Number of characters to concatenate.
    *
    * \return Pointer to destination string.
    *
    */
    char *ipth_dme_strncat(char* strDestination, const char* strSource, unsigned int n);

    /**
    * Compares two input strings.
    *
    * \param [in] s1       String 1.
    * \param [in] s2       String 2.
    *
    *  \retval  <0     's1' is less than 's2'.
    *  \retval  0      's1' is identical to 's2'.
    *  \retval  >0     's1' is greater than 's2'.
    *
    */
    int ipth_dme_strcmp(const char *s1, const char *s2);

    /**
    * Compares two input strings for up to the number of characters specified by the count parameter
    * or until the null terminator of string 1 is reached.
    *
    * \param [in] s1       String 1.
    * \param [in] s2       String 2.
    * \param [in] count    Number of characters to compare.
    *
    *  \retval  <0     's1' is less than 's2'.
    *  \retval  0      's1' is identical to 's2'.
    *  \retval  >0     's1' is greater than 's2'.
    *
    */
    int ipth_dme_strncmp(const char *s1, const char *s2, unsigned int count);

    /**
    * Compares two input strings, ignoring case.
    *
    * \param [in] s1       String 1.
    * \param [in] s2       String 2.
    *
    *  \retval  <0     's1' is less than 's2'.
    *  \retval  0      's1' is identical to 's2'.
    *  \retval  >0     's1' is greater than 's2'.
    *
    */
    int ipth_dme_stricmp(const char *s1, const char *s2);

    /**
    * Compares two input strings for up to the number of characters specified
    * by the count parameter or until the null terminator of the source string
    * is reached, ignoring case.
    *
    * \param [in] s1       String 1.
    * \param [in] s2       String 2.
    * \param [in] count    Number of characters to compare.
    *
    *  \retval  <0     's1' is less than 's2'.
    *  \retval  0      's1' is identical to 's2'.
    *  \retval  >0     's1' is greater than 's2'.
    *
    */
    int ipth_dme_strnicmp(const char *s1, const char *s2, unsigned int count);

    /**
    * Searches for a search character in the input string.
    *
    * \param [in] string    String to search in.
    * \param [in] ch        Character to search for.
    *
    * \retval non-NULL Pointer to the first character in the string that matches the search character.
    * \retval NULL Search character is not found in the string.
    *
    */
    char *ipth_dme_strchr(const char *string, int ch);

    /**
    * Searches backwards for the search character in the string.
    *
    * \param [in] string    String to search in.
    * \param [in] ch        Character to search for.
    *
    * \retval non-NULL Pointer to the first character in the string that matches the search character.
    * \retval NULL Search character is not found in the string.
    *
    */
    char *ipth_dme_strrchr(const char *string, int ch);

    /**
    * Searches for a substring in the specified string.
    *
    * \param [in] string         String to search in.
    * \param [in] strCharSet     Substring to search for.
    *
    * \retval non-NULL Pointer to the substring.
    * \retval NULL Substring is not found in the string.
    *
    */
    char *ipth_dme_strstr(const char *string, const char* strCharSet);

    /**
    * Searches for a substring in the specified string, ignoring case.
    *
    * \param [in] string         String to search in.
    * \param [in] strCharSet     Substring to search for.
    *
    * \retval non-NULL Pointer to the substring.
    * \retval NULL Substring is not found in the string.
    *
    */
    char *ipth_dme_stristr(const char* string, const char* strCharSet);

    /**
     * Splits a string into smaller strings (tokens), with each token
     * delimited by any character from the control parameter. For example, you can
     * tokenize the string "This is an example string" into individual words by
     * using the space character as the delimiter. You can specify multiple tokens
     * in the control parameter. The string will be tokenized when any one of the
     * delimiters is encountered.
     *
     * Note that only the first call to ipth_dme_strtok_r uses the str parameter. Every
     * subsequent call to ipth_dme_strtok_r must set 'str' to NULL.
     * The control parameter, which is the delimiter string, always needs to be passed.
     * To start over, or to tokenize a new string, call ipth_dme_strtok_r with the
     * str argument non-null.
     *
     * \param[in] str           String to tokenize.
     * \param[in] control       String that contains the delimiter characters.
     * \param[in,out] context   Pointer to the environment that is needed to get
     *                          consecutive tokens from a string.
     * \return Pointer to the token if found; otherwise, NULL.
     */
    char *ipth_dme_strtok_r(char *str, const char *control, char **context);

    /**
     * Converts the string referenced by the nptr parameter
     * to unsigned long int type. First, the function decomposes the input
     * string into three parts: an initial, possibly empty, sequence of white-space
     * characters (as specified by ipth_dme_isspace); a subject sequence interpreted
     * as an integer represented in some radix determined by the value of the base parameter;
     * and a final string of one or more unrecognized characters, including the
     * terminating null of the input string. Next, the function attempts to convert the
     * subject sequence to an unsigned integer.
     *
     * \param [in] nptr        String to be converted to integer.
     * \param [in,out] endptr  If not NULL, the endptr parameter points to the nptr parameter
     *						  if the subject sequence is empty or does not have the expected form;
     *						  otherwise, 'endptr' points to the substring of one or more unrecognized
     *						  characters, including the terminating null of the input string if successful.
     * \param [in] base        Radix used for converting string to integer.
     *
     * \retval converted unsigned long integer if success.
     * \retval 0xFFFFFFFF if the correct value is outside the range of representable values.
     * \retval 0 No conversion could be performed.
     *
     */
    unsigned long ipth_dme_strtoul(const char *nptr, char **endptr, int base);

    /**
    * Checks whether the input character is numeric or not.
    *
    *  \param [in] c    Character value to be checked.
    *
    *  \retval 1 Character is numeric.
    *  \retval 0 Character is not numeric.
    *
    */
    int ipth_dme_isdigit(int c);

    /**
    * Checks whether the input character is alphabetic or not.
    *
    *  \param [in] c    Character value to be checked.
    *
    *  \retval 1 Character is alphabetic.
    *  \retval 0 Character is not alphabetic.
    *
    */
    int ipth_dme_isalpha(int c);

    /**
    * Checks whether the input character is a white-space character or not.
    *
    *  \param [in] c    Character value to be checked.
    *
    *  \retval 1 Character is a white-space character.
    *  \retval 0 Character is not a white-space character.
    *
    */
    int ipth_dme_isspace(int c);

    /**
    * Copies memory from the source to destination.
    * Source and destination ranges cannot overlap.
    * If the ranges overlap, use ipth_dme_memmove.
    *
    * \param[in] des   Destination address for the copy.
    * \param[in] src   Source address for the copy.
    * \param[in] len   Length, in bytes, to copy.
    *
    * \return Pointer to the destination address.
    *
    */
    void *ipth_dme_memcpy(void *des, const void *src, unsigned int len);

    /**
    * Copies memory from source address 's2' to destination address 's1'.
    * Unlike ipth_dme_memcpy, the source and destination ranges can overlap.
    *
    * \param[in] s1   Destination address for the copy.
    * \param[in] s2   Source address for the copy.
    * \param[in] n    Number of bytes to copy.
    *
    * \return Pointer to the destination address.
    *
    */
    void *ipth_dme_memmove(void *s1, const void *s2, unsigned int n);

    /**
    * Sets all bytes in the memory range to the specified value.
    *
    * \param[in] des   Destination address for the start of the set operation.
    * \param[in] val   Value used to set each memory byte.
    * \param[in] len   Number of bytes to set.
    *
    * \return Pointer to the destination address.
    *
    */
    void *ipth_dme_memset(void *des, int val, unsigned int len);

    /**
    * Compares two memory blocks of a specified size.
    *
    * \param[in] src1   The first memory block address.
    * \param[in] src2   The second memory block address.
    * \param[in] len    Length, in bytes, of the memory blocks to compare.
    *
    * \retval 0  Contents of the two memory blocks is identical.
    * \retval 1  Content of the two memory blocks is different.
    *
    */
    int ipth_dme_memcmp(const void *src1, const void *src2, int len);

    /**
    * Locates the first occurrence of the character 'c'
    * (converted to an unsigned character) in the object 's' for the number of bytes specified by 'n'.
    *
    * \param[in] s    Pointer to an object that is interpreted as a sequence of unsigned characters.
    * \param[in] c    Value to be found.
    * \param[in] n    Maximum number of bytes to be searched.
    *
    * \return Pointer to the first occurrence of the specified value if found; otherwise,
    *         NULL.
    */
    void *ipth_dme_memchr(const void *s, int c, unsigned int n);

    /**
    * Allocates unused space for an array of num parameter elements,
    * each of whose size in bytes is specified by the size parameter.
    * The space is initialized to all 0 bits.
    *
    * \param[in] num    Number of elements to allocate.
    * \param[in] size   Size of each element in bytes.
    *
    * \return Pointer to the allocated space if successful; otherwise, NULL if the function failed.
    *
    */
    void *ipth_dme_calloc(unsigned int num, unsigned int size);

    /**
    * Converts a string of decimal ASCII character to an integer value.
    *
    * \param[in] str    String of decimal ASCII characters.
    *
    * \return Converted integer value.
    *
    */
    int ipth_dme_atoi(const char* str);

    /**
    * Allocates unused space for an object of size parameter bytes
    * with an unspecified value.
    *
    * \param[in] size    Size in bytes.
    *
    * \return Pointer to the allocated space if successful; otherwise, NULL.
    *
    */
    void *ipth_HALStdcMalloc(unsigned int size);

    /**
    * Changes the size of the memory object referenced by the ptr parameter
    * to the size specified by the size parameter. The contents of the object shall remain
    * unchanged up to the lesser of the new and old sizes.
    *
    * \param[in] ptr    Pointer to an object.
    * \param[in] size   Size in bytes.
    *
    * \return Pointer to the allocated space if successful; otherwise, NULL.
    *
    */
    void *ipth_HALStdcRealloc(void *ptr, unsigned int size);

    /**
    * Frees the space pointed to by the ptr parameter so that is becomes available memory.
    *
    * \param[in] ptr    Pointer to an object.
    *
    */
    void ipth_HALStdcFree(void *ptr);

    /**
    * Outputs the formatted string, followed by the null character,
    * to the str parameter. The user must ensure that enough space is available.
    *
    * \param[out] str     Pointer to space for output.
    * \param[in] format   Pointer to output format.
    *
    * \return Number of characters output, excluding the terminating null byte, if successful;
    *         otherwise, a negative integer.
    *
    */
    int ipth_HALStdcSprintf(char *str, const char *format, ...);

    /**
    * Outputs the formatted string, followed by the null character,
    * to the str parameter, using the size parameter
    * as follows: if size is zero, nothing is written; otherwise, characters
    * beyond size-1 are discarded.
    *
    * \param[out] str     Pointer to output string.
    * \param[in] size     Number of characters in the str parameter.
    * \param[in] format   Pointer to output format.
    *
    * \return Number of characters output, excluding the terminating null character, if successful;
    *         otherwise, a negative integer.
    *
    */
    int ipth_HALStdcSnprintf(char *str, unsigned int size, const char *format, ...);

#ifdef __cplusplus
}
#endif

/* ipth_xxx APIs to be defined using platform provided STDLIB APIs */

#endif	/* IPTH_HAL_STDLIB_H */

