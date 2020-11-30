/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

/**
 *   Module to check status for WiFi and Cellular networks.
 */
#include <cutils/log.h>
#include <stdlib.h>
#include <string.h>
#include "omadm_service_api.h"
#include "pal.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL"

#define CALLBACKS_LEN 3
static network_conditions_changed_t network_changed_listener[CALLBACKS_LEN];

// module private function to handle network conditions changed events
static void network_conditions_changed(void *);
static int find_empty_callback_id(network_conditions_changed_t
        netm_handler_process_event);
/**
 * Set callback function for process cellular network events
 *
 * @param[in] netm_handler_process_event Callback function
 * @return non-negative id of registered callback if success, otherwise error
 * code.
 */
int pal_notify_on_network_conditions_changed(network_conditions_changed_t
        netm_handler_process_event)
{
    ALOGD("pal_notify_on_network_conditions_changed()");
    int status = 0;
    if (netm_handler_process_event == NULL) {
        ALOGE("pal_notify_on_network_conditions_changed() fault with error: \
            Callback wasn't specified");
    }
    int id = find_empty_callback_id(netm_handler_process_event);
    if (id < 0) {
        return id;
    }
    network_changed_listener[id] = netm_handler_process_event;
    // register callback listener
    status = pal_register_omadm_callback(OMADM_NET_MANAGER_REPLY,
            network_conditions_changed);
    if (status == NO_ERROR) {
        return id;
    } else {
        network_changed_listener[id] = NULL;
    }
    return -ERROR_ADD_OMADM_CALLBACK;
}

/**
 * Reset callback function to stop processing of cellular network events
 * @param callback_id returned by pal_notify_on_network_conditions_changed call
 * @return result code 0 if success
 */
int pal_notify_stop_on_network_conditions_changed(int callback_id)
{
    ALOGD("pal_notify_stop_on_network_conditions_changed()");
    if (callback_id < 0 || callback_id > CALLBACKS_LEN - 1) {
        return -ERROR_REMOVE_OMADM_CALLBACK;
    }
    network_changed_listener[callback_id] = NULL;
    // unregister callback listener
    int status = pal_unregister_omadm_callback(OMADM_NET_MANAGER_REPLY);
    return -status;
}

void network_conditions_changed(void *network_conditions)
{
    network_conditions_t *nc = (network_conditions_t *)network_conditions;
    if (nc == NULL) {
        ALOGE("network_conditions_changed() called with NULL arguments");
        return;
    }
    ALOGD("network_conditions_changed() type [%d], enabled [%d]", (int)nc->net_feature, (int)nc->enabled );
    for (int i = 0; i < CALLBACKS_LEN; ++i) {
        if (network_changed_listener[i] == NULL) {
            ALOGD("network_conditions_changed() network_changed_listener[%d] wasn't properly set and missed", i);
        } else {
            network_conditions_t local;
            memcpy(&local, nc, sizeof(network_conditions_t));
            ALOGE("notify listener [%d]", i);
            network_changed_listener[i](local);
        }
    }
    free(nc);
    return;
}

/**
 * @return -1 if specified callback was already registered.
 * -2 if there is no free slot for a new callback was found.
 */
int find_empty_callback_id(network_conditions_changed_t
        netm_handler_process_event) {
    int id;
    int i;
    for (i = 0; i < CALLBACKS_LEN; ++i) {
        if (network_changed_listener[i] == NULL) {
            id = i;
            break;
        } else if (network_changed_listener[i] == netm_handler_process_event) {
            id = -1;
            break;
        }
    }
    if (i == CALLBACKS_LEN) {
        id = -2;
        ALOGE("find_empty_callback_id() can't find\
        a free slot for the new callback");
    }
    return id;
}