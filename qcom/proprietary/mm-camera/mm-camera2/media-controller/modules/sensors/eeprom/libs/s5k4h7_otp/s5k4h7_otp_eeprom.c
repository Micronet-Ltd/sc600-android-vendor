/*============================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#include "s5k4h7_otp_eeprom.h"

struct camera_i2c_reg_setting g_reg_setting;
struct camera_i2c_reg_array g_reg_array[17] = {
    {0x307a,0x0a,0x00},
    {0x3079,0x0a,0x00},
    {0x306e,0x71,0x00},
    {0x306f,0x28,0x00},
    {0x301f,0x20,0x00},
    {0x306b,0x9a,0x00},
    {0x3091,0x1f,0x00},
    {0x3906,0x04,0x00},
    {0x30c4,0x06,0x00},
    {0x3200,0x09,0x00},
    {0x306a,0x79,0x00},
    {0x30b0,0xff,0x00},
    {0x306d,0x08,0x00},
    {0x3080,0x01,0x00},
    {0x3b45,0x01,0x00},
    {0x3400,0x00,0x00},
    {0x0b00,0x01,0x00},//enable shading correction
    //{0x0100,0x01,0x00},
 };

/**
  CheckSum_OTP
 *    @buff: address of page buffer
 *
 **/
static int CheckSum_OTP(unsigned char *buf, uint16_t start,uint16_t end, uint16_t checksum)
{
  unsigned int i = 0, sum = 0;
  unsigned char ret;
  unsigned int first;
  unsigned int last;
  unsigned int check_sum;

  RETURN_ERROR_ON_NULL(buf);
  SLOW(" Enter");

  first = start;
  last  = end;
  check_sum = checksum;

  for(i = first; i <= last; i++){
    sum += buf[i];
  }
  ret = sum%MOD_8BIT;

  if(ret != buf[check_sum]){
    SERR("Checksum fail,expect 0x%x,actual 0x%x",ret,buf[check_sum]);
    return SENSOR_FAILURE;
  }

  SDBG(" Exit");
  return SENSOR_SUCCESS;
}


static int s5k4h7_eeprom_format_module_info(sensor_eeprom_data_t * e_ctrl)
{
  module_info_t *module_info = NULL;
  int group,group_addr,rc = 0;

  RETURN_ERROR_ON_NULL(e_ctrl);
  RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

  module_info = (module_info_t *)(e_ctrl->eeprom_params.buffer +
                                  MODULE_INFO_START);

  SERR("module_id = 0x%x, year/month/day: %d/%d/%d",
      module_info->module_id,
      module_info->year,
      module_info->month,
      module_info->day);

  SDBG(" Exit");
  return SENSOR_SUCCESS;
}


