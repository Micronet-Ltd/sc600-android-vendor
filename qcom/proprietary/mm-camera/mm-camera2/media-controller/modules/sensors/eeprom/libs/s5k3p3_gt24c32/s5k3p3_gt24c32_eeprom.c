/*============================================================================

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

#include "s5k3p3_gt24c32_eeprom.h"

static int CheckSum_OTP(unsigned char *buf, st_reg_addr *flag_addr,int offset)
 {
    unsigned int i = 0, sum = 0;
    unsigned char ret;
    unsigned int first,last,checksum;

    RETURN_ERROR_ON_NULL(buf);
    RETURN_ERROR_ON_NULL(flag_addr);
    SLOW("checksum Enter");

    first = flag_addr->start - offset;
    last  = flag_addr->end - offset;
    checksum = flag_addr->checksum - offset;

    for(i = first; i <= last; i++){
       sum += buf[i];
    }

    ret = sum%MOD_8BIT;
    if(ret != buf[checksum]){
        SERR("check fail! expect is 0x%x, actual is 0x%x",ret,buf[checksum]);
        return SENSOR_FAILURE;
    }

    SDBG("Exit");
    return SENSOR_SUCCESS;
}

/** s5k3p3_gt24c32_eeprom_format_module_info:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get sensor module info.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k3p3_gt24c32_eeprom_format_module_info(sensor_eeprom_data_t* e_ctrl)
{
    uint8_t flag;
    int rc;
    module_info_t* module_info;
    st_reg_addr *module_info_addr;

    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    module_info_addr = &eeprom_addr_table[MODULE_INFO];
    /* Check validity */
    flag = *(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(module_info_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("Module Info: flag invaild!");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,module_info_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("Module Info: checksum failed!");
        return SENSOR_FAILURE;
    }

    SERR("Module Info: checksum success!");
    module_info = (module_info_t *)(e_ctrl->eeprom_params.buffer +
                                    TRANSLATE_ADDR(module_info_addr->start));

    SLOW("module_id=%d,module_version = %d,software_version = %d,%d-%d-%d",
         module_info->module_id,
         module_info->module_version,
         module_info->software_version,
         module_info->year,
         module_info->month,
         module_info->day);


    return SENSOR_SUCCESS;
}

/** s5k3p3_gt24c32_eeprom_format_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k3p3_gt24c32_eeprom_format_wbdata(sensor_eeprom_data_t *e_ctrl)
{
    unsigned char flag;
    int index = 0;
    float r_over_gr, b_over_gr, gr_over_gb;
    awb_data_t* buff;
    st_reg_addr *awb_addr;
    int rc;

    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    awb_addr = &eeprom_addr_table[AWB_INFO];
    /* Check validity */
    flag = *(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(awb_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("AWB Info: flag invaild");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,awb_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("AWB Info: checksum failed");
        return SENSOR_FAILURE;
    }

    SERR("AWB Info: checksum success");
    awb_present = TRUE;
    buff = (awb_data_t*)(e_ctrl->eeprom_params.buffer + awb_addr->start);


    if (QVALUE == 0 ){
        SERR("fatal error: QVALUE can not equal 0");
        return SENSOR_FAILURE;
    }

    r_over_gr = ((uint16_t)buff->r_over_gr_h << 8 | buff->r_over_gr_l)/(float)QVALUE;
    b_over_gr = ((uint16_t)buff->b_over_gr_h << 8 | buff->b_over_gr_l)/(float)QVALUE;

    if (!(buff->gb_over_gr_h || buff->gb_over_gr_l)){
        SERR("AWB Info:invaild gr_over_gb = %d",(uint16_t)buff->gb_over_gr_h << 8 | buff->gb_over_gr_l);
        return SENSOR_FAILURE;
    }
    gr_over_gb = (float)QVALUE/((uint16_t)buff->gb_over_gr_h << 8 | buff->gb_over_gr_l);

    SERR("AWB Info: r/gr=%f,b/gr=%f,gr/gb=%f",r_over_gr,b_over_gr,gr_over_gb);

    for ( index = 0; index < AGW_AWB_MAX_LIGHT; index++ ) {
        e_ctrl->eeprom_data.wbc.r_over_g[index] = r_over_gr;
        e_ctrl->eeprom_data.wbc.b_over_g[index] = b_over_gr;
    }

	/**
	 BGGR/BGRG:  gr_over_gb
	 RGGB/GRBG:  1/gr_over_gb
	**/
    e_ctrl->eeprom_data.wbc.gr_over_gb = 1/gr_over_gb;

    return SENSOR_SUCCESS;
}

