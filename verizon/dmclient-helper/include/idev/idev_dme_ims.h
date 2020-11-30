#ifndef IDEV_DME_IMS_H
#define IDEV_DMS_IMS_H

#ifdef __cplusplus
extern "C" {
#endif

void idev_modify_ims_sms_configuration(char *sms_format,char *sms_over_ip);

void idev_get_ims_sms_configuration(char *sms_format,char *sms_over_ip);

void idev_get_ims_domain(char *ims_domain);

void idev_modify_ims_domain(char *ims_domain);

void idev_get_ims_sip_configuration(char *sip_t1,char *sip_t2,char *sip_tf);

void idev_modify_ims_sip_configuration(char *sip_t1,char *sip_t2,char *sip_tf);

void idev_get_volte_publish_timer(char *publish_timer);

void idev_get_volte_publish_timer_extened(char *publish_timer_extended);

void idev_get_volte_capabites_cache_expiration(char *capabities_cache_expiration);

void idev_get_volte_availability_cache_expiration(char *availability_cahce_expiration);

void idev_get_volte_capabities_poll_interval(char *capabities_poll_interval);

void idev_get_volte_capab_list_subscription_expiry_timer(char *capab_list_subscription_expiry_timer);

void idev_get_volte_max_subscription_list_entries(char *max_subcription_list_entries);

void idev_get_volte_gzip_flag(char *gzip_flag);

void idev_modify_volte_publish_timer(char *publish_timer);

void idev_modify_volte_publish_timer_extened(char *publish_timer_extended);

void idev_modify_volte_capabites_cache_expiration(char *capabities_cache_expiration);

void idev_modify_volte_availability_cache_expiration(char *availability_cahce_expiration);

void idev_modify_volte_capabities_poll_interval(char *capabities_poll_interval);

void idev_modify_volte_capab_list_subscription_expiry_timer(char *capab_list_subscription_expiry_timer);

void idev_modify_volte_max_subscription_list_entries(char *max_subcription_list_entries);

void idev_modify_volte_gzip_flag(char *gzip_flag);

void idev_get_voip_sip_session_timer(char *session_timer);

void idev_get_voip_min_sip_session_timer(char *min_se);

void idev_get_voip_amrmodeset(char *amrmodeset);

void idev_get_voip_amrwebmodeset(char *amrwebmodeset);

void idev_modify_voip_sip_session_timer(char *session_timer);

void idev_modify_voip_min_sip_session_timer(char *min_se);

void idev_modify_voip_amrmodeset(char *amrmodeset);

void idev_modify_voip_amrwebmodeset(char *amrwebmodeset);

#ifdef __cplusplus
}
#endif
#endif
