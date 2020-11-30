/**
 * s5k3p3_pdaf.h
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* uncomment PDAF2D_GAIN to enable 2d calibration pd parser */
//#define PDAF2D_GAIN
 .vendor_id = QTI,
 .orientation= PDAF_ORIENTATION_DEFAULT,
 .sensor_native_pattern_info =
 {
  /* Res 0 */
  {
   .block_pattern = {
    .pd_offset_horizontal = 12,
    .pd_offset_vertical = 12,
    .pix_count = 32,
    .block_dim =
    {
     .width= 64,
     .height = 64,
    },
    .pix_coords =
    {
     {16, 19, PDAF_LEFT_PIXEL,},
     {16, 23, PDAF_RIGHT_PIXEL,},
     {32, 23, PDAF_LEFT_PIXEL,},
     {32, 27, PDAF_RIGHT_PIXEL,},
     {52, 23, PDAF_LEFT_PIXEL,},
     {52, 27, PDAF_RIGHT_PIXEL,},
     {68, 19, PDAF_LEFT_PIXEL,},
     {68, 23, PDAF_RIGHT_PIXEL,},
     {20, 39, PDAF_LEFT_PIXEL,},
     {20, 35, PDAF_RIGHT_PIXEL,},
     {36, 43, PDAF_LEFT_PIXEL,},
     {36, 39, PDAF_RIGHT_PIXEL,},
     {48, 43, PDAF_LEFT_PIXEL,},
     {48, 39, PDAF_RIGHT_PIXEL,},
     {64, 39, PDAF_LEFT_PIXEL,},
     {64, 35, PDAF_RIGHT_PIXEL,},
     {20, 55, PDAF_LEFT_PIXEL,},
     {20, 59, PDAF_RIGHT_PIXEL,},
     {36, 51, PDAF_LEFT_PIXEL,},
     {36, 55, PDAF_RIGHT_PIXEL,},
     {48, 51, PDAF_LEFT_PIXEL,},
     {48, 55, PDAF_RIGHT_PIXEL,},
     {64, 55, PDAF_LEFT_PIXEL,},
     {64, 59, PDAF_RIGHT_PIXEL,},
     {16, 75, PDAF_LEFT_PIXEL,},
     {16, 71, PDAF_RIGHT_PIXEL,},
     {32, 71, PDAF_LEFT_PIXEL,},
     {32, 67, PDAF_RIGHT_PIXEL,},
     {52, 71, PDAF_LEFT_PIXEL,},
     {52, 67, PDAF_RIGHT_PIXEL,},
     {68, 75, PDAF_LEFT_PIXEL,},
     {68, 71, PDAF_RIGHT_PIXEL,},
    },
   },
   /* use estimate blk level instead of ref to chromatix */
   .block_count_horizontal = 72,
   .block_count_vertical = 54,
  },
 },
 .buffer_block_pattern_info =
 {
  /* Res 0 */
  {
 .buffer_data_type = PDAF_DATA_TYPE_RAW10_LSB,
 .buffer_type = PDAF_BUFFER_FLAG_SPARSE,
  },
 },
 .black_level = 64,
 .cali_version = CALIBRATION_VERSION_2D,
 .window_configure = {
  .pdaf_sw_window_mode = FIXED_GRID_WINDOW,
  /* ingore the floating window configuration */
  .fixed_grid_window_configure =
  {
   .af_fix_window = {
    .pdaf_address_start_hori = 0.33333,
    .pdaf_address_start_ver  = 0.33334,
    .pdaf_address_end_hori   = 0.66668,
    .pdaf_address_end_ver    = 0.66668,
   },
   .window_number_hori = 1,
   .window_number_ver  = 1,
  },
 },
 .defocus_confidence_th = 200,
