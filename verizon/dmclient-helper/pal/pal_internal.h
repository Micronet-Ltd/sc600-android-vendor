/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

/*
 * Declarations for internal use only.
 * Don't include this file outside PAL.
 */

#ifndef PAL_INTERNAL_H_
#define PAL_INTERNAL_H_

#include <math.h>
#include <pal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>
#include <utils/Log.h>

#define MAX_DATA_SIZE           127
// private static final int EX_SECURITY = -1;
#define PARCEL_EXCP_SECURITY (-1)
// private static final int EX_BAD_PARCELABLE = -2;
#define PARCEL_EXCP_BAD_PARCELABLE (-2)
// private static final int EX_ILLEGAL_ARGUMENT = -3;
#define PARCEL_EXCP_ILLEGAL_ARGUMENT (-3)
/*
 * private static final int EX_NULL_POINTER = -4;
 * reserved to pass PAL error code
 */
#define PARCEL_EXCP_NULL_POINTER (-4)
// private static final int EX_ILLEGAL_STATE = -5;
#define PARCEL_EXCP_ILLEGAL_STATE (-5)

#define UNUSED(__X__) (void)(__X__)

#define CODE2STR(__CODE__) #__CODE__
#define LINE2STR(__CODE__) CODE2STR(__CODE__)

#define __STRLINE__ LINE2STR(__LINE__)

#define __SHORT_FILE__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DBGPRINT(__FMT__, ...)        \
    ALOGD("%15s:%04d:%15s: " __FMT__, \
        __SHORT_FILE__, __LINE__,     \
        __FUNCTION__, ##__VA_ARGS__); \

#define WRNPRINT(__FMT__, ...)        \
    ALOGW("%15s:%04d:%15s: " __FMT__, \
        __SHORT_FILE__, __LINE__,     \
        __FUNCTION__, ##__VA_ARGS__); \

#define DEV_MINORBITS  8
#define DEV_MINORMASK  ((1U << DEV_MINORBITS) - 1)

#define DEV_MAJOR(dev) ((unsigned int) ((dev) >> DEV_MINORBITS))
#define DEV_MINOR(dev) ((unsigned int) ((dev) & DEV_MINORMASK))

#define BZEROTYPE(__VALUE__) bzero((void*)(&(__VALUE__)), sizeof(__VALUE__))

#define IPAL_S_THIS BpOmadmServiceAPI::mBindrToService

/**
 * Initializes connection to the OmadmService if it is not initialized.
 * Returns from calling function with error code RESULT_ERROR_PAL_NO_INIT
 * if connection could not be established
 * @note returns from calling function
 */
#define OMADM_SERVICE_INITIALIZED()               \
    if (!BpOmadmServiceAPI::isPalInitialized()) { \
        WRNPRINT("RESULT_ERROR_PAL_NO_INIT");     \
        return RESULT_ERROR_PAL_NO_INIT;          \
    }                                             \

__BEGIN_DECLS

static inline int log10i(int arg) {
    int cntr = 0;
    while (arg >= 10) {
        arg /= 10;
        cntr ++;
    }
    return cntr;
}

static inline int print_size_10(int arg) {
  if (arg < 0) {
    arg *= 10;
  }
  arg = abs(arg);
  arg ++;
  arg = log10i(arg) + 1;
  return arg;
}

/**
 * Release array of c-strings
 * @param argc size of the array
 * @param argv pointer to the array
 */
void __pal_argvs_free(int argc, char** argv);

#define __PAL_FREE_ARGVS(__ARG_C__, __ARG_V__) \
  __pal_argvs_free((__ARG_C__), (__ARG_V__));  \
  (__ARG_C__) = 0;                             \
  (__ARG_V__) = NULL;                          \

/*
 * Appends c-string to *p_argv array of c-strings
 * @return @see enum result_states
 */
int __pal_argvs_append(int* p_argc, char*** p_argv, const char* item);

/**
 * Returns c string of the PAL error code
 * @param pal_err_code PAL error code
 * @return readable c string of PAL error code
 */
const char* __pal_str_err_code(int pal_err_code);

/**
 * Reallocates memory in data_buffer_t
 * @param thiz pointer to the data_buffer_t structure. thiz->size contains
 * requested size.
 * @param old_size pointer to the value that contains current size of
 * tha allocated memory
 * @return @see enum result_states
 */
int __pal_realloc_data_buffer(data_buffer_t* thiz, size_t* old_size);

/**
 * Releases memory in data_buffer_t
 * It does not release memory which is used to story data_buffer_t
 * @param thiz pointer to the data_buffer_t structure.
 */
void __pal_free_data_buffer(data_buffer_t* thiz);

char* pal_strcat_path(const char* arg0, const char* arg1, char** out);

char* pal_getstr_from_file(const char* path);

int pal_is_path_dir(char* path);

int pal_is_path_file(char* path);

int pal_is_string_int(const char* str);

/**
 * Reads property by name
 * @param property_name property name
 * @p_value pointer to value to store false(0) or true(1)
 * @return @see enum result_states
 */
int android_get_property_bool(const char* property_name, int32_t* p_value);

int pal_readlink_recursive(const char* path, char** buff, size_t buff_size);

void pal_process_info_log();

void pal_meminfo_log();

__END_DECLS

/**
 * It is used to check data_buffer_t value
 * @param __BUFF__ data_buffer_t value
 * @note returns from calling function
 */
#define CHECK_BUFFER(__BUFF__) {              \
    if ((__BUFF__) == NULL) {                 \
        WRNPRINT("&data_buffer_t is null");   \
        return RESULT_ERROR_INVALID_ARGS;     \
    }                                         \
    if ((__BUFF__)->data == NULL) {           \
      WRNPRINT("data_buffer_t.data is null"); \
        return RESULT_BUFFER_NOT_DEFINED;     \
    }                                         \
    DBGPRINT("size = %d", (__BUFF__)->size);  \
}                                             \

/**
 * It is used to check pointer
 * @param __PVALUE__ pointer
 * @note returns from calling function
 */
#define CHECK_PVALUE(__PVALUE__) {            \
    if ((__PVALUE__) == NULL) {               \
        WRNPRINT(#__PVALUE__" is null");      \
        return RESULT_ERROR_INVALID_ARGS;     \
    }                                         \
}                                             \

/**
 * It is used to check data_buffer_t buffer
 * and store __ARG_INT__ value as string to this buffer
 * @param __BUFF__ data_buffer_t value
 * @param __ARG_INT__ int value
 * @note returns from calling function
 */
#define CHECK_AND_SET_BUFFER_INT2STR(__BUFF__, __ARG_INT__) {        \
    CHECK_BUFFER(__BUFF__);                                          \
    uint_t required_size = print_size_10(__ARG_INT__) + 1;           \
    if ((__BUFF__)->size < required_size) {                          \
        (__BUFF__)->size = required_size;                            \
        return RESULT_BUFFER_OVERFLOW;                               \
    }                                                                \
    snprintf((__BUFF__)->data, (__BUFF__)->size, "%d", __ARG_INT__); \
}                                                                    \

