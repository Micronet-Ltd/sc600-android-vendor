/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dirent.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/cdefs.h>
#include <malloc.h>
#include <unistd.h>

#include "pal_internal.h"

__BEGIN_DECLS

#define BUFF_SIZE 64

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "pal_utils"

/*
 * Release array of c-strings
 * @param argc size of the array
 * @param argv pointer to the array
 */
void __pal_argvs_free(int argc, char** argv)
{
    int idx = 0;
    if (argv != NULL) {
        for (idx = 0; idx < argc; idx ++) {
            FREE_IF(argv[idx]);
        }
        free(argv);
    }
}

/*
 * appends one more c-string to the argv array and update size
 * @param p_argc pointer size of the array
 * @param p_argv pointer to pointer to the array of c-strings
 * @item mount point path
 * @note memory is allocated for added item
 */
int __pal_argvs_append(int* p_argc, char*** p_argv, const char* item)
{
    DBGPRINT("(%p, %p, %s)", p_argc, p_argv, item);
    char** new_argv = (char**)malloc((*p_argc + 1) * sizeof(char*));
    if (new_argv == NULL) {
        WRNPRINT("could not allocate memory for new array");
        return RESULT_MEMORY_ERROR;
    }
    int idx = 0;
    for (idx = 0; idx < *p_argc; idx ++) {
        new_argv[idx] = (*p_argv)[idx];
    }
    if (item != NULL) {
        new_argv[*p_argc] = strdup(item);
        if (new_argv[*p_argc] == NULL) {
            WRNPRINT("could not allocate memory for new item");
            FREE_IF(new_argv);
            return RESULT_MEMORY_ERROR;
        }
    } else {
        new_argv[*p_argc] = NULL;
    }
    (*p_argc) ++;
    FREE_IF(*p_argv);
    *p_argv = new_argv;
    return RESULT_SUCCESS;
}

/**
 * Returns c string of the PAL error code
 * @param pal_err_code PAL error code
 * @return readable c string of PAL error code
 */
const char* __pal_str_err_code(int pal_err_code)
{
    switch(pal_err_code) {
        case RESULT_SUCCESS:
            return "RESULT_SUCCESS";
        case RESULT_BUFFER_NOT_DEFINED:
            return "RESULT_BUFFER_NOT_DEFINED";
        case RESULT_BUFFER_OVERFLOW:
            return "RESULT_BUFFER_OVERFLOW";
        case RESULT_EMPTY_BUFFER:
            return "RESULT_EMPTY_BUFFER";
        case RESULT_ERROR:
            return "RESULT_ERROR";
        case RESULT_ERROR_INVALID_ARGS:
            return "RESULT_ERROR_INVALID_ARGS";
        case RESULT_ERROR_INVALID_STATE:
            return "RESULT_ERROR_INVALID_STATE";
        case RESULT_ERROR_PAL_NO_INIT:
            return "RESULT_ERROR_PAL_NO_INIT";
        case RESULT_ERROR_PERMISSIONS:
            return "RESULT_ERROR_PERMISSIONS";
        case RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE:
            return "RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE";
        case RESULT_MEMORY_ERROR:
            return "RESULT_MEMORY_ERROR";
        case RESULT_PARSE_ERROR:
            return "RESULT_PARSE_ERROR";
        case RESULT_ERROR_UNDEFINED:
        default:
            return "RESULT_ERROR_UNDEFINED";
    }
}

/*
 * Reallocates memory in data_buffer_t
 * @param thiz pointer to the data_buffer_t structure. thiz->size contains
 * requested size.
 * @param old_size pointer to the value that contains current size of
 * tha allocated memory
 * @return @see enum result_states
 */
int __pal_realloc_data_buffer(data_buffer_t* thiz, size_t* old_size)
{
    CHECK_PVALUE(old_size);
    CHECK_PVALUE(thiz);
    if (thiz->data != NULL && thiz->size == *old_size) {
        WRNPRINT("realloc is not rewuired: new size = %d, old size = %d",
            thiz->size, *old_size);
        return RESULT_SUCCESS;
    }
    void* old_data = thiz->data;
    thiz->data = realloc(thiz->data, thiz->size);
    if (thiz->data == NULL) {
        thiz->data = old_data;
        WRNPRINT("could not reallocate momory from %d to %d",
            *old_size, thiz->size);
        thiz->size = *old_size;
        return RESULT_MEMORY_ERROR;
    }
    return RESULT_SUCCESS;
}

/*
 * Releases memory in data_buffer_t
 * It does not release memory which is used to story data_buffer_t
 * @param thiz pointer to the data_buffer_t structure.
 */
void __pal_free_data_buffer(data_buffer_t* thiz)
{
    if (thiz != NULL) {
        FREE_IF(thiz->data);
        thiz->size = 0;
    }
}