/** s5k4h7_eeprom_format_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k4h7_eeprom_format_wbdata(sensor_eeprom_data_t *e_ctrl)
{
   awb_data_t  *buff = NULL;
   int index;
   int i,rc = 0;
   uint_t group;
   float r_over_gr = 0.0, b_over_gb = 0.0, gr_over_gb = 0.0;
   float golden_r_over_gr = 0.0, golden_b_over_gb = 0.0,golden_gr_over_gb = 0.0;
   int flag;

   RETURN_ERROR_ON_NULL(e_ctrl);
   RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);
   SDBG("Enter");

   flag = e_ctrl->eeprom_params.buffer[FLAG_AWB_ADDR];
   switch (flag) {
        case 0x04:
             group = 0;
             break;
        case 0x0d:
             group = 1;
            break;
        default:
            SERR("Invalid flag 0x%x\n",flag);
            group = 0;
            break;
    }
    SERR("AWB: group = %d\n",group);
    buff = (awb_data_t  *)(e_ctrl->eeprom_params.buffer +
                                 AWB_GROUP0_START + (AWB_DATA_SIZE * group));

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,(FLAG_MODULE_INFO_ADDR + (AWB_DATA_SIZE * group)),
                                              (AWB_GROUP0_END + (AWB_DATA_SIZE * group)),
                                              (AWB_GROUP0_CHECKSUM + (AWB_DATA_SIZE * group)));

    if (rc != SENSOR_SUCCESS ) {
        SERR("AWB: checksum failed\n");
        return rc;
    }
    SERR("AWB: checksum succeed\n");
    SLOW("r_over_gr_h = 0x%x, r_over_gr_l = 0x%x",buff->r_over_gr_h,buff->r_over_gr_l);
    SLOW("b_over_gb_h = 0x%x, b_over_gb_l = 0x%x",buff->b_over_gb_h,buff->b_over_gb_l);
    SLOW("gb_over_gr_h= 0x%x, gb_over_gr_l = 0x%x",buff->gb_over_gr_h,buff->gb_over_gr_l);

    if (QVALUE == 0 ){
        SERR("fatal error: QVALUE can not equal 0");
        return SENSOR_FAILURE;
    }

    r_over_gr = ((uint16_t)buff->r_over_gr_h << 8 | buff->r_over_gr_l)/(float)QVALUE;
    b_over_gb = ((uint16_t)buff->b_over_gb_h << 8 | buff->b_over_gb_l)/(float)QVALUE;

    if (!(buff->gb_over_gr_h || buff->gb_over_gr_l)){
         SERR("invaild gr/gb = %d",(uint16_t)buff->gb_over_gr_h << 8 | buff->gb_over_gr_l);
    } else {
         gr_over_gb = (float)QVALUE/((uint16_t)buff->gb_over_gr_h << 8 | buff->gb_over_gr_l);
    }

    SERR("AWB Info:r/gr = %f,b/gb  = %f,gr/gb=%f", r_over_gr, b_over_gb,gr_over_gb);

    for ( index = 0; index < AGW_AWB_MAX_LIGHT; index++ ) {
         e_ctrl->eeprom_data.wbc.r_over_g[index] = r_over_gr;
         e_ctrl->eeprom_data.wbc.b_over_g[index] = b_over_gb;
    }
    e_ctrl->eeprom_data.wbc.gr_over_gb = gr_over_gb;

   awb_present = TRUE;

   golden_r_over_gr = ((uint16_t)buff->golden_r_over_gr_h  << 8 |
                                 buff->golden_r_over_gr_l)/ (float)QVALUE;

   golden_b_over_gb = ((uint16_t)buff->golden_b_over_gb_h  << 8 |
                                 buff->golden_b_over_gb_l)/ (float)QVALUE;

   if (!(buff->golden_gb_over_gr_h || buff->golden_gb_over_gr_l)){
       SERR("invaild golden_gr_over_gb = %d",(uint16_t)buff->golden_gb_over_gr_h << 8 |
                                                       buff->golden_gb_over_gr_l);
   } else {
       golden_gr_over_gb = (float)QVALUE / (uint16_t)(buff->golden_gb_over_gr_h << 8 |
                                                      buff->golden_gb_over_gr_l);
   }

   SERR("AWB golden Info :r/gr = %f,b/gb = %f,gr/gb=%f",\
        golden_r_over_gr, golden_b_over_gb,golden_gr_over_gb);

   SDBG("Exit");
   return SENSOR_SUCCESS;
}

/** s5k3p9_eeprom_format_afdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k4h7_eeprom_format_afdata(sensor_eeprom_data_t *e_ctrl)
{
    unsigned char     flag;
    af_data_t         *af;
    st_reg_addr       *af_addr;
    unsigned short    hori_macro_dac;
    unsigned short    hori_infinity_dac;
    unsigned short    vert_macro_dac;
    unsigned short    vert_infinity_dac;
    unsigned short    start_current;
    uint_t group;
    int rc;

    SDBG("Enter");
    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    e_ctrl->eeprom_data.afc.macro_dac = 600;
   // e_ctrl->eeprom_data.afc.starting_dac = ;
    e_ctrl->eeprom_data.afc.infinity_dac = 300;

   flag = e_ctrl->eeprom_params.buffer[AF_FLAG_ADDR];
   switch (flag) {
        case 0x04:
             group = 0;
             break;
        case 0x0d:
             group = 1;
            break;
        default:
            SERR("Invalid flag 0x%x,af addr start = %d\n",flag,AF_FLAG_ADDR);
            group = 0;
            break;
    }

    SERR("AF: group = %d\n",group);

    af = (af_data_t  *)(e_ctrl->eeprom_params.buffer +
                                 AF_FLAG_ADDR + 1 + AF_DATA_SIZE*group);


    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,(AF_GROUP0_START + AF_DATA_SIZE*group),
                                                   (AF_GROUP0_END + AF_DATA_SIZE*group),
                                                   (AF_GROUP0_CHECKSUM + AF_DATA_SIZE*group));
    if(rc < 0){
        SERR("AF Info: checksum failed\n");
        goto af_fail;
    }

    SERR("AF Info: checksum success");

    /* Use horizontal values */
    SINFO("AF_info: af macro_h = 0x%x,macro_l = 0x%x,infinity_h = 0x%x,infinity_l = 0x%x\n",
          af->macro_h,af->macro_l,af->infinity_h,af->infinity_l);

    hori_macro_dac = ((uint16_t)af->macro_h << 8 | af->macro_l);
    hori_infinity_dac = ((uint16_t)af->infinity_h << 8 | af->infinity_l);
    //start_current = ((uint16_t)af->startCurrent_h <<8 | (af->startCurrent_l));

    if (hori_macro_dac == 0 || hori_infinity_dac == 0) {
        SERR("Invalid macro or infi data， set to default\n");
        goto af_fail;
    } else {
           e_ctrl->eeprom_data.afc.macro_dac = hori_macro_dac;
           e_ctrl->eeprom_data.afc.infinity_dac = hori_infinity_dac;
    }

    e_ctrl->eeprom_data.afc.infinity_margin = INF_MARGIN;
    e_ctrl->eeprom_data.afc.macro_margin = MACRO_MARGIN;

    //e_ctrl->eeprom_data.afc.starting_dac = start_current;
    SERR("AF Info: Macro DAC: %d, Infinity DAC: %d\n",
        e_ctrl->eeprom_data.afc.macro_dac, e_ctrl->eeprom_data.afc.infinity_dac);



    af_present = TRUE;
    SDBG("Exit");
    return SENSOR_SUCCESS;

