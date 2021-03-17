#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_v01.h"
#include "customer.h"
#include "qmi.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "ipth_hal_typedef.h"
#include <cutils/log.h>

qmi_idl_service_object_type  ims_svc_obj_p;
qmi_client_type              ims_svc_client = NULL;
qmi_client_os_params         ims_os_params;
qmi_client_os_params         ims_notifier_os_params;
qmi_client_type              ims_notifier = NULL;
bool                         ims_svc_client_init = false;

int idev_ims_client_init()
{
    ALOGE("alpha idev_ims_client_init");
    qmi_client_error_type rc;
    qmi_service_info      info;
    unsigned int          num_entries = 1;
    unsigned int          num_services;

    ims_svc_obj_p = imss_get_service_object_v01();
    if(ims_svc_obj_p == NULL)
    {
        ALOGE("alpha init here0");
        return -1;
    }
    rc = qmi_client_notifier_init(ims_svc_obj_p,
            &ims_notifier_os_params,
            &ims_notifier);
    if(rc != QMI_NO_ERR)
    {
        ALOGE("alpha init here1");
        return -1;
    }

    QMI_CCI_OS_SIGNAL_WAIT(&ims_notifier_os_params, 3000);
    if(ims_notifier_os_params.timed_out)
    {
        ALOGE("alpha init here2");
        return -1;
    }
    rc = qmi_client_get_service_list(ims_svc_obj_p,
            &info,
            &num_entries,
            &num_services);
    if(rc != QMI_NO_ERR)
    {
        ALOGE("alpha init here3");
        qmi_client_release(ims_notifier);
        return -1;
    }
    rc = qmi_client_init(&info,
            ims_svc_obj_p,
            NULL,
            NULL,
            &ims_os_params,
            &ims_svc_client);
    if(rc != QMI_NO_ERR)
    {
        ALOGE("alpha init here4");
        qmi_client_release(ims_notifier);
        return -1;
    }
    ims_svc_client_init = true;
    return 0;
}

void idev_modify_ims_sms_configuration(char *sms_format,char *sms_over_ip)
{
    ims_settings_set_sms_config_req_msg_v01  req_msg;
    ims_settings_set_sms_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();

    if(ims_svc_client == NULL)
        return;
    memset(&req_msg, 0, sizeof(ims_settings_set_sms_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_set_sms_config_rsp_msg_v01));

    if(sms_format != NULL)
    {
        req_msg.sms_format_valid = 1;
        if(!strncmp(sms_format,"3GPP2",strlen("3GPP2"))){
            req_msg.sms_format = IMS_SETTINGS_SMS_FORMAT_3GPP2_V01;
        } 
        else if(!strncmp(sms_format,"3GPP",strlen("3GPP"))){
            req_msg.sms_format = IMS_SETTINGS_SMS_FORMAT_3GPP_V01;
        }
        else {
            req_msg.sms_format_valid = 0;
        }
    }
    if(sms_over_ip != NULL)
    {
        req_msg.sms_over_ip_network_indication_valid = 1;
        if(!strncasecmp(sms_over_ip,"true",strlen("true"))){
            req_msg.sms_over_ip_network_indication = 1;
        }
        else if (!strncasecmp(sms_over_ip,"false",strlen("false")))
        {
            req_msg.sms_over_ip_network_indication = 0;
        } else
        {
            return;  //do nothing
        }
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_SMS_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    } 
    else
    {
    } 
}

