/*!
  @file
  sendcmd.cpp

  @brief
  Places a Remote Procedure Call (RPC) to Android's AtCmdFwd Service

*/

/*===========================================================================

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_TAG "Atfwd_sendcmd"

#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>
#include "common_log.h"
#include <vendor/qti/hardware/radio/atcmdfwd/1.0/IAtCmdFwd.h>
#include <vendor/qti/hardware/radio/atcmdfwd/1.0/types.h>
#include "AtCmdFwd.h"
#include <hwbinder/ProcessState.h>


#include <cutils/properties.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>





//set hwbinder buffer size to 128KB
#define HW_BINDER_MMAP_SIZE 131072

namespace android {

using vendor::qti::hardware::radio::atcmdfwd::V1_0::IAtCmdFwd;

extern "C" void quec_qdevinfo_handle(const AtCmd *cmd, AtCmdResponse *response);
#ifdef QUECTEL_QAPCMD_CMD
extern "C" void quec_qapcmd_handle(const AtCmd *cmd, AtCmdResponse *response);
#endif

extern "C" void initWithMapSize() {
    android::hardware::ProcessState::initWithMmapSize((size_t)(HW_BINDER_MMAP_SIZE));
    return;
}


#if 0
static AtCmdResponse mAtResp;

static void handleResp(const
        ::vendor::qti::hardware::radio::atcmdfwd::V1_0::AtCmdResponse& resp) {
    const char *resp_str = resp.response.c_str();

    LOGI("handlResp AtCmdFwd Result %d", resp.result);
    mAtResp.result = (int) resp.result;
    LOGI("handlResp AtCmdFwd resp string %s", resp_str);

    if (resp.response == "") {
        mAtResp.response = nullptr;
        return;
    }

    int len = resp.response.size() + 1;
    mAtResp.response = (char *)malloc(sizeof(char) * len);
    if (mAtResp.response == NULL) {
        LOGE("malloc error!");
        return;
    }

    strlcpy(mAtResp.response, resp_str, len);

    return;
}

static bool sendAtCmd(const AtCmd *cmd, sp<IAtCmdFwd> atCmdFwd) {
    if (cmd == NULL) {
        LOGE("null cmd\n");
        return false;
    }

    vendor::qti::hardware::radio::atcmdfwd::V1_0::AtCmd atcmd;

    atcmd.opCode = cmd->opcode;

    if (cmd->name == NULL) {
        LOGE("null cmd name\n");
        return false;
    }

    atcmd.name = cmd->name;
    LOGI("AtCmdFwd cmd name %s", cmd->name);
    atcmd.token.num = cmd->ntokens;

    LOGI("AtCmdFwd token num %d", atcmd.token.num);
    for (unsigned int i = 0; i < atcmd.token.num; i++) {
        atcmd.token.items[i] = cmd->tokens[i];
        LOGI("token %s", atcmd.token.items[i].c_str());
    }

    atCmdFwd->processAtCmd(atcmd, handleResp);

    return true;
}
#endif 
/*
*
* add by Ben, 2017.05.24
* return the FCT test results for AT Command(AT+QFCT?)
* 
*/
#define QUECTEL_QGMR_CMD
#ifdef QUECTEL_QGMR_CMD
#define ATFWD_DATA_PROP_QUEC_VER            "ro.build.quectelversion.release"
#define ATFWD_DATA_PROP_ANDROID_VER			 "ro.build.version.release"
#define QGMR_RESP_BUF_SIZE (380*2) // RESP_BUF_SIZE msut less than the QMI_ATCOP_AT_RESP_MAX_LEN in the file vendor/qcom/proprietary/qmi/inc/qmi_atcop_srvc.h,
               // Maybe you should change QMI_ATCOP_AT_RESP_MAX_LEN for your requirement

