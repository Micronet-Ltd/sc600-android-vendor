/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../pal/pal_internal.h"
#include "./pal_test_engine.h"

static int pal_data_buffer_realloc(data_buffer_t* iobuff)
{
    FREE_IF(iobuff->data);
    if (iobuff->size > 0) {
        iobuff->data = calloc(iobuff->size, sizeof(char));
        if (iobuff->data == NULL) {
            return RESULT_MEMORY_ERROR;
        }
    }
    return RESULT_SUCCESS;
}

static int pal_data_buffer_release_data(data_buffer_t* iobuff)
{
  iobuff->size = 0;
  return pal_data_buffer_realloc(iobuff);
}

int __handle_pal_func_description(
    pal_func_description_t* desc, int argc, char** argv)
{
    void* dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        return -1;
    }
    int rc = -1;
    int arg0int = 0;
    char* arg0pc = NULL;
    if (argc > 1) {
        char* last_valid = NULL;
        arg0int = strtol(argv[1], &last_valid, 10);
        if (NULL != last_valid) {
            arg0int = 0;
        }
        arg0pc = argv[1];
    }
    if (arg0int < 0) {
        arg0int = 0;
    }
    if ((argc < 1) && (desc->key) != NULL) {
        /* print possible keys if key is not sent */
        printf("key:%s\n", desc->key);
    }
    data_buffer_t obuff = {
        MAX_BUFFER_SIZE,
        NULL
    };
    pal_data_buffer_realloc(&obuff);
    struct timespec time_start;
    struct timespec time_end;
    if(clock_gettime(CLOCK_MONOTONIC, &time_start)) {
        printf("clock_gettime(CLOCK_MONOTONIC, &time_start):ERROR\n");
    }
    switch(desc->args_types) {
        case __PAL_ARGS_I_S: {
            pal_int_int_pbuff_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                rc = pal_data_buffer_realloc(&obuff);
                if (RESULT_MEMORY_ERROR == rc) {
                    break;
                }
                rc = pal_fn(arg0int, &obuff);
                while (rc == RESULT_BUFFER_OVERFLOW) {
                    rc = pal_data_buffer_realloc(&obuff);
                    if (RESULT_MEMORY_ERROR == rc) {
                        break;
                    }
                    rc = pal_fn(arg0int, &obuff);
                }
                printf("%s(%d, &{%d, \"%s\"}) = %d\n",
                    desc->function, arg0int, obuff.size, obuff.data, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_I_PR4: {
            pal_int_int_pfloat_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                float val = 0;
                rc = pal_fn(arg0int, &val);
                printf("%s(%d, &%f) = %d\n",
                    desc->function, arg0int, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_I_PI4: {
            pal_int_int_pint32_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                int32_t val = 0;
                rc = pal_fn(arg0int, &val);
                printf("%s(%d, &%d) = %d\n",
                    desc->function, arg0int, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PC_PR4: {
            pal_int_pc_pfloat_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                float val = 0;
                rc = pal_fn(arg0pc, &val);
                printf("%s(%s, &%f) = %d\n",
                    desc->function, arg0pc, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PC_PI4: {
            pal_int_pc_pint32_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                int32_t val = 0;
                rc = pal_fn(arg0pc, &val);
                printf("%s(%s, &%d) = %d\n",
                    desc->function, arg0pc, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PC_PI: {
            pal_int_pc_pint_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                int val = 0;
                rc = pal_fn(arg0pc, &val);
                printf("%s(%s, &%d) = %d\n",
                    desc->function, arg0pc, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PI4: {
            pal_int_pint32_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                int32_t val = 0;
                rc = pal_fn(&val);
                printf("%s(&%d) = %d\n",
                    desc->function, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_I: {
            pal_int_int_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                rc = pal_fn(arg0int);
                printf("%s(%d) = %d\n",
                    desc->function, arg0int, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PI: {
            pal_int_pint_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                int val = 0;
                rc = pal_fn(&val);
                printf("%s(&%d) = %d\n",
                    desc->function, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_S: {
            pal_int_pbuff_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                rc = pal_data_buffer_realloc(&obuff);
                if (RESULT_MEMORY_ERROR == rc) {
                    break;
                }
                rc = pal_fn(&obuff);
                while (rc == RESULT_BUFFER_OVERFLOW) {
                    rc = pal_data_buffer_realloc(&obuff);
                    if (RESULT_MEMORY_ERROR == rc) {
                        break;
                    }
                    rc = pal_fn(&obuff);
                }
                printf("%s(&{%d, \"%s\"}) = %d\n",
                    desc->function, obuff.size, obuff.data, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_V: {
            pal_int_void pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                rc = pal_fn();
                printf("%s() = %d\n",
                    desc->function, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_PR4: {
            pal_int_pr4_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                float val;
                rc = pal_fn(&val);
                printf("%s(&%f) = %d\n",
                    desc->function, val, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        case __PAL_ARGS_FUMODLG: {
            pal_int_dlgui_fn pal_fn = dlsym(dl_handle, desc->function);
            if (pal_fn != NULL) {
                rc = pal_fumo_dlg_test(pal_fn, argc - 1, argv + 1);
                printf("%s(&dlg) = %d\n",
                    desc->function, rc);
            } else {
                printf("could not find function: %s\n", desc->function);
            }
            break;
        }
        default: {
            printf("unknown type %d function %s:\n", desc->args_types,
                desc->function);
            break;
        }
    }
    if(clock_gettime(CLOCK_MONOTONIC, &time_end)) {
        printf("clock_gettime(CLOCK_MONOTONIC, &time_end):ERROR\n");
    }
    time_end.tv_sec -= time_start.tv_sec;
    time_end.tv_nsec += time_end.tv_sec * 1000000000;
    time_end.tv_nsec -= time_start.tv_nsec;
    printf("duration : %ld s, %ld ns\n", time_end.tv_sec, time_end.tv_nsec);
    pal_data_buffer_release_data(&obuff);
    dlclose(dl_handle);
    return rc;
}

static void __printf_keys(pal_func_description_t* desc) {
    printf("possible keys:\n");
    do {
        if (desc->key != NULL) {
            printf("  key:%15s => function:%s\n", desc->key, desc->function);
        }
        desc++;
    } while(desc->key != NULL);
}

int pal_test_subsection(int argc, char** argv, pal_func_description_t* desc)
{
    printf("(%d, argv)\n", argc);
    int handled = 0;
    int idx = 0;
    for (idx = 0; idx < argc; idx ++) {
        printf("argv[%d] = %s\n", idx, argv[idx]);
    }
    pal_func_description_t* item = desc;
    do {
        if ((argc < 1) ||
            (item->key != NULL && (strcmp(item->key, argv[0]) == 0))
            ) {
            __handle_pal_func_description(item, argc, argv);
            handled = 1;
        }
        item++;
    } while(item->key != NULL);
    if (!handled) {
        __printf_keys(desc);
    }
    return handled;
}

int __pthread_sem_init(__pthread_sem_t* thiz)
{
    pthread_mutexattr_t mtx_attrs;
    __RETURN_NE(pthread_mutexattr_settype(&mtx_attrs, PTHREAD_MUTEX_RECURSIVE), 0);
    __RETURN_NE(pthread_mutex_init(&(thiz->mtx), &mtx_attrs), 0);
    pthread_condattr_t cv_attrs;
    __RETURN_NE(pthread_condattr_init(&cv_attrs), 0);
    __RETURN_NE(pthread_cond_init(&(thiz->cv), &cv_attrs), 0);
    return 0;
}

int __pthread_sem_deinit(__pthread_sem_t* thiz)
{
    int rc = 0;
    rc = __WRN_NE(pthread_mutex_destroy(&(thiz->mtx)), 0);
    rc = __WRN_NE(pthread_cond_destroy(&(thiz->cv)), 0);
    return rc;
}

int __pthread_sem_wait(__pthread_sem_t* thiz)
{
    int rc = 0;
    __RETURN_NE(pthread_mutex_lock(&(thiz->mtx)), 0);
    rc = __WRN_NE(pthread_cond_wait(&(thiz->cv), &(thiz->mtx)), 0);
    __WRN_NE(pthread_mutex_unlock(&(thiz->mtx)), 0);
    return rc;
}

int __pthread_sem_timedwait(__pthread_sem_t* thiz, struct timespec* tv)
{
    int rc = 0;
    __RETURN_NE(pthread_mutex_lock(&(thiz->mtx)), 0);
    rc = __WRN_NE(pthread_cond_timedwait(&(thiz->cv), &(thiz->mtx), tv), 0);
    __WRN_NE(pthread_mutex_unlock(&(thiz->mtx)), 0);
    return rc;
}

int __pthread_sem_notify(__pthread_sem_t* thiz)
{
    int rc = 0;
    __RETURN_NE(pthread_mutex_lock(&(thiz->mtx)), 0);
    rc = __WRN_NE(pthread_cond_signal(&(thiz->cv)), 0);
    __WRN_NE(pthread_mutex_unlock(&(thiz->mtx)), 0);
    return rc;
}


int __pthread_sem_notify_all(__pthread_sem_t* thiz)
{
    int rc = 0;
    __RETURN_NE(pthread_mutex_lock(&(thiz->mtx)), 0);
    rc = __WRN_NE(pthread_cond_broadcast(&(thiz->cv)), 0);
    __WRN_NE(pthread_mutex_unlock(&(thiz->mtx)), 0);
    return rc;
}
