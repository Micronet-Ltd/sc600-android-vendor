#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_v01.h"
#include "customer.h"
#include "qmi.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib.h"
#include "wireless_data_service_v01.h"
#include "network_access_service_v01.h"
#include "ipth_hal_typedef.h"
#include "data_common_v01.h"
#include "qmi_client_instance_defs.h"
#include "ds_util.h"
#include <cutils/log.h>

#define LOG_TAG "PAL_WDS"

qmi_idl_service_object_type  wds_svc_obj_p;
qmi_client_type              wds_svc_client = NULL;
qmi_client_os_params         wds_os_params;
qmi_client_os_params         wds_notifier_os_params;
qmi_client_type              wds_notifier = NULL;
bool                         wds_svc_client_init = false;

qmi_idl_service_object_type  nas_svc_obj_p;
qmi_client_type              nas_svc_client = NULL;
qmi_client_os_params         nas_os_params;
qmi_client_os_params         nas_notifier_os_params;
qmi_client_type              nas_notifier = NULL;
bool                         nas_svc_client_init = false;

int idev_wds_client_init()
{
  qmi_client_error_type rc;
  qmi_service_info      info;
  unsigned int          num_entries = 1;
  unsigned int          num_services;

  wds_svc_obj_p = wds_get_service_object_v01();
  if(wds_svc_obj_p == NULL)
  {
     
     return -1;
  }
  rc = qmi_client_notifier_init(wds_svc_obj_p,
                                &wds_notifier_os_params,
                                &wds_notifier);
  if(rc != QMI_NO_ERR)
  {
     
     return -1;
  }

  QMI_CCI_OS_SIGNAL_WAIT(&wds_notifier_os_params, 3000);
  if(wds_notifier_os_params.timed_out)
  {
     
     return -1;
  }
  rc = qmi_client_get_service_list(wds_svc_obj_p,
                                   &info,
                                   &num_entries,
                                   &num_services);
  if(rc != QMI_NO_ERR)
  {
     
     qmi_client_release(wds_notifier);
     return -1;
  }
  rc = qmi_client_init(&info,
                       wds_svc_obj_p,
                       NULL,
                       NULL,
                       &wds_os_params,
                       &wds_svc_client);
  if(rc != QMI_NO_ERR)
  {
     
     qmi_client_release(wds_notifier);
     return -1;
  }
  wds_svc_client_init = true;
  return 0;
}

void idev_get_lte_pdp_profile(int profile_id, char *apn_name,char *pdp_type, bool *enable)
{
  wds_get_profile_settings_req_msg_v01   req_msg;
  wds_get_profile_settings_resp_msg_v01  resp_msg;
  int req_len,resp_len;
  qmi_client_error_type rc;

  if(wds_svc_client_init == false)
  {
     idev_wds_client_init();
  }
  
  if(wds_svc_client == NULL)
  {
    
    return ;
  }
  //*apn_name = '\0';
  //*pdp_type = '\0'; 
  //*enable = false;
  memset(&req_msg, 0, sizeof(wds_get_profile_settings_req_msg_v01));
  memset(&resp_msg,0, sizeof(wds_get_profile_settings_resp_msg_v01));
  
  req_msg.profile.profile_index = (uint8_t)profile_id;
  req_msg.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

  rc = qmi_client_send_msg_sync(wds_svc_client,
                                QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
                                (void *)&req_msg,
                                sizeof(req_msg),
                                (void *)&resp_msg,
                                sizeof(resp_msg),
                                15000);
  if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
  {
     

     if(resp_msg.apn_name_valid && apn_name != NULL)
     {
         memcpy(apn_name,resp_msg.apn_name, strlen(resp_msg.apn_name));
     }

	 ALOGE("get profile apn_name = %s\n", apn_name);
     if(resp_msg.pdp_type_valid && pdp_type != NULL)
     {
        if(resp_msg.pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01)
          memcpy(pdp_type,"IPV4",strlen("IPV4"));
        else if(resp_msg.pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01)
          memcpy(pdp_type,"IPV6",strlen("IPV6"));
        else if(resp_msg.pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01)
          memcpy(pdp_type,"IPV4V6",strlen("IPV4V6"));
        else
          memcpy(pdp_type,"PPP",strlen("PPP"));
     }

     if (resp_msg.apn_disabled_flag_valid && enable != NULL) {
        *enable = !resp_msg.apn_disabled_flag;
     }
     else if (enable != NULL) {
        *enable = false;
     }

  }
}

