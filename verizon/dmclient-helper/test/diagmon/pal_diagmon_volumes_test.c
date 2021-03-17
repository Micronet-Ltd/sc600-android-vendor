/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pal.h>

#define VOLUME_LEVELS 5
#define VOLUME_LEVEL_SIZE 25
#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

const static char* __s_func_get_names[] = {
    "pal_device_volumes_ringtone_get",
    "pal_device_volumes_notifications_get",
    "pal_diagmon_sound_volumes_alarms_get",
    "pal_device_volumes_media_get",
    "pal_device_volumes_bluetooth_get"
};
const static char* __s_func_set_names[] = {
    "pal_device_volumes_ringtone_set",
    "pal_device_volumes_notifications_set",
    "pal_device_volumes_alarms_set",
    "pal_device_volumes_media_set",
    "pal_diagmon_sound_volumes_bluetooth_set"
};

static int __s_volume_level = 0;

typedef int (*pal_fn_volumes_get) (int*);
typedef int (*pal_fn_volumes_set) (int);

static void* __s_dl_handle = NULL;

int test_volumes_init() {
    int rc = 0;

    __s_dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (__s_dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        rc = -1;
    }
    return rc;
}

void test_volumes_close() {
    if (__s_dl_handle != NULL) {
        dlclose(__s_dl_handle);
        __s_dl_handle = NULL;
    }
}

int test_func_volumes_get() {
    int rc = -1;
    int idx = 0;
    int arr_get_size = 0;
    arr_get_size = sizeof(__s_func_get_names) / sizeof(char*);

    for(idx = 0; idx < arr_get_size; idx++) {
        pal_fn_volumes_get pal_function = dlsym(__s_dl_handle, __s_func_get_names[idx]);
        if (pal_function != NULL) {
            int ret_value = 0;
            rc = pal_function(&ret_value);
            printf("%s(&ret_value) = %d, *ret_value= %d\n", __s_func_get_names[idx], rc, ret_value);
        } else {
            printf("could not find function: %s\n", __s_func_get_names[idx]);
        }
    }

    return rc;
}

int test_func_volumes_set() {
    int rc = -1;
    int idx = 0;
    int arr_set_size = 0;
    arr_set_size = sizeof(__s_func_set_names) / sizeof(char*);

    for(idx = 0; idx < arr_set_size; idx++) {
        pal_fn_volumes_set pal_function = dlsym(__s_dl_handle, __s_func_set_names[idx]);
        if (pal_function != NULL) {
            int set_value = ((__s_volume_level + idx) % VOLUME_LEVELS) * VOLUME_LEVEL_SIZE;
            rc = pal_function(set_value);
            printf("%s(%d)) = %d\n", __s_func_set_names[idx], set_value, rc);
        } else {
            printf("could not find function: %s\n", __s_func_set_names[idx]);
        }
    }

    return rc;
}

int pal_diagmon_volumes_test()
{
    int ret = 0;
    int idx = 0;

    ret = test_volumes_init(); if (ret) return ret;
 
    for(idx = 0; idx < VOLUME_LEVELS; idx++) {

        printf("===============================\n");
        printf("Start volumes test %d\n", idx);
        printf("===============================\n");

        ret = test_func_volumes_get(); if (ret) break;
        ret = test_func_volumes_set(); if (ret) break;
        __s_volume_level++;
    }

    test_volumes_close();

    return ret;
}