void idev_get_ims_sms_configuration(char *sms_format,char *sms_over_ip)
{
    ALOGE("alpha idev_get_ims_sms_configuration");
    ims_settings_get_sms_config_req_msg_v01  req_msg;
    ims_settings_get_sms_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    *sms_format = '\0';
    *sms_over_ip = '\0';
    if(ims_svc_client_init == false)
        idev_ims_client_init();


    if(ims_svc_client == NULL){
        ALOGE("alpha pal here0-1");
        return ;
    }
    memset(&req_msg, 0, sizeof(ims_settings_get_sms_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_sms_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_SMS_CONFIG_REQ_V01,
            (void*)&req_msg,
            sizeof(req_msg),
            (void*)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
        ALOGE("alpha pal here0-2");
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.sms_format_valid == 1)
            {
                if(resp_msg.sms_format == IMS_SETTINGS_SMS_FORMAT_3GPP2_V01)
                    strcpy(sms_format,"3GPP2");
                else
                    strcpy(sms_format,"3GPP");
            }
            ALOGE("alpha here1");
            if(resp_msg.sms_over_ip_network_indication_valid == 1)
            {
                ALOGE("alpha here2");
                if(resp_msg.sms_over_ip_network_indication == 1){
                    strcpy(sms_over_ip,"true");
                    ALOGE("alpha here4");
                }else{
                    strcpy(sms_over_ip,"false");
                    ALOGE("alpha here5");
                }
            }
            ALOGE("alpha here3");
        }else{
            strcpy(sms_over_ip,"true");
            ALOGE("alpha here10");
        }
    }
}

void idev_get_ims_domain(char *ims_domain)
{
    ims_settings_get_user_config_req_msg_v01  req_msg;
    ims_settings_get_user_config_rsp_msg_v01  resp_msg;
    int req_len, resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    *ims_domain = '\0';
    memset(&req_msg, 0, sizeof(ims_settings_get_user_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_get_user_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_USER_CONFIG_REQ_V01,
            (void*)&req_msg,
            sizeof(req_msg),
            (void*)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.ims_domain_valid == 1)
            {
                memcpy(ims_domain,resp_msg.ims_domain,strlen(resp_msg.ims_domain));
            }
        }
    }   
}

void idev_modify_ims_domain(char *ims_domain)
{ 
    ims_settings_set_user_config_req_msg_v01   req_msg;
    ims_settings_set_user_config_rsp_msg_v01   resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    if(ims_domain != NULL)
    {
        req_msg.ims_domain_valid = 1;
        memset(req_msg.ims_domain,0,(IMS_SETTINGS_STRING_LEN_MAX_V01+1));
        memcpy(req_msg.ims_domain,ims_domain,strlen(ims_domain));
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_USER_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    } 
    else
    {
    }
}

void idev_get_ims_sip_configuration(char *sip_t1,char *sip_t2,char *sip_tf)
{
    ims_settings_get_sip_config_req_msg_v01  req_msg;
    ims_settings_get_sip_config_rsp_msg_v01  resp_msg;
    int req_len, resp_len;
    qmi_client_error_type rc;

    strcpy(sip_t1,"0");
    strcpy(sip_t2,"0");
    strcpy(sip_tf,"0");

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg,0,sizeof(ims_settings_get_sip_config_req_msg_v01));
    memset(&resp_msg,0,sizeof(ims_settings_get_sip_config_rsp_msg_v01));


    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_SIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.timer_t1_valid == 1)
            {
                sprintf(sip_t1,"%d",resp_msg.timer_t1);
            }	
            if(resp_msg.timer_t2_valid == 1)
            {
                sprintf(sip_t2,"%d",resp_msg.timer_t2);
            }
            if(resp_msg.timer_tf_valid == 1)
            {
                sprintf(sip_tf,"%d",resp_msg.timer_tf);
            } 
        }
    }
}

void idev_modify_ims_sip_configuration(char *sip_t1,char *sip_t2,char *sip_tf)
{
    ims_settings_set_sip_config_req_msg_v01  req_msg;
    ims_settings_set_sip_config_rsp_msg_v01  resp_msg;
    int req_len, resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_set_sip_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_set_sip_config_rsp_msg_v01));

    if(sip_t1 != NULL)
    {
        req_msg.timer_t1_valid = 1;
        req_msg.timer_t1 = atoi(sip_t1);
    }
    if(sip_t2 != NULL)
    {
        req_msg.timer_t2_valid = 1;
        req_msg.timer_t2 = atoi(sip_t2);
    }
    if(sip_tf != NULL)
    {
        req_msg.timer_tf_valid = 1;
        req_msg.timer_tf = atoi(sip_tf);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_SIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    } 
}

