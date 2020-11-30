/* s5k4h7_lib.h
 *
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __S5K4H7_LIB_H__
#define __S5K4H7_LIB_H__

#include "sensor_lib.h"
#define SENSOR_MODEL "s5k4h7"

/* S5K4H7 CONSTANTS */
#define S5K4H7_MAX_INTEGRATION_MARGIN   8

#define S5K4H7_DATA_PEDESTAL            0x40 /* 10bit */

#define S5K4H7_MIN_AGAIN_REG_VAL        32  /* 1.0x */
#define S5K4H7_MAX_AGAIN_REG_VAL        512 /* 16.0x */

#define S5K4H7_MIN_DGAIN_REG_VAL        256 /* 1.0x */
#define S5K4H7_MAX_DGAIN_REG_VAL        256 /* 1.0x */

/* S5K4H7 FORMULAS */
#define S5K4H7_MIN_AGAIN    (S5K4H7_MIN_AGAIN_REG_VAL / 32)
#define S5K4H7_MAX_AGAIN    (S5K4H7_MAX_AGAIN_REG_VAL / 32)

#define S5K4H7_MIN_DGAIN    (S5K4H7_MIN_DGAIN_REG_VAL / 256)
#define S5K4H7_MAX_DGAIN    (S5K4H7_MAX_DGAIN_REG_VAL / 256)

#define S5K4H7_MIN_GAIN     S5K4H7_MIN_AGAIN * S5K4H7_MIN_DGAIN
#define S5K4H7_MAX_GAIN     S5K4H7_MAX_AGAIN * S5K4H7_MAX_DGAIN

int32_t s5k4h7_calculate_exposure(float real_gain,
  uint32_t line_count, sensor_exposure_info_t *exp_info, float s_real_gain);

int32_t s5k4h7_fill_exposure_array(uint32_t gain,
  __attribute__((unused)) uint32_t digital_gain, uint32_t line,
  uint32_t fl_lines,  __attribute__((unused)) int32_t luma_avg,
  __attribute__((unused)) uint32_t hdr_param,
  struct camera_i2c_reg_setting* reg_setting,
  __attribute__((unused)) unsigned int s_gain,
  __attribute__((unused)) int s_linecount,
  __attribute__((unused)) int is_hdr_enabled);

#define START_REG_ARRAY \
{ \
  {0x0100, 0x0100, 0x00}, \
}

#define STOP_REG_ARRAY \
{ \
  {0x0100, 0x0000, 0x00}, \
}

#define GROUPON_REG_ARRAY \
{ \
  {0x0104, 0x01, 0x00}, \
}

#define GROUPOFF_REG_ARRAY \
{ \
  {0x0104, 0x00, 0x00}, \
}

#define INIT0_REG_ARRAY \
{ \
   {0x0100, 0x00, 0x00}, \
   {0x0000, 0x12, 0x00}, \
   {0x0000, 0x48, 0x00}, \
   {0x0A02, 0x15, 0x00}, \
   {0x0B05, 0x01, 0x00}, \
   {0x3074, 0x06, 0x00}, \
   {0x3075, 0x2F, 0x00}, \
   {0x308A, 0x20, 0x00}, \
   {0x308B, 0x08, 0x00}, \
   {0x308C, 0x0B, 0x00}, \
   {0x3081, 0x07, 0x00}, \
   {0x307B, 0x85, 0x00}, \
   {0x307A, 0x0A, 0x00}, \
   {0x3079, 0x0A, 0x00}, \
   {0x306E, 0x71, 0x00}, \
   {0x306F, 0x28, 0x00}, \
   {0x301F, 0x20, 0x00}, \
   {0x306B, 0x9A, 0x00}, \
   {0x3091, 0x1F, 0x00}, \
   {0x30C4, 0x06, 0x00}, \
   {0x3200, 0x09, 0x00}, \
   {0x306A, 0x79, 0x00}, \
   {0x30B0, 0xFF, 0x00}, \
   {0x306D, 0x08, 0x00}, \
   {0x3080, 0x00, 0x00}, \
   {0x3929, 0x3F, 0x00}, \
   {0x3084, 0x16, 0x00}, \
   {0x3070, 0x0F, 0x00}, \
   {0x3B45, 0x01, 0x00}, \
   {0x30C2, 0x05, 0x00}, \
   {0x3069, 0x87, 0x00}, \
   {0x3924, 0x7F, 0x00}, \
   {0x3925, 0xFD, 0x00}, \
   {0x3C08, 0xFF, 0x00}, \
   {0x3C09, 0xFF, 0x00}, \
   {0x3C31, 0xFF, 0x00}, \
   {0x3C32, 0xFF, 0x00}, \
   {0x0b00, 0x01, 0x00}, \
}

