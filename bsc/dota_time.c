/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_time.c
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#include "dota_time.h"

/* 
 * 时间精度是100毫秒，每加1表示时间流逝100毫秒，
 * 其实说成每加1表示一轮回合结束更合适。比如某
 * 个效果持续2秒，即2000毫秒，那说成持续20个回合
 * 更贴切。
 */
static unsigned long long g_dota_time = 1;

/* 每调用一次该函数，时间流逝100毫秒 */
void time_run(void)
{
    g_dota_time++;
    return;
}

/* 获取时间的函数，返回当前时间值 */
unsigned long long get_current_time(void)
{
    return g_dota_time;
}
