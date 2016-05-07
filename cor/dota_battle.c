/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_battle.c
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#include <stdio.h>
#include "dota_log.h"
#include "dota_frame.h"

int main()
{
    TRACE_BATTLE("=====BATTLE START=====\n");
    dota_register();
    dota_schedule();
    dota_finish();
    TRACE_BATTLE("=====BATTLE OVER=====\n");

    printf("hello, dota!\n");
    return 0;
}
