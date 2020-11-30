/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017 by Dolby Laboratories
 *                            All rights reserved.
 ******************************************************************************/

/*
* All codes in this file are taken from different Dolby IIDK source files.
*/

#ifndef DAWS_UTIL_H
#define DAWS_UTIL_H


#define SAMPLE_RATE             (48000ul)
#define MAX_EXAMPLE_IO_CHANNELS (DAP_CPDP_MAX_NUM_OUTPUT_CHANNELS)

#define MAX_NAME_SIZE              32
#define MAX_BRAND_SIZE             MAX_NAME_SIZE
#define MAX_DAP_PRESET_PROFILES    PRESET_MAX_MODES

#define MAX_VIRT_CHANNELS_COUNT    (8)
#define MAXIMUM_IN_CHANNELS            (MAX_VIRT_CHANNELS_COUNT)
#define MAXIMUM_OUT_CHANNELS           (MAX_VIRT_CHANNELS_COUNT)

#define DAWS_DAP_SUPPORT_VERSION   "2.5.4"
#define DAWS_DAP_SUPPORT_VERSION_SZ (5)


//Taken from dap_cpdp.h
#define DAP_CPDP_VIRTUAL_BASS_SUBGAINS_NUM             (3)
#define DAP_CPDP_REGULATOR_TUNING_BANDS_MAX            (20u)
#define DAP_CPDP_MAX_NUM_PROCESS_CHANNELS              (12)
#define DAP_CPDP_MAX_NUM_OUTPUT_CHANNELS               (12)
#define DAP_CPDP_GRAPHIC_EQUALIZER_BANDS_MAX           (20u)
#define DAP_CPDP_IEQ_MAX_BANDS_NUM                     (20u)
#define DAP_CPDP_AUDIO_OPTIMIZER_BANDS_MAX             (20u)
#define DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX        (20u)

#define CONFIG_FILE_INIT_ERR_STR "CONFIG_FILE_INIT_ERR_STR"
#define XML_PARSER_ERR_STR       "XML_PARSER_ERR_STR"
#define INVALID_SPKR_CFG_ERR_STR "INVALID_SPKR_CFG_ERR_STR"
#define TERMINATING_APPL_STR     "TERMINATING_APPL_STR"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//Taken from daws_options.h
/**
 * @brief Wireless Speaker processing modes
 */
typedef enum
{
     PRESET_DYNAMIC_MODE = 0/**< Processing optimized for dynamic content */
    ,PRESET_MOVIE_MODE      /**< Processing optimized for movie content */
    ,PRESET_MUSIC_MODE      /**< Processing optimized for music content */
    ,PRESET_GAME_MODE       /**< Processing optimized for game content  */
    ,PRESET_VOICE_MODE      /**< Processing optimized for voice content */
    ,PRESET_PERSONALIZE_MODE/**< Processing optimized for Personal content  */
    ,PRESET_OFF_MODE        /**< No optimization for processing content  */
    ,PRESET_CUSTOM_A_MODE   /**< Processing optimized for Custom A content  */
    ,PRESET_CUSTOM_B_MODE   /**< Processing optimized for Custom B content  */
    ,PRESET_CUSTOM_C_MODE   /**< Processing optimized for Custom C content  */
    ,PRESET_CUSTOM_D_MODE   /**< Processing optimized for Custom D content  */
    ,PRESET_MAX_MODES
} daws_preset_mode;

/**
 * @brief Maximum Main Speaker Output Supported.
 * In addition to these output speaker configuration supported,
 * Dolby Audio for Wireless Speaker products also support a SUB component,
 * Side and Height Channels (Upto 2 Height Channels can be specified)
 */
typedef enum
{
     DAWS_SPEAKER_OUTPUT_MONO_CHANNEL = 1  /**< Mono Channel Configuration      */
    ,DAWS_SPEAKER_OUTPUT_STEREO_CHANNEL    /**< Stero Channel Configuration     */
    ,DAWS_SPEAKER_OUTPUT_THREE_CHANNEL     /**< Three Channel Configuration     */
    ,DAWS_SPEAKER_OUTPUT_MAX_MODES         /**< Max Modes */
} daws_main_spkr_output;

/**
 * @brief Maximum Side Speaker Output Supported.
 * Side Speakers can be enabled/disabled
 */
