/* gc8034_lib.c
 *
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
//#include <utils/Log.h>
#include "gc2053_lib.h"
//#include "../../../../../../../../mm-camerasdk/sensor/includes/debug_lib.h"
#include <debug_lib.h>

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
  if (gain > 128)
    gain = 128;

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

  if (reg_gain < 64)
    reg_gain = 64;	
  else if (reg_gain > 64*128)
    reg_gain = 64*128;

  gain = (float)(reg_gain / 64.0f);
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
  if (!exp_info)
    return -1;

  ALOGE("GC2053 reg_gain: %d, real_gain: %f, line_cnt: %d\n",
    sensor_real_to_register_gain(real_gain),
    sensor_register_to_real_gain(exp_info->reg_gain),
    line_count);
  exp_info->reg_gain = sensor_real_to_register_gain(real_gain);
    exp_info->sensor_real_gain =sensor_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain = real_gain / exp_info->sensor_real_gain;
    exp_info->line_count = line_count;
ALOGE("GC2053 Gloria,reg_gain=%d, sensor_real_gain=%f, digital_gain=%f\n",
		 exp_info->reg_gain,exp_info->sensor_real_gain,exp_info->digital_gain);
	
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
	int16_t gain_index = 0;
    uint16_t reg_count = 0;
    uint16_t gain_b1 = 1, gain_b2 = 0;
	uint32_t temp_gain = 0;
    ALOGE("GC2053,fl_lines=%d,gain=%d, line=%d\n",fl_lines,gain,line);
    if (1 == line && gain <= 64)
        return rc;

    if (!reg_setting) {
      return -1;
    }

/***************************Shutter Start*********************************/
	line = line < 4 ? 4 : line; /*anti color deviation on shot expoure*/
	
	
	reg_setting->reg_setting[reg_count].reg_addr = 0xfe;
	reg_setting->reg_setting[reg_count].reg_data = 0x00;
	reg_count++;

	reg_setting->reg_setting[reg_count].reg_addr =0x03;
	reg_setting->reg_setting[reg_count].reg_data = line >> 8;
	reg_count++;

	reg_setting->reg_setting[reg_count].reg_addr =0x04;
	reg_setting->reg_setting[reg_count].reg_data = line & 0xFF;
	reg_count++;

/*****************************Shutter End********************************/

	
/***************************GIAN START*********************************/	
			for(gain_index = 0; gain_index < MAX_AG_INDEX; gain_index++)
			{
				if((gain_level[gain_index] <= gain)&&(gain < gain_level[gain_index+1]))
					break;
			}

			temp_gain = gain*64/gain_level[gain_index];	
			gain_b1 = temp_gain >> 6;
			gain_b2 = ((temp_gain & 0x3f)<<2);
			reg_setting->reg_setting[reg_count].reg_addr = 0xb4;
			reg_setting->reg_setting[reg_count].reg_data = agc_register[gain_index][0];
			reg_count++;
			reg_setting->reg_setting[reg_count].reg_addr = 0xb3;
			reg_setting->reg_setting[reg_count].reg_data = agc_register[gain_index][1];
			reg_count++;
			reg_setting->reg_setting[reg_count].reg_addr = 0xb8;
			reg_setting->reg_setting[reg_count].reg_data = agc_register[gain_index][2];
			reg_count++;
			reg_setting->reg_setting[reg_count].reg_addr = 0xb9;
   		    reg_setting->reg_setting[reg_count].reg_data = agc_register[gain_index][3];
			reg_count++;
			reg_setting->reg_setting[reg_count].reg_addr =0xb1;
			reg_setting->reg_setting[reg_count].reg_data = gain_b1;
			reg_count++;

			reg_setting->reg_setting[reg_count].reg_addr =0xb2;
			reg_setting->reg_setting[reg_count].reg_data = gain_b2;
		   reg_count++;
						
			/*******************************GIAN END*********************************/
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
