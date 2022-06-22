/* gc2053_lib.h
 *
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __GC2053_LIB_H__
#define __GC2053_LIB_H__

#include "sensor_lib.h"

#define   GC2053MinRegisterGain        64      ///< Minimum sensor register gain
#define   GC2053MaxRegisterGain        64*128    ///< Maximum sensor register gain
#define   GC2053MinRealGain            1.0	   ///< Minimum sensor real gain (1X)
#define   GC2053MaxRealGain            128.0	   ///< Maximum sensor real gain (32X)

#define SENSOR_MODEL "gc2053"

/*Mirror*/
#undef GC2053_MIRROR_NORMAL
#undef GC2053_MIRROR_H
#undef GC2053_MIRROR_V
#undef GC2053_MIRROR_HV
#define GC2053_MIRROR_HV
//#define GC2053_MIRROR_NORMAL

#if defined(GC2053_MIRROR_NORMAL)
#define GC2053_MIRROR        0x80
#elif defined(GC2053_MIRROR_H)
#define GC2053_MIRROR        0x81
#elif defined(GC2053_MIRROR_V)
#define GC2053_MIRROR        0x82
#elif defined(GC2053_MIRROR_HV)
#define GC2053_MIRROR        0x83
#else
#define GC2053_MIRROR        0x80
#endif
#define MIRROR GC2053_MIRROR


#define MAX_AG_INDEX           29
#define MEAG_INDEX             29
#define AGC_REG_NUM            4
#define MIN_EXP_LINE         4

uint16_t gain_level[MAX_AG_INDEX] = {
                               64 ,
                               74 ,
                               89 ,
                               102,
                               127,
                               147,
                               177,
                               203,
                               260,
                               300,
                               361,
                               415,
                               504,
                               581,
                               722,
                               832,
                               1027,
                               1182,
                               1408,
                               1621,
                               1990,
                               2291,
                               2850,
                               3282,
                               4048,
                               5180,
                               5500, 
                               6744, 
                               7073, 

};

uint8_t agc_register[MAX_AG_INDEX][AGC_REG_NUM] = {
	                            {0x00, 0x00,0x01,0x00},
                                {0x00, 0x10,0x01,0x0c},
                                {0x00, 0x20,0x01,0x1b},
                                {0x00, 0x30,0x01,0x2c},
                                {0x00, 0x40,0x01,0x3f},
                                {0x00, 0x50,0x02,0x16},
                                {0x00, 0x60,0x02,0x35},
                                {0x00, 0x70,0x03,0x16},
                                {0x00, 0x80,0x04,0x02},
                                {0x00, 0x90,0x04,0x31},
                                {0x00, 0xa0,0x05,0x32},
                                {0x00, 0xb0,0x06,0x35},
                                {0x00, 0xc0,0x08,0x04},
                                {0x00, 0x5a,0x09,0x19},
                                {0x00, 0x83,0x0b,0x0f},
                                {0x00, 0x93,0x0d,0x12},
                                {0x00, 0x84,0x10,0x00},
                                {0x00, 0x94,0x12,0x3a},
                                {0x01, 0x2c,0x1a,0x02},
                                {0x01, 0x3c,0x1b,0x20},
                                {0x00, 0x8c,0x20,0x0f},
                                {0x00, 0x9c,0x26,0x07},
                                {0x02, 0x64,0x36,0x21},
                                {0x02, 0x74,0x37,0x3a},
                                {0x00, 0xc6,0x3d,0x02},
                                {0x00, 0xdc,0x3f,0x3f},
                                {0x02, 0x85,0x3f,0x3f},
                                {0x02, 0x95,0x3f,0x3f},
                                {0x00, 0xce,0x3f,0x3f},

};
#define START_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
  {0x3e, 0x91, 0x00}, \
}

#define STOP_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
  {0x3e, 0x01, 0x00}, \
}

#define GROUPON_REG_ARRAY \
{ \
   {0xfe, 0x00, 0x00}, \
}

#define GROUPOFF_REG_ARRAY \
{ \
   {0xfe, 0x00, 0x00}, \
}

