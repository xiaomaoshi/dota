/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_Es.c
* Describe: HERO ES
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_enum.h"
#include "dota_equipment.h"
#include "dota_operate.h"
#include "dota_time.h"
#include "dota_log.h"
#include "dota_life.h"
#include "dota_fsm.h"

/* 老牛四个技能的ID */
#define ES_F 1
#define ES_E 2
#define ES_A 3
#define ES_C 4

/* 撼地神牛，好名字啊 */
#define ES_NAME "Earth Shaker"
#define ES_LEVEL 16

#define ES_C_DAMAGE         270
#define ES_C_JUMP_DAMAGE    70

#define ES_F_DAMAGE         275

/* 老牛的强化图腾增加的攻击力倍数 */
#define ES_E_TIMES          4

#define ES_A_DAMAGE         125
/* 余震眩晕1.5秒 */
#define ES_A_GIDDINESS_TIME 15

/* 老牛初始三围 */
#define ES_ORG_AGILITY  35
#define ES_ORG_STRENGTH 67
#define ES_ORG_MENTAL   45

#define ES_ORG_HEALTH   1423
#define ES_ORG_MANA     585
#define ES_ORG_ARMOR    6
#define ES_ORG_ATTACK   101

/* 老牛的攻击间隔，这里是1.4秒 */
#define ES_ATTACK_INTERVAL 14

static int es_a(LIFE_S *self);

#if 1
/* 撼地神牛的眩晕buff效果 */
int es_giddiness_effect(LIFE_S *self, LIFE_S *target)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    SET_GIDDINESS_EFFECT(self);
    return DOTA_SUCCESS;
}

/* 清除撼地神牛眩晕buff效果 */
int es_clean_giddiness(LIFE_S *self, LIFE_S *target)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    CLEAN_GIDDINESS_EFFECT(self);
    return DOTA_SUCCESS;
}

/* 撼地神牛强化图腾效果，增加400%的攻击力 */
int es_e_effect(LIFE_S *self, LIFE_S *target)
{
    unsigned short attack = 0;
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 增加400%的攻击力 */
    attack = ES_E_TIMES * self->org_hmaa.attack;
    self->cur_hmaa.attack += attack;
    return DOTA_SUCCESS;
}

/* 清除撼地神牛强化图腾效果 */
int es_e_clean(LIFE_S *self, LIFE_S *target)
{
    unsigned short attack = 0;
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 计算清除BUFF需要减掉的攻击 */
    attack = ES_E_TIMES * self->org_hmaa.attack;
    if (self->cur_hmaa.attack <= attack) {
         self->cur_hmaa.attack = 0;
         return DOTA_SUCCESS;
    }
    self->cur_hmaa.attack -= attack;
    return DOTA_SUCCESS;
}
#endif

