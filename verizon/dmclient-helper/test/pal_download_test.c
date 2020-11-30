/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <utils/Log.h>

#include "pal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PAL_TEST_DOWNLOAD_EMULATION"

#define CU_ASSERT_EQUAL(L, R) \
    if( (L) != (R) ) { \
        printf("CU_ASSERT_EQUAL on %s:%d\n", __FILE__, __LINE__); \
    failed_tests++; } \
    else { \
    passed_tests++; \
    }

#define CU_ASSERT_PTR_NOT_NULL_FATAL(V) \
    if( (V) == NULL ) { \
    printf("CU_ASSERT_PTR_NOT_NULL_FATAL on %s:%d\n", __FILE__, __LINE__); \
    failed_tests++; } \
    else { \
    passed_tests++; \
    }

#define CU_ASSERT_PTR_NOT_NULL(V) \
    if( (V) == NULL ) { \
    printf("CU_ASSERT_PTR_NOT_NULL on %s:%d\n", __FILE__, __LINE__); \
    failed_tests++; } \
    else { \
    passed_tests++; \
    }

#define CU_ASSERT_PTR_NULL(V) \
    if( (V) != NULL ) { \
    printf("CU_ASSERT_PTR_NULL on %s:%d\n", __FILE__, __LINE__); \
    failed_tests++; } \
    else { \
    passed_tests++; \
    }

#define CU_ASSERT_FALSE(V) \
    if( (V) != 0 ) { \
    printf("CU_ASSERT_FALSE on %s:%d\n", __FILE__, __LINE__); \
    failed_tests++; } \
    else { \
    passed_tests++; \
    }

#define CU_PASS(V) \
    { passed_tests++; }

#define CU_FAIL(V) \
    { \
    printf("CU_FAIL on %s:%d %s\n", __FILE__, __LINE__,V); \
    failed_tests++;  \
    }

#define CU_SKIP(V) \
    { \
    printf("CU_SKIP on %s:%d %s\n", __FILE__, __LINE__,V); \
    skipped_tests++;  \
    }

#ifdef BUILD_DIR
#   define FUMO_DESCRIPTOR_URL "file://download_descriptor_local.xml"
#   define FUMO_PACAKGE_URL "file://VZW_Model_123_456.pdf"
#else /*BUILD_DIR*/
#   define FUMO_DESCRIPTOR_URL "https://www.dropbox.com/s/avmoyrckmnwduqs/download_descriptor.xml?dl=1"
#   define FUMO_PACAKGE_URL "https://www.dropbox.com/s/pgafka2a7n1254m/package.pdf?dl=1"
#endif /*BUILD_DIR*/

static int passed_tests;
static int failed_tests;
static int skipped_tests;

/**
 * Test callback on changing state of downloading
 * @param context not used in this version
 * @param progress current state of downloading
 * @param err_code code of error
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS
 */
int onProgress(void *context, unsigned int progress, int err_code) {
    //send current progress to notification
    (void) context;
    printf("OnProgress: %d\n", progress);
    printf("   errcode: %d\n", err_code);
    return PAL_RC_FRMW_DOWNLOAD_SUCCESS;
}

int init_suite(void) {
    passed_tests = 0;
    failed_tests = 0;
    return 0;
}

int clean_suite(void) {
    if(0 == failed_tests){
        printf("======================\n");
        printf(" All %d tests passed \n", passed_tests);
        printf(" %d tests skipped \n", skipped_tests);
        printf("======================\n");
    } else {
        printf("==================\n");
        printf(" Failed %d tests \n", failed_tests);
        printf(" Passed %d tests \n", passed_tests);
        printf(" %d tests skipped \n", skipped_tests);
        printf("==================\n");
    }
    return 0;
}

void testCase_download(void)
{
    pal_download_status_t *status = (pal_download_status_t*) malloc(sizeof(pal_download_status_t));
    if (NULL == status) { CU_FAIL("memory error"); }
    status->serverUrl = FUMO_DESCRIPTOR_URL;
    status->packageName = NULL;
    status->on_progress = onProgress;
    void *context;
    CU_ASSERT_EQUAL(pal_download_firmware(status), PAL_RC_FRMW_DOWNLOAD_SUCCESS);
    free(status);
}

void testCase_download_ERROR_NULL(void)
{
    pal_download_status_t *status = (pal_download_status_t*) malloc(sizeof(pal_download_status_t));
    if (NULL == status) { CU_FAIL("memory error"); }
    status->serverUrl = NULL;
    status->context = NULL;
    status->on_progress = onProgress;
    CU_ASSERT_EQUAL(pal_download_firmware(status), PAL_RC_FRMW_DOWNLOAD_ERROR_NULL);
    free(status);
}

void testCase_download_ERROR(void)
{
    pal_download_status_t *status = NULL;
    CU_ASSERT_EQUAL(pal_download_firmware(status), PAL_RC_FRMW_DOWNLOAD_ERROR);
}

