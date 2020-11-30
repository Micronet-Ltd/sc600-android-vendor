/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */
#include <utils/Log.h>

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include "pal.h"
#include "pal_fumo_cfg.h"
#include "mo_error.h"
#include "pal_internal.h"

#define PALFU_FIRMWARE_FILE "firmware_update.bin"

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

int pal_get_firmware_version(char **fmwv)
{
    int rc = 500;
    FILE *fd = NULL;
    char *rawline = NULL;
    size_t line_len = 0;

    *fmwv = NULL;

    if(NULL != (fd=fopen(
        PALFU_WORK_PATH"/"PAL_FILENAME_LAST_UPD_FIRMWARE, "r")))
    {
        if(-1 == getline(&rawline, &line_len, fd))
            return rc;
        *fmwv = rawline;
        rc = 200;
    }

    return rc;
}


/**
 *  @brief update operation thread
 *  @param [in] args firmware_update_context_t pointer
 *  @return 0 if success, 1 if cancelled
*/
void* update_thread(void *args)
{
    int i = 0, chunk_size = 0, rc = 0, read_size = 0, thrc = 0;
    int process_result = PAL_RC_FRMW_UPD_COMPLETED_FAILED;
    FILE *fdin = NULL, *fdout = NULL;
    char *buff = 0;

    char cur_time_str[40] = "\0";
    time_t rawtime = 0;
    struct tm * ptm = NULL;
    int cur_time_str_size = 0;
    FILE *fd = NULL;
    char *indxDash = NULL, *fwname = NULL;

    firmware_update_context_t *context = (firmware_update_context_t*)args;
    chunk_size = context->update_descriptor->size / 4 + 1;

    do
    {
        if(NULL == (fdin=fopen(context->update_descriptor->name, "r")))
            break;
        if(NULL == (fdout=fopen(PALFU_WORK_PATH"/"PALFU_FIRMWARE_FILE, "w")))
            break;
        if(NULL == (buff=(char*)calloc(chunk_size, sizeof(char))))
            break;

        for(i=0; i < 100; i+=25)
        {
            sleep(10);
            if(context->cancel)
            {
                process_result = PAL_RC_FRMW_UPD_CANCELLED;
                break;
            }

            if( 0 == (rc=fread(buff, 1, chunk_size, fdin)))
                break;
            read_size = rc;

            if(read_size != (rc=fwrite(buff, 1, read_size, fdout)))
                break;

            if(context->update_descriptor->progress)
            {
                /** update in progress*/
                context->update_descriptor->
                    progress(context, i, PAL_RC_FRMW_UPD_INPROGRESS);
            }

            process_result = PAL_RC_FRMW_UPD_COMPLETED_SUCCESS;
            if(feof(fdin))
                break;
        }
    } while(0);

    if(fdin) fclose(fdin);
    if(fdout) fclose(fdout);
    if(buff) free(buff);

    /** store firmware version */
    /** must be an one line as <DD:MM:YYY HH:MM:SS {FIRMWARE VERSION}> */
    if(context->update_descriptor->name &&
        process_result == PAL_RC_FRMW_UPD_COMPLETED_SUCCESS)
    {
        if(NULL != (fd=fopen(
                PALFU_WORK_PATH"/"PAL_FILENAME_LAST_UPD_DATETIME, "w"))) {
            time ( &rawtime );
            ptm = gmtime ( &rawtime );
            cur_time_str_size = strftime(cur_time_str, sizeof(cur_time_str),
                PAL_DATEIME_FILE_FORMAT, ptm);
            fwrite(cur_time_str, 1, cur_time_str_size, fd);
            fclose(fd);
        }
        if(NULL != (fd=fopen(
                PALFU_WORK_PATH"/"PAL_FILENAME_LAST_UPD_FIRMWARE, "w"))) {
            fwname = context->update_descriptor->name;
            if(NULL == (indxDash=strrchr(fwname, (int)'_')) ||
                                    ++indxDash >= fwname+strlen(fwname))
                indxDash = fwname;

            fwrite(indxDash, 1, strlen(indxDash), fd);
            fclose(fd);
        }
        if(NULL != (fd=fopen(
                PALFU_WORK_PATH"/"PAL_FILENAME_FIRMWARE_PACKET_NAME, "w"))) {
            fwname = context->update_descriptor->name;
            if(NULL == (indxDash=strrchr(fwname, (int)'/')) ||
                                    ++indxDash >= fwname+strlen(fwname))
                indxDash = fwname;

            fwrite(indxDash, 1, strlen(indxDash), fd);
            fclose(fd);
        }
    }

    context->update_descriptor->progress(context, i, process_result);
    free(args);
    return (void*)(long)thrc;
}


int pal_update_firmware(pal_update_descriptor_t *ud)
{
    firmware_update_context_t *context = (firmware_update_context_t*)
                                calloc(1, sizeof(firmware_update_context_t) );
    if (context == NULL) {
        WRNPRINT("could not allocate memory");
        return 500;
    }

    ud->context = context;
    context->update_descriptor = ud;
	ALOGE("pal_update_firmware pkgName = %s\n", ud->name);

//    if (MO_ERROR_SUCCESS != pal_verify_Crc32(ud->name)
    if (MO_ERROR_SUCCESS != pal_verify_update_image(ud->name)) {   //modified by chapin
    //|| MO_ERROR_SUCCESS != pal_truncate_checksum(ud->name)) {
        context->update_descriptor->progress(context, 0, PAL_RC_FRMW_UPD_COMPLETED_FAILED);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_VERIFICATION_FAILED;
    }

    int rc = 0;
    char cur_time_str[40] = "\0";
    time_t rawtime = 0;
    struct tm * ptm = NULL;
    int cur_time_str_size = 0;
    FILE *fd = NULL;

  	if(NULL != (fd=fopen(
                PALFU_WORK_PATH"/"PAL_FILENAME_LAST_UPD_DATETIME, "w"))) {
            time ( &rawtime );
            ptm = gmtime ( &rawtime );
            cur_time_str_size = strftime(cur_time_str, sizeof(cur_time_str),
                PAL_DATEIME_FILE_FORMAT, ptm);
            fwrite(cur_time_str, 1, cur_time_str_size, fd);
			ALOGE("pal_update_firmware time = %s\n", cur_time_str);
            fclose(fd);
     }

    /*if ((rc = pthread_create(&context->thread_id, NULL, update_thread, context)) != 0)
    {
        WRNPRINT("could not create thread, rc = %d", rc);
        free(context);
        pal_meminfo_log();
        pal_process_info_log();
        ud->context = NULL;
        return 500;
    }*/

    pal_install_update_image(ud->name);
    return 200;
};

/**
 *  @brief it cancels firmware update
 *  @param [in] context context of update operation
 *  @return HTTP response status code
*/
int pal_update_firmware_cancel(void *context)
{
    void *thread_rc = NULL;
    firmware_update_context_t *c = (firmware_update_context_t*)context;
    c->cancel = 1;
    pthread_join(c->thread_id, &thread_rc);
    return 200;
};




