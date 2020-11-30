/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pal.h>

#define MAX_DATA_SIZE 1024

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

const static char* __s_wifi_func_get_int_names[] = {
    "pal_network_wifi_state_get",
    "pal_network_wifi_speed_get",
    "pal_network_wifi_status_get",
    "pal_network_wifi_hotspot_state_get",
    "pal_network_wifi_signal_get"
};

const static char* __s_wifi_func_get_float_names[] = {
};

const static char* __s_wifi_func_get_buf_names[] = {
    "pal_network_wifi_networks_get",
    "pal_network_wifi_ssid_get",
    "pal_system_wifi_mac_get",
    "pal_network_wifi_bssid_get"
};

const static char* __s_wifi_func_exec_enable = "pal_device_wifi_enable";
const static char* __s_wifi_func_exec_disable = "pal_device_wifi_disable";
const static char* __s_wifi_func_exec_hotspot_disable = "pal_device_wifi_hotspot_disable";

typedef int (*pal_fn_int) (int*);
typedef int (*pal_fn_float) (float*);
typedef int (*pal_fn_buf) (data_buffer_t*);
typedef int (*pal_fn_exec) ();

int test_func_get() {
    int idx = 0;
    int arr_int_size = 0;
    int arr_float_size = 0;
    int arr_buf_size = 0;
    arr_int_size = sizeof(__s_wifi_func_get_int_names) / sizeof(char*);
    arr_float_size = sizeof(__s_wifi_func_get_float_names) / sizeof(char*);
    arr_buf_size = sizeof(__s_wifi_func_get_buf_names) / sizeof(char*);
   int rc = -1;

    void* dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        return -1;
    }

    for(idx = 0; idx < arr_buf_size; idx++) {
        pal_fn_buf pal_function = dlsym(dl_handle, __s_wifi_func_get_buf_names[idx]);
        if (pal_function != NULL) {
            char out_str[MAX_DATA_SIZE];
            memset(out_str, 0, MAX_DATA_SIZE * sizeof(char));
            data_buffer_t obuff = {
                .data = out_str,
                .size = MAX_DATA_SIZE
            };
            rc = pal_function(&obuff);
            printf("%s(&{%d, \"%s\"}) = %d\n", __s_wifi_func_get_buf_names[idx], obuff.size, obuff.data, rc);
        } else {
            printf("could not find function: %s\n", __s_wifi_func_get_buf_names[idx]);
        }
    }

    for(idx = 0; idx < arr_int_size; idx++) {
        pal_fn_int pal_function = dlsym(dl_handle, __s_wifi_func_get_int_names[idx]);
        if (pal_function != NULL) {
            int ret_value = 0;
            rc = pal_function(&ret_value);
            printf("%s(&ret_value)) = %d, *ret_value= %d\n", __s_wifi_func_get_int_names[idx], rc, ret_value);
        } else {
            printf("could not find function: %s\n", __s_wifi_func_get_int_names[idx]);
        }
    }

    for(idx = 0; idx < arr_float_size; idx++) {
        pal_fn_float pal_function = dlsym(dl_handle, __s_wifi_func_get_float_names[idx]);
        if (pal_function != NULL) {
            float ret_value = 0;
            rc = pal_function(&ret_value);
            printf("%s(&ret_value)) = %d, *ret_value= %f\n", __s_wifi_func_get_float_names[idx], rc, ret_value);
        } else {
            printf("could not find function: %s\n", __s_wifi_func_get_float_names[idx]);
        }
    }

    if (dl_handle != NULL) {
        dlclose(dl_handle);
        dl_handle = NULL;
    }
    return rc;
}

int test_func_exec(const char* func_name) {
    int rc = -1;

    void* dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        return -1;
    }

    pal_fn_exec pal_function = dlsym(dl_handle, func_name);
    if (pal_function != NULL) {
        rc = pal_function();
        printf("%s() return %d\n", func_name, rc);
    } else {
        printf("could not find function: %s\n", func_name);
    }

    if (dl_handle != NULL) {
        dlclose(dl_handle);
        dl_handle = NULL;
    }
    return rc;
}


int pal_diagmon_wifi_test()
{
    printf("===============================\n");
    printf("Start WiFi enabled 1\n");
    printf("===============================\n");
    int ret = test_func_exec(__s_wifi_func_exec_enable);
    ret = test_func_get();
    printf("===============================\n");
    printf("Start WiFi disabled 1\n");
    printf("===============================\n");
    ret = test_func_exec(__s_wifi_func_exec_disable);
    ret = test_func_get();
    printf("===============================\n");
    printf("Start WiFi enabled 2\n");
    printf("===============================\n");
    ret = test_func_exec(__s_wifi_func_exec_enable);
    ret = test_func_get();
    printf("===============================\n");
    printf("Start WiFi Hotspot disabled 1\n");
    printf("===============================\n");
    ret = test_func_exec(__s_wifi_func_exec_hotspot_disable);
    ret = test_func_get();
    printf("===============================\n");
    printf("Start WiFi disabled 2\n");
    printf("===============================\n");
    ret = test_func_exec(__s_wifi_func_exec_disable);
    ret = test_func_get();

    return ret;
}
