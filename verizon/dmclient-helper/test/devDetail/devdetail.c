/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include <utils/Log.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_TEST"
#define MAX_DATA_SIZE 127

    /*
     * Helpers
     */

    // Allocates PAL data buffer
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

    /*
     * Main Test routine
     */
int main(int argc, char *argv[])
{
    printf("DevDetail tests\n");
    int reply = 0;
    data_buffer_t dataBuffer;




        /*
         * Get oem
         */

        // Get oem (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_oem_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_oem_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_oem_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get oem (NULL buffer)
        reply = pal_system_oem_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_oem_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_oem_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get oem (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_oem_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_oem_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_oem_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get firmware version
         */

        // Get firmware version (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_fwv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_fwv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get firmware version (NULL buffer)
        reply = pal_system_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_fwv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_fwv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get firmware version (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_fwv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_fwv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get software version
         */

        // Get software version (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_swv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_swv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get software version (NULL buffer)
        reply = pal_system_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_swv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_swv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get software version (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_swv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_swv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get hardware version
         */

        // Get hardware version (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hwv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hwv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get hardware version (NULL buffer)
        reply = pal_system_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hwv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hwv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get hardware version (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hwv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hwv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get support large object
         */

        // Get support large object (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_support_lrgobj_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_support_lrgobj_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_support_lrgobj_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get support large object (NULL buffer)
        reply = pal_system_support_lrgobj_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_support_lrgobj_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_support_lrgobj_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get support large object (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_support_lrgobj_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_support_lrgobj_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_support_lrgobj_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get date of latest update
         */

        // Get date of latest update (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_date_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_date_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_date_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get date of latest update (NULL buffer)
        reply = pal_system_date_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_date_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_date_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get date of latest update (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_date_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_date_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_date_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get time of latest update
         */

        // Get time of latest update (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_timeutc_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_timeutc_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_timeutc_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get time of latest update (NULL buffer)
        reply = pal_system_timeutc_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_timeutc_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_timeutc_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get time of latest update (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_timeutc_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_timeutc_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_timeutc_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get manufacture of host device
         */

        // Get manufacture of host device (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_manu_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_manu_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_manu_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get manufacture of host device (NULL buffer)
        reply = pal_system_hostdevice_manu_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_manu_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_manu_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get manufacture of host device (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_manu_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_manu_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_manu_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get SwV of host device
         */

        // Get SwV of host device (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_swv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_swv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get SwV of host device (NULL buffer)
        reply = pal_system_hostdevice_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_swv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_swv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get SwV of host device (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_swv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_swv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_swv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get FwV of host device
         */

        // Get FwV of host device (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_fwv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_fwv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get FwV of host device (NULL buffer)
        reply = pal_system_hostdevice_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_fwv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_fwv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get FwV of host device (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_fwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_fwv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_fwv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get HwV of host device
         */

        // Get HwV of host device (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_hwv_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_hwv_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get HwV of host device (NULL buffer)
        reply = pal_system_hostdevice_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_hwv_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_hwv_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get HwV of host device (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_hwv_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_hwv_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_hwv_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get Date and time of update host device
         */

        // Get Date and time of update host device (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_datestamp_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_datestamp_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_datestamp_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get Date and time of update host device (NULL buffer)
        reply = pal_system_hostdevice_datestamp_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_datestamp_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_datestamp_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get Date and time of update host device (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_datestamp_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_datestamp_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_datestamp_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get host device id
         */

        // Get host device id (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_hostdevice_deviceid_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_deviceid_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_deviceid_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get host device id (NULL buffer)
        reply = pal_system_hostdevice_deviceid_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_deviceid_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_deviceid_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get host device id (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_hostdevice_deviceid_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_hostdevice_deviceid_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_hostdevice_deviceid_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        /*
         * Get device type
         */

        // Get device type (normal case)
        getNewDataBuffer(&dataBuffer, MAX_DATA_SIZE);
        reply = pal_system_devtype_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_devtype_get() <normal case> error: %d\n", reply);
        } else {
            printf("pal_system_devtype_get() <normal case> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;

        // Get device type (NULL buffer)
        reply = pal_system_devtype_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_devtype_get() <NULL buffer> error: %d\n", reply);
        } else {
            printf("pal_system_devtype_get() <NULL buffer> : %s\n",
                    dataBuffer.data);
        }
        reply = 0;

        // Get device type (small buffer)
        getNewDataBuffer(&dataBuffer, 2);
        reply = pal_system_devtype_get(&dataBuffer);
        if(RESULT_SUCCESS != reply) {
            printf("pal_system_devtype_get() <small buffer> error: %d\n", reply);
        } else {
            printf("pal_system_devtype_get() <small buffer> : %s\n",
                    dataBuffer.data);
        }
        freeDataBuffer(&dataBuffer);
        reply = 0;



    return 0;
}

