/*============================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.
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

#define QVALUE                   512
#define S5K4H7_FLAG_GROUP0       0x0d
#define S5K4H7_FLAG_GROUP1       0x04

/* Flag addr */
#define LSC_GRID_SIZE           221
#define MOD_8BIT 255
typedef enum{
//normal otp info check
  OTP_CHECK_SUCCESS        = ( 0 ),
  OTP_CHECK_AF_FAIL        = ( 1 << 0 ),
  OTP_CHECK_AWB_FAIL       = ( 1 << 1 ),
  OTP_CHECK_LSC_FAIL       = ( 1 << 2 ),
  OTP_CHECK_PDAF_FAIL      = ( 1 << 3 ),
  OTP_CHECK_MODULE_FAIL    = ( 1 << 4 ),
  OTP_CHECK_CCT_FAIL       = ( 1 << 5 ),
  OTP_CHECK_STATION_FAIL   = ( 1 << 6 ),
  OTP_CHECK_TOTAL_SUM_FAIL = ( 1 << 7 ),
  OTP_CHECK_MAX,
}otp_result_t;


typedef struct {
  unsigned char module_id;
  unsigned char year;
  unsigned char month;
  unsigned char day;
  unsigned char lens_id;
  unsigned char vcm_id;
  unsigned char driverIC_id;
} module_info_t;

typedef struct {
  unsigned char r_over_gr_h;
  unsigned char r_over_gr_l;
  unsigned char b_over_gb_h;
  unsigned char b_over_gb_l;
  unsigned char gb_over_gr_h;
  unsigned char gb_over_gr_l;
  unsigned char golden_r_over_gr_h;
  unsigned char golden_r_over_gr_l;
  unsigned char golden_b_over_gb_h;
  unsigned char golden_b_over_gb_l;
  unsigned char golden_gb_over_gr_h;
  unsigned char golden_gb_over_gr_l;
} awb_data_t;

typedef struct {
    unsigned char infinity_h;
    unsigned char infinity_l;
    unsigned char macro_h;
    unsigned char macro_l;
    unsigned char checksum;
} af_data_t;

enum eeprom_table_index {
    MODULE_INFO,
    AWB_INFO,
    LSC_INFO,
    AF_INFO,
    INFO_MAX,
};

#define GLOBAL_OFFSET 0x201
#define TRANSLATE_ADDR(addr) ((addr < GLOBAL_OFFSET) ? 0: (addr - GLOBAL_OFFSET))


typedef struct s5k4h7_otp_reg_addr {
  uint16_t flag;
  uint16_t start;
  uint16_t end;
  uint16_t checksum;
}st_reg_addr;

typedef enum {
  GROUP_0 = 0,
  GROUP_1,
  GROUP_MAX,
}s5k4h7_groups_count;


#define BASE_PAGE_ADDR               0xa04
#define PER_PAGE_SIZE                0x40
#define LSC_DATA_SIZE                360

#define AWB_DATA_SIZE                13



#define LSC_GROUP0_ADDR_START   1
#define LSC_GROUP0_ADDR_END     LSC_GROUP0_ADDR_START + LSC_DATA_SIZE - 1

#define LSC_GROUP1_ADDR_START   LSC_GROUP0_ADDR_END + 1
#define LSC_GROUP1_ADDR_END     LSC_GROUP1_ADDR_START + LSC_DATA_SIZE - 1

/***********************************************************************/

#define FLAG_AWB_ADDR             LSC_GROUP1_ADDR_END   + 1
#define AWB_GROUP0_START          FLAG_AWB_ADDR         + 8
#define AWB_GROUP0_END            AWB_GROUP0_START      + 11
#define AWB_GROUP0_CHECKSUM       AWB_GROUP0_END        + 1


#define FLAG_MODULE_INFO_ADDR      FLAG_AWB_ADDR   + 1

#define LSC_FLAG_ADDR             (LSC_GROUP1_ADDR_END + (0xa2d - 0xa04) + 1)
#define LSC_CHECKSUM_GROUP0        LSC_FLAG_ADDR        + 1
#define LSC_CHECKSUM_GROUP1        LSC_CHECKSUM_GROUP0  + 1

#define LSC_DATA_TOTAL_SIZE        (11 * PER_PAGE_SIZE + (0xa13 - 0xa04 + 1))


#define AF_DATA_SIZE                5

#define AF_FLAG_ADDR                ((0xa30 - 0xa04) + LSC_DATA_TOTAL_SIZE)
#define AF_GROUP0_START            (AF_FLAG_ADDR + 1)
#define AF_GROUP0_END              (AF_FLAG_ADDR +  AF_DATA_SIZE)
#define AF_GROUP0_CHECKSUM         (AF_GROUP0_END + AF_DATA_SIZE + 1)

