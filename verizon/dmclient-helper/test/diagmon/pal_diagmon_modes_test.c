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

#define MAX_DATA_SIZE 127
#define PAL_MODULE "pal_network"
#define CMD_SET "set"
#define CMD_GET "get"
#define SECTION_APN "apn"

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

int pal_diagmon_nw_modes_test()
{
    printf("DiagMon Modes tests\n");
    int reply = 0;

    // Set global mode (normal case)
    reply = pal_network_globalmode_enable();
    if(RESULT_SUCCESS != reply) {
        printf("pal_network_globalmode_enable() <normal case> error: %d\n", reply);
    } else {
        printf("pal_network_globalmode_enable() <normal case>\n");
    }
    reply = 0;
    sleep(1);

    // Set LTE/CDMA mode (normal case)
    reply = pal_network_lte_cdma_mode_enable();
    if(RESULT_SUCCESS != reply) {
        printf("pal_network_lte_cdma_mode_enable() <normal case> error: %d\n", reply);
    } else {
        printf("pal_network_lte_cdma_mode_enable() <normal case>\n");
    }
    reply = 0;
    sleep(1);

    // Set GSM/UMTS mode (normal case)
    reply = pal_network_gsm_umts_mode_enable();
    if(RESULT_SUCCESS != reply) {
        printf("pal_network_gsm_umts_mode_enable() <normal case> error: %d\n", reply);
    } else {
        printf("pal_network_gsm_umts_mode_enable() <normal case>\n");
    }
    reply = 0;

  return 0;
}
