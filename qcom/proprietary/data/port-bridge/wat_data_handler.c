/**
  @file
  wat_data_hanler.c

  @brief
  Deal the buf form /dev/at_usb0 ,change the string format to AT format 
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


char wat_allowed_list[][MAX_CMD_NAME] = {
	//"AT"
	"ATI",
	"AT+QGMR",
	"AT+EGMR",
	"AT+QNVR",
	"AT+QNVW",
	"AT+QPRTPARA",
	"AT+QFCT",
	"AT+QCSN",
	"AT+QDATA",
	"AT+QAPSUB",
    "AT+QAPCMD",
    "AT+QDEVINFO",
	""
};

int quec_buf_skip_spechars(char *buf)
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
        if (!(buf[i] == ' ' || buf[i] == '\r'
                    || buf[i] == '\n')) {
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

#if 0
	if((buf==NULL)||(buf[0]=='\r')||((buf[0]=='\n'))||(buf[0]==0)) {
		LOGE("quec_buf_skip_spechars none:%s",buf);
		return 0;
	}

	while(*buf++==' ')
		strIn=buf;

	LOGW("quec_buf_skip_spechars:%s",strIn);
	if(strIn==NULL)
	{
		LOGE("quec_buf_at not_exit:%s",buf);
		return 0;
	}
	len=strlen(strIn)-1;
	while(len>0)
	{
	if((strIn[len] == ' ')||(strIn[len]=='\r')||(strIn[len]=='\n'))
		{
		strIn[len]='\0';
		len--;
		}
	else
		break;
	}
	LOGW("quec_buf_skip_spechars len:%d,[%s]",len,strIn);
	buf = strIn;
	if(buf==NULL)
		return 0;
	return 1;
#endif
}

int quec_at_ar_ins(char *buf,AtgCmd *at_cmd)
{
	int i = 0;
	int len=0;
	char *p = buf;
	char *pos1 = buf;
	char *pos2 = NULL;
	char *pos = strstr(p,",");

	//LOGI("%s:%d buf:%s\n", __func__, __LINE__, buf);
	at_cmd->token_num = 1;
	at_cmd->op |= AR;

	while (pos != NULL) {
		pos = strstr(pos,",");
		if (pos != NULL) {
			at_cmd->token_num++;
			pos = pos+1;
		}
	}

	at_cmd->tokens = (char **)malloc(at_cmd->token_num * sizeof(char *));

	if (at_cmd->tokens==NULL) {
        LOGE("%s:%d: out of memory", __func__, __LINE__);
		return -1;
    }

    for (i=0; i < at_cmd->token_num; i++) {
        at_cmd->tokens[i] = (char *)malloc(AT_DATE_LEN);
        memset(at_cmd->tokens[i], 0,AT_DATE_LEN);

        pos2 = strstr(pos1,",");
        if(pos2 !=NULL )
            len=strlen(pos1) - strlen(pos2); //abc,def 7 - 4 = 3
        else
            len=strlen(pos1);

        if (len <(AT_DATE_LEN - 1)) {
            memcpy(at_cmd->tokens[i], pos1, len);
            at_cmd->tokens[i][len]='\0';

	        LOGI("%s:%d tokens[%d]:%s\n", __func__, __LINE__, i, at_cmd->tokens[i]);
            //quec_buf_skip_spechars(at_cmd->tokens[i]);
            pos1 += len+1;

        } else {
	        LOGI("%s:%d len = %d is biger then AT_DATE_LEN\n", __func__, __LINE__, len);
            return -1;
        }
    }

    return 0;
}

int quec_at_ins_par(AtgCmd *at_cmd,char *buf)
{
    int i = 0;
    char *pos = buf;

    //LOGI("%s:%d fulinux", __func__, __LINE__);
    quec_buf_skip_spechars(pos);

    at_cmd->op = 0;
    if(strcasecmp(pos,"AT")==0) {
        at_cmd->name="AT";
        at_cmd->token_num=0;
        return 0;
    }

    for (i=0; i < ARRAY(wat_allowed_list); i++) {
        if(strncasecmp(pos,wat_allowed_list[i],strlen(wat_allowed_list[i]))==0)//buf要不要+3？？？
        {
            at_cmd->op |= NA;
            if(strcasecmp(pos,"ATI")==0)
            {
                at_cmd->name = wat_allowed_list[i];
                at_cmd->token_num=0;
                //buf += strlen(wat_allowed_list[i]);
                return 0;
            }

            at_cmd->name = wat_allowed_list[i]+2;
            pos += strlen(wat_allowed_list[i]);
            //quec_buf_skip_spechars(pos);

            if ((pos==NULL)||(pos[0]=='\r')||((pos[0]=='\n'))||(pos[0]==0)) {
                at_cmd->token_num=0;
                return 0;
            } else if ((pos[0]=='?') && (strlen(pos)==1)) {
                at_cmd->op |= QU;
                at_cmd->token_num=0;
                return 0;
            } else if ((strcasecmp(pos,"=?")==0) && (strlen(pos)==2)) {
                at_cmd->op |= QU;
                at_cmd->op |= EQ;
                at_cmd->token_num=0;
                return 0;
            }else if((pos[0] == '=')&&(strlen(pos)>1))
            {
                at_cmd->op |= EQ;
                pos+=1;
                //if(quec_buf_skip_spechars(pos)==0)
                //	return 0;
                return quec_at_ar_ins(pos,at_cmd);
            } else
                break;
        }
    }

    at_cmd->op = 0;
    at_cmd->token_num = 0;

    return -1;
}

