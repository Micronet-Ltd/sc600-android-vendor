/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */


/**
 *   Network module internal implementation
 */
#include <curl/curl.h>
#include <openssl/crypto.h>
#include <openssl/thread.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "pal.h"
#include "./pal_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_NETWORK"

/// Global flag for check PAL Network module was initialized or not
static bool global_init_flag = false;

/// Thread id for file download thread
static pthread_t curl_download_thread_id = 0L;

/// HTTP response code
#define HTTP_SUCCESS             200
#define HTTP_NOT_FORBIDDEN       403
#define HTTP_NOT_FOUND           404
#define HTTP_SERVICE_UNAVAILABLE 503

/// SSL threads saving support
#define MUTEX_TYPE       pthread_mutex_t
#define MUTEX_SETUP(x)   pthread_mutex_init(&(x), NULL)
#define MUTEX_CLEANUP(x) pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)    pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x)  pthread_mutex_unlock(&(x))
#define THREAD_ID        pthread_self()

/// Retrying when host is unreachable
#define MAX_RETRY_COUNT 3
#define RETRY_TIMEOUT   3
#define RETRY_RATIO     2

/* This array will store all of the mutexes available to OpenSSL. */
static MUTEX_TYPE *mutex_buf= NULL;

static void lock_callback(int mode, int type, const char *file, int line)
{
    UNUSED(file);
    UNUSED(line);
    if (mode & CRYPTO_LOCK) {
        MUTEX_LOCK(mutex_buf[type]);
    } else {
        MUTEX_UNLOCK(mutex_buf[type]);
    }
}

static unsigned long thread_id(void)
{
    return ((unsigned long)THREAD_ID);
}

static void init_mutexs(void)
{
    mutex_buf = (MUTEX_TYPE *)OPENSSL_malloc(CRYPTO_num_locks()*sizeof(MUTEX_TYPE));
    for (int i = 0; i < CRYPTO_num_locks(); i++) {
        MUTEX_SETUP(mutex_buf[i]);
    }
    CRYPTO_set_id_callback(thread_id);
    CRYPTO_set_locking_callback(lock_callback);
}

static void release_mutexs(void)
{
    if (!mutex_buf) {
        return;
    }
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    for (int i = 0; i < CRYPTO_num_locks(); i++) {
        MUTEX_CLEANUP(mutex_buf[i]);
    }
    OPENSSL_free(mutex_buf);
    mutex_buf = NULL;
}

/// Bundle of CA Root Certificates store in PEM format
#define CA_INFO_NAME "/system/etc/security/cacerts/ca-bundle.crt"
/// \todo The OEM shall implement own root certificate from the chain of
///       certificates available on the server.

/**
 *  Callback function for writing received data
 *  @param[in] contents  points to the delivered data. The data passed to this
 *                       function will not be zero terminated.
 *  @param[in] size      the size of one data item
 *  @param[in] nmemb     the number of data items. The real data size = size * nmemb.
 *  @param[in] userp     user data, should be set with the CURLOPT_WRITEDATA option
 *  @return the number of bytes actually taken or  0 - failed writing data, close connection
 */
static size_t store_reply_callback(void * contents,
                                   size_t size,
                                   size_t nmemb,
                                   void * userp)
{
    size_t total = size * nmemb;
    if( contents == NULL || userp == NULL ) {
        ALOGE("store_reply_callback - invalid parameters!");
        return 0;
    }
    if (userp) {
        data_buffer_t * reply = (data_buffer_t *)userp;
        reply->data = (char*)realloc(reply->data, reply->size + total);
        if (reply->data == NULL) {
            ALOGE("ERROR: Not enough memory");
            return 0;
        }
        memcpy(&(reply->data[reply->size]), contents, total);
        reply->size += total;
        return total;
    }
    return 0;
}

int pal_network_initialize()
{
    curl_global_init(CURL_GLOBAL_ALL);
    init_mutexs();
    return 0;
}

