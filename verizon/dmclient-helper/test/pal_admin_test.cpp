/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <condition_variable>
#include <mutex>
#include "omadm_service_api.h"
#include "pal_internal.h"
#include "pal.h"

#define TIMEOUT 30  // in seconds
#define FILE_MAX_SIZE  8000

static std::mutex mtx;
static std::condition_variable cv;
static int net_status = ADMIN_NET_UNKNOWN;

int request_admin_network(bool enable, omadmCallback admin_net_cb);

// wait-notify helper
bool is_network_available() {
    return (ADMIN_NET_UNKNOWN == net_status) ? false : true;
}

// admin network status changed callback
void on_net_status_changed(void* status)
{
    std::unique_lock<std::mutex> lck(mtx);
    net_status = *(int*)status;
    cv.notify_one();
    printf("on_net_status_changed(): status = %d\n", net_status);
}

static omadmCallback network_status_cb = on_net_status_changed;

/**
 *  Test https connection.
 *  precondition: none
 */
int pal_network_test_https(void)
{
    char * server_uri = "https://ivzwmdmii.iot.motive.com/southbound-connector/dm";
    data_buffer_t in;
    data_buffer_t out;
    int status;
    pal_http_options_t options;
    options.timeout = 30;
    options.hdr_content_data = "Content-Type: application/vnd.syncml.dm+xml";
    options.username = NULL;
    options.password = NULL;
    options.auth_type = AUTH_DIGEST;
    options.hdr_auth_data = NULL;
    options.ca_fname = NULL;
    options.client_cert = NULL;
    options.client_key = NULL;
    memset(&in,0,sizeof(in));
    memset(&out,0,sizeof(out));
    in.data = (char*)malloc(FILE_MAX_SIZE);

    status = pal_send_http_post_request(server_uri, &options, &out, &in);

    if(out.data)
        free(out.data);
    if(in.data)
        free(in.data);
    return status;
}


int main(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    int ret = -1;

    // Test connection via specific network
    printf("------------------------------------------------------\n");
    if( 0 != pal_init()) {
        printf("Could not init PAL");
        return 1;
    }

    ret = -1;
    ret = request_admin_network(true, network_status_cb);
    if( NO_ERROR == ret ) {
        printf("request_admin_network(true): return = %d\n", ret);
    } else {
        printf("Exiting with error. request_admin_network(true): return = %d\n", ret);
        return 1;
    }

    // wait for network
    while(1) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck,is_network_available);
        if(is_network_available())
            break;
    }

    if(ADMIN_NET_AVAILABLE == net_status) {
        ret = -1;
        ret = pal_network_test_https();
        if ( ret == 405)
            printf("Test test_https success\n");
        else
            printf("Test test_https fail, return = %d\n",ret);
    } else {
        printf("Network is not available. Status %s\n", (ADMIN_NET_UNAVAILABLE == net_status) ? "ADMIN_NET_UNAVAILABLE" : "ADMIN_NET_LOST");
        return 0;
    }

    ret = -1;
    ret = request_admin_network(false, NULL);
    if( NO_ERROR == ret ) {
        printf("request_admin_network(false): return = %d\n", ret);
    } else {
        printf("Exiting with error. request_admin_network(false): return = %d\n", ret);
        return 1;
    }

    return 0;
}
