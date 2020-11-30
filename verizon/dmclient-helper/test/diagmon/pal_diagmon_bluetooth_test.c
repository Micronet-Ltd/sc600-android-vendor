/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include <pal.h>
#include <stdio.h>
#include "pal_diagmon_test.h"

#define MAX_DATA_SIZE 127

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

int pal_diagmon_bluetooth_test(const char* command)
{
    int reply = 0;
    int value = 0;
    data_buffer_t dataBuffer;

    if (command == NULL) {
        printf("Type \"help\" to find list of supported bluetooth commands\n");
    } else if (strcmp(command, "help") == 0) {
        printf("Bluetooth commands:\n");
        printf("\"on\" - enable bluetooth\n");
        printf("\"off\" - disable bluetooth\n");
        printf("\"disc_on\" - start discovering\n");
        printf("\"disc_off\" - cancel discovering\n");
        printf("\"name\" - bluetooth name\n");
        printf("\"state\" - bluetooth enabling state\n");
        printf("\"disc_state\" - bluetooth discoverable state\n");
        printf("\"hdp_state\" - bluetooth hdp connection state\n");
        printf("\"a2dp_state\" - bluetooth a2dp connection state\n");
        printf("\"hsp_state\" - bluetooth hsp connection state\n");
    } else if (strcmp(command, "on") == 0) {
        reply = pal_device_bluetooth_enable();
        if (RESULT_SUCCESS != reply) {
            printf("Enable bluetooth error: %d\n", reply);
        } else {
            printf("Enable bluetooth success\n");
        }
    } else if (strcmp(command, "off") == 0) {
        reply = pal_device_bluetooth_disable();
        if (RESULT_SUCCESS != reply) {
            printf("Disable bluetooth error: %d\n", reply);
        } else {
            printf("Disable bluetooth success\n");
        }
    } else if (strcmp(command, "disc_on") == 0) {
        reply = pal_device_bluetooth_discoverable_enable();
        if (RESULT_SUCCESS != reply) {
            printf("Enable bluetooth discovering error: %d\n", reply);
        } else {
            printf("Enable bluetooth discovering success\n");
        }
    } else if (strcmp(command, "disc_off") == 0) {
        reply = pal_device_bluetooth_discoverable_disable();
        if (RESULT_SUCCESS != reply) {
            printf("Disable bluetooth discovering error: %d\n", reply);
        } else {
            printf("Disable bluetooth discovering success\n");
        }
    } else if (strcmp(command, "name") == 0) {
        // (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_device_bluetooth_name_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("Get bluetooth name <normal case> error: %d\n", reply);
        } else {
            printf("Get bluetooth name <normal case> success: %s\n", dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // (NULL buffer)
        reply = pal_device_bluetooth_name_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("Get bluetooth name <NULL buffer> error: %d\n", reply);
        } else {
            printf("Get bluetooth name <NULL buffer> success: %s\n", dataBuffer.data);
        }
        reply = 0;

        // (small buffer)
        getNewDataBuffer(&dataBuffer, 1);
        reply = pal_device_bluetooth_name_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("Get bluetooth name <small buffer> error: %d\n", reply);
        } else {
            printf("Get bluetooth name <small buffer> success: %s\n", dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;
    } else if (strcmp(command, "state") == 0) {
        reply = pal_device_bluetooth_state_get(&value);
        if (RESULT_SUCCESS != reply) {
            printf("Get bluetooth state error: %d\n", reply);
        } else {
            printf("Get bluetooth state: %d\n", value);
        }
    } else if (strcmp(command, "disc_state") == 0) {
        reply = pal_device_bluetooth_discoverable_state_get(&value);
        if (RESULT_SUCCESS != reply) {
            printf("Get bluetooth discoverable state error: %d\n", reply);
        } else {
            printf("Get bluetooth discoverable state: %d\n", value);
        }
    } else if (strcmp(command, "hdp_state") == 0) {
        reply = pal_device_bluetooth_hdp_get(&value);
        if (RESULT_SUCCESS != reply) {
            printf("Get bluetooth hdp connection state error: %d\n", reply);
        } else {
            printf("Get bluetooth hdp connection state: %d\n", value);
        }
    } else if (strcmp(command, "a2dp_state") == 0) {
        reply = pal_device_bluetooth_a2dpstate_get(&value);
        if (RESULT_SUCCESS != reply) {
            printf("Get bluetooth a2dp connection state error: %d\n", reply);
        } else {
            printf("Get bluetooth a2dp connection state: %d\n", value);
        }
    } else if (strcmp(command, "hsp_state") == 0) {
        reply = pal_device_bluetooth_hspstate_get(&value);
        if (RESULT_SUCCESS != reply) {
            printf("Get bluetooth hsp connection state error: %d\n", reply);
        } else {
            printf("Get bluetooth hsp connection state: %d\n", value);
        }
    } else {
        printf("Unknown command. Type \"help\" to find list of supported bluetooth commands\n");
    }
    return 0;
}
