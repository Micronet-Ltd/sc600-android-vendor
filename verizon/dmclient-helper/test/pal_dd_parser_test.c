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
#define LOG_TAG "PAL_TEST_DD_PARSER_EMULATION"

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

int init_suite(void) {
    passed_tests = 0;
    failed_tests = 0;
    skipped_tests = 0;
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

void testCase_isAvilable(void) {
    int error = 0;
    char *tags[] = {"objectURI", "size", "type",
            "name", "vendor", "installParam", "description",
            "PreDownloadMessage","PostDownloadMessage",
            "PreDownloadURL","PostDownloadURL",
            "PostUpdateMessage","PostUpdateURL",
            "wrong_tag", NULL};
    int i = 0;
    for (i = 0; NULL != tags[i]; i++) {
        error += is_avilable(tags[i]);
    }
    //-1+0+1+2+3+4+5+..+12=77
    if (77 == error) {
        CU_PASS("Passed")
    } else {
        CU_FAIL("Failed")
    }
}

void testCase_checkRightTagsOrder(void){
    const char *tags[] = {"objectURI", "size", "type",
                 "name", "vendor", "installParam", "description",
                 "PreDownloadMessage","PostDownloadMessage",
                 "PreDownloadURL","PostDownloadURL",
                 "PostUpdateMessage","PostUpdateURL", NULL};
    int i = 0;
    for (i = 0; NULL != tags[i]; i++) {
        if(i != is_avilable(tags[i])){
            printf("testCase_isAvilable FAILED :Order of tags was violated! See: pal_dd_parser.c\n");
            CU_FAIL("Failed");
            return;
        }
    }
    CU_PASS("Passed");
}

void testCase_createDescriptor(void) {
    pal_download_descriptor_t *descriptor = NULL;
    int error = pal_create_download_descriptor(NULL, &descriptor);
    if(NULL == descriptor || PAL_RC_FRMW_DOWNLOAD_SUCCESS != error) {
        CU_SKIP("Cannot find file to parse. Skipping...");
    } else if(6115 == descriptor->size &&//this value comes from download descriptor
       0 == strcmp(descriptor->objectUri, FUMO_PACAKGE_URL) &&
       0 == strcmp(descriptor->type, "application/octet-stream") &&
       0 == strcmp(descriptor->packageName, "package.pdf") &&
       0 == strcmp(descriptor->vendor, "DM Server Vendor Name Here") &&
       0 == strcmp(descriptor->installParam, "estimatedDownloadTimeInSecs=60;estimatedInstallTimeInSecs=15") &&
       0 == strcmp(descriptor->description, "Description Here")){
        CU_PASS("Passed");
    } else {
        CU_FAIL("Failed");
    }
}

void testCase_createDescriptor_noSuchFile(void){
    pal_download_descriptor_t *descriptor = NULL;
    int error = pal_create_download_descriptor("noSuchFile.xml", &descriptor);
    if (NULL == descriptor && PAL_RC_FRMW_DOWNLOAD_ERROR == error) {
        CU_PASS("Passed");
    } else {
        CU_FAIL("Failed");
    }
}

int main()
{
    if(init_suite())
    {
        printf("%s failed on init_suite\n", __FILE__);
        exit(1);
    }

    testCase_isAvilable();
    testCase_createDescriptor();
    testCase_createDescriptor_noSuchFile();
    testCase_checkRightTagsOrder();

    if(clean_suite())
    {
        printf("%s failed on clean_suite\n", __FILE__);
        exit(1);
    }

    return 0;
}
