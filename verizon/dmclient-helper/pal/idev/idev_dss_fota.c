#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "idev_dss_fota.h"

#define MAX_PROG_ARGS 18

#define DEFAULT_TIME_WAIT     0

#define DEFAULT_TECH  DSS_FOTA_TECH_UMTS

typedef struct {
    char * tech_name;
    int    tech_type;
} dss_fota_tech_name_to_type_map_t;

const dss_fota_tech_name_to_type_map_t dss_fota_tech_name_to_type_map_arr[] = {
    { "umts", DSS_FOTA_TECH_UMTS},
    { "gprs", DSS_FOTA_TECH_UMTS},
    { "cdma", DSS_FOTA_TECH_CDMA},
    { "1x",   DSS_FOTA_TECH_CDMA}
};

const char * Test_id_str;
const char * Test_name_str;
const char * Test_desc_str;
int          Test_tech;


struct {
    FILE * log_fp;
    int    bufsz;
    int    srvr_port;
    const char * srvr_ipaddr;
    int    time_wait;
    const char * master_script;
    const char * command;
    int    tech;
} dss_fota_cfg;

dss_fota_prog_args_template_t Prog_args_template[] = {
    { "--logfile", 'l', DSS_FOTA_ARG_STR},      /* log file name */
    { "--help", 'h', DSS_FOTA_ARG_VOID},        /* print help */
    { "--name", 'n', DSS_FOTA_ARG_VOID},        /* print test name */
    { "--desc", 'd', DSS_FOTA_ARG_VOID},        /* print test description */
    { "--bufsize", 'b', DSS_FOTA_ARG_INT},      /* buf/burst size */
    { "--datafile", 'f', DSS_FOTA_ARG_STR},     /* data file name */
    { "--serverip", 'i', DSS_FOTA_ARG_STR},     /* server ip addr */
    { "--serverport", 'p', DSS_FOTA_ARG_INT},   /* server port number */
    { "--timewait", 'w', DSS_FOTA_ARG_INT},     /* time to wait in ms */
    { "--udp", 'u', DSS_FOTA_ARG_VOID},         /* peer test app mode udp */
    { "--tcp", 't', DSS_FOTA_ARG_VOID},         /* peer test app mode tcp */
    { "--client", 'c', DSS_FOTA_ARG_VOID},      /* peer test app mode client */
    { "--master-script", 'm', DSS_FOTA_ARG_STR},/* master cmd script to use */
    { "--command", 'C', DSS_FOTA_ARG_STR},      /* command to send */
    { "--profile", 'k', DSS_FOTA_ARG_INT},      /* pdp profile number to use */
    { "--daemon", '-', DSS_FOTA_ARG_VOID},      /* run as daemon */
    { "--tech", 'r', DSS_FOTA_ARG_STR},         /* radio technology type */
    { "--family", 'v', DSS_FOTA_ARG_INT}        /* IP family version V4/V6*/
};

dss_fota_prog_args_t Prog_args[MAX_PROG_ARGS];
int Prog_args_size;

pthread_mutex_t Net_mutx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  Net_cond = PTHREAD_COND_INITIALIZER;
volatile sint15 Net_mutx_dss_nh;
volatile int    Net_ev_pending;

pthread_mutex_t Sock_mutx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  Sock_cond = PTHREAD_COND_INITIALIZER;
volatile sint15 Sock_mutx_dss_nh;
volatile int    Sock_ev_pending;

FILE *
dss_fota_cfg_get_log_fp (void)
{
    return dss_fota_cfg.log_fp;
}

const char *
dss_fota_prog_args_get_val_str (const dss_fota_prog_args_t * arg_ptr)
{
    ds_assert(arg_ptr);
    ds_assert(arg_ptr->type == DSS_FOTA_ARG_STR);
    ds_assert(arg_ptr->val.str_val);

    return arg_ptr->val.str_val;
}

int
dss_fota_prog_args_get_val_int (const dss_fota_prog_args_t * arg_ptr)
{
    ds_assert(arg_ptr);
    ds_assert(arg_ptr->type == DSS_FOTA_ARG_INT);

    return arg_ptr->val.int_val;
}