/*
 * concatenates two strings to new path
 * for example:
 * arg0 = "/abc"
 * arg1 = "def"
 * *out is "/abc/def"
 */
char* pal_strcat_path(const char* arg0, const char* arg1, char** out)
{
    if (out == NULL){
        return NULL;
    }
    FREE_IF(*out);
    size_t requested_size = strlen(arg0) + strlen(arg1) + 2;
    *out = (char*)malloc(sizeof(char) * requested_size);
    if (*out == NULL) {
        return NULL;
    }
    snprintf(*out, requested_size, "%s/%s", arg0, arg1);
    return *out;
}

/*
 * reads all requested bytes if it is possible
 */
static int readall(int fd, void* obuf, int requested)
{
    void* obuf_ = obuf;
    int read_bytes = 0;
    int rc = -1;
    do {
        rc = read(fd, obuf_, requested - read_bytes);
        if (rc > 0) {
            read_bytes += rc;
        }
    } while((rc > 0) && (read_bytes < requested));
    return read_bytes;
}

/*
 * returns content of the file as c-string
 */
char* pal_getstr_from_file(const char* path)
{
    int fd = open(path, O_RDONLY);
    int b_size;
    if (fd < 0) {
        WRNPRINT("could not open file: %s", path);
        return NULL;
    }
    struct stat file_info;
    BZEROTYPE(file_info);
    if (stat(path, &file_info) == 0) {
        b_size = file_info.st_size;
        DBGPRINT("%s : size = %d", path, b_size);
    } else {
        if (lseek(fd, 0, SEEK_END) >= 0) {
            b_size = lseek(fd, 0, SEEK_CUR);
        } else {
            b_size = BUFF_SIZE;
        }
        lseek(fd, 0, SEEK_SET);
    }
    DBGPRINT("%s : size = %d", path, b_size);
    int alloc_size = b_size + 1;
    char* buff = (char*)malloc(sizeof(char) * alloc_size);
    int rc = -1;
    rc = readall(fd, (void*)buff, b_size);
    if (rc < 0) {
        FREE_IF(buff);
    } else if (buff != NULL){
        buff[rc] = '\0';
    }
    close(fd);
    return buff;
}

#define READLINK_BUFFSIZE 1024

/*
 * checks is it directory
 */
int pal_is_path_dir(char* path)
{
    struct stat fsStat;
    BZEROTYPE(fsStat);
    if (stat(path, &fsStat) != 0) {
        DIR* dir = opendir(path);
        if (dir != NULL) {
            closedir(dir);
            return 1;
        }
    } else {
        if (S_ISDIR(fsStat.st_mode)) {
            return 1;
        } else if (S_ISLNK(fsStat.st_mode)) {
            char* new_path = (char*)malloc(READLINK_BUFFSIZE * sizeof(char));
            char* old_path = (char*)malloc(READLINK_BUFFSIZE * sizeof(char));
            if (new_path != NULL && old_path != NULL){
                strncpy(old_path, path, READLINK_BUFFSIZE);
                do {
                    bzero(new_path, READLINK_BUFFSIZE * sizeof(char));
                    readlink(old_path, new_path, READLINK_BUFFSIZE);
                    strncpy(old_path, new_path, READLINK_BUFFSIZE);
                    stat(old_path, &fsStat);
                } while(S_ISLNK(fsStat.st_mode));
            } else {
                WRNPRINT("new_path = %p, old_path = %p", new_path, old_path);
            }
            FREE_IF(new_path);
            FREE_IF(old_path);
            if (S_ISDIR(fsStat.st_mode)) {
                return 1;
            }
        }
    }
    return 0;
}

int pal_readlink_recursive(const char* path, char** buff, size_t buff_size)
{
    struct stat fsStat;
    BZEROTYPE(fsStat);
    if (stat(path, &fsStat) != 0) {
        WRNPRINT("stat(%s, &fsStat) != 0", path);
        return RESULT_ERROR;
    } else {
        if (S_ISLNK(fsStat.st_mode)) {
            char* new_path = (char*)malloc(buff_size * sizeof(char));
            char* old_path = (char*)malloc(buff_size * sizeof(char));
            if (new_path != NULL && old_path != NULL){
                strncpy(old_path, path, READLINK_BUFFSIZE);
                do {
                    bzero(new_path, buff_size * sizeof(char));
                    readlink(old_path, new_path, buff_size);
                    strncpy(old_path, new_path, buff_size);
                    if(stat(old_path, &fsStat) != 0) {
                        WRNPRINT("%s : stat(%s, &fsStat) != 0",
                            path, old_path);
                        FREE_IF(new_path);
                        FREE_IF(old_path);
                        return RESULT_ERROR;
                    }
                } while(S_ISLNK(fsStat.st_mode));
                if (buff != NULL) {
                    *buff = new_path;
                } else {
                    FREE_IF(new_path);
                    FREE_IF(old_path);
                    return RESULT_ERROR_INVALID_ARGS;
                }
            } else {
                WRNPRINT("new_path = %p, old_path = %p", new_path, old_path);
                FREE_IF(new_path);
                FREE_IF(old_path);
                return RESULT_MEMORY_ERROR;
            }
            FREE_IF(new_path);
        } else {
            if (buff != NULL) {
                *buff = (char*)path;
            }
        }
    }
    return RESULT_SUCCESS;
}