/** s5k3p3_gt24c32_eeprom_format_lensshading:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of lens shading correction calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k3p3_gt24c32_eeprom_format_lensshading(sensor_eeprom_data_t *e_ctrl)
{
    unsigned char  flag;
    int index = 0;
    uint16_t i, light;
    uint8_t *r_lsc, *gr_lsc, *gb_lsc, *b_lsc;
    float    r_gain, gr_gain, gb_gain, b_gain;
    int lsc_grid_size;
    unsigned char *lsc_buf;
    st_reg_addr *lsc_addr;
    int rc;

    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    lsccalib_data_t* lsccal = &e_ctrl->eeprom_data.lsc;

    lsc_addr = &eeprom_addr_table[LSC_INFO];
    /* Check validity */
    flag = *(e_ctrl->eeprom_params.buffer +  TRANSLATE_ADDR(lsc_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("LSC Info: flag invaild");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,lsc_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("LSC Info: checksum failed");
        return SENSOR_FAILURE;
    }

    SERR("LSC Info: checksum success");
    lsc_present = TRUE;

    lsc_buf =(uint8_t *)(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(lsc_addr->start));

    lsc_grid_size = LSC_GRID_SIZE > MESH_ROLLOFF_SIZE ? MESH_ROLLOFF_SIZE : LSC_GRID_SIZE;
    SLOW("LSC Info: lsc_grid_size = %d,ROLLOFF_MAX_LIGHT=%d",lsc_grid_size,ROLLOFF_MAX_LIGHT);

    for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
        lsccal->lsc_calib[light].mesh_rolloff_table_size =
        lsc_grid_size;
    }

#if 0
    for (i = 0; i < LSC_GRID_SIZE * 2 ; i += 2) {
        r_gain  = (uint16_t)lsc_buf[i] << 8 | lsc_buf[i + 1];
        gr_gain = (uint16_t)lsc_buf[LSC_GRID_SIZE*2 + i] << 8 | lsc_buf[LSC_GRID_SIZE*2 + i + 1];
        gb_gain = (uint16_t)lsc_buf[LSC_GRID_SIZE*4 + i] << 8 | lsc_buf[LSC_GRID_SIZE*4 + i + 1];
        b_gain  = (uint16_t)lsc_buf[LSC_GRID_SIZE*6 + i] << 8 | lsc_buf[LSC_GRID_SIZE*6 + i + 1];

        for(light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
            lsccal->lsc_calib[light].r_gain[i/2] = r_gain;
            lsccal->lsc_calib[light].gr_gain[i/2] = gr_gain;
            lsccal->lsc_calib[light].gb_gain[i/2] = gb_gain;
            lsccal->lsc_calib[light].b_gain[i/2] = b_gain;
        }
     }
#endif

    for (i = 0; i < LSC_GRID_SIZE * 8; i += 8) {
         r_gain  = (uint16_t)lsc_buf[i + 0] << 8 | lsc_buf[i + 1];
         gr_gain = (uint16_t)lsc_buf[i + 2] << 8 | lsc_buf[i + 3];
         gb_gain = (uint16_t)lsc_buf[i + 4] << 8 | lsc_buf[i + 5];
         b_gain  = (uint16_t)lsc_buf[i + 6] << 8 | lsc_buf[i + 7];

        for(light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
            lsccal->lsc_calib[light].r_gain[i/8] = r_gain;
            lsccal->lsc_calib[light].gr_gain[i/8] = gr_gain;
            lsccal->lsc_calib[light].gb_gain[i/8] = gb_gain;
            lsccal->lsc_calib[light].b_gain[i/8] = b_gain;
        }
    }

    SLOW("Exit");
    return SENSOR_SUCCESS;
}

