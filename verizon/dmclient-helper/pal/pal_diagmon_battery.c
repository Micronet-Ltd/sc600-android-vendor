/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pal_internal.h"

__BEGIN_DECLS

typedef struct {
  char* name;
  char* content;
} sysfs_value_t;

typedef struct {
  char* path;
  int size;
  sysfs_value_t** values;
} sysfs_power_supply_t;

typedef struct {
  int32_t size;
  sysfs_power_supply_t** objs;
} sysfs_power_supplies_t;

static sysfs_power_supplies_t* sysfs_power_supplies_new();

static sysfs_value_t* sysfs_value_new(const char* path);

static void sysfs_value_del(sysfs_value_t* thiz);

static void sysfs_power_supply_del(sysfs_power_supply_t* thiz);

static sysfs_power_supply_t* sysfs_power_supply_new(const char* path);

#define POWER_SUPPLY_SUBSYSTEM "power_supply"
#define POWER_SUPPLY_SYSFS_PATH "/sys/class/" POWER_SUPPLY_SUBSYSTEM
#define FAKE_BATTERY_CAPACITY 42
#define FAKE_BATTERY_TEMPERATURE 424
#define ALWAYS_PLUGGED_CAPACITY 100

#if 1
static int quectel_debug = 0;
#endif

static sysfs_power_supplies_t* sysfs_power_supplies_new()
{
    sysfs_power_supplies_t* thiz = MALLOCTZ(sysfs_power_supplies_t);
    char* full_path = NULL;
    DIR* dir = opendir(POWER_SUPPLY_SYSFS_PATH);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir))) {
          if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
              continue;
          if(pal_strcat_path(POWER_SUPPLY_SYSFS_PATH,
              entry->d_name, &full_path) != NULL) {
            if (pal_is_path_dir(full_path)) {
              thiz->size ++;
            }
          }
        }
        closedir(dir);
    } else {
        WRNPRINT("could not open dir:%s", POWER_SUPPLY_SYSFS_PATH);
        FREE_IF(full_path);
        free(thiz);
        return NULL;
    }
    FREE_IF(full_path);
    thiz->objs = (sysfs_power_supply_t**)malloc(thiz->size *
        sizeof(sysfs_power_supply_t*));
    if (NULL == thiz->objs) {
        free(thiz);
        return NULL;
    }
    int argc = 0;
    dir = opendir(POWER_SUPPLY_SYSFS_PATH);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;
            if(pal_strcat_path(POWER_SUPPLY_SYSFS_PATH,
                entry->d_name, &full_path) != NULL) {
                if (pal_is_path_dir(full_path)) {
                    thiz->objs[argc ++] = sysfs_power_supply_new(full_path);
                }
            }
        }
        closedir(dir);
    }
    return thiz;
}

static void sysfs_power_supplies_del(sysfs_power_supplies_t* thiz)
{
    if (thiz != NULL) {
        int idx = 0;
        for (idx = 0; idx < thiz->size; idx ++) {
            sysfs_power_supply_del(thiz->objs[idx]);
        }
        FREE_IF(thiz->objs);
        free(thiz);
    }
}

static sysfs_power_supply_t* sysfs_power_supply_new(const char* path)
{
    DBGPRINT("(%s)", path);
    sysfs_power_supply_t* thiz = MALLOCTZ(sysfs_power_supply_t);
    if (thiz == NULL) {
        DBGPRINT("(%s) thiz is NULL", path);
        return thiz;
    }
    thiz->path = strdup(path);
    if (NULL == thiz->path) {
        free(thiz);
        DBGPRINT("(%s) path is NULL", path);
        return NULL;
    }
    char* full_path = NULL;
    DIR* dir = opendir(path);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;
            if(pal_strcat_path(path, entry->d_name, &full_path) != NULL) {
                if (pal_is_path_file(full_path)) {
                    thiz->size ++;
                }
            }
        }
        closedir(dir);
    }
    FREE_IF(full_path);
    thiz->values = (sysfs_value_t**)malloc(thiz->size *sizeof(sysfs_value_t*));
    if (NULL == thiz->values) {
        free(thiz);
        DBGPRINT("(%s) values is NULL", path);
        return NULL;
    }
    int argc = 0;
    dir = opendir(path);
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;
            if(pal_strcat_path(path, entry->d_name, &full_path) != NULL) {
                if (pal_is_path_file(full_path)) {

                #if 1
                    if(strcmp("/sys/class/power_supply/usb",full_path) == 0)
                    {

                        DBGPRINT("Quectel path (%s) ,quectel_debug = %d", path,quectel_debug);
                        if(quectel_debug == 0)
                        {
                          quectel_debug = 1;
                        }
                    }
                #endif

                    thiz->values[argc ++] = sysfs_value_new(entry->d_name);

                }
            }
        }
        closedir(dir);
    }
    FREE_IF(full_path);
    return thiz;
}