const dss_fota_prog_args_t *
dss_fota_find_prog_args
(
    char short_name,
    const char * argname,
    const dss_fota_prog_args_t * arg_arr,
    int   arg_arr_sz
)
{
    const dss_fota_prog_args_t * arg_ptr;
    const dss_fota_prog_args_t * ret_ptr = NULL;
    int i;

    for (i = 0; i < arg_arr_sz; ++i) {
        arg_ptr = arg_arr + i;
        if (arg_ptr->name == NULL) {
            break;
        }
        if (short_name == '-') {
            ds_assert(argname);
            if (strcmp(argname, arg_ptr->name) == 0) {
                ret_ptr = arg_ptr;
                break;
            }
        } else {
            if (short_name == arg_ptr->short_name) {
                ret_ptr = arg_ptr;
                break;
            }
        }
    }

    return ret_ptr;
}

const dss_fota_prog_args_template_t *
dss_fota_find_prog_args_template
(
    const char * argname,
    const dss_fota_prog_args_template_t * arg_template_arr,
    int arg_template_arr_sz
)
{
    int i;
    const dss_fota_prog_args_template_t * ret_ptr = NULL;
    const dss_fota_prog_args_template_t * tmp_ptr;
    char short_name = '-';


    if ((strlen(argname) == 2) && (argname[0] == '-')) {
        short_name = argname[1];
    }

    for (i = 0; i < arg_template_arr_sz; ++i) {
        tmp_ptr = arg_template_arr + i;
        if (short_name != '-') {
            if (short_name == tmp_ptr->short_name) {
                ret_ptr = tmp_ptr;
                break;
            }
        } else {
            if (strcmp(argname, tmp_ptr->name) == 0) {
                ret_ptr = tmp_ptr;
                break;
            }
        }
    }

    return ret_ptr;
}

dss_fota_arg_t
dss_fota_store_prog_arg
(
    dss_fota_prog_args_t * arg_arr,
    int arg_arr_sz,
    int arr_index,
    int argc,
    char ** argv,
    int arg_index,
    const dss_fota_prog_args_template_t * template_ptr
)
{
    dss_fota_prog_args_t * arg_ptr;
    char * int_val_chk = NULL;

    if (arg_index >= arg_arr_sz) {
        return -1;
    }

    arg_ptr = arg_arr + arr_index;

    if (template_ptr->type != DSS_FOTA_ARG_VOID) {
        if (arg_index >= argc) {
            return -1;
        }
    }

    arg_ptr->name = argv[arg_index];
    arg_ptr->str_val = argv[arg_index+1];
    arg_ptr->type = template_ptr->type;
    arg_ptr->short_name = template_ptr->short_name;

    switch (template_ptr->type) {
    case DSS_FOTA_ARG_STR:
        arg_ptr->val.str_val = arg_ptr->str_val;
        break;

    case DSS_FOTA_ARG_INT: {
        int int_val;

        int_val = strtol(arg_ptr->str_val, &int_val_chk, 0);

        /*Make sure we convert the whole string successfully*/
        if ((int_val_chk != NULL) && (*int_val_chk != '\0')) {
            idev_log( "strtol conversion of arg_val:%s "
                      "unsuccessful", arg_ptr->str_val);

            return -1;
        } else {
            arg_ptr->val.int_val = int_val;
        }
    }
    break;

    default:
        break;
    }

    return template_ptr->type;
}

int
dss_fota_parse_prog_args
(
    int argc,
    char ** argv,
    const dss_fota_prog_args_template_t * arg_template_arr,
    int   arg_template_arr_sz,
    dss_fota_prog_args_t * arg_arr,
    int   arg_arr_sz
)
{
    int i;
    int rval = -1;
    const dss_fota_prog_args_template_t * template_ptr;
    int arg_index = 0;
    dss_fota_arg_t arg_type;

    for (i = 1; i < argc; ++i) {
        template_ptr = dss_fota_find_prog_args_template
                       (
                           argv[i],
                           arg_template_arr,
                           arg_template_arr_sz
                       );

        if (!template_ptr) {
            idev_log("Unknown arg %s specified. Ignoring..\n", argv[i]);
            continue;
        }

        if (arg_index > arg_arr_sz) {
            idev_log("Too many args specified.\n");
            goto error;
        }
        arg_type = dss_fota_store_prog_arg (
                       arg_arr,
                       arg_arr_sz,
                       arg_index++,
                       argc,
                       argv,
                       i,
                       template_ptr
                   );

        if (arg_type > DSS_FOTA_ARG_VOID) {
            idev_log("Error parsing arg %s..\n", argv[i]);
            goto error;
        } else if (arg_type != DSS_FOTA_ARG_VOID) {
            ++i;
        }
    }

    rval = 0;

error:
    return rval;
}