void idev_get_volte_publish_timer(char *publish_timer)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.publish_expiry_timer_valid == 1)
            {
                sprintf(publish_timer,"%d",resp_msg.publish_expiry_timer);
            }
        }
    }
}
void idev_get_volte_publish_timer_extened(char *publish_timer_extended)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.publish_extended_expiry_timer_valid == 1)
            {
                sprintf(publish_timer_extended,"%d",resp_msg.publish_extended_expiry_timer);
            }
        }
    }
}
void idev_get_volte_capabites_cache_expiration(char *capabities_cache_expiration)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.capabilites_cache_expiration_valid == 1)
            {
                sprintf(capabities_cache_expiration,"%d",resp_msg.capabilites_cache_expiration);
            }
        }
    }
}
void idev_get_volte_availability_cache_expiration(char *availability_cahce_expiration)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.availability_cache_expiration_valid == 1)
            {
                sprintf(availability_cahce_expiration,"%d",resp_msg.availability_cache_expiration);
            }
        }
    }
}

void idev_get_volte_capabities_poll_interval(char *capabities_poll_interval)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.capability_poll_interval_valid == 1)
            {
                sprintf(capabities_poll_interval,"%d",resp_msg.capability_poll_interval);
            }
        }
    }
}

void idev_get_volte_capab_list_subscription_expiry_timer(char *capab_list_subscription_expiry_timer)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.capability_poll_list_subscription_expiry_timer_valid == 1)
            {
                sprintf(capab_list_subscription_expiry_timer,"%d",resp_msg.capability_poll_list_subscription_expiry_timer);
            }
        }
    }
}

void idev_get_volte_max_subscription_list_entries(char *max_subcription_list_entries)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.max_subcription_list_entries_valid == 1)
            {
                sprintf(max_subcription_list_entries,"%d",resp_msg.max_subcription_list_entries);
            }
        }
    }
}

void idev_get_volte_gzip_flag(char *gzip_flag)
{
    ims_settings_get_presence_config_req_msg_v01  req_msg;
    ims_settings_get_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return ;

    memset(&req_msg, 0, sizeof(ims_settings_get_presence_config_req_msg_v01));
    memset(&resp_msg, 0, sizeof(ims_settings_get_presence_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.gzip_enabled_valid == 1)
            {
                if(resp_msg.gzip_enabled == 1)
                    strcpy(gzip_flag,"true");
                else
                    strcpy(gzip_flag,"false");
            }
        }
    }
}

void idev_modify_volte_publish_timer(char *publish_timer)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(publish_timer != NULL)
    {
        req_msg.publish_expiry_timer_valid = 1;
        req_msg.publish_expiry_timer = atoi(publish_timer);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_publish_timer_extened(char *publish_timer_extended)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(publish_timer_extended != NULL)
    {
        req_msg.publish_extended_expiry_timer_valid = 1;
        req_msg.publish_extended_expiry_timer = atoi(publish_timer_extended);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_capabites_cache_expiration(char *capabities_cache_expiration)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(capabities_cache_expiration != NULL)
    {
        req_msg.capabilites_cache_expiration_valid = 1;
        req_msg.capabilites_cache_expiration = atoi(capabities_cache_expiration);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}
void idev_modify_volte_availability_cache_expiration(char *availability_cahce_expiration)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(availability_cahce_expiration != NULL)
    {
        req_msg.availability_cache_expiration_valid = 1;
        req_msg.availability_cache_expiration = atoi(availability_cahce_expiration);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_capabities_poll_interval(char *capabities_poll_interval)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(capabities_poll_interval != NULL)
    {
        req_msg.capability_poll_interval_valid = 1;
        req_msg.capability_poll_interval = atoi(capabities_poll_interval);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_capab_list_subscription_expiry_timer(char *capab_list_subscription_expiry_timer)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(capab_list_subscription_expiry_timer != NULL)
    {
        req_msg.capability_poll_list_subscription_expiry_timer_valid = 1;
        req_msg.capability_poll_list_subscription_expiry_timer = atoi(capab_list_subscription_expiry_timer);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_max_subscription_list_entries(char *max_subcription_list_entries)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(max_subcription_list_entries != NULL)
    {
        req_msg.max_subcription_list_entries_valid = 1;
        req_msg.max_subcription_list_entries = atoi(max_subcription_list_entries);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_volte_gzip_flag(char *gzip_flag)
{
    ims_settings_set_presence_config_req_msg_v01  req_msg;
    ims_settings_set_presence_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_presence_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_presence_config_rsp_msg_v01));

    if(gzip_flag != NULL)
    {
        req_msg.gzip_enabled_valid = 1;
        if(!strncmp(gzip_flag,"true",strlen("true")))
            req_msg.gzip_enabled = 1;
        else
            req_msg.gzip_enabled = 0;
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_get_voip_sip_session_timer(char *session_timer)
{
    ims_settings_get_voip_config_req_msg_v01  req_msg;
    ims_settings_get_voip_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_get_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_get_voip_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.session_expiry_timer_valid == 1)
            {
                sprintf(session_timer,"%d",resp_msg.session_expiry_timer);
            }
        }
    }

}

void idev_get_voip_min_sip_session_timer(char *min_se)
{
    ims_settings_get_voip_config_req_msg_v01  req_msg;
    ims_settings_get_voip_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_get_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_get_voip_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.min_session_expiry_valid == 1)
            {
                sprintf(min_se,"%d",resp_msg.min_session_expiry);
            }
        }
    }
}