/**
 * It is used to check data_buffer_t buffer
 * and store __ARG_STR__ value to this buffer
 * @param __BUFF__ data_buffer_t value
 * @param __ARG_STR__ c-string
 * @note returns from calling function
 */
#define CHECK_AND_SET_BUFFER_STR2STR(__BUFF__, __ARG_STR__) { \
    CHECK_BUFFER(__BUFF__);                                   \
    uint_t required_size = strlen(__ARG_STR__) + 1;           \
    if ((__BUFF__)->size < required_size) {                   \
        (__BUFF__)->size = required_size;                     \
        return RESULT_BUFFER_OVERFLOW;                        \
    }                                                         \
    strncpy((__BUFF__)->data, __ARG_STR__, (__BUFF__)->size); \
}                                                             \

/**
 * It tries to get error code from exception.
 * It expects that error code is sent via exception by this way:
 * throw new IllegalStateException("" + errorCode);.
 * @param __RESULT__ Parcel which is filled by
 * IBinder::transact(uint32_t, const Parcel&, Parcel* reply, uint32_t flags = 0)
 * method where reply is pointer to the __RESULT__ value
 * @see Parcel.writeException(Exception e)
 */
#define GET_ERR_CODE(__RESULT__)                                    \
{                                                                   \
    int32_t err = __RESULT__.readExceptionCode();                   \
    if (err < 0) {                                                  \
        if (__RESULT__.dataAvail() > 0) {                           \
            int pal_error = 0;                                      \
            String16 strErrCode = __RESULT__.readString16();        \
            const char* cstrErrCode = String8(strErrCode).string(); \
            WRNPRINT("strErrCode = (%d) %s", err, cstrErrCode);     \
            if (pal_is_string_int(cstrErrCode)) {                   \
                pal_error = atoi(cstrErrCode);                      \
            }                                                       \
            if (pal_error != 0) {                                   \
                err = pal_error;                                    \
            } else {                                                \
                switch(err) {                                       \
                    case PARCEL_EXCP_SECURITY:                      \
                        err = RESULT_ERROR_PERMISSIONS;             \
                        break;                                      \
                    case PARCEL_EXCP_BAD_PARCELABLE:                \
                        err = RESULT_ERROR_UNDEFINED;               \
                        break;                                      \
                    case PARCEL_EXCP_ILLEGAL_ARGUMENT:              \
                        err = RESULT_ERROR_INVALID_ARGS;            \
                        break;                                      \
                    case PARCEL_EXCP_ILLEGAL_STATE:                 \
                        err = RESULT_ERROR_INVALID_STATE;           \
                        break;                                      \
                    default:                                        \
                        err = RESULT_ERROR;                         \
                        break;                                      \
                }                                                   \
            }                                                       \
        }                                                           \
        return err;                                                 \
    } else {                                                        \
        DBGPRINT("err = %d", err);                                  \
    }                                                               \
}                                                                   \