void idev_modify_lte_pdp_profile(int profile_id, char *apn_name, char *pdp_type)
{
   wds_modify_profile_settings_req_msg_v01   req_msg;
   wds_modify_profile_settings_resp_msg_v01  resp_msg;
   int req_len,resp_len;
   qmi_client_error_type rc;

   if(wds_svc_client_init == false)
   {
      idev_wds_client_init();
   }

   if(wds_svc_client == NULL)
   {
      
      return ;
   }

   memset(&req_msg, 0, sizeof(wds_modify_profile_settings_req_msg_v01));
   memset(&resp_msg,0, sizeof(wds_modify_profile_settings_resp_msg_v01));

   req_msg.profile.profile_index = (uint8_t)profile_id;
   req_msg.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
   if(pdp_type != NULL)
   {
     req_msg.pdp_type_valid = 1;
     if(strcasecmp(pdp_type,"IPV4") == 0)
       req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
     else if(strcasecmp(pdp_type,"IPV6") == 0)
       req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
     else if(strcasecmp(pdp_type,"ipv4 and ipv6")==0)
       req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
     else if(strcasecmp(pdp_type,"ipv4v6")==0)
       req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
     else
       req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
   }
   if(apn_name != NULL)
   {
      req_msg.apn_name_valid = 1;
      memcpy(req_msg.apn_name,apn_name,strlen(apn_name));
   }
   rc = qmi_client_send_msg_sync(wds_svc_client,
                                 QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
                                 (void *)&req_msg,
                                 sizeof(req_msg),
                                 (void *)&resp_msg,
                                 sizeof(resp_msg),
                                 15000);
   if(rc != QMI_NO_ERR)
   {
   } 
}

void idev_add_lte_pdp_profile(int *new_profile_id, char *apn_name,char *pdp_type)
{
   wds_create_profile_req_msg_v01      req_msg;
   wds_create_profile_resp_msg_v01     resp_msg;
   int req_len,resp_len;
   qmi_client_error_type rc;
   
   if(wds_svc_client_init == false)
   {
      idev_wds_client_init();
   } 
   *new_profile_id = 0;
   if(wds_svc_client == NULL)
   {
     
     return ;
   }
  
   req_msg.profile_type = WDS_PROFILE_TYPE_3GPP_V01; 
   if(pdp_type != NULL)
   {
      req_msg.pdp_type_valid = 1;
      if(strncmp(pdp_type,"IPV4",strlen("IPV4")) == 0)
        req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
      else if(strncmp(pdp_type,"IPV6",strlen("IPV6")) == 0)
        req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
      else if(strncmp(pdp_type,"IPV4V6",strlen("IPV4V6")) == 0)
        req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
      else
        req_msg.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
   }
   if(apn_name != NULL)
   {
     req_msg.apn_name_valid = 1;
     printf("apn_name:%s\n",apn_name);
     memcpy(req_msg.apn_name,apn_name,strlen(apn_name));
   }
   rc = qmi_client_send_msg_sync(wds_svc_client,
                                 QMI_WDS_CREATE_PROFILE_REQ_V01,
                                 (void*)&req_msg,
                                 sizeof(req_msg),
                                 (void*)&resp_msg,
                                 sizeof(resp_msg),
                                 15000);
   printf("%s,rc=%d,resp_msg.resp.result=%d,error=%d\n",__func__,rc, resp_msg.resp.result,resp_msg.resp.error);
   if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
   {
      printf("resp_msg.profile.profile_index:%d\n",resp_msg.profile.profile_index);
      *new_profile_id =(int)resp_msg.profile.profile_index;
   }
   printf("%s,new_profile_id=%d\n",__func__,*new_profile_id);
}


