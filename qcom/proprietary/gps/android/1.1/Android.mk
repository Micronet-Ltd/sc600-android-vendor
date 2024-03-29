ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifneq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss@1.1-impl
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    LocHidlAGnss.cpp \
    LocHidlDebugReportService.cpp \
    LocHidlFlpService.cpp \
    LocHidlGeofenceService.cpp \
    LocHidlIzatProvider.cpp \
    LocHidlGnssNi.cpp \
    LocHidlIzatOsNpGlue.cpp \
    LocHidlRilInfoMonitor.cpp \
    LocHidlIzatSubscription.cpp \
    LocHidlGnss.cpp \
    LocHidlXT.cpp \
    LocHidlWiFiDBReceiver.cpp \

LOCAL_SRC_FILES += \
    location_api/LocHidlNiClient.cpp \
    location_api/LocHidlFlpClient.cpp \
    location_api/LocHidlGeofenceClient.cpp \
    location_api/LocHidlUtils.cpp \

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/location_api \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/libxtadapter \
    $(TARGET_OUT_HEADERS)/libizat_core \
    $(TARGET_OUT_HEADERS)/libflp \
    $(TARGET_OUT_HEADERS)/liblbs_core \

LOCAL_HEADER_LIBRARIES := \
    izat_remote_api_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    liblocationservice_jni_headers \
    liblocationservice_glue_headers \
    liblocationservice_headers \
    libdataitems_headers \
    izat_remote_api_prop_headers \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libhidlbase \
    libhidltransport \
    libhwbinder \
    libutils \
    android.hardware.gnss@1.0 \
    vendor.qti.gnss@1.0 \
    vendor.qti.gnss@1.1 \

LOCAL_SHARED_LIBRARIES += \
    libloc_core \
    libgps.utils \
    libdl \
    liblocation_api \
    libflp \
    liblocationservice_glue \
    liblbs_core \
    libdataitems \
    liblocationservice \
    libxtadapter \
    libizat_core \
    libizat_client_api \

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss@1.1-service
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_INIT_RC := service/vendor.qti.gnss@1.1-service.rc
LOCAL_SRC_FILES := \
    service/service.cpp \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \

LOCAL_SHARED_LIBRARIES += \
    libhwbinder \
    libhidlbase \
    libhidltransport \
    android.hardware.gnss@1.0 \
    android.hardware.gnss@1.1 \
    vendor.qti.gnss@1.0 \
    vendor.qti.gnss@1.1 \

LOCAL_CFLAGS += $(GNSS_CFLAGS)

ifeq ($(TARGET_ARCH),arm)
    LOCAL_CFLAGS += -DARCH_ARM_32
endif

include $(BUILD_EXECUTABLE)

endif # TARGET_SUPPORTS_ANDROID_WEAR
endif # BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
