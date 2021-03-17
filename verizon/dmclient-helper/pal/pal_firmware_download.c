/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <curl/curl.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <string.h>


#include "pal.h"
#include "pal_fumo_cfg.h"
#include "mo_error.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FUMO_FIRMWARE_DOWNLOAD"

#define MAXSTRINGLEN 4096
#define TEMPFILE "tmp.txt"

int parse_download_descriptor(const char *filename, pal_download_descriptor_t **out);
static int current_progress = 0;
static int save_link_and_name_to_file(const char *url, const char *name);
static int check_download_aborted(char **url, char **name);
static int check_garbage(const char *filename);

/**
 * The libcurl callback gets information about how much data
 * will transfer and has transferred, in number of bytes
 * @param dltotal is the total number of bytes libcurl expects to download in this transfer
 * @param dlnow is the number of bytes downloaded so far
 * @param ultotal is the total number of bytes libcurl expects to upload in this transfer
 * @param ulnow is the number of bytes uploaded so far
 * @return zero value by default any other value will cause libcurl to abort the transfer
 */
int download_firmware_progress_callback(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow)
{
    int percent = (int)((dlnow/dltotal)*100);
    if (current_progress != percent) {
        pal_ft_cb_progress pl_ft_cb_prg = (pal_ft_cb_progress)userdata;
        int rc = pl_ft_cb_prg(NULL, percent, 200);
        if (rc == MO_ERROR_DEVICE_FULL){
            return 1;
        }
        current_progress = percent;
    }
    return 0;
}

/**
 * @brief Perform a downloading package from server
 * @param [in] status, Description for download process.
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS of success
 *         PAL_RC_FRMW_DOWNLOAD_ERROR if status is NULL
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_NULL if status.serverUrl is NULL
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_DOWNLOAD if cannot download firmware package
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR if the information
 *                                                            from manager server is broken
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY if cannot start loading of package with update
 */
int pal_download_firmware(pal_download_status_t *status)
{
    if (NULL == status) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR %s:%d", __FILE__, __LINE__);
        return PAL_RC_FRMW_DOWNLOAD_ERROR;
    }
    if (NULL == status->serverUrl) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_NULL %s:%d", __FILE__, __LINE__);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_NULL;
    }
    int result = 0;

    char *loadedDescriptorName = NULL;//use default name
    pal_download_descriptor_t *pdd = NULL;

    //start ANOTHER download thread
    result = pal_request_download_descriptor(status->serverUrl);
    if (PAL_RC_FRMW_DOWNLOAD_SUCCESS != result){
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_DOWNLOAD %s:%d", __FILE__, __LINE__);
        ALOGE("URL:%s", status->serverUrl);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_DOWNLOAD;
    }

    //perform parsing in THIS thread
    result = parse_download_descriptor(loadedDescriptorName, &pdd);

    if (PAL_RC_FRMW_DOWNLOAD_SUCCESS != result) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR %s:%d", __FILE__, __LINE__);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR;
    }

    if (NULL == pdd || NULL == pdd->objectUri || NULL == pdd->packageName) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR %s:%d", __FILE__, __LINE__);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR;
    }

    // Removing old unnessary update package files
    check_garbage(pdd->packageName);
    save_link_and_name_to_file(pdd->objectUri, pdd->packageName);

    pal_download_options_t* pdo = (pal_download_options_t*)calloc(1, sizeof(pal_download_options_t));
    if (pdo == NULL) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }
    pdo->download_url = pdd->objectUri;
    pdo->total_download_size = pdd->size;
    int ret;
    ret = asprintf(&pdo->out_file, "%s/%s/%s", MO_WORK_PATH, MO_DATA_PFX, pdd->packageName);
    if (ret <= 0) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }
    pdo->download_progress_callback = download_firmware_progress_callback;
    pdo->download_progress_fn = (void*)status->on_progress;
    current_progress = 0;

    int rc = PAL_RC_FRMW_DOWNLOAD_SUCCESS;
    if (0 != (rc = pal_network_download_file(pdo))) {
        if(rc == CURLE_COULDNT_CONNECT || rc == CURLE_COULDNT_RESOLVE_HOST) {
            ALOGE("Download Server is Unavailable error = %d",rc);
            rc = PAL_RC_FRMW_DOWNLOAD_SERVER_UNAVAILABLE;
        }
        else
            rc = PAL_RC_FRMW_DOWNLOAD_ERROR;
    }
    if (rc == PAL_RC_FRMW_DOWNLOAD_SUCCESS)
        if(MO_ERROR_SUCCESS != pal_verify_Crc32(pdo->out_file)){
            rc = PAL_RC_FRMW_DOWNLOAD_ERROR_VERIFICATION_FAILED;
        }
    free(pdo->out_file);
    free(pdo);
    pdo = NULL;

    if (rc != PAL_RC_FRMW_DOWNLOAD_SUCCESS) {
        free_download_descriptor_t(&pdd);
        return rc;
    }

    free_download_descriptor_t(&pdd);
    remove(MO_WORK_PATH "/" MO_DATA_PFX "/" TEMPFILE);

    return rc;
}

