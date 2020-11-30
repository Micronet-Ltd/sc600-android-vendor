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
#define LOG_TAG "PAL_CHECK_TEST"

#include "../pal/pal_internal.h"

//using namespace android;

static std::mutex mtx;
static std::condition_variable cv;
static bool is_callback_executed = false;

void checkButtonStateFumoCallback() {
    std::unique_lock<std::mutex> lck(mtx);
    is_callback_executed = true;
    cv.notify_one();
}

bool is_check_button_callback_available() {
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

    omadmCallback button_state;
    button_state = (void (*)(void*)) checkButtonStateFumoCallback;
    status = pal_register_omadm_callback(OMADM_FUMO_CHECK_FOR_UPDATE, button_state);
    if (status != NO_ERROR)
    {
        ALOGE("pal_register_omadm_callback() type->OMADM_FUMO_CHECK_FOR_UPDATE, error: %d", status);
        return 1;
    }

    while(1) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, is_check_button_callback_available);
        if(is_check_button_callback_available()) {
            ALOGD("Button was pressed");
            is_callback_executed = false;
        }
    }

    return 0;
}