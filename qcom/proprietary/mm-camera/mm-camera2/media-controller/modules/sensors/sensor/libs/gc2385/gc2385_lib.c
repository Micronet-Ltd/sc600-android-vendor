/* gc2385_lib.c
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include "gc2385_lib.h"
#include "debug_lib.h"
//#include <string.h>
//#include "sensor_lib.h"
#include <utils/Log.h>

/**
 * FUNCTION: sensor_real_to_register_gain
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/
static unsigned int sensor_real_to_register_gain(float gain)
{
  uint16_t reg_gain;

  if (gain < 1.0)
    gain = 1.0;
  if (gain > MAX_ANALOG_GAIN)
    gain = MAX_ANALOG_GAIN;

  reg_gain = (uint16_t)(gain * 64.0f);

  return reg_gain;
}

/**
 * FUNCTION: sensor_register_to_real_gain
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/
static float sensor_register_to_real_gain(unsigned int reg_gain)
{
  float gain;
  if (reg_gain > 0x0200)
    reg_gain = 0x0200;
  gain = (float)(reg_gain/64.0f);

  return gain;
}

/**
 * FUNCTION: sensor_calculate_exposure
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/
static int sensor_calculate_exposure(float real_gain,
  unsigned int line_count, sensor_exposure_info_t *exp_info,
  __attribute__((unused)) float s_real_gain)
{

  if (!exp_info) {
    return -1;
  }

  exp_info->reg_gain = sensor_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain = sensor_register_to_real_gain(exp_info->reg_gain);
  exp_info->sensor_digital_gain = 256;
  exp_info->sensor_real_dig_gain = 1.0;
  exp_info->digital_gain = real_gain / exp_info->sensor_real_gain;
  exp_info->line_count = line_count;

  return 0;
}

/**
 * FUNCTION: sensor_fill_exposure_array
 *
 * DESCRIPTION: Fill the sensor exposure array
 **/
static int sensor_fill_exposure_array(unsigned int gain,
  __attribute__((unused))unsigned int digital_gain, unsigned int line,
  __attribute__((unused)) unsigned int fl_lines, __attribute__((unused)) int luma_avg,
  __attribute__((unused)) unsigned int fgain,
  struct camera_i2c_reg_setting* reg_setting,
  __attribute__((unused)) unsigned int s_gain,
  __attribute__((unused)) int s_linecount,
  __attribute__((unused)) int is_hdr_enabled)
{
  int32_t rc = 0;

  uint16_t reg_count = 0;
  uint16_t gain_b6,gain_b1,gain_b2;
  uint16_t iReg,temp,line_temp;
  int32_t i;

  ALOGE("GC2385,fl_lines=%d,gain=%d, line=%d\n",fl_lines,gain,line);

  if (1 == line && gain <= 64)
    return rc;

  if (!reg_setting) {
    return -1;
  }

  reg_setting->reg_setting[reg_count].reg_addr =0xfe;
  reg_setting->reg_setting[reg_count].reg_data =0x00;
  reg_count++;

  iReg = gain;
  if(iReg < 0x40)
    iReg = 0x40;
  if((ANALOG_GAIN_1<= iReg)&&(iReg < ANALOG_GAIN_2))  //ANALOG_GAIN_1 == 64
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x73;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa2;
    reg_count++;

    //analog gain
    gain_b6 = 0x00;
    temp = 256*iReg/ANALOG_GAIN_1;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_2<= iReg)&&(iReg < ANALOG_GAIN_3))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x73;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa2;
    reg_count++;

    //analog gain
    gain_b6 = 0x01;
    temp = 256*iReg/ANALOG_GAIN_2;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_3<= iReg)&&(iReg < ANALOG_GAIN_4))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x73;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa2;
    reg_count++;

    //analog gain
    gain_b6 = 0x02;
    temp = 256*iReg/ANALOG_GAIN_3;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_4<= iReg)&&(iReg < ANALOG_GAIN_5))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x73;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa2;
    reg_count++;

    //analog gain
    gain_b6 = 0x03;
    temp = 256*iReg/ANALOG_GAIN_4;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_5<= iReg)&&(iReg < ANALOG_GAIN_6))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x73;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa3;
    reg_count++;

    //analog gain
    gain_b6 = 0x04;
    temp = 256*iReg/ANALOG_GAIN_5;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_6<= iReg)&&(iReg<ANALOG_GAIN_7))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x74;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa3;
    reg_count++;

    //analog gain
    gain_b6 = 0x05;
    temp = 256*iReg/ANALOG_GAIN_6;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_7<= iReg)&&(iReg<ANALOG_GAIN_8))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x74;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa3;
    reg_count++;

    //analog gain
    gain_b6 = 0x06;
    temp = 256*iReg/ANALOG_GAIN_7;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else if((ANALOG_GAIN_8<= iReg)&&(iReg<ANALOG_GAIN_9))
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x74;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa3;
    reg_count++;

    //analog gain
    gain_b6 = 0x07;
    temp = 256*iReg/ANALOG_GAIN_8;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }
  else
  {
    reg_setting->reg_setting[reg_count].reg_addr =0x20;
    reg_setting->reg_setting[reg_count].reg_data =0x75;
    reg_count++;
    reg_setting->reg_setting[reg_count].reg_addr =0x22;
    reg_setting->reg_setting[reg_count].reg_data =0xa4;
    reg_count++;

    //analog gain
    gain_b6 = 0x08;
    temp = 256*iReg/ANALOG_GAIN_9;
    gain_b1 = temp>>8;
    gain_b2 = temp&0xff;
  }

/***************************Shutter Start*********************************/
  if(line < 6) line = 6; /*anti color deviation on shot expoure*/

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0x3F00) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = line & 0xFF;
  reg_count++;

/*****************************Shutter End********************************/

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr + 4;
  reg_setting->reg_setting[reg_count].reg_data = gain_b6;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr - 1;
  reg_setting->reg_setting[reg_count].reg_data = gain_b1;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = gain_b2;
  reg_count++;

  reg_setting->size = reg_count;
  reg_setting->addr_type = CAMERA_I2C_BYTE_ADDR;
  reg_setting->data_type = CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;

  return rc;
}

/**
 * FUNCTION: sensor_open_lib
 *
 * DESCRIPTION: Open sensor library and returns data pointer
 **/
void *sensor_open_lib(void)
{
    return &sensor_lib_ptr;
}
