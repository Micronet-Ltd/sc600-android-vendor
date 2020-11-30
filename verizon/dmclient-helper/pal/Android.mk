LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


IDEV_SRC_FILES := \
					idev/idev_app_init.c \
					idev/idev_dme_init.c \
					idev/idev_thread_ui.c \
					idev/idev_dme_callback.c \
					idev/idev_dme_ui.c \
					idev/idev_thread_dme.c \
					idev/md5sum.c \
					idev/idev_dme_ims.c \
					idev/idev_dme_wds.c \

LOCAL_SRC_FILES := $(IDEV_SRC_FILES)

LOCAL_SRC_FILES += ../service_api/service_api_impl.cpp
LOCAL_SRC_FILES += ../service_api/service_listner_impl.cpp
LOCAL_SRC_FILES += pal.cpp
LOCAL_SRC_FILES += pal_devdetail.cpp
LOCAL_SRC_FILES += pal_fumo.cpp
LOCAL_SRC_FILES += pal_omadm_controller.cpp
LOCAL_SRC_FILES += pal_admin_apn.cpp
LOCAL_SRC_FILES += pal_network.cpp
LOCAL_SRC_FILES += pal_devinfo.cpp
LOCAL_SRC_FILES += pal_firmware_update.c
LOCAL_SRC_FILES += pal_event_handler.cpp
LOCAL_SRC_FILES += pal_event_listener.cpp
LOCAL_SRC_FILES += pal_firmware_download.c
LOCAL_SRC_FILES += pal_download_descriptor_parser.c
LOCAL_SRC_FILES += pal_package_verification.c
LOCAL_SRC_FILES += pal_network_status.cpp
LOCAL_SRC_FILES += pal_diagmon_battery.c
LOCAL_SRC_FILES += pal_utils.c
LOCAL_SRC_FILES += pal_diagmon_storage.cpp
LOCAL_SRC_FILES += pal_diagmon_modes.cpp
LOCAL_SRC_FILES += pal_diagmon_wifi.cpp
LOCAL_SRC_FILES += pal_diagmon_memory.c
LOCAL_SRC_FILES += pal_diagmon_volumes.cpp
LOCAL_SRC_FILES += pal_diagmon_bluetooth.cpp
LOCAL_SRC_FILES += pal_diagmon_network.cpp
LOCAL_SRC_FILES += pal_diagmon_nfc.cpp
LOCAL_SRC_FILES += pal_diagmon_gps.cpp
LOCAL_SRC_FILES += pal_diagmon_security.cpp
LOCAL_SRC_FILES += pal_dcmo_vlt.cpp
LOCAL_SRC_FILES += pal_dcmo_lvc.cpp
LOCAL_SRC_FILES += pal_dcmo_vwf.cpp
LOCAL_SRC_FILES += pal_dcmo_vce.cpp
LOCAL_SRC_FILES += pal_dcmo_eab.cpp
LOCAL_SRC_FILES += pal_fumo_policy.c
LOCAL_SRC_FILES += pal_connmo.cpp


LOCAL_SHARED_LIBRARIES := libc++ libutils libbinder liblog libcutils libcurl libcrypto libexpat libnetd_client libqmi libqmi_cci libqmiservices_ext

OMADM_C_INCLUDES := $(LOCAL_PATH)/../../../../frameworks/base/include/ \
                    $(LOCAL_PATH)/../../../../system/netd/include/ \
                    $(LOCAL_PATH)/../../../../bionic/libc/dns/include/ \
                    $(LOCAL_PATH)/../../../../external/libcxx/include/ \
                    $(LOCAL_PATH)/../../../../external/curl/include/ \
                    $(LOCAL_PATH)/../../../../external/boringssl/include/ \
                    $(LOCAL_PATH)/../../../../external/expat/lib/ \
					$(TOP)/vendor/qcom/proprietary/qmi/services_ext \
					$(TOP)/vendor/qcom/proprietary/qmi/inc \
	
OMADM_C_INCLUDES        += $(TOP)/vendor/qcom/proprietary/qmi/platform
OMADM_C_INCLUDES        += $(TOP)/vendor/qcom/proprietary/qmi/core/lib/inc
OMADM_C_INCLUDES        += $(TOP)/vendor/qcom/proprietary/qmi-framework/inc
OMADM_C_INCLUDES        += $(TOP)/vendor/qcom/proprietary/qmi/services_ext
OMADM_C_INCLUDES        += $(TOP)/vendor/qcom/proprietary/data/dsutils/inc
OMADM_C_INCLUDES        += $(TOP)/vendor//qcom/proprietary/common/inc
OMADM_C_INCLUDES        += $(TOP)/system/core/libcutils/include

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../include \
                    $(LOCAL_PATH)/../../dmclient/_include/ \

IDEV_C_INCLUDES := $(LOCAL_PATH)/../include/idev \
				   $(LOCAL_PATH)/../include/internal \
				   $(LOCAL_PATH)/../include/released/dme \
				   $(LOCAL_PATH)/../include/released/hal \
				   $(LOCAL_PATH)/idev \
					

LOCAL_C_INCLUDES += $(OMADM_C_INCLUDES) $(IDEV_C_INCLUDES)

LOCAL_CLANG := true

LOCAL_CPPFLAGS := \
        -std=c++11 \

LOCAL_MODULE:= libpal
LOCAL_MODULE_RELATIVE_PATH := drm
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := verizon

include $(BUILD_SHARED_LIBRARY)
# Create folder for prebuilt proprietary verizon libs and init files
#$(shell mkdir -p --mode=755 $(TARGET_OUT_SHARED_LIBRARIES)/../vendor/verizon/dmclient/lib;)
#$(shell mkdir -p --mode=755 $(TARGET_OUT_SHARED_LIBRARIES)/../vendor/verizon/dmclient/init;)
$(shell mkdir -p --mode=755 $(TARGET_OUT_VENDOR)/verizon/dmclient/lib;)
$(shell mkdir -p --mode=755 $(TARGET_OUT_VENDOR)/verizon/dmclient/init;)
