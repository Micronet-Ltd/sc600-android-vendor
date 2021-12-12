/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_TAG "android.hardware.nfc@1.1-impl"

#include "NfcApiGet.h"
#include <log/log.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <cstring>
#include <dlfcn.h>

struct nqx_devinfo {
    // Must be kept in sync with nqx_ UAPI
    unsigned char chip_type;
    unsigned char rom_version;
    unsigned char fw_major;
    unsigned char fw_minor;
};

union nqx_uinfo {
    unsigned int i;
    struct nqx_devinfo info;
};

bool nfc_debug_enabled = false;
static hal_api_struct_t *hal_api_s = nullptr;
static bool struct_initialized = false;
static void* hal_handle = nullptr;

/******************************************************************************
 * Function         chipInfo
 *
 * Description      This function returns a success/fail integer, and populates
 *                  nqx_info with chip information.
 *
 * Returns          int return status (0/-1).
 *
 ******************************************************************************/
static int chipInfo(union nqx_uinfo* nqx_info) {
    int fp;

    fp = open(DEV_NODE, O_RDONLY);
    if (fp < 0) {
        ALOGE("Not able to open dev node %s due to errno (%d) %s", DEV_NODE, errno, strerror(errno));
        return STATUS_FAILURE;
    }

    nqx_info->i = ioctl(fp, NFCC_GET_INFO, 0);
    int retval = static_cast<int> (nqx_info->i);
    if (retval < 0) {
        ALOGE("Failed to retrieve chip ID from hardware due to errno (%d), %s", errno, strerror(errno));
        return STATUS_FAILURE;
    }

    ALOGD("NFC chip id = 0x%x\n", nqx_info->info.chip_type);
    ALOGD("NFC FW Ver = %02x.%02x.%02x\n", nqx_info->info.rom_version, nqx_info->info.fw_major, nqx_info->info.fw_minor);

    close(fp);
    return STATUS_SUCCESS;
}

/******************************************************************************
 * Function         getHalFileName
 *
 * Description      This function determines which library should be opened,
 *                  based on the NFC chip ID from chipInfo().
 *
 * Returns          char const* file_name
 *
 ******************************************************************************/
static char const* getHalFileName() {
    // get chip info from hardware, and return the correct HAL file name.
    union nqx_uinfo nqx_info;
    int ret = chipInfo(&nqx_info);

    if (ret == 0) {

        switch(nqx_info.info.chip_type) {

            case NQ310_ID:
            case NQ210_ID:
            case NQ440_ID: // ALSO NQ330
            case NQ220_ID:
                ALOGD("NFC HW chip-id matches with NQxxx");
                return NQX_FILE_NAME;

            case SN100X_ID_A:
            case SN100X_ID_B:
                // SN100 has chip id 0xa4 or 0xa3
                ALOGD("NFC HW chip-id matches with SN100X");
                return SN100X_FILE_NAME;

            default:
                // if chip id isn't matched, load the NQxxx HAL.
                ALOGD("Chip does not match known ID - defaulting to NQX library.");
                return NQX_FILE_NAME;
        }
    }

    else {
        ALOGD("Obtaining Chip ID failed, attempting loading of default shared library instead.");
        return NQX_FILE_NAME;
    }
}

/******************************************************************************
 * Function         unloadHalApiStruct
 *
 * Description      This function is responsible for closing the dynamic library
 *                  and unloading the HAL struct; in order to save memory on low
 *                  memory targets.
 *
 * Returns          void.
 *
 ******************************************************************************/
void unloadHalApiStruct() {
    hal_api_s = nullptr;
    struct_initialized = false;
    dlclose(hal_handle);
    ALOGD("Unloaded HAL API Struct");
}

/******************************************************************************
 * Function        getHalApiStruct
 *
 * Description      This function initializes a struct of pointers to functions
 *                  within the NFC HAL implementation.
 *
 * Returns          hal_api_s pointer or nullptr.
 *
 ******************************************************************************/
hal_api_struct_t* getHalApiStruct() {

    char const* linkerror = nullptr;
    char const* file_name = nullptr;

    /*
     * If our HAL has been loaded successfully, we don't want to re-open it on every NFC function call.
     * Conversely, if loading the HAL failed, we want to ensure we're not repeatedly trying to link.
     */
    if (struct_initialized) {
        return hal_api_s;
    }

    // Obtain the file name for the HAL library we wish to load
    file_name = getHalFileName();
    // Reset any errors
    dlerror();

    /*
     * Attempt to load the library!
     * /vendor/lib(64)/hw/nci_nfc.nqx.default.hw.so for NQx
     * /vendor/lib(64)/hw/nci_nfc.sn100x.default.hw.so otherwise
     */
    hal_handle = dlopen(file_name, RTLD_NOW);
    linkerror = dlerror();
    if (linkerror != nullptr) {
        ALOGE("Error opening HAL file: %s with linking error: %s", file_name, linkerror);
        /*
         * Set struct as initialized even if loading fails, so that loading is only attempted
         * once per NFC service initialization attempt.
         */
        struct_initialized = true;
        return nullptr;
    }

    ALOGD("Loading nfc HAL function table");
    // Reset errors again
    dlerror();
    hal_api_s = (hal_api_struct_t*)dlsym(hal_handle, "api_funcs");
    linkerror = dlerror();
    if (linkerror != nullptr) {
        ALOGE("Error loading HAL function API symbol table: %s", linkerror);
        struct_initialized = true;
        return nullptr;
    }

    //set the debug status and set the active status of the HIDL.
    nfc_debug_enabled = 0; //hal_api_s->phNxpNciHal_get_debug_status();
    struct_initialized = true;
    return hal_api_s;
}