/***********************************************************************************************************************
* Function name: es_f
* Skill name: 沟壑
* Describe: 撼地神牛用图腾撕裂地表，形成巨大的沟壑，8秒内不能通行，
*           造成275点伤害，眩晕1.75秒。沟壑造成的眩晕跟余震的眩晕
*           不叠加。
* Mana Cost: 175点
* Cold Down: 10秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int es_f(LIFE_S *self)
{
    BUFF_NODE_S   *buff_node = NULL;
    unsigned short damage    = 0;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    damage = HERO_MAGIC_DAMAGE(ES_F_DAMAGE, target->ms.spell_rst);
    TRACE_BATTLE("%s use ES_F make %hu damage to %s.\n",
                 self->name, damage, target->name);

    if (target->cur_hmaa.health <= damage) {
        target->life_state = LIFE_ZOMBIE;
        target->murderer = self;
        queue_in(self->kill_queue, target);
        return DOTA_SUCCESS;
    }

    target->cur_hmaa.health -= damage;

    /* 创建新的buff节点，这里是增加眩晕效果 */
    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = self->skills[ES_F].attr.name;
    buff_node->duration = self->skills[ES_F].attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = es_giddiness_effect;
    buff_node->clean_buff = es_clean_giddiness;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

    /* 效果不叠加，清理掉目标身上同样的buff，假如有的话 */
    (void)clean_buff_byname(target, buff_node->buff_name);
    add_buff_node(buff_node, &(target->buff_list));

    /* 撼地神牛的特色，只要释放技能，则余震技能(A)都会自动释放 */
    (void)es_a(self);
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: es_e
* Skill name: 强化图腾
* Describe: 在撼地神牛背上的图腾中注入力量，使撼地神牛的下一次攻击能造成
*           额外的伤害。效果持续14秒或维持一次攻击。
* Mana Cost: 50点
* Cold Down: 5秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int es_e(LIFE_S *self)
{
    BUFF_NODE_S   *buff_node = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 给自己增加攻击的buff */
    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = self->skills[ES_E].attr.name;
    buff_node->duration = self->skills[ES_E].attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = es_e_effect;
    buff_node->clean_buff = es_e_clean;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_ONETIME;

    /* 效果不叠加，清理掉原来的强化图腾buff，假如有的话 */
    (void)clean_buff_byname(self, buff_node->buff_name);
    add_buff_node(buff_node, &(self->buff_list));

    /* 眩晕敌人的buff */
    (void)es_a(self);
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: es_a
* Skill name: 余震
* Describe: 每当撼地神牛施放技能时，都会使周围300范围内的地面震动
*           造成125点额外伤害，眩晕1.5秒。
* Mana Cost: 0点
* Cold Down: 0秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int es_a(LIFE_S *self)
{
    BUFF_NODE_S   *buff_node = NULL;
    unsigned short damage   = 0;
    unsigned short spell = 0;
    LIFE_S **enemy = NULL;
    LIFE_S **all_enemy = NULL;
    unsigned int dis = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    dis = self->skills[ES_A].attr.dis;
    all_enemy = find_enemy_area(self, dis);
    if (!all_enemy)
        return DOTA_SUCCESS;

    enemy = all_enemy;
    while (*enemy) {

        spell = (*enemy)->ms.spell_rst;
        damage = HERO_MAGIC_DAMAGE(ES_A_DAMAGE, spell);

        TRACE_BATTLE("%s use ES_A make %u damage to %s.\n",
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

        buff_node = create_buff_node();
        DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

        /* 给作用范围内的所有敌人施加眩晕效果 */
        buff_node->buff_name = self->skills[ES_A].attr.name;
        buff_node->duration = ES_A_GIDDINESS_TIME;
        buff_node->start = get_current_time();
        buff_node->last_exec = 0;
        buff_node->do_buff = es_giddiness_effect;
        buff_node->clean_buff = es_clean_giddiness;
        buff_node->owner = self;
        buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

        /* 
         * 假如目标身上已经有眩晕buff，其实并不需要清除，可以考虑
         * 把新产生的眩晕buff跟已有的比较，若到期时间更靠后可以把
         * 原来的清除掉。
         */
        (void)clean_buff_byname(*enemy, buff_node->buff_name);
        add_buff_node(buff_node, &((*enemy)->buff_list));
        enemy++;
    }
   DOTA_FREE(all_enemy);
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: es_c
* Skill name: 回音击
* Describe: 撼地神牛重击地面发出震波，对周围的敌方单位造成340点基础伤害，
*           震波相互弹射会造成更大伤害！伤害无视魔法免疫。
* Mana Cost: 265点
* Cold Down: 110秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int es_c(LIFE_S *self)
{
    unsigned short damage = 0, tmp_dmg;
    unsigned short spell = 0;
    LIFE_S **enemy = NULL;
    LIFE_S **all_enemy = NULL;
    unsigned int dis = 0;
    unsigned int enmey_num = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    dis = self->skills[ES_C].attr.dis;
    all_enemy = find_enemy_area(self, dis);
    if (!all_enemy)
        return DOTA_SUCCESS;

    enemy = all_enemy;
    /* 计算敌方单位个数，越多造成的弹射伤害(JUMP)越大 */
    while (*enemy++)
        enmey_num++;

    enemy = all_enemy;
    /*
     * 不知道大招如何结算，经测试发现，只有一个斧王的情况下，伤害
     * 才145点左右，减去被动技能余震的伤害，大招的伤害才50点左右
     * ，我彻底晕了。基础伤害340，在只有一个单位的情况下其实是把
     * 肯定有的那一次弹射伤害70点加上去了，默认的应该只有270点。
     */
    //tmp_dmg = ES_C_DAMAGE + (enmey_num * ES_C_JUMP_DAMAGE);
    tmp_dmg = 125 + (enmey_num * ES_C_JUMP_DAMAGE);
    while (*enemy) {
        spell = (*enemy)->ms.spell_rst;
        damage = HERO_MAGIC_DAMAGE(tmp_dmg, spell);
        TRACE_BATTLE("%s use ES_C make %hu damage to %s.\n",
                     self->name, damage, (*enemy)->name);

        if ((*enemy)->cur_hmaa.health <= damage) {
            (*enemy)->life_state = LIFE_ZOMBIE;
            (*enemy)->murderer = self;
            queue_in(self->kill_queue, *enemy);
            enemy++;
            continue;
        }
        (*enemy)->cur_hmaa.health -= damage;
        enemy++;
    }
    (void)es_a(self);
    return DOTA_SUCCESS;
}


/* 这个函数会按照一定的策略释放物品和技能 */
int es_policy(LIFE_S *self)
{
    int ret = DOTA_INVALID;
    OPERATE_S *self_ops;
    LIFE_S *target;

    /* 获取操作集 */
    self_ops = self->ops;

    target = self->target;

    /* 牛头单挑时的攻击策略肯定是多用E，一刀流 */
    ret = self_ops->skill(self, ES_E);
    DOTA_RETURN_IF_SUCCESS(ret);

    /* 紧接着使用物理攻击 */
    ret = self_ops->attack(self);
    if (DOTA_SUCCESS == ret) {
        /* 物理攻击OK时，把自身强化图腾的BUFF清掉，如果有的话 */
        (void)clean_buff_byname(self, "ES_E");
        return DOTA_SUCCESS;
    }

    /* 如果E和物理攻击释放不成功的话，释放沟壑F */
    ret = self_ops->skill(self, ES_F);
    DOTA_RETURN_IF_SUCCESS(ret);

    /* 前面技能都释放完了，考虑放大 */
    ret = self_ops->skill(self, ES_C);
    DOTA_RETURN_IF_SUCCESS(ret);

    /* 目标不处于眩晕状态下释放风杖 */
    if ((NULL == find_buff_byname(target, "ES_A"))
        && (NULL == find_buff_byname(target, "ES_F"))) {
        /* 使用风杖 */
        ret = self_ops->equip(self, EQUIPMENT_0);
        DOTA_RETURN_IF_SUCCESS(ret);
    }
    return ret;
}


int es_init(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 初始化链表头 */
    INIT_LIST_HEAD(&(self->buff_list));
    INIT_LIST_HEAD(&(self->life_list));

    self->kill_queue = (QUEUE_S *)malloc(sizeof(QUEUE_S));
    if (!self->kill_queue)
        return ERR_MALLOC_FAILED;

    queue_init(self->kill_queue, TMP_SIZE);

    /* 把道具真正的装备上 */
    init_box(self);

    self->life_state = LIFE_RUNNING;
    return DOTA_OK;
}


FSM_NODE_S es_fsm_nodes[] = {
    {RUN_START, default_run_start,
     RUN_EXEC_BUFF, RUN_OVER},

    {RUN_EXEC_BUFF, default_run_buff,
     RUN_FIND_TARGET, RUN_OVER},

    {RUN_FIND_TARGET, default_find_target,
     RUN_EXEC_POLICY, RUN_OVER},

    {RUN_EXEC_POLICY, es_policy,
     RUN_EXIT, RUN_EXIT},

    {RUN_EXIT, NULL,
     RUN_EXIT, RUN_EXIT},

    {RUN_OVER, NULL,
     RUN_OVER, RUN_OVER},
};

FSM_S es_fsm = {
    RUN_START,
    LIFE_PHASE,
    es_fsm_nodes,
};


int es_run(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    return fsm_run(&es_fsm, self);
}

SKILL_S es_skill[] = {
    {{NAME_PHY_ATK, SKILL_NORMAL_IMI, 0, 0,
     ES_ATTACK_INTERVAL, 2, 128, NULL}, 0},

    {{"ES_F", SKILL_NORMAL, 0, 170, 150, 17, 1400, es_f}, 0},
    {{"ES_E", SKILL_NORMAL, 0, 50, 50, 140, 0, es_e}, 0},
    {{"ES_A", SKILL_PASSIVE, 0, 0, 0, 0, 300, es_a}, 0},
    {{"ES_C", SKILL_NORMAL,  0, 265, 1100, 0, 575, es_c}, 0},
};

EQUIP_LIFE_S es_eqp[EQUIP_MAX] = {
    {&g_shop[EQP_EUL_SCEPTER_OF_DIVINITY], 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
};

OPERATE_S es_op = {
    default_death,
    default_rebirth,
    default_kill,
    default_attack,
    default_skill,
    default_equip,
    default_execbuff,
    default_clean,

    es_init,
    es_run,
};

/* ES的生命体 */
LIFE_S es_life = {
    ES_NAME,
    ES_LEVEL,
    HERO_STATE,
    HERO_STATE,
    SEN_HERO,

    0,
    0,
    HERO_ORGINAL_MONEY,
    {25, 25},
    NULL,

    {NULL, NULL},
    {NULL, NULL},

    {0, HERO_SPELL_RESISTANCE},
    {ES_ORG_HEALTH, ES_ORG_MANA, ES_ORG_ARMOR, ES_ORG_ATTACK},
    {ES_ORG_HEALTH, ES_ORG_MANA, ES_ORG_ARMOR, ES_ORG_ATTACK},
    {ES_ORG_AGILITY, ES_ORG_STRENGTH, ES_ORG_MENTAL},
    {ES_ORG_AGILITY, ES_ORG_STRENGTH, ES_ORG_MENTAL},

    /* 技能集 */
    es_skill,

    /* 装备集 */
    es_eqp,

    /* 操作集 */
    &es_op,

    NULL,
    NULL,
    NULL,
    LIFE_BIRTH,
};


/* 注册函数 */
int es_register()
{
    return register_life(&es_life);
}
