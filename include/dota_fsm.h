/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_fsm.h
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/26
* History:
***********************************************************************************************************************/


#ifndef __DOTA_FSM_H__
#define __DOTA_FSM_H__

#include "dota_data.h"

typedef enum
{
    RUN_START,                 // 起始状态
    RUN_EXEC_BUFF,             // 清算BUFF
    RUN_FIND_TARGET,           // 寻找目标
    RUN_EXEC_POLICY,           // 执行策略
    RUN_OVER,                  // 结束，该单位不应该再参与调度
    RUN_EXIT,                  // 正常退出

    RUN_BUTT,
}RUN_STATE_E;

typedef struct tag_fsm_node
{
    RUN_STATE_E state;
    OPER1_PF    state_func;
    RUN_STATE_E sucs_st;
    RUN_STATE_E fail_st;
}FSM_NODE_S;

#define LIFE_PHASE 6

typedef struct tag_fsm
{
    RUN_STATE_E cur_st;
    int number;
    FSM_NODE_S *node;
}FSM_S;


int default_run_start(LIFE_S *self);
int default_run_buff(LIFE_S *self);
int default_find_target(LIFE_S *self);
int default_exec_policy(LIFE_S *self);

int fsm_run(FSM_S *fsm, LIFE_S *self);

#endif

