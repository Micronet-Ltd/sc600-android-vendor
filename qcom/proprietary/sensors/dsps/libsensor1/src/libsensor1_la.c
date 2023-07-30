/*=============================================================================
  @file libsensor1_la.c

  @brief LA-specific implementation of the sensor1 library.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <grp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <sys/system_properties.h>
#include <limits.h>
#include <stdlib.h>

#include "sensor1.h"
#include "libsensor1_osa.h"
#include "libsensor1.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/
/* Definitions for logging */
#define LIBSENSOR1_DEBUG_PROP_NAME "debug.vendor.sns.libsensor1"

/* Retry a standard library command if the error code is EINTR */
#define  RETRY_ON_EINTR(ret,cond) \
do {                              \
  ret = (cond);                   \
} while (ret < 0 && errno == EINTR)

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define FD_ADD( fd, fdset, max )                \
  do {                                          \
    FD_SET((fd),(fdset));                       \
    (max) = MAX((max),(fd));                    \
  } while(0);

/* file path for sensors settings file */
/* If file exists and contains '0', then disable external sensor clients */
#define SENSORS_SETTINGS_FILE "sensors_settings"

/*============================================================================
  Static Variable Definitions
  ============================================================================*/

/* Notifier for changes to the sensor_settings file */
static int inotify_fd;

static int settings_fd = -1;

/* Whether access to sensor1 has been enabled */
static bool sensor1_enabled;
/* Mutex is held permanently whenever sensor1_enabled is false */
static OS_EVENT *sensor1_enabled_mutex;

static char sns_settings_file_name[255] = "";

/*============================================================================
  Static Function Definitions
  ============================================================================*/

/**
 * Checks the settings file to see if sensors are enabled.
 */
static void
check_sensors_enabled( void )
{
  char enabled;
  uint8_t err;
  char sns_settings_dir[255] = "";

 if(SNS_SUCCESS == sensor1_has_acessto_directory(SENSORS_SETTINGS_FILE_NAME)){
    strlcpy(sns_settings_file_name, SENSORS_SETTINGS_FILE_NAME, strlen(SENSORS_SETTINGS_FILE_NAME) + 1);
 }
 else{
    strlcpy(sns_settings_file_name, SENSORS_SETTINGS_FILE_NAME_LEGACY, strlen(SENSORS_SETTINGS_FILE_NAME_LEGACY) + 1);
 }

  if( -1 == settings_fd ) {
    settings_fd = open( sns_settings_file_name, O_RDONLY );
  }
  if( -1 != settings_fd ) {
    /* File exists. Set up inotify to be notified of file changes */
    inotify_add_watch( inotify_fd,
                       sns_settings_file_name,
                       IN_CLOSE_WRITE );

    /* Read value from settings file, and enable/disable as needed */
    lseek( settings_fd, 0, SEEK_SET );
    if( 1 == read( settings_fd, &enabled, 1 ) ) {
      if( enabled == '1' ) {
        LOG_ERROR("%s: Sensors enabled = true", __func__);
        sensor1_enabled = true;
        sns_os_mutex_post( sensor1_enabled_mutex );
        return;
      }
    } else {
      close( settings_fd );
      settings_fd = -1;
    }
  } else {
    /* File does not exist */
    LOG_ERROR("%s: open error: settings file \"%s\", errno %d", __func__,
        sns_settings_file_name, errno);
    strlcpy(sns_settings_dir, sns_settings_file_name, strlen(sns_settings_file_name) - strlen(SENSORS_SETTINGS_FILE) - 1);
    if( -1 == inotify_add_watch( inotify_fd, sns_settings_dir, IN_CREATE ) ) {
      /* Error creating the watch */
      LOG_ERROR("%s: inotify error: settings path \"%s\", errno %d", __func__,
          sns_settings_dir, errno);
    }
  }
  LOG_ERROR("%s: Sensors enabled = false", __func__);
  sensor1_enabled = false;
  sns_os_mutex_pend( sensor1_enabled_mutex, 0, &err );
  sensor1_close_clients();
}

static void
inotify_thread( void *unused )
{
  UNREFERENCED_PARAMETER(unused);

  LOG_DEBUG("%s: Entered inotify thread", __func__);

  for( ;; ) {
    int sel_err;
    int max_fd = 0;
    fd_set read_fd;
    fd_set error_fd;

    FD_ZERO( &read_fd );
    FD_ZERO( &error_fd );
    FD_ADD( inotify_fd, &read_fd, max_fd );

    error_fd = read_fd;
    max_fd++;

    errno = 0;
    RETRY_ON_EINTR( sel_err, select( max_fd, &read_fd, (fd_set*)NULL, &error_fd,
                                     (struct timeval*) NULL) );
    if( sel_err < 0 ) {
      LOG_ERROR("%s: Error on select %i", __func__, sel_err);
    } else if( FD_ISSET( inotify_fd, &read_fd ) ) {
      /* Change in settings file */
      char buf[500];
      struct inotify_event *evt = (struct inotify_event*)buf;
      read(inotify_fd, evt, 500);
      if(evt->mask & IN_IGNORED) {
        /* Previous watch was removed. Nothing to do here */
      } else if( evt->len == 0 ||
                 ( (evt->mask & IN_CREATE) &&
                   (0 == strncmp( evt->name, SENSORS_SETTINGS_FILE, evt->len)))) {
        inotify_rm_watch( inotify_fd, evt->wd );
        check_sensors_enabled();
      }
    }
  }

  LOG_DEBUG("%s: Exiting inotify thread", __func__);
}

#define ALS_2S_STATE "/proc/als_2s"
static int notify_fd;
static char prv_als_s[8];
int als_changed = -1;

int als_2s_changed(void)
{
    return als_changed;
}

