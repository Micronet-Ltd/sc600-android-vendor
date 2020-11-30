/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#include "./pal_internal.h"

const static char* const MEMINFO_PATH = "/proc/meminfo";

const static int BUFF_SIZE = 128;

typedef struct {
  int64_t total;  // total memory available in kiB
  int64_t free; // free memory in kiB
  int64_t shared; // shared memory in kiB
  int64_t buffers;  // buffers memory in kiB
  int64_t cached; // cached memory in kiB
  int64_t swap_total; // total swap space in kiB
  int64_t swap_free;  // free swap space in kiB
} meminfo_t;

static int __parse_meminfo_line(const char* buffer,
    char** p_key, int64_t* p_value, char** p_mesure)
{
    if ((p_key == NULL) || (p_value == NULL) ||
        (p_mesure == NULL) || (buffer == NULL)) {
        WRNPRINT("(%p, %p, %p, %p):INV", buffer, p_key, p_value, p_mesure);
        return RESULT_ERROR_INVALID_ARGS;
    }
    char* buffer_copy = strdup(buffer);
    if (buffer_copy == NULL) {
        WRNPRINT("could not allocate memory for strdup");
        return RESULT_MEMORY_ERROR;
    }
    char* buffer_d = buffer_copy;
    char* item = NULL;
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        *p_key = item;
    } else {
        WRNPRINT("could not get key");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        char* end_point;
        *p_value = strtoll(item, &end_point, 10);
        if (*p_value == LLONG_MIN || *p_value == LLONG_MAX) {
            WRNPRINT("could not convert %s to int64", item);
            FREE_IF(buffer_copy);
            return RESULT_PARSE_ERROR;
        }
    } else {
        WRNPRINT("could not get value");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    if (item != NULL) {
        *p_mesure = item;
    } else {
        WRNPRINT("could not get mesure");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    FREE_IF(buffer_copy);
    return RESULT_SUCCESS;
}

static int __pal_read_meminfo(meminfo_t* thiz)
{
    static size_t STR_SIZE = BUFF_SIZE - 1;
    int rc = 0;
    char* buffer = (char*)malloc(sizeof(char) * BUFF_SIZE);
    if (buffer == NULL) {
        WRNPRINT("could not allocate buffer to read lines");
        return RESULT_MEMORY_ERROR;
    }
    bzero(buffer, sizeof(char) * BUFF_SIZE);
    char* key = NULL;
    char* mesure = NULL;
    int64_t value = -1L;
    if (thiz != NULL) {
        thiz->free = -1L;
        thiz->total = -1L;
        thiz->cached = -1L;
        thiz->buffers = -1L;
        FILE* fp = fopen(MEMINFO_PATH, "r");
        if (fp != NULL) {
            while(fgets(buffer, STR_SIZE * sizeof(char), fp) != NULL) {
                value = -1L;
                if (strlen(buffer) >= STR_SIZE) {
                    WRNPRINT("strlen(%s) >= STR_SIZE", buffer);
                }
                if (__parse_meminfo_line(buffer, &key, &value, &mesure)
                    != RESULT_SUCCESS) {
                    WRNPRINT("could not parse string: %s", buffer);
                }
                DBGPRINT("%s : %s : %lld : %s", buffer, key, value, mesure);
                if (strstr(key, "MemTotal") == key){
                    thiz->total = value;
                }
                if (strstr(key, "MemFree") == key) {
                    thiz->free = value;
                }
                if (strstr(key, "Cached") == key) {
                    thiz->cached = value;
                }
                if (strstr(key, "Buffers") == key) {
                    thiz->buffers = value;
                }
            }
            fclose(fp);
        } else {
            rc = RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
            WRNPRINT("fopen(%s, \"r\"): failed", MEMINFO_PATH);
        }
        DBGPRINT("B:%lld, C:%lld, A:%lld, T:%lld"
            ,thiz->buffers
            ,thiz->cached
            ,thiz->free
            ,thiz->total
            );
    } else {
        rc = RESULT_ERROR_INVALID_ARGS;
        WRNPRINT("meminfo_t* thiz is NULL");
    }
    free(buffer);
    return rc;
}

/**
 * Reads available RAM to the meminfo_arg
 * @param[out] percents percents of available RAM
 * @see ./ManagedObjects/DiagMon/Memory/Avail
 * @return : @see enum result_states
 */
int pal_system_memory_avail_get(float* p_mbs)
{
    CHECK_PVALUE(p_mbs);
    meminfo_t meminfo;
    int rc = __pal_read_meminfo(&meminfo);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    DBGPRINT("nusd.free = %lld", meminfo.free);
    if (meminfo.free >= 0L) {
        if (meminfo.cached > 0L) {
            meminfo.free += meminfo.cached;
            DBGPRINT("cchd.free = %lld", meminfo.free);
        }
        if (meminfo.buffers > 0L) {
            meminfo.free += meminfo.buffers;
            DBGPRINT("bfrs.free = %lld", meminfo.free);
        }
    } else {
        WRNPRINT("meminfo.free < 0");
        return RESULT_ERROR_UNDEFINED;
    }
    *p_mbs = (float)(meminfo.free) / 1024.0f;
    return rc;
}

/**
 * Reads percent available RAM to the iobuf->data buffer as int32_t value
 * @param[out] percents percents of available RAM
 * @see ./ManagedObjects/DiagMon/Memory/PercentFree
 * @return @see enum result_states
 */
int pal_system_memory_percentfree_get(int* percents) {
    CHECK_PVALUE(percents);
    float avail = -1.f;
    float total = -1.f;
    BZEROTYPE(avail);
    BZEROTYPE(total);
    int rc = pal_system_memory_avail_get(&avail);
    if (rc != RESULT_SUCCESS) {
        WRNPRINT("pal_system_memory_avail_get:failed: rc = %d", rc);
        return rc;
    }
    rc = pal_system_memory_total_get(&total);
    if (rc != RESULT_SUCCESS) {
        WRNPRINT("pal_system_memory_total_get:failed: rc = %d", rc);
        return rc;
    }
    if (total < -0.5f) {
        WRNPRINT("total == %f", total);
        return RESULT_ERROR_UNDEFINED;
    }
    *percents = (int)(avail/total * 100.f);
    return rc;
}

/**
 * Reads size of RAM to the meminfo_arg
 * @see ./ManagedObjects/DiagMon/Memory/Total
 * @param[out] percents percents of total RAM
 * @return @see enum result_states
 */
int pal_system_memory_total_get(float* p_mbs)
{
    CHECK_PVALUE(p_mbs);
    meminfo_t meminfo;
    int rc = __pal_read_meminfo(&meminfo);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    DBGPRINT("nusd.total = %lld", meminfo.total);
    if (meminfo.free < 0L) {
        WRNPRINT("meminfo.total < 0");
        return RESULT_ERROR_UNDEFINED;
    }
    *p_mbs = (float)(meminfo.total) / 1024.0f;
    return rc;
}

/**
 * Reads size of ROM to the p_mbs
 * @param[in]path path on filesystem
 * @param[out]p_bs amount of memory
 * @return @see enum result_states
 */
int pal_system_get_memory_free_by_path(const char* path, unsigned long long* p_bs)
{
    struct statvfs vfs;
    int res = statvfs(path, &vfs);
    *p_bs = vfs.f_bfree * vfs.f_frsize;
    if (0 == res) {
        DBGPRINT("RP2 PAL DiagMon: There are %llu BYETS avilable space in %s", *p_bs, path);
        return RESULT_SUCCESS;
    } else {
        DBGPRINT("RP2 PAL DiagMon: Failed to calculate free space on folder %s",
                  path);
        return RESULT_ERROR;
    }
}
