#ifndef IDEV_DME_WDS_H
#define IDEV_DME_WDS_H

#ifdef __cplusplus
extern "C" {
#endif
void idev_get_lte_pdp_profile(int profile_id, char *apn_name,char *pdp_type, bool *enable);
void idev_modify_lte_pdp_profile(int profile_id, char *apn_name, char *pdp_type);
void idev_add_lte_pdp_profile(int *new_profile_id, char *apn_name,char *pdp_type);
void idev_enable_lte_pdp_profile(int profile_id);
void idev_disable_lte_pdp_profile(int profile_id);
void idev_detach_ps(void);
void idev_attach_ps(void);
#ifdef __cplusplus
}
#endif

#endif
