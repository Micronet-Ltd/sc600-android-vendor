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
#define LOG_TAG "PAL_TEST_FIRMWARE_UPDATE_EMULATION"

#define CU_ASSERT_EQUAL(L, R) \
    if( (L) != (R) ) { \
        ALOGE("CU_ASSERT_EQUAL on %s:%d\n", __FILE__, __LINE__); \
    exit(42); }

#define CU_ASSERT_PTR_NOT_NULL_FATAL(V) \
    if( (V) == NULL ) { \
    ALOGE("CU_ASSERT_PTR_NOT_NULL_FATAL on %s:%d\n", __FILE__, __LINE__); \
    exit(42); }

#define CU_ASSERT_PTR_NOT_NULL(V) \
    if( (V) == NULL ) { \
    ALOGE("CU_ASSERT_PTR_NOT_NULL on %s:%d\n", __FILE__, __LINE__); \
    exit(42); }

#define CU_ASSERT_PTR_NULL(V) \
    if( (V) != NULL ) { \
    ALOGE("CU_ASSERT_PTR_NULL on %s:%d\n", __FILE__, __LINE__); \
    exit(42); }

#define CU_ASSERT_FALSE(V) \
    if( (V) != 0 ) { \
    ALOGE("CU_ASSERT_FALSE on %s:%d\n", __FILE__, __LINE__); \
    exit(42); }

/**
 *  @brief context of update thread
*/
typedef struct firmware_update_context_t
{
    pal_update_descriptor_t *update_descriptor; /*!< in, Descriptor of update \
operation */
    pthread_t thread_id; /*!< in, ID of update thread */
    int cancel; /*!< in, out, order to cancel update if it equal to 1 */
} firmware_update_context_t;

/**
 *  @brief update operation thread
 *  @param [in] args firmware_update_context_t pointer
 *  @return 0 if success, 1 if cancelled
*/
extern void* update_thread(void *args);

static pal_update_descriptor_t pud;
#define FRMWRUPD_FILE "manuf_model_vbase_vtarget"
#define FRMWRUPD_SIZE ((int)1028)