typedef enum
{
     DAWS_SIDE_SPEAKER_OUTPUT_DISABLED   /**< Disable Side Speaker Config.  */
    ,DAWS_SIDE_SPEAKER_OUTPUT_ENABLED    /**< Enable Side Speaker Config.   */
    ,DAWS_SIDE_SPEAKER_OUTPUT_MAX_MODES  /**< Max Modes */
} daws_side_spkr_output;


/**
 * @brief Complete Speaker configuration
 * Given the main speaker number, side, height and sub-component
 * status, the output generated by the DAP will conform to one
 * overall configuration.
 */
typedef enum
{
      DAWS_MONO_SIDE_DISABLE_HEIGHT_DISABLE_NO_SUB    /* 1.0.0 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_DISABLE_NO_SUB     /* 3.0.0 */
     ,DAWS_MONO_SIDE_DISABLE_HEIGHT_SINGLE_NO_SUB     /* 1.0.1 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_SINGLE_NO_SUB      /* 3.0.1 */
     ,DAWS_MONO_SIDE_DISABLE_HEIGHT_DOUBLE_NO_SUB     /* 1.0.2 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_DOUBLE_NO_SUB      /* 3.0.2 */
     ,DAWS_MONO_SIDE_DISABLE_HEIGHT_DISABLE_SUB       /* 1.1.0 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_DISABLE_SUB        /* 3.1.0 */
     ,DAWS_MONO_SIDE_DISABLE_HEIGHT_SINGLE_SUB        /* 1.1.1 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_SINGLE_SUB         /* 3.1.1 */
     ,DAWS_MONO_SIDE_DISABLE_HEIGHT_DOUBLE_SUB        /* 1.1.2 */
     ,DAWS_MONO_SIDE_ENABLE_HEIGHT_DOUBLE_SUB         /* 3.1.2 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_DISABLE_NO_SUB     /* 2.0.0 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_DISABLE_NO_SUB      /* 4.0.0 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_SINGLE_NO_SUB      /* 2.0.1 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_SINGLE_NO_SUB       /* 4.0.1 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_DOUBLE_NO_SUB      /* 2.0.2 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_DOUBLE_NO_SUB       /* 4.0.2 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_DISABLE_SUB        /* 2.1.0 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_DISABLE_SUB         /* 4.1.0 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_SINGLE_SUB         /* 2.1.1 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_SINGLE_SUB          /* 4.1.1 */
     ,DAWS_TWO_SIDE_DISABLE_HEIGHT_DOUBLE_SUB         /* 2.1.2 */
     ,DAWS_TWO_SIDE_ENABLE_HEIGHT_DOUBLE_SUB          /* 4.1.2 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_DISABLE_NO_SUB   /* 3.0.0 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_DISABLE_NO_SUB    /* 5.0.0 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_SINGLE_NO_SUB    /* 3.0.1 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_SINGLE_NO_SUB     /* 5.0.1 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_DOUBLE_NO_SUB    /* 3.0.2 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_DOUBLE_NO_SUB     /* 5.0.2 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_DISABLE_SUB      /* 3.1.0 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_DISABLE_SUB       /* 5.1.0 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_SINGLE_SUB       /* 3.1.1 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_SINGLE_SUB        /* 5.1.1 */
     ,DAWS_THREE_SIDE_DISABLE_HEIGHT_DOUBLE_SUB       /* 3.1.2 */
     ,DAWS_THREE_SIDE_ENABLE_HEIGHT_DOUBLE_SUB        /* 5.1.2 */
     ,DAWS_MAX_CONFIGURATION
} daws_spkr_config;


/**
 * @brief Enum values of the DAP Speaker Port
 * Tuning Tool specific port handlers.
 */
typedef enum
{
    HDMI_PORT,      /**< Tuning Tool uses a HDMI port to contact the device to be tuned */
    USB_PORT,       /**< Tuning Tool uses a USB port to contact the device to be tuned */
    BLUETOOTH_PORT, /**< Tuning Tool uses Bluetooth to contact the device to be tuned */
    OTHER_PORT      /**< Tuning Tool uses other Misc port to contact the device to be tuned */
} daws_dap_speaker_port;


/**
 * @brief The Audio Regulator structure used by DAP
 * For more information, please use the dap_cpdp/include/dap_cpdp.h
 * in the DAP 2.5.4 CIDK delivered seperately
 */
