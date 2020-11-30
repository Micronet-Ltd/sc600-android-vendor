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
#include "pal_diagmon_test.h"

#define MAX_DATA_SIZE 127

void pal_diagmon_network_test_network_mode_set(void)
{
    int reply = 0;

    // GlobalMode
    reply = pal_network_preferred_network_mode_set(0);
    if(RESULT_SUCCESS != reply) {
        printf("Set network mode Global error: %d\n", reply);
    } else {
        printf("Set network mode Global success\n");
    }
    reply = 0;
    sleep(1);

    // LTE_CDMA_Mode
    reply = pal_network_preferred_network_mode_set(1);
    if(RESULT_SUCCESS != reply) {
        printf("Set network mode LTE/CDMA error: %d\n", reply);
    } else {
        printf("Set network mode LTE/CDMA success\n");
    }
    reply = 0;
    sleep(1);

    // GSM_UMTS_Mode
    reply = pal_network_preferred_network_mode_set(2);
    if(RESULT_SUCCESS != reply) {
        printf("Set network mode GSM/UMTS error: %d\n", reply);
    } else {
        printf("Set network mode GSM/UMTS success\n");
    }
    reply = 0;
    sleep(1);

    // Invalid input
    reply = pal_network_preferred_network_mode_set(3);
    if(RESULT_SUCCESS != reply) {
        printf("Set network mode <invalid_input> error: %d\n", reply);
    } else {
        printf("Set network mode <invalid_input> success\n");
    }
    reply = 0;
}

void pal_diagmon_network_test_mobile_data(void)
{
    int reply = 0;

    // Enable
    reply = pal_network_mobiledata_state_set(1);
    if(RESULT_SUCCESS != reply) {
        printf("Enable mobile data error: %d\n", reply);
    } else {
        printf("Enable mobile data success\n");
    }
    reply = 0;
    sleep(1);

    // Disable
    reply = pal_network_mobiledata_state_set(0);
    if(RESULT_SUCCESS != reply) {
        printf("Disable mobile data error: %d\n", reply);
    } else {
        printf("Disable mobile data success\n");
    }
    reply = 0;
    sleep(1);

    // Invalid input
    reply = pal_network_mobiledata_state_set(2);
    if(RESULT_SUCCESS != reply) {
        printf("Mobile data <invalid_input> error: %d\n", reply);
    } else {
        printf("Mobile data <invalid_input> success\n");
    }
    reply = 0;
}

void pal_diagmon_network_test_base_station_id_get(void)
{
    int reply = 0;
    int value = 0;

    reply = pal_network_basestationid_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get base station id error: %d\n", reply);
    } else {
        printf("Get base station id success: %d\n", value);
    }
}

void pal_diagmon_network_test_system_id_get(void)
{
    int reply = 0;
    int value = 0;

    reply = pal_network_systemid_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get system id error: %d\n", reply);
    } else {
        printf("Get system id success: %d\n", value);
    }
}

void pal_diagmon_network_test_mcc_get(void)
{
    int reply = 0;
    int value = 0;

    data_buffer_t bf;
    bf.size = 4;
    bf.data = calloc(bf.size, sizeof(char));
    reply = pal_network_mcc_get(&bf);
    if(RESULT_SUCCESS != reply) {
        printf("Get mcc error: %d\n", reply);
    } else {
        printf("Get mcc success: %s\n", bf.data);
    }
    free(bf.data);
}

void pal_diagmon_network_test_network_id_get(void)
{
    int reply = 0;
    int value = 0;

    reply = pal_network_networkid_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get network id error: %d\n", reply);
    } else {
        printf("Get network id success: %d\n", value);
    }
}

