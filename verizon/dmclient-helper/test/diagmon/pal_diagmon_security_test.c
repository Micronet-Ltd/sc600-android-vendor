/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pal.h>

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

static void* __s_dl_handle = NULL;

const static char* __s_func_encryption_get_name = "pal_device_encryption_state_get";
const static char* __s_func_verifyapp_get_name = "pal_device_verifyapp_state_get";
const static char* __s_func_verifyapp_enable_name = "pal_device_verifyapp_enable";

typedef int (*pal_fn_int) (int*);
typedef int (*pal_fn_exec) ();

int test_security_init() {
    int rc = 0;

    __s_dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (__s_dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        rc = -1;
    }
    return rc;
}

void test_security_close() {
    if (__s_dl_handle != NULL) {
        dlclose(__s_dl_handle);
        __s_dl_handle = NULL;
    }
}

int test_func_encription() {
    int rc = -1;

    pal_fn_int pal_function = dlsym(__s_dl_handle, __s_func_encryption_get_name);
    if (pal_function != NULL) {
        int ret_value = 0;
        rc = pal_function(&ret_value);
        printf("%s(&ret_value) = %d, *ret_value= %d\n", __s_func_encryption_get_name, rc, ret_value);
    } else {
        printf("could not find function: %s\n", __s_func_encryption_get_name);
    }

    return rc;
}
int test_func_verifyapp_state() {
    int rc = -1;

    pal_fn_int pal_function = dlsym(__s_dl_handle, __s_func_verifyapp_get_name);
    if (pal_function != NULL) {
        int ret_value = 0;
        rc = pal_function(&ret_value);
        printf("%s(&ret_value) = %d, *ret_value= %d\n", __s_func_verifyapp_get_name, rc, ret_value);
    } else {
        printf("could not find function: %s\n", __s_func_verifyapp_get_name);
    }

    return rc;
}

int test_func_verifyapp_enable() {
    int rc = -1;

    pal_fn_exec pal_function = dlsym(__s_dl_handle, __s_func_verifyapp_enable_name);
    if (pal_function != NULL) {
        rc = pal_function();
        printf("%s() return %d\n", __s_func_verifyapp_enable_name, rc);
    } else {
        printf("could not find function: %s\n", __s_func_verifyapp_enable_name);
    }

    return rc;
}


int pal_diagmon_security_test()
{
    int ret = 0;

    ret = test_security_init(); if (ret) return ret;
 
    printf("===============================\n");
    printf("Start security test\n");
    printf("===============================\n");

    ret = test_func_encription();
    ret = test_func_verifyapp_state();
    ret = test_func_verifyapp_enable();
    ret = test_func_verifyapp_state();

    test_security_close();

    return ret;
}
