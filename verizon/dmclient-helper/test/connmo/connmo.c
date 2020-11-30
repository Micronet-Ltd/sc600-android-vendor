/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pal.h>

#define MAX_DATA_SIZE 127
#define PAL_MODULE "pal_network"
#define CMD_SET "set"
#define CMD_GET "get"
#define SECTION_APN "apn"
#define SECTION_IMS "ims"

#define PAL_LIB_PATH "/vendor/lib/drm/libpal.so"

static char* __s_argv0 = "./connmo";

#define SMS_FORMAT "smsformat"

const static char* __s_apn_selections[] = {
    "class1", "class2", "class3", "class4", "class6", "sms_over_ip_network",
    "domain", "sipt1", SMS_FORMAT
};

const static char* __s_apn_names[] = {
    "id", "name", "ip", "state", "enable", "disabled", "indication"
};

const static char* __s_ims_values[] = {
    "True", "False", "3GPP", "3GPP2"
};

;void getNewdataBuffer(data_buffer_t* pDBuffer, unsigned int size) {
    pDBuffer->data = (char*) malloc(size);
    pDBuffer->size = size;
    memset(pDBuffer->data, 0, pDBuffer->size);
}

    // Releases PAL data buffer
void freedataBuffer(data_buffer_t* pDBuffer) {
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        free(pDBuffer->data);
        pDBuffer->data == NULL;
        pDBuffer->size = 0;
    }
}

    // Clears content of PAL data bufer
void cleardataBuffer(data_buffer_t* pDBuffer) {
    if((NULL != pDBuffer->data) && (0 != pDBuffer->size)) {
        memset(pDBuffer->data, 0, pDBuffer->size);
    }
}

    // Wraps given string to PAL data buffer
void wrapdataBuffer(char* strToWrap, data_buffer_t* pDBuffer) {
    int size = strlen(strToWrap);
    pDBuffer->data =(char*) malloc(size+1);
    pDBuffer->size = size + 1;
    memset(pDBuffer->data, 0, pDBuffer->size);
    strncpy(pDBuffer->data, strToWrap, size);
}

void test_show_help() {
    int idx = 0;
    int arr_size = 0;
    printf("Usage:\n" );
    printf("%s timers\n", __s_argv0);
    printf("  OR\n");
    printf("%s set <section> <selection> <name> <value>\n", __s_argv0);
    printf("%s get <section> <selection> <name>\n", __s_argv0 );
    printf("Possible values for \"section\":\n");
    printf("  " SECTION_APN "\n");
    printf("  " SECTION_IMS "\n");
    printf("Possible values for \"selection\":\n");
    arr_size = sizeof(__s_apn_selections) / sizeof(char*);
    for (idx = 0; idx < arr_size; idx ++) {
        printf("  %s\n", __s_apn_selections[idx]);
    }
    printf("Possible values for \"name\":\n");
    arr_size = sizeof(__s_apn_names) / sizeof(char*);
    for (idx = 0; idx < arr_size; idx ++) {
        printf("  %s\n", __s_apn_names[idx]);
    }
    printf("Possible values for \"value\" for \"ims\" section:\n");
    arr_size = sizeof(__s_ims_values) / sizeof(char*);
    for (idx = 0; idx < arr_size; idx ++) {
        printf("  %s\n", __s_ims_values[idx]);
    }
    printf("examples:\n");
    printf("adb shell connmo.test get ims sms_over_ip_network indication\n");
    printf("adb shell connmo.test set ims sms_over_ip_network indication False\n");
    printf("adb shell connmo.test set ims sms_over_ip_network indication True\n");
    // pal_network_ims_domain_get => "pal_network" _ "ims" _ "domain" _ "get"
    printf("adb shell connmo.test get ims domain\n");
    printf("adb shell connmo.test get ims %s\n", SMS_FORMAT);
    printf("adb shell connmo.test set ims %s 3gpP\n", SMS_FORMAT);
    printf("adb shell connmo.test get apn class1 name\n");
}

int test_apn_snprints_function_name(
    char* dst, int max_size,
    int argc, char** argv)
{
    if ((argc > 4) ||
        (
            (argc > 3) &&
            (
                (strcmp("domain", argv[3]) == 0) ||
                (strcmp("sipt1", argv[3]) == 0) ||
                (strcmp(SMS_FORMAT, argv[3]) == 0)
            )
        )) {
        int str_size = 1;
        str_size += strlen(PAL_MODULE) + 1;
        str_size += strlen(argv[2]) + 1;
        str_size += strlen(argv[3]) + 1;
        if (argc > 4) {
            str_size += strlen(argv[4]) + 1;
        }
        str_size += strlen(argv[1]);
        if (str_size < max_size) {
            if (argc > 4 && (strcmp("domain", argv[3]) != 0) &&
                (strcmp(SMS_FORMAT, argv[3]) != 0) &&
                (strcmp("sipt1", argv[3]) != 0)) {
                if ((strcmp("disabled", argv[4]) == 0) ||
                    (strcmp("enable", argv[4]) == 0)) {
                    snprintf(dst, max_size - 1, "%s_%s_%s_%s",
                        PAL_MODULE, argv[2], argv[3], argv[4]);
                } else {
                    snprintf(dst, max_size - 1, "%s_%s_%s_%s_%s",
                        PAL_MODULE, argv[2], argv[3], argv[4], argv[1]);
                }
            } else {
                snprintf(dst, max_size - 1, "%s_%s_%s_%s",
                    PAL_MODULE, argv[2], argv[3], argv[1]);
            }
            return strlen(dst);
        } else {
            return (-1) * str_size;
        }
    } else {
        printf("there are not enough arguments to create function name\n");
        test_show_help();
        return 0;
    }
}

