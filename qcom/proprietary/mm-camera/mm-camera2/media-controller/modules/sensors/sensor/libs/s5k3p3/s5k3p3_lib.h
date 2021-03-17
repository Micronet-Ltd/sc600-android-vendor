/* s5k3p3_lib.h
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __s5k3p3_LIB_H__
#define __s5k3p3_LIB_H__

#include "sensor_lib.h"
#define SENSOR_MODEL "s5k3p3"

/* s5k3p3 Regs */
#define s5k3p3_DIG_GAIN_GR_ADDR       0x020E /* 0x4000020E */
#define s5k3p3_DIG_GAIN_R_ADDR        0x0210 /* 0x40000210 */
#define s5k3p3_DIG_GAIN_B_ADDR        0x0212 /* 0x40000212 */
#define s5k3p3_DIG_GAIN_GB_ADDR       0x0214 /* 0x40000214 */

#define s5k3p3_DATA_PEDESTAL            0x40 /* 10bit */

#define s5k3p3_MIN_DGAIN_REG_VAL        256 /* 1.0x */
#define s5k3p3_MAX_DGAIN_REG_VAL        256 /* 1.0x */

/* s5k3p3 FORMULAS */
#define s5k3p3_MIN_DGAIN    (s5k3p3_MIN_DGAIN_REG_VAL / 256)
#define s5k3p3_MAX_DGAIN    (s5k3p3_MAX_DGAIN_REG_VAL / 256)

int s5k3p3_calculate_exposure(float real_gain,
  unsigned int line_count, sensor_exposure_info_t *exp_info, float s_real_gain);


int s5k3p3_fill_exposure_array(unsigned int gain,
  __attribute__((unused)) unsigned int digital_gain, unsigned int line,
  unsigned int fl_lines,  __attribute__((unused)) int luma_avg,
  __attribute__((unused)) unsigned int hdr_param,
  struct camera_i2c_reg_setting* reg_setting,
  __attribute__((unused)) unsigned int s_gain,
  __attribute__((unused)) int s_linecount,
  __attribute__((unused)) int is_hdr_enabled);

/* uncomment FLIP_MIRROR macro to
   enable flip and mirror in sensor readout
   change bayer pattern
   load pdaf flip and mirror header
*/
//#define FLIP_MIRROR

//#define DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE

