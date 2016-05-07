/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_log.h
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/

#ifndef __DOTA_LOG_H__
#define __DOTA_LOG_H__

#if defined(__linux__)
#define BATTLE_LOG "log/battle.log"
#else
#define BATTLE_LOG "battle.log"
#endif

#define TIME_FORMAT "[%04llu]"
#define LOG_MAX_LEN 1024

#define INFO_FORMAT     "[INFO]"
#define NORMAL_FORMAT   "[NORMAL]"
#define WARNNING_FORMAT "[WARNNING]"
#define ERROR_FORMAT    "[ERROR]"

#define TRACE_BATTLE trace_battle

/* DOTA中记录日志函数，分四个级别 */
void trace_battle(const char *fmt, ...);
#endif