static int
dss_fota_get_tech_type_from_name (const char * tech_str)
{
    int tech_type = DEFAULT_TECH;
    int i;
    const dss_fota_tech_name_to_type_map_t * map_entry;

    for (i = 0; i < (int)ARR_SIZ(dss_fota_tech_name_to_type_map_arr); ++i) {
        map_entry = dss_fota_tech_name_to_type_map_arr + i;

        if (strcasecmp(tech_str, map_entry->tech_name) == 0) {
            tech_type = map_entry->tech_type;
            break;
        }
    }

    return tech_type;
}

int
dss_fota_tech_get (void)
{
    return dss_fota_cfg.tech;
}

dss_iface_name_enum_type
dss_fota_get_if_name_for_tech (int tech)
{
    dss_iface_name_enum_type if_name = (dss_iface_name_enum_type)0;

    switch (tech) {
    case DSS_FOTA_TECH_UMTS:
        if_name = DSS_IFACE_UMTS;
        break;
    case DSS_FOTA_TECH_CDMA:
        if_name = DSS_IFACE_CDMA_SN;
        break;
    default:
        break;
    }

    return if_name;
}

static __inline__ int
dss_fota_get_bufsize (void)
{
    return dss_fota_cfg.bufsz;
}

static __inline__ FILE *
dss_fota_open_data_file (FILE * def_fp, const char * mode)
{
    const dss_fota_prog_args_t * arg_ptr;
    FILE * fp;
    const char * data_f_name;

    if ((arg_ptr = dss_fota_find_prog_args('f', NULL, &Prog_args[0], ARR_SIZ(Prog_args))) == NULL) {
        fp = def_fp;
    } else {
        data_f_name = dss_fota_prog_args_get_val_str(arg_ptr);
        fp = fopen(data_f_name, mode);

        if (fp == NULL) {
            fp = def_fp;
        }
    }

    return fp;
}

static __inline__ int
dss_fota_get_srvr_port (void)
{
    return dss_fota_cfg.srvr_port;
}

static __inline__ const char *
dss_fota_get_srvr_ipaddr (void)
{
    return dss_fota_cfg.srvr_ipaddr;
}

static __inline__ int
dss_fota_get_time_wait (void)
{
    return dss_fota_cfg.time_wait;
}

static __inline__ const char *
dss_fota_get_master_script (void)
{
    return dss_fota_cfg.master_script;
}

static __inline__ const char *
dss_fota_get_command (void)
{
    return dss_fota_cfg.command;
}

static void
dss_fota_set_sockaddr (struct sockaddr_in * saddr, const char * ipaddr, int port)
{
    memset(saddr, 0, sizeof(struct sockaddr_in));
    saddr->sin_family = AF_INET;
    saddr->sin_port = port;

    if (inet_aton(ipaddr, &saddr->sin_addr) == 0) {
        dss_fota_log("error in inet_aton call!\n");
        ds_assert(0);
    }
    return;
}

static void
dss_fota_get_srvr_addr (struct sockaddr_in * saddr)
{
    int port;
    const char * ipaddr;

    port = dss_fota_get_srvr_port();
    ipaddr = dss_fota_get_srvr_ipaddr();

    dss_fota_set_sockaddr(saddr, ipaddr, port);
    return;
}

int
dss_fota_wait_for_net_event (sint15 dss_nh)
{
    sint15 dss_nh_saved;

    if (pthread_mutex_lock(&Net_mutx) != 0) {
        ds_assert(0);
    }

    Net_mutx_dss_nh = dss_nh;

    if (!Net_ev_pending) {
        if (pthread_cond_wait(&Net_cond, &Net_mutx) != 0) {
            ds_assert(0);
        }
    }

    Net_ev_pending = 0;
    dss_nh_saved = Net_mutx_dss_nh;

    if (pthread_mutex_unlock(&Net_mutx) != 0) {
        ds_assert(0);
    }

    if (dss_nh_saved != dss_nh) {
        return -1;
    }

    return 0;
}

int
dss_fota_wait_for_sock_event (sint15 dss_nh)
{
    sint15 dss_nh_saved;

    if (pthread_mutex_lock(&Sock_mutx) != 0) {
        ds_assert(0);
    }

    Sock_mutx_dss_nh = dss_nh;

    if (!Sock_ev_pending) {
        if (pthread_cond_wait(&Sock_cond, &Sock_mutx) != 0) {
            ds_assert(0);
        }
    }

    Sock_ev_pending = 0;
    dss_nh_saved = Sock_mutx_dss_nh;

    if (pthread_mutex_unlock(&Sock_mutx) != 0) {
        ds_assert(0);
    }

    if (dss_nh_saved != dss_nh) {
        return -1;
    }

    return 0;
}

