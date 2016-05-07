/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_fsm.c
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/26
* History:
***********************************************************************************************************************/

#include "dota_fsm.h"
#include "dota_data.h"
#include "dota_life.h"
#include "dota_log.h"


int default_run_start(LIFE_S *self)
{
    if (IS_DEAD(self)) {
        return DOTA_FAILED;
    }

    return DOTA_OK;
}


int default_run_buff(LIFE_S *self)
{
    OPERATE_S *self_ops;

    self_ops = self->ops;
    self_ops->buff(self);

    TRACE_BATTLE("%s: Health %u, Mana %u, Armor %u, Attack %u, Money %u.\n",
                 self->name,
                 self->cur_hmaa.health,
                 self->cur_hmaa.mana,
                 self->cur_hmaa.armor,
                 self->cur_hmaa.attack,
                 self->money);

    if (IS_DEAD(self)) {
        return DOTA_FAILED;
    }

    return DOTA_OK;
}


int default_find_target(LIFE_S *self)
{
    LIFE_S * target;

    target = self->target;
    if ((!target) || IS_DEAD(target)) {
        target = find_enemy(self, self->skills[PY_ATTK].attr.dis);
        DOTA_RETURN_IF_NULL(target, DOTA_FAILED);
        self->target = target;
    }

    return DOTA_OK;
}

/* 默认的攻击策略就是无脑砍 */
int default_exec_policy(LIFE_S *self)
{
    OPERATE_S *self_ops;

    self_ops = self->ops;
    if (DOTA_SUCCESS == self_ops->attack(self)) {
        return DOTA_OK;
    }

    return DOTA_FAILED;
}


int fsm_run(FSM_S *fsm, LIFE_S *self)
{
    RUN_STATE_E state = RUN_BUTT;
    FSM_NODE_S *node;
    int ret, num, i;

    node = fsm->node;
    num = fsm->number;
    state = fsm->cur_st;

    for (i = 0; i < num; i++) {

        if (state == node[i].state) {

            if (RUN_EXIT == state) {
                return DOTA_OK;
            } else if (RUN_OVER == state) {
                return ERR_SHOULD_OVER;
            }

            ret = node[i].state_func(self);

            if (DOTA_OK == ret) {
                state = node[i].sucs_st;
            } else {
                state = node[i].fail_st;
            }

            /* 从头开始找，跳转到下一个阶段 */
            i = -1;
        }
    }

    TRACE_BATTLE(ERROR_FORMAT "unknow fsm state %d.\n", state);
    return DOTA_FAILED;
}