#define RES0_REG_ARRAY \
{ \
   {0x0136, 0x18,0x00}, \
   {0x0137, 0x00,0x00}, \
   {0x0305, 0x06,0x00}, \
   {0x0306, 0x00,0x00}, \
   {0x0307, 0x8C,0x00}, \
   {0x030D, 0x06,0x00}, \
   {0x030E, 0x00,0x00}, \
   {0x030F, 0xE1,0x00}, \
   {0x3C1F, 0x00,0x00}, \
   {0x3C17, 0x00,0x00}, \
   {0x3C1C, 0x05,0x00}, \
   {0x3C1D, 0x15,0x00}, \
   {0x0301, 0x04,0x00}, \
   {0x0820, 0x03,0x00}, \
   {0x0821, 0x84,0x00}, \
   {0x0822, 0x00,0x00}, \
   {0x0823, 0x00,0x00}, \
   {0x0112, 0x0A,0x00}, \
   {0x0113, 0x0A,0x00}, \
   {0x0114, 0x01,0x00}, \
   {0x3906, 0x04,0x00}, \
   {0x0344, 0x00,0x00}, \
   {0x0345, 0x08,0x00}, \
   {0x0346, 0x00,0x00}, \
   {0x0347, 0x08,0x00}, \
   {0x0348, 0x0C,0x00}, \
   {0x0349, 0xC7,0x00}, \
   {0x034A, 0x09,0x00}, \
   {0x034B, 0x97,0x00}, \
   {0x034C, 0x0C,0x00}, \
   {0x034D, 0xC0,0x00}, \
   {0x034E, 0x09,0x00}, \
   {0x034F, 0x90,0x00}, \
   {0x0900, 0x00,0x00}, \
   {0x0901, 0x00,0x00}, \
   {0x0381, 0x01,0x00}, \
   {0x0383, 0x01,0x00}, \
   {0x0385, 0x01,0x00}, \
   {0x0387, 0x01,0x00}, \
   {0x0101, 0x00,0x00}, \
   {0x0340, 0x09,0x00}, \
   {0x0341, 0xE4,0x00}, \
   {0x0342, 0x14,0x00}, \
   {0x0343, 0x8C,0x00}, \
   {0x0200, 0x13,0x00}, \
   {0x0201, 0xFC,0x00}, \
   {0x0202, 0x00,0x00}, \
   {0x0203, 0x02,0x00}, \
   {0x3400, 0x01,0x00}, \
}