extern "C" void quec_qgmr_handle( const AtCmd *cmd ,AtCmdResponse *response)
{
	int ret=0;
	char target[128] = {0}; // Stores target info      
	char *resp_buf=NULL;
	char *ptr=NULL;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(QGMR_RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, QGMR_RESP_BUF_SIZE);
     }

	ret = property_get(ATFWD_DATA_PROP_ANDROID_VER, target, "");
	LOGI("qgmr_modem:%s",cmd->tokens[0]);
	LOGI("qgmr_andorid:%s",target);

	if (cmd->ntokens >= 3) {
		//LOGE("cmd->tokens[0]:%s",cmd->tokens[0]);
		//LOGE("cmd->tokens[1]:%s",cmd->tokens[1]);
		//LOGE("cmd->tokens[2]:%s",cmd->tokens[2]);
		sprintf(resp_buf,"Quectel\n%s\nRevision: ", cmd->tokens[2]);
	} else {
		ret = 60;
		ptr = strstr(cmd->tokens[0], "SC");
		if (ptr) {
			sscanf(ptr + 2, "%d%*s", &ret);
		}

		if (strstr(cmd->tokens[0], "SC600Y") != NULL)
			sprintf(resp_buf,"Quectel\nSC600Y\nRevision: ");
		else if (strstr(cmd->tokens[0], "SC600T") != NULL)
			sprintf(resp_buf,"Quectel\nSC600T\nRevision: ");
		else if (strstr(cmd->tokens[0], "SC650T") != NULL)
			sprintf(resp_buf,"Quectel\nSC650T\nRevision: ");
		else if (strstr(cmd->tokens[0], "SC650Y") != NULL)
			sprintf(resp_buf,"Quectel\nSC650Y\nRevision: ");
		else
			sprintf(resp_buf,"Quectel\nSC%d\nRevision: ", ret);
	}

	if(cmd->ntokens >= 2)
		sprintf(resp_buf,"%s%s%s_Android%s.0.0",resp_buf, cmd->tokens[0],cmd->tokens[1],target);
	else
		sprintf(resp_buf,"%s%s_Android%s.0.0",resp_buf, cmd->tokens[0],target);

	memset(target,0,64);
	ret = property_get(ATFWD_DATA_PROP_QUEC_VER, target, "");
	LOGI("qgmr_quectel:%s",target);
	// remove V01
	if((ptr=strchr(target,'V'))!=NULL)
	{
		*ptr='\0';  // 'V' -> '\0'
	}
	sprintf(resp_buf,"%s.%s",resp_buf,target);
	if((response->response = resp_buf) == NULL )
	 {
		response->result = 0; // error
	    LOGE("QGMR_AT error");
	 }
     response->result = 1;
	 
	 return;
}
#endif
#if defined QUECTEL_AT_QAPSUB_FEATURE
#define QUECTEL_ANDROID_VERSION            "ro.build.quectelversion.release"
#define Q_RESP_BUF_SIZE 390


extern "C" void quec_qapsub_handle(AtCmdResponse *response)
{
	int ret = 0;
	char target[PROPERTY_VALUE_MAX] = {0};
	char *resp_buf=NULL;
	char *ptr=NULL;
	//char apsub[8] = {0};
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(Q_RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory.\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, Q_RESP_BUF_SIZE);
     }
	ret = property_get(QUECTEL_ANDROID_VERSION, target, ""); //sample: 01.001V01
	LOGI("andorid version:%s",target);
	if(ret<=0)
	{
		free(resp_buf);
		LOGE("%s:%d Get property error.\n", __func__, __LINE__);
		return; // get property of android version error.
	}
	// get sub version from android version, 01.001V01 -> V01
	if((ptr=strchr(target,'V'))==NULL)
	{
		free(resp_buf);
		LOGE("%s:%d Get apsubversion error.\n", __func__, __LINE__);
		return; // get property of android version error.
	}
	//strncpy(resp_buf, ptr, RESP_BUF_SIZE);
	snprintf(resp_buf, Q_RESP_BUF_SIZE, "APSubEdition: %s",ptr);
	response->response = resp_buf;
	LOGI("resp buf:%s",resp_buf);
	response->result = 1;
	return;
}
#endif // QUECTEL_AT_QAPSUB_FEATURE
#define QUECTEL_FCT_TEST  
#ifdef QUECTEL_FCT_TEST

#define ARRARY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define RESP_BUF_SIZE (380) // RESP_BUF_SIZE msut less than the QMI_ATCOP_AT_RESP_MAX_LEN in the file vendor/qcom/proprietary/qmi/inc/qmi_atcop_srvc.h,
			  // Maybe you should change QMI_ATCOP_AT_RESP_MAX_LEN for your requirement

