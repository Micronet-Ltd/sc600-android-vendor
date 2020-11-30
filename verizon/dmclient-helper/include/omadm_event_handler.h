/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#ifndef __OMADM_EVENT_HANDLER__
#define __OMADM_EVENT_HANDLER__

#include "pal.h"

/// omadm request data type (internals.h)
typedef struct {
    uint8_t * buffer;
    size_t    len;
} buffer_t;

/// Request handler types
typedef enum {
    OMADM, ///< request for omadm handler
    UDM    ///< request for udm handler
} request_handler_t;

/// Management object alert structure
typedef struct {
    char *mo;         ///< management object
    char *alert_data; ///< management object alert data
} alert_t;

/// OMADM request structure
typedef struct {
    omadm_request_type_t type; ///< request type
    buffer_t *package0;        ///< data as package0 message
    alert_t *alert;            ///< data as alert message
} omadm_request_t;

/// Queue request structure
typedef struct {
    request_handler_t handler; ///< type of omadm handler
    void *request;             ///< request structure
} request_t;

/// Definition of pal_notify_on_omadm_event for dynamic linking
#define PAL_NOTIFY_ON_OMADM_EVENT "pal_notify_on_omadm_event"


/**
 * Event Handler Initialization
 * Link to PAL and setup event callback through call API function
 * pal_notify_on_omadm_event
 */
#ifdef __cplusplus
extern "C" {
#endif
int omadm_event_handler_init();
#ifdef __cplusplus
}
#endif

/**
 * Process WAP push or “Check for update” requests, that
 * require a new OMADM session and puts them to the queue.
 *
 * @param[in] event_type OMADM event id
 * @param[in] data OMADM event request data
 * @return Result code
 */
int omadm_event_handler_process_event(int event_type, void *data);

/**
 * Process alert request, that require a new OMADM session
 * and puts them to the queue.
 *
 * @param[in] alert Alert request data
 * @return Result code
 */
int omadm_event_handler_process_alert(alert_t *alert);

/**
 * put request to gueue
 * 
 * @param[in] request
 */
#ifdef __cplusplus
extern "C" {
#endif
void put_request_to_queue(request_t * req);
#ifdef __cplusplus
}
#endif

#endif