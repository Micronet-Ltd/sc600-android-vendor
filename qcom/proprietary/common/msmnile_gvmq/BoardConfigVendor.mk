## BoardConfigVendor.mk
## Qualcomm Technologies proprietary product specific compile-time definitions.
#
USE_CAMERA_STUB := false

ifeq ($(TARGET_BOARD_AUTO),true)
# Still is not used for automotive
DISABLE_CAMERA_STILL := true
# AIS is used for automotive
USE_IMAGING_AIS := true
# Since in AutoPLs, AIS build is true
USE_CAMERA_STUB := true
endif

TARGET_SCVE_DISABLED := true
TARGET_FASTCV_DISABLED := true
TARGET_USES_SSC := true
TARGET_DISABLE_MSMBUSPM := true
TARGET_INIT_COLDBOOT_TIMEOUT := 10000
TARGET_INIT_CONSOLE_TIMEOUT := 10000
##SECIMAGE tool feature flags
USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN := 1
USES_SEC_POLICY_INTEGRITY_CHECK := 1
USE_SOC_HW_VERSION := true
SOC_HW_VERSION := 0x60030000
SOC_VERS := 0x6003
BOARD_USES_LIBC_WRAPPER := false
#TARGET_USES_IMS := true
#BOARD_USES_QCNE := true
BOARD_USES_DPM := true
#Flags for generating signed images
USESECIMAGETOOL := true
QTI_GENSECIMAGE_MSM_IDS := msmnile_gvmq
QTI_GENSECIMAGE_SIGNED_DEFAULT := msmnile_gvmq
#USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN := 1
#USES_SEC_POLICY_INTEGRITY_CHECK := 1
BOARD_USES_ADRENO := true
HAVE_ADRENO_SOURCE := true
HAVE_ADRENO_SC_SOURCE := true
HAVE_ADRENO_FIRMWARE := true
TARGET_USES_ION := true
TARGET_COMPILE_WITH_MSM_KERNEL := false

## wlan flags
TARGET_USES_ICNSS_QMI := true
TARGET_USES_SUBNET_DETECTION := true
BOARD_HAS_ATH_WLAN_AR6320 := true
BOARD_WLAN_DEVICE := qcwcn
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211

ifneq ($(PRODUCT_WLAN_DRIVER_ALWAYS_LOADED), true)

ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED), true)
WIFI_DRIVER_MODULE_PATH := "/system/vendor/lib/modules/qca_cld3_wlan.ko"
else
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/qca_cld3_wlan.ko"
endif

WIFI_DRIVER_MODULE_NAME := "wlan"
WIFI_DRIVER_MODULE_ARG := ""
endif

WIFI_DRIVER_BUILT := qca_cld3
WIFI_DRIVER_DEFAULT := qca_cld3
WIFI_DRIVER_INSTALL_TO_KERNEL_OUT := true
WPA_SUPPLICANT_VERSION := VER_0_8_X
HOSTAPD_VERSION := VER_0_8_X
CONFIG_ACS := true
CONFIG_IEEE80211AC := true
CONFIG_IEEE80211AX := true
BOARD_HAS_CFG80211_KERNEL3_4 := true
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)

## GPS
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := false
#ifeq ($(AUDIO_FEATURE_ENABLED_3D_AUDIO), true)
#-include $(TOPDIR) vendor/qcom/proprietary/mm-audio-noship/3DAudio/3DAudio-buildspec.mk
#endif


## wigig flags
WIGIG_DRIVER_MODULE_PATH := "/system/vendor/lib/modules/wil6210.ko"
WIGIG_DRIVER_MODULE_NAME := "wil6210"
WIGIG_DRIVER_MODULE_ARG := "alt_ifname=1"

## Bluetooth/ANT/FM/Wipower
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_QCOM := true
BOARD_ANT_WIRELESS_DEVICE := "qualcomm-hidl"
BOARD_HAVE_QCOM_FM := true
BOARD_HAS_QCA_FM_SOC := "cherokee"
BOARD_USES_WIPOWER := false
TARGET_HAS_SPLIT_A2DP_FEATURE := true
#Enable uibeautification
TARGET_TS_MAKEUP := true

# TODO: Bring-up exception - all modules must be fixed by corresponding teams
INTERNAL_LOCAL_CLANG_EXCEPTION_PROJECTS += \
  $(abspath $(TOPDIR)vendor/qcom/proprietary/gles/adreno200) \
  vendor/qcom/proprietary/gles/adreno200 \
  vendor/qcom/proprietary/fastcv-noship \
  vendor/qcom/proprietary/gps-noship \
  vendor/qcom/proprietary/kernel-tests \
  vendor/qcom/proprietary/mare-noship \
  vendor/qcom/proprietary/mm-camera-lib \
  vendor/qcom/proprietary/mm-camera/mm-camera2/media-controller/modules/imglib \
  vendor/qcom/proprietary/mm-camera/mm-camera2/tests \
  vendor/qcom/proprietary/mm-mux

# TODO: Bring-up exception - all modules must be fixed by corresponding teams
TARGET_CINCLUDES_EXCEPTION_PROJECTS := \
  $(abspath $(TOPDIR)vendor/qcom/proprietary/gles/adreno200) \
  vendor/qcom/proprietary/gles/adreno200 \
  vendor/qcom/proprietary/gps-noship-external \
  vendor/qcom/proprietary/gps-noship \
  disregard/filesystems/mtd-utils

WIFI_DRIVER_STATE_CTRL_PARAM := "/dev/wlan"
WIFI_DRIVER_STATE_ON := "ON"
WIFI_DRIVER_STATE_OFF := "OFF"

# Pre-sil Flags to fix the compliation issues with minimal configuration.
# ------Start----------------
TARGET_DISABLE_CAMX-LIB :=true
TARGET_DISABLE_GNSS := true
TARGET_DISABLE_QTEE := true

#Enable STA + SAP Concurrency.
WIFI_HIDL_FEATURE_DUAL_INTERFACE := true
# ----- end -----------------

TARGET_USES_GPQESE := true
