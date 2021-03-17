/* gc2385_lib.h
 *
 *Copyright (c) 2016 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GC2385_LIB_H__
#define __GC2385_LIB_H__

#include "sensor_lib.h"

#define SENSOR_MODEL "gc2385"
#define MAX_ANALOG_GAIN 8
/* SENSOR MIRROR FLIP INFO */
#define GC2385_MIRROR_FLIP_ENABLE    0
#if GC2385_MIRROR_FLIP_ENABLE
#define GC2385_MIRROR        0xd7
#define GC2385_STARTY        0x04
#define GC2385_STARTX        0x06
#define GC2385_BLK_Select_H  0x00
#define GC2385_BLK_Select_L  0x3c
#else
#define GC2385_MIRROR        0xd4
#define GC2385_STARTY        0x03
#define GC2385_STARTX        0x05
#define GC2385_BLK_Select_H  0x3c
#define GC2385_BLK_Select_L  0x00
#endif

#define ANALOG_GAIN_1 64  // 1.00x
#define ANALOG_GAIN_2 92  // 1.43x
#define ANALOG_GAIN_3 127 // 1.99x
#define ANALOG_GAIN_4 183 // 2.86x
#define ANALOG_GAIN_5 257 // 4.01x
#define ANALOG_GAIN_6 369 // 5.76x
#define ANALOG_GAIN_7 531 // 8.30x
#define ANALOG_GAIN_8 750 // 11.72x
#define ANALOG_GAIN_9 1092 // 17.06x

#define START_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
  {0xed, 0x90, 0x00}, \
  {0xfe, 0x00, 0x00}, \
}

#define STOP_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
  {0xed, 0x00, 0x00}, \
  {0xfe, 0x00, 0x00}, \
}

#define GROUPON_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
}

#define GROUPOFF_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
}

/*
03 04:6fc

*/
#define INIT0_REG_ARRAY \
{  \
    /*System*/ \
  {0xfe, 0x00, 0x00}, \
  {0xfe, 0x00, 0x00}, \
  {0xfe, 0x00, 0x00}, \
  {0xf2, 0x02, 0x00}, \
  {0xf4, 0x03, 0x00}, \
  {0xf7, 0x01, 0x00}, \
  {0xf8, 0x28, 0x00}, \
  {0xf9, 0x02, 0x00}, \
  {0xfa, 0x08, 0x00}, \
  {0xfc, 0x8e, 0x00}, \
  {0xe7, 0xcc, 0x00}, \
  {0x88, 0x03, 0x00}, \
  {0x03, 0x04, 0x00}, \
  {0x04, 0x80, 0x00}, \
  {0x05, 0x02, 0x00}, \
  {0x06, 0x86, 0x00}, \
  {0x07, 0x00, 0x00}, \
  {0x08, 0x20, 0x00}, \
  {0x09, 0x00, 0x00}, \
  {0x0a, 0x04, 0x00}, \
  {0x0b, 0x00, 0x00}, \
  {0x0c, 0x02, 0x00}, \
  {0x17, GC2385_MIRROR, 0x00}, \
  {0x18, 0x02, 0x00}, \
  {0x19, 0x17, 0x00}, \
  {0x1c, 0x18, 0x00}, \
  {0x20, 0x73, 0x00}, \
  {0x22, 0xa2, 0x00}, \
  {0x21, 0x38, 0x00}, \
  {0x29, 0x20, 0x00}, \
  {0x2f, 0x14, 0x00}, \
  {0x3f, 0x40, 0x00}, \
  {0xcd, 0x94, 0x00}, \
  {0xce, 0x45, 0x00}, \
  {0xd1, 0x0c, 0x00}, \
  {0xd7, 0x9b, 0x00}, \
  {0xd8, 0x99, 0x00}, \
  {0xda, 0x3b, 0x00}, \
  {0xd9, 0xb5, 0x00}, \
  {0xdb, 0x75, 0x00}, \
  {0xe3, 0x1b, 0x00}, \
  {0xe4, 0xf8, 0x00}, \
  {0x40, 0x22, 0x00}, \
  {0x43, 0x07, 0x00}, \
  {0x4e, GC2385_BLK_Select_H, 0x00}, \
  {0x4f, GC2385_BLK_Select_L, 0x00}, \
  {0x68, 0x00, 0x00}, \
  {0xb0, 0x46, 0x00}, \
  {0xb1, 0x01, 0x00}, \
  {0xb2, 0x00, 0x00}, \
  {0xb6, 0x00, 0x00}, \
  {0x90, 0x01, 0x00}, \
  {0x92, GC2385_STARTY, 0x00}, \
  {0x94, GC2385_STARTX, 0x00}, \
  {0x95, 0x04, 0x00}, \
  {0x96, 0xb0, 0x00}, \
  {0x97, 0x06, 0x00}, \
  {0x98, 0x40, 0x00}, \
  {0xfe, 0x00, 0x00}, \
  {0xed, 0x00, 0x00}, \
  {0xfe, 0x03, 0x00}, \
  {0x01, 0x03, 0x00}, \
  {0x02, 0x82, 0x00}, \
  {0x03, 0xd0, 0x00}, \
  {0x04, 0x04, 0x00}, \
  {0x05, 0x00, 0x00}, \
  {0x15, 0x02, 0x00}, \
  {0x06, 0x80, 0x00}, \
  {0x11, 0x2b, 0x00}, \
  {0x12, 0xd0, 0x00}, \
  {0x13, 0x07, 0x00}, \
  {0x15, 0x00, 0x00}, \
  {0x1b, 0x10, 0x00}, \
  {0x1c, 0x10, 0x00}, \
  {0x21, 0x08, 0x00}, \
  {0x22, 0x05, 0x00}, \
  {0x23, 0x13, 0x00}, \
  {0x24, 0x02, 0x00}, \
  {0x25, 0x13, 0x00}, \
  {0x26, 0x06, 0x00}, \
  {0x29, 0x06, 0x00}, \
  {0x2a, 0x08, 0x00}, \
  {0x2b, 0x06, 0x00}, \
  {0xfe, 0x00, 0x00}, \
}

