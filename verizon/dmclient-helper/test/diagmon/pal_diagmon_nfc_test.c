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

#define PAL_MODULE "pal_network"
#define CMD_SET "set"
#define CMD_GET "get"
#define SECTION_APN "apn"

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

int pal_diagmon_nfc_test()
{
    printf("DiagMon NFC tests\n");

    int reply = 0;
    int value = 0;

    // Enable
    reply = pal_device_nfc_enable();
    if(RESULT_SUCCESS != reply) {
        printf("Enable NFC error: %d\n", reply);
    } else {
        printf("Enable NFC success\n");
    }
    reply = 0;
    sleep(1);

    // Disable
    reply = pal_device_nfc_disable();
    if(RESULT_SUCCESS != reply) {
        printf("Disable NFC error: %d\n", reply);
    } else {
        printf("Disable NFC success\n");
    }
    reply = 0;
    sleep(1);

    // Get status
    reply = pal_device_nfc_state_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get NFC status error: %d\n", reply);
    } else {
        printf("Get NFC status success: %d\n", value);
    }
    return 0;
}