static void sysfs_power_supply_del(sysfs_power_supply_t* thiz) {
    if (thiz != NULL) {
        int idx = 0;
        for (idx = 0; idx < thiz->size; idx ++) {
            sysfs_value_del(thiz->values[idx]);
        }
        FREE_IF(thiz->path);
        FREE_IF(thiz->values);
        free(thiz);
    }
}

sysfs_value_t* sysfs_value_new(const char* name)
{
    sysfs_value_t* thiz = MALLOCTZ(sysfs_value_t);
    if (thiz != NULL) {
        thiz->name = strdup(name);
    }
    return thiz;
}

static void sysfs_value_del(sysfs_value_t* thiz)
{
    if (thiz != NULL) {
        FREE_IF(thiz->name);
        FREE_IF(thiz->content);
        free(thiz);
    }
}

static int sysfs_value_update(sysfs_value_t* thiz, char* path)
{
    if (thiz != NULL) {
        FREE_IF(thiz->content);
        thiz->content = pal_getstr_from_file(path);
        DBGPRINT("%s : %s : %s", thiz->name, path, thiz->content);
        if (thiz->content == NULL) {
         DBGPRINT("(%s) thiz->content is NULL", path);
            return -1;
        }
        return 0;
    }
    return -1;
}

static int sysfs_power_supply_update(sysfs_power_supply_t* thiz, const char* name)
{
    int idx = 0;
    int rc = -1;
    sysfs_value_t* value = NULL;
    DBGPRINT("(%p:%d:%s, %s)", thiz, thiz->size, thiz->path, name);
    char* full_path = NULL;
    for (idx = 0; idx < thiz->size; idx ++) {
        value = thiz->values[idx];
        if (value != NULL && value->name) {
            DBGPRINT("value->name %s, index = %d", value->name,idx);
            if (name != NULL) {
                if (strcasecmp(value->name, name) == 0) {
                    break;
                }
            } else {
                if (pal_strcat_path(thiz->path,
                    value->name, &full_path) != NULL) {
                    rc = sysfs_value_update(value, full_path);
                }
            }
        } else {
            WRNPRINT("value is null at index[%d] supplier : %s",
                idx, thiz->path);
        }
      value = NULL;
    }
    if (value != NULL) {

        DBGPRINT("(thiz->path:%s,value->name:%s,name %s),index = %d", thiz->path,value->name, name,idx);

        if(pal_strcat_path(thiz->path, value->name, &full_path) != NULL) {
            rc = sysfs_value_update(value, full_path);
        }
    }
    FREE_IF(full_path);
    return rc;
}

static sysfs_value_t*
sysfs_power_supply_get_value(sysfs_power_supply_t* thiz, const char* name)
{
    if (thiz == NULL || name == NULL) {
        if(thiz == NULL)
        {
          DBGPRINT("sysfs_power_supply_get_value this is null");
        }
        if(name == NULL)
        {
            DBGPRINT("sysfs_power_supply_get_value name is null");
        }
        return NULL;
    }

    DBGPRINT("sysfs_power_supply_get_value name is %s",name);

    int idx = 0;
    for (idx = 0; idx < thiz->size; idx ++) {
        if ((thiz->values != NULL) &&
            (thiz->values[idx] != NULL) &&
            (thiz->values[idx]->name != NULL) &&
#if 1
            (strcmp(thiz->values[idx]->name, name) == 0)
#else
            (strcasestr(thiz->values[idx]->name, name) != NULL)
#endif

            )
            {

            DBGPRINT("sysfs_power_supply_get_value thiz->values[idx]->name is %s,name %s,index %d",thiz->values[idx]->name,name,idx);
            if(thiz->values[idx]->content != NULL)
            {

              DBGPRINT("sysfs_power_supply_get_value thiz->values[idx]->content is %s",thiz->values[idx]->content);
            }
            else
            {
                DBGPRINT("sysfs_power_supply_get_value thiz->values[idx]->content is NULL");
            }
            return thiz->values[idx];
        }
    }
    return NULL;
}

