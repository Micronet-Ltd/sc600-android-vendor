/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pal.h>
#include <utils/Log.h>
#include <stdio.h>

#define PAL_MODULE "pal_diagmon"
#define CMD_SET "set"
#define CMD_GET "get"
#define SECTION_APN "apn"

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

int pal_diagmon_gps_test()
{
    printf("DiagMon GPS tests\n");

    int reply = 0;
    int value = 0;
    float fvalue = 0;
    // Enable
    reply = pal_device_gps_enable();
    if(RESULT_SUCCESS != reply) {
        printf("Enable GPS error: %d\n", reply);
    } else {
        printf("Enable GPS success\n");
    }
    reply = 0;
    sleep(1);

    // Get status
    reply = pal_device_gps_state_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get GPS status error: %d\n", reply);
    } else {
        printf("Get GPS status success: %d\n", value);
    }
    reply = 0;
    sleep(1);

    // Get snr_inviewsatellites
    reply = pal_device_gps_snr_inviewsatellites_get(&fvalue);
    if(RESULT_SUCCESS != reply) {
        printf("Get GPS snr error: %d\n", reply);
    } else {
        printf("Get GPS snr success: %f\n", fvalue);
    }
    reply = 0;
    sleep(1);

    // Get num_of_satellites
    reply = pal_device_gps_num_of_satellites_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get GPS satellites error: %d\n", reply);
    } else {
        printf("Get GPS satellites success: %d\n", value);
    }
    reply = 0;
    sleep(1);

    // Disable
    reply = pal_device_gps_disable();
    if(RESULT_SUCCESS != reply) {
        printf("Disable GPS error: %d\n", reply);
    } else {
        printf("Disable GPS success\n");
    }

    return 0;
}
