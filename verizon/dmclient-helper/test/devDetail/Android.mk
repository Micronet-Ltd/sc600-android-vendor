# Copyright (C) 2016 Verizon. All Rights Reserved.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= devdetail.c
LOCAL_SHARED_LIBRARIES := libdl liblog libcutils libpal

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../../system/core/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../include \
                    $(LOCAL_PATH)/../../../dmclient/_include/ \
LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES)

LOCAL_CLANG := true

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= devdetail.test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon

include $(BUILD_EXECUTABLE)
