/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <utils/Log.h>

#include "omadm_service_api.h"
#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_BATTERY_TEST"

#include "../pal/pal_internal.h"

//using namespace android;

static std::mutex mtx;
static std::condition_variable cv;
static bool is_callback_executed = false;


// Omadm battery state Scomo plugin callback function
void batteryStateScomoCallback(pal_battery_state* bsp) {
    std::unique_lock<std::mutex> lck(mtx);
    ALOGD("batteryStateScomoCallback(): battery_status = %d, battery_level = %d",
        bsp->battery_status, bsp->battery_level);
    is_callback_executed = true;
    cv.notify_one();
}

// Omadm battery state Fumo plugin callback function
void batteryStateFumoCallback(pal_battery_state* bsp) {
    std::unique_lock<std::mutex> lck(mtx);
    ALOGD("batteryStateFumoCallback(): battery_status = %d, battery_level = %d",
        bsp->battery_status, bsp->battery_level);
    is_callback_executed = true;
    cv.notify_one();
}

bool is_battery_callback_available() {
    return is_callback_executed ? true : false;
}


int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    int status = NO_ERROR;

    // Initialize PAL interface
    if( 0 != pal_init()) {
        ALOGD("Could not init PAL");
        return 1;
    }

    omadmCallback battery_state_cb;
    // Register Omadm battery state callback for Scomo plugin
    battery_state_cb = (void (*)(void*)) batteryStateScomoCallback;
    status = pal_register_omadm_callback(OMADM_SCOMO_GET_BATTERY_STATE, battery_state_cb);
    if (status != NO_ERROR)
    {
        ALOGE("pal_register_omadm_callback() type->OMADM_SCOMO_GET_BATTERY_STATE, error: %d", status);
        return 1;
    }


    // Register Omadm battery state callback for Fumo plugin
    battery_state_cb = (void (*)(void*)) batteryStateFumoCallback;
    status = pal_register_omadm_callback(OMADM_FUMO_GET_BATTERY_STATE, battery_state_cb);
    if (status != NO_ERROR)
    {
        ALOGE("pal_register_omadm_callback() type->OMADM_FUMO_GET_BATTERY_STATE, error: %d", status);
        return 1;
    }

    // infinite wait battery state updates
    while(1) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, is_battery_callback_available);
        if(is_battery_callback_available()) {
            ALOGD("Battery state was changed");
            is_callback_executed = false;
        }
    }

    return 0;
}