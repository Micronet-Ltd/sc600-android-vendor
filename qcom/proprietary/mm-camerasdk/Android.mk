LOCAL_HEADER_LIBRARIES := libutils_headers

ifneq ($(strip $(USE_CAMERA_STUB)),true)
ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),arm arm64))
include $(call all-subdir-makefiles)
endif
endif #!USE_CAMERA_STUB