void testCase_download_ERROR_CANNOT_DOWNLOAD(void)
{
    pal_download_status_t *status = (pal_download_status_t*) malloc(sizeof(pal_download_status_t));
    if (NULL == status) { CU_FAIL("memory error"); }
    status->serverUrl = "";
    status->context = NULL;
    status->on_progress = NULL;
    CU_ASSERT_EQUAL(pal_download_firmware(status), PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_DOWNLOAD);
    free(status);
}

void testCase_downloadCancel(void)
{
    void *context = 0;
    CU_ASSERT_EQUAL(pal_download_cancel(context), PAL_RC_FRMW_DOWNLOAD_SUCCESS);
}

void testCase_requestDownloadDescriptor_OK(void)
{
    char *serverUrl = FUMO_DESCRIPTOR_URL;//url is defined
    char *descriptorFileName = NULL;//descriptor is NULL
    int err = request_download_descriptor(serverUrl, &descriptorFileName);
    if(0 == strcmp("fumo_download_descriptor.xml", descriptorFileName) &&
       PAL_RC_FRMW_DOWNLOAD_SUCCESS == err){
        CU_PASS("Passed");
    } else {
        CU_FAIL("Failed");
    }
}

void testCase_requestDownloadDescriptor_ERRUR_EMP_URL(void)
{
    char *serverUrl = "";//url is empty
    char *descriptorFileName = NULL;//descriptor is NULL
    CU_ASSERT_EQUAL(request_download_descriptor(serverUrl, &descriptorFileName), PAL_RC_FRMW_DOWNLOAD_ERROR_EMPTY_URL);
    free(descriptorFileName);
}

void testCase_requestDownloadDescriptor_ERROR_NULL(void)
{
    char *serverUrl = NULL;//url is NULL
    char *descriptorFileName = NULL;//descriptor is NULL
    CU_ASSERT_EQUAL(request_download_descriptor(serverUrl, &descriptorFileName), PAL_RC_FRMW_DOWNLOAD_ERROR_NULL);
    free(descriptorFileName);
}

void testCase_parseDDescriptor_OK(void)
{
    pal_download_descriptor_t *out = NULL;
    CU_ASSERT_EQUAL(parse_download_descriptor(NULL, &out), PAL_RC_FRMW_DOWNLOAD_SUCCESS);
    free_download_descriptor_t(&out);
}

void testCase_parseDDescriptor_ERROR_NULL(void)
{
    char *in = NULL;
    pal_download_descriptor_t *out;
    CU_ASSERT_EQUAL(parse_download_descriptor(in, &out), PAL_RC_FRMW_DOWNLOAD_ERROR_NULL);
    free_download_descriptor_t(&out);
}

void testCase_parseDDescriptor_ERROR_CANNOT_PARSE(void)
{
    char *in = "";
    pal_download_descriptor_t *out;
    CU_ASSERT_EQUAL(parse_download_descriptor(in, &out), PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR);
    free_download_descriptor_t(&out);
}

void testCase_parseDDescriptor_CHECK_PARSE(void)
{
    pal_download_descriptor_t *out = NULL;
    int err = parse_download_descriptor(NULL, &out);
    if(PAL_RC_FRMW_DOWNLOAD_SUCCESS != err){
        CU_FAIL("Failed");
        return;
    }
    if (6115 == out->size &&//this value comes from download descriptor
        0 == strcmp(out->objectUri, FUMO_PACAKGE_URL) &&
        0 == strcmp(out->type, "application/octet-stream") &&
        0 == strcmp(out->packageName, "package.pdf") &&
        0 == strcmp(out->vendor, "DM Server Vendor Name Here") &&
        0 == strcmp(out->installParam,
                    "estimatedDownloadTimeInSecs=60;estimatedInstallTimeInSecs=15") &&
        0 == strcmp(out->description, "Description Here")) {
        CU_PASS("Passed");
    } else {
        CU_FAIL("Failed");
    }
    free_download_descriptor_t(&out);// do not forget too free mamory after
                                     // parse_download_descriptor(...)!

}

void testCase_startDownloadingFW_OK(void)
{
    pal_download_status_t *status = (pal_download_status_t*) malloc(sizeof(pal_download_status_t));
    if (NULL == status) { CU_FAIL("memory error"); }
    status->serverUrl = FUMO_PACAKGE_URL;
    status->packageName = "package.pdf";
    status->on_progress = onProgress;
    CU_ASSERT_EQUAL(start_downloading_firmware(status), PAL_RC_FRMW_DOWNLOAD_SUCCESS);
    free(status);
}

int main()
{
    if(init_suite())
    {
        printf("%s failed on init_suite\n", __FILE__);
        exit(1);
    }

    testCase_download();
    testCase_downloadCancel();
    testCase_download_ERROR();
    testCase_download_ERROR_NULL();
    testCase_download_ERROR_CANNOT_DOWNLOAD();
    testCase_requestDownloadDescriptor_OK();
    testCase_requestDownloadDescriptor_ERRUR_EMP_URL();
    testCase_requestDownloadDescriptor_ERROR_NULL();
    testCase_parseDDescriptor_OK();
    testCase_parseDDescriptor_CHECK_PARSE();
    testCase_startDownloadingFW_OK();

    if(clean_suite())
    {
        printf("%s failed on clean_suite\n", __FILE__);
        exit(1);
    }

    return 0;
}

