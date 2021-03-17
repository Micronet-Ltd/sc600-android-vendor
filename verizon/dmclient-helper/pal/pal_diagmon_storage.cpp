/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dirent.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>

#include <omadm_service_api.h>

#include <pal.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif // #ifdef LOG_TAG

#define LOG_TAG "DiagMon.Storage"

#include "pal_internal.h"

using namespace android;
using namespace omadm_service_api;

#define MOUNT_POINTS_PATH "/proc/mounts"
#define PROC_PARTITIONS "/proc/partitions"
#define BUFF_SIZE 512
#define DEFAULT_BLOCK_SIZE 512

__BEGIN_DECLS

#define __MNT_DIR "/mnt/"
static const char* const __STR_FS_FUSE = "fuse";
#define READ_LINK_SIZE 1024

/* is used to call OmadmService.getDiskUsage(Apps.TYPE_APPS) */
const static int AOSP_TYPE_APPS = 0;
/* is used to call OmadmService.getDiskUsage(Apps.TYPE_CACHE) */
const static int AOSP_TYPE_CACHE = 1;

typedef struct {
    dev_t   dev_id;
    int64_t mtime;
    int64_t blocks;
    int64_t blksize;
} storage_info_t;

typedef struct __pal_storages_s {
    int numTypes;
    int* numStoragesInType;
    char*** mount_paths;
    storage_info_t** dev_infos;
} __pal_storages_t;

static __pal_storages_t* __pal_storages_new();

static void __pal_storages_clear(__pal_storages_t* thiz);

static void __pal_storages_del(__pal_storages_t* thiz);

#define __PAL_STORAGES_DEL(__ARG__) \
    __pal_storages_del(__ARG__);    \
    (__ARG__) = NULL;               \

static const char* __pal_storages_get_mount_path(
    __pal_storages_t* thiz,
    const char* name);