void idev_enable_lte_pdp_profile(int profile_id)
{
    wds_modify_profile_settings_req_msg_v01   req_msg;
    wds_modify_profile_settings_resp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;
    bool enable;

    if(wds_svc_client_init == false)
    {
        idev_wds_client_init();
    } 
   
    if(wds_svc_client == NULL)
    {
        
        return ;
    }
 
    idev_get_lte_pdp_profile(profile_id, NULL,NULL,&enable);
    
 
    memset(&req_msg, 0, sizeof(wds_modify_profile_settings_req_msg_v01));
    memset(&resp_msg,0, sizeof(wds_modify_profile_settings_resp_msg_v01));
  
    req_msg.profile.profile_index = (uint8_t)profile_id;
    req_msg.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    req_msg.apn_disabled_flag_valid = 1;
    req_msg.apn_disabled_flag = FALSE;

    rc = qmi_client_send_msg_sync(
                                  wds_svc_client,
                                  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
    if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
    {
        
	    return;
    }

    
    if (resp_msg.extended_error_code_valid) {
	    
    }
    
    return;
}


void idev_disable_lte_pdp_profile(int profile_id)
{
    wds_modify_profile_settings_req_msg_v01   req_msg;
    wds_modify_profile_settings_resp_msg_v01  resp_msg;
    int req_len,resp_len;
    qmi_client_error_type rc;
    bool enable;

    if(wds_svc_client_init == false)
    {
        idev_wds_client_init();
    } 
   
    if(wds_svc_client == NULL)
    {
        
        return ;
    }

    idev_get_lte_pdp_profile(profile_id, NULL,NULL,&enable);
    

    memset(&req_msg, 0, sizeof(wds_modify_profile_settings_req_msg_v01));
    memset(&resp_msg,0, sizeof(wds_modify_profile_settings_resp_msg_v01));
  
    req_msg.profile.profile_index = (uint8_t)profile_id;
    req_msg.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    req_msg.apn_disabled_flag_valid = 1;
    req_msg.apn_disabled_flag = TRUE;

    rc = qmi_client_send_msg_sync(wds_svc_client,
                                  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
    if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
    {
        
	    return;
    }

    
    if (resp_msg.extended_error_code_valid) {
	    
    }
    
    return;
}


int idev_nas_client_init()
{
  qmi_client_error_type rc;
  qmi_service_info      info;
  unsigned int          num_entries = 1;
  unsigned int          num_services;

  nas_svc_obj_p = nas_get_service_object_v01();
  if(nas_svc_obj_p == NULL)
  {
     
     return -1;
  }
  rc = qmi_client_notifier_init(nas_svc_obj_p,
                                &nas_notifier_os_params,
                                &nas_notifier);
  if(rc != QMI_NO_ERR)
  {
     
     return -1;
  }

  QMI_CCI_OS_SIGNAL_WAIT(&nas_notifier_os_params, 3000);
  if(nas_notifier_os_params.timed_out)
  {
     
     return -1;
  }
  rc = qmi_client_get_service_list(nas_svc_obj_p,
                                   &info,
                                   &num_entries,
                                   &num_services);
  if(rc != QMI_NO_ERR)
  {
     
     qmi_client_release(nas_notifier);
     return -1;
  }
  rc = qmi_client_init(&info,
                       nas_svc_obj_p,
                       NULL,
                       NULL,
                       &nas_os_params,
                       &nas_svc_client);
  if(rc != QMI_NO_ERR)
  {
     
     qmi_client_release(nas_notifier);
     return -1;
  }
  nas_svc_client_init = true;
  return 0;
}


void idev_detach_ps(void)
{
#if 0
    nas_initiate_attach_req_msg_v01   req_msg;
    nas_initiate_attach_resp_msg_v01  resp_msg;
#else
    nas_set_system_selection_preference_req_msg_v01   req_msg;
    nas_set_system_selection_preference_resp_msg_v01  resp_msg;
#endif
    
    int req_len,resp_len;
    qmi_client_error_type rc;
    bool enable;

    if(!nas_svc_client_init)
    {
        idev_nas_client_init();
    } 
   
    if(nas_svc_client == NULL)
    {
        
        return ;
    }

#if 0
    memset(&req_msg, 0, sizeof(nas_initiate_attach_req_msg_v01));
    memset(&resp_msg,0, sizeof(nas_initiate_attach_resp_msg_v01));

 
    req_msg.ps_attach_action_valid = 1;
    req_msg.ps_attach_action = NAS_PS_ACTION_DETACH_V01;

    rc = qmi_client_send_msg_sync(
                                  nas_svc_client,
                                  QMI_NAS_INITIATE_ATTACH_REQ_MSG_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
  #else
    memset(&req_msg, 0, sizeof(nas_set_system_selection_preference_req_msg_v01));
    memset(&resp_msg,0, sizeof(nas_set_system_selection_preference_resp_msg_v01));

  
    req_msg.change_duration_valid = 1;
    req_msg.change_duration = NAS_PERMANENT_V01;

    req_msg.srv_domain_pref_valid = 1;
    req_msg.srv_domain_pref = QMI_SRV_DOMAIN_PREF_CS_ONLY_V01;

    rc = qmi_client_send_msg_sync(
                                  nas_svc_client,
                                  QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
  #endif
    if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
    {
        
	    return;
    }

    
    return;
}


void idev_attach_ps(void)
{
#if 0
    nas_initiate_attach_req_msg_v01   req_msg;
    nas_initiate_attach_resp_msg_v01  resp_msg;
#else
    nas_set_system_selection_preference_req_msg_v01   req_msg;
    nas_set_system_selection_preference_resp_msg_v01  resp_msg;
#endif
    
    int req_len,resp_len;
    qmi_client_error_type rc;
    bool enable;

    if(!nas_svc_client_init)
    {
        idev_nas_client_init();
    } 
   
    if(nas_svc_client == NULL)
    {
        
        return ;
    }

#if 0
    memset(&req_msg, 0, sizeof(nas_initiate_attach_req_msg_v01));
    memset(&resp_msg,0, sizeof(nas_initiate_attach_resp_msg_v01));

 
    req_msg.ps_attach_action_valid = 1;
    req_msg.ps_attach_action = NAS_PS_ACTION_ATTACH_V01;

    rc = qmi_client_send_msg_sync(
                                  nas_svc_client,
                                  QMI_NAS_INITIATE_ATTACH_REQ_MSG_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
#else
    memset(&req_msg, 0, sizeof(nas_set_system_selection_preference_req_msg_v01));
    memset(&resp_msg,0, sizeof(nas_set_system_selection_preference_resp_msg_v01));

  
    req_msg.change_duration_valid = 1;
    req_msg.change_duration = NAS_PERMANENT_V01;

    req_msg.srv_domain_pref_valid = 1;
    req_msg.srv_domain_pref = QMI_SRV_DOMAIN_PREF_CS_PS_V01;

    rc = qmi_client_send_msg_sync(
                                  nas_svc_client,
                                  QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
                                  (void *)&req_msg,
                                  sizeof(req_msg),
                                  (void *)&resp_msg,
                                  sizeof(resp_msg),
                                  15000
                                  );
#endif
    if(rc == QMI_NO_ERR && resp_msg.resp.result == QMI_RESULT_SUCCESS_V01)
    {
        
        return;
    }

    
    return;
}