/*
 * returns array of pointer to power suppliers
 * size of array cannot be greater than quantity af all power suppliers
 * release memory with pointers after use it
 */
static sysfs_power_supply_t** sysfs_power_supplies_search(
    sysfs_power_supplies_t* thiz,
    const char* srch_name,
    const char* srch_value)
{
    sysfs_power_supply_t** power_supplies = NULL;
    int counter = 0;
    if (thiz != NULL) {
        power_supplies = (sysfs_power_supply_t**)
             malloc(thiz->size * sizeof(sysfs_power_supply_t*));
        if (power_supplies == NULL) {
            WRNPRINT("memory error");
            return NULL;
        }
        bzero(power_supplies, thiz->size * sizeof(sysfs_power_supply_t*));
        int idx = 0;
        for (idx = 0; idx < thiz->size; idx ++) {
            sysfs_power_supply_t* power_supply = thiz->objs[idx];
            if (srch_name != NULL) {

                DBGPRINT("srch_name = %s",srch_name);

                if(sysfs_power_supply_update(power_supply, srch_name) >= 0) {

                  sysfs_value_t* value =
                      sysfs_power_supply_get_value(power_supply, srch_name);

                  if(value == NULL)
                  {
                      DBGPRINT("value = %s","sysfs_power_supply_get_value == NULL");
                  }
                  if(value->content == NULL)
                  {
                      DBGPRINT("value content = %s","sysfs_power_supply_get_value == NULL");
                  }
                  else
                  {
                      DBGPRINT("srch_name value = %s path = %s content = %s",srch_name,power_supply->path,value->content);
                  }

                  DBGPRINT("srch_name value = %s path = %s content = %s,srch_value %s",srch_name,power_supply->path,value->content,srch_value);
                  if ((srch_value == NULL) ||
                      (strcasestr(value->content, srch_value) != NULL)) {
                      DBGPRINT("power_supplies[%d] = {%s, %s}",
                          counter, srch_name, srch_value);
                      power_supplies[counter ++] = power_supply;
                  }
                }
            } else {
                DBGPRINT("sysfs_power_supplies_search srch_name is null");
                power_supplies[counter ++] = power_supply;
            }
        }
    } else {
        WRNPRINT("THIZ is NULL");
    }
    DBGPRINT("power_supplies.size = %d", counter);
    return power_supplies;
}

#define SYSFS_BATTERY_TYPE_FILE "type"
#define SYSFS_BATTERY_TYPE_FILE_CONTENT "Battery"

/**
 * Reads number of batteries to the p_value pointer
 * @param[out] p_value *p_value contains number of batteries
 * @see ./ManagedObjects/DiagMon/Battery
 * @return @see enum result_states
 */
int pal_system_batteries_get(int32_t* p_value) {
    CHECK_PVALUE(p_value);
    int counter = -1;
    int idx;
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL) {
        for (idx = 0; (idx < thiz->size) && (suppliers[idx] != NULL); idx ++){
            counter = idx;
        }
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    *p_value = counter + 1;
    DBGPRINT("(%d) = 0", *p_value);
    return RESULT_SUCCESS;
}

/**
 * Reads current battery temperature to the iobuf->data buffer as C-string
 * @param[in] battery_idx battery number
 * @param[out] p_val
 * @parblock
 *   outbuf->data buffer contains C-string
 *   example: "84.0 F / 28.9 C"
 * @endparblock
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Temp
 * @return @see enum result_states
 */
