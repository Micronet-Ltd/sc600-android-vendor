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

#define MAX_DATA_SIZE 127
#define PAL_MODULE "pal_network"
#define CMD_SET "set"
#define CMD_GET "get"
#define SECTION_APN "apn"

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"
extern int pal_system_ext_get(data_buffer_t *Buffer);


void getNewDataBuffer(data_buffer_t* pDBuffer, unsigned int size) {
    pDBuffer->data = (char*) malloc(size);
    pDBuffer->size = size;
    memset(pDBuffer->data, 0, pDBuffer->size);
}

    // Releases PAL data buffer
void freeDataBuffer(data_buffer_t* pDBuffer) {
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        free(pDBuffer->data);
        pDBuffer->data == NULL;
        pDBuffer->size = 0;
    }
}


int main(int argc, char** argv)
{
    printf("DevInfo tests\n");
    int reply = 0;
    data_buffer_t dataBuffer;
    // Get Device ID (normal case)
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_dev_id_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_dev_id_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_dev_id_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;

    // Get Device ID (NULL buffer)
    reply = pal_system_dev_id_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_dev_id_get() <NULL buffer> error: %d\n", reply);
    } else {
        printf("pal_system_dev_id_get() <NULL buffer> : %s\n",
                dataBuffer.data);
    }
    reply = 0;

    // Get Device ID (small buffer)
    getNewDataBuffer(&dataBuffer, 2);
    reply = pal_system_dev_id_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_dev_id_get() <small buffer> error: %d\n", reply);
    } else {
        printf("pal_system_dev_id_get() <small buffer> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    //Get manufacturer normal case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_man_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_man_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_man_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    //Get model normal case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_mod_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_mod_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_mod_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    //Get manufacturer dmv case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_dmv_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_dmv_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_dmv_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    //Get default langauge normal case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_lang_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_lang_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_lang_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    // Get sim serial number normal case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_iccid_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_iccid_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_iccid_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;
    //Get system ext normal case
    getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_system_ext_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        printf("pal_system_ext_get() <normal case> error: %d\n", reply);
    } else {
        printf("pal_system_ext_get() <normal case> : %s\n",
                dataBuffer.data);
    }
    freeDataBuffer(&dataBuffer);
    reply = 0;

  return 0;
}