void dss_fota_dss_net_cb_fcn
(
    sint15            dss_nethandle,                       /* Application id */
    dss_iface_id_type iface_id,                    /* Interfcae id structure */
    sint15            dss_errno, /* type of network error, ENETISCONN, ENETNONET.*/
    void            * net_cb_user_data               /* Call back User data  */
)
{
    dss_fota_log("In dss_fota_dss_net_cb_fcn: nethandle = %d, iface_id = %ld, errno = %d, net_cb_user_data = %d\n",
                 dss_nethandle, iface_id, dss_errno, (int) net_cb_user_data);

    if (pthread_mutex_lock(&Net_mutx) < 0) {
        ds_assert(0);
    }

    Net_mutx_dss_nh = dss_nethandle;
    Net_ev_pending = 1;

    if (pthread_cond_signal(&Net_cond) < 0) {
        ds_assert(0);
    }

    if (pthread_mutex_unlock(&Net_mutx) < 0) {
        ds_assert(0);
    }

    return;
}

void dss_fota_dss_sock_cb_fcn
(
    sint15 dss_nethandle,                                  /* Application id */
    sint15 sockfd,                                      /* socket descriptor */
    uint32 event_mask,                                     /* Event occurred */
    void * sock_cb_user_data       /* User data specfied during registration */
)
{
    dss_fota_log("In dss_fota_dss_sock_cb_fcn: nethandle = %d, sockfd = %d, event_mask = %ld, sock_cb_user_data = %d\n",
                 dss_nethandle, sockfd, event_mask, (int) sock_cb_user_data);

    if (pthread_mutex_lock(&Sock_mutx) < 0) {
        ds_assert(0);
    }

    Sock_mutx_dss_nh = dss_nethandle;
    Sock_ev_pending = 1;

    if (pthread_cond_signal(&Sock_cond) < 0) {
        ds_assert(0);
    }

    if (pthread_mutex_unlock(&Sock_mutx) < 0) {
        ds_assert(0);
    }

    return;
}

int
dss_fota_dss_init_net_policy_info (dss_net_policy_info_type * net_policy)
{
    dss_init_net_policy_info(net_policy);

    return 0;
}

int
dss_fota_dsnet_set_policy
(
    sint15 dss_nh,
    dss_net_policy_info_type * net_policy,
    sint15 dss_status_val,
    sint15 dss_errno_val
)
{
    sint15 dss_status;
    sint15 dss_errno;

    dss_status = dsnet_set_policy(dss_nh, net_policy, &dss_errno);

    if (dss_status != dss_status_val) {
        return -1;
    }

    if (dss_status == DSS_SUCCESS) {
        return 0;
    }

    if (dss_errno_val == dss_errno) {
        return 0;
    }

    return -1;
}

int
dss_fota_dsnet_get_handle (dss_net_policy_info_type * net_policy, sint15 * dss_nh)
{
    sint15 nh;
    sint15 dss_errno;

    nh = dsnet_get_handle(&dss_fota_dss_net_cb_fcn, (void *)0,
                          &dss_fota_dss_sock_cb_fcn, (void *)0,
                          net_policy, &dss_errno);

    if (nh == DSS_ERROR) {
        return -1;
    }

    *dss_nh = nh;

    return 0;
}

int
dss_fota_ex_dsnet_get_handle
(
    dss_net_policy_info_type * net_policy,
    sint15 * dss_nh,
    sint15 dss_status_val,
    sint15 dss_errno_val
)
{
    sint15 nh;
    sint15 dss_errno;

    nh = dsnet_get_handle(&dss_fota_dss_net_cb_fcn, (void *)0,
                          &dss_fota_dss_sock_cb_fcn, (void *)0,
                          net_policy, &dss_errno);

    *dss_nh = nh;

    if (dss_status_val == DSS_SUCCESS) {
        if (nh != DSS_ERROR) {
            return 0;
        } else {
            return -1;
        }
    }

    if (nh == DSS_ERROR) {
        if (dss_errno_val == dss_errno) {
            return 0;
        } else {
            return -1;
        }
    }

    return -1;
}