af_fail:
        e_ctrl->eeprom_data.afc.macro_dac = 600;
        e_ctrl->eeprom_data.afc.infinity_dac = 300;
        e_ctrl->eeprom_data.afc.starting_dac = 350;
        return SENSOR_FAILURE;
}

/** s5k4h7_hlt_eeprom_format_lensshading:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of lens shading correction calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k4h7_eeprom_format_lensshading(sensor_eeprom_data_t *e_ctrl)
{
    uint8_t *r_lsc, *gr_lsc, *gb_lsc, *b_lsc;
    float    r_gain, gr_gain, gb_gain, b_gain;
    uint8_t *lsc_buf;
    int rc;
    uint8_t group;
    int flag;

    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    lsccalib_data_t* lsccal = &e_ctrl->eeprom_data.lsc;

    flag = e_ctrl->eeprom_params.buffer[LSC_FLAG_ADDR];
    switch (flag) {
        case 0x04:
             group = 0;
             break;
        case 0x0d:
             group = 1;
            break;
        default:
            SERR("Invalid flag 0x%x\n",flag);
            group = 0;
            break;
    }
    SERR("LSC: group = %d\n",group);
    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,(LSC_GROUP0_ADDR_START + (LSC_DATA_SIZE * group)),
                                              (LSC_GROUP0_ADDR_END + (LSC_DATA_SIZE * group)),
                                              (LSC_CHECKSUM_GROUP0 + (1 * group)));
    SERR("LSC: checksum succeed\n");
    if (rc != SENSOR_SUCCESS ) {
        SERR("LSC: checksum failed\n");
        return rc;
    }

    lsc_buf =(uint8_t *)(e_ctrl->eeprom_params.buffer +
                        LSC_GROUP0_ADDR_START + (LSC_DATA_SIZE * group));

    /*use sensor lsc calibration function， not need to enable platform calibration*/
    lsc_present = FALSE;
    SLOW("Exit");
    return SENSOR_SUCCESS;
}

/** s5k4h7_eeprom_get_calibration_items:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get calibration capabilities and mode items.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void s5k4h7_eeprom_get_calibration_items(void *e_ctrl)
{
  RETURN_VOID_ON_NULL(e_ctrl);
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);

  e_items->is_wbc       = awb_present ? TRUE: FALSE;
  e_items->is_afc       = af_present ? TRUE: FALSE;;
  e_items->is_lsc       = lsc_present ? TRUE: FALSE;
  e_items->is_dpc       = FALSE;
  e_items->is_insensor  = TRUE;
  e_items->is_ois       = FALSE;

  SERR("is_wbc:%d,is_afc:%d,is_lsc:%d,is_dpc:%d,is_insensor:%d,is_ois:%d",
       e_items->is_wbc,e_items->is_afc, e_items->is_lsc,e_items->is_dpc,
        e_items->is_insensor, e_items->is_ois);
}



static int s5k4h7_get_raw_data(void *e_ctrl, void *data)
{
  RETURN_ERR_ON_NULL(e_ctrl, SENSOR_FAILURE);
  RETURN_ERR_ON_NULL(data, SENSOR_FAILURE);

  g_reg_setting.addr_type = CAMERA_I2C_WORD_ADDR;
  g_reg_setting.data_type = CAMERA_I2C_BYTE_DATA;
  g_reg_setting.reg_setting = &g_reg_array[0];
  g_reg_setting.size = (sizeof(g_reg_array)/sizeof(struct camera_i2c_reg_array));
  g_reg_setting.delay = 0;

  memcpy(data, &g_reg_setting, sizeof(g_reg_setting));
  SERR("size = %d\n",g_reg_setting.size);

  return 0;
}



/** s5k4h7_eeprom_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
static void s5k4h7_eeprom_format_calibration_data(void *e_ctrl) {

  sensor_eeprom_data_t * ctrl = (sensor_eeprom_data_t *)e_ctrl;
  unsigned short  rc = SENSOR_FAILURE;

  RETURN_VOID_ON_NULL(e_ctrl);

  rc = s5k4h7_eeprom_format_module_info(ctrl);
  if( rc != SENSOR_SUCCESS){
    otp_check_result |= OTP_CHECK_MODULE_FAIL;
    SERR("Module Info: format fail");
    return;
  }

  rc = s5k4h7_eeprom_format_wbdata(ctrl);
  if( rc != SENSOR_SUCCESS){
    otp_check_result |= OTP_CHECK_AWB_FAIL;
    SERR("WB Info: format fail");
    return;
  }

  rc = s5k4h7_eeprom_format_afdata(ctrl);
  if( rc != SENSOR_SUCCESS){
    otp_check_result |= OTP_CHECK_AF_FAIL;
    SERR("AF Info: format failed");
    return;
  }

}

/** s5k4h7_eeprom_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* s5k4h7_otp_eeprom_open_lib(void) {
  SLOW("Enter");
  return &s5k4h7_eeprom_lib_func_ptr;
}