#define RES0_REG_ARRAY \
{ \
  {0xfe,0x00, 0x00}, \
}

static sensor_lib_t sensor_lib_ptr =
{
  .sensor_slave_info =
  {
    .sensor_name = SENSOR_MODEL,
    .slave_addr = 0x6e,
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .sensor_id_info =
    {
      .sensor_id_reg_addr = 0xf0,
      .sensor_id = 0x2385,
    },
    .power_setting_array =
    {
      .power_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_LOW,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_RESET,
          .config_val = GPIO_OUT_LOW,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_VDIG,
          .config_val = GPIO_OUT_HIGH,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VANA,
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 24000000,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_HIGH,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_RESET,
          .config_val = GPIO_OUT_HIGH,
          .delay =1,
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
          .seq_val = CAMERA_VANA,
          .config_val = 0,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_VDIG,
          .config_val = GPIO_OUT_LOW,
          .delay = 0,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
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
    .filter_arrangement = SENSOR_BGGR,
  },
  .output_reg_addr =
  {
    .x_output = 0xff,
    .y_output = 0xff,
    .line_length_pclk = 0xff,
    .frame_length_lines = 0xff,
  },
  .exp_gain_info =
  {
    .coarse_int_time_addr = 0x03,
    .global_gain_addr = 0xb2,
    .vert_offset = 16,
  },
  .aec_info =
  {
  .max_gain = 8.0,
  .max_linecount = 9407, //9423-16
  },
  .sensor_num_frame_skip = 2,
  .sensor_num_HDR_frame_skip = 2,
  .sensor_max_pipeline_frame_delay = 2,
  .sensor_property =
  {
    .pix_size = 1.75,
    .sensing_method = SENSOR_SMETHOD_ONE_CHIP_COLOR_AREA_SENSOR,
    .crop_factor = 1.0,
  },
  .pixel_array_size_info =
  {
    .active_array_size =
    {
      .width = 1600,
      .height = 1200,
    },
    .left_dummy = 0,
    .right_dummy = 0,
    .top_dummy = 0,
    .bottom_dummy = 0,
  },
  .color_level_info =
  {
    .white_level = 1023,
    .r_pedestal = 64,
    .gr_pedestal = 64,
    .gb_pedestal = 64,
    .b_pedestal = 64,
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
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 1,
  },
  .stop_settings =
  {
    .reg_setting_a = STOP_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 1,
  },
  .groupon_settings =
  {
    .reg_setting_a = GROUPON_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .groupoff_settings =
  {
    .reg_setting_a = GROUPOFF_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .embedded_data_enable_settings =
  {
    .reg_setting_a = {},
    .addr_type = 0,
    .data_type = 0,
    .delay = 0,
  },
  .embedded_data_disable_settings =
  {
    .reg_setting_a = {},
    .addr_type = 0,
    .data_type = 0,
    .delay = 0,
  },
  .init_settings_array =
  {
    .reg_settings =
    {
      {
        .reg_setting_a = INIT0_REG_ARRAY,
        .addr_type = CAMERA_I2C_BYTE_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 60,
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
        .addr_type = CAMERA_I2C_BYTE_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 0,
      },
      /* Res 1 */
      /* Res 2 */
      /* Res 3 */
      /* Res 4 */
    },
    .size = 1,
  },
  .out_info_array =
  {
    .out_info =
    {
      /* For SNAPSHOT */
      {
        .x_output = 1600,
        .y_output = 1200,
        .line_length_pclk =   1079,
        .frame_length_lines = 1264, 
        .vt_pixel_clk = 41000000,
        .op_pixel_clk = 82000000,
        .binning_factor = 1,
        .max_fps = 30.0,
        .min_fps = 8,   
        .mode = SENSOR_DEFAULT_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .data_rate = 1200000000ULL, //if android version < 9.0, delete it
      },
     /* For PREVIEW */
    /* For 1280*960_60fps*/
     /* For 800*600_90fps*/
    /* For HFR_120fps*/
    },
    .size = 1,
  },
  .csi_params =
  {
    .lane_cnt = 1,
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
      /* Res 2 */
      /* Res 3 */
      /* Res 4 */
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
    },
    .size = 1,
  },
  .exposure_func_table =
  {
    .sensor_calculate_exposure = sensor_calculate_exposure,
    .sensor_fill_exposure_array = sensor_fill_exposure_array,
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
  .app_delay = {
    [SENSOR_DELAY_EXPOSURE] = 0,
    [SENSOR_DELAY_ANALOG_SENSOR_GAIN] = 0,
    [SENSOR_DELAY_DIGITAL_SENSOR_GAIN] = 0,

  },
  .noise_coeff = {
    .gradient_S = 3.738032e-06,
    .offset_S = 3.651935e-04,
    .gradient_O = 4.499952e-07,
    .offset_O = -2.968624e-04,
  },
};

#endif /* __gc2385_LIB_H__ */