/** s5k3p3_gt24c32_eeprom_format_afdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k3p3_gt24c32_eeprom_format_afdata(sensor_eeprom_data_t *e_ctrl)
{
    unsigned char     flag;
    af_data_t         *af;
    st_reg_addr       *af_addr;
    unsigned short    hori_macro_dac;
    unsigned short    hori_infinity_dac;
    unsigned short    vert_macro_dac;
    unsigned short    vert_infinity_dac;
    unsigned short    start_current;
    int rc;

    SDBG("Enter");
    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    e_ctrl->eeprom_data.afc.macro_dac = INVALID_DATA;
    e_ctrl->eeprom_data.afc.starting_dac = INVALID_DATA;
    e_ctrl->eeprom_data.afc.infinity_dac = INVALID_DATA;

    af_addr = &eeprom_addr_table[AF_INFO];
    /* Check validity */
    flag = *(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(af_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("AF Info: flag invaild");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer, af_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("AF Info: checksum failed");
        return SENSOR_FAILURE;
    }

    SERR("AF Info: checksum success");
    af_present = TRUE;
    /* Get AF data */
    af = (af_data_t *)(e_ctrl->eeprom_params.buffer + af_addr->start);


    /* Use horizontal values */
    SLOW("AF_info: af macro_h = 0x%x,macro_l = 0x%x,infinity_h = 0x%x,infinity_l = 0x%x",
          af->macro_h,af->macro_l,af->infinity_h,af->infinity_l);

    hori_macro_dac = ((uint16_t)af->macro_h << 8 | af->macro_l);
    hori_infinity_dac = ((uint16_t)af->infinity_h << 8 | af->infinity_l);
    start_current = ((uint16_t)af->startCurrent_h <<8 | (af->startCurrent_l));

    e_ctrl->eeprom_data.afc.macro_dac = hori_macro_dac;
    e_ctrl->eeprom_data.afc.infinity_dac = hori_infinity_dac;

    e_ctrl->eeprom_data.afc.infinity_margin = INF_MARGIN;
    e_ctrl->eeprom_data.afc.macro_margin = MACRO_MARGIN;

    e_ctrl->eeprom_data.afc.starting_dac = start_current;

    SERR("AF Info: Macro DAC: %d, Infinity DAC: %d",
        e_ctrl->eeprom_data.afc.macro_dac, e_ctrl->eeprom_data.afc.infinity_dac);

    SDBG("Exit");
    return SENSOR_SUCCESS;
}