static int __parse_partition_line(const char* buffer,
    int* p_proc_major, int* p_proc_minor)
{
    if ((buffer == NULL) || (p_proc_major == NULL) ||
        (p_proc_minor == NULL)) {
        WRNPRINT("(%p, %p, %p):INV", buffer, p_proc_major, p_proc_minor);
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
        char* end_point;
        *p_proc_major = strtoll(item, &end_point, 10);
        if (*p_proc_major == LONG_MAX || *p_proc_major == LONG_MIN) {
            WRNPRINT("could not convert %s to int32", item);
            FREE_IF(buffer_copy);
            return RESULT_PARSE_ERROR;
        }
    } else {
        WRNPRINT("could not get value");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        char* end_point;
        *p_proc_minor = strtoll(item, &end_point, 10);
        if (*p_proc_major == LONG_MAX || *p_proc_major == LONG_MIN) {
            WRNPRINT("could not convert %s to int32", item);
            FREE_IF(buffer_copy);
            return RESULT_PARSE_ERROR;
        }
    } else {
        WRNPRINT("could not get value");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    FREE_IF(buffer_copy);
    return RESULT_SUCCESS;
}

static int inline __fsinfo_is_device_local_partition(
    const char* mount_point, dev_t* p_div_id)
{
    static size_t STR_SIZE = BUFF_SIZE - 1;
    char* buffer = (char*)malloc(sizeof(char) * BUFF_SIZE);
    if (buffer == NULL) {
        WRNPRINT("could not allocate buffer to read lines");
        return RESULT_MEMORY_ERROR;
    }
    bzero(buffer, sizeof(char) * BUFF_SIZE);
    int line_number = 0;
    struct stat fs_obj_info;
    int arg_major = -1;
    int arg_minor = -1;
    int status = stat(mount_point, &fs_obj_info);
    if (status < 0) {
        WRNPRINT("stat(%s, &fs_obj_info) = %d, errno = %d",
            mount_point, status, errno);
    } else {
        arg_major = DEV_MAJOR(fs_obj_info.st_dev);
        arg_minor = DEV_MINOR(fs_obj_info.st_dev);
        DBGPRINT("MAJ:%d, MIN:%d, (%s)", arg_major, arg_minor, mount_point);
    }
    int proc_major;
    int proc_minor;
    FILE* fp = fopen(PROC_PARTITIONS, "r");
    if (fp != NULL) {
        while(fgets(buffer, sizeof(char) * STR_SIZE, fp) != NULL) {
            if (strlen(buffer) >= STR_SIZE) {
                WRNPRINT("strlen(%s) >= STR_SIZE", buffer);
            }
            if (line_number++ > 1) {
                proc_major = -1;
                proc_minor = -1;
                if (RESULT_SUCCESS !=
                    __parse_partition_line(buffer, &proc_major, &proc_minor)) {
                    WRNPRINT("could not parse line: %s", buffer);
                    break;
                }
                if (arg_major == proc_major && arg_minor == proc_minor) {
                    if (p_div_id != NULL) {
                        *p_div_id = fs_obj_info.st_dev;
                    }
                    fclose(fp);
                    FREE_IF(buffer);
                    return 1;
                }
            }
        }
        fclose(fp);
    } else {
        WRNPRINT("Could not open file " PROC_PARTITIONS);
    }
    FREE_IF(buffer);
    return 0;
}
/*
 * note@ allocates memory to store dev path, mount point, fs type
 */
static int __parse_mount_line(
    const char* buffer,
    char** p_dev_path,
    char** p_mount_point,
    char** p_fs_type)
{
    char* buffer_copy = strdup(buffer);
    if (buffer_copy == NULL) {
        WRNPRINT("could not allocate memory for strdup");
        return RESULT_MEMORY_ERROR;
    }
    char* buffer_d = buffer_copy;
    char* item = NULL;
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        if (p_dev_path != NULL) {
            if ((*p_dev_path = strdup(item)) == NULL) {
                WRNPRINT("could not allocate memory for strdup");
                FREE_IF(buffer_copy);
                return RESULT_MEMORY_ERROR;
            }
        }
    } else {
        WRNPRINT("could not get mount device");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        if (p_mount_point != NULL) {
            if ((*p_mount_point = strdup(item)) == NULL) {
                WRNPRINT("could not allocate memory for strdup");
                FREE_IF(buffer_copy);
                return RESULT_MEMORY_ERROR;
            }
        }
    } else {
        WRNPRINT("could not get mount point");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    item = strtok_r(buffer_d, " ", &buffer_d);
    if (item != NULL) {
        if (p_fs_type != NULL) {
            if ((*p_fs_type = strdup(item)) == NULL) {
                WRNPRINT("could not allocate memory for strdup");
                FREE_IF(buffer_copy);
                return RESULT_MEMORY_ERROR;
            }
        }
    } else {
        WRNPRINT("could not get fs type");
        FREE_IF(buffer_copy);
        return RESULT_PARSE_ERROR;
    }
    FREE_IF(buffer_copy);
    return RESULT_SUCCESS;
}

static int __fsinfo_get_mount_point_at_index(
    int idx, char** p_path, dev_t* p_dev_id)
{
    if (idx < 0) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    int counter = 0;
    size_t buff_size = 0;
    char* buffer = NULL;
    char* mount_point = NULL;
    char* dev_path = NULL;
    char* fs_type = NULL;
    FILE* fp = fopen(MOUNT_POINTS_PATH, "r");
    if (fp != NULL) {
        while(getline(&buffer, &buff_size, fp) >= 0) {
            if (buffer == NULL) {
                WRNPRINT("could not allocate buffer to read lines");
                return RESULT_MEMORY_ERROR;
            }
            FREE_IF(dev_path);
            FREE_IF(mount_point);
            FREE_IF(fs_type);
            buffer[BUFF_SIZE - 1] = '\0';
            if (RESULT_SUCCESS !=
                __parse_mount_line(buffer, &dev_path, &mount_point, &fs_type)) {
                FREE_IF(dev_path);
                FREE_IF(mount_point);
                FREE_IF(fs_type);
                FREE_IF(buffer);
                WRNPRINT("could not parse line: %s", buffer);
                break;
            }
            if (__fsinfo_is_device_local_partition(mount_point, p_dev_id) ||
                (strcmp(__STR_FS_FUSE, fs_type) == 0)) {
                /*
                 * Android device has many emulated sdcards.
                 * I hope this feature will be removed someday.
                 */
                DBGPRINT("storage[%d] : %s", counter, mount_point);
                if (idx == counter) {
                    if (p_path != NULL) {
                        *p_path = strdup(mount_point);
                        if (*p_path == NULL) {
                            fclose(fp);
                            FREE_IF(buffer)
                            FREE_IF(dev_path);
                            FREE_IF(mount_point);
                            FREE_IF(fs_type);
                            return RESULT_MEMORY_ERROR;
                        }
                    }
                    fclose(fp);
                    FREE_IF(buffer)
                    FREE_IF(dev_path);
                    FREE_IF(mount_point);
                    FREE_IF(fs_type);
                    return RESULT_SUCCESS;
                }
                counter ++;
            } else {
                DBGPRINT("NO BLOCK DEV:%s", buffer);
            }
        }
        FREE_IF(dev_path);
        FREE_IF(mount_point);
        FREE_IF(fs_type);
        FREE_IF(buffer)
        fclose(fp);
        if (counter > 0) {
            WRNPRINT("number of storages = %d, idx = %d", counter, idx);
            return RESULT_ERROR_INVALID_ARGS;
        } else {
            WRNPRINT("There are not any storages on the device");
            return RESULT_ERROR;
        }
        return RESULT_SUCCESS;
    } else {
        WRNPRINT("Could not open %s file", MOUNT_POINTS_PATH);
        FREE_IF(dev_path);
        FREE_IF(mount_point);
        FREE_IF(fs_type);
        FREE_IF(buffer)
        return RESULT_ERROR;
    }
    FREE_IF(buffer)
    return RESULT_ERROR;
}

enum __pal_storage_op_code_e {
    PAL_STORAGE_OP_AVAIL = 1,
    PAL_STORAGE_OP_TOTAL = 2,
    PAL_STORAGE_OP_USED = 3,
};

static inline int __fsinfo_op_by_mount_path(
    int op_code,
    const char* mount_point,
    fsinfo_t* p_value)
{
    if (NULL == mount_point) {
        WRNPRINT("mount_point is null");
        return RESULT_ERROR_INVALID_ARGS;
    }
    struct statfs stvf;
    int status = statfs(mount_point, &stvf);
    if (status >= 0) {
        switch(op_code) {
            case PAL_STORAGE_OP_AVAIL:
                p_value->blocks = stvf.f_bfree;
                break;
            case PAL_STORAGE_OP_TOTAL:
                p_value->blocks = stvf.f_blocks;
                break;
            case PAL_STORAGE_OP_USED:
                p_value->blocks = stvf.f_blocks - stvf.f_bfree;
                break;
            default:
                WRNPRINT("wrong op_code = %d", op_code);
                break;
        }
        p_value->block_size = stvf.f_bsize;
        DBGPRINT("statfs(%s, {%lld, %lld})",
            mount_point, p_value->blocks, p_value->block_size);
        return RESULT_SUCCESS;
    }
    WRNPRINT("statfs(%s, ...) = %d, errno = %d", mount_point, status, errno);
    return RESULT_ERROR;
}

static inline int __fsinfo_op(int op_code, int32_t idx, fsinfo_t* p_value)
{
    CHECK_PVALUE(p_value);
    char* mount_point = NULL;
    int status = __fsinfo_get_mount_point_at_index(idx, &mount_point, NULL);
    if (status != RESULT_SUCCESS) {
        FREE_IF(mount_point);
        return status;
    }
    status = __fsinfo_op_by_mount_path(op_code, mount_point, p_value);
    FREE_IF(mount_point);
    DBGPRINT("(%d, %d, {%lld, %lld}) = %d",
        op_code, idx, p_value->blocks, p_value->block_size, status);
    return status;
}

static int __check_and_set_buff_str2str(data_buffer_t* obuf, const char* arg)
{
    CHECK_AND_SET_BUFFER_STR2STR(obuf, arg);
    return RESULT_SUCCESS;
}

static int __get_storage_dev_id_by_path(const char* path, dev_t* p_dev_id)
{
    if (p_dev_id == NULL || path == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    struct stat fs_obj_info;
    int status = stat(path, &fs_obj_info);
    if (status < 0) {
        WRNPRINT("stat(%s, &fs_obj_info) = %d, errno = %d",
            path, status, errno);
        return RESULT_ERROR;
    } else {
        *p_dev_id = fs_obj_info.st_dev;
        DBGPRINT("MAJ:%d, MIN:%d, (%s)",
            DEV_MAJOR(*p_dev_id), DEV_MINOR(*p_dev_id), path);
    }
    return RESULT_SUCCESS;
}

static int __get_storage_dev_id(int idx, dev_t* p_dev_id)
{
    if (p_dev_id == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    return __fsinfo_get_mount_point_at_index(idx, NULL, p_dev_id);
}

static int __get_external_storage_dev_id(dev_t* p_dev_id)
{
    if (p_dev_id == NULL) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    *p_dev_id = 0;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    data_buffer_t ext_path;
    ext_path.data = (char*)malloc(BUFF_SIZE);
    ext_path.size = BUFF_SIZE;
    int rc = IPAL_S_THIS->rpc(PAL_EXT_STORAGE_PATHS_GET, &ext_path);
    if (rc != RESULT_SUCCESS) {
        WRNPRINT("PAL_EXT_STORAGE_PATHS_GET: rc = %d", rc);
        FREE_IF(ext_path.data);
        return rc;
    }
    if (strlen(ext_path.data) > 0) {
        /*
         * If external storage path is empty string then it means
         * that there are not any external storages on the device
         */
        rc = __get_storage_dev_id_by_path(ext_path.data, p_dev_id);
        if (rc != RESULT_SUCCESS) {
            WRNPRINT("stat(%s, &fs_obj_info) = %d", ext_path.data, rc);
        } else {
            DBGPRINT("MAJ:%d, MIN:%d, (%s)",
                DEV_MAJOR(*p_dev_id), DEV_MINOR(*p_dev_id), ext_path.data);
        }
    }
    FREE_IF(ext_path.data);
    return rc;
}

static char* strcat_fs_alloc(const char* str0, char* str1, char** out)
{
    if (out == NULL) {
        return NULL;
    }
    FREE_IF(*out);
    *out = (char *)malloc(strlen(str0) + 1 + strlen(str1) + 1);
    if (*out == NULL) {
        WRNPRINT("FAILED:malloc");
        return NULL;
    }
    **out = '\0';
    sprintf(*out, "%s/", str0);
    strcat(*out, str1);
    return *out;
}

static int64_t __get_du_bytes(
    const char* path, dev_t storage_dev, int64_t* p_blk_sz)
{
    int64_t crntSize = 0;
    struct stat fsObjInfo;
    bzero((void*)(&fsObjInfo), sizeof(fsObjInfo));
    if (stat(path, &fsObjInfo) >= 0) {
        if (fsObjInfo.st_dev == storage_dev) {
            crntSize += (int64_t)(fsObjInfo.st_blocks);
            if (p_blk_sz != NULL) {
                *p_blk_sz = (int64_t)(fsObjInfo.st_blksize);
            }
        } else {
            DBGPRINT("S.DEV:0x%x, F.DEV:0x%llx, F.RDEV:0x%llx : %s",
                storage_dev, fsObjInfo.st_dev, fsObjInfo.st_rdev, path);
        }
    } else {
        WRNPRINT("FAILED:stat(%s, &fsObjInfo), err=%d", path, errno);
    }
    if(S_ISDIR(fsObjInfo.st_mode)){
        DIR* dir;
        dir = opendir(path);
        if (dir != NULL) {
            struct dirent *fileP;
            char* full_path = NULL;
            while ((fileP = readdir(dir)) != NULL) {
                if (strcmp(fileP->d_name, "..") != 0
                    && strcmp(fileP->d_name, ".") != 0) {
                    full_path = strcat_fs_alloc(
                        path, fileP->d_name, &full_path);
                    if (full_path != NULL) {
                        crntSize += __get_du_bytes(
                            full_path, storage_dev, p_blk_sz);
                    } else {
                        WRNPRINT("FAILED:strcat_fs_alloc returns NULL");
                    }
                }
            }
            FREE_IF(full_path);
            closedir(dir);
        } else {
            WRNPRINT("could not open dir:%s, errno = %d", path, errno);
        }
    }
    return crntSize;
}

/*
 * gets dev_id by pal name
 * @return @see enum result_states
 */
static int __pal_storage_get_dev_id(const char* name, dev_t* dev_id)
{
    DBGPRINT("(%s, %p)", name, dev_id);
    CHECK_PVALUE(name);
    CHECK_PVALUE(dev_id);
    __pal_storages_t* thiz = __pal_storages_new();
    const char* mount_path = __pal_storages_get_mount_path(thiz, name);
    if (NULL == mount_path) {
        __PAL_STORAGES_DEL(thiz);
        WRNPRINT("could not find storage by name : \"%s\"", name);
        return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
    }
    struct stat mount_point_info;
    BZEROTYPE(mount_point_info);

    int rc = stat(mount_path, &mount_point_info);
    DBGPRINT("stat(%s, &{%llu}) = %d", mount_path, mount_point_info.st_dev, rc);
    if (rc < 0) {
        WRNPRINT("stat(%s, struct stat*) = %d, errno = %d",
            mount_path, rc, errno);
        rc = RESULT_ERROR;
    } else {
        *dev_id = mount_point_info.st_dev;
    }
    __PAL_STORAGES_DEL(thiz);
    return rc;
}

#define __BYTES_IN_MIB__ 1048576

static int __pal_storages_get_downloads_paths(int* p_argc, char*** p_argv)
{
    CHECK_PVALUE(p_argc);
    CHECK_PVALUE(p_argv);
    size_t data_buffer_size = 256;
    data_buffer_t downloads_path;
    downloads_path.size = data_buffer_size;
    downloads_path.data =
        (char*)calloc(downloads_path.size, sizeof(char));
    if (downloads_path.data == NULL) {
        return RESULT_MEMORY_ERROR;
    }
    int rc = RESULT_BUFFER_OVERFLOW;
    do {
        rc = IPAL_S_THIS->rpc(PAL_DOWNLOADS_PATH_GET, &downloads_path);
        if (rc == RESULT_BUFFER_OVERFLOW) {
            DBGPRINT("buffer too small: %d => %d", data_buffer_size,
                downloads_path.size);
            rc = __pal_realloc_data_buffer(
                &downloads_path, &data_buffer_size);
        }
    } while(rc == RESULT_BUFFER_OVERFLOW);
    if (rc != RESULT_SUCCESS) {
        __pal_free_data_buffer(&downloads_path);
        return rc;
    }
    *p_argc = 1;
    *p_argv = (char**)malloc(sizeof(char**));
    if (NULL == *p_argv) {
        WRNPRINT("memory error");
        __pal_free_data_buffer(&downloads_path);
        return RESULT_MEMORY_ERROR;
    }
    **p_argv = downloads_path.data;
    DBGPRINT("argv[0] = %s", **p_argv);
    return RESULT_SUCCESS;
}

static int __pal_storages_du_by_type_get(
    const char* name, uint32_t code, int64_t* bytes)
{
    DBGPRINT("(%s, %p)", name, bytes);
    CHECK_PVALUE(name);
    CHECK_PVALUE(bytes);
    dev_t storage_dev;
    *bytes = 0L;
    int rc = __pal_storage_get_dev_id(name, &storage_dev);
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
#ifdef HAVE_REAL_SDCARD
    dev_t ext_storage_dev;
    rc = PAL_CHECK_CALL(__get_external_storage_dev_id(&ext_storage_dev));
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    int32_t external = (storage_dev == ext_storage_dev) ? 2 : 1;
#else
    int32_t external = 3;
#endif
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int32_t argc = 0;
    char** argv = NULL;
    switch(code) {
        case PAL_APPS_PATHS_GET:
            rc = IPAL_S_THIS->rpc(PAL_GET_DISK_USAGE, AOSP_TYPE_APPS, bytes);
            break;
        case PAL_CACHE_PATHS_GET:
            rc = IPAL_S_THIS->rpc(PAL_GET_DISK_USAGE, AOSP_TYPE_CACHE, bytes);
            break;
        case PAL_VIDEO_PATHS_GET:
        case PAL_PICTS_PATHS_GET:
        case PAL_AUDIO_PATHS_GET:
            rc = IPAL_S_THIS->rpc(code, external, &argc, &argv);
            break;
        case PAL_DOWNLOADS_PATH_GET: {
            rc = __pal_storages_get_downloads_paths(&argc, &argv);
            break;
        }
    }
    if (rc != RESULT_SUCCESS) {
        __pal_argvs_free(argc, argv);
        return rc;
    }
    if ((0 == *bytes) && (argc > 0)) {
        /*
         * *bytes is still zero and array path is not empty
         * if argc is 0 then argv is NULL too
         */
        int64_t size_in_bytes = 0;
        int32_t path_idx = 0;
        int64_t blksize = 0;
        DBGPRINT("argc = %d", argc);
        for (path_idx = 0; path_idx < argc; path_idx ++) {
            DBGPRINT("argv[%d] = %s", path_idx, argv[path_idx]);
            size_in_bytes += __get_du_bytes(argv[path_idx], storage_dev, &blksize);
        }
        __pal_argvs_free(argc, argv);
        DBGPRINT("blocks = %lld, blksize = %lld", size_in_bytes, blksize);
        size_in_bytes *= DEFAULT_BLOCK_SIZE;
        if ((NULL != bytes) && (argc > 0)) {
            *bytes = size_in_bytes;
        }
    }
    DBGPRINT("(%s, %d, &(%lld))", name, code, *bytes);
    return RESULT_SUCCESS;
}

/**
 * Reads size of the used disk space by applications
 * @param mount_point_path : mount point
 * @param[in] name name of mounted storage
 * @param[out] mbs information about used space by applications on the mounted
 * storage in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Applications
 * @return @see enum result_states
 */
int pal_system_storage_x_used_applications_get(const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    CHECK_PVALUE(mbs);
    if ((NULL == name) || strcasecmp(name, "SDCard0")) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    int64_t bytes = 0L;
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = RESULT_SUCCESS;
    rc = IPAL_S_THIS->rpc(PAL_GET_DISK_USAGE, AOSP_TYPE_APPS, &bytes);
#if 0
    rc = __pal_storages_du_by_type_get(
        name, PAL_APPS_PATHS_GET, &bytes);
#endif
    *mbs = (int32_t)(bytes >> 20);
    DBGPRINT("(%s, &(%d)) = %d", name, (NULL != mbs) ? *mbs : -1, rc);
    return rc;
}

/**
 * Reads size of the used disk space by video
 * and pictures on the selected storage
 * @param[in] name name of mounted storage
 * @param[out] mbs information about used space by video and pictures in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Pictures_Video
 * @return @see enum result_states
 */
int pal_system_storage_x_used_pictures_video_get(
    const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    int64_t mbs_video = 0;
    int64_t mbs_picts = 0;
    int rc = PAL_CHECK_CALL(
        __pal_storages_du_by_type_get(
            name, PAL_VIDEO_PATHS_GET, &mbs_video));
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    rc = PAL_CHECK_CALL(
        __pal_storages_du_by_type_get(
            name, PAL_PICTS_PATHS_GET, &mbs_picts));

    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    *mbs = (int32_t)((mbs_video + mbs_picts) >> 20);
    DBGPRINT("(%s, &(%d)) = %d", name, (NULL != mbs) ? *mbs : -1, rc);
    return rc;
}

/**
 * Reads size of the used disk space by audio files on the selected storage
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Audio
 * @param[in] name name of mounted storage
 * @param[out] mbs space used by audio files in MiBs
 * @return @see enum result_states
 */
int pal_system_storage_x_used_audio_get(const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    int64_t bytes = 0L;
    int rc = PAL_CHECK_CALL(
        __pal_storages_du_by_type_get(name, PAL_AUDIO_PATHS_GET, &bytes));
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    *mbs = (int32_t)(bytes >> 20);
    DBGPRINT("(%s, &(%d)) = %d", name, (NULL != mbs) ? *mbs : -1, rc);
    return rc;
}

/**
 * Reads size of the used disk space by cache on the selected storage
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Cache
 * @param[in] name name of mounted storage
 * @param[out] mbs space used by caches in MiBs
 * @return @see enum result_states
 */
int pal_system_storage_x_used_caches_get(const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    int64_t bytes = 0L;
    CHECK_PVALUE(mbs);
    if ((NULL == name) || strcasecmp(name, "SDCard0")) {
        return RESULT_ERROR_INVALID_ARGS;
    }
    OMADM_SERVICE_INITIALIZED();
    AutoMutex _l(BpOmadmServiceAPI::mPalLock);
    int rc = RESULT_SUCCESS;
    rc = IPAL_S_THIS->rpc(PAL_GET_DISK_USAGE, AOSP_TYPE_CACHE, &bytes);
#if 0
    rc = PAL_CHECK_CALL(
        __pal_storages_du_by_type_get(name, PAL_CACHE_PATHS_GET, &bytes));
#endif
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    *mbs = (int32_t)(bytes >> 20);
    DBGPRINT("(%s, &(%d)) = %d", name, (NULL != mbs) ? *mbs : -1, rc);
    return rc;
}

/**
 * Reads size of the used disk space by downloaded files on the selected storage
 * @param[in] name name of mounted storage
 * @param[out] mbs space used by downloads files in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Downloads
 * @return @see enum result_states
 */
int pal_system_storage_x_used_downloads_get(const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    int64_t bytes;
    int rc = PAL_CHECK_CALL(
        __pal_storages_du_by_type_get(name, PAL_DOWNLOADS_PATH_GET, &bytes));
    if (rc != RESULT_SUCCESS) {
        return rc;
    }
    *mbs = (int32_t)(bytes >> 20);
    DBGPRINT("(%s, &(%d)) = %d", name, (NULL != mbs) ? *mbs : -1, rc);
    return rc;
}

/**
 * Reads size of the used disk space by miscellaneous files
 * on the selected storage
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used/Miscellaneaous
 * @param[in] name name of mounted storage
 * @param[out] mbs space used by miscellaneous in MiBs
 * @return @see enum result_states
 */
int pal_system_storage_x_used_miscellaneaous_get(
    const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    CHECK_PVALUE(name);
    CHECK_PVALUE(mbs);
    int64_t all = 0;
    int64_t apps = 0;
    int64_t video = 0;
    int64_t picts = 0;
    int64_t audio = 0;
    int64_t cache = 0;
    int64_t downloads = 0;
    __pal_storages_t* thiz = __pal_storages_new();
    const char* mount_path = __pal_storages_get_mount_path(thiz, name);
    if (NULL == mount_path) {
        /*
         * name is not null but storage could not found for it
         */
        WRNPRINT("mount_path is null for storage:%s", name);
        __PAL_STORAGES_DEL(thiz);
        return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
    }
    fsinfo_t fsinfo;
    int rc = __fsinfo_op_by_mount_path(
        PAL_STORAGE_OP_USED, mount_path, &fsinfo);
    if (RESULT_SUCCESS == rc) {
        all = fsinfo.blocks * fsinfo.block_size;
    }
    __PAL_STORAGES_DEL(thiz);
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(name, PAL_APPS_PATHS_GET, &apps));
    all -= apps;
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(name, PAL_VIDEO_PATHS_GET, &video));
    all -= video;
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(name, PAL_PICTS_PATHS_GET, &picts));
    all -= picts;
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(name, PAL_AUDIO_PATHS_GET, &audio));
    all -= audio;
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(name, PAL_CACHE_PATHS_GET, &cache));
    all -= cache;
    PAL_RETURN_IF_ERROR(
        __pal_storages_du_by_type_get(
            name, PAL_DOWNLOADS_PATH_GET, &downloads));
    all -= downloads;
    *mbs = (int32_t)(all >> 20);
    DBGPRINT("(%s, &(%d)) : OK", name, *mbs);
    return RESULT_SUCCESS;
}

