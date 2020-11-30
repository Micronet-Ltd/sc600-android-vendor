/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../pal/pal_internal.h"
#include <sys/cdefs.h>
#include <pal.h>

#ifndef PAL_TEST_ENGINE_H_
#define PAL_TEST_ENGINE_H_

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

__BEGIN_DECLS

typedef struct {
    char* key;
    char* function;
    int args_types;
    char* comment;
} pal_func_description_t;

enum {
    __PAL_ARGS_I_S, // (int, data_buffer_t*)
    __PAL_ARGS_I_PR4, // (int, float*)
    __PAL_ARGS_PC_PR4, // (char*, float*)
    __PAL_ARGS_PC_PI4, // (char*, int32_t*)
    __PAL_ARGS_PC_PI, // (char*, int_t*)
    __PAL_ARGS_I_PI4, // (int, int32_t*)
    __PAL_ARGS_PI4, // (int32_t*)
    __PAL_ARGS_I, // (int)
    __PAL_ARGS_PI, // (int*)
    __PAL_ARGS_S, // (data_buffer_t*)
    __PAL_ARGS_V, // (void)
    __PAL_ARGS_PR4, // int fn(float*)
    __PAL_ARGS_FUMODLG, // int fn(pal_fumo_gui_message_descriptor*)
};

typedef int (*pal_int_int_pbuff_fn) (int, data_buffer_t*);
typedef int (*pal_int_int_pfloat_fn) (int, float*);
typedef int (*pal_int_int_pint32_fn) (int, int32_t*);
typedef int (*pal_int_pc_pfloat_fn) (char*, float*);
typedef int (*pal_int_pc_pint32_fn) (char*, int32_t*);
typedef int (*pal_int_pc_pint_fn) (char*, int*);
typedef int (*pal_int_pint32_fn) (int32_t*);
typedef int (*pal_int_pint_fn) (int*);
typedef int (*pal_int_int_fn) (int);
typedef int (*pal_int_pbuff_fn) (data_buffer_t*);
typedef int (*pal_int_void) (void);

typedef int (*pal_int_int_ppc_fn) (int, char**);

typedef pal_fumo_gui_message_descriptor pal_dialog_ui_t;

typedef int (*pal_int_dlgui_fn)(pal_dialog_ui_t*);

typedef struct {
    char* section_key;
    pal_int_int_ppc_fn section_func;
} pal_section_desc_t;

int __handle_pal_func_description(
    pal_func_description_t* desc, int argc, char** argv);

int pal_test_subsection(int argc, char** argv, pal_func_description_t* desc);

void getNewDataBuffer(data_buffer_t* pDBuffer, unsigned int size);

void freeDataBuffer(data_buffer_t* pDBuffer);

int pal_fumo_dlg_test(pal_int_dlgui_fn pfn, int argc, char** argv);

typedef struct{
    pthread_mutex_t mtx;
    pthread_cond_t cv;
} __pthread_sem_t;

int __pthread_sem_init(__pthread_sem_t*);

int __pthread_sem_deinit(__pthread_sem_t*);

int __pthread_sem_wait(__pthread_sem_t*);

int __pthread_sem_timedwait(__pthread_sem_t*, struct timespec*);

int __pthread_sem_notify(__pthread_sem_t*);

int __pthread_sem_notify_all(__pthread_sem_t* thiz);

/**
 * Prints line to the log if function returns error in this line
 * end returns from calling function
 * @param __CALL_STR__ call of function with arguments
 * @return @see enum result_states
 * @note returns from calling function
 */
#define __WRN_NE(__CALL_STR__,__RC__)                    \
({                                                       \
    int err_code = __CALL_STR__;                         \
    if (err_code != (__RC__)) {                          \
        WRNPRINT(#__CALL_STR__ ":%s : return from %s",   \
            __pal_str_err_code(err_code), __FUNCTION__); \
    }                                                    \
    err_code;                                        \
})

/**
 * Prints line to the log if function returns error in this line
 * end returns from calling function
 * @param __CALL_STR__ call of function with arguments
 * @return @see enum result_states
 * @note returns from calling function
 */
#define __RETURN_NE(__CALL_STR__,__RC__)                 \
{                                                        \
    int err_code = __CALL_STR__;                         \
    if (err_code != (__RC__)) {                          \
        WRNPRINT(#__CALL_STR__ ":%s : return from %s",   \
            __pal_str_err_code(err_code), __FUNCTION__); \
        return err_code;                                 \
    }                                                    \
}


__END_DECLS

#endif /* PAL_TEST_ENGINE_H_ */
