/*============================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#include "dw9763_2d_eeprom.h"

/** dw9763_2d_eeprom_get_calibration_items:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get calibration capabilities and mode items.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void dw9763_2d_eeprom_get_calibration_items(void *e_ctrl)
{
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);

  e_items->is_wbc = awb_present ? TRUE : FALSE;
 // e_items->is_wbc = FALSE;
  e_items->is_afc = af_present ? TRUE : FALSE;
  //e_items->is_lsc = lsc_present ? TRUE : FALSE;
  e_items->is_lsc =  lsc_present ? TRUE : FALSE;
  e_items->is_dpc = FALSE;
  e_items->is_insensor = FALSE;
  e_items->is_ois = FALSE;

  SLOW("is_wbc:%d,is_afc:%d,is_lsc:%d,is_dpc:%d,is_insensor:%d,\
  is_ois:%d",e_items->is_wbc,e_items->is_afc,
    e_items->is_lsc,e_items->is_dpc,e_items->is_insensor,
    e_items->is_ois);

}

static void dw9763_2d_eeprom_print_matrix(__attribute__((unused))float* paramlist)
{
  int j =0;
  for (j=0; j < LSC_GRID_SIZE; j = j + 17) {
   SLOW("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f,%.2f, %.2f, %.2f,"
     "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f",
     paramlist[j], paramlist[j+1], paramlist[j+2], paramlist[j+3],
     paramlist[j+4], paramlist[j+5], paramlist[j+6], paramlist[j+7],
     paramlist[j+8], paramlist[j+9], paramlist[j+10], paramlist[j+11],
     paramlist[j+12], paramlist[j+13], paramlist[j+14], paramlist[j+15],
     paramlist[j+16]);
  }
}

/** dw9763_2d_eeprom_format_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void dw9763_2d_eeprom_format_wbdata(sensor_eeprom_data_t *e_ctrl)
{
  unsigned char flag;
  module_info_t *module_info;
  awb_data_t    *wb;
  float         r_over_gr, b_over_gb, gr_over_gb;
  int           i;

  SDBG("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[AWB_FLAG_OFFSET];
  SLOW("e_ctrl->eeprom_params.buffer[0] %d,\n ",e_ctrl->eeprom_params.buffer[0]);
  if (flag != VALID_FLAG) {
    awb_present = FALSE;
    SERR("AWB : empty or invalid data");
    return;
  }
  awb_present = TRUE;
  #if 0
  /* Print module info */
  module_info = (module_info_t *)
    (e_ctrl->eeprom_params.buffer + MODULE_INFO_OFFSET);
  SLOW("Module ID : 0x%x", module_info->id);
  SLOW("Y/M/D : %d/%d/%d",
    module_info->year, module_info->month, module_info->day);
  #endif

  /* Get AWB data */
  wb = (awb_data_t *)(e_ctrl->eeprom_params.buffer + AWB_OFFSET);

  r_over_gr = ((float)((wb->r_over_gr_h << 8) | wb->r_over_gr_l)) / QVALUE;
  b_over_gb = ((float)((wb->b_over_gb_h << 8) | wb->b_over_gb_l)) / QVALUE;
  gr_over_gb = ((float)((wb->gr_over_gb_h << 8) | wb->gr_over_gb_l)) / QVALUE;

  SLOW("AWB : r/gr = %f", r_over_gr);
  SLOW("AWB : b/gb = %f", b_over_gb);
  SLOW("AWB : gr/gb = %f", gr_over_gb);

  for (i = 0; i < AGW_AWB_MAX_LIGHT; i++) {
    e_ctrl->eeprom_data.wbc.r_over_g[i] = r_over_gr;
    e_ctrl->eeprom_data.wbc.b_over_g[i] = b_over_gb;
  }
  e_ctrl->eeprom_data.wbc.gr_over_gb = gr_over_gb;
  SDBG("Exit");
}

