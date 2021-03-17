/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pal_test_engine.h"
#include <sys/cdefs.h>

#ifndef PAL_DIAGMON_TEST_H_
#define PAL_DIAGMON_TEST_H_

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

__BEGIN_DECLS

int pal_diagmon_battery_test(int argc, char** argv);
int pal_diagmon_storage_test(int argc, char** argv);
int pal_diagmon_wifi_test();
int pal_diagmon_nw_modes_test();
int pal_diagmon_memory_test(int argc, char** argv);
int pal_diagmon_volumes_test();
int pal_diagmon_bluetooth_test(const char* command);
int pal_diagmon_network_test();
int pal_diagmon_network_test_1(int argc, char** argv);
int pal_diagmon_nfc_test();
int pal_dcmo_amr_test(const char* command, const char* value);
int pal_diagmon_security_test();
int pal_dcmo_vlt_test(int argc, char** argv);
int pal_dcmo_lvc_test(int argc, char** argv);
int pal_dcmo_vwf_test(int argc, char** argv);
int pal_dcmo_vce_test(int argc, char** argv);
int pal_dcmo_eab_test(int argc, char** argv);
int pal_dcmo_twzw_test(int argc, char** argv);
int pal_diagmon_gps_test();
int pal_fumo_test(int argc, char** argv);

__END_DECLS

#endif /* PAL_DIAGMON_TEST_H_ */