void als_2s_clr_change(void)
{
    als_changed = 0;
    return;
}

static void inotify_als_2s_thread (void *unused)
{
    UNREFERENCED_PARAMETER(unused);
    int rd;
    char als_s[8];

    LOG_WARN("%s: ALS-2S entry\n", __func__);

//    notify_fd = inotify_init();
//    if (-1 == notify_fd) {
//        LOG_ERROR( "%s: inotify_init error %d", __func__, errno );
//        return;
//    }

//    inotify_add_watch(notify_fd, ALS_2S_STATE, IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE);
    notify_fd = open(ALS_2S_STATE, O_RDONLY);
    for( ;; ) {
//        int sel_err;
//        int max_fd = 0;
//        fd_set read_fd;
//        fd_set error_fd;

//        FD_ZERO(&read_fd);
//        FD_ZERO(&error_fd);
//        FD_ADD(notify_fd, &read_fd, max_fd);

//        error_fd = read_fd;
//        max_fd++;

//        errno = 0;
//        LOG_WARN("%s: wait for ALS-2S changed\n", __func__);
//        RETRY_ON_EINTR (sel_err, select(max_fd, &read_fd, (fd_set*)NULL, &error_fd, (struct timeval*) NULL));

//        if (sel_err < 0) {
//            LOG_ERROR("%s: Error on select %i", __func__, sel_err);
//        } else if (FD_ISSET(notify_fd, &read_fd)) {
            // Change in settings file
//            char buf[512];
//            struct inotify_event *evt = (struct inotify_event*)buf;
//            read(notify_fd, evt, sizeof(buf));
//            if (evt->mask & IN_IGNORED) {
//                // Previous watch was removed. Nothing to do here
//            } else if (evt->len == 0 || ((evt->mask & (IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE)) &&
//                       (0 == strncmp(evt->name, ALS_2S_STATE, evt->len)))) {
//                inotify_rm_watch(notify_fd, evt->wd);
//                LOG_WARN("%s: ALS-2S changed\n", __func__);
//                inotify_add_watch(notify_fd, ALS_2S_STATE, IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE);
//            }
//        }
        if (notify_fd < 0) {
            notify_fd = open(ALS_2S_STATE, O_RDONLY); 
        }
        if (notify_fd >= 0) {
            rd = read(notify_fd, als_s, 4);

            if (rd > 0) {
                if (als_s[0] != prv_als_s[0]) {
                    memcpy(prv_als_s, als_s, 5);
                    if ('h' == prv_als_s[0]) {
                        als_changed = 2;
                        als_s[4] = 0;
                    } else {
                        als_changed = 1;
                        als_s[3] = 0;
                    }
                    //LOG_WARN("%s: ALS-2S changed %s\n", __func__, prv_als_s);
                }
            }
            close(notify_fd);
            notify_fd = -1;
        }

        sleep(1);
    }

    LOG_WARN("%s: ALS-2S finished\n", __func__);
}

/*============================================================================
  Public Function Definitions
  ============================================================================*/

bool
sensor1_enabled_check( void )
{
  return sensor1_enabled;
}

void
sensor1_enabled_wait( void )
{
  uint8_t err;

  sns_os_mutex_pend( sensor1_enabled_mutex, 0, &err );
  sns_os_mutex_post( sensor1_enabled_mutex );
}

void
sensor1_osa_init_once( void )
{
  uint8_t err;
  int debug_prop_len;
  char debug_prop[ PROP_VALUE_MAX ];

  debug_prop_len = __system_property_get( LIBSENSOR1_DEBUG_PROP_NAME, debug_prop );
  if( 1 == debug_prop_len ) {
    switch( debug_prop[0] ) {
      case '0':
        sensor1_log_level = LOG_LEVEL_DISABLED;
        break;
      case '1':
        sensor1_log_level = LOG_LEVEL_ALL;
        break;
      case 'v':
      case 'V':
        sensor1_log_level = LOG_LEVEL_VERBOSE;
        break;
      case 'd':
      case 'D':
        sensor1_log_level = LOG_LEVEL_DEBUG;
        break;
      case 'i':
      case 'I':
        sensor1_log_level = LOG_LEVEL_INFO;
        break;
      case 'w':
      case 'W':
        sensor1_log_level = LOG_LEVEL_WARN;
        break;
      case 'e':
      case 'E':
        sensor1_log_level = LOG_LEVEL_ERROR;
        break;
      default:
        break;
    }
    LOG_ERROR("%s: Setting log level to %d", __func__, sensor1_log_level);
  } else if( debug_prop_len > 1 ) {
    LOG_ERROR("%s: Invalid value for %s: %s. Enabling all logs", __func__,
              LIBSENSOR1_DEBUG_PROP_NAME, debug_prop );
    sensor1_log_level = LOG_LEVEL_ALL;
  }

  sensor1_enabled = true;
  sensor1_enabled_mutex = sns_os_mutex_create( 0, &err );
  if( 0 != err ) {
    LOG_ERROR("%s: Cannot create mutex %i", __func__, err);
    SNS_ASSERT(false);
  }

  inotify_fd = inotify_init();
  if( -1 == inotify_fd ) {
    LOG_ERROR( "%s: inotify_init error %d", __func__, errno );
    SNS_ASSERT(false);
  }

  check_sensors_enabled();
  err = sns_os_task_create( inotify_thread, NULL, NULL, 0 );
  if( 0 != err ) {
    LOG_ERROR("%s: Error spawning WOULDBLOCK thread %i", __func__, err);
  }

  err = sns_os_task_create(inotify_als_2s_thread, 0, 0, 0);
  if( 0 != err ) {
    LOG_ERROR("%s: can't create ALS-2S task %i\n", __func__, err);
  }
}
