/*============================================================================

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../eeprom_util/eeprom.h"
#include "eeprom_lib.h"
#include "eeprom_util.h"
#include "debug_lib.h"
#include "sensor_lib.h"


#define VALID_FLAG             0x01
#define QVALUE                 1023.0
#define LSC_GRID_SIZE          221
#define PDAF_GRID_SIZE         221
#define LSC_VALUE              256.0
#define INVALID_DATA           0xFFFF

#define MOD_8BIT               255
#define GLOBAL_OFFSET          0
#define TRANSLATE_ADDR(addr) ((addr < GLOBAL_OFFSET) ? 0: (addr - GLOBAL_OFFSET))

#define INF_MARGIN          (-0.35)
#define MACRO_MARGIN        (0.30)
#define USE_DEFAULT_PD_COORD  0

typedef struct eeprom_reg_addr {
    uint16_t flag;
    uint16_t start;
    uint16_t end;
    uint16_t checksum;
}st_reg_addr;

typedef enum{
//normal otp info check
  OTP_CHECK_SUCCESS           = ( 0 ),
  OTP_CHECK_MODULE_FAIL       = ( 1 << 0 ),
  OTP_CHECK_AF_FAIL           = ( 1 << 1 ),
  OTP_CHECK_AWB_FAIL          = ( 1 << 2 ),
  OTP_CHECK_LSC_FAIL          = ( 1 << 3 ),
  OTP_CHECK_PDAF_FAIL         = ( 1 << 4 ),
  OTP_CHECK_DC_DATA_FAIL      = ( 1 << 5 ),
  OTP_CHECK_AEC_DATA_FAIL     = ( 1 << 6 ),
  OTP_CHECK_TOTAL_SUM_FAIL    = ( 1 << 7 ),
  OTP_CHECK_MAX,
}otp_result_t;

typedef enum{
    MODULE_INFO = 0,
    AWB_INFO,
    AF_INFO,
    LSC_INFO,
    PDAF_GMAP_INFO,
    PDAF_DCC_INFO,
    DC_DATA_INFO,
    AEC_DATA_INFO,
    DATA_TABLE_MAX,
} eeprom_table_index;

typedef struct {
    unsigned char brightness_ratio_h;
    unsigned char brightness_ratio_l;
    unsigned char ref_bayer_gain_h;
    unsigned char ref_bayer_gain_l;
    unsigned char ref_bayer_linecount_h;
    unsigned char ref_bayer_linecount_l;
    unsigned char ref_mono_gain_h;
    unsigned char ref_mono_gain_l;
    unsigned char ref_mono_linecount_h;
    unsigned char ref_mono_linecount_l;
}aec_sync_t;


typedef struct {
    unsigned char module_id;
    unsigned char module_version;
    unsigned char software_version;
	unsigned char reserved0;
    unsigned char year;
    unsigned char month;
    unsigned char day;
} module_info_t;

typedef struct {
    unsigned char macro_h;
    unsigned char macro_l;
    unsigned char infinity_h;
    unsigned char infinity_l;
    unsigned char startCurrent_h;
    unsigned char startCurrent_l;
} af_data_t;

typedef struct {
    unsigned char r_over_gr_h;
    unsigned char r_over_gr_l;
    unsigned char b_over_gr_h;
    unsigned char b_over_gr_l;
    unsigned char gb_over_gr_h;
    unsigned char gb_over_gr_l;
} awb_data_t;

static st_reg_addr eeprom_addr_table[] = {
/**  flag,  start,  end,  checksum**/
    {0x0000,0x0001,0x0007,0x0008}, /*MODULE_INFO */
    {0x0019,0x001A,0x0025,0x0026}, /*AWB_INFO*/
    {0x000C,0x000D,0x0012,0x0013}, /*AF_INFO*/
    {0x0027,0x0028,0x070F,0x0710}, /*LSC_INFO*/
    {0x0711,0x0712,0x0B47,0x0B48}, /*PDAF_GMAP_INFO*/
    {0x0D99,0x0D9A,0x0D9D,0x0D9E}, /*PDAF_DCC_INFO*/
};

static uint16_t otp_check_result = OTP_CHECK_SUCCESS;
static unsigned char awb_present      = FALSE;
static unsigned char af_present       = FALSE;
static unsigned char lsc_present      = FALSE;
static unsigned char insensor_present = FALSE;
static unsigned char dc_present       = FALSE;

static void s5k3p3_gt24c32_eeprom_get_calibration_items(void *e_ctrl);
static void s5k3p3_gt24c32_eeprom_format_calibration_data(void *e_ctrl);
//static boolean s5k3p3_gt24c32_autofocus_calibration(void *e_ctrl);
extern int fchmod(int fd, mode_t mode);
static eeprom_lib_func_t s5k3p3_gt24c32_eeprom_lib_func_ptr= {
  .get_calibration_items    = s5k3p3_gt24c32_eeprom_get_calibration_items,
  .format_calibration_data  = s5k3p3_gt24c32_eeprom_format_calibration_data,
  .do_af_calibration        = eeprom_autofocus_calibration,
  .do_wbc_calibration       = eeprom_whitebalance_calibration,
  .do_lsc_calibration       = eeprom_lensshading_calibration,
  .get_raw_data             = NULL,
  .get_ois_raw_data         = NULL,
  .eeprom_info =
  {
    .power_setting_array =
    {
      .power_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 0,
        },
      },
      .size = 1,
      .power_down_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 0,
        },
      },
      .size_down = 1,
     },
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .mem_map_array =
    {
      .memory_map =
      {
        {
          .slave_addr = 0xb0,
          .mem_settings =
          {
            {0x0000, CAMERA_I2C_WORD_ADDR,
              0x0d9F, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0 },
          },
          .memory_map_size = 1,
        },
      },
      .size_map_array = 1,
    },
    //.userspace_probe = 0,
  },
};
