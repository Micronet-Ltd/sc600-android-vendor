/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include <android/log.h>
#include <ctype.h>
#include <dlfcn.h>
#include <expat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "pal.h"
#include "pal_fumo_cfg.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FUMO DDP"

pal_download_descriptor_t **descriptor = NULL;

/**
 * DO NOT CHANGE order of this tags!
 * There are 14 avilable tags below
 */
const char *tags[] = {"objectURI", "size", "type",
             "name", "vendor", "installParam", "description",
             "PreDownloadMessage","PostDownloadMessage",
             "PreDownloadURL","PostDownloadURL",
             "PostUpdateMessage","PostUpdateURL",
             "RequiredInstallParameter", NULL};
short int paramNo = -1;
short int callbackError = 0;
int depth = 0;

/**
 * Check is tag avilable
 * @param tag xml tag
 * @return number of tag if OK
 *         -1 if ERROR
 */
int is_avilable(const char *tag)
{
    int i = 0;
    for(i = 0;;i++){
        if(NULL == tags[i])
            return -1;
        if(0 == strcmp(tags[i], tag)){
            return i;
        }
    }
    return -1;
}

/**
 * Check is mandatory fields exist
 * @return 1 if OK
 *         0 if not
 */
static int check_mandatory_fields()
{
    if (NULL == descriptor)
        return 0;
    if (NULL != (*descriptor)->packageName &&
        NULL != (*descriptor)->objectUri)
        return 1;
    else
        return 0;
}

/**
 * Callback for tags looks like <tag>
 * @param data not used
 * @param tag name of current tag
 * @param attr parameters of current tag
 */
static void start_handler(void *data, const char *tag, const char **attr)
{
    (void) data;
    (void) attr;
    if(NULL == descriptor){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Descriptor does not exist");
        return;
    }
    depth++;
    if (depth > 2) {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Too much depth: %d", depth);
        callbackError = 1;
        return;
    }
    paramNo = is_avilable(tag);
}

/**
 * Callback for tags looks like </tag>
 * @param data not used
 * @param tag current tag
 */
static void end_handler(void *data, const char *tag)
{
    (void) data;
    (void) tag;
    depth--;
    paramNo = -1;
    if (depth < 0)
        callbackError = 1;
}

/**
 * Truncate invisible symbols from the begin and from the end of line
 * @param line[out][in] line to truncate
 */
static void truncate_spaces(char **line)
{
    if (NULL == (*line) || 0 == strlen(*line))
        return;
    int i_b = 0;// begin index
    int i_e = strlen(*line);// end index
    while(isspace((*line)[i_b])) i_b++;
    while(isspace((*line)[i_e])) i_e--;
    if(0 == i_b && strlen(*line) - 1 == i_e) return;//there are not spaces
    int new_strlen = i_e - i_b;
    char buf[new_strlen + 1];
    int i;
    for (i = i_b; i < i_e; i++) {
        buf[i - i_b] = (*line)[i];
    }
    buf[new_strlen]='\0';
    free(*line);
    *line = strdup(buf);
}

/**
 * Append one line to another
 * @param dst[out][in] designation
 * @param append[in] last part of line
 */
static void append_line(char **dst, char *append)
{
    if(NULL == *dst || NULL == append || 0 == strlen(append)) return;
    char *new_str = NULL;
    int len = strlen(*dst) + strlen(append) + 2;
    if ((new_str = calloc(1, len * sizeof(char))) != NULL) {
        strcat(new_str, *dst);
        strcat(new_str, " ");
        strcat(new_str, append);
        free(*dst);
        *dst = strdup(new_str);
        free(new_str);
    }
    return;
}

/**
 * Concatenate message from different lines
 * @param message[out][in] existing message
 * @param part[in] next line
 */
static void generate_message(char **message, char **part){
    truncate_spaces(part);
    if (NULL == *message)
        *message = strdup(*part);
    else {
        append_line(message, *part);
    }
}

/**
 * Fill the pal_download_descriptor_t structure
 * @param data not used
 * @param text text into tag
 * @param textlen length of the text into tag
 */
