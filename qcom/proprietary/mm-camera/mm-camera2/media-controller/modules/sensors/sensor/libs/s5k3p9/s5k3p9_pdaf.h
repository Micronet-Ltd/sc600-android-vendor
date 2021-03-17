/**
 * s5k3l8_pdaf.h
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* uncomment PDAF2D_GAIN to enable 2d calibration pd parser */
//#define PDAF2D_GAIN
 .vendor_id = QTI,
 .stats_dt = 0x30,
 .orientation= PDAF_ORIENTATION_DEFAULT,
 .sensor_native_pattern_info =
 {
  /* Res 0 */
  {
	/* Only for type3 PD sensor */
	/* For internal verfication purpose on type3 PD sensor */
   .block_pattern = {
    .pd_offset_horizontal = 16,
    .pd_offset_vertical = 16,
    .pix_count = 8,
    .block_dim =
    {
     .width= 32,
     .height = 16,
    },
    .pix_coords =
    {
		{20, 19, PDAF_LEFT_PIXEL,},
		{36, 19, PDAF_LEFT_PIXEL,},
		{28, 23, PDAF_RIGHT_PIXEL,},
		{44, 23, PDAF_RIGHT_PIXEL,},
		{20, 27, PDAF_RIGHT_PIXEL,},
		{36, 27, PDAF_RIGHT_PIXEL,},
		{28, 31, PDAF_LEFT_PIXEL,},
		{44, 31, PDAF_LEFT_PIXEL,},
    },
   },
   /* use estimate blk level instead of ref to chromatix */
   .block_count_horizontal = 288,//144,
   .block_count_vertical = 216,
  },
  /* Res 1 */
  {
	/* Only for type3 PD sensor */
	/* For internal verfication purpose on type3 PD sensor */
   .block_pattern = {
    .pd_offset_horizontal = 8,
    .pd_offset_vertical = 8,
    .pix_count = 8,
    .block_dim =
    {
     .width= 16,
     .height = 8,
    },
    .pix_coords =
    {
		{10, 9, PDAF_LEFT_PIXEL,},
		{18, 9, PDAF_LEFT_PIXEL,},
		{14, 11, PDAF_RIGHT_PIXEL,},
		{22, 11, PDAF_RIGHT_PIXEL,},
		{10, 13, PDAF_RIGHT_PIXEL,},
		{18, 13, PDAF_RIGHT_PIXEL,},
		{14, 15, PDAF_LEFT_PIXEL,},
		{22, 15, PDAF_LEFT_PIXEL,},
    },
   },
   /* use estimate blk level instead of ref to chromatix */
   .block_count_horizontal = 288,//144,
   .block_count_vertical = 216,
  },
  /* Res 2 */
  {
	/* Only for type3 PD sensor */
	/* For internal verfication purpose on type3 PD sensor */
   .block_pattern = {
    .pd_offset_horizontal = 8,
    .pd_offset_vertical = 8,
    .pix_count = 8,
    .block_dim =
    {
     .width= 16,
     .height = 8,
    },
    .pix_coords =
    {
		{10, 9, PDAF_LEFT_PIXEL,},
		{18, 9, PDAF_LEFT_PIXEL,},
		{14, 11, PDAF_RIGHT_PIXEL,},
		{22, 11, PDAF_RIGHT_PIXEL,},
		{10, 13, PDAF_RIGHT_PIXEL,},
		{18, 13, PDAF_RIGHT_PIXEL,},
		{14, 15, PDAF_LEFT_PIXEL,},
		{22, 15, PDAF_LEFT_PIXEL,},
    },
   },
   /* use estimate blk level instead of ref to chromatix */
   .block_count_horizontal = 288,//144,
   .block_count_vertical = 164,
  },
 },
 .buffer_block_pattern_info =
 {
  /* Res 0 */
  {
  .block_pattern = {
	.pix_count = 8, // NYNY
  	.pix_coords =
     {
	     {.x = 0, .y = 0, .flag = PDAF_LEFT_PIXEL,},
	     {.x = 1, .y = 0, .flag = PDAF_LEFT_PIXEL,},
	     {.x = 0, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
	     {.x = 1, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
	     {.x = 0, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
	     {.x = 1, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
		 {.x = 0, .y = 3, .flag = PDAF_LEFT_PIXEL,},
 	     {.x = 1, .y = 3, .flag = PDAF_LEFT_PIXEL,},
  	},
    .pd_offset_horizontal = 0, //NYNY
    .pd_offset_vertical = 0, // NYNY
	.block_dim = {
		.width = 2,
		.height = 4,
	},
  },
 .stride = 288*2,	/* How many bytes needed to store one line's PD pixels */
 .buffer_data_type = PDAF_DATA_TYPE_RAW16,
 .buffer_type = PDAF_BUFFER_FLAG_SEQUENTIAL_LINE,
  },
  /* Res 1 */
  {
  .block_pattern = {
	.pix_count = 8, // NYNY
  	.pix_coords =
     {
		{.x = 0, .y = 0, .flag = PDAF_LEFT_PIXEL,},
		{.x = 1, .y = 0, .flag = PDAF_LEFT_PIXEL,},
		{.x = 0, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 1, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 0, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 1, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 0, .y = 3, .flag = PDAF_LEFT_PIXEL,},
		{.x = 1, .y = 3, .flag = PDAF_LEFT_PIXEL,},
  	},
    .pd_offset_horizontal = 0, //NYNY
    .pd_offset_vertical = 0, // NYNY
	.block_dim = {
		.width = 2,
		.height = 4,
	},
  },
 .stride = 288*2,	/* How many bytes needed to store one line's PD pixels */
 .buffer_data_type = PDAF_DATA_TYPE_RAW16,
 .buffer_type = PDAF_BUFFER_FLAG_SEQUENTIAL_LINE,
  },
  /* Res 2 */
  {
  .block_pattern = {
	.pix_count = 8, // NYNY
  	.pix_coords =
     {
		{.x = 0, .y = 0, .flag = PDAF_LEFT_PIXEL,},
		{.x = 1, .y = 0, .flag = PDAF_LEFT_PIXEL,},
		{.x = 0, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 1, .y = 1, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 0, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 1, .y = 2, .flag = PDAF_RIGHT_PIXEL,},
		{.x = 0, .y = 3, .flag = PDAF_LEFT_PIXEL,},
		{.x = 1, .y = 3, .flag = PDAF_LEFT_PIXEL,},
  	},
    .pd_offset_horizontal = 0, //NYNY
    .pd_offset_vertical = 0, // NYNY
	.block_dim = {
		.width = 2,
		.height = 4,
	},
  },
 .stride = 288*2,	/* How many bytes needed to store one line's PD pixels */
 .buffer_data_type = PDAF_DATA_TYPE_RAW16,
 .buffer_type = PDAF_BUFFER_FLAG_SEQUENTIAL_LINE,
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
 .defocus_confidence_th = 250,
