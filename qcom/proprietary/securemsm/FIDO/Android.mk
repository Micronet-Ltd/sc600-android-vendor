ifeq ($(call is-board-platform-in-list,msm8974 msm8960 msm8660 msm8226 msm8610 msm8916 msm8916_32 msm8916_32_k64 msm8916_64 apq8084 msm8939 msm8994 msm8909 msm8996 msm8992 msm8952 msm8937 msm8953 msmcobalt),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES    := move_fido_session.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE       := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_PREBUILT)

endif # end filter