static void char_handler(void *data, const char *text, int textlen)
{
    (void) data;
    if(NULL == descriptor)
        return;
    char *str = strndup(text, textlen);
    if(NULL == str)
        return;

    if (paramNo == -1) {
        //return
    } else if (paramNo == 0) {
        (*descriptor)->objectUri = strdup(str);// all fields in structure can be NULL
                                               // nullcheck is not reqired
    } else if (paramNo == 1) {
        (*descriptor)->size = strtoul (text, NULL, 10);
    } else if (paramNo == 2) {
        (*descriptor)->type = strdup(str);
    } else if (paramNo == 3) {
        (*descriptor)->packageName = strdup(str);
    } else if (paramNo == 4) {
        (*descriptor)->vendor = strdup(str);
    } else if (paramNo == 5) {
        (*descriptor)->installParam = strdup(str);
    } else if (paramNo == 6) {
        (*descriptor)->description = strdup(str);
    } else if (paramNo == 7) {
        generate_message(&((*descriptor)->preDownloadMessage), &str);
    } else if (paramNo == 8) {
        generate_message(&((*descriptor)->postDownloadMessage), &str);
    } else if (paramNo == 9) {
        (*descriptor)->preDownloadURL = strdup(str);
    } else if (paramNo == 10) {
        (*descriptor)->postDownloadURL = strdup(str);
    } else if (paramNo == 11) {
        generate_message(&((*descriptor)->postUpdateMessage), &str);
    } else if (paramNo == 12) {
        (*descriptor)->postUpdateURL = strdup(str);
    } else if (paramNo == 13) {
        (*descriptor)->requiredInstall = strtoul (text, NULL, 10);
    }
    free(str);
}

/**
 * Fills descriptor structure from a file on local file system
 * @param[in] filename name of file in CWD
 *            if NULL default file name will be used
 * @param[out] out Download Descriptor. MUST free manually after usage!
 * @return PAL_RC_FRMW_DOWNLOAD_SUCCESS of OK
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY if there is not memory for parser or
 *                                  if there is not memory for descriptor or
 *                                  cannot set dynamic link to libexpat.so
 *         PAL_RC_FRMW_DOWNLOAD_ERROR if file with "filename" does not exist
 *         PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR if file with "filename"
 *                                                   contains some errors
 */