/**
 * Cancel download process if exists
 * @param context
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS if success
 */
int pal_download_cancel(const void *context)
{
    (void) context;
    return PAL_RC_FRMW_DOWNLOAD_SUCCESS;
}

//////////////////////////////////////////////////////
/// Utils
//////////////////////////////////////////////////////

/**
 * Launch a routine with downloading of Download Descriptor
 * @param[in] serverUrl URL of OMADM server
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS if success
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_NULL if serverUrl is NULL
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_EMPTY_URL if serverURL is empty
 *         PAL_RC_FRMW_DOWNLOAD_ERROR if request failed
 */
int pal_request_download_descriptor(const char *serverUrl)
{
    if (NULL == serverUrl) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_NULL;
    }
    if (0 >= strlen(serverUrl)) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_EMPTY_URL;
    }
    /** ask server to get download descriptor */
    // first, remove old
    if(0 != remove(MO_WORK_PATH "/" MO_DATA_PFX "/" DOWNLOAD_DESCRIPTOR_FILE_NAME)){
        ALOGE("Cannot find old %s", DOWNLOAD_DESCRIPTOR_FILE_NAME);
    }

    pal_download_options_t* pdo = (pal_download_options_t*)calloc(1, sizeof(pal_download_options_t));
    if (pdo == NULL) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }
    pdo->download_url = strdup(serverUrl);
    int ret;
    ret = asprintf(&pdo->out_file, "%s/%s/%s", MO_WORK_PATH, MO_DATA_PFX, DOWNLOAD_DESCRIPTOR_FILE_NAME);
    if (ret <= 0) {
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }

    ALOGE("[Quectel]pal_network_download_file");

    int rc = PAL_RC_FRMW_DOWNLOAD_SUCCESS;
    if (0 != pal_network_download_file(pdo)) {

        ALOGE("[Quectel]pal_network_download_file pdo");
        rc = PAL_RC_FRMW_DOWNLOAD_ERROR;
    }

    ALOGE("[Quectel]pal_network_download_file rc = %d", rc);
    if(pdo == NULL)
    {
      ALOGE("[Quectel]pal_network_download_file pdo");
    }
     if(pdo->out_file == NULL)
    {
      ALOGE("[Quectel]pal_network_download_file pdo->out_file");
    }
    if(pdo->download_url == NULL)
    {
      ALOGE("[Quectel]pal_network_download_file download_url");
    }
    free(pdo->download_url);
    free(pdo->out_file);
    free(pdo);
    pdo = NULL;
    ALOGE("[Quectel]pal_network_download_file rc = %d end", rc);

    return rc;
}

