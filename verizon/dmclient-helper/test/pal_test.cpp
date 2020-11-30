/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <condition_variable>
#include <dlfcn.h>
#include <mutex>
#include <private/android_filesystem_config.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <utils/Log.h>
#include <utils/Vector.h>
#include <utils/String16.h>

#include "omadm_event_handler.h"

#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_TEST"

#include "../pal/pal_internal.h"

using namespace android;

static std::mutex mtx;
static std::condition_variable cv;
static Vector<request_t> input_events_queue;

    /*********************************************************************
     * Helpers
     */

     // For test purpose only. It has to be replaced with an actual one
void put_request_to_queue(request_t * req) {
    std::unique_lock<std::mutex> lck(mtx);
    input_events_queue.push_front(*req);
    cv.notify_one();
    ALOGD("push request to queue(): tid: %d handler: %d", gettid(), req->handler);
}

    // wait-notify helper
bool is_events_available() {
    return !input_events_queue.isEmpty();
}

    // Allocates PAL data buffer
void getNewDataBuffer(data_buffer_t* pDBuffer, unsigned int size) {
    pDBuffer->data = new char[size];
    pDBuffer->size = size;
    memset(pDBuffer->data, 0, pDBuffer->size);
}

    // Releases PAL data buffer
void freeDataBuffer(data_buffer_t* pDBuffer) {
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        delete[] pDBuffer->data;
        pDBuffer->data == NULL;
        pDBuffer->size = 0;
    }
}

    // Clears content of PAL data bufer
void clearDataBuffer(data_buffer_t* pDBuffer) {
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        memset(pDBuffer->data, 0, pDBuffer->size);
    }
}

    // Wraps given string to PAL data buffer
void wrapDataBuffer(char* strToWrap, data_buffer_t* pDBuffer) {
    int size = strlen(strToWrap);
    pDBuffer->data = new char[size + 1];
    pDBuffer->size = size + 1;
    memset(pDBuffer->data, 0, pDBuffer->size);
    strncpy(pDBuffer->data, strToWrap, size);
}
    /**********************************************************************/

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

typedef int (*pal_init_t)(void);
static pal_init_t pal_init_fn = NULL;

    /*
     * Main Test routine
     */
int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    int reply = 0;
    data_buffer_t dataBuffer;
    // Initialize PAL. PAL opens framework service binder interface
    // and initializes event handler.
    void * pal_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (NULL != pal_handle) {
        pal_init_fn = (pal_init_t)dlsym(pal_handle,"pal_init");
        if (pal_init_fn != NULL) {
            if ( NO_ERROR != pal_init_fn()) {
                ALOGE("Could not initialize PAL");
                return 1;
            }
        } else {
            ALOGE("Could not find pal_init()");
            return 1;
        }

       /**
        * Start to PAL API TEST
        */

#if 0
        // Get Device ID (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_deviceID_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            ALOGE("pal_system_hostdevice_deviceID_get() <normal case> error: %d", reply);
        } else {
            ALOGD("pal_system_hostdevice_deviceID_get() <normal case> : %s",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get Device ID (NULL buffer)
        reply = pal_system_hostdevice_deviceID_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            ALOGE("pal_system_hostdevice_deviceID_get() <NULL buffer> error: %d", reply);
        } else {
            ALOGD("pal_system_hostdevice_deviceID_get() <NULL buffer> : %s",
                    dataBuffer.data);
        }
        reply = 0;

        // Get Device ID (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_deviceID_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            ALOGE("pal_system_hostdevice_deviceID_get() <small buffer> error: %d", reply);
        } else {
            ALOGD("pal_system_hostdevice_deviceID_get() <small buffer> : %s",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Send user notification
         */

        data_buffer_t titleBuffer;
        data_buffer_t messageBuffer;
        char title[] = "New updates available";
        char message[] = "Start download";
        wrapDataBuffer(title, &titleBuffer);
        wrapDataBuffer(message, &messageBuffer);

        reply = pal_device_volumes_notifications_set(&titleBuffer, &messageBuffer);
        if(RESULT_SUCCESS != reply) {
            ALOGE("pal_device_volumes_notifications_set()) error: %d", reply);
        }
        reply = 0;
        freeDataBuffer(&titleBuffer);
        freeDataBuffer(&messageBuffer);
#endif
        /*
         * Test Death recipient and Callbacks
         * Emulation of the main loop of the DM Client.
         * Lopos for some time.
         */

        //////////////////////////////////////////////////////////////////////
        //                OMADM EVENT HANDLER TEST SECTION                  //
        //////////////////////////////////////////////////////////////////////

        // Set callback notification. pal_init() shall be invoked before
        reply = omadm_event_handler_init();
        if(RESULT_SUCCESS != reply) {
            ALOGE("omadm_event_handler_init() error: %d", reply);
        } else {
            ALOGD("omadm_event_handler_init() success: %d", reply);
        }
        reply = 0;

        // Start main loop for a while...
        ALOGD("Start main loop for extracting requests from event queue. Wait..");
        for(int i=0; i<10; i++) {
            while(1) {
                {
                    std::unique_lock<std::mutex> lck(mtx);
                    if(!is_events_available()) break;
                    ALOGI("Extraction: queue size: %d",
                            input_events_queue.top().request,
                            input_events_queue.top().handler,
                            input_events_queue.size());

                    if (input_events_queue.top().handler == OMADM) {
                        omadm_request_t * omadm_req = (omadm_request_t *)input_events_queue.top().request;
                         ALOGI("Extracting OMADM request...");
                        if (omadm_req != NULL) {
                            if (omadm_req->package0 != NULL) {
                                ALOGI("Extracting wap push package0 event");
                                free(omadm_req->package0);
                            } else if (omadm_req->alert != NULL) {
                                ALOGI("Extracting MO alert event");
                                free(omadm_req->alert);
                            } else {
                                ALOGI("Extracting user system update request");
                            }
                            free(omadm_req);
                        }
                    }
                    input_events_queue.pop();
                }
                ALOGI("main loop is processing event... tid: %d", gettid());
                sleep(1);
            }
            ALOGI("** Waiting for new event(s)... **");
            std::unique_lock<std::mutex> lck(mtx);
            cv.wait(lck,is_events_available);
        }

        // Shutdown omadm pal
        ALOGD("Shutdown omadm service");
        reply = dlclose(pal_handle);
        if(NO_ERROR != reply) {
            ALOGE("dlclose() error: %d", reply);
        } else {
            ALOGD("pal_fini() success: %d", reply);
        }
        reply = 0;

        /*
        * End to check PAL API
        */
    }
    return 0;
}

