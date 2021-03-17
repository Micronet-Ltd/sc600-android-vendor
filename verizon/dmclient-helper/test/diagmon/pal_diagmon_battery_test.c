/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "./pal_test_engine.h"

static pal_func_description_t __battery_funcs[] = {
    {"batteries", "pal_system_batteries_get", __PAL_ARGS_PI4,
        "(int32_t*)"},
    {"temp", "pal_system_battery_x_temp_get", __PAL_ARGS_I_S,
        "(int, data_buffer_t*)"},
    {"level", "pal_system_battery_x_level_get", __PAL_ARGS_I_PR4,
        "(int, float*)"},
    {"status", "pal_system_battery_x_status_get", __PAL_ARGS_I_PI4,
        "(int, int32_t*)"},
    {"type", "pal_system_battery_x_type_get", __PAL_ARGS_I_S,
        "(int, data_buffer_t*)"},
    {"voltage", "pal_system_battery_x_voltage_get", __PAL_ARGS_I_PI4,
        "(int, int32_t*)"},
    {"plug_status", "pal_system_battery_x_plug_status_get",
        __PAL_ARGS_I_PI4, "(int, int32_t*)"},
    {NULL, NULL, -1, NULL},
};


int pal_diagmon_battery_test(int argc, char** argv)
{
    printf("(%d, argv)\n", argc );
    return pal_test_subsection(argc, argv, __battery_funcs);
}