/**
 * Parse Download Descriptor from XML to structure
 * @param filename not used
 * @param out parsed descriptor
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS of OK
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY if there is not memory for parser or descriptor
 *         PAL_RC_FRMW_DOWNLOAD_ERROR if file with "filename" does not exist
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR if file with descriptor
 *                                                   contains some errors
 */
int parse_download_descriptor(const char *filename, pal_download_descriptor_t **out)
{
    (void) filename;
    int error = pal_create_download_descriptor(NULL, out);
    return error;
}

/**
 * Save link and file name to temporary file
 * @param url link of downloaded file
 * @param name name of package on local filesystem
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS if ok
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_NULL if url or name is NULL
 *                                         or cannot write to file
 */
static int save_link_and_name_to_file(const char *url, const char *name){
    if(NULL == url || NULL == name) {
        // nothing to save
        return PAL_RC_FRMW_DOWNLOAD_ERROR_NULL;
    }
    //remove old file then create it again
    FILE *file = fopen(MO_WORK_PATH "/" MO_DATA_PFX "/" TEMPFILE, "wb");
    if(NULL == file) {
        // nowhere to save
        return PAL_RC_FRMW_DOWNLOAD_ERROR_NULL;
    }
    // save link
    fprintf(file, "%s\n", url);
    fprintf(file, "%s", name);
    fclose(file);
    return PAL_RC_FRMW_DOWNLOAD_SUCCESS;
}

/**
 * Check was downloading aborted emergency, for example on unexpected power-off
 * @param[out] url url of download. User must allocate and free it manually
 * @param[out] name name of downloaded file. User must allocate and free it manually
 * @return 1 if yes
 *         0 if no
 */
static int check_download_aborted(char **url, char **name){
    FILE *file = fopen(MO_WORK_PATH "/" MO_DATA_PFX "/" TEMPFILE, "r");
    if (NULL == file) return 0;

    const int STRINGSNUM = 2;// two strings: URL and the name of file
    char line[STRINGSNUM][MAXSTRINGLEN];
    int i = 0;
    while (fgets(line[i], MAXSTRINGLEN, file) != NULL) {
        if(0 != strlen(line[i])) i++;
    }
    fclose(file);
    *url[0] = '\0';
    *name[0] = '\0';
    strcpy(*url, line[0]);
    strcpy(*name, line[1]);
    if (STRINGSNUM == i) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Check and remove old broken package
 * @param filename name of file with package
 * @return 0 if package was broken and removed
 *           OR package not exists
 *         1 if CURL can continue of loading of this package
 */
static int check_garbage(const char *filename){
    if (NULL == filename) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_NULL %s:%d", __FILE__, __LINE__);
        return 0;
    }
    char *url = calloc(MAXSTRINGLEN, sizeof(char));
    if (NULL == url) {
        ALOGE("PAL_RC_FRMW_DOWNLOAD_ERROR_NULL %s:%d", __FILE__, __LINE__);
        return 0;
    }
    char *package_name = calloc(MAXSTRINGLEN, sizeof(char));
    if(NULL == package_name){
        free(url);
        ALOGD("PAL_RC_FRMW_DOWNLOAD_ERROR_NULL %s:%d", __FILE__, __LINE__);
        return 0;
    }
    int continue_status = check_download_aborted(&url, &package_name);
    int result = 0;
    if (1 == continue_status &&
        0 == strcmp(filename, package_name)) {
        //continue download
        result = 1;
        struct stat st = {0};
        stat(filename, &st);
        unsigned long file_size = st.st_size;
        ALOGD("FW update package loading continued from offset: %lu", file_size);
    } else {
        //remove old file then download new
        int len = strlen(filename) + strlen(MO_WORK_PATH "/" MO_DATA_PFX "/") + 1;
        char string[len];
        string[0] = '\0';
        strcat(string, MO_WORK_PATH "/" MO_DATA_PFX "/");
        strcat(string, filename);
        remove(string);
        result = 0;
        ALOGD("FW update package loading started from zero");
    }
    free(url);
    free(package_name);
    return result;
}