struct daws_dap_regulator
{
    uint32_t bands_num;        /**< The number of bands being configured for this instance of DAP Audio Regulator */
    int32_t relaxation_amount;    /**< Controls the amount that the DAP Audio Regulator can control the distortion masking algorithm */
    int32_t overdrive;            /**< Controls the amount of boost to be applied to low and high thresholds */
    int32_t timbre_preservation; /**< Timbre preservation in DAP */
    int32_t speaker_dist_enable; /**< Sets the operating mode of the DAP Audio Regulator */
    int32_t thresh_low[DAP_CPDP_REGULATOR_TUNING_BANDS_MAX];      /**< Specify the lower excitation threshold that will cause the DAP Audio Regulator to kick in */
    int32_t thresh_high[DAP_CPDP_REGULATOR_TUNING_BANDS_MAX];     /**< Specify the high excitation threshold that will cause the DAP Audio Regulator to kick in */
    int32_t isolated_bands[DAP_CPDP_REGULATOR_TUNING_BANDS_MAX];  /**< Specify if particular bands must be isolated from DAP Audio Regulator */
    uint32_t freqs[DAP_CPDP_REGULATOR_TUNING_BANDS_MAX];  /**< Frequency bands being worked on by the DAP Audio Regulator */
};


/**
 * @brief The Audio Regulator structure used by DAP
 * For more information, please use the dap_cpdp/include/dap_cpdp.h
 * in the DAP 2.5.4 CIDK delivered seperately
 * Virtual Bass must be disabled in "Dolby Audio for Wireless Speaker" products
 */
struct virtual_bass
{
    int32_t mode;
    int32_t overall_gain;
    int32_t slope_gain;
    uint32_t low_src_freq;
    uint32_t high_src_freq;
    int32_t subgains[DAP_CPDP_VIRTUAL_BASS_SUBGAINS_NUM];
    uint32_t low_mix_freq;
    uint32_t high_mix_freq;
};


/**
 * @brief DAWS Content Processing structure
 * For more information, please use the dap_cpdp/include/dap_cpdp.h
 * in the DAP 2.5.4 CIDK delivered seperately
 */
struct daws_profile_processing {
    int32_t audio_optimizer_enable;
    uint32_t audio_optimizer_bands_num;
    int32_t audio_optimizer_gains[DAP_CPDP_MAX_NUM_OUTPUT_CHANNELS][DAP_CPDP_AUDIO_OPTIMIZER_BANDS_MAX];
    uint32_t audio_optimizer_freq[DAP_CPDP_AUDIO_OPTIMIZER_BANDS_MAX];

    int32_t be_enable;
    int32_t bex_enable;
    int32_t be_boost;
    int32_t be_cutoff_freq;
    int32_t be_width;
    int32_t bex_cutoff_freq;

    int32_t calibration_boost;

    int32_t dialog_enhancer_enable;
    int32_t dialog_enhancer_amount;
    int32_t dialog_enhancer_ducking;

    int32_t graphic_equalizer_enable;
    uint32_t graphic_equalizer_num;
    uint32_t graphic_equalizer_freqs[DAP_CPDP_GRAPHIC_EQUALIZER_BANDS_MAX];
    int32_t graphic_equalizer_gains[DAP_CPDP_GRAPHIC_EQUALIZER_BANDS_MAX];

    int32_t height_filter_mode;

    /* Intelligent Equalizer */
    int32_t ieq_enable;
    uint32_t ieq_num;
    int32_t ieq_amount;
    int32_t ieq_bands_set;
    uint32_t ieq_freqs[DAP_CPDP_IEQ_MAX_BANDS_NUM];
    int32_t ieq_gains[DAP_CPDP_IEQ_MAX_BANDS_NUM];

    /* Media Intelligence Steering Controls */
    int32_t mi_dialog_enhancer_steering_enable;
    int32_t mi_dv_leveler_steering_enable;
    int32_t mi_ieq_steering_enable;
    int32_t mi_surround_compressor_steering_enable;
    int32_t mi_virtualizer_steering_enable;

    /* Gains */
    int32_t postgain;
    int32_t pregain;
    int32_t systemgain;

