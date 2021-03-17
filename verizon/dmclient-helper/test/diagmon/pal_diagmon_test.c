/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <string.h>
#include "../../pal/pal_internal.h"
#include "pal_diagmon_test.h"

void getNewDataBuffer(data_buffer_t* pDBuffer, unsigned int size)
{
    pDBuffer->data = (char*) malloc(size);
    pDBuffer->size = size;
    memset(pDBuffer->data, 0, pDBuffer->size);
}

    // Releases PAL data buffer
void freeDataBuffer(data_buffer_t* pDBuffer)
{
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        FREE_IF(pDBuffer->data);
        pDBuffer->size = 0;
    }
}

static int pal_diagmon_wifi_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_wifi_test();
}

static int pal_diagmon_volumes_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_volumes_test();
}

static int pal_diagmon_bluetooth_test_(int argc, char** argv)
{
    if (argc > 0) {
        return pal_diagmon_bluetooth_test(argv[0]);
    }
    return pal_diagmon_bluetooth_test(NULL);
}

static int pal_diagmon_network_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_network_test();
}

static int pal_diagmon_nfc_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_nfc_test();
}

static int pal_diagmon_security_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_security_test();
}

static int pal_diagmon_gps_test_(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    return pal_diagmon_gps_test();
}

static const pal_section_desc_t pal_section_descs[] = {
    {"storage", pal_diagmon_storage_test},
    {"battery", pal_diagmon_battery_test},
    {"wifi", pal_diagmon_wifi_test_},
    {"volumes", pal_diagmon_volumes_test_},
    {"bluetooth", pal_diagmon_bluetooth_test_},
    {"network", pal_diagmon_network_test_},
    {"network1", pal_diagmon_network_test_1},
    {"nfc", pal_diagmon_nfc_test_},
    {"security", pal_diagmon_security_test_},
    {"memory", pal_diagmon_memory_test},
    {"dcmo.vlt", pal_dcmo_vlt_test},
    {"dcmo.lvc", pal_dcmo_lvc_test},
    {"dcmo.vwf", pal_dcmo_vwf_test},
    {"dcmo.vce", pal_dcmo_vce_test},
    {"dcmo.eab", pal_dcmo_eab_test},
    {"dcmo.tvzw", pal_dcmo_twzw_test},
    {"gps", pal_diagmon_gps_test_},
    {"fumo", pal_fumo_test},
    {NULL, NULL}
};

int main (int argc, char** argv)
{
    int handled = 0;
    pal_section_desc_t* item = (pal_section_desc_t*)pal_section_descs;
    do {
        if (argc > 1) {
            if (strcmp(argv[1], item->section_key) == 0) {
                item->section_func(argc - 2, argv + 2);
                handled = 1;
            }
        }
        item ++;
    } while (item->section_key != NULL);
    if (!handled) {
        /* print possible sections */
        printf("usage: %s <section_name>\n", argv[0]);
        printf("possible sections:\n");
        item = (pal_section_desc_t*)pal_section_descs;
        do {
            printf("  %s\n", item->section_key);
            item ++;
        } while (item->section_key != NULL);
    }
    return 0;
}
