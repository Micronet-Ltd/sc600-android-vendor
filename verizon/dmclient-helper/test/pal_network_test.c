/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pal.h"

#define TIMEOUT 30  // in seconds
#define TEST_FILE_NAME "packet1.xml"
#define FILE_MAX_SIZE  8000
#define URI_FUNAMBOL "http://localhost:8080/funambol/dm"
#define URI_GITHUB "https://github.com/"
#define URI_UNKNOWN "http://ooops.http"

/**
 *  Service Unavailable test
 *  precondition: none
 *  504 Gateway Timeout should be returned
 */
int pal_network_test_unknown_host(void)
{
    data_buffer_t in;
    data_buffer_t out;
    char * server_uri = URI_UNKNOWN;
    int status;
    pal_http_options_t options;
    options.timeout = TIMEOUT;
    options.hdr_content_data = "Content-Type: text/xml";
    options.username = "test";
    options.password = "test";
    options.auth_type = AUTH_HMAC;
    options.hdr_auth_data ="x-syncml-hmac:algorithm=MD5, username=\"user\", mac=NTI2OTJhMDAwNjYxODkwYmQ3NWUxN2R";
    options.ca_fname = NULL;
    options.client_cert = NULL;
    options.client_key = NULL;

    memset(&in,0,sizeof(in));
    memset(&out,0,sizeof(out));

    status = pal_send_http_post_request (server_uri, &options, &out, &in);//504
    return status;
}

/**
 *  Test https connection.
 *  precondition: none
 */
int pal_network_test_https(void)
{
    char * server_uri = URI_GITHUB;
    data_buffer_t in;
    data_buffer_t out;
    int status;
    pal_http_options_t options;
    options.timeout = TIMEOUT;
    options.hdr_content_data = "Content-Type: text/xml";
    options.username = "g3110"; // test account for github
    options.password = "g123123456";
    options.auth_type = AUTH_BASIC;
    options.hdr_auth_data = "Authorization: Basic ZzMxMTA6ZzEyMzEyMzQ1Ng==";
    options.ca_fname = "/etc/ssl/certs/DigiCert_High_Assurance_EV_Root_CA.pem";
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

/**
 *  Test connection with invalid parameters
 */
int pal_network_test_invalid_param(void)
{
    char * server_uri = URI_GITHUB;
    int status;
    status = pal_send_http_post_request(server_uri, NULL, NULL, NULL);
    return status;
}

/**
 *  Test connection with invalid uri
 */
int pal_network_test_invalid_uri(void)
{
    int status = pal_send_http_post_request (NULL, NULL, NULL, NULL);
    return status;
}

/**
 *  Test server unavailable
 */
int pal_network_test_funambol_unavailable(void)
{
    data_buffer_t in;
    data_buffer_t out;
    int status;
    char * server_uri = URI_FUNAMBOL;
    out.data = (char *)malloc(512);
    strcpy(out.data, "<SyncML xmlns='SYNCML:SYNCML1.2'><SyncHdr></SyncHdr></SyncML>");
    out.size = strlen(out.data)+1;
    memset(&in,0,sizeof(in));

    pal_http_options_t options;
    options.timeout = TIMEOUT;
    options.hdr_content_data = "Content-Type: application/vnd.syncml+xml";
    options.username = "DMCTest";
    options.password = "DMCTest";
    options.auth_type = AUTH_DIGEST;
    options.hdr_auth_data = "Authorization: Digest username=\"DMCtest\", nonce=\"RTYIoqwer\", realm=\"funambol.com\", response=\"6ZzEyMzEyMzQ1Ng=\"";
    options.ca_fname = NULL;
    options.client_cert = NULL;
    options.client_key = NULL;

    status = pal_send_http_post_request (server_uri,
                                                    &options,
                                                    &out, NULL);
    if(out.data)
        free(out.data);
    if(in.data)
        free(in.data);
    return status;
}

int main(int argc, char** argv)
{
    int status;
    status = pal_network_test_unknown_host();
    if ( status == 504)
        printf("Test unknown_host success\n");
    else
        printf("Test unknown_host fail, status = %d\n",status);
    status = pal_network_test_https();
    if ( status == 200)
        printf("Test test_https success\n");
    else
        printf("Test test_https fail, status = %d\n",status);

    status = pal_network_test_invalid_param();
    if ( status == 200)
        printf("Test invalid_param success\n");
    else
        printf("Test invalid_param fail, status = %d\n",status);

    status = pal_network_test_invalid_uri();
    if ( status == 503)
        printf("Test invalid_uri success\n");
    else
        printf("Test invalid_uri, status = %d\n",status);

    status = pal_network_test_funambol_unavailable();
    if ( status != 200 )
        printf("Test funambol_unavailable success\n");
    else
        printf("Test funambol_unavailable, status = %d\n",status);

    return 0;
}