#define MODULE_INFO_START          FLAG_AWB_ADDR + 1

#define INF_MARGIN          (-0.35)
#define MACRO_MARGIN        (0.30)

static uint16_t otp_check_result = OTP_CHECK_SUCCESS;
static unsigned char awb_present      = FALSE;
static unsigned char af_present       = FALSE;
static unsigned char lsc_present      = FALSE;
static unsigned char insensor_present = FALSE;

static int s5k4h7_get_raw_data(void *e_ctrl,void *data);
static void s5k4h7_eeprom_get_calibration_items(void *e_ctrl);
static void s5k4h7_eeprom_format_calibration_data(void *e_ctrl);

static eeprom_lib_func_t s5k4h7_eeprom_lib_func_ptr = {
  .get_calibration_items    = s5k4h7_eeprom_get_calibration_items,
  .format_calibration_data  = s5k4h7_eeprom_format_calibration_data,
  .do_af_calibration        = eeprom_autofocus_calibration,
  .do_wbc_calibration       = eeprom_whitebalance_calibration,
  .do_lsc_calibration       = NULL,//eeprom_lensshading_calibration,//if exist lsc data,must add this function
  .get_raw_data             = s5k4h7_get_raw_data, //
  .get_ois_raw_data         = NULL,
  .eeprom_info =
  {
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
              .config_val = 1,
              .delay = 0,
            },
            {
              .seq_type = CAMERA_POW_SEQ_VREG,
              .seq_val = CAMERA_VDIG,
              .config_val = 0,
              .delay = 1,
            },
            {
              .seq_type = CAMERA_POW_SEQ_VREG,
              .seq_val = CAMERA_VIO,
              .config_val = 1,
              .delay = 0,
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
              .delay = 1,
            },
            {
              .seq_type = CAMERA_POW_SEQ_GPIO,
              .seq_val = CAMERA_GPIO_STANDBY,
              .config_val = GPIO_OUT_HIGH,
              .delay = 1,
            },
        },
        .size = 8,
        .power_down_setting_a =
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
              .seq_type = CAMERA_POW_SEQ_CLK,
              .seq_val = CAMERA_MCLK,
              .config_val = 0,
              .delay = 1,
            },
            {
              .seq_type = CAMERA_POW_SEQ_VREG,
              .seq_val = CAMERA_VIO,
              .config_val = 0,
              .delay = 0,
            },
            {
              .seq_type = CAMERA_POW_SEQ_VREG,
              .seq_val = CAMERA_VDIG,
              .config_val = 0,
              .delay = 1,
            },
            {
              .seq_type = CAMERA_POW_SEQ_VREG,
              .seq_val = CAMERA_VANA,
              .config_val = 0,
              .delay = 0,
            },
        },
     .size_down = 6,
     },
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .mem_map_array =
    {
      .memory_map =
      {
        {
          .slave_addr = 0x5a,
          .mem_settings =
          {
            {0x0136, CAMERA_I2C_WORD_ADDR, 0x18, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0137, CAMERA_I2C_WORD_ADDR, 0x00, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0305, CAMERA_I2C_WORD_ADDR, 0x06, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0306, CAMERA_I2C_WORD_ADDR, 0x00, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0307, CAMERA_I2C_WORD_ADDR, 0x8C, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x030D, CAMERA_I2C_WORD_ADDR, 0x06, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x030E, CAMERA_I2C_WORD_ADDR, 0x00, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x030F, CAMERA_I2C_WORD_ADDR, 0xAF, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0301, CAMERA_I2C_WORD_ADDR, 0x04, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},

            {0x0100, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50}, //STREAMON
/*PAGE 0**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0X00, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A3d, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 1**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0X01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high

/*PAGE 2**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x02, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high

/*PAGE 3**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0X03, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 4**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x04, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 5**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x05, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 6**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x06, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 7**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x07, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 8**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x08, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 9**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x09, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ,0},//high
/*PAGE 10**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x0a, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//high
/*PAGE 11**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x0b, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x40, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},//
/*PAGE 12**/
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x0c, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x10, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},

//page 21
            {0x0A02, CAMERA_I2C_WORD_ADDR, 0x15, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0A00, CAMERA_I2C_WORD_ADDR, 0x01, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 50},
            {0x0A04, CAMERA_I2C_WORD_ADDR, 0x37, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0},

//read end
            {0x0a00, CAMERA_I2C_WORD_ADDR, 0x04,  CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
            {0x0a00, CAMERA_I2C_WORD_ADDR, 0x00,  CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_WRITE, 0},
          },
          .memory_map_size = 54,
        },
      },
      .size_map_array = 1,
    },
  },
};