long pal_send_http_post_request (char *server_url, void *options, void * data, void * reply)
{
    CURL *curl;
    struct curl_slist * headerList = NULL;
    long status = HTTP_SERVICE_UNAVAILABLE;

    pal_http_options_t *opt = NULL;
    if(options)
        opt = (pal_http_options_t *)options;
    data_buffer_t *data_out = NULL;
    if(data)
        data_out = (data_buffer_t*)data;
    if (!global_init_flag) {
        pal_network_initialize();
        global_init_flag = true;
    }
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL,server_url);
        if(opt) {
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, opt->timeout);
            if(opt->hdr_content_data)
                headerList = curl_slist_append(headerList, opt->hdr_content_data);
            if(opt->hdr_auth_data)
                headerList = curl_slist_append(headerList, opt->hdr_auth_data);
            if(opt->auth_type == AUTH_BASIC)
                curl_easy_setopt(curl, CURLOPT_HTTPAUTH,  (long)CURLAUTH_BASIC);
            else if(opt->auth_type == AUTH_DIGEST)
                curl_easy_setopt(curl, CURLOPT_HTTPAUTH,  (long)CURLAUTH_DIGEST);
            // add to HTTP header
            if(headerList)
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        }
        // specify path to Certificate Authority bundle
        curl_easy_setopt(curl, CURLOPT_CAINFO, CA_INFO_NAME);
        // curl verifies the authenticity of the peer's certificate
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        // verify the certificate's name against host
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        // request using TLS1.2 for the transfer (Verizon req, 30.3 Security)
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION,CURL_SSLVERSION_TLSv1_2);
        /*
         * set cipher list according to OMA Device Management Security,
         * 5.5.1.1 Transport protocols that support encryption
         */
        curl_easy_setopt(curl,CURLOPT_SSL_CIPHER_LIST,
                        "AES128-SHA:DES-CBC3-SHA:RC4-SHA");
        /*
         *  TLS1.0 cipher suite names:
         *  AES128-SHA   - TLS_RSA_WITH_AES_128_CBC_SHA
         *  DES-CBC3-SHA - TLS_RSA_WITH_3DES_EDE_CBC_SHA
         *  RC4-SHA      - TLS_RSA_WITH_RC4_128_SHA
         */
        // debug info
        // request SSL certificate information
        // curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        // request a HTTP POST
        if(data_out && data_out->size > 0) {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)(data_out->size));
            curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data_out->data);
        }
        if(reply) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_reply_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, reply);
        }

        // To prevent sending not thread safe signals in case when we work outside of main thread
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        int sleepTimer = RETRY_TIMEOUT;
        for (int retry=0; retry < MAX_RETRY_COUNT; retry++) {
            CURLcode res = curl_easy_perform(curl);
            if (CURLE_OK == res) {
                // RESPONSE_CODE
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
                ALOGD("curl - response code: [%ld]", status);
                // PRIMARY_IP & PRIMARY_PORT
                char *str_primary_ip;
                long port;
                curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &str_primary_ip);
                curl_easy_getinfo(curl, CURLINFO_PRIMARY_PORT, &port);
                ALOGD("curl - primary ip: [%s], port: [%ld]", str_primary_ip, port);
                // LOCAL_IP & LOCAL_PORT
                char *str_local_ip;
                curl_easy_getinfo(curl, CURLINFO_LOCAL_IP, &str_local_ip);
                curl_easy_getinfo(curl, CURLINFO_LOCAL_PORT , &port);
                ALOGD("curl - local ip: [%s], port: [%ld]", str_local_ip, port);
                // SSL_VERIFYRESULT
                long crt_res = 0;
                curl_easy_getinfo(curl, CURLINFO_SSL_VERIFYRESULT, &crt_res);
                if(crt_res!=0) {
                    ALOGE("certificate verify error [%ld]",crt_res);
                }
                retry = MAX_RETRY_COUNT;
            } else if ( /* Couldn't resolve host. The given remote host was not resolved */
                        res == CURLE_COULDNT_RESOLVE_HOST ||
                        /* Failed to connect() to host or proxy */
                        res == CURLE_COULDNT_CONNECT      ||
                        /* A problem occurred somewhere in the SSL/TLS handshake */
                        res == CURLE_SSL_CONNECT_ERROR ) {
                ALOGD("CURL can't perform command, res = %d, retry counter = %d", res, retry);
                sleep(sleepTimer);
                sleepTimer = sleepTimer * RETRY_RATIO;
            } else {
                ALOGE("CURL can't perform command, res = %d", res);
                retry = MAX_RETRY_COUNT;
            }
        }

        if(headerList)
           curl_slist_free_all(headerList);

        curl_easy_cleanup(curl);
    }

    if (global_init_flag) {
        pal_network_deinitialize();
        global_init_flag = false;
    }

    return status;
}

