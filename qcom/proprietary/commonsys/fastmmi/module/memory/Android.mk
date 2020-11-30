LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE        := mmi_memory
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE  := false
LOCAL_MULTILIB      := 32
LOCAL_MODULE_OWNER  := qcom
LOCAL_MODULE_TAGS   := optional
LOCAL_SRC_FILES     := lib/mmi_memory.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mmi_memory
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE  := false
LOCAL_MULTILIB      := 64
LOCAL_MODULE_OWNER  := qcom
LOCAL_MODULE_TAGS   := optional
LOCAL_SRC_FILES     := lib64/mmi_memory.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib64
include $(BUILD_PREBUILT)