int pal_system_battery_x_temp_get(int idx, data_buffer_t* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    CHECK_PVALUE(p_val);
    int rc = RESULT_ERROR;
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        sysfs_power_supply_update(suppliers[idx], "temp");
        sysfs_value_t* value = sysfs_power_supply_get_value(suppliers[idx], "temp");
        if (value != NULL && value->content != NULL) {
            float temp_c = ((float)atoi(value->content)) * 0.1f;
            float temp_f = temp_c * 1.8f + 32.0f;
            char temp_buffer[64];
            snprintf(temp_buffer, 64, "%.1f F / %.1f C", temp_f, temp_c);
            DBGPRINT("TMP_BUFF:%s", temp_buffer);
            if (p_val == NULL) {
                rc = RESULT_ERROR_INVALID_ARGS;
            } else if (p_val->data == NULL) {
                rc = RESULT_EMPTY_BUFFER;
            } else if (p_val->size < (strlen(temp_buffer) + 1)) {
                rc = RESULT_BUFFER_OVERFLOW;
            } else {
                strcpy(p_val->data, temp_buffer);
                rc = RESULT_SUCCESS;
            }
        } else {
            WRNPRINT("value not found");
        }
    } else {
        WRNPRINT("supplier not found");
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, %s) = %d", idx,
        (rc == RESULT_SUCCESS ? p_val->data : ""), rc);
    return rc;
}

/**
 * Reads current battery level to the p_value pointer
 * @param[in] battery_idx number of battery
 * @param[out] out battery level in can ve from 0.0f to 100.0f
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Level
 * @return @see enum result_states
 */
int pal_system_battery_x_level_get(int idx, float* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    CHECK_PVALUE(p_val);
    int rc = RESULT_ERROR;
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        sysfs_power_supply_update(suppliers[idx], "capacity");
        sysfs_value_t* value = sysfs_power_supply_get_value(suppliers[idx],
            "capacity");
        if (value != NULL && value->content != NULL) {
            *p_val = ((float)atoi(value->content));
            rc = RESULT_SUCCESS;
        } else {
            WRNPRINT("value not found");
        }
    } else {
        WRNPRINT("supplier not found");
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, %p) = %d", idx, p_val, rc);
    return rc;
}

/**
 * Reads current battery status to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Status
 * @param[in] battery_idx number of battery
 * @param[out] p_value
 * @parblock
 * pointer to store current battery status
 *   0 : Charging
 *   1 : Discharging
 *   2 : Charging Complete
 *   3 : Unknown
 *   4 : Not Installed
 * @endparblock
 * @return @see enum result_states
 */
int pal_system_battery_x_status_get(int idx, int32_t* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    CHECK_PVALUE(p_val);
    const static char* read_name = "status";
    CHECK_PVALUE(p_val);
    int rc = RESULT_SUCCESS;
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        sysfs_power_supply_update(suppliers[idx], read_name);
        sysfs_value_t* value = sysfs_power_supply_get_value(suppliers[idx], read_name);
        if (value != NULL && value->content != NULL) {
            if (STRCASESTART(value->content, "Charging")) {
                *p_val = 0;
            } else if (STRCASESTART(value->content, "Discharging")) {
                *p_val = 1;
            } else if (STRCASESTART(value->content, "Full")) {
                *p_val = 2;
            } else {
                *p_val = 3;
            }
        } else {
            WRNPRINT("value not found");
        }
    } else {
        DBGPRINT("supplier not found");
        *p_val = 4;
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, %p) = %d", idx, p_val, rc);
    return rc;
}

/**
 * Reads battery type to the iobuf->data buffer as C-string
 * @param[in] battery_idx number of battery
 * @param[out] iobuf iobuf->data contains type of selected battery as C-string
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Type
 * @return @see enum result_states
 */
