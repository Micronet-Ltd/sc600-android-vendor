ifeq ($(call is-board-platform-in-list,sdm660 msm8998 apq8098_latv),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#
# unit test executables
#

# Global flag and include definitions
TEST_CFLAGS := -Werror \
  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags) \
  -DMSM_CAMERA_BIONIC

TEST_C_INCLUDES:= $(LOCAL_PATH)
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../includes/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/bus/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/controller/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/object/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/includes/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/tools/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/event/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/pipeline/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/stream/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/module/
TEST_C_INCLUDES+= $(LOCAL_PATH)/../../../../../media-controller/mct/port/
TEST_C_INCLUDES+= \
 $(LOCAL_PATH)/../../../../../../../../../../hardware/qcom/camera/QCamera2/stack/common


#
# test_bet_8998
#
include $(CLEAR_VARS)
LOCAL_LDFLAGS := $(mmcamera_debug_lflags)

LOCAL_CFLAGS := $(TEST_CFLAGS)
LOCAL_CFLAGS  += -D_ANDROID_
LOCAL_CFLAGS += -Werror

LOCAL_MMCAMERA_PATH := $(LOCAL_PATH)/../../../../../

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/includes/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/server-tuning/tuning/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/includes/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_C_INCLUDES += $(LOCAL_CHROMATIX_PATH)
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/../../mm-camerasdk/sensor/includes/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp/

LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp/test/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/chromatix-wrapper/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/platform/linux
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/include
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/isp48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/sub_module
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/linearization/linearization40/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/pedestal_correct/pedestal_correct46/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/bpc/bpc48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/chroma_enhan/chroma_enhan40/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/chroma_suppress/chroma_suppress40/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/mce/mce40/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/sce/sce40/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/rolloff/mesh_rolloff44/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/gamma/gamma44/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/demosaic/demosaic48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/gamma/gamma44/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/gamma/adrc/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/cac/cac47/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/color_xform/color_xform46/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/black_level/black_level48/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/black_level/black_level48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/demux/demux48/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/demux/demux48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/color_correct/color_correct46/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/abf/abf48/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/snr/snr47/
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/gic/gic48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/hdr/hdr48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/hdr/hdr48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/gtm/gtm46/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/ltm/ltm47/include47
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/pdaf/pdaf48/include48
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/hw/ltm/adrc
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/common/
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/include/mm-camera-interface
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_C_INCLUDES += $(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common

LOCAL_HEADER_LIBRARIES := libutils_headers

ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif


LOCAL_SRC_FILES:= vfe_test_vector.c \
               ../test_bet.c \
               ../test_linearization44.c \
               ../test_chroma_enhan44.c \
               ../test_colorcorrect44.c \
               ../test_gamma44.c \
               ../test_clf46.c \
               ../test_chroma_suppress44.c \
               ../test_mce44.c \
               ../test_sce44.c \
               ../test_mesh_rolloff44.c \
               ../test_gic48.c \
               ../test_abf48.c \
               ../test_bpc48.c \
               ../test_pdaf48.c \
               ../test_black_level48.c \
               ../test_demux48.c \
               ../test_colorxform46.c \
               ../test_demosaic48.c \
               ../test_gtm46.c \
               ../test_hdr48.c \
               ../test_ltm47.c
ifeq ($(call is-board-platform-in-list,msmcobalt msm8998 apq8098_latv),true)
LOCAL_SRC_FILES +=../test_pedestal46.c \
               ../test_cac47.c \
               ../test_snr47.c
LOCAL_CFLAGS  += -DMSMCOBALT_MSM8998
endif

LOCAL_SHARED_LIBRARIES:= libdl libcutils liblog libmmcamera2_mct libmmcamera2_isp_modules libmmcamera_dbg

LOCAL_PROPRIETARY_MODULE := true

ifeq ($(call is-board-platform-in-list,msm8998 apq8098_latv),true)
LOCAL_MODULE:= test_bet_8998
else
LOCAL_MODULE:= test_bet_sdm660
endif
LOCAL_MODULE_TAGS := optional

# copy data files
$(shell cp -R $(LOCAL_PATH)/data/gain $(TARGET_OUT_ETC))
$(shell cp -R $(LOCAL_PATH)/data/lux $(TARGET_OUT_ETC))

ifeq ($(32_BIT_FLAG), true)
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)
#END
endif