/** dw9763_2d_eeprom_format_lensshading:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of lens shading correction calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void dw9763_2d_eeprom_format_lensshading(sensor_eeprom_data_t *e_ctrl)
{
  unsigned char  flag;
  unsigned short i, light, grid_size;
  unsigned char  *lsc_r_h, *lsc_b_h, *lsc_gr_h, *lsc_gb_h,   *lsc_r_l, *lsc_b_l, *lsc_gr_l, *lsc_gb_l;
  float          gain;

  SDBG("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[LSC_FLAG_OFFSET];
  if (flag != VALID_FLAG) {
   lsc_present = FALSE;
   SERR("LSC : empty or invalid data flag is %d",e_ctrl->eeprom_params.buffer[LSC_FLAG_OFFSET]);
   return;
  }


  lsc_present = TRUE;
  lsc_r_h = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET]);
  lsc_r_l = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 1]);
  lsc_gr_h = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 2]);
  lsc_gr_l = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 3]);
  lsc_gb_h = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 4]);
  lsc_gb_l = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 5]);
  lsc_b_h = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 6]);
  lsc_b_l= (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_OFFSET + 7]);
  

/* missing information about order in eeprom data sheet */
  for (i = 0; i < LSC_GRID_SIZE; i++) {
    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].r_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_D65_LIGHT].r_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_A_LIGHT].r_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_H_LIGHT].r_gain[i] =
      (float) (*lsc_r_l | (*lsc_r_h) <<8);
    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].gr_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_D65_LIGHT].gr_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_A_LIGHT].gr_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_H_LIGHT].gr_gain[i] =
    (float) (*lsc_gr_l | (*lsc_gr_h)<<8);
    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].gb_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_D65_LIGHT].gb_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_A_LIGHT].gb_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_H_LIGHT].gb_gain[i] =
    (float) (*lsc_gb_l | (*lsc_gb_h)<<8);
    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].b_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_D65_LIGHT].b_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_A_LIGHT].b_gain[i] =
      e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_H_LIGHT].b_gain[i] =
    (float) (*lsc_b_l | (*lsc_b_h)<<8);
    lsc_r_h += 8;
    lsc_r_l += 8;
    lsc_gr_h += 8;
    lsc_gr_l += 8;
    lsc_gb_h += 8;
    lsc_gb_l += 8;
    lsc_b_h += 8;
    lsc_b_l += 8;
  }

    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].mesh_rolloff_table_size
    = LSC_GRID_SIZE;
  e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_A_LIGHT].mesh_rolloff_table_size
    = LSC_GRID_SIZE;
  e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_D65_LIGHT].mesh_rolloff_table_size
    = LSC_GRID_SIZE;
    e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_H_LIGHT].mesh_rolloff_table_size
    = LSC_GRID_SIZE;

  SLOW("OTP: LSC MESH R MATRIX");
  dw9763_2d_eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].r_gain);
  SLOW("OTP: LSC MESH GR MATRIX");
  dw9763_2d_eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].gr_gain);
  SLOW("OTP: LSC MESH GB MATRIX");
  dw9763_2d_eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].gb_gain);
  SLOW("OTP: LSC MESH B MATRIX");
  dw9763_2d_eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[ROLLOFF_TL84_LIGHT].b_gain);
  
  SDBG("Exit");
}

