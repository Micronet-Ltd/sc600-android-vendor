/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <string.h>

#include <pal.h>
#include "pal_diagmon_test.h"

#define STR_BUFF_SIZE 256

static const pal_func_description_t __pal_funcs[] = {
    {"list", "pal_system_storages_get",
        __PAL_ARGS_S, "(data_buffer_t*)"},
    {"total", "pal_system_storage_x_total_get",
        __PAL_ARGS_PC_PR4, "(char*, float*)"},
    {"avail", "pal_system_storage_x_avail_get",
        __PAL_ARGS_PC_PR4, "(char*, float*)"},
    {"used", "pal_system_storage_x_used_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"percents", "pal_system_storage_percentfree_get",
        __PAL_ARGS_PC_PI, "(char*, int*)"},
    {"apps", "pal_system_storage_x_used_applications_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"visuals", "pal_system_storage_x_used_pictures_video_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"audio", "pal_system_storage_x_used_audio_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"downloads", "pal_system_storage_x_used_downloads_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"misc", "pal_system_storage_x_used_miscellaneaous_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"cache", "pal_system_storage_x_used_caches_get",
        __PAL_ARGS_PC_PI4, "(char*, int32_t*)"},
    {"sw.date", "pal_system_date_get",
        __PAL_ARGS_S, "(data_buffer_t*)"},
    {"sw.time", "pal_system_timeutc_get",
        __PAL_ARGS_S, "(data_buffer_t*)"},
    {NULL, NULL, -1, NULL},
};

int pal_diagmon_storage_test(int argc, char** argv)
{
    return pal_test_subsection(argc, argv,
        (pal_func_description_t*)__pal_funcs);
}