enum __pal_storage_types{
    __PAL_STORAGE_TYPE_NONE   = -1,
    __PAL_STORAGE_TYPE_SDCARD = 0,
    __PAL_STORAGE_TYPE_USB    = 1,
    __PAL_STORAGE_TYPE_USB11  = 2,
    __PAL_STORAGE_TYPE_USB20  = 3,
    __PAL_STORAGE_TYPE_USB30  = 4,
    __PAL_STORAGE_TYPE_OTHER  = 5,
    __PAL_STORAGE_TYPE_MAX,
};

static const char* __pal_storages_types_str[] = {
    "SDCard", "USB", "USB1.1", "USB2.0", "USB3.0", "Other"
};

typedef int (*__on_mount_point_read)
    (
        __pal_storages_t* thiz,
        const char* dev_path,
        const char* mount_point_path,
        const char* fs_type);

static int __get_all_storages(
    __pal_storages_t* thiz, __on_mount_point_read cb)
{
    int counter = 0;
    char* buffer = NULL;
    size_t buff_size = 0;
    char* fs_type = NULL;
    char* mount_point = NULL;
    char* dev_path = NULL;
    FILE* fp = fopen(MOUNT_POINTS_PATH, "r");
    if (fp != NULL) {
        while(getline(&buffer, &buff_size, fp) >= 0) {
            FREE_IF(dev_path);
            FREE_IF(mount_point);
            FREE_IF(fs_type);
            if (RESULT_SUCCESS !=
                __parse_mount_line(buffer,
                    &dev_path, &mount_point, &fs_type)) {
                WRNPRINT("could not parse line: %s", buffer);
                FREE_IF(dev_path);
                FREE_IF(mount_point);
                FREE_IF(fs_type);
                FREE_IF(buffer);
                buff_size = 0;
                return RESULT_PARSE_ERROR;
                break;
            }
            FREE_IF(buffer);
            buff_size = 0;
            if (cb != NULL) {
                cb(thiz, dev_path, mount_point, fs_type);
            }
        }
        fclose(fp);
        FREE_IF(dev_path);
        FREE_IF(mount_point);
        FREE_IF(fs_type);
        FREE_IF(buffer);
    } else {
        WRNPRINT("Could not open %s file", MOUNT_POINTS_PATH);
        FREE_IF(dev_path);
        FREE_IF(mount_point);
        FREE_IF(fs_type);
        FREE_IF(buffer);
        return RESULT_PARSE_ERROR;
    }
    /*
     * check all directories in the "/mnt/" directory
     */
    DIR* mnt_dir = opendir(__MNT_DIR);
    if (mnt_dir != NULL) {
        char* real_path = NULL;
        struct dirent* entry;
        while ((entry = readdir(mnt_dir))) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") ||
                (strstr(entry->d_name, "sdcard") == NULL)) {
                continue;
            }
            if(pal_strcat_path(__MNT_DIR,
                entry->d_name, &mount_point) != NULL) {
                if (pal_readlink_recursive(
                    mount_point, &real_path, READ_LINK_SIZE) == RESULT_SUCCESS) {
                    if (cb != NULL) {
                        cb(thiz, dev_path, real_path, __STR_FS_FUSE);
                    }
                }

            }
            FREE_IF_2P(mount_point, real_path);
        }
        closedir(mnt_dir);
        mnt_dir = NULL;
    }
    return RESULT_SUCCESS;
}

