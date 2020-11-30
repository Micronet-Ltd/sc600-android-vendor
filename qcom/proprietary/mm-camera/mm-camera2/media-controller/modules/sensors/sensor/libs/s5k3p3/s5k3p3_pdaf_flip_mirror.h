/**
 * s5k3p3_pdaf.h
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* uncomment PDAF2D_GAIN to enable 2d calibration pd parser */

 .vendor_id = QTI,
 .orientation= PDAF_ORIENTATION_MIRROR_AND_FLIP,
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
      {19, 12, PDAF_LEFT_PIXEL,},
      {19, 16, PDAF_RIGHT_PIXEL,},
      {23, 32, PDAF_LEFT_PIXEL,},
      {23, 28, PDAF_RIGHT_PIXEL,},
      {35, 16, PDAF_LEFT_PIXEL,},
      {35, 20, PDAF_RIGHT_PIXEL,},
      {39, 36, PDAF_LEFT_PIXEL,},
      {39, 32, PDAF_RIGHT_PIXEL,},
      {51, 36, PDAF_LEFT_PIXEL,},
      {51, 32, PDAF_RIGHT_PIXEL,},
      {55, 16, PDAF_LEFT_PIXEL,},
      {55, 20, PDAF_RIGHT_PIXEL,},
      {67, 32, PDAF_LEFT_PIXEL,},
      {67, 28, PDAF_RIGHT_PIXEL,},
      {71, 12, PDAF_LEFT_PIXEL,},
      {71, 16, PDAF_RIGHT_PIXEL,},
      {19, 68, PDAF_LEFT_PIXEL,},
      {19, 64, PDAF_RIGHT_PIXEL,},
      {23, 48, PDAF_LEFT_PIXEL,},
      {23, 52, PDAF_RIGHT_PIXEL,},
      {35, 64, PDAF_LEFT_PIXEL,},
      {35, 60, PDAF_RIGHT_PIXEL,},
      {39, 44, PDAF_LEFT_PIXEL,},
      {39, 48, PDAF_RIGHT_PIXEL,},
      {51, 44, PDAF_LEFT_PIXEL,},
      {51, 48, PDAF_RIGHT_PIXEL,},
      {55, 64, PDAF_LEFT_PIXEL,},
      {55, 60, PDAF_RIGHT_PIXEL,},
      {67, 48, PDAF_LEFT_PIXEL,},
      {67, 52, PDAF_RIGHT_PIXEL,},
      {71, 68, PDAF_LEFT_PIXEL,},
      {71, 64, PDAF_RIGHT_PIXEL,},
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

