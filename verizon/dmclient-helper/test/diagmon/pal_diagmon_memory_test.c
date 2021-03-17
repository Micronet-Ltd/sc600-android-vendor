/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "pal_diagmon_test.h"

static pal_func_description_t __pal_func_descriptions[] = {
    {"avail", "pal_system_memory_avail_get", __PAL_ARGS_PR4,
        "(meminfo_param_t*)"},
    {"total", "pal_system_memory_total_get", __PAL_ARGS_PR4,
        "(meminfo_param_t*)"},
    {"percents", "pal_system_memory_percentfree_get", __PAL_ARGS_PI,
        "(float*)"},
    {NULL, NULL, -1, NULL},
};

int pal_diagmon_memory_test(int argc, char** argv)
{
    return pal_test_subsection(argc, argv,
        (pal_func_description_t*)__pal_func_descriptions);
}