int pal_create_download_descriptor(const char *filename,
                                   pal_download_descriptor_t **out)
{
    XML_Parser (*XML_ParserCreate)(XML_Char);
    void (*XML_UseParserAsHandlerArg)(XML_Parser);
    void (*XML_SetElementHandler)(XML_Parser, XML_StartElementHandler,
                                  XML_EndElementHandler);
    void (*XML_SetCharacterDataHandler)(XML_Parser, XML_CharacterDataHandler);
    int (*XML_Parse)(XML_Parser, const char*, int, int);

    void *expat_handle = dlopen("libexpat.so", RTLD_LAZY);
    dlerror();//clear error buffer
    if(NULL == expat_handle){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"Cannot open libexpat.so!");
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    } else {
        char *error;
        XML_ParserCreate = dlsym(expat_handle, "XML_ParserCreate");
        XML_UseParserAsHandlerArg = dlsym(expat_handle,
                                          "XML_UseParserAsHandlerArg");
        XML_SetElementHandler = dlsym(expat_handle, "XML_SetElementHandler");
        XML_SetCharacterDataHandler = dlsym(expat_handle,
                                            "XML_SetCharacterDataHandler");
        XML_Parse = dlsym(expat_handle, "XML_Parse");
        if (NULL != (error = dlerror()) ||
            NULL == XML_ParserCreate ||
            NULL == XML_UseParserAsHandlerArg ||
            NULL == XML_SetElementHandler ||
            NULL == XML_SetCharacterDataHandler ||
            NULL == XML_Parse) {
            dlclose(expat_handle);
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,"Expat error: %s", error);
            //ALOGE("%s\n", error);
            return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
        }
    }
    XML_Parser parser = XML_ParserCreate((char)NULL);
    if (NULL == parser) {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Couldn't allocate memory for parser");
        dlclose(expat_handle);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }

    *out = calloc(1, sizeof(pal_download_descriptor_t));
    if(NULL == *out){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Couldn't allocate memory for descriptor");
        dlclose(expat_handle);
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    }
    descriptor = out;

    FILE *file;
    const int MAXFILEPATH = 4095;
    char fileToOpen[MAXFILEPATH];
    fileToOpen[0] = '\0';
    if (MAXFILEPATH < strlen(MO_WORK_PATH) + strlen(MO_DATA_PFX) + 3)// "/"+ "/"+'\0'
        return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
    strcat(fileToOpen, MO_WORK_PATH);
    strcat(fileToOpen, "/");
    strcat(fileToOpen, MO_DATA_PFX);
    strcat(fileToOpen, "/");

    if(NULL == filename){
        if (MAXFILEPATH < strlen(fileToOpen)
                        + strlen(DOWNLOAD_DESCRIPTOR_FILE_NAME) + 1) {
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Too long file path");
            dlclose(expat_handle);
            return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
        }
        strcat(fileToOpen, DOWNLOAD_DESCRIPTOR_FILE_NAME);
        file = fopen(fileToOpen, "r");
        chmod(fileToOpen,0777);
    } else {
        if (MAXFILEPATH < strlen(fileToOpen)
                        + strlen(filename) + 1) {
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Too long file path");
            dlclose(expat_handle);
            return PAL_RC_FRMW_DOWNLOAD_ERROR_MEMORY;
        }
        strcat(fileToOpen, filename);
        file = fopen(fileToOpen, "r");
        chmod(fileToOpen,0777);
    }

    if(NULL == file){
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "File %s does not exist", fileToOpen);
        *out = NULL;
        dlclose(expat_handle);
        return PAL_RC_FRMW_DOWNLOAD_ERROR;
    }

    XML_UseParserAsHandlerArg(parser);
    XML_SetElementHandler(parser, start_handler, end_handler);
    XML_SetCharacterDataHandler(parser, char_handler);
    char buff[DOWNLOAD_DESCRIPTOR_MAX_FILE_LENGTH];

    int done;
    do {
        int len;
        fgets(buff, sizeof(buff), file);
        len = strlen(buff);
        if (0 == len) {
            fclose(file);
            dlclose(expat_handle);
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Cannot parse download descriptor");
            return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR;
        }
        done = feof(file);
        int err = XML_Parse(parser, buff, len, done);
        if (err < 0) {
            // Get number of line with error:
            //    int numline =  XML_GetCurrentLineNumber(parser),
            // Get error code:
            //    int errcode = XML_ErrorString(XML_GetErrorCode(parser)));
            // do not forget to open this via dlopen()
            dlclose(expat_handle);
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Cannot parse download descriptor: syntax error");
            return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR;
        }
        if (err == 0)
            break;
    } while (0 == done);
    fclose(file);
    dlclose(expat_handle);

    if(!check_mandatory_fields()){
        return PAL_RC_FRMW_DOWNLOAD_ERROR_CANNOT_PARSE_DESCRIPTOR;
    }

    return PAL_RC_FRMW_DOWNLOAD_SUCCESS;
}

/**
 * Free memory, allocated for pal_download_descriptor_t structure
 * @param dd pointer to structure
 */
void free_download_descriptor_t(pal_download_descriptor_t **dd)
{
    if(NULL == (*dd)) return;
    free((*dd)->packageName);
    (*dd)->packageName = NULL;
    free((*dd)->objectUri);
    (*dd)->objectUri = NULL;
    (*dd)->size = 0;
    free((*dd)->type);
    (*dd)->type = NULL;
    free((*dd)->vendor);
    (*dd)->vendor = NULL;
    free((*dd)->description);
    (*dd)->description = NULL;
    free((*dd)->installParam);
    (*dd)->installParam = NULL;
    free((*dd)->preDownloadMessage);
    (*dd)->preDownloadMessage = NULL;
    free((*dd)->postDownloadMessage);
    (*dd)->postDownloadMessage = NULL;
    free((*dd)->preDownloadURL);
    (*dd)->preDownloadURL = NULL;
    free((*dd)->postDownloadURL);
    (*dd)->postDownloadURL = NULL;
    free((*dd)->postUpdateURL);
    (*dd)->postUpdateURL = NULL;
    free((*dd)->postUpdateMessage);
    (*dd)->postUpdateMessage = NULL;
    (*dd)->requiredInstall = 0;
    free(*dd);
    (*dd) = NULL;
}