typedef int (*pal_fn) (data_buffer_t*);

int test_set(int argc, char** argv) {
    int rc = -1;
    char function_name[MAX_DATA_SIZE];
    if (test_apn_snprints_function_name(function_name, MAX_DATA_SIZE, argc, argv) < 1) {
        return -1;
    }
    if ((argc < 6) && ((argc < 5) &&
        (strcmp(SMS_FORMAT, argv[3]) != 0) &&
        (strcmp("domain", argv[3]) != 0) &&
        (strcmp("sipt1", argv[3]) != 0)
        )) {
        printf("command \"set\" requires <value>\n");
        test_show_help();
        return -1;
    }
    printf("function name: %s\n", function_name);
    void* dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        return -1;
    }
    pal_fn pal_function = dlsym(dl_handle, function_name);
    if (pal_function != NULL) {
        data_buffer_t ibuff = {
            .data = argv[argc - 1],
            .size = strlen(argv[argc - 1]) + 1
        };
        rc = pal_function(&ibuff);
        printf("%s(&{%d, %s} = %d\n", function_name, ibuff.size, ibuff.data, rc);
    } else {
        printf("could not find function: %s\n", function_name);
    }
    if (dl_handle != NULL) {
        dlclose(dl_handle);
        dl_handle = NULL;
    }
    return rc;
}

int test_get(int argc, char** argv) {
    int rc = -1;
    char function_name[MAX_DATA_SIZE];
    if (test_apn_snprints_function_name(function_name, MAX_DATA_SIZE, argc, argv) < 1) {
        return -1;
    }
    printf("function name: %s\n", function_name);
    void* dl_handle = dlopen(PAL_LIB_PATH, RTLD_LAZY);
    if (dl_handle == NULL) {
        printf("could not load PAL:" PAL_LIB_PATH "\n");
        return -1;
    }
    pal_fn pal_function = dlsym(dl_handle, function_name);
    if (pal_function != NULL) {
        char out_str[MAX_DATA_SIZE];
        memset(out_str, 0, MAX_DATA_SIZE * sizeof(char));
        data_buffer_t obuff = {
            .data = out_str,
            .size = MAX_DATA_SIZE
        };
        rc = pal_function(&obuff);
        printf("%s(&{%d, \"%s\"}) = %d\n", function_name, obuff.size, obuff.data, rc);
    } else {
        printf("could not find function: %s\n", function_name);
    }
    if (dl_handle != NULL) {
        dlclose(dl_handle);
        dl_handle = NULL;
    }
    return rc;
}

#if 0
// disable unused functions

int test_getImsDomain(){
    int reply = 0;
    data_buffer_t dataBuffer;
    // Get ImsDomain (normal case)
    getNewdataBuffer(&dataBuffer, MAX_DATA_SIZE);
    reply = pal_network_ims_domain_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        ALOGE("pal_network_ims_domain_get() <normal case> error: %d", reply);
    } else {
        ALOGD("pal_network_ims_domain_get() <normal case> : %s",
        dataBuffer.data);
    }
    freedataBuffer(&dataBuffer);
    reply = 0;

    // Get ImsDomain (NULL buffer)
    reply = pal_network_ims_domain_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        ALOGE("pal_network_ims_domain_get() <NULL buffer> error: %d", reply);
    } else {
        ALOGD("pal_network_ims_domain_get() <NULL buffer> : %s",
        dataBuffer.data);
    }
    reply = 0;

    // Get ImsDomain (small buffer)
    getNewdataBuffer(&dataBuffer, 2);
    reply = pal_network_ims_domain_get(&dataBuffer);
    if(RESULT_SUCCESS != reply) {
        ALOGE("pal_network_ims_domain_get() <small buffer> error: %d", reply);
    } else {
        ALOGD("pal_network_ims_domain_get() <small buffer> : %s",
        dataBuffer.data);
    }
    freedataBuffer(&dataBuffer);
    reply = 0;

        return 0;
}

#endif // 0 disable unused functions

int main(int argc, char** argv)
{
    /**
     * ./connmo set apn class1 name smth_name
     */
    __s_argv0 = argv[0];
    if (argc < 2) {
        test_show_help();
        return -1;
    }
    char* cmd_str = argv[1];
    if (strcmp(cmd_str, CMD_SET) == 0) {
        return test_set(argc, argv);
    } else if (strcmp(cmd_str, CMD_GET) == 0){
        return test_get(argc, argv);
    } else {
        printf("incorrect cmd\n");
        test_show_help();
    }
  return 0;
}