int init_suite(void)
{
    FILE *fd = 0;
    char buff[FRMWRUPD_SIZE];

    fd = fopen(FRMWRUPD_FILE, "w");
    if(fd == NULL)
        return 1;

    if( fwrite(buff, sizeof(buff), 1, fd) != 1)
    {
        fclose(fd);
        return 1;
    }

    fclose(fd);

    return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite(void)
{
    return (remove(FRMWRUPD_FILE) < 0 ? 1 : 0 );
}


volatile int cb_count = 0;
int upd_cb_progress_ut_callback(void *context, unsigned int progress,
                                int err_code)
{
    (void)context;
    (void)progress;
    (void)err_code;
    ++cb_count;
    return 0;
}


void test_case_firmware_update_callbacks(void)
{
    cb_count = 0;
    memset(&pud, 0, sizeof(pud));
    pud.progress = upd_cb_progress_ut_callback;
    pud.name = FRMWRUPD_FILE;
    pud.size = FRMWRUPD_SIZE;
    CU_ASSERT_EQUAL( pal_update_firmware(&pud), 200);
    sleep(1);
    CU_ASSERT_EQUAL(cb_count, 5);
}

void test_case_firmware_update_cancel(void)
{
    cb_count = 0;
    memset(&pud, 0, sizeof(pud));
    pud.progress = upd_cb_progress_ut_callback;
    pud.name = FRMWRUPD_FILE;
    pud.size = FRMWRUPD_SIZE;
    CU_ASSERT_EQUAL( pal_update_firmware(&pud), 200);
    CU_ASSERT_EQUAL( pal_update_firmware_cancel(pud.context), 200);
    sleep(1);
    CU_ASSERT_EQUAL(cb_count, 1);
}


int upd_cb_progress_ut_callback_failed(void *context, unsigned int progress,
                                int err_code)
{
    (void)context;
    (void)progress;
    (void)err_code;
    cb_count = err_code;
    return 0;

}

void test_case_firmware_update_failed(void)
{
    cb_count = 0;
    memset(&pud, 0, sizeof(pud));
    pud.progress = upd_cb_progress_ut_callback_failed;
//    pud.name = FRMWRUPD_FILE;
    pud.size = FRMWRUPD_SIZE;
    CU_ASSERT_EQUAL( pal_update_firmware(&pud), 200);
    sleep(1);
    CU_ASSERT_EQUAL(cb_count, PAL_RC_FRMW_UPD_COMPLETED_FAILED);
}


void test_case_firmware_update_thread(void)
{
    firmware_update_context_t *uc = NULL;
    void* rc = NULL;

    uc = (firmware_update_context_t*)malloc(sizeof(firmware_update_context_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL( uc )

    pud.name = "/tmp_test_case_firmware_update_thread";
    uc->update_descriptor = &pud;

    rc = update_thread(uc);
    CU_ASSERT_PTR_NULL( rc );
}


void test_case_update_get_firmware_version(void)
{
    firmware_update_context_t *uc = NULL;
    void* rc = NULL;
    char *firmvare_version = NULL;
    FILE *fd = 0;
    char buff[FRMWRUPD_SIZE];

    uc = (firmware_update_context_t*)malloc(sizeof(firmware_update_context_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL( uc )
    memset(uc, 0, sizeof(firmware_update_context_t));

    pud.name = FRMWRUPD_FILE;
    pud.size = FRMWRUPD_SIZE;
    uc->update_descriptor = &pud;

    rc = update_thread(uc);
    if(((long)rc)==5)
    {
        uc = (firmware_update_context_t*)
            malloc(sizeof(firmware_update_context_t));
        memset(uc, 0, sizeof(firmware_update_context_t));
        CU_ASSERT_PTR_NOT_NULL_FATAL( uc )
        uc->update_descriptor = &pud;
        rc = update_thread(uc);
    }

    CU_ASSERT_PTR_NULL( rc );

    CU_ASSERT_EQUAL(pal_get_firmware_version(&firmvare_version), 200);
    printf("firmvare_version = %s\n", firmvare_version);
    CU_ASSERT_FALSE(strcmp("vtarget", firmvare_version));
    free(firmvare_version);

    pud.name = "filenamewithout";
    pud.size = FRMWRUPD_SIZE;

    fd = fopen(pud.name, "w");
    CU_ASSERT_PTR_NOT_NULL(fd);
    CU_ASSERT_EQUAL(fwrite(buff, sizeof(buff), 1, fd), 1);
    fclose(fd);

    uc = (firmware_update_context_t*)malloc(sizeof(firmware_update_context_t));
    memset(uc, 0, sizeof(firmware_update_context_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL( uc )
    uc->update_descriptor = &pud;
    rc = update_thread(uc);
    if(((long)rc)==5)
    {
        uc = (firmware_update_context_t*)
            malloc(sizeof(firmware_update_context_t));
        memset(uc, 0, sizeof(firmware_update_context_t));
        CU_ASSERT_PTR_NOT_NULL_FATAL( uc )
        uc->update_descriptor = &pud;
        rc = update_thread(uc);
    }
    CU_ASSERT_PTR_NULL( rc );

    CU_ASSERT_EQUAL(pal_get_firmware_version(&firmvare_version), 200);
    printf("firmvare_version = %s\n", firmvare_version);
    CU_ASSERT_FALSE(strcmp("filenamewithout", firmvare_version));
    free(firmvare_version);
}


int main()
{
    if(init_suite())
    {
        printf("%s failed on init_suite\n", __FILE__);
        exit(1);
    }

    test_case_firmware_update_callbacks();
    test_case_firmware_update_failed();
    test_case_firmware_update_cancel();
    test_case_firmware_update_thread();
    test_case_update_get_firmware_version();

    if(clean_suite())
    {
        printf("%s failed on clean_suite\n", __FILE__);
        exit(1);
    }

    return 0;
}
