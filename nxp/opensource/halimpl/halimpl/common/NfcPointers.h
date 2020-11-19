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

#include "phNxpNciHal_Adaptation.h"  //NfcConfig, nfc_stack_callback_t*, nfc_stack_data_callback_t*
#include <stdint.h>

/* This struct of function pointers can be exported with extern "C" and used to
 * access member functions without modifying those functions to be external.
 */
typedef struct HAL_API_struct {
    // phNxpNciHal_open
    int(*phNxpNciHal_open)(nfc_stack_callback_t*, nfc_stack_data_callback_t*);
    // phNxpNciHal_write
    int(*phNxpNciHal_write)(uint16_t, const uint8_t*);
    // phNxpNciHal_core_initialized
    int(*phNxpNciHal_core_initialized)(uint8_t*);
    // phNxpNciHal_pre_discover
    int(*phNxpNciHal_pre_discover)(void);
    // phNxpNciHal_close
    int(*phNxpNciHal_close)(bool);
    // phNxpNciHal_control_granted
    int(*phNxpNciHal_control_granted)(void);
    // phNxpNciHal_power_cycle
    int(*phNxpNciHal_power_cycle)(void);
    // phNxpNciHal_reset_nfcee_session
    void(*phNxpNciHal_do_factory_reset)(void);
    // phNxpNciHal_configDiscShutdown
    int(*phNxpNciHal_configDiscShutdown)(void);
    // phNxpNciHal_getVendorConfig
    void(*phNxpNciHal_getVendorConfig)(NfcConfig&);
    // phNxpNciHal_ioctl
    int(*phNxpNciHal_ioctl)(long, void*);
    // phNxpNciHal_get_debug_status
    bool(*phNxpNciHal_get_debug_status)(void);
} hal_api_struct_t;