#define INIT0_REG_ARRAY \
{ \
/****system****/ \
{0xfe,0x80,0x00}, \
{0xfe,0x80,0x00}, \
{0xfe,0x80,0x00}, \
{0xfe,0x00,0x00}, \
{0xf2,0x00,0x00}, \
{0xf3,0x00,0x00}, \
{0xf4,0x36,0x00}, \
{0xf5,0xc0,0x00}, \
{0xf6,0x44,0x00}, \
{0xf7,0x01,0x00}, \
{0xf8,0x63,0x00}, \
{0xf9,0x40,0x00}, \
{0xfc,0x8e,0x00}, \
/****CISCTL & ANALOG****/ \
{0xfe,0x00,0x00}, \
{0x87,0x18,0x00}, \
{0xee,0x30,0x00}, \
{0xd0,0xb7,0x00}, \
{0x03,0x04,0x00}, \
{0x04,0x60,0x00}, \
{0x05,0x04,0x00}, \
{0x06,0x4c,0x00}, \
{0x07,0x00,0x00}, \
{0x08,0x11,0x00}, \
{0x09,0x00,0x00}, \
{0x0a,0x02,0x00}, \
{0x0b,0x00,0x00}, \
{0x0c,0x02,0x00}, \
{0x0d,0x04,0x00}, \
{0x0e,0x40,0x00}, \
{0x12,0xe2,0x00}, \
{0x13,0x16,0x00}, \
{0x17,MIRROR,0x00}, \
{0x19,0x0a,0x00}, \
{0x21,0x1c,0x00}, \
{0x28,0x0a,0x00}, \
{0x29,0x24,0x00}, \
{0x2b,0x04,0x00}, \
{0x32,0xf8,0x00}, \
{0x37,0x03,0x00}, \
{0x39,0x15,0x00}, \
{0x43,0x07,0x00}, \
{0x44,0x40,0x00}, \
{0x46,0x0b,0x00}, \
{0x4b,0x20,0x00}, \
{0x4e,0x08,0x00}, \
{0x55,0x20,0x00}, \
{0x66,0x05,0x00}, \
{0x67,0x05,0x00}, \
{0x77,0x01,0x00}, \
{0x78,0x00,0x00}, \
{0x7c,0x93,0x00}, \
{0x8c,0x12,0x00}, \
{0x8d,0x92,0x00}, \
{0x90,0x01,0x00}, \
{0x9d,0x10,0x00}, \
{0xce,0x7c,0x00}, \
{0xd2,0x41,0x00}, \
{0xd3,0xdc,0x00}, \
{0xe6,0x50,0x00}, \
/*gain*/ \
{0xb6,0xc0,0x00}, \
{0xb0,0x70,0x00}, \
{0xb1,0x01,0x00}, \
{0xb2,0x00,0x00}, \
{0xb3,0x00,0x00}, \
{0xb4,0x00,0x00}, \
{0xb8,0x01,0x00}, \
{0xb9,0x00,0x00}, \
/*blk*/ \
{0x26,0x30,0x00}, \
{0xfe,0x01,0x00}, \
{0x40,0x23,0x00}, \
{0x55,0x07,0x00}, \
{0x60,0x40,0x00}, \
{0xfe,0x04,0x00}, \
{0x14,0x78,0x00}, \
{0x15,0x78,0x00}, \
{0x16,0x78,0x00}, \
{0x17,0x78,0x00}, \
/*window*/ \
{0xfe,0x01,0x00}, \
{0x92,0x00,0x00}, \
{0x94,0x03,0x00}, \
{0x95,0x04,0x00}, \
{0x96,0x38,0x00}, \
{0x97,0x07,0x00}, \
{0x98,0x80,0x00}, \
/*ISP*/ \
{0xfe,0x01,0x00}, \
{0x01,0x05,0x00}, \
{0x02,0x89,0x00}, \
{0x04,0x01,0x00}, \
{0x07,0xa6,0x00}, \
{0x08,0xa9,0x00}, \
{0x09,0xa8,0x00}, \
{0x0a,0xa7,0x00}, \
{0x0b,0xff,0x00}, \
{0x0c,0xff,0x00}, \
{0x0f,0x00,0x00}, \
{0x50,0x1c,0x00}, \
{0x89,0x03,0x00}, \
{0xfe,0x04,0x00}, \
{0x28,0x86,0x00}, \
{0x29,0x86,0x00}, \
{0x2a,0x86,0x00}, \
{0x2b,0x68,0x00}, \
{0x2c,0x68,0x00}, \
{0x2d,0x68,0x00}, \
{0x2e,0x68,0x00}, \
{0x2f,0x68,0x00}, \
{0x30,0x4f,0x00}, \
{0x31,0x68,0x00}, \
{0x32,0x67,0x00}, \
{0x33,0x66,0x00}, \
{0x34,0x66,0x00}, \
{0x35,0x66,0x00}, \
{0x36,0x66,0x00}, \
{0x37,0x66,0x00}, \
{0x38,0x62,0x00}, \
{0x39,0x62,0x00}, \
{0x3a,0x62,0x00}, \
{0x3b,0x62,0x00}, \
{0x3c,0x62,0x00}, \
{0x3d,0x62,0x00}, \
{0x3e,0x62,0x00}, \
{0x3f,0x62,0x00}, \
/****DVP & MIPI****/ \
{0xfe,0x01,0x00}, \
{0x9a,0x06,0x00}, \
{0xfe,0x00,0x00}, \
{0x7b,0x2a,0x00}, \
{0x23,0x2d,0x00}, \
{0xfe,0x03,0x00}, \
{0x01,0x27,0x00}, \
{0x02,0x56,0x00}, \
{0x03,0xb6,0x00}, \
{0x12,0x80,0x00}, \
{0x13,0x07,0x00}, \
{0x15,0x10,0x00}, \
{0xfe,0x00,0x00}, \
{0x3e,0x01,0x00}, \
}