static int __pal_storage_is_sdcard(const char* mount_point_path, int is_fuse)
{
    if (strcasestr(mount_point_path, "sdcard") != NULL) {
        return 1;
    }
    int rc = 0;
    char* full_path = NULL;
    char* real_path = NULL;
    DIR* dir = opendir(__MNT_DIR);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir))) {
          if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")
              || (strcasestr(entry->d_name, "sdcard") == NULL))
              continue;
          if(pal_strcat_path(__MNT_DIR,
              entry->d_name, &full_path) != NULL) {
            if (RESULT_SUCCESS == pal_readlink_recursive(
                full_path, &real_path, READ_LINK_SIZE)) {
                DBGPRINT("%s : link_to : %s", full_path, real_path);
                if (is_fuse) {
                    if( strstr(mount_point_path, real_path) == mount_point_path) {
                        FREE_IF_2P(full_path, real_path);
                        return 1;
                    }
                }
                struct stat fsStat;
                struct stat fsStatItem;
                BZEROTYPE(fsStat);
                BZEROTYPE(fsStatItem);
                if( stat(mount_point_path, &fsStat) != 0) {
                    WRNPRINT("stat(%s, &fsStat) != 0", mount_point_path);
                    FREE_IF_2P(full_path, real_path);
                    return 0;
                }
                if( stat(real_path, &fsStatItem) != 0) {
                    WRNPRINT("stat(%s, &fsStatItem) != 0", real_path);
                    FREE_IF_2P(full_path, real_path);
                    return 0;
                }
                DBGPRINT("%s : st_dev = %" PRIu64 ", MT{%ld}",
                    mount_point_path, fsStat.st_dev,
                    fsStat.st_mtime);
                DBGPRINT("%s : st_dev = %" PRIu64 ", MT{%ld}",
                    real_path, fsStatItem.st_dev,
                    fsStatItem.st_mtime);
                if (fsStat.st_dev == fsStatItem.st_dev) {
                    FREE_IF_2P(full_path, real_path);
                    DBGPRINT("FOUND_SDCARD:%s", mount_point_path);
                    return 1;
                }

            }
          }
        }
        closedir(dir);
    }
    FREE_IF_2P(full_path, real_path);
    return rc;
}

