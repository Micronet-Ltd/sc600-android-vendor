/**
  @file
  quec_wat_hanle.c

  @brief
  Deal the AT cmd,Realization  function
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

#include "ds_util.h"
#include "quec_port_bridge.h"

extern int write_to_device(int to_fd,const char* to_filename,int bytes_to_write,char* buffer);
extern void close_fd_mutexed(const char* file_path, volatile int* const fd, pthread_mutex_t* mutex);
extern int usb_fd;
pthread_mutex_t quec_mutex   = PTHREAD_MUTEX_INITIALIZER;

int quec_watcmd_handle(const AtgCmd *at_cmd ,AtgCmdResponse *response)
{

	if(strcasecmp(at_cmd->name, "ATI")==0)
	{
		quec_ati_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QGMR")==0)
	{
		quec_qgmr_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+EGMR")==0)
	{
		quec_egmr_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QNVR")==0)
	{
		quec_qnvr_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QNVW")==0)
	{
		quec_qnvw_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QPRTPARA")==0)
	{
		quec_qprtpara_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QCSN")==0)
	{
		quec_qcsn_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QDATA")==0)
	{
		quec_qdata_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "AT")==0)
	{
		quec_at_handle(at_cmd,response);
	}else if(strcasecmp(at_cmd->name, "+QFCT")==0)
	{
		LOGE("calvin atcomd tokens = %s\n",at_cmd->tokens[0]);
		if(NULL != at_cmd->tokens) {
			LOGI("ATFWD AtCmdFwd Tokens Not NULL ntokens=%d",at_cmd->tokens);
			if(at_cmd->tokens == 0 || at_cmd->tokens[0] == NULL){
				LOGI("ATFWD AtCmdFwd Tokens[0] is NULL");
				quec_fct_handle(at_cmd,response);
			}else if(0 == strncmp("ble-start",at_cmd->tokens[0],strlen("ble-start"))){

				LOGE("ATFWD AtCmdFwd:%s",at_cmd->tokens[0]);
				property_set("vendor.bt.start", "true");
				response->result = 1; // success
			}else if(0 == strncmp("ble-end",at_cmd->tokens[0],strlen("ble-end"))){

				LOGE("ATFWD AtCmdFwd:%s",at_cmd->tokens[0]);
				property_set("vendor.bt.start", "false");
				response->result = 1; // success
			}else if(0 == strncmp("wifi-start",at_cmd->tokens[0],strlen("wifi-start"))){

				LOGE("ATFWD AtCmdFwd:%s",at_cmd->tokens[0]);
				property_set("vendor.wifi.start", "true");
				response->result = 1; // success
			}else if(0 == strncmp("wifi-end",at_cmd->tokens[0],strlen("wifi-end"))){

				LOGE("ATFWD AtCmdFwd:%s",at_cmd->tokens[0]);
				property_set("vendor.wifi.start", "false");
				response->result = 1; // success
			}else{
				LOGI("ATFWD AtCmdFwd Default Handle");
				quec_fct_handle(at_cmd,response);
			}
		}else{
			LOGI("ATFWD AtCmdFwd Tokens is NULL");
			quec_fct_handle(at_cmd,response);
		}
	}else if(strcasecmp(at_cmd->name, "+QAPSUB")==0)
	{
		quec_qapsub_handle(at_cmd,response);
    } else if (strcasecmp(at_cmd->name, "+QAPCMD") == 0) {
        quec_qapcmd_handle(at_cmd, response);
    } else if (strcasecmp(at_cmd->name, "+QDEVINFO") == 0) {
        quec_qdevinfo_handle(at_cmd, response);
    }
	return 1;
}

void qfree( AtgCmd *at_cmd)
{	
	int i=0;

    LOGI("%s:%d fulinux", __func__, __LINE__);

	for(i=0;i<at_cmd->token_num;i++)
		{
		if(at_cmd->tokens[i]!=NULL)
		free(at_cmd->tokens[i]);//[i]);
            at_cmd->tokens[i] = NULL;
		}
	if(at_cmd->tokens!=NULL) {	
		free(at_cmd->tokens);
        at_cmd->tokens = NULL;
    }
}

int quec_atg_response(char *buf,AtgCmdResponse *response)
{	
    int ret=0;
    int offset=0;

    char *resp_buf = (char *)malloc(RESP_BUF_SIZE);
    if (resp_buf == NULL) {
        LOGE("%s:%d out of memory", __func__, __LINE__);
        return -1;
    }

    memset(resp_buf, 0, RESP_BUF_SIZE);

    offset+= snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "%s\n", buf);
    if(response->result == 0)
        offset+= snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "ERROR\n");
    else {
        if (response->response)
            offset+= snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "%s",response->response);
        offset+= snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\nOK\n");
    }

    if (response->response) {
        free(response->response);
        response->response = NULL;
    }

    LOGW("%s:%d: resp_buf:%s",__func__, __LINE__, resp_buf);

    PB_PTHREAD_MUTEX_LOCK(&quec_mutex, USB_FILE_PATH);
    ret = write_to_device(usb_fd, USB_FILE_PATH, offset, resp_buf);
    PB_PTHREAD_MUTEX_UNLOCK(&quec_mutex, USB_FILE_PATH);
    if (ret < 0) {
        close_fd_mutexed(USB_FILE_PATH, &usb_fd, &quec_mutex);
    }

    free(resp_buf);
    resp_buf = NULL;

    return ret;
}

int quec_wat_cmd_translate(AtgCmd *at_cmd,void *buf)
{	
    //int i = 0;

    if (quec_at_ins_par(at_cmd, buf) < 0) {
        LOGE("%s:%d error1", __func__, __LINE__);
        return -1;
    }

#if 0
    LOGI("%s:%d op=%d,name=%s,token_num=%d\r\n", __func__, __LINE__,
            at_cmd->op, at_cmd->name, at_cmd->token_num);

    if (at_cmd->token_num =< 0) {
        LOGE("%s:%d error1", __func__, __LINE__);
        return 0;
    }

    for(i=0; i<at_cmd->token_num; i++) {
            LOGI("%s:%d tokens[%d]=%s", __func__, __LINE__, i, at_cmd->tokens[i]);
    }
#endif

    return 0;
}

int quec_atg_handdler(void *buf)
{
    int ret=0;
    AtgCmdResponse resp;
    AtgCmd at_cmd;
    memset(&at_cmd,0,sizeof(AtgCmd));
    memset(&resp,0,sizeof(AtgCmdResponse));

    LOGW("%s:%d: buf:%s",__func__, __LINE__, buf);

    if(quec_wat_cmd_translate(&at_cmd, buf) == 0) {
        quec_watcmd_handle(&at_cmd, &resp);
    }

    ret = quec_atg_response(buf, &resp);

    qfree(&at_cmd);

    return ret;
}