//#define QUECTEL_FCT_TEST_DEBUG // for print debug log info
#ifdef QUECTEL_FCT_TEST_DEBUG
#define DGprintf(fmt,args...) printf(fmt,##args) 
#else
#define DGprintf(fmt, args...) 
#endif 
typedef struct{
	const char *name; // item name
	int result; // -1: fail  0: none  1: success
}fct_item_type;
fct_item_type fct_items_all[]=
//fct_item_type fct_items[]=
{
	{"SPEAKER LOOP", 0},
	{"SECOND LCD", 0},
	{"FLASHLIGHT", 0},
	{"KEY", 0},
	{"HANDSET", 0},
    {"VIBRATOR", 0},
	{"CAMERA_BACK_MAIN", 0},
    {"HEADSET LOOP", 0},
	{"CAMERA_BACK_AUX", 0},
	{"CAMERA_FRONT", 0},
    {"LSENSOR", 0},
	{"NOISE MIC", 0},
	{"SDCARD", 0},
    {"STORAGE", 0},
	{"SIMCARD1",0},
	{"SIMCARD2", 0},
    {"WIFI", 0},
    {"BLUETOOTH", 0},
    {"GPS", 0},
};
fct_item_type fct_items_auto_all[]=
//fct_item_type fct_items[]=
{
        {"SECOND LCD",0},
        {"L HEADSET",0},
	{"R HEADSET", 0},
	{"LOUDSPEAKER", 0},
	{"CAMERA BACK", 0},
	{"CAMERA AUX", 0},
	{"CAMERA FRONT", 0},
	{"HANDSET",0},
	{"SDCARD", 0},
        {"EMMC",0},
		{"VIBRATOR", 0},
	{"SIMCARD1", 0},
	{"SIMCARD2", 0},

};

extern "C" char* set_response_buf(fct_item_type *fct_items, int num)
{
	int i,offset=0;
	int test_num=0, success_num=0,fail_num=0,null_num=0;
	char *resp_buf=NULL;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
			LOGI("%s:%d No Memory\n", __func__, __LINE__);
			return resp_buf; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
	}
	for(i=0; i<num; i++)
	{
		if(fct_items[i].result !=0 )
			test_num ++;
		if(fct_items[i].result == 1)
			success_num ++;
		if(fct_items[i].result == 0)
			null_num ++;
		if(fct_items[i].result == -1)
			fail_num ++;
	}
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QFCT: %d,%d,%d",(success_num==num)?1:0, num, test_num );
	if((success_num==num) || (test_num == 0))
	{
		return resp_buf; // all fct items pass or not test
	}
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\r\n");
	
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "Success_num = %d,Fail_num = %d,Null_num = %d",success_num, fail_num, null_num );
	
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\r\n");
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "%d:%s,",num-1,fct_items[num-1].name);
	switch(fct_items[num-1].result)
	{
		case -1:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "fail");
			break;
		case 0:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "null");
			break;
			
		case 1:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "pass");
			break;
	}

	/*
	for(i=0; i<num; i++)
	{
		if(offset+32>RESP_BUF_SIZE)
		{
			LOGI("There is no space to store results. offset=%d RESP_BUF_SIZE=%d\r\n", offset, RESP_BUF_SIZE);
			break;
		}
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "%d:%s,", i,fct_items[i].name);
		LOGI("[%s] %d\n", fct_items[i].name, fct_items[i].result);
		switch(fct_items[i].result)
		{
			case -1:
				offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "fail");
				break;
			case 0:
				offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "null");
				break;
				
			case 1:
				offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "pass");
				break;
		}
		if(i<num-1)
		{
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\r\n");
		}
		
	}*/
	//LOGI("%s:%d: RESP_BUF_SIZE:%d offset:%d \n", __FILE__, __LINE__,RESP_BUF_SIZE, offset);
	//printf("%s:%d: RESP_BUF_SIZE:%d offset:%d \n", __FILE__, __LINE__,RESP_BUF_SIZE, offset);
#if  1// for debug
//	DGprintf("<<<<<<<<<<<< respbuf >>>>>>>>>>>>>>\n");
//	LOGI("%s", resp_buf);
//	DGprintf("<<<<<<<<<<<< respbuf end >>>>>>>>>>>>>>\n");
#endif

	return resp_buf;
}
extern "C" char* get_string_from_two_char(char *src, char *dest, int size, char start, char end)
{
	char *p=NULL;
	char *q=NULL;
	int i=0;
	if(NULL==src || NULL==dest)
		return NULL;
	memset(dest, 0, size);
	if((p=strchr(src,start)) && (q=strchr(src,end)))
	{
		p++; // skip start char, from next char
		for(i=0;i<size&&p!=q;i++)
		{
			*dest++=*p;
			p++;
			
		}
	}
	else
	{
		return NULL;
	}
	return dest;
	
}

