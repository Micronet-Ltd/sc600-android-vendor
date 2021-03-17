/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/Log.h>

#include "omadm_event_handler.h"
//#include "omadm_queue.h"  // uncomment when queue API will provided

typedef int (*pal_notify_on_omadm_event_t)(process_omadm_event_t);
static pal_notify_on_omadm_event_t pal_notify_on_omadm_event_fn = NULL;

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

int omadm_event_handler_init()
{
    int status = RESULT_ERROR;
    void * pal_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (NULL != pal_handle) {
        ALOGD("omadm_event_handler_init() library found");
        pal_notify_on_omadm_event_fn = (pal_notify_on_omadm_event_t)dlsym(pal_handle,
                PAL_NOTIFY_ON_OMADM_EVENT);
        if (pal_notify_on_omadm_event_fn != NULL) {
            ALOGD("omadm_event_handler_init() function is found");
            status = pal_notify_on_omadm_event_fn(
                    &omadm_event_handler_process_event);
        } else {
            ALOGE("omadm_event_handler_init() error %s", dlerror());
        }
    }
    return status;
}

int  omadm_event_handler_process_event(int event_type, void *data)
{
    request_t *req = (request_t*)malloc(sizeof(request_t));
    omadm_request_t *dm_req = (omadm_request_t*)malloc(sizeof(omadm_request_t));
    if (!req | !dm_req) {
        if (req)
            free(req);
        if (dm_req)
            free(dm_req);
        return 1;
    }
    req->handler = OMADM;
    dm_req->package0 = (buffer_t*)data;
    dm_req->type = event_type;
    req->request = dm_req;
    put_request_to_queue(req);
    return 0;
}

int  omadm_event_handler_process_alert(alert_t *alert)
{
    request_t * req = (request_t*)malloc(sizeof(request_t));
    omadm_request_t * dm_req = (omadm_request_t*)malloc(sizeof(omadm_request_t));
    if (!req | !dm_req) {
        if (req)
            free(req);
        if (dm_req)
            free(dm_req);
        return 1;
    }
    req->handler = OMADM;
    dm_req->alert = alert;
    dm_req->type = GENERIC_ALERT;
    req->request = dm_req;
    put_request_to_queue(req);
    return 0;
}