/** dw9763_2d_eeprom_format_afdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void dw9763_2d_eeprom_format_afdata(sensor_eeprom_data_t *e_ctrl)
{
  unsigned char    flag;
  af_data_t        *af;

  SDBG("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[AF_FLAG_OFFSET];
  if (flag != VALID_FLAG) {
   /* need to call autofocus caliberation to boost up code_per_step
      invalid AF EEPROM data will not be consumed by af parameters
   */
   SERR("AF : empty or invalid data");
  }
  af_present = TRUE;
  /* Get AF data */
  af = (af_data_t *)(e_ctrl->eeprom_params.buffer + AF_OFFSET);

  e_ctrl->eeprom_data.afc.macro_dac = ((af->macro_h << 8) | af->macro_l);
  e_ctrl->eeprom_data.afc.infinity_dac =
    ((af->infinity_h << 8) | af->infinity_l);
  e_ctrl->eeprom_data.afc.starting_dac = e_ctrl->eeprom_data.afc.infinity_dac;

  SLOW("AF : macro %d infinity %d (no starting DAC set to infinity)",
    e_ctrl->eeprom_data.afc.macro_dac, e_ctrl->eeprom_data.afc.infinity_dac);
  SDBG("Exit");
}

static void dw9763_2d_eeprom_format_pdaf2Dgain(sensor_eeprom_data_t *e_ctrl)
{
  pdaf_2D_cal_data_t          *pdafc_2d = &e_ctrl->eeprom_data.pdafc_2d;
  unsigned char               *buffer = e_ctrl->eeprom_params.buffer;
  unsigned char               flag;
  unsigned short               i = 0;
  int                          pd_offset = PDGAIN;
  int                          dcc_offset = DCC_OFFSET;

  if (buffer[PDGAIN_FLAG] == VALID_FLAG) {
    pdafc_2d->VersionNum = buffer[pd_offset] << 8 | buffer[pd_offset+1];
    pd_offset += 2;
	pdafc_2d->MapWidth = buffer[pd_offset] << 8 | buffer[pd_offset+1];
    pd_offset += 2;
    pdafc_2d->MapHeight = buffer[pd_offset] << 8 | buffer[pd_offset+1];
    pd_offset += 2;
   // pdafc_2d->OffsetX = buffer[pd_offset] << 8 | buffer[pd_offset+1];
   // pd_offset += 2;
   // pdafc_2d->OffsetY = buffer[pd_offset] << 8 | buffer[pd_offset+1];
   // pd_offset += 2;
   // pdafc_2d->RatioX = buffer[pd_offset] << 8 | buffer[pd_offset+1];
   // pd_offset += 2;
   // pdafc_2d->RatioY = buffer[pd_offset] << 8 | buffer[pd_offset+1];
   // pd_offset += 2;
    
    for (i = 0; i < 221; i++) {
      pdafc_2d->Right_GainMap[i] = buffer[pd_offset] << 8 | buffer[pd_offset+1];
      pd_offset += 2;
    }
    for (i = 0; i < 221; i++) {
      pdafc_2d->Left_GainMap[i] = buffer[pd_offset] << 8 | buffer[pd_offset+1];
      pd_offset += 2;
    }
    //pdafc_2d->PD_conversion_coeff[0] =  buffer[dcc_offset] << 8 | buffer[dcc_offset+1];
  }
  else
    SERR("failed, valid flag: %d. No 2D PDAF cal data for s5k3m2xm",
      buffer[PDGAIN_FLAG]);
      pd_offset += 2;//  skip checksum and dcc flag
      flag = e_ctrl->eeprom_params.buffer[pd_offset-1];//DCC FLAG

  if (flag == VALID_FLAG) {
	//dcc_offset += 2;
     pdafc_2d->Q_factor_DCC = buffer[pd_offset] << 8 | buffer[pd_offset+1];
	 pd_offset += 2;
     pdafc_2d->MapWidth_DCC = buffer[pd_offset] << 8 | buffer[pd_offset+1];
	 pd_offset += 2;
     pdafc_2d->MapHeight_DCC = buffer[pd_offset] << 8 | buffer[pd_offset+1];
     pd_offset += 2;
     SLOW("dcc Q format %d", pdafc_2d->Q_factor_DCC);
     SLOW("dcc map width %d", pdafc_2d->MapWidth_DCC);
     SLOW("dcc map height %d", pdafc_2d->MapHeight_DCC);
     //dcc_map = e_ctrl->eeprom_params.buffer + DCC_OFFSET;
     for (i = 0; i < 48; i++) {
// 	  i = 2 * i;
       pdafc_2d->PD_conversion_coeff[i] = buffer[pd_offset] << 8 | buffer[pd_offset+1];
       pd_offset += 2;
     }
  }else
        SERR("failed, valid flag: %d. No DCC_FLAG  data for s5k3m2xm", buffer[DCC_FLAG]);


  SLOW("-------------PDAF GAIN DUMP------------");
  SLOW("VersionNum: %d, OffsetX %d, OffsetY %d, RatioX %d, RatioY %d,"
       "MapWidth %d, MapHeight %d",
       pdafc_2d->VersionNum,
       pdafc_2d->OffsetX,
       pdafc_2d->OffsetY,
       pdafc_2d->RatioX,
       pdafc_2d->RatioY,
       pdafc_2d->MapWidth,
       pdafc_2d->MapHeight
  );
  SLOW("-----------Left_GainMap----------------");
  for (i = 0; i < 221; i = i + 17) {
   SLOW("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
     "%d, %d, %d, %d, %d",
     pdafc_2d->Left_GainMap[i],    pdafc_2d->Left_GainMap[i+1],
     pdafc_2d->Left_GainMap[i+2],  pdafc_2d->Left_GainMap[i+3],
     pdafc_2d->Left_GainMap[i+4],  pdafc_2d->Left_GainMap[i+5],
     pdafc_2d->Left_GainMap[i+6],  pdafc_2d->Left_GainMap[i+7],
     pdafc_2d->Left_GainMap[i+8],  pdafc_2d->Left_GainMap[i+9],
     pdafc_2d->Left_GainMap[i+10], pdafc_2d->Left_GainMap[i+11],
     pdafc_2d->Left_GainMap[i+12], pdafc_2d->Left_GainMap[i+13],
     pdafc_2d->Left_GainMap[i+14], pdafc_2d->Left_GainMap[i+15],
     pdafc_2d->Left_GainMap[i+16]
   );
  }
  SLOW("-----------Right_GainMap----------------");
  for (i = 0; i < 221; i = i + 17) {
   SLOW("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
     "%d, %d, %d, %d, %d",
     pdafc_2d->Right_GainMap[i],    pdafc_2d->Right_GainMap[i+1],
     pdafc_2d->Right_GainMap[i+2],  pdafc_2d->Right_GainMap[i+3],
     pdafc_2d->Right_GainMap[i+4],  pdafc_2d->Right_GainMap[i+5],
     pdafc_2d->Right_GainMap[i+6],  pdafc_2d->Right_GainMap[i+7],
     pdafc_2d->Right_GainMap[i+8],  pdafc_2d->Right_GainMap[i+9],
     pdafc_2d->Right_GainMap[i+10], pdafc_2d->Right_GainMap[i+11],
     pdafc_2d->Right_GainMap[i+12], pdafc_2d->Right_GainMap[i+13],
     pdafc_2d->Right_GainMap[i+14], pdafc_2d->Right_GainMap[i+15],
     pdafc_2d->Right_GainMap[i+16]
   );
  }
#if 0
   SERR("-----------DCC----------------");
   SERR(" MapWidth %d, MapHeight %d, Q factor %d",
   pdafc_2d->MapWidth_DCC,
   pdafc_2d->MapHeight_DCC,
   pdafc_2d->Q_factor_DCC);
  for (i = 0; i < 48; i = i + 8) {
        SERR("%d, %d, %d, %d, %d, %d, %d, %d",
          pdafc_2d->PD_conversion_coeff[i],    pdafc_2d->PD_conversion_coeff[i+1],
          pdafc_2d->PD_conversion_coeff[i+2],  pdafc_2d->PD_conversion_coeff[i+3],
          pdafc_2d->PD_conversion_coeff[i+4],  pdafc_2d->PD_conversion_coeff[i+5],
          pdafc_2d->PD_conversion_coeff[i+6],  pdafc_2d->PD_conversion_coeff[i+7]);
  };
#endif
}