#define __SYSFS_PATH_BLOCKS_DEV_IDS "/sys/dev/block/"

#define __MAX_PATH_SIZE 1024

#define __SYSFS_PATH_BUS_USB "/sys/bus/usb/devices"

static int __pal_storage_usb_version(const char* usb_sysfs_path)
{
    DBGPRINT("(%s)", usb_sysfs_path);
    int int_version = -1;
    char* sysfs_usb_ver = NULL;
    if (pal_strcat_path(usb_sysfs_path, "version", &sysfs_usb_ver) != NULL) {
        char* version = pal_getstr_from_file(sysfs_usb_ver);
        DBGPRINT("USB: %s : version : %s", usb_sysfs_path, version)
        if (version != NULL) {
            if (strcasestr(version, "1.1")) {
                int_version = 1;
            } else if (strcasestr(version, "2.0")) {
                int_version = 2;
            } else if (strcasestr(version, "3.0")) {
                int_version = 3;
            }
        }
        FREE_IF(version);
    }
    FREE_IF(sysfs_usb_ver);
    return int_version;
}

/*
 * @return 1 + usb version {0: undefined, 1: USB1.1, 2: 2.0, 3: 3.0},
 * 0 : it is not USB storage
 */
static int __pal_storage_is_usb(
    const char* mount_point,
    const char* dev_path)
{
    DBGPRINT("(%s, %s)", mount_point, dev_path);
    if (mount_point == NULL || dev_path == NULL) {
        return 0;
    }
    int arg_major = -1;
    int arg_minor = -1;
    struct stat fs_obj_info;
    BZEROTYPE(fs_obj_info);
    int status = stat(mount_point, &fs_obj_info);
    if (status < 0) {
        WRNPRINT("stat(%s, &fs_obj_info) = %d", mount_point, status);
        char* dev_path_copy = strdup(dev_path);
        char* dev_path_copy_d = dev_path_copy;
        char* item = strtok_r(dev_path_copy_d, ":", &dev_path_copy_d);
        if (item == NULL) {
            FREE_IF(dev_path_copy);
            return 0;
        } else {
            item = strtok_r(dev_path_copy_d, ",", &dev_path_copy_d);
            if (item == NULL) {
                FREE_IF(dev_path_copy);
                return 0;
            } else {
                arg_major = atoi(item);
                arg_minor = atoi(dev_path_copy_d);
            }
            FREE_IF(dev_path_copy);
        }
    } else {
        arg_major = DEV_MAJOR(fs_obj_info.st_dev);
        arg_minor = DEV_MINOR(fs_obj_info.st_dev);
    }
    DBGPRINT("MAJ:%d, MIN:%d, (%s)", arg_major, arg_minor, mount_point);
    if ((arg_major == -1) || (arg_minor == -1)) {
        return 0;
    }
    int sysfs_path_by_dev_id_l = strlen(__SYSFS_PATH_BLOCKS_DEV_IDS) + 21;
    char* sysfs_path_by_dev_id =
        (char*)calloc(sysfs_path_by_dev_id_l + 1, sizeof(char));
    if (sysfs_path_by_dev_id == NULL) {
        WRNPRINT("could not allocate memory for sysfs_path_by_dev_id");
        return 0;
    }
    snprintf(sysfs_path_by_dev_id, sysfs_path_by_dev_id_l,
        __SYSFS_PATH_BLOCKS_DEV_IDS "%d:%d", arg_major, arg_minor);
    char* sysfs_dev_link = (char*)calloc(__MAX_PATH_SIZE, sizeof(char));
    if (sysfs_dev_link == NULL) {
        FREE_IF(sysfs_path_by_dev_id);
        WRNPRINT("could not allocate memory for sysfs_dev_link");
        return 0;
    }
    int rc = readlink(sysfs_path_by_dev_id, sysfs_dev_link, __MAX_PATH_SIZE);
    if (rc == __MAX_PATH_SIZE || rc < 0) {
        WRNPRINT("readlink(%s, char*, %d) = %d",
            sysfs_path_by_dev_id, __MAX_PATH_SIZE, rc);
        FREE_IF(sysfs_path_by_dev_id);
        FREE_IF(sysfs_dev_link);
        return 0;
    }
    rc = 0;
    DBGPRINT("%s => %s", sysfs_path_by_dev_id, sysfs_dev_link);
    char* usb_subpath = NULL;
    if ((usb_subpath = strcasestr(sysfs_dev_link, "/usb")) != NULL) {
        usb_subpath ++;
        DBGPRINT("usb_subpath = %s", usb_subpath);
        char* item = strtok_r(usb_subpath, "/", &usb_subpath);
        DBGPRINT("item = %s", item);
        // "usb3/3-1/3-1:1.0" => "usb3", "3-1/3-1:1.0"
        if (item) item = strtok_r(usb_subpath, "/", &usb_subpath);
        DBGPRINT("item = %s", item);
        // "3-1/3-1:1.0" => "3-1"
        rc = 1;
        if (item != NULL) {
            char* usb_sysfs_path = NULL;
            if (pal_strcat_path(
                __SYSFS_PATH_BUS_USB, item, &usb_sysfs_path) != NULL) {
                DBGPRINT("usb_sysfs_path = %s", usb_sysfs_path);
                rc = __pal_storage_usb_version(usb_sysfs_path) + 1;
                DBGPRINT("usb_sysfs_path = %s : ver = %d",
                    usb_sysfs_path, rc);
            }
            FREE_IF(usb_sysfs_path);
        }
    }
    FREE_IF(sysfs_path_by_dev_id);
    FREE_IF(sysfs_dev_link);
    return rc;
}