/** s5k3p3_gt24c32_eeprom_format_pdafgain:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of pdaf gain
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static int s5k3p3_gt24c32_eeprom_format_pdafgain(sensor_eeprom_data_t *e_ctrl)
{
    unsigned char       *pdaf_ptr;
    int                 i, ver_num;
    uint8_t    flag;
    uint8_t  *pdaf_gmap_buffer,*pdaf_dcc_buffer;
    uint8_t  *pdaf_gmap_buffer_l,*pdaf_gmap_buffer_r;
    st_reg_addr *pdaf_gmap_addr,*pdaf_dcc_addr;
    int rc;
    int gain_offset;
    int EnablePdSwap= 0;
    char prop[PROPERTY_VALUE_MAX];

    SDBG("Enter");
    RETURN_ERROR_ON_NULL(e_ctrl);
    RETURN_ERROR_ON_NULL(e_ctrl->eeprom_params.buffer);

    pdaf_2D_cal_data_t  *pdafc_2d = &e_ctrl->eeprom_data.pdafc_2d;

    pdaf_gmap_addr = &eeprom_addr_table[PDAF_GMAP_INFO];
    flag = *(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(pdaf_gmap_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("PDAF GMAP Info: flag invaild");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,pdaf_gmap_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("PDAF GMAP Info: checksum failed");
        return SENSOR_FAILURE;
    }

    SERR("PDAF GMAP Info: checksum success");

    gain_offset = 0;
    pdaf_gmap_buffer =(uint8_t *)(e_ctrl->eeprom_params.buffer + pdaf_gmap_addr->start);

    pdafc_2d->VersionNum = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                     pdaf_gmap_buffer[gain_offset+1];
    gain_offset += 2;
    pdafc_2d->MapWidth = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                   pdaf_gmap_buffer[gain_offset+1];
    gain_offset += 2;
    pdafc_2d->MapHeight = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                    pdaf_gmap_buffer[gain_offset+1];
    gain_offset += 2;
    property_get("persist.camera.swap.pdaf.enable", prop, "0");
    EnablePdSwap = (uint8_t) atoi(prop);

    /*******************************************************
     * if need to swap pd coordinate, please set EnablePdSwap = 1
     * default not swap pd coordinate
     **************************************************************/
    if (EnablePdSwap) {
        // Right ->left
        for (i = 0; i < PDAF_GRID_SIZE ; i++) {
            pdafc_2d->Right_GainMap[i] = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                                   pdaf_gmap_buffer[gain_offset+1];
            gain_offset += 2;
        }

        for (i = 0; i < PDAF_GRID_SIZE ; i++) {
            pdafc_2d->Left_GainMap[i] = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                                  pdaf_gmap_buffer[gain_offset+1];
            gain_offset += 2;
        }
   } else {
        // left -> left
        for (i = 0; i < PDAF_GRID_SIZE ; i++) {
            pdafc_2d->Left_GainMap[i] = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                                  pdaf_gmap_buffer[gain_offset+1];
            gain_offset += 2;
        }

        for (i = 0; i < PDAF_GRID_SIZE ; i++) {
            pdafc_2d->Right_GainMap[i] = (uint16_t)pdaf_gmap_buffer[gain_offset] << 8 |
                                                  pdaf_gmap_buffer[gain_offset+1];
            gain_offset += 2;
        }
    }

// DCC calibration
    pdaf_dcc_addr = &eeprom_addr_table[PDAF_DCC_INFO];
    flag = *(e_ctrl->eeprom_params.buffer + TRANSLATE_ADDR(pdaf_dcc_addr->flag));
    if (flag != VALID_FLAG) {
        SERR("PDAF DCC Info:flag invaild");
        return SENSOR_FAILURE;
    }

    rc = CheckSum_OTP(e_ctrl->eeprom_params.buffer,pdaf_dcc_addr,GLOBAL_OFFSET);
    if(rc < 0){
        SERR("PDAF DCC Info: checksum failed");
        return SENSOR_FAILURE;
    }
    SERR("PDAF DCC Info: checksum success");

    gain_offset = 0;
    pdaf_dcc_buffer = (uint8_t *)(e_ctrl->eeprom_params.buffer + pdaf_dcc_addr->start);


    pdafc_2d->Q_factor_DCC =  (uint16_t)pdaf_dcc_buffer[gain_offset] << 8 |
                                        pdaf_dcc_buffer[gain_offset+1];
    gain_offset += 2;
    pdafc_2d->MapWidth_DCC=  (uint16_t)pdaf_dcc_buffer[gain_offset] << 8 |
                                       pdaf_dcc_buffer[gain_offset+1];
    gain_offset += 2;
    pdafc_2d->MapHeight_DCC=  (uint16_t)pdaf_dcc_buffer[gain_offset] << 8 |
                                        pdaf_dcc_buffer[gain_offset+1];
    gain_offset += 2;

    for (i = 0; i < 48; i++) {
        pdafc_2d->PD_conversion_coeff[i] = (uint16_t)pdaf_dcc_buffer[gain_offset] << 8 |
                                                     pdaf_dcc_buffer[gain_offset+1];
        gain_offset += 2;
    }

