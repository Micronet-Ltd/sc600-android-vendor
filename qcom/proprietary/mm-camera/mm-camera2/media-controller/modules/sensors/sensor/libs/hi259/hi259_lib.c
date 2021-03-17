/* ov5670_lib.c
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <stdio.h>
#include "hi259_lib.h"
//#include <utils/Log.h>
#include <string.h>
#include "debug_lib.h"

/**
 * FUNCTION: sensor_real_to_register_gain
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/


static uint32_t hi259_real_to_register_gain(float real_gain)
{
    unsigned int reg_gain = 0;

    if (real_gain < HI259_MIN_AGAIN)
    {
	real_gain = HI259_MIN_AGAIN;
	SLOW("HI259 Requested gain(%f) is lower than the sensor minimum(%f)",
	real_gain, HI259_MIN_AGAIN);
    }
	if (real_gain > HI259_MAX_AGAIN)
	real_gain=HI259_MAX_AGAIN;
	reg_gain = (uint16_t)512/real_gain-34;

    return reg_gain;
}

/**
 * FUNCTION: sensor_register_to_real_gain
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/

static float hi259_register_to_real_gain(uint32_t reg_gain)
{
    float real_gain;
   if (reg_gain > 0x1de)
        reg_gain = 0x1de;

    real_gain = (float)512.0/(reg_gain + 34);

    return real_gain;
}

/**
 * FUNCTION: sensor_digital_gain_calc
 *
 * DESCRIPTION: Calcuate the sensor digital gain
 **/

static uint32_t hi259_digital_gain_calc(float real_gain, float sensor_real_gain)
{
    uint32_t reg_dig_gain = HI259_MIN_DGAIN_REG_VAL;
    float real_dig_gain = HI259_MIN_DGAIN;

    if(real_gain > HI259_MAX_AGAIN)
    {
        real_dig_gain = real_gain / sensor_real_gain;
    }
    else
    {
        real_dig_gain = HI259_MIN_DGAIN;
    }

    if(real_dig_gain > HI259_MAX_DGAIN)
    {
        real_dig_gain = HI259_MAX_DGAIN;
    }

    reg_dig_gain = (uint32_t)(real_dig_gain * 256);

    return reg_dig_gain;
}


/**
 * FUNCTION: sensor_calculate_exposure
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/

int32_t sensor_calculate_exposure(float real_gain,
  uint32_t line_count, sensor_exposure_info_t *exp_info,
  __attribute__((unused)) float s_real_gain)
{
    if (!exp_info)
    {
      return -1;
    }

    exp_info->reg_gain = hi259_real_to_register_gain(real_gain);
    exp_info->sensor_real_gain = hi259_register_to_real_gain(exp_info->reg_gain);

	    exp_info->sensor_digital_gain =
	    hi259_digital_gain_calc(real_gain, exp_info->sensor_real_gain);
	    exp_info->sensor_real_dig_gain = exp_info->sensor_digital_gain / 256;
	    exp_info->digital_gain = real_gain/
	      (exp_info->sensor_real_gain * exp_info->sensor_real_dig_gain);

    exp_info->line_count = line_count;

    return 0;
}

/**
 * FUNCTION: sensor_fill_exposure_array
 *
 * DESCRIPTION: Fill the sensor exposure array
 **/
int32_t sensor_fill_exposure_array(unsigned int gain,
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
    uint16_t i = 0;

   // uint16_t max_legal_gain = 0x00AC;
    uint16_t min_legal_line = 0x04;

    // dummy
    rc = digital_gain;
    rc = 0;

    if (!reg_setting){
      return -1;
    }

   // if(gain > max_legal_gain) gain = max_legal_gain;
    if(line < min_legal_line) line = min_legal_line;

    for (i = 0; i < sensor_lib_ptr.groupon_settings.size; i++) {
        reg_setting->reg_setting[reg_count].reg_addr =
          sensor_lib_ptr.groupon_settings.reg_setting_a[i].reg_addr;
        reg_setting->reg_setting[reg_count].reg_data =
          sensor_lib_ptr.groupon_settings.reg_setting_a[i].reg_data;
        reg_count = reg_count + 1;
    }

  reg_setting->reg_setting[reg_count].reg_addr = 0x03;
  reg_setting->reg_setting[reg_count].reg_data = 0x00;
  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines + 1;
  reg_setting->reg_setting[reg_count].reg_data = fl_lines & 0xFF;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr = 0x03;
  reg_setting->reg_setting[reg_count].reg_data = 0x20;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF00) >> 8;
  reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = line & 0xFF;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain >>1)&0xff;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = gain & 0x01;
  reg_count = reg_count + 1;

  for (i = 0; i < sensor_lib_ptr.groupoff_settings.size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupoff_settings.reg_setting_a[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupoff_settings.reg_setting_a[i].reg_data;
    reg_count = reg_count + 1;
  }

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