static int __pal_is_storage_added(
    __pal_storages_t* thiz,
    const char* mount_point_path,
    storage_info_t storage_info)
{
    DBGPRINT("(%p, %s, {%d, %lld, %lld, %lld})",
        thiz, mount_point_path,
        storage_info.dev_id, storage_info.blocks, storage_info.blksize,
        storage_info.mtime);
    int idx = 0;
    int typeIdx;
    for (typeIdx = 0;
        typeIdx < __PAL_STORAGE_TYPE_MAX;
        typeIdx ++)
    {
        for (idx = 0; idx < thiz->numStoragesInType[typeIdx]; idx ++) {
            char* mount_point_path_item = (thiz->mount_paths[typeIdx])[idx];
            storage_info_t item_info = (thiz->dev_infos[typeIdx])[idx];
            DBGPRINT("item{%s, %d, %lld, %lld, %lld}",
                mount_point_path_item,
                item_info.dev_id, item_info.blocks, item_info.blksize,
                item_info.mtime);
            if ((mount_point_path_item != NULL) &&
                (strcmp(mount_point_path_item, mount_point_path) == 0)) {
                return 1;
            }
            if ((item_info.dev_id == storage_info.dev_id) || (
                memcmp((char*)(&item_info) + sizeof(dev_t),
                    (char*)(&storage_info) + sizeof(dev_t),
                    sizeof(storage_info_t) - sizeof(dev_t)) == 0)) {
                return 1;
            }
        }
    }
    return 0;
}

static int __pal_storages_add_storage_by_type(
    __pal_storages_t* thiz,
    int typeIdx,
    const char* mount_point_path,
    storage_info_t storage_info)
{
    if (0 == __pal_is_storage_added(
        thiz, mount_point_path, storage_info))
    {
        int rc = __pal_argvs_append(
            &(thiz->numStoragesInType[typeIdx]),
            &(thiz->mount_paths[typeIdx]),
            mount_point_path);
        if (rc == RESULT_SUCCESS) {
            storage_info_t* new_dev_infos = (storage_info_t*)realloc(
                thiz->dev_infos[typeIdx],
                sizeof(storage_info_t) * thiz->numStoragesInType[typeIdx]);
            if (new_dev_infos != NULL) {
                new_dev_infos[thiz->numStoragesInType[typeIdx] - 1] = storage_info;
            } else {
                WRNPRINT("could not reallocate memory for dev_infos");
                return RESULT_MEMORY_ERROR;
            }
            thiz->dev_infos[typeIdx] = new_dev_infos;
        }
    } else {
        WRNPRINT("duplicate storage : %s", mount_point_path);
    }
    return RESULT_SUCCESS;
}

/*
 * allocates memory for internal arrays if it is not allocated
 */
static int __pal_storages_init_types(__pal_storages_t* thiz)
{
    if (thiz->numStoragesInType == NULL) {
        thiz->numStoragesInType =
            (int*)calloc(__PAL_STORAGE_TYPE_MAX, sizeof(int));
        if (thiz->numStoragesInType == NULL) {
            WRNPRINT("could not allocate memory for numStoragesInType");
            return RESULT_MEMORY_ERROR;
        }
    }
    if (thiz->dev_infos == NULL) {
        thiz->dev_infos =
            (storage_info_t**)calloc(__PAL_STORAGE_TYPE_MAX, sizeof(storage_info_t*));
        if (thiz->dev_infos == NULL) {
            WRNPRINT("could not allocate memory for dev_infos");
            FREE_IF(thiz->numStoragesInType);
            return RESULT_MEMORY_ERROR;
        }
    }
    if (thiz->mount_paths == NULL) {
        thiz->mount_paths =
            (char***)calloc(__PAL_STORAGE_TYPE_MAX, sizeof(char**));
        if (thiz->mount_paths == NULL) {
            WRNPRINT("could not allocate memory for mount_paths");
            FREE_IF(thiz->numStoragesInType);
            FREE_IF(thiz->dev_infos);
            return RESULT_MEMORY_ERROR;
        }
    }
    return RESULT_SUCCESS;
}

/*
 * adds mounted devices
 */