void pal_diagmon_network_test_supported_modes_get(void)
{
    int reply = 0;
    data_buffer_t dataBuffer;

    // (normal case)
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_network_supported_modes_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("Get supported network modes <normal case> error: %d\n", reply);
    } else {
        printf("Get supported network modes <normal case> success: %s\n", dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;

    // (NULL buffer)
    reply = pal_network_supported_modes_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("Get supported network modes <NULL buffer> error: %d\n", reply);
    } else {
        printf("Get supported network modes <NULL buffer> success: %s\n", dataBuffer.data);
    }
    reply = 0;

    // (small buffer)
    getNewDataBuffer(&dataBuffer, 1);
    reply = pal_network_supported_modes_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("Get supported network modes <small buffer> error: %d\n", reply);
    } else {
        printf("Get supported network modes <small buffer> success: %s\n", dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
}

void pal_diagmon_network_test_preferred_mode_get(void)
{
    int reply = 0;
    int value = 0;

    reply = pal_network_preferred_network_mode_get(&value);
    if(RESULT_SUCCESS != reply) {
        printf("Get preferred network mode error: %d\n", reply);
    } else {
        printf("Get preferred network mode success: %d\n", value);
    }
}

static pal_func_description_t __network_funcs[] = {
    {"current.voice", "pal_network_curr_voice_get", __PAL_ARGS_S, ""},
    {"current.data", "pal_network_curr_data_get", __PAL_ARGS_S, ""},
    {"homeroam", "pal_network_homeroam_get", __PAL_ARGS_S, ""},
    {"mobiledata.get", "pal_network_mobiledata_state_get", __PAL_ARGS_PI4, ""},
    {"1xsignal", "pal_network_1xsignal_get", __PAL_ARGS_PR4, ""},
    {"4gsignal", "pal_network_4gsignal_get", __PAL_ARGS_PR4, ""},
    {"mnc", "pal_network_mnc_get", __PAL_ARGS_S, ""},
    {"mcc", "pal_network_mcc_get", __PAL_ARGS_S, ""},
    {"3gsignal", "pal_network_3gsignal_get", __PAL_ARGS_PR4, ""},
    {"currentnetwork", "pal_network_currentnetwork_get", __PAL_ARGS_PI4, ""},
    {"callstate", "pal_network_callstate_get", __PAL_ARGS_PI4, ""},
    {"connecttype", "pal_network_connecttype_get", __PAL_ARGS_PI4, ""},
    {"countryiso", "pal_network_network_countryiso_get", __PAL_ARGS_S, ""},
    {"simstate", "pal_network_simstate_get", __PAL_ARGS_PI4, ""},
    {"data.roaming.get", "pal_network_global_data_roaming_access_get",
        __PAL_ARGS_PI4, ""},
    {"data.roaming.set", "pal_network_global_data_roaming_access_set",
        __PAL_ARGS_I, ""},
    {"mobiledata.set", "pal_network_mobiledata_state_set", __PAL_ARGS_I, ""},
    {"bid", "pal_network_basestationid_get", __PAL_ARGS_PI4, ""},
    {"sid", "pal_network_systemid_get", __PAL_ARGS_PI4, ""},
    {"nid", "pal_network_networkid_get", __PAL_ARGS_PI4, ""},
    {"pref.modes", "pal_network_supported_modes_get", __PAL_ARGS_S, ""},
    {"pref.mode.get", "pal_network_preferred_network_mode_get", __PAL_ARGS_PI4, ""},
    {"pref.mode.set", "pal_network_preferred_network_mode_set", __PAL_ARGS_I, ""},
    {NULL, NULL, -1, NULL},
};

int pal_diagmon_network_test_1(int argc, char** argv)
{
    printf("(%d, argv)\n", argc );
    return pal_test_subsection(argc, argv, __network_funcs);
}

int pal_diagmon_network_test()
{
    printf("DiagMon Network tests\n");
    pal_diagmon_network_test_network_mode_set();
    pal_diagmon_network_test_mobile_data();
    pal_diagmon_network_test_base_station_id_get();
    pal_diagmon_network_test_system_id_get();
    pal_diagmon_network_test_mcc_get();
    pal_diagmon_network_test_network_id_get();
    pal_diagmon_network_test_supported_modes_get();
    pal_diagmon_network_test_preferred_mode_get();
  return 0;
}