int read_file(const char *filepath, char *buf, int size){
    int fd, len;

    fd = open(filepath, O_RDONLY);
    if(fd == -1){
        printf("[%s]:file(%s) open fail, error=%s\n", __FUNCTION__, filepath, strerror(errno));
        return -1;
    }

    len = read(fd, buf, size - 1);
    if(len > 0){
        if(buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else
            buf[len] = '\0';
    }

    close(fd);
    return 0;
}

extern "C" void quec_qfct_handle(AtCmdResponse *response)
{
//	#define FCT_RESULT_FILE	 "/mnt/vendor/persist/FTM_AP/mmi.res"
	char FCT_RESULT_FILE[64] = {0};
	int total_items;
	char line_text[64] = {0};
	FILE *fp = NULL;
	char mmi_res_name[32] = {0};
	int  i,check_num=0,offset=-1;
	int  pmi_enable = 1;
	int  sim_type=-1;
	int fct_arrary_type = 0;
  int items_length = 0;//ARRARY_SIZE(fct_items_all);
  
	LOGE("quec_qfct_handle start\n");
	if((fp=fopen("/mnt/vendor/persist/FTM_AP/mmi-check","r"))!=NULL){
			while(fgets(line_text,64,fp)!=NULL){
					if(strstr(line_text, "mmi-auto.res")){
							//char* FCT_RESULT_FILE	= "/persist/FTM_AP/mmi-auto.res"
							snprintf(FCT_RESULT_FILE,64,"/mnt/vendor/persist/FTM_AP/mmi-auto.res");
							fct_arrary_type = 1;
					}else if(strstr(line_text, "mmi.res")){
							//char* FCT_RESULT_FILE	="/persist/FTM_AP/mmi.res"
							snprintf(FCT_RESULT_FILE,64,"/mnt/vendor/persist/FTM_AP/mmi.res");
							fct_arrary_type = 0;
					}
			}
	}else{
			snprintf(FCT_RESULT_FILE,64,"/mnt/vendor/persist/FTM_AP/mmi.res");
			//char* FCT_RESULT_FILE="/persist/FTM_AP/mmi.res"
	}
	
	
	if(fct_arrary_type == 0){
			items_length = ARRARY_SIZE(fct_items_all);      
	}else{
			items_length = ARRARY_SIZE(fct_items_auto_all);   
	}
	

  total_items = items_length;
  fct_item_type fct_items_check[total_items]; 
  fct_item_type fct_items[total_items];        

	if(fct_arrary_type == 0){      
		  for(i=0; i<total_items; i++) {
		      fct_items[i].name = fct_items_all[i].name;
		      fct_items[i].result = 0;
		  }
	}else{
		  for(i=0; i<total_items; i++) {
		      fct_items[i].name = fct_items_auto_all[i].name;
		      fct_items[i].result = 0;
		  }		
	}

	if((fp=fopen(FCT_RESULT_FILE, "r")) == NULL) {
		LOGI("open file:%s failed!\n", FCT_RESULT_FILE);
		
		if((response->response = set_response_buf(fct_items, total_items)) == NULL ) {
			response->result = 0; // error
			LOGI("%s:%d open file %s failed!\n", __func__, __LINE__, FCT_RESULT_FILE);
		}
		response->result = 1;
		return;
	}
	// get line from file
	while(fgets(line_text, 64, fp)!=NULL)	
	{
        DGprintf("Line: %s \n",line_text);
		if(strchr(line_text, '[') && strchr(line_text, ']')) // [name]
		{
			offset = -1;
			if(get_string_from_two_char(line_text, mmi_res_name, 32, '[',']') == NULL)
			{
				printf("error!\n");
				continue;
			}
			else
			{
				LOGI("get name:%s\n", mmi_res_name);
			}
			for(i=0; i<total_items; i++)
			{
				DGprintf("fct_item.name:%s\n",fct_items[i].name);
				//if(strstr(line_text, fct_items[i].name))
				if(strcasecmp(mmi_res_name, fct_items[i].name)==0)
				{
					LOGI("%d.Match",i);
					offset = i;
					break;
				}
				else
				{
					LOGI("Not Match.\n");
				}
			}
			
		}
		else if(strstr(line_text, "Result"))  // item result
		{
			if(strcasecmp(mmi_res_name,"PMI_STATAS")==0 && strstr(line_text, "no_exist"))
			{
				pmi_enable=0;
			}
			if(strcasecmp(mmi_res_name,"SIM_TYPE")==0 && strstr(line_text, "w_version"))
			{
				sim_type=0;
			}
			if(strcasecmp(mmi_res_name,"SIM_TYPE")==0 && strstr(line_text, "ssss"))
			{
				sim_type=1;
			}
			if(strcasecmp(mmi_res_name,"SIM_TYPE")==0 && strstr(line_text, "dsds"))
			{
				sim_type=2;
			}
			
			if(offset>=0)
			{
				if(strstr(line_text, "pass"))
					fct_items[offset].result = 1;
				else if(strstr(line_text, "fail"))
					fct_items[offset].result = -1;
			}
		}
	}
	if(fp)
		fclose(fp);
	
	if(pmi_enable == 0){
				for(i=0;i<total_items;i++)
				{
					if((strcasecmp(fct_items[i].name,"FLASHLIGHT")==0)||(strcasecmp(fct_items[i].name,"VIBRATOR")==0))
						fct_items[i].result = -2 ;
				}
	}
	if(sim_type == 0)
	{
				for(i=0;i<total_items;i++)
				{
					if((strcasecmp(fct_items[i].name,"SIMCARD1")==0)|| (strcasecmp(fct_items[i].name,"SIMCARD2")==0) ||(strcasecmp(fct_items[i].name,"GPS")==0))
						fct_items[i].result = -2 ;
				}
	}
	if(sim_type == 1)
	{
				for(i=0;i<total_items;i++)
				{
					if((strcasecmp(fct_items[i].name,"SIMCARD2")==0))
						fct_items[i].result = -2 ;
				}
	}
	for (i = 0; i < total_items; i++) {
			if(fct_items[i].result !=-2){
        fct_items_check[check_num].name = fct_items[i].name;
        fct_items_check[check_num].result = fct_items[i].result;
        LOGI("fct_items[%d] =%s result=%d\n",check_num,fct_items_check[check_num].name,fct_items_check[check_num].result);
        check_num++;
      }
   }
	
	
	
	if((response->response = set_response_buf(fct_items_check, check_num)) == NULL)
	{
		response->result = 0;
		return;
	}
	response->result = 1; // success
	return;
	
}


#endif



extern "C" AtCmdResponse *sendit(const AtCmd *cmd)
{
    AtCmdResponse *result;

    if (!cmd) return NULL;
#if 1//def QUECTEL_FCT_TEST
	result = new AtCmdResponse;
	result->response = NULL;
	LOGI("ATFWD AtCmdFwd cmd = %s",cmd->name);
	if(strcasecmp(cmd->name, "+QFCT")==0)
	{
			LOGI("ATFWD AtCmdFwd QFCT");
		if(NULL != cmd->tokens) {
			LOGI("ATFWD AtCmdFwd Tokens Not NULL ntokens=%d",cmd->ntokens);
			if(cmd->ntokens == 0 || cmd->tokens[0] == NULL){
				LOGI("ATFWD AtCmdFwd Tokens[0] is NULL");
				quec_qfct_handle(result);
			}else if(0 == strncmp("wifi-start",cmd->tokens[0],strlen("wifi-start"))){
				LOGI("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
			//	char *args[5] = { PTT_SOCKET_BIN, "-v", "-d", "-f", NULL };
				//do_handle(result,args,true);
				property_set("vendor.wifi.p_socket_app", "true");
				result->result = 1; // success
			}else if(0 == strncmp("wifi-end",cmd->tokens[0],strlen("wifi-end"))){
				LOGI("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
			//	char *args[5] = { PTT_SOCKET_BIN, "-f", "-d", "-v", NULL };
				//do_handle(result,args,false);
				property_set("vendor.wifi.p_socket_app", "false");
				result->result = 1; // success
			}else if(0 == strncmp("ble-start",cmd->tokens[0],strlen("ble-start"))){
			//	const char *args[3] = { FTMDAEMON_BIN, "-n", NULL };
				LOGE("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
				property_set("vendor.bt.start", "true");
				result->result = 1; // success
			}else if(0 == strncmp("ble-end",cmd->tokens[0],strlen("ble-end"))){
			//	const char *args[3] = { FTMDAEMON_BIN, "-n", NULL };
				LOGE("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
				property_set("vendor.bt.start", "false");
				result->result = 1; // success
			}else{
				LOGI("ATFWD AtCmdFwd Default Handle");
				quec_qfct_handle(result);
			}
		}else{
			LOGI("ATFWD AtCmdFwd Tokens is NULL");
			quec_qfct_handle(result);		
		}

	} 
    else if(strcasecmp(cmd->name, "+QGMR") == 0) 
    {
		quec_qgmr_handle(cmd,result);
	}
#ifdef QUECTEL_AT_QAPSUB_FEATURE
	else if( strcasecmp(cmd->name, "+QAPSUB")==0 )
	{
		quec_qapsub_handle(result);
#endif
    } 
	else if (strcasecmp(cmd->name, "+QDEVINFO") == 0) 
	{
	quec_qdevinfo_handle(cmd, result);
#ifdef QUECTEL_QAPCMD_CMD
	} 
	else if (strcasecmp(cmd->name, "+QAPCMD") == 0) 
	{
        quec_qapcmd_handle(cmd, result);
#endif
    }
#else
    result = nullptr;
#endif
    return result;
}
};  /* namespace android */
