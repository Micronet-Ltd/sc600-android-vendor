# Copyright (C) 2016 Verizon. All Rights Reserved.

# C++11 support is enabled
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= pal_test.cpp omadm_event_handler.c
LOCAL_SHARED_LIBRARIES := libc++ libutils libbinder liblog libcutils

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/core/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_CLANG := true
LOCAL_CPPFLAGS := \
        -std=c++11 \

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= paltest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build network test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_network_test.c
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils libcurl

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/core/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \


LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= palnettest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)


# build admin network test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_admin_test.cpp
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils libcurl

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \


LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
                    $(LOCAL_PATH) $(LOCAL_PATH)/../pal \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= paladmintest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build firmware update imitation test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_firmware_update_test.c
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils libcurl

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/core/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= palfwuptest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build download descriptor parser test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_dd_parser_test.c
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils libcurl libexpat

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/core/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \
                    $(LOCAL_PATH)/../../../../external/expat/lib/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= palparsertest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build download test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_download_test.c
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils libcurl

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/core/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= paldownloadtest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build battery state test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_battery_test.cpp
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../external/libcxx/include/


LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
                    $(LOCAL_PATH) $(LOCAL_PATH)/../pal \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= palbatterytest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

# build check for update state test executable module

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pal_button_update_test.cpp
LOCAL_SHARED_LIBRARIES := libpal libc++ libutils libbinder liblog libcutils

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../external/libcxx/include/

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \
                    $(LOCAL_PATH) $(LOCAL_PATH)/../pal \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= palbuttonupdatetest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon
include $(BUILD_EXECUTABLE)

include $(LOCAL_PATH)/connmo/Android.mk
include $(LOCAL_PATH)/../devDetail/Android.mk
include $(LOCAL_PATH)/../devinfo/Android.mk
include $(LOCAL_PATH)/../diagmon/Android.mk