    /* Process Optimizer Variables */
    int32_t proc_optimizer_enable;
    int32_t proc_optimizer_gain[DAP_CPDP_MAX_NUM_PROCESS_CHANNELS][DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
    uint32_t proc_optimizer_freq[DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
    uint32_t proc_optimizer_bands_num;

    int32_t regulator_enable;
    struct daws_dap_regulator regulator;

    int32_t surround_boost;
    int32_t surround_decoder_enable;

    uint32_t virtualizer_front_speaker_angle;
    uint32_t virtualizer_height_speaker_angle;
    uint32_t virtualizer_surround_speaker_angle ;

    struct virtual_bass vb;

    int32_t volmax_boost;
    int32_t volume_leveler_enable;
    int32_t volume_leveler_amount;
    int32_t volume_leveler_in_target;
    int32_t volume_leveler_out_target;
    int32_t volume_modeler_calibration;
    int32_t volume_modeler_enable;

    int32_t spkr_virt_mode;             /**< Speaker Virtualization Mode: DAWS uses DAP_CPDP_PROCESS_5_1_2_SPEAKER */
    int32_t nb_output_channels;         /**< Number of output channels for which Tuning XML file is configured for */
    int32_t mix_matrix[MAXIMUM_OUT_CHANNELS * MAXIMUM_IN_CHANNELS]; /**< Pointer to the channel mix matrix */

    uint32_t center_spreading;
};


/**
 * @brief DAP Supported Sample Rates
 * DAP supports the following sample rates as enumerated here
 */
typedef enum
{
   DAP_SUPPORTED_SAMPLE_RATE_32_KHZ
  ,DAP_SUPPORTED_SAMPLE_RATE_44_1_KHZ
  ,DAP_SUPPORTED_SAMPLE_RATE_48_KHZ
  ,DAP_SUPPORTED_SAMPLE_RATE_MAX
} daws_dap_sample_rates;

/**
 * @brief Intelligent Equalizer Modes
 * In addition to these modes, the DAWS supports a user defined custom mode
 */
typedef enum
{
   IEQ_PROFILE_OPEN
  ,IEQ_PROFILE_RICH
  ,IEQ_PROFILE_FOCUSSED
  ,IEQ_PROFILE_CUSTOM
  ,IEQ_PROFILES_MAX
} daws_dap_ieq_profiles;

/**
 * @brief Profiles in Tuning Tool
 * Allows for configuration of DAP parameters for Content and Device Processing
 */
struct daws_dap_profiles
{
    struct daws_profile_processing cp;
};


/**
 * @brief The various DAP constants used in the DAWS reference code
 */
struct daws_dap_constants
{
  int _dap_freq_bands[DAP_SUPPORTED_SAMPLE_RATE_MAX][DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
  int _ieq_profile[IEQ_PROFILES_MAX][DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
  int _negative_max[DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
  int _empty_freq_bands[DAP_CPDP_PROCESS_OPTIMIZER_NB_BANDS_MAX];
};

/**
 * @brief XML Version used in the Tuning Tool for "Dolby Audio for Wireless Speakers"
 */
struct xml_version {
  int major;
  int minor;
  int update;
};

/**
 * @brief The Dolby Tuning Tool Version
 */
struct dtt_version {
  int major;
  int minor;
  int update;
};

/**
 * @brief This is the DAP configuration structure that holds the DAP values and parameters
 */
typedef struct daws_dap_config_t
{
   struct xml_version _xml_version;
   struct dtt_version _dtt_version;
   char speaker_name[MAX_NAME_SIZE];
   char speaker_brand[MAX_BRAND_SIZE];
   struct daws_dap_constants constants;
   struct daws_dap_profiles profiles[MAX_DAP_PRESET_PROFILES];
   daws_dap_speaker_port port;
   daws_preset_mode use_profile;
   daws_preset_mode preset_idx;
   char preset_str[MAX_DAP_PRESET_PROFILES][MAX_NAME_SIZE];

   daws_main_spkr_output spkr_out_mode;
   int height_channels;
   int enable_sub;
   int daws_bass_management;
   daws_side_spkr_output side_spkr_enabled;

   daws_spkr_config spkr_config;
   const int * downmix;

   unsigned input_format;

   unsigned int is_profile_valid[MAX_DAP_PRESET_PROFILES];

} daws_dap_config;

int32_t get_sample_rate_idx(int32_t sample_rate);

void
daws_get_speaker_config
    ( daws_main_spkr_output speaker/**< [in] The output mode of the Speaker configure */
     ,daws_side_spkr_output side   /**< [in] Status of the Side Speakers */
     ,int enable_sub               /**< [in] Status of the Sub component */
     ,int height                   /**< [in] Status of the Height Channels */
     ,daws_spkr_config *config    /**< [out] Speaker configuration */
     ,const int **downmix
    );

#ifdef __cplusplus
}
#endif

#endif //DAWS_UTIL_H
