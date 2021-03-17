# Copyright (C) 2016 Verizon. All Rights Reserved.
LOCAL_PATH:= $(call my-dir)

appcompat_dir := ../../../../../prebuilts/sdk/current/support/v7/appcompat/res
res_dirs := res $(appcompat_dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := verizon

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_JAVA_LIBRARIES := telephony-common
LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-appcompat
LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4

# some package path can not be used by compiler due to this setting
# LOCAL_SDK_VERSION := current
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PACKAGE_NAME := DMBrowser
LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))
LOCAL_PRIVILEGED_MODULE := true
LOCAL_AAPT_FLAGS += --auto-add-overlay
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.appcompat:android.support.v4

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
