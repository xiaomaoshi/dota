/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_log.c
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dota_time.h"
#include "dota_log.h"

#define DEBUG_INFO     3
#define DEBUG_NORMAL   2
#define DEBUG_WARNNING 1
#define DEBUG_ERROR    0

/* 日志的默认级别为NORMAL */
unsigned int g_debug_level = DEBUG_NORMAL;

static char * g_debug_str[] = 
{
    ERROR_FORMAT,
    WARNNING_FORMAT,
    NORMAL_FORMAT,
    INFO_FORMAT,
    NULL,
};

void trace_battle(const char *fmt, ...)
{
    char buff[LOG_MAX_LEN] = {0};
    va_list arg_list = NULL;
    FILE *file = NULL;
    unsigned int loop = 0;
    unsigned int level = DEBUG_NORMAL;
    char **debug_str = g_debug_str;
 
    /*
     * 根据fmt传入的参数，来判断该条日志的级别，如果该条
     * 日志未使用级别，那默认是NORMAL
     */
    for (; NULL != (*debug_str); debug_str++, loop++) {
        if (0 == strncmp(fmt, *debug_str, strlen(*debug_str))) {
            level = loop;
            break;
        }
    }

    if (level > g_debug_level)
        return;

    file = fopen(BATTLE_LOG, "a+");
    if (NULL == file)
        return;

    va_start(arg_list, fmt);

#if defined(__linux__)
    vsnprintf(buff, LOG_MAX_LEN, fmt, arg_list);
#else
    _vsnprintf(buff, LOG_MAX_LEN, fmt, arg_list);
#endif
    /* 记录日志时会加入时间信息 */ 
    fprintf(file, TIME_FORMAT "%s", get_current_time(), buff);
    va_end(arg_list);
    fclose(file);
    return;
}

