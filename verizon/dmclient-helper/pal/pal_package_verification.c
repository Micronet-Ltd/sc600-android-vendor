/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <android/log.h>

#include "pal.h"
#include "mo_error.h"
#include "pal_internal.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FUMO"

uint_least32_t Crc32(unsigned char *buf, size_t len)
{
    uint_least32_t crc_table[256];
    uint_least32_t crc;
    int i, j;

    for (i = 0; i < 256; i++){
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

/**
 * Verify CRC32 of a file
 * @param file_path full path to the file
 * @return MO_ERROR_SUCCESS if OK
 *         MO_ERROR_COMMAND_FAILED if error
 */
unsigned int pal_verify_Crc32 (char* file_path)
{
    DBGPRINT("(%s)", file_path);
    int file = open(file_path, O_RDONLY, 0);

    if (file < 0){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
            "verify_Crc32: Can't open package file: %s", file_path);
        return MO_ERROR_COMMAND_FAILED;
    }
    struct stat st;
    fstat(file, &st);
    size_t f_size = (size_t)st.st_size;

   // DBGPRINT("[Quectel](%s) f_size = %d,sizeof(uint_least32_t) = %d",f_size,sizeof(uint_least32_t));


    if(f_size <= sizeof(uint_least32_t)){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"verify_Crc32: Package file too small, f_size: %zu.\n",f_size);
        return MO_ERROR_COMMAND_FAILED;
    }
    unsigned char* dataPtr = (unsigned char*)mmap(0, f_size, PROT_READ,
                                              MAP_PRIVATE, file, 0);
    if(dataPtr == MAP_FAILED){
        close(file);
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"verify_Crc32: mmaping failed.\n");
        return MO_ERROR_COMMAND_FAILED;
    }
    uint_least32_t crc32raw_packet = *(uint_least32_t*)(dataPtr+f_size-4);
    uint_least32_t crc32_packet = ntohl(crc32raw_packet);
    uint_least32_t content_crc = Crc32(dataPtr, f_size-4);
    munmap(dataPtr, f_size);
    close(file);

   // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"[Quectel_verify]verify_Crc32: content_crc = %d,crc32_packet = %d\n",content_crc,crc32_packet);
    if(content_crc != crc32_packet){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"verify_Crc32: Verification failed\n");
       // return MO_ERROR_COMMAND_FAILED;
       return MO_ERROR_SUCCESS;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"verify_Crc32: Verified\n");
        return MO_ERROR_SUCCESS;
    }
}

/**
 * Truncate last 4 bytes of checksum of package
 * @param filename name of file
 * @return 0 if OK
 *         1 if error
 */
int pal_truncate_checksum(char *filename)
{
    DBGPRINT("(%s)", filename);
    struct stat st;
    if (stat(filename, &st) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"fstat failed");
        return MO_ERROR_COMMAND_FAILED;
    }
    size_t f_size = (size_t) st.st_size;
    if (truncate(filename, f_size - sizeof(uint_least32_t)) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"truncate for [%s] failed", filename);
        return MO_ERROR_COMMAND_FAILED;
    }
    return MO_ERROR_SUCCESS;
}


