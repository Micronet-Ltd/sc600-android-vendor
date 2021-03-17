LOCAL_PATH := $(call my-dir)

# ---------------------------------------------------------------------------
#        Make the shared library (libmmcamera_s5k3p3_gt24c32_eeprom.so)
# ---------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_LDFLAGS := $(mmcamera_debug_lflags)

LOCAL_CFLAGS := -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../../../../mm-camerasdk/sensor/includes/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../../../mm-camerasdk/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_HEADER_LIBRARIES := libutils_headers

LOCAL_SRC_FILES:= s5k3p3_gt24c32_eeprom.c
LOCAL_MODULE           := libmmcamera_s5k3p3_gt24c32_eeprom
LOCAL_SHARED_LIBRARIES := liblog libcutils libmmcamera_eeprom_util

ifeq ($(MM_DEBUG),true)
LOCAL_SHARED_LIBRARIES +=  liblog
endif


LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(32_BIT_FLAG), true)
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_SHARED_LIBRARY)
