/**
  @file
  quec_wat_hanle.c

  @brief
  AT cmd handler
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

#define DSAT_ERROR 0
#define DSAT_OK 1
#include "ds_util.h"
#include "quec_port_bridge.h"
extern Quec_wifi_fsg_data  *qwifi_data;
int check_sn_format(char *buf)
{
	int len=strlen(buf);
	if((buf[0]=='"')&&(buf[len-1]=='"')&&(len>2)&&(len<130))
	{
	buf[len-1]='\0';
	return 1;
	}
	return 0;
}
int check_bt_wifi_format(char *buf)
{
	
	int len=strlen(buf);
	if((buf[0]=='"')&&(buf[len-1]=='"')&&(len==14))
	{
	buf[len-1]='\0';
	return 1;
	}
	LOGE("addr string error!!\n");
	return 0;
}

int quec_buf_remove_quotation_marks(char *buf)
{
    int i = 0;
    int j = 0;
	int len = 0;
	char *strbuff = NULL;

    if (buf == NULL) {
        LOGE("%s:%d buf is NULL", __func__, __LINE__);
		return -1;
    }

    len = strlen(buf) + 1;

    strbuff = (char *)malloc(len);
    if (strbuff == NULL) {
        LOGE("%s:%d out of malloc", __func__, __LINE__);
        return -1;
    }

    for (i = 0, j = 0; i < len; i++) {
        if (!(buf[i] == '\'' || buf[i] == '\"')) {
            strbuff[j++] = buf[i];
        }

        buf[i] = '\0'; //原空间清零
    }

    strbuff[j] = '\0';

    strcpy(buf, strbuff);

    if (strbuff) {
        free(strbuff);
        strbuff = NULL;
    }

    return j;
}

void quec_egmr_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		LOGW("emgr:%d,%d\n",atoi(cmd->tokens[0]),atoi(cmd->tokens[1]));
		if((cmd->token_num == 2)&&(atoi(cmd->tokens[0])==0)&&(atoi(cmd->tokens[1])==5))
		{
			offset += snprintf(resp_buf+offset, sizeof(qwifi_data->sn), "+EGMR: \"%s\"",qwifi_data->sn);//flash
			result = DSAT_OK;
		}else if((cmd->token_num == 3)&&(atoi(cmd->tokens[0])==1)&&(atoi(cmd->tokens[1])==5)&&(check_sn_format(cmd->tokens[2])==1))
		{
			memset(qwifi_data->sn,0,sizeof(qwifi_data->sn));
			strncpy(qwifi_data->sn,cmd->tokens[2]+1,strlen(cmd->tokens[2]));//delet ""
			modem_sync();
			result = DSAT_OK;
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+EGMR:(0,1),(5)");//flash
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		result = DSAT_ERROR;
  	}
	else
	{
		result = DSAT_ERROR;
	}	

	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR;
	    LOGE("EGMR_AT error");
	 }
     response->result = result;
	 
	 return;
}

void quec_qnvr_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		if ((cmd->token_num == 2)&&(atoi(cmd->tokens[0])==447)&&(atoi(cmd->tokens[1])==0)) //read bt mac
		{
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVR: \"%s\"",qwifi_data->bt_mac);//flash
			result = DSAT_OK;
		}else if ((cmd->token_num == 2)&&(atoi(cmd->tokens[0])==4678)&&(atoi(cmd->tokens[1])==0))//read wifi mac
		{
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVR: \"%s\"",qwifi_data->wifi_mac);//flash
			result = DSAT_OK;
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVR:(447,4678),(0-255)\n");//flash
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		result = DSAT_ERROR;
  	}
	else
	{
		result = DSAT_ERROR;
	}	

	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR;
	    LOGE("QNVR_AT error");
	 }
     response->result = result;
	 
	 return;
}

void quec_qnvw_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		if ((cmd->token_num == 3)&&(atoi(cmd->tokens[0])==447)&&(atoi(cmd->tokens[1])==0)&&(check_bt_wifi_format(cmd->tokens[2])==1)) //write bt mac
		{
			memset(qwifi_data->bt_mac,0,sizeof(qwifi_data->bt_mac));
			strncpy(qwifi_data->bt_mac,cmd->tokens[2]+1,strlen(cmd->tokens[2]));//delet ""
			modem_sync();
			quec_write_bt_mac();
			//offset += snprintf(response->response+offset, (RESP_BUF_SIZE-offset), "\"%s\n\"",qwifi_data->bt_mac);//flash
			result = DSAT_OK;
		}else if ((cmd->token_num == 3)&&(atoi(cmd->tokens[0])==4678)&&(atoi(cmd->tokens[1])==0)&&(check_bt_wifi_format(cmd->tokens[2])==1))//write wifi mac
		{
			memset(qwifi_data->wifi_mac,0,sizeof(qwifi_data->wifi_mac));
			strncpy(qwifi_data->wifi_mac,cmd->tokens[2]+1,strlen(cmd->tokens[2]));//delet ""
			modem_sync();
			
			quec_write_wifi_mac();
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVW:(447,4678),(0-255)\n");//flash
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		result = DSAT_ERROR;
  	}
	else
	{
		result = DSAT_ERROR;
	}	

	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR;
	    LOGE("EGMR_AT error");
	 }
     response->result = result;
	 
	 return;
}
void quec_qprtpara_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		if ((cmd->token_num == 1)&&(atoi(cmd->tokens[0])==1)) //write bt mac
		{
			modem_backup();
			if(fsc_check_data_valid()==1)
				result = DSAT_OK;
			else
				result = DSAT_ERROR;
			//offset += snprintf(response->response+offset, (RESP_BUF_SIZE-offset), "\"%s\n\"",qwifi_data->bt_mac);//flash
		}else if ((cmd->token_num == 1)&&(atoi(cmd->tokens[0])==3)) //write bt mac
		{
			if(fsc_rmts_fresh_data()==1)
				offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "Quec_restore OK!!\n");//flash
			else
				offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "Quec_restore Fail!!\n");//flash
			
		}		else if ((cmd->token_num == 1)&&(atoi(cmd->tokens[0])==4)) //write bt mac
		{
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "Q4,%d\n",fsc_check_data_valid());//flash
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		//offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QPRTPARA\n");//flash
        result = DSAT_OK;
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QPRTPARA:%d\n",fsc_check_data_valid());//flash
  	}
	else
	{
		result = DSAT_ERROR;
	}	

	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR; // error
	    LOGE("QPRTPARA_AT error");
	 }
     response->result = result;
	 
	 return;
}
void quec_qcsn_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		if ((cmd->token_num == 1)&&(check_sn_format(cmd->tokens[0])==1)) //write qcsn
		{
			memset(qwifi_data->qcsn,0,sizeof(qwifi_data->qcsn));
			strncpy(qwifi_data->qcsn,cmd->tokens[0]+1,strlen(cmd->tokens[0])-1);//delet ""
			modem_sync();
			result = DSAT_OK;
			//offset += snprintf(response->response+offset, (RESP_BUF_SIZE-offset), "\"%s\n\"",qwifi_data->bt_mac);//flash
			
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QCSN: <qcsn_info>\n");//flash
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QCSN: \"%s\"\n",qwifi_data->qcsn);//flash
  	}
	else
	{
		result = DSAT_ERROR;
	}	

	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR;
	    LOGE("EGMR_AT error");
	 }
     response->result = result;
	 
	 return;
}
void quec_qdata_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	int result=DSAT_OK;
	if(cmd->op == (NA|EQ|AR)) // =0,5//1,5,"value"
    {     
		if ((cmd->token_num == 1)&&(check_sn_format(cmd->tokens[0])==1)) //write nvdata
		{
			memset(qwifi_data->nv_data,0,sizeof(qwifi_data->nv_data));
			strncpy(qwifi_data->nv_data,cmd->tokens[0]+1,strlen(cmd->tokens[0])-1);//delet ""
			modem_sync();
			result = DSAT_OK;
			//offset += snprintf(response->response+offset, (RESP_BUF_SIZE-offset), "\"%s\n\"",qwifi_data->bt_mac);//flash
			
		}else if ((cmd->token_num == 2)&&(atoi(cmd->tokens[0])==0)&&(atoi(cmd->tokens[1])<128)) //write nvdata
		{
			int read=atoi(cmd->tokens[1]);
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "QNVDATA[%d]=[%c]\n",read,qwifi_data->nv_data[read]);//read nvdata
			result = DSAT_OK;			
		}else if ((cmd->token_num == 3)&&(atoi(cmd->tokens[0])==1)) //write nvdata
		{
			int write=atoi(cmd->tokens[1]);
			if((write>127)||(write<0)){
			result = DSAT_ERROR;
			}else{
				if((cmd->tokens[2][0]=='"')&&(cmd->tokens[2][2]=='"')&&(strlen(cmd->tokens[2])==3))
				qwifi_data->nv_data[write]=cmd->tokens[2][1];
				result = DSAT_OK;
			}
		}else{
			result = DSAT_ERROR;
		}
    }
	else if(cmd->op == (NA|EQ|QU)) // =?
	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVDATA: <qdata_info>\n");
	}
	else if(cmd->op == (NA|QU)) //?
  	{
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QNVDATA: \"%s\"\n",qwifi_data->nv_data);
  	}
	else
	{
		result = DSAT_ERROR;
	}	


	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR; // error
	    LOGE("AT error");
	 }
     response->result = result;
	 
	 return;
}
void quec_at_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int result=DSAT_OK;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	//sprintf(resp_buf,"%s");
	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR; // error
	    LOGE("AT error");
	 }
     response->result = result;
	 
	 return;
}
void quec_ati_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int result=DSAT_OK;
	//get_modem_version();
	char version[64]={0};
	int count=0;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	 memcpy(version,qwifi_data->modem_version,64);
	 for(count=0;count<64;count++){
		 if(version[count]=='_'){
			version[count]='\0';
			break;
		 }
	 }
		 
	 //if(qwifi_data->modem_version[0]==0)
	sprintf(resp_buf,"Quectel\nSC66\nRevision:%s\n",version);
	//offset += snprintf(resp_buf+offset, count+1, "%s",version);//flash
	//sprintf(resp_buf,"%s");
	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR; // error
	    LOGE("ATI error");
	 }
     response->result = result;
	 
	 return;
}

void quec_qapsub_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int result=DSAT_OK;
	char *ptr=NULL;
	char target[128] = {0}; // Stores target info      
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	 if(cmd->op == (NA))
	 {
		memset(target, 0, 128);
		property_get(ATFWD_DATA_PROP_QUEC_VER, target, "");
		LOGI("qapsub_quectel:%s",target);
		// remove V01
		if((ptr=strchr(target,'V'))==NULL)
		{
			result = DSAT_ERROR;
		}else{
		sprintf(resp_buf,"APSubEdition:%s",ptr);
		LOGI("Line[%d]: resp_buf:%s", __LINE__, resp_buf);
		}
	 }else{
		result = DSAT_ERROR;
	}	
	if((response->response=resp_buf)  == NULL )
	 {
		result = DSAT_ERROR; // error
	    LOGE("ATI error");
	 }
     response->result = result;
	 
	 return;
}
void quec_qgmr_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	char *resp_buf=NULL;
	int offset=0;
	char *ptr=NULL;
	char target[128] = {0}; // Stores target info      
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
     }
	 int result=DSAT_OK;
	 LOGE("%s:%d qgmr_test\n", __func__, __LINE__);
	if(cmd->op == (NA|EQ|QU)) // =?
	{
			 LOGE("%s:%d qgmr_test\n", __func__, __LINE__);
		offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QGMR");//flash
	}
	else if((cmd->op == (NA|QU))||(cmd->op == (NA)))//?
  	{
		LOGE("%s:%d qgmr_test\n", __func__, __LINE__);
		property_get(ATFWD_DATA_PROP_ANDROID_VER, target, "");
		LOGI("qgmr_andorid:%s",target);
		sprintf(resp_buf,"Quectel\nSC66\nRevision: ");
		sprintf(resp_buf,"%s%s_Android%s.0.0",resp_buf,qwifi_data->modem_version,target);
		memset(target,0,64);
		property_get(ATFWD_DATA_PROP_QUEC_VER, target, "");
		LOGI("qgmr_quectel:%s",target);
		// remove V01
		if((ptr=strchr(target,'V'))!=NULL)
		{
			*ptr='\0';  // 'V' -> '\0'
		}
		sprintf(resp_buf,"%s.%s",resp_buf,target);
		LOGI("Line[%d]: resp_buf:%s", __LINE__, resp_buf);
		result=DSAT_OK;
	}else
	{
		result = DSAT_ERROR;
	}	
	//sprintf(resp_buf,"%s");
	if((response->response=resp_buf)  == NULL )
	 {
		result= 0; // error
	    LOGE("QGMR_AT error");
	 }
     response->result = result;
	 return;
}


#define QUECTEL_FCT_TEST  
#ifdef QUECTEL_FCT_TEST

#define ARRARY_SIZE(a) (sizeof(a)/sizeof(a[0]))
//#define RESP_BUF_SIZE (380) // RESP_BUF_SIZE msut less than the QMI_ATCOP_AT_RESP_MAX_LEN in the file vendor/qcom/proprietary/qmi/inc/qmi_atcop_srvc.h,
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
	{"KEY",0},
	{"HEADSET LOOP", 0},
	{"CAM MAIN",0},
	{"HANDSET", 0},
	{"CAM AUX", 0},
	{"CAM FRONT", 0},
	{"MSENSOR", 0},
	{"NOISE MIC",0},
	{"SD",0},
	{"STORAGE",0},
	{"WIFI",0},
	{"BT",0},
};

char* set_response_buf(fct_item_type *fct_items, int num)
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
	return resp_buf;
}
char* get_string_from_two_char(char *src, char *dest, int size, char start, char end)
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

void quec_fct_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
	#define FCT_RESULT_FILE	 "/mnt/vendor/persist/FTM_AP/mmi.res"

	int total_items;

	char line_text[64] = {0};
	FILE *fp = NULL;

	char mmi_res_name[32] = {0};
	int  i,offset=-1;
	
	if(cmd->op == (NA|EQ|QU)) // =?
	{
	if(response->response == NULL)
	{
		response->response = (char *)malloc(RESP_BUF_SIZE);
		if(response->response == NULL)
		{
         LOGE("%s:%d No Memory\n", __func__, __LINE__);
         return; // error
		}
		memset(response->response, 0, RESP_BUF_SIZE);
     }
  	response->result = 1; // success
	return;
	}
	
  int items_length = ARRARY_SIZE(fct_items_all);
	

  total_items = items_length;

  fct_item_type fct_items[total_items];        

	for(i=0; i<total_items; i++) {
	    fct_items[i].name = fct_items_all[i].name;
	    fct_items[i].result = 0;
	}
        
	if((fp=fopen(FCT_RESULT_FILE, "r")) == 0)
	{
		printf("open file:%s failed!\n", FCT_RESULT_FILE);
		
		if((response->response = set_response_buf(fct_items, total_items)) == NULL )
		{
			response->result = 0; // error
			LOGI("%s:%d open file %s failed!\n", __func__, __LINE__, FCT_RESULT_FILE);
			//printf("%s:%d open file %s failed!\n", __func__, __LINE__, FCT_RESULT_FILE);
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
					LOGI("Match\n");
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
	
	if((response->response = set_response_buf(fct_items, ARRARY_SIZE(fct_items))) == NULL)
	{
		response->result = 0;
		return;
	}
	response->result = 1; // success
	return;
	
}

#define QUEC_CHGE_INFO "/sys/class/power_supply/battery/input_suspend"
void quec_qapcmd_handle( const AtgCmd *cmd ,AtgCmdResponse *response)
{
    int i = 0;
    int offset=0;
    FILE *fp = NULL;
    char *resp_buf=NULL;
    char buf[10] = {0};

    //LOGI("%s:%d I'm here", __func__, __LINE__);

    resp_buf = (char *)malloc(RESP_BUF_SIZE);
    if(resp_buf == NULL) {
        LOGE("%s:%d  No Memory", __func__, __LINE__);
        return; // error
    }
    memset(resp_buf, 0, RESP_BUF_SIZE);

    response->result = 1;

    response->response = resp_buf;

    if (cmd->op == (NA|EQ|QU)) { // =?
        offset += sprintf(resp_buf+offset, "+QAPCMD:version1.1\n");
        offset += sprintf(resp_buf+offset, "+QAPCMD=\"CHGENABLE\"\n");

        response->result = 1; // success
        return;
    }

    if (cmd->op == (NA|QU)) { // ?
        //show chage state
        fp = fopen(QUEC_CHGE_INFO, "r");
        if (fp == NULL) {
            LOGE("%s: unable to open %s", __func__, QUEC_CHGE_INFO);
            return;
        }

        while (fgets(buf, sizeof(buf), fp) != NULL);

        fclose(fp);

        offset += sprintf(resp_buf+offset, "+QAPCMD=CHGENABLE,%s", buf);
        LOGI("+QAPCMD:CHGENABLE get: %s", buf);

        response->result = 1; // success
        return;
    }

    if (cmd->token_num <= 0) {//else or anomaly show how to use cmd
        free(response->response);
        response->response = NULL;
        response->result = 0; //failure
        return;
    }

    for (i = 0; i < cmd->token_num; i++)
        if (cmd->tokens == NULL || quec_buf_remove_quotation_marks(cmd->tokens[i]) < 0) {
            LOGE("%s:%d: tokens is NULL or failed to remove quotation", __func__, __LINE__);
            response->result = 0; // failure
            return;
        }

    if(strcasecmp(cmd->tokens[0], "CHGENABLE")==0) {
        //LOGI("%s:%d token_num = %d", __func__, __LINE__, cmd->token_num);
        if(cmd->token_num==1) {
            fp = fopen(QUEC_CHGE_INFO, "r");
            if (fp == NULL) {
                LOGE("%s: unable to open %s", __func__, QUEC_CHGE_INFO);
                return;
            }

            while (fgets(buf, sizeof(buf), fp) != NULL);

            fclose(fp);

            //LOGI("%s:%d I'm here", __func__, __LINE__);
            offset += snprintf(resp_buf + offset, (RESP_BUF_SIZE - offset),
                    "+QAPCMD:CHGENABLE,%s", buf);

            LOGI("+QAPCMD:CHGENABLE get: %s", buf);
            response->result = 1;
        }else if((cmd->token_num==2)&&((strcasecmp(cmd->tokens[1],"0")==0)
                    ||(strcasecmp(cmd->tokens[1], "1")==0))) {
            fp = fopen(QUEC_CHGE_INFO, "w");
            if (fp == NULL) {
                LOGE("%s: unable to open %s", __func__, QUEC_CHGE_INFO);
                return;
            }

            if (fputs(cmd->tokens[1], fp) < 0) {
                LOGE("%s: failed to write %s to %s", __func__, cmd->tokens[1], QUEC_CHGE_INFO);
                return;
            }

            fclose(fp);

            LOGI("+QAPCMD:CHGENABLE set: %s", cmd->tokens[1]);
            response->result = 1;
        }else{
            response->result = 0; // error
            LOGE("QAPCMD_AT CHGENABLE error");
        }
    } else if((strcasecmp(cmd->tokens[0], "CHECK")==0) && (cmd->token_num==1)) {
        offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QAPCMD:version1.1\n");
        offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QAPCMD:CHGENABLE");
    } else if((strcasecmp(cmd->tokens[0], "ALL")==0) && (cmd->token_num==1)) {
        fp = fopen(QUEC_CHGE_INFO, "r");
        if (fp == NULL) {
            LOGE("%s: unable to open %s", __func__, QUEC_CHGE_INFO);
            return;
        }

        while (fgets(buf, sizeof(buf), fp) != NULL);

        fclose(fp);

        offset += snprintf(resp_buf + offset, (RESP_BUF_SIZE - offset),
                "+QAPCMD:CHGENABLE,%s", buf);

        LOGI("+QAPCMD:CHGENABLE get: %s", buf);
        response->result = 1;
    } else {
        LOGI("%s:%d something wrong!", __func__, __LINE__);
        response->result = 0;
        free(response->response);
        response->response = NULL;
    }

    return;
}

#define QUECTEL_QDEVINFO_CMD
#ifdef QUECTEL_QDEVINFO_CMD
#define QUEC_EMCP_INFO 	"/proc/quec_emcp_info"
#define QUEC_PMU_INFO 	"/proc/quec_pmu_info"
#define QUEC_CPU_INFO 	"/proc/cpuinfo"
#define QUEC_EMCP_NAME  "/sys/class/mmc_host/mmc0/mmc0:0001/name"
#define QUEC_EMCP_MEMINFO  "/proc/meminfo"
#define QUEC_EMMC_SIZE   "/sys/block/mmcblk0/size"
#define QUEC_INFO_LEN 128

int quectel_search_str( char *str1,char *str2)
{
    int i;//,j;
    char *buf;
    char str_Hardware[]="Qualcomm Technologies, Inc ";

    buf=strstr(str1,str_Hardware)+strlen(str_Hardware);
    for(i=0; buf[i] != '\0'; i++) {
        if(buf[i]=='\n') {
            buf[i]='\0';
            break;
        }
    }

    if((buf != NULL) && (strlen(buf) < QUEC_INFO_LEN))
        snprintf(str2,strlen(buf)+1,"%s",buf);
    else
        sprintf(str2,"get cpuinfo error!!");

    return 0;
}

void get_cpu_info( char *info)
{
    //LOGI("%s:%d I'am here", __func__, __LINE__);

    char buf[4096]={'\0'};
    char cpu_str[QUEC_INFO_LEN]={'\0'};
    FILE *fp = NULL;

    if((fp=fopen(QUEC_CPU_INFO, "r")) != 0) {
        fread(buf,4096,1,fp);
        fclose(fp);

        quectel_search_str(buf,cpu_str);
        sprintf(info,"%s",cpu_str);
    }

    return;
}

void get_file_info( const char *file,char *info)
{
    int i;
    char devinfo_str[QUEC_INFO_LEN]={'\0'};
    FILE *fp = NULL;
    if ((fp=fopen(file, "r")) != 0) {
        fread(devinfo_str,QUEC_INFO_LEN,1,fp);

        fclose(fp);

        for (i=0;devinfo_str[i]!='\0';i++) {
            if (devinfo_str[i]=='\n') {
                devinfo_str[i]='\0';
                break;
            }
        }

        sprintf(info,"%s",devinfo_str);
    }

    return;
}

int get_flash_info(char *buf)
{
    FILE *fp = NULL;
    size_t filesize = 0;
    ssize_t readsize = 0;
    int totlesize = 0;
    int offset = 0;
    char *line = NULL;
    char buffer[50] = {0};
    char *ptr;

    //LOGI("%s:%d I'am here", __func__, __LINE__);

    if (buf == NULL) {
        LOGI("%s:%d buf is NULL", __func__, __LINE__);
        return -1;
    }

    /* EMCP name */
    fp = fopen(QUEC_EMCP_NAME, "r");
    if (fp == 0) {
        LOGE("%s: unable to open %s", __func__, QUEC_EMCP_NAME);
        return -1;
    }

    readsize = getline(&line, &filesize, fp);
    if (readsize < 0) {
        LOGE("%s:%d failed to getline ", __func__, __LINE__);
        return -1;
    }

    offset += snprintf(buf, readsize, "%s", line);

    if (line)
        free(line);
    line = NULL;

    fclose(fp);

    /* EMMC size */
    fp = fopen(QUEC_EMMC_SIZE, "r");
    if (fp == 0) {
        LOGE("%s: unable to open %s", __func__, QUEC_EMMC_SIZE);
        return -1;
    }

    readsize = getline(&line, &filesize, fp);
    if (readsize < 0) {
        LOGE("%s:%d failed to getline ", __func__, __LINE__);
        return -1;
    }

    LOGI("%s:%d  line:%s", __func__, __LINE__, line);

    sscanf(line, "%d", &totlesize); //MB

    if (line)
        free(line);
    line = NULL;

    fclose(fp);

    totlesize /= 2048;
    if (totlesize % 4096 == 0)
        totlesize = (totlesize/4096) * 4; //GB
    else
        totlesize = (totlesize/4096 + 1) * 4; //GB
    //LOGI("%s:%d totlesize = %d, buf is:%s. offset = %d", __func__, __LINE__, totlesize, buf, offset);
    offset += sprintf(buf + offset - 1, ",%dGB", totlesize);
    //LOGI("%s:%d totlesize = %d, buf is:%s. offset = %d", __func__, __LINE__, totlesize, buf, offset);

    /* size */
    fp = fopen(QUEC_EMCP_MEMINFO, "r");
    if (fp == 0) {
        LOGE("%s: unable to open %s", __func__, QUEC_EMCP_MEMINFO);
        return -1;
    }

    while(fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
        ptr = strchr(buffer, ':');
        if (!strncmp(buffer, "MemTotal", ptr - buffer)) {
            sscanf(ptr + 1, "%d%*s", &totlesize);
            totlesize = totlesize/1024;

            LOGI("%s:%d  memTotal size: %dMB", __func__, __LINE__, totlesize);

            if (totlesize <= 512)
                offset += sprintf(buf + offset - 1, ",512MB");
            else {
                totlesize = totlesize/1024 + 1;
                offset += sprintf(buf + offset - 1, ",%dGB", totlesize);
            }
            //LOGI("%s:%d totlesize = %d, buf is:%s", __func__, __LINE__, totlesize, buf);
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(fp);

    return 0;
}

void quec_qdevinfo_handle(const AtgCmd *cmd, AtgCmdResponse *response)
{
    int i = 0;
    char *resp_buf=NULL;
    char devinfo[QUEC_INFO_LEN]={'\0'};
    int offset=0;

    resp_buf = (char *)malloc(RESP_BUF_SIZE);
    if(resp_buf == NULL) {
        LOGE("%s:%d  No Memory", __func__, __LINE__);
        return; // error
    }
    memset(resp_buf, 0, RESP_BUF_SIZE);

    response->result = 1;

    response->response = resp_buf;

    //LOGI("%s:%d  Peeta I am here", __func__, __LINE__);

    if (cmd->op == (NA|QU)) {//AT+QDEVINFO? show all infos
        memset(devinfo, 0, QUEC_INFO_LEN);
        get_cpu_info(devinfo);
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"BB\",%s\n", devinfo);

        memset(devinfo, 0, QUEC_INFO_LEN);
        if (get_flash_info(devinfo) == 0)
            offset += sprintf(resp_buf + offset, "+QDEVINFO: \"FLASH\",%s\n", devinfo);
        else
            offset += sprintf(resp_buf + offset, "+QDEVINFO: \"FLASH\",NULL\n");

        memset(devinfo, 0, QUEC_INFO_LEN);
        get_file_info(QUEC_PMU_INFO,devinfo);
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PMIC\",%s\n", devinfo);

        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PA\",NULL");

        response->result = 1; // success

        return;
    }

    if (cmd->op == (NA|EQ|QU)) {//AT+QDEVINFO=?
        //show help, how to use cmd
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"BB\"\n");
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"FLASH\"\n");
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PMIC\"\n");
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PA\"");
        response->result = 1; // success

        return;
    }

    if (cmd->token_num <= 0) {//AT+QDEVINFO=? or AT+QDEVINFO= or other anomaly.
        LOGI("%s:%d  Peeta", __func__, __LINE__);

        free(response->response);
        response->response = NULL;
        response->result = 0; //failure
        return;
    }

    for (i = 0; i < cmd->token_num; i++) {
        if (cmd->tokens[i] == NULL || quec_buf_remove_quotation_marks(cmd->tokens[i]) < 0) {
            LOGE("%s:%d tokens is NULL or failed to remove quotation", __func__, __LINE__);
            free(resp_buf);
            response->response = NULL;
            response->result = 0; // failure
            return;
        }
    }

    if(strcasecmp(cmd->tokens[0], "FLASH") == 0) {
        //get_file_info(QUEC_EMCP_INFO,devinfo);
        if (get_flash_info(devinfo) == 0) {
            offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QDEVINFO: \"FLASH\",%s", devinfo);
            response->result = 1;
        }
    } else if(strcasecmp(cmd->tokens[0], "BB") == 0) {
        get_cpu_info(devinfo);
        offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QDEVINFO: \"BB\",%s",devinfo);
    } else if(strcasecmp(cmd->tokens[0], "PA")==0) {
        offset += sprintf(resp_buf+offset, "+QDEVINFO: \"PA\",NULL");//\nPA:1,%s",cmd->tokens[1],cmd->tokens[2]);
    } else if (strcasecmp(cmd->tokens[0], "PMIC") == 0) {
        get_file_info(QUEC_PMU_INFO,devinfo);
        offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QDEVINFO: \"PMIC\",%s",devinfo);
#if 0
    } else if(strcasecmp(cmd->tokens[0], "ALL") == 0) {//show all infos
        memset(devinfo, 0, QUEC_INFO_LEN);
        get_cpu_info(devinfo);
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"BB\",%s\n", devinfo);

        memset(devinfo, 0, QUEC_INFO_LEN);
        if (get_flash_info(devinfo) == 0)
            offset += sprintf(resp_buf + offset, "+QDEVINFO: \"FLASH\",%s\n", devinfo);
        else
            offset += sprintf(resp_buf + offset, "+QDEVINFO: \"FLASH\",NULL\n");

        memset(devinfo, 0, QUEC_INFO_LEN);
        get_file_info(QUEC_PMU_INFO,devinfo);
        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PMIC\",%s\n", devinfo);

        offset += sprintf(resp_buf + offset, "+QDEVINFO: \"PA\",NULL");

        response->result = 1; // success
#endif
    } else {
        LOGW("%s:%d something wrong!", __func__, __LINE__);
        response->result = 0;
        free(response->response);
        response->response = NULL;
    }

    return;
}
#endif /* QUECTEL_QDEVINFO_CMD */

#endif