/**
 *  Callback function for writing download file data
 *  @param[in] data  points to the delivered data. The data passed to this
 *                       function will not be zero terminated.
 *  @param[in] size      the size of one data item
 *  @param[in] nmemb     the number of data items. The real data size = size * nmemb.
 *  @param[in] userdata  user data, should be set with the CURLOPT_WRITEDATA option
 *  @return the number of bytes actually written
 */
static size_t download_write_callback_func(char *data, size_t size, size_t nmemb, void *userdata)
{
    FILE* out_file = (FILE*) userdata;
    size_t written_size;
    written_size = fwrite(data, size, nmemb, out_file);
    return written_size;
}

/**
 *  File download thread routine
 */
static void* download_work_thread(void* args) {
    ALOGD("download_work_thread begin PID=%d; TID =%d", getpid(), getpid());

    pal_download_options_t *pdo = (pal_download_options_t*) args;

    //CURLcode status = (CURLcode)0;
	 CURLcode status;
    CURL *curl;
    long http = 0;
    FILE *out_file;
#if 1

    struct stat st = {0};
    //memset(st,0,sizeof(struct stat));
    stat(pdo->out_file, &st);
    unsigned long offset = st.st_size;
    ALOGD("download_work_thread file %s - offset=%ld",pdo->out_file ,offset);

    // File already there just exit
    if (pdo->total_download_size > 0 && offset == pdo->total_download_size) {
        ALOGD("download_work_thread file=%s already there just exit", pdo->download_url);
        pthread_exit((void*)0);
    }

    const char *mode = pdo->total_download_size > 0 ? "ab" : "wb";
    out_file = fopen(pdo->out_file, mode);
    chmod(pdo->out_file,0777);
    if (out_file == NULL) {
        ALOGE("download_work_thread error to open file=%s", pdo->out_file);
        pthread_exit((void*)-1);
    }

    curl = curl_easy_init();
    if (curl == NULL) {
        ALOGE("download_work_thread invalid curl initialization");
        pthread_exit((void*)-1);
    }

    curl_easy_setopt(curl, CURLOPT_URL, pdo->download_url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, pdo->timeout);
	ALOGD("downloadwork_thread timeout = %d\n", pdo->timeout);
    if (pdo->username != NULL) {
        char* usrpwd;
        asprintf(&usrpwd, "%s%s%s", pdo->username, ":", pdo->password ? pdo->password : "");
        curl_easy_setopt(curl, CURLOPT_USERPWD, usrpwd);
        free(usrpwd);
    }

    // Setup SSL configuration
    if (pdo->ca_fname) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, pdo->ca_fname);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
    } else {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    // Setup Data Callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_file);

    // Setup resume download if it is needed
    if (offset > 0 && offset < pdo->total_download_size) {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)st.st_size);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)pdo->total_download_size);
    }

    // Setup Progress Callback
    if (pdo->download_progress_callback) {
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, pdo->download_progress_callback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, pdo->download_progress_fn);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    } else {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    }

    // Setup minimal download speed threshold & time of low speed endured before aborting
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, OMADM_CURL_LOW_SPEED_LIMIT);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, OMADM_CURL_LOW_SPEED_TIME);

    // Setup redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, OMADM_CURL_MAXREDIRS);

    // To prevent sending not thread safe signals in case when we work outside of main thread
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    int sleepTimer = RETRY_TIMEOUT;
    for (int retry=0; retry < MAX_RETRY_COUNT; retry++) {
        status = curl_easy_perform(curl);
        if (CURLE_OK == status) {
            // PRIMARY_IP & PRIMARY_PORT
            char *str_primary_ip;
            long port;
            curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &str_primary_ip);
            curl_easy_getinfo(curl, CURLINFO_PRIMARY_PORT, &port);
            ALOGD("DOWNLOAD: curl - primary ip: [%s], port: [%ld]", str_primary_ip, port);
            // LOCAL_IP & LOCAL_PORT
            char *str_local_ip;
            curl_easy_getinfo(curl, CURLINFO_LOCAL_IP, &str_local_ip);
            curl_easy_getinfo(curl, CURLINFO_LOCAL_PORT , &port);
            ALOGD("DOWNLOAD: curl - local ip: [%s], port: [%ld]", str_local_ip, port);
            // SSL_VERIFYRESULT
            long crt_res = 0;
            curl_easy_getinfo(curl, CURLINFO_SSL_VERIFYRESULT, &crt_res);
            if(crt_res!=0) {
                ALOGE("DOWNLOAD: certificate verify error [%ld]",crt_res);
            }
            retry = MAX_RETRY_COUNT;
        } else if ( /* Couldn't resolve host. The given remote host was not resolved */
                    status == CURLE_COULDNT_RESOLVE_HOST ||
                    /* Failed to connect() to host or proxy */
                    status == CURLE_COULDNT_CONNECT      ||
                    /* A problem occurred somewhere in the SSL/TLS handshake */
                    status == CURLE_SSL_CONNECT_ERROR ) {
            ALOGD("DOWNLOAD: CURL can't perform comand, res = %d, retry counter = %d", status, retry);
            sleep(sleepTimer);
            sleepTimer = sleepTimer * RETRY_RATIO;
        } else {
            ALOGE("DOWNLOAD: CURL can't perform comand, res = %d", status);
            retry = MAX_RETRY_COUNT;
        }
    }
    if (status != CURLE_OK) {
        ALOGE("download_work_thread curl_easy_perform() failed: %s", (char*)curl_easy_strerror(status));
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http);
    ALOGD("download_work_thread curl: Server response code %ld", http);
    if (status == CURLE_OK &&
           (http == HTTP_NOT_FORBIDDEN ||
            http == HTTP_NOT_FOUND ||
            http == HTTP_SERVICE_UNAVAILABLE)) {
        status = CURLE_COULDNT_CONNECT;
    }

    ALOGD("1[Quectel]download_work_thread curl: Server response http %ld,status = %d", http,status);

    fclose(out_file);
    curl_easy_cleanup(curl);
    ALOGD("2[Quectel]download_work_thread curl: Server response http %ld", http);

