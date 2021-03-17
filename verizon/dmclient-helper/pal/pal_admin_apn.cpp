/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <condition_variable>
#include <cutils/log.h>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "omadm_service_api.h"
#include "pal.h"
#include "pal_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

using namespace android;
using namespace omadm_service_api;

static std::mutex mtx;
static std::condition_variable cv;
static long net_status = ADMIN_NET_UNKNOWN;

// wait-notify helper
bool is_network_available() {
    return (ADMIN_NET_UNKNOWN == net_status) ? false : true;
}

// admin network status changed callback
void on_net_status_changed(void* status)
{
    std::unique_lock<std::mutex> lck(mtx);
    net_status = (long)status;
    cv.notify_one();
    ALOGD("on_net_status_changed(): status = %d\n", net_status);
}

static omadmCallback network_status_cb = on_net_status_changed;

int request_admin_network(bool enable, omadmCallback admin_net_cb)
{
    int status = NO_ERROR;
    OMADM_SERVICE_INITIALIZED();

    if(enable) {
        status = pal_register_omadm_callback(OMADM_ADMIN_NETWORK_STATUS, admin_net_cb);
        if (status != NO_ERROR) {
            ALOGE("pal_register_omadm_callback(OMADM_ADMIN_NETWORK_STATUS): error %d", status);
            return status;
        }
    } else {
        status = pal_unregister_omadm_callback(OMADM_ADMIN_NETWORK_STATUS);
        if (status != NO_ERROR) {
            ALOGE("pal_unregister_omadm_callback(OMADM_ADMIN_NETWORK_STATUS): error %d", status);
        }
    }
    // returns true on success
    bool reply = BpOmadmServiceAPI::mBindrToService->requestAdminNetwork(enable);
    ALOGD("Request admin network (%d); reply = %d", enable, reply);
    return reply;
}

int pal_request_admin_network(bool enable)
{
    int ret = -1;
    ALOGD("pal_request_admin_network() enter");
    if (enable) {
        ret = request_admin_network(enable, network_status_cb);
        if( NO_ERROR == ret ) {
            ALOGD("pal_request_admin_network(true): return = %d\n", ret);
        } else {
            ALOGE("Exiting with error. pal_request_admin_network(false): return = %d\n", ret);
            return net_status;
        }
        // wait for network
        while(1) {
            std::unique_lock<std::mutex> lck(mtx);
            cv.wait(lck, is_network_available);
            if(is_network_available())
                break;
        }
    } else {
        ret = request_admin_network(enable, NULL);
        if( NO_ERROR == ret ) {
            ALOGD("pal_request_admin_network(false): return = %d\n", ret);
        } else {
            ALOGE("Exiting with error. pal_request_admin_network(false): return = %d\n", ret);
        }
        net_status = ADMIN_NET_UNKNOWN;
    }
    //if (ADMIN_NET_UNKNOWN != net_status) {
        // timeout in seconds to establish admin apn connection
        sleep(ADMIN_NET_SETUP_TIMEOUT);
    //}
    return net_status;
}