#ifndef FLIP_MIRROR
#define START_REG_ARRAY \
{ \
  {0x0100, 0x0100, 0x00}, \
} 
#else
#define START_REG_ARRAY \
{ \
  {0x0100, 0x0103, 0x00}, \
}
#endif

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
  {0x6028, 0x4000, 0x0000}, \
  {0x6214, 0x7971, 0x0000}, \
  {0x6218, 0x0100, 0x0000}, \
  {0x6010, 0x0001, 0x0bb8}, \
  {0x6214, 0x7970, 0x0000}, \
  {0x6218, 0x0100, 0x0000}, \
  {0x3D7C, 0x1110, 0x0000}, \
  {0x3D88, 0x0064, 0x0000}, \
  {0x3D8A, 0x0068, 0x0000}, \
  {0xF408, 0x0048, 0x0000}, \
  {0xF40C, 0x0000, 0x0000}, \
  {0xF4AA, 0x0060, 0x0000}, \
  {0xF442, 0x0800, 0x0000}, \
  {0xF43E, 0x2020, 0x0000}, \
  {0xF440, 0x0000, 0x0000}, \
  {0xF4AC, 0x004B, 0x0000}, \
  {0xF492, 0x0016, 0x0000}, \
  {0xF480, 0x0040, 0x0000}, \
  {0xF4A4, 0x0010, 0x0000}, \
  {0x3E58, 0x004B, 0x0000}, \
  {0x3A38, 0x006C, 0x0000}, \
  {0x3552, 0x00D0, 0x0000}, \
  {0x3CD6, 0x0100, 0x0000}, \
  {0x3CD8, 0x017F, 0x0000}, \
  {0x3CDA, 0x1000, 0x0000}, \
  {0x3CDC, 0x104F, 0x0000}, \
  {0x3CDE, 0x0180, 0x0000}, \
  {0x3CE0, 0x01FF, 0x0000}, \
  {0x3CE2, 0x104F, 0x0000}, \
  {0x3CE4, 0x104F, 0x0000}, \
  {0x3CE6, 0x0200, 0x0000}, \
  {0x3CE8, 0x03FF, 0x0000}, \
  {0x3CEA, 0x104F, 0x0000}, \
  {0x3CEC, 0x1058, 0x0000}, \
  {0x3CEE, 0x0400, 0x0000}, \
  {0x3CF0, 0x07FF, 0x0000}, \
  {0x3CF2, 0x1057, 0x0000}, \
  {0x3CF4, 0x1073, 0x0000}, \
  {0x3CF6, 0x0800, 0x0000}, \
  {0x3CF8, 0x1000, 0x0000}, \
  {0x3CFA, 0x1073, 0x0000}, \
  {0x3CFC, 0x10A2, 0x0000}, \
  {0x3D16, 0x0100, 0x0000}, \
  {0x3D18, 0x017F, 0x0000}, \
  {0x3D1A, 0x1000, 0x0000}, \
  {0x3D1C, 0x104F, 0x0000}, \
  {0x3D1E, 0x0180, 0x0000}, \
  {0x3D20, 0x01FF, 0x0000}, \
  {0x3D22, 0x104F, 0x0000}, \
  {0x3D24, 0x104F, 0x0000}, \
  {0x3D26, 0x0200, 0x0000}, \
  {0x3D28, 0x03FF, 0x0000}, \
  {0x3D2A, 0x104F, 0x0000}, \
  {0x3D2C, 0x1058, 0x0000}, \
  {0x3D2E, 0x0400, 0x0000}, \
  {0x3D30, 0x07FF, 0x0000}, \
  {0x3D32, 0x1057, 0x0000}, \
  {0x3D34, 0x1073, 0x0000}, \
  {0x3D36, 0x0800, 0x0000}, \
  {0x3D38, 0x1000, 0x0000}, \
  {0x3D3A, 0x1073, 0x0000}, \
  {0x3D3C, 0x10A2, 0x0000}, \
  {0x3002, 0x0001, 0x0000}, \
  {0x0216, 0x0000, 0x0000}, \
  {0x021B, 0x0100, 0x0000}, \
  {0x0202, 0x0100, 0x0000}, \
  {0x0200, 0x0200, 0x0000}, \
  {0x021E, 0x0100, 0x0000}, \
  {0x021C, 0x0200, 0x0000}, \
  {0x3072, 0x03C0, 0x0000}, \
  {0x6028, 0x4000, 0x0000}, \
  {0x0B08, 0x0000, 0x0000}, \
  {0x3058, 0x0001, 0x0000}, \
  {0x0B0E, 0x0100, 0x0000}, \
  {0x316C, 0x0084, 0x0000}, \
  {0x316E, 0x1283, 0x0000}, \
  {0x3170, 0x0060, 0x0000}, \
  {0x3172, 0x0DDF, 0x0000}, \
  {0x3D66, 0x0010, 0x0000}, \
  {0x3D68, 0x1004, 0x0000}, \
  {0x3D6A, 0x0404, 0x0000}, \
  {0x3D6C, 0x0704, 0x0000}, \
  {0x3D6E, 0x0B08, 0x0000}, \
  {0x3D70, 0x0708, 0x0000}, \
  {0x3D72, 0x0B08, 0x0000}, \
  {0x3D74, 0x0B08, 0x0000}, \
  {0x3D76, 0x0F00, 0x0000}, \
  {0x9920, 0x0104, 0x0000}, \
  {0x9928, 0x03CB, 0x0000}, \
  {0x3D78, 0x396C, 0x0000}, \
  {0x3D7A, 0x93C6, 0x0000}, \
}

