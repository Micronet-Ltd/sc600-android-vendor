/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "pal_diagmon_test.h"

const static pal_func_description_t __pal_func_descriptions[] = {
    {"get", "pal_mobile_lvc_state_get", __PAL_ARGS_PI, ""},
    {"enable", "pal_mobile_lvc_enable", __PAL_ARGS_V, ""},
    {"disable", "pal_mobile_lvc_disable", __PAL_ARGS_V, ""},
    {NULL, NULL, -1, NULL},
};

int pal_dcmo_lvc_test(int argc, char** argv)
{
    return pal_test_subsection(argc, argv,
        (pal_func_description_t*)__pal_func_descriptions);
}