int
dss_fota_dsnet_release_handle (sint15 dss_nh)
{
    sint15 dss_errno;
    int rval;

    rval = dsnet_release_handle(dss_nh, &dss_errno);

    if (rval == DSS_ERROR) {
        return -1;
    }

    return 0;
}

#ifndef FEATURE_DS_NO_DCM

int
dss_fota_dsnet_start (sint15 dss_nh)
{
    int rval;
    sint15 dss_errno;

    rval = dsnet_start(dss_nh, &dss_errno);

    if (rval == DSS_SUCCESS) {
        return 0;
    }

    if (dss_errno != DS_EWOULDBLOCK) {
        return -1;
    }

    return 0;
}

int
dss_fota_dss_netstatus (sint15 dss_nh, sint15 dss_errno_val)
{
    sint15 dss_errno;
    int rval;

    rval = dss_netstatus(dss_nh, &dss_errno);

    if (rval != DSS_ERROR) {
        return -1;
    }

    if (dss_errno != dss_errno_val) {
        return -1;
    }

    return 0;
}

int
dss_fota_dss_get_iface_id (sint15 dss_nh, dss_iface_id_type * if_id, sint15 dss_status_val)
{
    dss_iface_id_type iface_id;

    iface_id = dss_get_iface_id(dss_nh);

    if (if_id != NULL) {
        *if_id = iface_id;
    }

    if (dss_status_val == DSS_SUCCESS) {
        if (iface_id != DSS_IFACE_INVALID_ID) {
            return 0;
        }
    } else {
        if (iface_id == DSS_IFACE_INVALID_ID) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dss_get_iface_id_by_policy
(
    const dss_net_policy_info_type * net_policy,
    dss_iface_id_type * if_id,
    sint15 dss_status_val,
    sint15 dss_errno_val
)
{
    dss_iface_id_type iface_id;
    sint15 dss_errno;

    iface_id = dss_get_iface_id_by_policy(*net_policy, &dss_errno);

    if (if_id) {
        *if_id = iface_id;
    }

    if (dss_status_val == DSS_SUCCESS) {
        if (iface_id != DSS_IFACE_INVALID_ID) {
            return 0;
        }
    } else {
        if ((iface_id == DSS_IFACE_INVALID_ID) &&
                (dss_errno_val == dss_errno)) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dss_iface_ioctl_get_iface_name
(
    dss_iface_id_type        iface_id,
    dss_iface_name_enum_type iface_name,
    sint15                   dss_status_val,
    sint15                   dss_errno_val
)
{
    sint15 dss_errno;
    int rval;
    dss_iface_name_enum_type if_name;

    rval = dss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_GET_IFACE_NAME, &if_name, &dss_errno);
    dss_fota_log("if_name:%d, iface_name:%d", if_name, (int)iface_name);
    if ((rval == 0) && (dss_status_val == DSS_SUCCESS)) {
        if (iface_name == if_name) {
            return 0;
        }
    } else if ((rval != 0) && (dss_status_val == DSS_ERROR)) {
        if (dss_errno_val == dss_errno) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dss_iface_ioctl_get_state
(
    dss_iface_id_type           iface_id,
    dss_iface_ioctl_state_type  iface_state,
    sint15                      dss_status_val,
    sint15                      dss_errno_val
)
{
    sint15 dss_errno;
    int rval;
    dss_iface_ioctl_state_type if_state;

    rval = dss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_GET_STATE, &if_state, &dss_errno);

    if ((rval == 0) && (dss_status_val == DSS_SUCCESS)) {
        if (iface_state == if_state) {
            return 0;
        }
    } else if ((rval != 0) && (dss_status_val == DSS_ERROR)) {
        if (dss_errno_val == dss_errno) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dss_iface_ioctl_reg_event_cb
(
    dss_iface_id_type            iface_id,
    dss_iface_ioctl_ev_cb_type * iface_ev_cb,
    sint15                       dss_status_val,
    sint15                       dss_errno_val
)
{
    sint15 dss_errno;
    int rval;

    rval = dss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_REG_EVENT_CB, iface_ev_cb, &dss_errno);

    if ((rval == 0) && (dss_status_val == DSS_SUCCESS)) {
        return 0;
    } else if ((rval != 0) && (dss_status_val == DSS_ERROR)) {
        if (dss_errno_val == dss_errno) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dss_iface_ioctl_dereg_event_cb
(
    dss_iface_id_type            iface_id,
    dss_iface_ioctl_ev_cb_type * iface_ev_cb,
    sint15                       dss_status_val,
    sint15                       dss_errno_val
)
{
    sint15 dss_errno;
    int rval;

    rval = dss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_DEREG_EVENT_CB, iface_ev_cb, &dss_errno);

    if ((rval == 0) && (dss_status_val == DSS_SUCCESS)) {
        return 0;
    } else if ((rval != 0) && (dss_status_val == DSS_ERROR)) {
        if (dss_errno_val == dss_errno) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_dsnet_start_sync (sint15 dss_nh, int dss_status_val, sint15 dss_errno_val)
{
    int dss_status;
    sint15 dss_errno;

    dss_status = dsnet_start(dss_nh, &dss_errno);

    if ((dss_status == DSS_SUCCESS) || (dss_errno != DS_EWOULDBLOCK)) {
        goto done;
    }

    if (dss_status_val == dss_status) {
        goto done;
    }

    while (1) {
        if (dss_fota_wait_for_net_event(dss_nh) < 0) {
            ds_assert(0);
        }
        dss_status = dss_netstatus(dss_nh, &dss_errno);

        if (dss_status != DSS_ERROR) {
            goto error;
        }

        if ((dss_status_val == DSS_SUCCESS) && (dss_errno == DS_ENETINPROGRESS)) {
            continue;
        } else {
            if (dss_errno == DS_ENETISCONN) {
                dss_status = DSS_SUCCESS;
            }
            break;
        }
    }

done:

    if ((dss_status == dss_status_val) &&
            ((dss_status == DSS_SUCCESS) || (dss_errno_val == dss_errno))) {
        return 0;
    }

error:
    return -1;
}

int
dss_fota_dsnet_stop_sync (sint15 dss_nh, int dss_status_val, sint15 dss_errno_val)
{
    int dss_status;
    sint15 dss_errno;

    dss_status = dsnet_stop(dss_nh, &dss_errno);

    if ((dss_status == DSS_SUCCESS) || (dss_errno != DS_EWOULDBLOCK)) {
        goto done;
    }

    if (dss_status_val == dss_status) {
        goto done;
    }

    while (1) {
        if (dss_fota_wait_for_net_event(dss_nh) < 0) {
            ds_assert(0);
        }
        dss_status = dss_netstatus(dss_nh, &dss_errno);

        if (dss_status != DSS_ERROR) {
            goto error;
        }

        if ((dss_status_val == DSS_SUCCESS) && (dss_errno == DS_ENETCLOSEINPROGRESS)) {
            continue;
        } else {
            if (dss_errno == DS_ENETNONET) {
                dss_status = DSS_SUCCESS;
            }
            break;
        }
    }

done:

    if ((dss_status == dss_status_val) &&
            ((dss_status == DSS_SUCCESS) || (dss_errno_val == dss_errno))) {
        return 0;
    }

error:
    return -1;
}

int
dss_fota_match_nh_ifaces(sint15 dss_nh_1, sint15 dss_nh_2, sint15 dss_status_val)
{
    dss_iface_id_type if_1, if_2;

    if_1 = dss_get_iface_id(dss_nh_1);
    if_2 = dss_get_iface_id(dss_nh_2);

    if ((if_1 == DSS_IFACE_INVALID_ID) || (if_2 == DSS_IFACE_INVALID_ID)) {
        return -1;
    }

    if (dss_status_val == DSS_SUCCESS) {
        if (if_1 == if_2) {
            return 0;
        }
    } else {
        if (if_1 != if_2) {
            return 0;
        }
    }

    return -1;
}

int
dss_fota_match_nh_ifaces_3
(
    sint15 dss_nh_1,
    sint15 dss_nh_2,
    sint15 dss_nh_3,
    sint15 dss_status_val
)
{
    dss_iface_id_type if_1, if_2, if_3;
    sint15 all_same = DSS_ERROR;

    if_1 = dss_get_iface_id(dss_nh_1);
    if_2 = dss_get_iface_id(dss_nh_2);
    if_3 = dss_get_iface_id(dss_nh_3);

    if ((if_1 == DSS_IFACE_INVALID_ID) ||
            (if_2 == DSS_IFACE_INVALID_ID) ||
            (if_3 == DSS_IFACE_INVALID_ID)) {
        return -1;
    }

    if ((if_1 == if_2) && (if_2 == if_3) && (if_1 == if_3)) {
        all_same = DSS_SUCCESS;
    }

    if (dss_status_val == all_same) {
        return 0;
    }

    return -1;
}

#endif /* !FEATURE_DS_NO_DCM */


