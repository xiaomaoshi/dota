/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_wilders.c
* Describe: all wilders
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_enum.h"
#include "dota_time.h"
#include "dota_log.h"
#include "dota_life.h"
#include "dota_operate.h"

#include "dota_fsm.h"

extern int es_giddiness_effect(LIFE_S *self, LIFE_S *target);
extern int es_clean_giddiness(LIFE_S *self, LIFE_S *target);

#define HOOF_STOMP             1
#define HOOF_STOMP_NAME        "HOOF STOMP"

#define CENTAUR_NAME           "Centaur"
#define CENTAUR_LEVEL          5
#define CENTAUR_MONEY          75
#define CENTAUR_STOMP_DAMAGE   25

#define CENTAUR_HEALTH         1100
#define CENTAUR_MANA           200
#define CENTAUR_ATTACK         55
#define CENTAUR_ARMOR          4

#define CENTAUR_ATTACK_INTEVAL 15

/***********************************************************************************************************************
* Function name:  centaur_hoofstomp
* Describe: 对敌方地面单位造成25点伤害，眩晕2秒
* Mana Cost: 100点
* Cold Down: 20秒
* Dis:       250码
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
static int centaur_hoofstomp(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    LIFE_S **enemy = NULL;
    LIFE_S **all_enemy = NULL;
    unsigned int dis = 0;
    unsigned short damage = 0;
    unsigned short duration = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    dis = self->skills[HOOF_STOMP].attr.dis;
    all_enemy = find_enemy_area(self, dis);
    if (!all_enemy)
        return DOTA_SUCCESS;

    duration = self->skills[HOOF_STOMP].attr.duration;
    damage = CENTAUR_STOMP_DAMAGE;
    enemy = all_enemy;
    while (*enemy) {
        TRACE_BATTLE("%s use HoofStomp make %u damage to %s.\n",
                     self->name, damage, (*enemy)->name);

        /* 对敌人造成伤害，若该攻击致死，则不会为其增加buff节点 */
        if ((*enemy)->cur_hmaa.health <= damage) {
            (*enemy)->life_state = LIFE_ZOMBIE;
            (*enemy)->murderer = self;
            queue_in(self->kill_queue, *enemy);
            enemy++;
            continue;
        }
        (*enemy)->cur_hmaa.health -= damage;

        /* 给作用范围内的所有敌人施加眩晕效果 */
        buff_node = create_buff_node();
        DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

        buff_node->owner = self;
        buff_node->buff_name = GIDDINESS_NAME;
        buff_node->duration = duration;
        buff_node->start = get_current_time();
        buff_node->last_exec = 0;
        buff_node->do_buff = es_giddiness_effect;
        buff_node->clean_buff = es_clean_giddiness;
        buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

        /* 
         * 假如目标身上已经有眩晕buff，此时并不需要清除，可以考虑
         * 把新产生的眩晕buff跟已有的比较，若到期时间更靠后可以把
         * 原来的清除掉。
         */
        add_buff_node(buff_node, &((*enemy)->buff_list));
        enemy++;
    }
    DOTA_FREE(all_enemy);
    return DOTA_SUCCESS;
}


int centaur_init(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 初始化链表头 */
    INIT_LIST_HEAD(&(self->buff_list));
    INIT_LIST_HEAD(&(self->life_list));

    self->kill_queue = queue_init(TMP_SIZE);
    if (!self->kill_queue)
        return ERR_MALLOC_FAILED;

    self->life_state = LIFE_RUNNING;
    return DOTA_OK;
}


int centaur_policy(LIFE_S *self)
{
    int ret;
    OPERATE_S *self_ops;

    self_ops = self->ops;

    ret = self_ops->skill(self, HOOF_STOMP);
    DOTA_RETURN_IF_SUCCESS(ret);

    ret = self_ops->attack(self);
    return ret;
}


FSM_NODE_S centaur_fsm_nodes[] = {
    {RUN_START, default_run_start,
     RUN_EXEC_BUFF, RUN_OVER},

    {RUN_EXEC_BUFF, default_run_buff,
     RUN_FIND_TARGET, RUN_OVER},

    {RUN_FIND_TARGET, default_find_target,
     RUN_EXEC_POLICY, RUN_OVER},

    {RUN_EXEC_POLICY, centaur_policy,
     RUN_EXIT, RUN_EXIT},

    {RUN_EXIT, NULL,
     RUN_EXIT, RUN_EXIT},

    {RUN_OVER, NULL,
     RUN_OVER, RUN_OVER},
};

FSM_S centaur_fsm = {
    RUN_START,
    LIFE_PHASE,
    centaur_fsm_nodes,
};


int centaur_run(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    return fsm_run(&centaur_fsm, self);
}


SKILL_S centaur_skill[] = {
    {{NAME_PHY_ATK, SKILL_NORMAL_IMI, 0, 0,
     CENTAUR_ATTACK_INTEVAL, 2, 128, NULL}, 0},

    {{HOOF_STOMP_NAME, SKILL_NORMAL_IMI, 0, 100,
     200, 20, 250, centaur_hoofstomp}, 0},
};


OPERATE_S centaur_op = {
    wilders_death,
    default_rebirth,
    wilders_kill,
    default_attack,
    default_skill,
    NULL,
    default_execbuff,
    default_clean,

    centaur_init,
    centaur_run,
};

/* 半人马(野怪)的生命体 */
LIFE_S centaur_life = {
    CENTAUR_NAME,
    CENTAUR_LEVEL,
    WILD_STATE,
    WILD_STATE,
    NEUTRAL_WILD,

    0,
    0,
    CENTAUR_MONEY,
    {24, 24},
    NULL,

    {NULL, NULL},
    {NULL, NULL},

    {0, HERO_SPELL_RESISTANCE},
    {CENTAUR_HEALTH, CENTAUR_MANA, CENTAUR_ARMOR, CENTAUR_ATTACK},
    {CENTAUR_HEALTH, CENTAUR_MANA, CENTAUR_ARMOR, CENTAUR_ATTACK},
    {0, 0, 0},
    {0, 0, 0},

    /* 技能集 */
    centaur_skill,

    /* 野怪毛都不会装备的 */
    NULL,

    /* 操作集 */
    &centaur_op,

    NULL,
    NULL,
    NULL,
    LIFE_BIRTH,
};


/* 注册函数 */
int centaur_register()
{
    return register_life(&centaur_life);
}