static int __pal_storages_add_storage(
    __pal_storages_t* thiz,
    const char* dev_path,
    const char* mount_point_path,
    const char* fs_type)
{
    DBGPRINT("(thiz, %s, %s, %s)", dev_path, mount_point_path, fs_type);
    int rc = RESULT_SUCCESS;
    int usb_version = -1;
    __pal_storages_init_types(thiz);
    storage_info_t storage_info;
    BZEROTYPE(storage_info);
    if (mount_point_path != NULL) {
        struct stat stv;
        if (stat(mount_point_path, &stv) >= 0) {
            storage_info.dev_id = stv.st_dev;
            storage_info.mtime = stv.st_mtime;
        }
        struct statfs stvf;
        if (statfs(mount_point_path, &stvf) >= 0) {
            storage_info.blocks = stvf.f_blocks;
            storage_info.blksize = stvf.f_bsize;
        }
    }
    int typeIdx = __PAL_STORAGE_TYPE_NONE;
    if ((mount_point_path != NULL) &&
        (__pal_storage_is_sdcard(mount_point_path,
            strcmp(fs_type, __STR_FS_FUSE) == 0)))
    {
        typeIdx = __PAL_STORAGE_TYPE_SDCARD;
    } else if ((mount_point_path != NULL) &&
        (usb_version = __pal_storage_is_usb(mount_point_path, dev_path)))
    {
        typeIdx = __PAL_STORAGE_TYPE_USB + usb_version - 1;
        DBGPRINT("typeIdx = %d, usb.ver = %d", typeIdx, usb_version);
    } else if ((mount_point_path == NULL) ||
        __fsinfo_is_device_local_partition(mount_point_path, NULL) ||
        (strcmp(__STR_FS_FUSE, fs_type) == 0))
    {
        typeIdx = __PAL_STORAGE_TYPE_OTHER;
    }
    if (typeIdx != __PAL_STORAGE_TYPE_NONE) {
        rc = __pal_storages_add_storage_by_type(
            thiz, typeIdx, mount_point_path, storage_info);
        if (rc != RESULT_SUCCESS) {
            WRNPRINT("could not add sdcard storage");
            return rc;
        }
    } else {
        DBGPRINT("unknown type storage : %s", mount_point_path);
    }
    return rc;
}

/*
 * removes all storages from __pal_storages_t instance
 * @param thiz pointer to the __pal_storages_t instance
 */
static void __pal_storages_clear(__pal_storages_t* thiz)
{
    if (thiz != NULL) {
        int type_idx;
        for (type_idx = 0; type_idx < thiz->numTypes; type_idx) {
            __PAL_FREE_ARGVS(
                thiz->numStoragesInType[type_idx],
                thiz->mount_paths[type_idx]);
            if (thiz->dev_infos != NULL) {
                FREE_IF(thiz->dev_infos[type_idx]);
            }
        }
        FREE_IF(thiz->numStoragesInType);
        FREE_IF(thiz->mount_paths);
        FREE_IF(thiz->dev_infos);
        thiz->numTypes = 0;
    }
}

/*
 * destroys __pal_storages_t instance
 * @param thiz pointer to the __pal_storages_t instance
 */
static void __pal_storages_del(__pal_storages_t* thiz)
{
    __pal_storages_clear(thiz);
    FREE_IF(thiz);
}

/*
 * creates __pal_storages_t with found storages
 * @return instance of __pal_storages_t or NULL if some error occurs
 */
static __pal_storages_t* __pal_storages_new()
{
    __pal_storages_t* thiz = MALLOCTZ(__pal_storages_t);
    int rc = RESULT_SUCCESS;
    if (thiz != NULL) {
        /* add the "Other0" storage as wrong storage */
        __pal_storages_add_storage(thiz, NULL, NULL, NULL);
        rc = __get_all_storages(thiz, __pal_storages_add_storage);
        if (rc != RESULT_SUCCESS) {
            WRNPRINT("could not read all devices");
            __pal_storages_del(thiz);
            return NULL;
        }
    } else {
        WRNPRINT("could not allocacte memory for __pal_storages_t object");
    }
    return thiz;
}

/*
 * calculates required size to store storages names of selected type
 * @param[in] thiz pointer to __pal_storages_t instance
 * @param[in] type of selected storages if it equals -1 than return size to
 * store names of all storages
 */
static int __pal_storages_get_size_str(__pal_storages_t* thiz, int typeIdx)
{
    int strsize = 0;
    if (typeIdx >= 0) {
        if (typeIdx < __PAL_STORAGE_TYPE_MAX) {
            int strlen_type = strlen(__pal_storages_types_str[typeIdx]);
            strsize = (strlen_type + 11) * thiz->numStoragesInType[typeIdx];
        }
    } else {
        for (typeIdx = 0; typeIdx < __PAL_STORAGE_TYPE_MAX; typeIdx ++) {
            strsize += __pal_storages_get_size_str(thiz, typeIdx) - 1;
        }
    }
    return strsize + 1;
}

/*
 * returns c-string with found storage names
 * @param thiz pointer to the instance of the __pal_storages_t
 * @param typeIdx requested type of storages, if it is -1 then
 * return storages all types
 * @param outStr pointer to allocated memory to story storages names
 * if it is NULL, then memory is allocated
 */
static char* __pal_storages_get_names(
    __pal_storages_t* thiz,
    int typeIdx,
    char* outStr)
{
    char* str_names = outStr;
    if (str_names == NULL) {
        int requested_size = __pal_storages_get_size_str(thiz, typeIdx);
        str_names = (char*)calloc(requested_size, sizeof(char));
    }
    char* str_names_d = str_names + strlen(str_names);
    if (typeIdx >= 0) {
        if (typeIdx < __PAL_STORAGE_TYPE_MAX) {
            int storageIdx = 0;
            DBGPRINT("names[typeIdx = %d] = %s",
                typeIdx, __pal_storages_types_str[typeIdx]);
            for (
                storageIdx = 0;
                storageIdx < thiz->numStoragesInType[typeIdx];
                storageIdx ++) {
                if (thiz->mount_paths[typeIdx][storageIdx] != NULL) {
                    str_names_d += strlen(str_names_d);
                    strcat(str_names_d, __pal_storages_types_str[typeIdx]);
                    str_names_d += strlen(str_names_d);
                    if (typeIdx < __PAL_STORAGE_TYPE_USB ||
                        typeIdx > __PAL_STORAGE_TYPE_USB30 ||
                        thiz->numStoragesInType[typeIdx] > 1) {
                        snprintf(str_names_d, 10, "%d", storageIdx);
                    }
                    if (storageIdx < (thiz->numStoragesInType[typeIdx] - 1)) {
                        str_names_d += strlen(str_names_d);
                        *str_names_d = '/';
                    }
                }
            }
        }
    } else {
        for (typeIdx = 0; typeIdx < __PAL_STORAGE_TYPE_MAX; typeIdx ++) {
            __pal_storages_get_names(thiz, typeIdx, str_names_d);
            int append_length = strlen(str_names_d);
            str_names_d += append_length;
            if (typeIdx < (__PAL_STORAGE_TYPE_MAX - 1) && append_length) {
                *str_names_d = '/';
                str_names_d += strlen(str_names_d);
            }
        }
    }
    return str_names;
}

/**
 * Reads list of mounted storages
 * @see ./ManagedObjects/DiagMon/Storage
 * @param[out] list buffer with slash separated device names
 * example: "SDCard0/SDCard1/SDCard2/USB2.0/USB3.0/Other1/Other2"
 * @return @see enum result_states
 */
