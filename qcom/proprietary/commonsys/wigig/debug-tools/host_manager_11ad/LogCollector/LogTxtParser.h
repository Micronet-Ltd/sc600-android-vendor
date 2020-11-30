/*
* Copyright (c) 2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef LOG_TXT_PARSER_H_
#define LOG_TXT_PARSER_H_


#include <string>
#include <queue>
#include <vector>
#include "LogCollectorDefinitions.h"
#include "OperationStatus.h"

namespace log_collector
{

#define MAX_PARAMS (7)
#define MAX_MODULES (16)
#define MAX_VERBOS_LEVEL (4)
    enum { str_mask = 0xFFFFF };

    typedef union arg_t {
        uintmax_t i;
        double f;
        const void *p;
    } arg;


    class LogTxtParser
    {
    public:
        std::string ParseLogMessage(unsigned int stringOffset, const std::vector<int> &params,
                                    int module, int verbosityLevel, const std::string &timeStamp);
        OperationStatus LogParserInit(const char* stringConversionPath, CpuType cpuType,
                                      struct log_collector::module_level_info* modulesInfo);
        int print_trace_string(std::stringstream &ss, const char *fmt,
            const std::vector<int> &params, const std::string & prefix);
        ~LogTxtParser();
    private:
        std::string ParseMessageFmt(const std::string &prefix, int module, const char* fmt, const std::vector<int> &params);
        char *m_stringsConversionBuffer = nullptr;
        char *ReadStringsConversionFile(const char *fileName, size_t & outRealSize, size_t extra);
        const char *CORRUPTED_PARAM_MARK = "ZZZZ";
        const char *NNL = "[NNL]";
        //int print_trace_string(std::stringstream &ss, const char *fmt, const std::vector<int> &params, const std::string & prefix);
        int printf_core_ss(std::stringstream &ss, const char *fmt, arg_t *nl_arg,
            int *nl_type, const std::string &module_string, const std::vector<int> &params);
        size_t str_sz;
        bool pop_arg(int type, const std::vector<int> & params, unsigned int &lastParamIndex, arg_t& newArg);
        const char* m_levels[MAX_VERBOS_LEVEL] = { "[ERROR]","[WARN ]","[INFO ]","[VERBO]" };
        std::string m_moduleNames[MAX_MODULES + 1];
        struct log_collector::module_level_info *m_modulesInfo;
        const char *next_mod(const char *mod) const;
        void ReadModuleNames();
        const char *GetVerbosityLevelByIndex(int index) const;
        const std::string& GetModuleNameByIndex(int index);
        unsigned char states[3]['z' - 'A' + 1] = { { 0 } };
        void SetStates();
        bool m_newLineRequired;
        //bool m_tralingNlChar;
        int m_prevModule;
    };

}
#endif /* LOG_TXT_PARSER_H_*/