/*
 * checks is it file
 */
int pal_is_path_file(char* path)
{
    struct stat fsStat;
    BZEROTYPE(fsStat);
    if (stat(path, &fsStat) != 0) {
        int fd = open(path, O_RDONLY);
        if (fd >= 0) {
            close(fd);
            return 1;
        }
    } else {
        if (S_ISREG(fsStat.st_mode)) {
            return 1;
        } else if (S_ISLNK(fsStat.st_mode)) {
            char* new_path = (char*)malloc(READLINK_BUFFSIZE * sizeof(char));
            char* old_path = (char*)malloc(READLINK_BUFFSIZE * sizeof(char));
            if (new_path != NULL && old_path != NULL){
                strncmp(old_path, path, READLINK_BUFFSIZE);
                do{
                    readlink(old_path, new_path, READLINK_BUFFSIZE);
                    strcmp(old_path, new_path);
                    stat(old_path, &fsStat);
                } while(S_ISLNK(fsStat.st_mode));
            } else {
                WRNPRINT("new_path = %p, old_path = %p", new_path, old_path);
            }
            FREE_IF(new_path);
            FREE_IF(old_path);
            if (S_ISREG(fsStat.st_mode)) {
                return 1;
            }
        }
    }
    return 0;
}

int android_get_property_bool(const char* property_name, int32_t* p_value)
{
    int rc = RESULT_ERROR;
    CHECK_PVALUE(p_value);
    char buf[PROPERTY_VALUE_MAX];
    int32_t is_enabled = -1;
    int len = property_get(property_name, buf, "");
    DBGPRINT("property_get(%s, %s, \"\"", property_name, buf);
    if (len == 1) {
        char ch = buf[0];
        if (ch == '0' || ch == 'n') {
            is_enabled = 0;
            rc = RESULT_SUCCESS;
        } else if (ch == '1' || ch == 'y') {
            is_enabled = 1;
            rc = RESULT_SUCCESS;
        } else {
            WRNPRINT("%s: undefined", property_name);
        }
    } else if (len > 1) {
        if (!strcmp(buf, "no") || !strcmp(buf, "false") ||
            !strcmp(buf, "off")) {
            is_enabled = 0;
            rc = RESULT_SUCCESS;
        } else if (!strcmp(buf, "yes") || !strcmp(buf, "true") ||
            !strcmp(buf, "on")) {
            is_enabled = 1;
            rc = RESULT_SUCCESS;
        } else {
            WRNPRINT("%s: undefined", property_name);
        }
    } else {
        WRNPRINT("%s: not found", property_name);
    }
    *p_value = is_enabled;
    return rc;
}

int pal_is_string_int(const char* str)
{
    if (str == NULL || *(str) == '\0') {
        DBGPRINT("null or empty string: return 0;");
        return 0;
    }
    int rc = 1;
    do {
        if ((*str != '\0') && ((*str < '0') || (*str > '9'))) {
            rc = 0;
            break;
        }
    } while(*(str++) != '\0');
    return rc;
}

#define PROC_INFO_BSZ 128

void pal_process_info_log()
{
    FILE* fp = fopen("/proc/self/status", "r");
    if (NULL == fp) {
        WRNPRINT("could open /proc/self/status");
        return;
    }
    char buff[PROC_INFO_BSZ];
    bzero(buff, PROC_INFO_BSZ);
    while(fgets(buff, PROC_INFO_BSZ, fp) != NULL) {
        int __strl = strlen(buff);
        if (__strl > 0) {
            buff[__strl - 1] = '\0';
        }
        DBGPRINT("pros.status: %s", buff);
        bzero(buff, PROC_INFO_BSZ);
    }
    fclose(fp);
}

void pal_meminfo_log()
{
    struct mallinfo mi = mallinfo();
    DBGPRINT("MI:{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d}",
        mi.arena,
        mi.fordblks,
        mi.fsmblks,
        mi.hblkhd,
        mi.hblks,
        mi.keepcost,
        mi.ordblks,
        mi.smblks,
        mi.uordblks,
        mi.usmblks);
}

__END_DECLS
