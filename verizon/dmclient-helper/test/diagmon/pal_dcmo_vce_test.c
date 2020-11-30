/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "pal_diagmon_test.h"

static pal_func_description_t __timer_funcs[] = {
    {"enable", "pal_mobile_vce_enable", __PAL_ARGS_V, ""},
    {"disable", "pal_mobile_vce_disable", __PAL_ARGS_V, ""},
    {"get", "pal_mobile_vce_state_get", __PAL_ARGS_PI, ""},
    {NULL, NULL, -1, NULL},
};

int pal_dcmo_vce_test(int argc, char** argv)
{
    printf("(%d, argv)\n", argc );
    return pal_test_subsection(argc, argv, __timer_funcs);
}