#if 0
    curl_global_cleanup();
#endif
#endif // if 0
    ALOGD("3[Quectel]download_work_thread curl: Server response status %d", status);

    pthread_exit((void*)status);
    ALOGD("4[Quectel]download_work_thread curl: Server response status %ld", http);
}

long pal_network_download_file(pal_download_options_t* pdo)
{
    ALOGD("pal_network_download_file begin PID=%d; TID =%d url= %s", getpid(), getpid(),pdo->download_url);
    long status = 0;
    //int status ;
    if (pdo == NULL) {
        ALOGE("pal_network_download_file invalid arguments");
        status = -1;
        goto EXIT;
    }

    if (!global_init_flag) {
        pal_network_initialize();
        global_init_flag = true;
    }

    if (0 != pthread_create(&curl_download_thread_id, NULL, download_work_thread, pdo)) {
        ALOGE("pal_network_download_file thread create failed");
        status = -1;
        goto EXIT;
    }

    ALOGE("pal_network_download_filepal_net thread create success,status = %d",status);

    pthread_join(curl_download_thread_id, (void**)&status);
    curl_download_thread_id = 0L;

    ALOGE("pal_network_download_filepal_net thread exit status = %d,global_init_flag = %d",status,global_init_flag);

    EXIT:
        if (global_init_flag) {

            ALOGE("pal_network_download_filepal_net global_init_flag = %d",global_init_flag);

            pal_network_deinitialize();
            global_init_flag = false;
        }

        ALOGE("pal_network_download_filepal_net thread exit status = %d",status);

        return status;
}

int pal_network_deinitialize()
{
    ALOGE("[Quectel]pal_network_deinitialize pal_network_deinitialize");

    release_mutexs();
    curl_global_cleanup();

    ALOGE("[Quectel]pal_network_deinitialize");
    return 0;
}