int pal_system_battery_x_type_get(int idx, data_buffer_t* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    CHECK_PVALUE(p_val);
    const static char* read_name = "technology";
    int rc = RESULT_ERROR;
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        sysfs_power_supply_update(suppliers[idx], read_name);
        sysfs_value_t* value = sysfs_power_supply_get_value(suppliers[idx], read_name);
        if (value != NULL && value->content != NULL) {
            if ((strlen(value->content) > 0) &&
                ((value->content[strlen(value->content) - 1] == '\n') ||
                    ((value->content[strlen(value->content) - 1] == '\r')))
                ) {
                value->content[strlen(value->content) - 1] = '\0';
            }
            if (p_val == NULL) {
                rc = RESULT_ERROR_INVALID_ARGS;
            } else if (p_val->data == NULL) {
                rc = RESULT_EMPTY_BUFFER;
            } else if (p_val->size < (strlen(value->content) + 1)) {
                rc = RESULT_BUFFER_OVERFLOW;
            } else {
                strcpy(p_val->data, value->content);
                rc = RESULT_SUCCESS;
            }
        } else {
            WRNPRINT("value not found");
        }
    } else {
        WRNPRINT("supplier not found");
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, %p) = %d", idx, p_val, rc);
    return rc;
}

/**
 * Reads current voltage of battery in mV
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Voltage
 * @param[in] battery_idx number of battery
 * @param[out] p_value *p_value : current voltage of battery in mV
 * @return @see enum result_states
 */
int pal_system_battery_x_voltage_get(int idx, int32_t* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    CHECK_PVALUE(p_val);
    int rc = RESULT_ERROR;
    const static char* read_name = "voltage_now";
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, SYSFS_BATTERY_TYPE_FILE_CONTENT);
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        sysfs_power_supply_update(suppliers[idx], read_name);
        sysfs_value_t* value = sysfs_power_supply_get_value(suppliers[idx], read_name);
        if (value != NULL && value->content != NULL) {
            if (p_val == NULL) {
                rc = RESULT_ERROR_INVALID_ARGS;
            } else {
                int muV = atoi(value->content);
                *p_val = muV / 1000;
                rc = RESULT_SUCCESS;
            }
        } else {
            WRNPRINT("value not found");
        }
    } else {
        WRNPRINT("supplier not found");
    }
    FREE_IF(suppliers);
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, %p) = %d", idx, p_val, rc);
    return rc;
}

/**
 * Reads plug status to the p_value pointer
 * @see ./ManagedObjects/DiagMon/Battery/<X>/Plug_Status
 * @param[in] battery_idx number of battery
 * @param[out] p_value
 * @parblock
 * pointer to store current battery status
 *   0 : AC
 *   1 : USB
 *   2 : Wireless
 *   3 : Other
 *   4 : Unplugged
 *   5 : Unknown
 * @endparblock
 * @return @see enum result_states
 */
int pal_system_battery_x_plug_status_get(int idx, int32_t* p_val)
{
    DBGPRINT("(%d, %p)", idx, p_val);
    int rc = RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
    CHECK_PVALUE(p_val);
    sysfs_power_supplies_t* thiz = sysfs_power_supplies_new();
    if (NULL == thiz) {
        WRNPRINT("sysfs_power_supplies_new() returned NULL");
        return RESULT_MEMORY_ERROR;
    }
    DBGPRINT("thiz->size = %d", thiz->size);
    *p_val = 5;
    sysfs_power_supply_t** suppliers =
        sysfs_power_supplies_search(thiz,
            SYSFS_BATTERY_TYPE_FILE, "AC");
    if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL) {
        *p_val = 0;
        rc = RESULT_SUCCESS;
    }
    FREE_IF(suppliers);
    if (rc != 0) {
        suppliers =
            sysfs_power_supplies_search(thiz,
                SYSFS_BATTERY_TYPE_FILE, "USB");
        if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL)
        {
            *p_val = 1;
            rc = RESULT_SUCCESS;
        }
        FREE_IF(suppliers);
    }
    if (rc != 0) {
        suppliers =
            sysfs_power_supplies_search(thiz,
                SYSFS_BATTERY_TYPE_FILE, "WIRELESS");
        if (suppliers != NULL && (idx < thiz->size) && suppliers[idx] != NULL)
        {
            *p_val = 2;
            rc = RESULT_SUCCESS;
        }
        FREE_IF(suppliers);
    }
    rc = RESULT_SUCCESS;
    sysfs_power_supplies_del(thiz);
    DBGPRINT("(%d, &(%d)) = %d", idx, *p_val, rc);
    return rc;
}

__END_DECLS