int pal_system_storages_get(data_buffer_t* list)
{
    DBGPRINT("(%p)", list);
    __pal_storages_t* thiz = __pal_storages_new();
    if (thiz == NULL) {
        WRNPRINT("could not get list of storages");
        return RESULT_ERROR;
    }
    char* storages_names = __pal_storages_get_names(thiz, -1, NULL);
    if (storages_names == NULL) {
        WRNPRINT("could not allocate memory to store temporary list of names");
        __pal_storages_del(thiz);
        return RESULT_MEMORY_ERROR;
    }
    __pal_storages_del(thiz);
    DBGPRINT("storages_names = %s", storages_names);
    int rc = __check_and_set_buff_str2str(list, storages_names);
    FREE_IF(storages_names);
    return rc;
}

/*
 * gets type index by name
 * @return type index,
 * if type was not found than type is __PAL_STORAGE_TYPE_NONE
 */
static int __get_type_by_name(const char* name)
{
    int typeIdx = __PAL_STORAGE_TYPE_MAX;
    if (NULL == name) {
        WRNPRINT("name is null");
        return __PAL_STORAGE_TYPE_NONE;
    }
    do {
        if (strcasestr(name, __pal_storages_types_str[typeIdx]) == name) {
            break;
        }
    } while(--typeIdx > __PAL_STORAGE_TYPE_NONE);
    DBGPRINT("typeIdx[%s] = %d", name, typeIdx);
    return typeIdx;
}

static const char* __pal_storages_get_mount_path(
    __pal_storages_t* thiz,
    const char* name)
{
    if (NULL == thiz) {
        WRNPRINT("thiz is null");
        return NULL;
    }
    char* mount_path = NULL;
    int typeIdx = __get_type_by_name(name);
    if (typeIdx <= __PAL_STORAGE_TYPE_NONE ||
        typeIdx >= __PAL_STORAGE_TYPE_MAX) {
        WRNPRINT("typeIdx[%s] = %d", name, typeIdx);
        return NULL;
    }
    char* storageIdxStr = (char*)name
        + strlen(__pal_storages_types_str[typeIdx]);
    int storageIdx = 0;
    if (strlen(storageIdxStr) > 0) {
        if( pal_is_string_int(storageIdxStr) == 0) {
            storageIdx = -1;
        } else {
            storageIdx = atoi(storageIdxStr);
        }
    } else if (thiz->numStoragesInType[typeIdx] > 1) {
        storageIdx = -1;
    }
    if (storageIdx >= 0 && storageIdx < thiz->numStoragesInType[typeIdx]) {
        mount_path = thiz->mount_paths[typeIdx][storageIdx];
    }
    DBGPRINT("storageIdxStr = %s, storageIdx = %d, mount_path = %s",
        storageIdxStr, storageIdx, mount_path);
    return mount_path;
}

/*
 * Reads information about storage
 * @param[in] op_code :
 *   PAL_STORAGE_OP_TOTAL : gets total size
 *   PAL_STORAGE_OP_AVAIL : gets available size
 *   PAL_STORAGE_OP_USED  : gets used size
 * @param[in] name name of mounted storage
 * @param[out] mbs information about size of storage in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/<OP_CODE>
 * @return @see enum result_states
 */
static int pal_system_storage_x_opcode_get(
    int op_code,
    const char* name, float* mbs)
{
    DBGPRINT("(%d, %s, %p)", op_code, name, mbs);
    CHECK_PVALUE(mbs);
    CHECK_PVALUE(name);
    __pal_storages_t* thiz = __pal_storages_new();
    const char* mount_path = __pal_storages_get_mount_path(thiz, name);
    if (NULL == mount_path) {
        /*
         * name is not null but storage could not found for it
         */
        __PAL_STORAGES_DEL(thiz);
        WRNPRINT("mount_path is null for storage:%s", name);
        return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
    }
    fsinfo_t fsinfo;
    int rc = __fsinfo_op_by_mount_path(
        op_code, mount_path, &fsinfo);
    if (RESULT_SUCCESS == rc) {
        *mbs = (float)(fsinfo.blocks * fsinfo.block_size) / (1024.0f * 1024.f);
    }
    __pal_storages_del(thiz);
    DBGPRINT("(%d, %s, &(%f)) = %d", op_code, name, *mbs, rc);
    return rc;
}

/**
 * Reads size of the storage to the fsinfo pointer
 * @param[in] name name of mounted storage
 * @param[out] mbs information about size of storage in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Total
 * @return @see enum result_states
 */
int pal_system_storage_x_total_get(const char* name, float* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    return pal_system_storage_x_opcode_get(
        PAL_STORAGE_OP_TOTAL, name, mbs);
}

/**
 * Reads free space on the storage
 * @param[in] name name of mounted storage
 * @param[out] mbs information about free space on the file system
 * in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Avail
 * @return @see enum result_states
 */
int pal_system_storage_x_avail_get(const char* name, float* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    return pal_system_storage_x_opcode_get(
        PAL_STORAGE_OP_AVAIL, name, mbs);
}

/**
 * Reads size of used space on the mounted storage
 * @param[in] name name of mounted storage
 * @param[out] mbs information about used space on the mounted
 * storage in MiBs
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Used
 * @return @see enum result_states
 */
int pal_system_storage_x_used_get(const char* name, int32_t* mbs)
{
    DBGPRINT("(%s, %p)", name, mbs);
    float mbs_r4;
    CHECK_PVALUE(mbs);
    int rc = pal_system_storage_x_opcode_get(
        PAL_STORAGE_OP_USED, name, &mbs_r4);
    if (RESULT_SUCCESS == rc) {
        *mbs = (int32_t)mbs_r4;
    }
    return rc;
}

/**
 * Reads free space on the storage
 * @param[in] name name of mounted storage
 * @param[out] percents information about free space on the file system
 * in percents
 * @see ./ManagedObjects/DiagMon/Storage/<X>/Avail
 * @return @see enum result_states
 */
int pal_system_storage_percentfree_get(const char* name, int* percents)
{
    DBGPRINT("(%s, %p)", name, percents);
    CHECK_PVALUE(percents);
    float mbs_r4_avail;
    float mbs_r4_total;
    int rc = pal_system_storage_x_opcode_get(
        PAL_STORAGE_OP_AVAIL, name, &mbs_r4_avail);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("pal_system_storage_x_opcode_get_v2("
        "PAL_STORAGE_OP_AVAIL, %s, float*) = %d", name, rc);
        return rc;
    }
    rc = pal_system_storage_x_opcode_get(
        PAL_STORAGE_OP_TOTAL, name, &mbs_r4_total);
    if (RESULT_SUCCESS != rc) {
        WRNPRINT("pal_system_storage_x_opcode_get_v2("
        "PAL_STORAGE_OP_TOTAL, %s, float*) = %d", name, rc);
        return rc;
    }
    if (mbs_r4_total <= 0.0f) {
        WRNPRINT("(%s):total = %f", name, mbs_r4_total);
        return RESULT_ERROR;
    }
    *percents = (int)(mbs_r4_avail * 100.f/ mbs_r4_total);
    DBGPRINT("(%s, &(%d)) : OK", name, *percents);
    return RESULT_SUCCESS;
}

__END_DECLS