#define RES0_REG_ARRAY \
{ \
  {0x6028, 0x2000, 0x0000}, \
  {0x602A, 0x2E26, 0x0000}, \
  {0x6F12, 0x0103, 0x0000}, \
  {0x6028, 0x4000, 0x0000}, \
  {0x0344, 0x0008, 0x0000}, \
  {0x0346, 0x0008, 0x0000}, \
  {0x0348, 0x121F, 0x0000}, \
  {0x034A, 0x0D9F, 0x0000}, \
  {0x034C, 0x1218, 0x0000}, \
  {0x034E, 0x0D98, 0x0000}, \
  {0x0900, 0x0011, 0x0000}, \
  {0x0380, 0x0001, 0x0000}, \
  {0x0382, 0x0001, 0x0000}, \
  {0x0384, 0x0001, 0x0000}, \
  {0x0386, 0x0001, 0x0000}, \
  {0x0400, 0x0000, 0x0000}, \
  {0x0404, 0x0010, 0x0000}, \
  {0x0114, 0x0300, 0x0000}, \
  {0x0110, 0x0002, 0x0000}, \
  {0x0136, 0x1800, 0x0000}, \
  {0x0304, 0x0006, 0x0000}, \
  {0x0306, 0x008C, 0x0000}, \
  {0x0302, 0x0001, 0x0000}, \
  {0x0300, 0x0004, 0x0000}, \
  {0x030C, 0x0004, 0x0000}, \
  {0x030E, 0x0074, 0x0000}, \
  {0x030A, 0x0001, 0x0000}, \
  {0x0308, 0x0008, 0x0000}, \
  {0x0342, 0x141C, 0x0000}, \
  {0x0340, 0x0E2A, 0x0000}, \
  {0x0B0E, 0x0000, 0x0000}, \
}

#define RES1_REG_ARRAY \
{ \
  {0x6028, 0x2000, 0x0000}, \
  {0x602A, 0x2E26, 0x0000}, \
  {0x6F12, 0x0103, 0x0000}, \
  {0x6028, 0x4000, 0x0000}, \
  {0x0344, 0x0008, 0x0000}, \
  {0x0346, 0x0008, 0x0000}, \
  {0x0348, 0x121F, 0x0000}, \
  {0x034A, 0x0D9F, 0x0000}, \
  {0x034C, 0x090C, 0x0000}, \
  {0x034E, 0x06CC, 0x0000}, \
  {0x0900, 0x0122, 0x0000}, \
  {0x0380, 0x0001, 0x0000}, \
  {0x0382, 0x0003, 0x0000}, \
  {0x0384, 0x0001, 0x0000}, \
  {0x0386, 0x0003, 0x0000}, \
  {0x0400, 0x0001, 0x0000}, \
  {0x0404, 0x0010, 0x0000}, \
  {0x0114, 0x0300, 0x0000}, \
  {0x0110, 0x0002, 0x0000}, \
  {0x0136, 0x1800, 0x0000}, \
  {0x0304, 0x0006, 0x0000}, \
  {0x0306, 0x008C, 0x0000}, \
  {0x0302, 0x0001, 0x0000}, \
  {0x0300, 0x0004, 0x0000}, \
  {0x030C, 0x0004, 0x0000}, \
  {0x030E, 0x0074, 0x0000}, \
  {0x030A, 0x0001, 0x0000}, \
  {0x0308, 0x0008, 0x0000}, \
  {0x0342, 0x141C, 0x0000}, \
  {0x0340, 0x0E2A, 0x0000}, \
  {0x0B0E, 0x0100, 0x0000}, \
}