#define RES0_REG_ARRAY \
{ \
  {0xfe, 0x00, 0x00}, \
}

/* Sensor Handler */
static sensor_lib_t sensor_lib_ptr = {
  .sensor_slave_info = {
    .sensor_name = SENSOR_MODEL,
    .slave_addr = 0x6e, /*0x7e*/
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .sensor_id_info = {
      .sensor_id_reg_addr = 0xf0,
      .sensor_id = 0x2053,
    },
    .power_setting_array = {
      .power_setting_a = {
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
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VANA,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 24000000,
          .delay = 5,
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
          .delay = 1,
        },
      },
      .size = 8,
      .power_down_setting_a = {
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
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 0,
          .delay = 5,
        },
		{
		  .seq_type = CAMERA_POW_SEQ_VREG,
		  .seq_val = CAMERA_VANA,
		  .config_val = 0,
		  .delay = 5,
		},
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 5,
        },
      },
      .size_down = 6,
    },
  },
  .sensor_output = {
    .output_format = SENSOR_BAYER,
    .connection_mode = SENSOR_MIPI_CSI,
    .raw_output = SENSOR_10_BIT_DIRECT,
#if defined(GC2053_MIRROR_NORMAL)
    .filter_arrangement = SENSOR_RGGB,
#elif defined(GC2053_MIRROR_H)
    .filter_arrangement = SENSOR_GRBG,
#elif defined(GC2053_MIRROR_V)
    .filter_arrangement = SENSOR_GBRG,
#elif defined(GC2053_MIRROR_HV)
    .filter_arrangement = SENSOR_RGGB,
//    .filter_arrangement = SENSOR_BGGR,
      //.filter_arrangement = SENSOR_Y,
#else
    //.filter_arrangement = SENSOR_RGGB,
    .filter_arrangement = SENSOR_Y,
#endif
  },
  .output_reg_addr = {
    .x_output = 0x97,
    .y_output = 0x95,
    .line_length_pclk = 0xff,
    .frame_length_lines = 0x41,
  },
  .exp_gain_info = {
    .coarse_int_time_addr = 0x03,
    .global_gain_addr = 0xb2,
    .vert_offset = 16,
  },
  .aec_info = {
    .min_gain = GC2053MinRealGain,
    .max_gain = GC2053MaxRealGain,
    .max_analog_gain = GC2053MaxRealGain,
    .max_linecount = 16364,//16383-16
  },
  .sensor_num_frame_skip = 2,
  .sensor_num_HDR_frame_skip = 2,
  .sensor_max_pipeline_frame_delay = 2,
  .sensor_property = {
    .pix_size = 1.12,
    .sensing_method = SENSOR_SMETHOD_ONE_CHIP_COLOR_AREA_SENSOR,
    .crop_factor = 1.0,
  },
  .pixel_array_size_info = {
    .active_array_size = {
      .width = 1920,
      .height = 1080,
    },
    .left_dummy = 0,
    .right_dummy = 0,
    .top_dummy = 0,
    .bottom_dummy = 0,
  },
  .color_level_info = {
    .white_level = 1023,
    .r_pedestal = 64,
    .gr_pedestal = 64,
    .gb_pedestal = 64,
    .b_pedestal = 64,
  },
  .sensor_stream_info_array = {
    .sensor_stream_info = {
      {
        .vc_cfg_size = 1,
        .vc_cfg = {
          {
            .cid = 0,
            .dt = CSI_RAW10,
            .decode_format = CSI_DECODE_10BIT,
          },
        },
        .pix_data_fmt = {
          SENSOR_BAYER,
        },
      },
    },
    .size = 1,
  },
  .start_settings = {
    .reg_setting_a = START_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .stop_settings = {
    .reg_setting_a = STOP_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .groupon_settings = {
    .reg_setting_a = GROUPON_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .groupoff_settings = {
    .reg_setting_a = GROUPOFF_REG_ARRAY,
    .addr_type = CAMERA_I2C_BYTE_ADDR,
    .data_type = CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  .embedded_data_enable_settings = {
    .reg_setting_a = {},
    .addr_type = 0,
    .data_type = 0,
    .delay = 0,
  },
  .embedded_data_disable_settings = {
    .reg_setting_a = {},
    .addr_type = 0,
    .data_type = 0,
    .delay = 0,
  },
  .test_pattern_info = {
    .test_pattern_settings = {
      {
        .mode = SENSOR_TEST_PATTERN_OFF,
        .settings = {
          .reg_setting_a = {
            {0xfe, 0x01, 0x00},
            {0x8c, 0x10, 0x00},
            {0xfe, 0x00, 0x00},
          },
          .addr_type = CAMERA_I2C_BYTE_ADDR,
          .data_type = CAMERA_I2C_BYTE_DATA,
          .delay = 0,
        }
      },
      {
        .mode = SENSOR_TEST_PATTERN_SOLID_COLOR, //CUSTOM1
        .settings =
        {
          .reg_setting_a = {
            {0xfe, 0x01, 0x00},
            {0x8c, 0x11, 0x00},
            {0xfe, 0x00, 0x00},
          },
          .addr_type = CAMERA_I2C_BYTE_ADDR,
          .data_type = CAMERA_I2C_BYTE_DATA,
          .delay = 0,
        },
      },
    },
    .size = 2,
    .solid_mode_addr = {
      .r_addr = 0,
      .gr_addr = 0,
      .gb_addr = 0,
      .b_addr = 0,
    },
  },
  .init_settings_array = {
    .reg_settings = {
      {
        .reg_setting_a = INIT0_REG_ARRAY,
        .addr_type = CAMERA_I2C_BYTE_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 0,
      },
    },
    .size = 1,
  },
  .res_settings_array = {
    .reg_settings = {
      /* Res 0 */
      {
        .reg_setting_a = RES0_REG_ARRAY,
        .addr_type = CAMERA_I2C_BYTE_ADDR,
        .data_type = CAMERA_I2C_BYTE_DATA,
        .delay = 0,
      },
    },
    .size = 1,
  },
  .out_info_array = {
    .out_info = {
      /* Res 0 */
      {
        .x_output = 1920,
        .y_output = 1080,
        .line_length_pclk = 2200,
        .frame_length_lines = 1125,
        .vt_pixel_clk = 148500000,
        .op_pixel_clk = 148500000,
        .binning_factor = 1,
        .min_fps = 8.0,
        .max_fps = 30,
        .mode = SENSOR_DEFAULT_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 1,
      },
    },
    .size = 1,
  },
  .csi_params = {
    .lane_cnt = 2,
    .settle_cnt = 0x14,
    .is_csi_3phase = 0,
  },
  .csid_lut_params_array = {
    .lut_params = {
      /* Res 0 */
      {
        .num_cid = 1,
        .vc_cfg_a = {
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
  .crop_params_array = {
    .crop_params = {
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
  .exposure_func_table = {
    .sensor_calculate_exposure = sensor_calculate_exposure,
    .sensor_fill_exposure_array = sensor_fill_exposure_array,
  },
  .meta_data_out_info_array = {
    .meta_data_out_info = {
      {
        .width = 0,
        .height = 0,
        .stats_type = 0,
      },
    },
    .size = 0,
  },
  .rolloff_config = {
    .enable = FALSE,
    .full_size_info = {
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


#endif /* __gc8034_LIB_H__ */
