/**
  @file
  faker_modem.c

  @brief
  Faker modem to process relevant data,such as wifi/bt mac,sn qcsn,modem version 
*/

/*===========================================================================
  Copyright (c) 2019-2-12
  ---Geoff.liu--- Quectel......
===========================================================================*/
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <time.h>
#include <android/log.h>
#ifdef FEATURE_PB_LINUX_ANDROID
  #include <cutils/properties.h>
#endif
#include <sys/types.h> 
#include <sys/stat.h>

#include "ds_util.h"
#include "quec_port_bridge.h"

#define TURE 1
#define FALSE 0



Quec_wifi_fsg_data  *qwifi_data;
int change_bt_format(char *buf)
{
	if(strlen(buf)!=12)
		return 0;
	int count=0;
	char addr[12]={0};
	if((buf[0]=='"')&&(buf[12]=='"'))
	{
		for(count=0;count<6;count++)
		{
			addr[10-2*count]=buf[2*count+1];
			addr[11-2*count]=buf[2*count+2];
		}
		memset(buf,0,14);
		memcpy(buf,addr,12);
		buf[12]='\0';
		return 1;
	}
	return 0;
}
int check_mac_vaild(char *buf)
{
	int count=0;
	for(count=0;count<strlen(buf);count++)
	{
		if(buf[count]!='0')
			return 1;
	}
	LOGE("check_mac_invaild\n");
	return 0;
}
int quec_write_bt_mac(void)
{	
	FILE *fd ;
	char dest[18]={0};
	int count =0;
	LOGE("%s: Trying %s ", __func__ ,qwifi_data->bt_mac);
	if((strlen(qwifi_data->bt_mac)!=12)||(check_mac_vaild(qwifi_data->bt_mac)==0))
	{
		LOGE("%s: mac  error  ", __func__ );
		return FALSE;
	}
	fd=fopen(PROPERTY_BT_BDADDR_PATH, "w");
	for(count=0;count<6;count++)
	{
		dest[15-3*count]=qwifi_data->bt_mac[2*count+0];
		dest[16-3*count]=qwifi_data->bt_mac[2*count+1];
		dest[17-3*count]=':';
	}
	dest[17]='\0';
	LOGW("write bT  mac ---%s",dest);
	fprintf(fd, "%s",dest);
    fclose(fd);
    chmod(PROPERTY_BT_BDADDR_PATH, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	return TURE;
}

int quec_write_wifi_mac(void)
{	
    FILE *fd ;
	LOGE("%s: Trying %s ", __func__ ,qwifi_data->wifi_mac);
	if((strlen(qwifi_data->wifi_mac)!=12)||(check_mac_vaild(qwifi_data->wifi_mac)==0))
		{
		LOGE("%s: mac error  ", __func__ );
		return FALSE;
		}
	if(!( fd=fopen(WLAN_BDADDR_PATH, "w"))){
		LOGE("%s: fopen fail  ", __func__ );
		return FALSE;
	}
	fprintf(fd, "Intf0MacAddress=%s\n",qwifi_data->wifi_mac);
	fprintf(fd, "%s\n","END");
    LOGW("write wifi mac--- %s\n",qwifi_data->wifi_mac);
    fclose(fd);
	return TURE;
}

void check_mac_exit(void)
{
	if(access(WLAN_BDADDR_PATH,0)!=0)
		quec_write_wifi_mac();
	if(access(PROPERTY_BT_BDADDR_PATH,0)!=0)
		quec_write_bt_mac();
}
char* get_cmd_result(char* cmd)
{
    FILE *fp;
    char result[256];
    memset (result, 0,sizeof(result));
    if((fp=popen(cmd,"r"))==NULL)
    {
        LOGE("the command %s not exist\n",cmd);
        pclose(fp);
        return 0;
    }
    while(fgets(result,sizeof(result)-1,fp)!=NULL)
    {
        LOGW("cmd_result: %s\n", result);
    }
    pclose(fp);
    return result;
}
boolean wifi_modem_is_ture =FALSE;
int get_modem_version(void)
{
	char *buf;
	int count=0;
	char mp_version[64];
	char baseband[64];
	char *bp_version=NULL;
	if(system("echo 11 >/sys/devices/soc0/select_image")!=0)
		LOGE("Check_modem_version_echo error\n");
	buf=get_cmd_result("cat /sys/devices/soc0/image_crm_version");
	LOGW("modem_vension_buf:%s\n",buf);
	if(strncmp(buf,":SC66W_",7)!=0)
		return FALSE;
	memset(qwifi_data->modem_version,0, sizeof(qwifi_data->modem_version));
	memcpy(qwifi_data->modem_version,buf+7,sizeof(qwifi_data->modem_version));
	LOGW("modem_vension:%s\n",qwifi_data->modem_version);
	quec_buf_skip_spechars(qwifi_data->modem_version);
	memcpy(mp_version,qwifi_data->modem_version,sizeof(qwifi_data->modem_version));
	for(count=0;count<64;count++){
		 if(mp_version[count]=='_'){
			 mp_version[count]='\0';
			break;
		 }
	 }
	 if((bp_version=strchr(qwifi_data->modem_version, 'B'))!=NULL)
	 {
		snprintf(baseband, 64, "%s(%s)",bp_version,mp_version);//flash
		LOGE("baseband:[%s]",baseband);
		property_set("persist.version.baseband",baseband);
	 }

	property_set("persist.vendor.version.ati","SC66W");
	wifi_modem_is_ture =TURE;
	return TURE;
}

static int quec_write(char *dev,Quec_wifi_fsg_data *data)
{	
	int ret,count,fd;
	fd = open(dev, O_RDWR);
	if(fd < 0)
		return -1;	
	count = write(fd, (char *)data, sizeof(Quec_wifi_fsg_data));
	close(fd);
	if(count==sizeof(Quec_wifi_fsg_data))
	return TURE;
	else 
	return FALSE;
}
static int quec_read(char *dev,Quec_wifi_fsg_data *data)
{	
	int ret,count,fd;
	memset( (char *)qwifi_data,0, sizeof(Quec_wifi_fsg_data));
	fd = open(dev, O_RDWR);
	if(fd < 0)
		return -1;	
	count = read(fd, (char *)data, sizeof(Quec_wifi_fsg_data));
	close(fd);
	if(count==sizeof(Quec_wifi_fsg_data))
	return TURE;
	else 
	return FALSE;
}
int modem_sync(void)
{	
	memcpy(qwifi_data->magic_head,fsg_head_string,sizeof(qwifi_data->magic_head));
	memcpy(qwifi_data->magic_tail,fsg_tail_string,sizeof(qwifi_data->magic_tail));
	LOGW("faker_modem_sync!!!!!");
	return quec_write(DEV_DATA,qwifi_data);
}
 int modem_backup(void)
{

	memcpy(qwifi_data->magic_head,fsc_head_string,sizeof(qwifi_data->magic_head));
	memcpy(qwifi_data->magic_tail,fsc_tail_string,sizeof(qwifi_data->magic_tail));
	LOGW("faker_modem_backup!!!!!");
	return quec_write(DEV_BACKUP,qwifi_data);
}
int fsg_rmts_fresh_data(void)
{
	Quec_wifi_fsg_data *buf=malloc(sizeof(Quec_wifi_fsg_data));
	memset(buf,0,sizeof(Quec_wifi_fsg_data));
	quec_read(DEV_DATA,buf);
	if((strncmp(buf->magic_head,fsg_head_string,16)!=0)||(strncmp(buf->magic_tail,fsg_tail_string,16)!=0))
		return FALSE;
	memcpy(qwifi_data,buf,sizeof(Quec_wifi_fsg_data));
	free(buf);
	LOGW("Quec_fsg_rmts_fresh_data!!");
	return TURE;
}
int fsc_rmts_fresh_data(void)
{
	Quec_wifi_fsg_data *buf=malloc(sizeof(Quec_wifi_fsg_data));
	memset(buf,0,sizeof(Quec_wifi_fsg_data));
	quec_read(DEV_BACKUP,buf);
	if((strncmp(buf->magic_head,fsc_head_string,16)!=0)||(strncmp(buf->magic_tail,fsc_tail_string,16)!=0))
		return FALSE;
	memcpy(qwifi_data,buf,sizeof(Quec_wifi_fsg_data));
	free(buf);
	LOGW("Quec_fsc_rmts_fresh_data!!");
	return TURE;
}
int fsg_check_data_valid(void)
{	
	int ret=0;
	Quec_wifi_fsg_data *buf=malloc(sizeof(Quec_wifi_fsg_data));
	memset(buf,0,sizeof(Quec_wifi_fsg_data));
	quec_read(DEV_DATA,buf);
	if((strncmp(buf->magic_head,fsg_head_string,16)==0)&&(strncmp(buf->magic_tail,fsg_tail_string,16)==0))
		ret=TURE;
	else
		LOGE("Quec_fsg_data_invalid!!");
	free(buf);
	return ret;
}

int fsc_check_data_valid(void)
{	
	int ret=0;
	Quec_wifi_fsg_data *buf=malloc(sizeof(Quec_wifi_fsg_data));
	memset(buf,0,sizeof(Quec_wifi_fsg_data));
	quec_read(DEV_BACKUP,buf);
	if((strncmp(buf->magic_head,fsc_head_string,16)==0)&&(strncmp(buf->magic_tail,fsc_tail_string,16)==0))
		ret=TURE;
	else
		LOGE("Quec_fsg_data_invalid!!");
	free(buf);
	return ret;
}

void faker_modem_init(void)
{ 	
	//int ret=0;
	if(qwifi_data==NULL)
	qwifi_data=malloc(sizeof(Quec_wifi_fsg_data));
	memset( (char *)qwifi_data,0, sizeof(Quec_wifi_fsg_data));
	if(get_modem_version()==FALSE)
		return;
	if(fsg_rmts_fresh_data()==FALSE)
	{
		if(fsc_rmts_fresh_data()==FALSE){
			LOGW("faker_modem_creat new zore data!!!!!\n");
			memset( (char *)qwifi_data,0, sizeof(Quec_wifi_fsg_data));
			strncpy(qwifi_data->sn,"11111",6);//delet ""
			strncpy(qwifi_data->wifi_mac,"000000000000",12);//delet ""
			strncpy(qwifi_data->bt_mac,"000000000000",12);//delet ""
		}
	}
	get_modem_version();
	modem_sync();
	check_mac_exit();
	
}