#define RES2_REG_ARRAY \
{ \
  {0x6028, 0x2000, 0x0000}, \
  {0x602A, 0x2E26, 0x0000}, \
  {0x6F12, 0x0103, 0x0000}, \
  {0x6028, 0x4000, 0x0000}, \
  {0x3D7C, 0x0010, 0x0000}, \
  {0x3D88, 0x0064, 0x0000}, \
  {0x3D8A, 0x0068, 0x0000}, \
  {0x0344, 0x0000, 0x0000}, \
  {0x0346, 0x0008, 0x0000}, \
  {0x0348, 0x1227, 0x0000}, \
  {0x034A, 0x0D9F, 0x0000}, \
  {0x034C, 0x090C, 0x0000}, \
  {0x034E, 0x06CC, 0x0000}, \
  {0x0408, 0x0004, 0x0000}, \
  {0x0900, 0x0122, 0x0000}, \
  {0x0380, 0x0001, 0x0000}, \
  {0x0382, 0x0003, 0x0000}, \
  {0x0384, 0x0001, 0x0000}, \
  {0x0386, 0x0003, 0x0000}, \
  {0x0400, 0x0001, 0x0000}, \
  {0x0404, 0x0010, 0x0000}, \
  {0x0114, 0x0300, 0x0000}, \
  {0x0110, 0x0002, 0x0000}, \
  {0x0136, 0x1800, 0x0000}, \
  {0x0304, 0x0006, 0x0000}, \
  {0x0306, 0x008C, 0x0000}, \
  {0x0302, 0x0001, 0x0000}, \
  {0x0300, 0x0004, 0x0000}, \
  {0x030C, 0x0004, 0x0000}, \
  {0x030E, 0x0037, 0x0000}, \
  {0x030A, 0x0001, 0x0000}, \
  {0x0308, 0x0008, 0x0000}, \
  {0x0342, 0x14A2, 0x0000}, \
  {0x0340, 0x06E7, 0x0000}, \
  {0x0B0E, 0x0100, 0x0000}, \
  {0x0216, 0x0000, 0x0000}, \
}

#define RES3_REG_ARRAY \
{ \
  {0x6028, 0x2000, 0x0000}, \
  {0x602A, 0x2E26, 0x0000}, \
  {0x6F12, 0x0103, 0x0000}, \
  {0x6028, 0x4000, 0x0000}, \
  {0x3D7C, 0x0010, 0x0000}, \
  {0x3D88, 0x0064, 0x0000}, \
  {0x3D8A, 0x0068, 0x0000}, \
  {0x0344, 0x0000, 0x0000}, \
  {0x0346, 0x0008, 0x0000}, \
  {0x0348, 0x1227, 0x0000}, \
  {0x034A, 0x0D9F, 0x0000}, \
  {0x034C, 0x0480, 0x0000}, \
  {0x034E, 0x0366, 0x0000}, \
  {0x0408, 0x0002, 0x0000}, \
  {0x0900, 0x0144, 0x0000}, \
  {0x0380, 0x0001, 0x0000}, \
  {0x0382, 0x0007, 0x0000}, \
  {0x0384, 0x0001, 0x0000}, \
  {0x0386, 0x0007, 0x0000}, \
  {0x0400, 0x0001, 0x0000}, \
  {0x0404, 0x0010, 0x0000}, \
  {0x0114, 0x0300, 0x0000}, \
  {0x0110, 0x0002, 0x0000}, \
  {0x0136, 0x1800, 0x0000}, \
  {0x0304, 0x0006, 0x0000}, \
  {0x0306, 0x008C, 0x0000}, \
  {0x0302, 0x0001, 0x0000}, \
  {0x0300, 0x0004, 0x0000}, \
  {0x030C, 0x0004, 0x0000}, \
  {0x030E, 0x0037, 0x0000}, \
  {0x030A, 0x0001, 0x0000}, \
  {0x0308, 0x0008, 0x0000}, \
  {0x0342, 0x141C, 0x0000}, \
  {0x0340, 0x04B8, 0x0000}, \
  {0x0B0E, 0x0100, 0x0000}, \
  {0x0216, 0x0000, 0x0000}, \
}