/** dw9763_2d_eeprom_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
void dw9763_2d_eeprom_format_calibration_data(void *e_ctrl)
{
  sensor_eeprom_data_t * ctrl = (sensor_eeprom_data_t *)e_ctrl;

  SDBG("Enter");
  RETURN_VOID_ON_NULL(ctrl);

  SLOW("Total bytes in OTP buffer: %d", ctrl->eeprom_params.num_bytes);

  if (!ctrl->eeprom_params.buffer || !ctrl->eeprom_params.num_bytes) {
    SERR("failed: Buff pointer %p buffer size %d", ctrl->eeprom_params.buffer,
      ctrl->eeprom_params.num_bytes);
    return;
  }

  dw9763_2d_eeprom_format_wbdata(ctrl);
  dw9763_2d_eeprom_format_lensshading(ctrl);
  dw9763_2d_eeprom_format_afdata(ctrl);
  dw9763_2d_eeprom_format_pdaf2Dgain(ctrl);
}

static int dw9763_2d_autofocus_calibration(void *e_ctrl) {
  sensor_eeprom_data_t    *ectrl = (sensor_eeprom_data_t *) e_ctrl;
  int                     i = 0;
  actuator_tuned_params_t *af_driver_tune = NULL;
  actuator_params_t       *af_params = NULL;
  unsigned int            total_steps = 0;
  unsigned short          macro_dac, infinity_dac;
  unsigned short          new_step_bound, otp_step_bound;
  unsigned int            qvalue = 0;

  SDBG("Enter");
  RETURN_ON_NULL(e_ctrl);
  RETURN_ON_NULL(ectrl->eeprom_afchroma.af_driver_ptr);

  af_driver_tune =
    &(ectrl->eeprom_afchroma.af_driver_ptr->actuator_tuned_params);
  af_params = &(ectrl->eeprom_afchroma.af_driver_ptr->actuator_params);

  /* Get the total steps */
  total_steps = af_driver_tune->region_params[af_driver_tune->region_size - 1].
    step_bound[0] - af_driver_tune->region_params[0].step_bound[1];

  if (!total_steps) {
    SERR("Invalid total_steps count: %d",total_steps);
    return FALSE;
  }

  qvalue = af_driver_tune->region_params[0].qvalue;
  if(qvalue < 1 && qvalue > 4096){
    SERR("Invalid qvalue %d", qvalue);
    return FALSE;
  }
  if ((unsigned short)ectrl->eeprom_data.afc.macro_dac < INVALID_DATA)
  {
   macro_dac = ectrl->eeprom_data.afc.macro_dac;
   infinity_dac = ectrl->eeprom_data.afc.infinity_dac;
   otp_step_bound = macro_dac - infinity_dac;
   /* adjust af_driver_ptr */
   af_driver_tune->initial_code = infinity_dac - otp_step_bound * INFINITY_MARGIN;
   new_step_bound = otp_step_bound * (1 + INFINITY_MARGIN + MACRO_MARGIN);
   af_driver_tune->region_params[0].code_per_step =
    new_step_bound / (float)total_steps * qvalue;
  }
  else{
   /* if AF data is invalid, only boost code_per_step */
     af_driver_tune->region_params[0].code_per_step = qvalue;
  }

  SLOW("initial code %d, adjusted code_per_step: %d, qvalue: %d",
    af_driver_tune->initial_code,
    af_driver_tune->region_params[0].code_per_step,
    qvalue);

  SDBG("Exit");

  return TRUE;
}

/** dw9763_2d_eeprom_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* dw9763_2d_eeprom_open_lib(void) {
  return &dw9763_2d_eeprom_lib_func_ptr;
}