#if USE_DEFAULT_PD_COORD
   SLOW("PD: no valid gain map, use default table");
   unsigned short left_gain[PDGAIN_LENGTH2D] = {
      252, 256, 257, 255, 253, 248, 241, 235, 232, 230, 229, 229,233, 240, 248, 255,
      258, 260, 259, 259, 255, 249, 242, 235, 231, 228, 226, 226,232, 242, 251, 258,
      263, 264, 264, 263, 258, 251, 242, 235, 230, 226, 223, 224,232, 244, 256, 265,
      267, 268, 268, 266, 260, 251, 242, 234, 229, 224, 220, 222,231, 246, 260, 270,
      269, 270, 270, 267, 260, 251, 240, 232, 225, 220, 216, 218,229, 246, 262, 273,
      270, 272, 271, 267, 261, 251, 239, 230, 224, 219, 215, 217,230, 247, 264, 274,
      270, 273, 272, 268, 261, 252, 239, 230, 224, 219, 214, 217,230, 248, 264, 275,
      271, 273, 272, 268, 262, 253, 241, 231, 225, 220, 215, 218,231, 248, 263, 273,
      271, 271, 271, 268, 263, 255, 244, 234, 228, 223, 219, 221,233, 249, 263, 272,
      268, 269, 269, 266, 262, 254, 245, 236, 229, 225, 221, 223,233, 248, 262, 269,
      265, 265, 265, 263, 260, 253, 244, 236, 231, 227, 224, 226,234, 247, 261, 268,
      261, 262, 262, 260, 257, 251, 245, 239, 234, 231, 229, 230,235, 245, 255, 263,
   };
   unsigned short right_gain[PDGAIN_LENGTH2D] = {
      271, 263, 255, 246, 238, 235, 237, 239, 242, 245, 251, 256,262, 266, 271, 270,
      275, 267, 257, 245, 235, 232, 233, 237, 240, 244, 250, 257,263, 269, 272, 274,
      278, 270, 257, 243, 231, 227, 228, 232, 237, 243, 250, 258,265, 271, 275, 276,
      282, 272, 257, 242, 228, 222, 224, 228, 234, 242, 250, 259,267, 273, 277, 277,
      285, 275, 259, 242, 226, 219, 220, 226, 233, 241, 251, 260,269, 276, 279, 280,
      286, 276, 260, 241, 225, 217, 219, 225, 231, 238, 248, 259,269, 277, 281, 281,
      286, 275, 259, 242, 224, 216, 218, 224, 230, 238, 248, 259,269, 276, 281, 282,
      284, 274, 259, 242, 225, 217, 219, 225, 232, 239, 249, 260,269, 276, 282, 283,
      282, 272, 257, 241, 225, 219, 221, 227, 232, 239, 249, 259,267, 274, 281, 282,
      278, 270, 256, 241, 228, 222, 224, 230, 235, 241, 250, 260,267, 274, 279, 282,
      273, 266, 254, 241, 230, 226, 229, 232, 236, 242, 251, 258,266, 270, 274, 277,
      267, 260, 251, 242, 234, 231, 232, 235, 239, 244, 251, 257,263, 267, 270, 272,
   };
   e_ctrl->eeprom_data.pdafc_2d.OffsetX   = 72;
   e_ctrl->eeprom_data.pdafc_2d.OffsetY   = 64;
   e_ctrl->eeprom_data.pdafc_2d.RatioX    = 8;
   e_ctrl->eeprom_data.pdafc_2d.RatioY    = 8;
   e_ctrl->eeprom_data.pdafc_2d.MapWidth  = 16;
   e_ctrl->eeprom_data.pdafc_2d.MapHeight = 12;
   e_ctrl->eeprom_data.pdafc_2d.PD_conversion_coeff[0] = -25525;
   memcpy(e_ctrl->eeprom_data.pdafc_2d.Left_GainMap, &left_gain,
    sizeof(unsigned short) * PDGAIN_LENGTH2D);
   memcpy(e_ctrl->eeprom_data.pdafc_2d.Right_GainMap, &right_gain,
    sizeof(unsigned short) * PDGAIN_LENGTH2D);
  }
  //e_ctrl->eeprom_data.pdaf_ptr = (void *)&e_ctrl->eeprom_data.pdafc_2d;

  SLOW("-------------PDAF GAIN DUMP------------");
  SLOW("OffsetX %d, OffsetY %d, RatioX %d, RatioY %d,"
       "MapWidth %d, MapHeight %d, PD_conversion_coeff[0] %d",
       pdafc_2d->OffsetX,
       pdafc_2d->OffsetY,
       pdafc_2d->RatioX,
       pdafc_2d->RatioY,
       pdafc_2d->MapWidth,
       pdafc_2d->MapHeight,
       pdafc_2d->PD_conversion_coeff[0]
  );

  SLOW("-----------Left_GainMap----------------");
  for (i = 0; i < PDGAIN_LENGTH2D; i = i + 16) {
   SLOW("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"
     "%d, %d, %d, %d",
     pdafc_2d->Left_GainMap[i],    pdafc_2d->Left_GainMap[i+1],
     pdafc_2d->Left_GainMap[i+2],  pdafc_2d->Left_GainMap[i+3],
     pdafc_2d->Left_GainMap[i+4],  pdafc_2d->Left_GainMap[i+5],
     pdafc_2d->Left_GainMap[i+6],  pdafc_2d->Left_GainMap[i+7],
     pdafc_2d->Left_GainMap[i+8],  pdafc_2d->Left_GainMap[i+9],
     pdafc_2d->Left_GainMap[i+10], pdafc_2d->Left_GainMap[i+11],
     pdafc_2d->Left_GainMap[i+12], pdafc_2d->Left_GainMap[i+13],
     pdafc_2d->Left_GainMap[i+14], pdafc_2d->Left_GainMap[i+15]
   );
  }
  SLOW("-----------Right_GainMap----------------");
  for (i = 0; i < PDGAIN_LENGTH2D; i = i + 16) {
   SLOW("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"
     "%d, %d, %d, %d",
     pdafc_2d->Right_GainMap[i],    pdafc_2d->Right_GainMap[i+1],
     pdafc_2d->Right_GainMap[i+2],  pdafc_2d->Right_GainMap[i+3],
     pdafc_2d->Right_GainMap[i+4],  pdafc_2d->Right_GainMap[i+5],
     pdafc_2d->Right_GainMap[i+6],  pdafc_2d->Right_GainMap[i+7],
     pdafc_2d->Right_GainMap[i+8],  pdafc_2d->Right_GainMap[i+9],
     pdafc_2d->Right_GainMap[i+10], pdafc_2d->Right_GainMap[i+11],
     pdafc_2d->Right_GainMap[i+12], pdafc_2d->Right_GainMap[i+13],
     pdafc_2d->Right_GainMap[i+14], pdafc_2d->Right_GainMap[i+15]
     );
  }