#define RES4_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x0000}, \
  {0x0344, 0x0008, 0x0000}, \
  {0x0346, 0x0008, 0x0000}, \
  {0x0348, 0x1207, 0x0000}, \
  {0x034A, 0x0D9F, 0x0000}, \
  {0x034C, 0x0480, 0x0000}, \
  {0x034E, 0x0366, 0x0000}, \
  {0x0900, 0x0144, 0x0000}, \
  {0x0380, 0x0001, 0x0000}, \
  {0x0382, 0x0007, 0x0000}, \
  {0x0384, 0x0001, 0x0000}, \
  {0x0386, 0x0007, 0x0000}, \
  {0x0400, 0x0001, 0x0000}, \
  {0x0404, 0x0010, 0x0000}, \
  {0x0114, 0x0300, 0x0000}, \
  {0x0110, 0x0002, 0x0000}, \
  {0x0136, 0x1800, 0x0000}, \
  {0x0304, 0x0006, 0x0000}, \
  {0x0306, 0x008D, 0x0000}, \
  {0x0302, 0x0001, 0x0000}, \
  {0x0300, 0x0004, 0x0000}, \
  {0x030C, 0x0004, 0x0000}, \
  {0x030E, 0x0037, 0x0000}, \
  {0x030A, 0x0001, 0x0000}, \
  {0x0308, 0x0008, 0x0000}, \
  {0x0342, 0x141C, 0x0000}, \
  {0x0340, 0x0388, 0x0000}, \
  {0x0B0E, 0x0100, 0x0000}, \
}

