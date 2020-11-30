/*!
  @file
  quec_port_bridge.h

  @brief
	port-bridge Service
*/

/*===========================================================================
  Copyright (c) 2019-2-12
  ---Geoff.liu--- Quectel......
===========================================================================*/
#define NA    1                 /*  Name field found      */
#define EQ    2                 /*  <=> found             */
#define QU    4                 /*  <?> found             */
#define AR    8                 /*  Argument field found  */
#define AT_DATE_LEN 64
#define MAX_CMD_NAME 	20
#define ARRAY(array) 	sizeof(array)/sizeof(array[0])

#define LOGR(errcode, ...) __android_log_print(ANDROID_LOG_ERROR , "port-bridge", __VA_ARGS__) 
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "port-bridge", __VA_ARGS__) 
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "port-bridge", __VA_ARGS__) 
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "port-bridge", __VA_ARGS__) 
 
#define USB_FILE_PATH         ("/dev/at_usb0")
#define RESP_BUF_SIZE (380)
#define ATFWD_DATA_PROP_QUEC_VER            "ro.build.quectelversion.release"
#define ATFWD_DATA_PROP_ANDROID_VER			 "ro.build.version.release"
#define PB_PTHREAD_MUTEX_LOCK( mutex, path )             \
  do {                                                   \
   LOGI("trying to lock [0x%p] %s\n", mutex, path);       \
   if(pthread_mutex_lock(mutex) < 0 )                    \
     LOGE("mutex lock failed [0x%p] %s\n",mutex, path);  \
   LOGI("locked [0x%p] %s\n", mutex, path);              \
  }while (0);

#define PB_PTHREAD_MUTEX_UNLOCK( mutex, path )           \
  do {                                                   \
   LOGI("trying to unlock [0x%p] %s\n", mutex,path);      \
   if(pthread_mutex_unlock(mutex) < 0 )                  \
     LOGE("mutex unlock failed [0x%p] %s\n",mutex, path);\
   LOGI("unlocked [0x%p] %s\n", mutex, path);            \
  }while(0);

#define fsg_head_string "Quec-Head-FSG-1"
#define fsg_tail_string "Quec-Tail-FSG-1"
#define fsc_head_string "Quec-Head-FSC-1"
#define fsc_tail_string "Quec-Tail-FSC-1"
#define DEV_DATA "/dev/block/bootdevice/by-name/fsg"
#define DEV_BACKUP "/dev/block/bootdevice/by-name/fsc"
#define PROPERTY_BT_BDADDR_PATH "/mnt/vendor/persist/quec_bt_mac"
#define WLAN_BDADDR_PATH "/mnt/vendor/persist/wlan_mac.bin"
typedef struct {
	int op;
	char *name;
	int token_num;
	char **tokens;//[][AT_DATE_LEN];
}AtgCmd;

typedef struct {
  int result;
  char *response;
} AtgCmdResponse; 

typedef struct {
  char magic_head[16];
  char modem_version[64];
  char sn[128];
  char wifi_mac[16];
  char bt_mac[16];
  char qcsn[128];
  char nv_data[128];
  char magic_tail[16];
} Quec_wifi_fsg_data; 

int fsc_check_data_valid(void);
int fsc_rmts_fresh_data(void);
int get_modem_version(void);
int quec_write_bt_mac(void);
int quec_write_wifi_mac(void);
int modem_backup(void);
int modem_sync(void);
void faker_modem_init(void);
int quec_buf_skip_spechars(char *buf);
int quec_at_ins_par(AtgCmd *at_cmd,char *buf);
int quec_at_ar_ins(char *buf,AtgCmd *at_cmd);
void quec_qnvr_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qnvw_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qprtpara_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qcsn_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qdata_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_at_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_ati_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qgmr_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_egmr_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_fct_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
//void quec_csub_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qapsub_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qapcmd_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
void quec_qdevinfo_handle( const AtgCmd *cmd ,AtgCmdResponse *response);