#define FREE_IF(__POINTER__)     \
    if (NULL != (__POINTER__)) { \
      free(__POINTER__);         \
      (__POINTER__) = NULL;      \
    }

#define FREE_IF_2P(__P0__, __P1__) \
    if ((__P0__) == (__P1__)) {    \
      FREE_IF(__P0__);             \
      (__P1__) = NULL;             \
    } else {                       \
      FREE_IF(__P0__);             \
      FREE_IF(__P1__);             \
    }

/**
 * Prints line to the log if function returns error in this line
 * @param __CALL_STR__ call of function with arguments
 * @return @see enum result_states
 */
#define PAL_CHECK_CALL(__CALL_STR__)                                 \
({                                                                   \
    int err_code = __CALL_STR__;                                     \
    if (err_code != RESULT_SUCCESS) {                                \
        WRNPRINT(#__CALL_STR__ ":%s", __pal_str_err_code(err_code)); \
    }                                                                \
    err_code;                                                        \
})

/**
 * Prints line to the log if function returns error in this line
 * end returns from calling function
 * @param __CALL_STR__ call of function with arguments
 * @return @see enum result_states
 * @note returns from calling function
 */
#define PAL_RETURN_IF_ERROR(__CALL_STR__)                            \
{                                                                    \
    int err_code = __CALL_STR__;                                     \
    if (err_code != RESULT_SUCCESS) {                                \
        WRNPRINT(#__CALL_STR__ ":%s : return from %s",               \
            __pal_str_err_code(err_code), __FUNCTION__);             \
        return err_code;                                             \
    }                                                                \
}

#define MALLOCTZ(__TYPE__)                                  \
({                                                          \
    __TYPE__* __thiz = (__TYPE__*)malloc(sizeof(__TYPE__)); \
    if (__thiz != NULL) {                                   \
        bzero(__thiz, sizeof(__TYPE__));                    \
    }                                                       \
    __thiz;                                                 \
})

/*
 * Calls in the OmadmService the "int method(void)" to
 * get int32_t value
 * @param __IPAL_CODE__ @see enum OMADM_API_ENUM
 * @param __P_VALUE__ pointer to int32_t value to store
 * received value
 * @return @see enum result_states
 */
#define OMADM_PAL_GET_I4(__IPAL_CODE__, __P_VALUE__)    \
({                                                      \
  DBGPRINT("(%p)", __P_VALUE__);                        \
  CHECK_PVALUE(__P_VALUE__);                            \
  int32_t out32;                                        \
  AutoMutex _l(BpOmadmServiceAPI::mPalLock);            \
  OMADM_SERVICE_INITIALIZED();                          \
  int __rc = IPAL_S_THIS->rpc((__IPAL_CODE__), &out32); \
  *(__P_VALUE__) = out32;                               \
  DBGPRINT("(&%d) = %d", *__P_VALUE__, __rc);           \
  __rc;                                                 \
})

/*
 * Calls in the OmadmService the "void method(int)" to
 * set int32_t value
 * @param __IPAL_CODE__ @see enum OMADM_API_ENUM
 * @param __VALUE__ sent value
 * @return @see enum result_states
 */
#define OMADM_PAL_SET_I4(__IPAL_CODE__, __VALUE__)           \
({                                                           \
  DBGPRINT("(%d)", __VALUE__);                               \
  AutoMutex _l(BpOmadmServiceAPI::mPalLock);                 \
  OMADM_SERVICE_INITIALIZED();                               \
  int __rc = IPAL_S_THIS->rpc((__IPAL_CODE__), (__VALUE__)); \
  DBGPRINT("(&%d) = %d", (__VALUE__), __rc);                 \
  __rc;                                                      \
})

/**
 * Returns RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE and add
 * warning message to the logcat buffer
 * @return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE
 */
#define OMADM_PAL_NOT_SUPPORTED()         \
({                                        \
  WRNPRINT("NOT_SUPPORTED");              \
  RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE; \
})

#define STRSTART(__STR__, __PATTERN__) \
    strstr((__STR__),(__PATTERN__)) == (__STR__)

#define STRCASESTART(__STR__, __PATTERN__) \
    strcasestr((__STR__),(__PATTERN__)) == (__STR__)

#endif /* PAL_INTERNAL_H_ */