void idev_get_voip_amrmodeset(char *amrmodeset)
{
    ims_settings_get_voip_config_req_msg_v01  req_msg;
    ims_settings_get_voip_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_get_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_get_voip_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.amr_mode_valid == 1)
            {
                sprintf(amrmodeset,"%d",resp_msg.amr_mode);
            }
        }
    }
}

void idev_get_voip_amrwebmodeset(char *amrwebmodeset)
{
    ims_settings_get_voip_config_req_msg_v01  req_msg;
    ims_settings_get_voip_config_rsp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_get_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_get_voip_config_rsp_msg_v01));

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
        if(resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
        {
            if(resp_msg.amr_wb_mode_valid == 1)
            {
                sprintf(amrwebmodeset,"%d",resp_msg.amr_wb_mode);
            }
        }
    }
}

void idev_modify_voip_sip_session_timer(char *session_timer)
{
    ims_settings_set_voip_config_req_msg_v01   req_msg;
    ims_settings_set_voip_config_rsp_msg_v01   resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_voip_config_rsp_msg_v01));

    if(session_timer != NULL)
    {
        req_msg.session_expiry_timer_valid = 1;
        req_msg.session_expiry_timer = atoi(session_timer);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	

}

void idev_modify_voip_min_sip_session_timer(char *min_se)
{
    ims_settings_set_voip_config_req_msg_v01   req_msg;
    ims_settings_set_voip_config_rsp_msg_v01   resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_voip_config_rsp_msg_v01));

    if(min_se != NULL)
    {
        req_msg.min_session_expiry_valid = 1;
        req_msg.min_session_expiry = atoi(min_se);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_voip_amrmodeset(char *amrmodeset)
{
    ims_settings_set_voip_config_req_msg_v01   req_msg;
    ims_settings_set_voip_config_rsp_msg_v01   resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_voip_config_rsp_msg_v01));

    if(amrmodeset != NULL)
    {
        req_msg.amr_mode_valid = 1;
        req_msg.amr_mode = atoi(amrmodeset);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}

void idev_modify_voip_amrwebmodeset(char *amrwebmodeset)
{
    ims_settings_set_voip_config_req_msg_v01   req_msg;
    ims_settings_set_voip_config_rsp_msg_v01   resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;

    if(ims_svc_client_init == false)
        idev_ims_client_init();
    if(ims_svc_client == NULL)
        return;

    memset(&req_msg, 0, sizeof(ims_settings_set_voip_config_req_msg_v01));
    memset(&resp_msg,0, sizeof(ims_settings_set_voip_config_rsp_msg_v01));

    if(amrwebmodeset != NULL)
    {
        req_msg.amr_wb_mode_valid = 1;
        req_msg.amr_wb_mode = atoi(amrwebmodeset);
    }

    rc = qmi_client_send_msg_sync(ims_svc_client,
            QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01,
            (void *)&req_msg,
            sizeof(req_msg),
            (void *)&resp_msg,
            sizeof(resp_msg),
            15000);
    if(rc != QMI_NO_ERR)
    {
    }
    else
    {
    }	
}