/* Sensor Handler */
static sensor_lib_t sensor_lib_ptr =
{
  .sensor_slave_info =
  {
    .sensor_name = SENSOR_MODEL,
    .slave_addr = 0x5a,
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .sensor_id_info =
    {
      .sensor_id_reg_addr = 0x0000,
      .sensor_id = 0x487B,
    },
    .power_setting_array =
    {
      .power_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_RESET,
          .config_val = GPIO_OUT_LOW,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_LOW,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VANA,
          .config_val = 1,
          .delay = 0,
        },
        {/*SC60 is different from SC66 on VDIG, which is controlled by vreg*/
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 1,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 24000000,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_RESET,
          .config_val = GPIO_OUT_HIGH,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_HIGH,
          .delay = 1,
        },
      },
      .size = 8,
      .power_down_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_LOW,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_RESET,
          .config_val = GPIO_OUT_LOW,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 0,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 0,
        },
        {/*SC60 is different from SC66 on VDIG, which is controlled by vreg*/
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VANA,
          .config_val = 0,
          .delay = 0,
        },
      },
      .size_down = 6,
    },
  },
  .sensor_output =
  {
    .output_format = SENSOR_BAYER,
    .connection_mode = SENSOR_MIPI_CSI,
    .raw_output = SENSOR_10_BIT_DIRECT,
    .filter_arrangement = SENSOR_GRBG,
  },
  .output_reg_addr =
  {
    .x_output = 0x034C,
    .y_output = 0x034E,
    .line_length_pclk = 0x0342,
    .frame_length_lines = 0x0340,
  },
  .exp_gain_info =
  {
    .coarse_int_time_addr = 0x0202,
    .global_gain_addr = 0x0204,
    .vert_offset = S5K4H7_MAX_INTEGRATION_MARGIN,
  },
  .aec_info =
  {
    .min_gain = S5K4H7_MIN_GAIN,
    .max_gain = S5K4H7_MAX_GAIN,
    .max_analog_gain = S5K4H7_MAX_AGAIN,
    .max_linecount = 65535 - S5K4H7_MAX_INTEGRATION_MARGIN,
  },
  .sensor_num_frame_skip = 2,
  .sensor_num_HDR_frame_skip = 2,
  .sensor_max_pipeline_frame_delay = 2,
  .sensor_property =
  {
    .pix_size = 1.12,
    .sensing_method = SENSOR_SMETHOD_ONE_CHIP_COLOR_AREA_SENSOR,
    .crop_factor = 1.0,
  },
  .pixel_array_size_info =
  {
    .active_array_size =
    {
      .width = 3264,
      .height = 2448,
    },
    .left_dummy = 8,
    .right_dummy = 8,
    .top_dummy = 8,
    .bottom_dummy = 8,
  },
  .color_level_info =
  {
    .white_level = 1023,
    .r_pedestal = S5K4H7_DATA_PEDESTAL,
    .gr_pedestal = S5K4H7_DATA_PEDESTAL,
    .gb_pedestal = S5K4H7_DATA_PEDESTAL,
    .b_pedestal = S5K4H7_DATA_PEDESTAL,
  },
  .sensor_stream_info_array =
  {
    .sensor_stream_info =
    {
      {
        .vc_cfg_size = 1,
        .vc_cfg =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
        .pix_data_fmt =
        {
          SENSOR_BAYER,
        },
      },
    },
    .size = 1,
  },
  .start_settings =
  {
    .reg_setting_a = START_REG_ARRAY,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .data_type = CAMERA_I2C_WORD_DATA,
    .delay = 0,
  },
  .stop_settings =
  {
    .reg_setting_a = STOP_REG_ARRAY,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .data_type = CAMERA_I2C_WORD_DATA,
    .delay = 0,
  },
  .groupon_settings =
  {
    .reg_setting_a = GROUPON_REG_ARRAY,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .groupoff_settings =
  {
    .reg_setting_a = GROUPOFF_REG_ARRAY,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .test_pattern_info =
  {
    .test_pattern_settings =
    {
      {
        .mode = SENSOR_TEST_PATTERN_OFF,
        .settings =
        {
          .reg_setting_a =
          {
            {0x6000, 0x0000, 0x00},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        }
      },
      {
        .mode = SENSOR_TEST_PATTERN_SOLID_COLOR,
        .settings =
        {
          .reg_setting_a =
          {
            {0x6000, 0x0001, 0x00},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        },
      },
      {
        .mode = SENSOR_TEST_PATTERN_COLOR_BARS,
        .settings =
        {
          .reg_setting_a =
          {
            {0x6000, 0x0002, 0x00},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        },
      },
      {
        .mode = SENSOR_TEST_PATTERN_COLOR_BARS_FADE_TO_GRAY,
        .settings =
        {
          .reg_setting_a =
          {
            {0x6000, 0x0003, 0x00},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        },
      },
      {
        .mode = SENSOR_TEST_PATTERN_PN9,
        .settings =
        {
          .reg_setting_a =
          {
            {0x6000, 0x0004, 0x00},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        },
      },
    },
    .size = 6,
    .solid_mode_addr =
    {
      .r_addr = 0x0602,
      .gr_addr = 0x0604,
      .gb_addr = 0x0608,
      .b_addr = 0x0606,
    },
  },
  .init_settings_array =
  {
    .reg_settings =
    {
      {
        .reg_setting_a = INIT0_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 0,
      },
    },
    .size = 1,
  },
  .res_settings_array =
  {
    .reg_settings =
    {
      /* Res 0 */
      {
        .reg_setting_a = RES0_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 0,
      },
    },
    .size = 1,
  },
  .out_info_array =
  {
    .out_info =
    {
      /* Res 0 */
      {
        .x_output = 3264,
        .y_output = 2448,
        .line_length_pclk = 5260,
        .frame_length_lines = 2532,
        .op_pixel_clk = 280000000,
        .binning_factor = 1,
        .min_fps = 7.500,
        .max_fps = 30,//actually only 21,for 2 lane mipi only support 21fps at fill size
        .mode = SENSOR_DEFAULT_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 1.000,
        .is_pdaf_supported = 0,
      },
    },
    .size = 1,
  },
  .csi_params =
  {
    .lane_cnt = 2,
    .settle_cnt = 0x14,
    .is_csi_3phase = 0,
  },
  .csid_lut_params_array =
  {
    .lut_params =
    {
      /* Res 0 */
      {
        .num_cid = 1,
        .vc_cfg_a =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
      },
      /* Res 1 */
      {
        .num_cid = 1,
        .vc_cfg_a =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
      },
      /* Res 2 */
      {
        .num_cid = 1,
        .vc_cfg_a =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
      },
      /* Res 3 */
      {
        .num_cid = 1,
        .vc_cfg_a =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
      },
      /* Res 4 */
      {
        .num_cid = 1,
        .vc_cfg_a =
        {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT
          },
        },
      },
    },
    .size = 1,
  },
  .crop_params_array =
  {
    .crop_params =
    {
      /* Res 0 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
      /* Res 1 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
      /* Res 2 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
      /* Res 3 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
      /* Res 4 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
    },
    .size = 1,
  },
  .exposure_func_table =
  {
    .sensor_calculate_exposure = s5k4h7_calculate_exposure,
    .sensor_fill_exposure_array = s5k4h7_fill_exposure_array,
  },
  .meta_data_out_info_array =
  {
    .meta_data_out_info =
    {
      {
        .width = 0,
        .height = 0,
        .stats_type = 0,
      },
    },
    .size = 0,
  },
  .sensor_capability = 0,
  .awb_func_table =
  {
    .sensor_fill_awb_array = 0,
    .awb_table_size = 0,
  },
  .rolloff_config =
  {
    .enable = FALSE,
    .full_size_info =
    {
      .full_size_width = 0,
      .full_size_height = 0,
      .full_size_left_crop = 0,
      .full_size_top_crop = 0,
    },
  },
  .adc_readout_time = 0,
  .sensor_num_fast_aec_frame_skip = 0,

  .noise_coeff = {
    .gradient_S = 3.738032e-06,
    .offset_S = 3.651935e-04,
    .gradient_O = 4.499952e-07,
    .offset_O = -2.968624e-04,
  },
};

#endif /* __S5K4H7_LIB_H__ */
