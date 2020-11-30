# Copyright (C) 2016 Verizon. All Rights Reserved.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := pal_diagmon_test.c
LOCAL_SRC_FILES += pal_diagmon_battery_test.c
LOCAL_SRC_FILES += pal_diagmon_storage_test.c
LOCAL_SRC_FILES += pal_diagmon_wifi_test.c
LOCAL_SRC_FILES += pal_diagmon_modes_test.c
LOCAL_SRC_FILES += pal_diagmon_memory_test.c
LOCAL_SRC_FILES += pal_diagmon_volumes_test.c
LOCAL_SRC_FILES += pal_diagmon_bluetooth_test.c
LOCAL_SRC_FILES += pal_diagmon_network_test.c
LOCAL_SRC_FILES += pal_diagmon_nfc_test.c
LOCAL_SRC_FILES += pal_diagmon_security_test.c
LOCAL_SRC_FILES += pal_dcmo_vlt_test.c
LOCAL_SRC_FILES += pal_dcmo_lvc_test.c
LOCAL_SRC_FILES += pal_dcmo_vwf_test.c
LOCAL_SRC_FILES += pal_dcmo_vce_test.c
LOCAL_SRC_FILES += pal_dcmo_eab_test.c
LOCAL_SRC_FILES += pal_dcmo_tvzw_test.c
LOCAL_SRC_FILES += pal_diagmon_gps_test.c
LOCAL_SRC_FILES += pal_test_engine.c
LOCAL_SRC_FILES += pal_fumo_test.c

LOCAL_SHARED_LIBRARIES := libpal liblog libcutils libdl

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../../system/core/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../dmclient/_include/

LOCAL_CLANG := true

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= diagmon.test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon

include $(BUILD_EXECUTABLE)