#endif

//printf info
  SLOW("-------------PDAF GAIN DUMP------------");
  SLOW("VersionNum: %d, OffsetX %d, OffsetY %d, RatioX %d, RatioY %d,"
       "MapWidth %d, MapHeight %d",
       pdafc_2d->VersionNum,
       pdafc_2d->OffsetX,
       pdafc_2d->OffsetY,
       pdafc_2d->RatioX,
       pdafc_2d->RatioY,
       pdafc_2d->MapWidth,
       pdafc_2d->MapHeight);
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
     pdafc_2d->Left_GainMap[i+16]);
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
     pdafc_2d->Right_GainMap[i+16]);
  }
  SLOW("-----------DCC----------------");
  SLOW("VersionNum: %d, MapWidth %d, MapHeight %d, Q factor %d",
   pdafc_2d->VersionNum,
   pdafc_2d->MapWidth_DCC,
   pdafc_2d->MapHeight_DCC,
   pdafc_2d->Q_factor_DCC);
  for (i = 0; i < 48; i = i + 8) {
   SLOW("%d, %d, %d, %d, %d, %d, %d, %d,",
     pdafc_2d->PD_conversion_coeff[i],    pdafc_2d->PD_conversion_coeff[i+1],
     pdafc_2d->PD_conversion_coeff[i+2],  pdafc_2d->PD_conversion_coeff[i+3],
     pdafc_2d->PD_conversion_coeff[i+4],  pdafc_2d->PD_conversion_coeff[i+5],
     pdafc_2d->PD_conversion_coeff[i+6],  pdafc_2d->PD_conversion_coeff[i+7]);
  }

   return SENSOR_SUCCESS;
}