//add by allenyao
/*#define RES0_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x141C, 0x00}, \
  {0x0340, 0x0E2A, 0x00}, \
  {0x034c, 0x0000, 0x00}, \
}
#define RES0_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x141C, 0x00}, \
  {0x0340, 0x0E2A, 0x00}, \
  {0x304c, 0x0000, 0x00}, \
}
#define RES1_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x141C, 0x00}, \
  {0x0340, 0x0E2A, 0x00}, \
  {0x304c, 0x0000, 0x00}, \
}
#define RES2_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x14A2, 0x00}, \
  {0x0340, 0x06E7, 0x00}, \
  {0x304c, 0x0000, 0x00}, \
}
#define RES3_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x141C, 0x00}, \
  {0x0340, 0x04B8, 0x00}, \
  {0x304c, 0x0000, 0x00}, \
}
#define RES4_EXPOSE_REG_ARRAY \
{ \
  {0x6028, 0x4000, 0x00}, \
  {0x305a, 0x0000, 0x00}, \
  {0x0342, 0x141C, 0x00}, \
  {0x0340, 0x0388, 0x00}, \
  {0x304c, 0x0000, 0x00}, \
}
*/
/* Sensor Handler */
static sensor_lib_t sensor_lib_ptr =
{
  .sensor_slave_info =
  {
    .sensor_name = SENSOR_MODEL,
    .slave_addr = 0x20,
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .addr_type = CAMERA_I2C_WORD_ADDR,
    .sensor_id_info =
    {
      .sensor_id_reg_addr = 0x0000,
      .sensor_id = 0x3103,
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
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 1,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_VDIG,
          .config_val = GPIO_OUT_HIGH,
          .delay = 5,
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
          .delay = 10,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_STANDBY,
          .config_val = GPIO_OUT_HIGH,
          .delay = 20,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VAF,
          .config_val = 0,
          .delay = 5,
        },
      },
      .size = 10,
      .power_down_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VAF,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_CLK,
          .seq_val = CAMERA_MCLK,
          .config_val = 0,
          .delay = 5,
        },
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
          .seq_val = CAMERA_VDIG,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_GPIO,
          .seq_val = CAMERA_GPIO_VDIG,
          .config_val = GPIO_OUT_LOW,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 5,
        },
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VANA,
          .config_val = 0,
          .delay = 5,
        },
      },
      .size_down = 8,
    },
  },
  .sensor_output =
  {
    .output_format = SENSOR_BAYER,
    .connection_mode = SENSOR_MIPI_CSI,
    .raw_output = SENSOR_10_BIT_DIRECT,
#ifndef FLIP_MIRROR
    .filter_arrangement = SENSOR_GRBG,
#else
    .filter_arrangement = SENSOR_GBRG,
#endif
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
    .vert_offset = 8,
  },
  .aec_info =
  {
    .min_gain = 1.0,
    .max_gain = 16.0,
    .max_analog_gain = 16.0,
    //.max_linecount = 65535 - 4,
    .max_linecount = 108780*40,//30s
  },
  .sensor_num_frame_skip = 2,
  .sensor_num_HDR_frame_skip = 1,
  .sensor_max_pipeline_frame_delay = 2,
  .sensor_property =
  {
    .pix_size = 1.12,
    .sensing_method = SENSOR_SMETHOD_ONE_CHIP_COLOR_AREA_SENSOR,
    .crop_factor = 1.33,
  },
  .pixel_array_size_info =
  {
    .active_array_size =
    {
      .width = 4632,
      .height = 3480,
    },
    .left_dummy = 8,
    .right_dummy = 8,
    .top_dummy = 8,
    .bottom_dummy = 8,
  },
  .color_level_info =
  {
    .white_level = 1023,
    .r_pedestal = s5k3p3_DATA_PEDESTAL,
    .gr_pedestal = s5k3p3_DATA_PEDESTAL,
    .gb_pedestal = s5k3p3_DATA_PEDESTAL,
    .b_pedestal = s5k3p3_DATA_PEDESTAL,
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
            {0x6028, 0x4000, 0x00},
            {0x602A, 0x0600, 0x00},
            {0x6F12, 0x0000, 0x00},
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
            {0x6028, 0x4000, 0x0000},
            {0x602A, 0x0600, 0x0000},
            {0x6F12, 0x0001, 0x0000},
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
            {0x6028, 0x4000, 0x0000},
            {0x602A, 0x0600, 0x0000},
            {0x6F12, 0x0002, 0x0000},
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
            {0x6028, 0x4000, 0x0000},
            {0x602A, 0x0600, 0x0000},
            {0x6F12, 0x0003, 0x0000},
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
            {0x6028, 0x4000, 0x0000},
            {0x602A, 0x0600, 0x0000},
            {0x6F12, 0x0004, 0x0000},
          },
          .addr_type = CAMERA_I2C_WORD_ADDR,
          .data_type = CAMERA_I2C_WORD_DATA,
          .delay = 0,
        },
      },
      {
        .mode = SENSOR_TEST_PATTERN_CUSTOM1, /* Macbeth */
        .settings =
        {
          .reg_setting_a =
          {
            {0x6028, 0x4000, 0x0000},
            {0x602A, 0x0600, 0x0000},
            {0x6F12, 0x0100, 0x0000},
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
      .gb_addr = 0x0606,
      .b_addr = 0x0608,
    },
  },
  .init_settings_array =
  {
    .reg_settings =
    {
      {
        .reg_setting_a = INIT0_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
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
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
      },
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
      /* Res 1 */
      {
        .reg_setting_a = RES1_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
      },
#endif
      /* Res 2 */
      {
        .reg_setting_a = RES2_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
      },
      /* Res 3 */
      {
        .reg_setting_a = RES3_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
      },
      /* Res 4 */
      {
        .reg_setting_a = RES4_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
      },
    },
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
    .size = 5,
#else
    .size = 4, 
#endif
  },
#if 0
  //add by allenyao
    .res_expose_settings_array =
  {
      /* Res 0 */
      {
        .reg_setting_a = RES0_EXPOSE_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
        .size=5, 
      },
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
      /* Res 1 */
      {
        .reg_setting_a = RES1_EXPOSE_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA, 
        .delay = 0,
        .size=5,
      },
#endif
      /* Res 2 */
      {
        .reg_setting_a = RES2_EXPOSE_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
        .size=5,
      },
      /* Res 3 */
      {
        .reg_setting_a = RES3_EXPOSE_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
        .size=5,
      },
      /* Res 4 */
      {
        .reg_setting_a = RES4_EXPOSE_REG_ARRAY,
        .addr_type = CAMERA_I2C_WORD_ADDR,
        .data_type = CAMERA_I2C_WORD_DATA,
        .delay = 0,
        .size=5,
      },
  },
  .long_expose_flag=1,//add by allenyao
  //end
#endif
  .out_info_array =
  {
    .out_info =
    {
      /* Res 0 */
      {
        .x_output = 4632,
        .y_output = 3480,
        .line_length_pclk = 5148,
        .frame_length_lines = 3626,
        .vt_pixel_clk = 560000000,
        .op_pixel_clk = 556800000,
        .binning_factor = 1,
        .min_fps = 4,//7.5
        .max_fps = 30.1,
        .mode = SENSOR_DEFAULT_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .is_pdaf_supported = 0,//when the pdaf cal data is not aready ,not use pdaf;or ,will cannot come into snashot camera,
        					//and the third camera apk cannot capture
      },
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
      /* Res 1 */
      {
        .x_output = 2316,
        .y_output = 1740,
        .line_length_pclk = 5148,
        .frame_length_lines = 3626,
        .vt_pixel_clk = 560000000,
        .op_pixel_clk = 556800000,
        .binning_factor = 1,
        .min_fps = 7.5,
        .max_fps = 30.1,
        .mode = SENSOR_DEFAULT_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .is_pdaf_supported = 0,
      },
#endif
      /* Res 2 */
      {
        .x_output = 2316,
        .y_output = 1740,
        .line_length_pclk = 5282,
        .frame_length_lines = 1767,
        .vt_pixel_clk = 560000000,
        .op_pixel_clk = 264000000,
        .binning_factor = 1,
        .min_fps = 7.50,
        .max_fps = 60.0,
        .mode = SENSOR_HFR_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .is_pdaf_supported = 0,
      },
      /* Res 3 */
      {
        .x_output = 1152,
        .y_output = 870,
        .line_length_pclk = 5282,
        .frame_length_lines = 1767,
        .vt_pixel_clk = 560000000,
        .op_pixel_clk = 264000000,
        .binning_factor = 1,
        .min_fps = 7.5,
        .max_fps = 90.0,
        .mode = SENSOR_HFR_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .is_pdaf_supported = 0,
      },
      /* Res 4 */
      {
        .x_output = 1152,
        .y_output = 870,
        .line_length_pclk = 5282,
        .frame_length_lines = 1767,
        .vt_pixel_clk = 564000000,
        .op_pixel_clk = 264000000,
        .binning_factor = 1,
        .min_fps = 7.5,
        .max_fps = 120.0,
        .mode = SENSOR_HFR_MODE,
        .offset_x = 0,
        .offset_y = 0,
        .scale_factor = 0,
        .is_pdaf_supported = 0,
      },
    },
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
    .size = 5,
#else
    .size = 4,
#endif
  },
  .csi_params =
  {
    .lane_cnt = 4,
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
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
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
#endif
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
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
    .size = 5,
#else
    .size = 4,
#endif
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
#ifndef DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
      /* Res 1 */
      {
        .top_crop = 0,
        .bottom_crop = 0,
        .left_crop = 0,
        .right_crop = 0,
      },
#endif
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
#ifndef  DISABLE_RES1_TO_USE_PDAF_IN_VIDEO_OR_NOZSL_MODE
    .size = 5,
#else
    .size = 4,
#endif
  },
  .exposure_func_table =
  {
    .sensor_calculate_exposure = s5k3p3_calculate_exposure,
    .sensor_fill_exposure_array = s5k3p3_fill_exposure_array,
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
  .pdaf_config = {
#ifndef FLIP_MIRROR
    #include "s5k3p3_pdaf.h"
#else
    #include "s5k3p3_pdaf_flip_mirror.h"
#endif
  }
};

#endif /* __s5k3p3_LIB_H__ */