static void s5k3p3_gt24c32_eeprom_get_calibration_items(void *e_ctrl)
{
    sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
    eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);

    e_items->is_wbc         = awb_present ? TRUE: FALSE;
    e_items->is_afc         = af_present ? TRUE: FALSE;
    e_items->is_lsc         = lsc_present ? TRUE: FALSE;
    e_items->is_dpc         = FALSE;
    e_items->is_insensor    = FALSE;
    e_items->is_ois         = FALSE;
    e_items->is_dual        = dc_present ? TRUE: FALSE;

    SLOW("is_wbc:%d,is_afc:%d,is_lsc:%d,is_dpc:%d,is_insensor:%d,is_ois:%d,e_items->is_dual:%d",
          e_items->is_wbc,e_items->is_afc,e_items->is_lsc,
          e_items->is_dpc,e_items->is_insensor, e_items->is_ois,e_items->is_dual);
}


/** s5k3p3_gt24c32_eeprom_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
void s5k3p3_gt24c32_eeprom_format_calibration_data(void *e_ctrl) {
    sensor_eeprom_data_t *ctrl = (sensor_eeprom_data_t *)e_ctrl;
    unsigned short rc = SENSOR_FAILURE;
    unsigned int i = 0;

    RETURN_VOID_ON_NULL(ctrl);

    SLOW("Total bytes in OTP buffer: %d", ctrl->eeprom_params.num_bytes);

    if (!ctrl->eeprom_params.buffer || !ctrl->eeprom_params.num_bytes) {
        SERR("Buff pointer %p buffer size %d", ctrl->eeprom_params.buffer,
              ctrl->eeprom_params.num_bytes);
        return;
    }

    for(i = 0; i < ctrl->eeprom_params.num_bytes; i++)
    {
        SLOW("ctrl->eeprom_params.buffer[%x]= %x", i, ctrl->eeprom_params.buffer[i]);
    }
    /* bypass the eeprom page crc check for allowing manual calibration */
    rc = s5k3p3_gt24c32_eeprom_format_module_info(ctrl);
    if( rc != SENSOR_SUCCESS){
        otp_check_result |= OTP_CHECK_MODULE_FAIL;
        SERR("Module Info: format fail");
    }

    rc = s5k3p3_gt24c32_eeprom_format_afdata(ctrl);
    if( rc != SENSOR_SUCCESS){
        otp_check_result |= OTP_CHECK_AF_FAIL;
        SERR("AF Info: format fail");
    }

    rc = s5k3p3_gt24c32_eeprom_format_wbdata(ctrl);
    if( rc != SENSOR_SUCCESS){
        otp_check_result |= OTP_CHECK_AWB_FAIL;
        SERR("WB Info: format fail");
    }

    rc = s5k3p3_gt24c32_eeprom_format_lensshading(ctrl);
    if( rc != SENSOR_SUCCESS){
        otp_check_result |= OTP_CHECK_LSC_FAIL;
        SERR("LSC Info: format fail");
    }

    rc = s5k3p3_gt24c32_eeprom_format_pdafgain(ctrl);
    if( rc != SENSOR_SUCCESS){
        otp_check_result |= OTP_CHECK_PDAF_FAIL ;
        SERR("PDAF Info: format fail");
    }

}

/** s5k3p3_gt24c32_eeprom_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* s5k3p3_gt24c32_eeprom_open_lib(void) {
    SLOW("enter");
    return &s5k3p3_gt24c32_eeprom_lib_func_ptr;
}
