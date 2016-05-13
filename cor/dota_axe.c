/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!  
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_axe.c
* Describe: HERO AXE
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_enum.h"
#include "dota_equipment.h"
#include "dota_time.h"
#include "dota_log.h"
#include "dota_life.h"
#include "dota_operate.h"
#include "dota_fsm.h"

/* 斧王四个技能的ID */
#define AXE_E 1
#define AXE_R 2
#define AXE_X 3
#define AXE_C 4

/* 斧王淘汰之刃，若血量低于400直接斩杀 */
#define AXE_C_DAMAGE_LOWER_LIMIT 300
#define AXE_C_DAMAGE_UPPER_LIMIT 400

/* 反击螺旋的伤害 */
#define AXE_X_DAMAGE      205
/* 概率是20% */
#define AXE_X_POSSIBILITY 20

#define AXE_R_DAMAGE_PER_SECOND 40

/* 斧王狂战士的怒吼效果，增加自身40点护甲 */
#define AXE_E_INCREASE_ARMOR    40
/* 斧王狂战士的怒吼效果，目标不能移动，不能施法，不能使用物品 */
#define AXE_E_TARGET_EFFECT \
    (MOVE_MASK | EQUIP_MASK | MAGIC_ATTACK_MASK)

/* 斧王的大名，蒙哥可汗，响当当啊 */
#define AXE_NAME "Mogul Kahn"
#define AXE_LEVEL 16

/* 斧王初始三围 */
#define AXE_ORG_AGILITY  55
#define AXE_ORG_STRENGTH 64
#define AXE_ORG_MENTAL   44

#define AXE_ORG_HEALTH   1366
#define AXE_ORG_MANA     572
#define AXE_ORG_ARMOR    6
#define AXE_ORG_ATTACK   92

/* 斧王的攻击间隔为1.2秒，瞎写的，我也不知道多少 */
#define AXE_ATTACK_INTERVAL 12


/* 不能使用物品，不能施法 */
int axe_e_target_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    self->state &= (~AXE_E_TARGET_EFFECT);
    self->target = owner;
    return DOTA_SUCCESS;
}

/* 清除负面效果，即不能使用物品，不能施法 */
int axe_e_clean_target_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    self->state |= AXE_E_TARGET_EFFECT;
    self->target = NULL;
    return DOTA_SUCCESS;
}

/* 增加40点护甲，持续3秒 */
int axe_e_self_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    self->cur_hmaa.armor += AXE_E_INCREASE_ARMOR;
    return DOTA_SUCCESS;
}


/* 清除40点护甲 */
int axe_e_clean_self_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 减掉40点护甲 */
    if (self->cur_hmaa.armor < AXE_E_INCREASE_ARMOR) {
        self->cur_hmaa.armor = 0;
        return DOTA_SUCCESS;
    }
    self->cur_hmaa.armor -= AXE_E_INCREASE_ARMOR;
    return DOTA_SUCCESS;
}

/***********************************************************************************************************************
* Function name: axe_e
* Skill name: 狂战士的怒吼
* Describe: 斧王将周围敌人的恨意集中到自己身上，让他们不计任何代价都要攻击他。
* Mana Cost: 110点
* Cold Down: 100秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int axe_e(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    LIFE_S **enemy = NULL;
    LIFE_S **all_enemy = NULL;
    unsigned int dis = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 给自身加40点护甲的BUFF */
    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->owner = self;
    buff_node->buff_name = self->skills[AXE_E].attr.name;
    buff_node->duration = self->skills[AXE_E].attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = axe_e_self_buff;
    buff_node->clean_buff = axe_e_clean_self_buff;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_ONETIME;

    /* 效果不叠加，需要清理原来的buff，假如有的话 */
    (void)clean_buff_byname(self, buff_node->buff_name);
    add_buff_node(buff_node, &(self->buff_list));

    dis = self->skills[AXE_E].attr.dis;
    all_enemy = find_enemy_area(self, dis);
    if (!all_enemy)
        return DOTA_SUCCESS;

    enemy = all_enemy;
    while (*enemy) {
        /* 给敌人加buff，不能移动、施法以及使用物品 */
        buff_node = create_buff_node();
        DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

        buff_node->owner = self;
        buff_node->buff_name = self->skills[AXE_E].attr.name;
        buff_node->duration = self->skills[AXE_E].attr.duration;
        buff_node->start = get_current_time();
        buff_node->last_exec = 0;
        buff_node->do_buff = axe_e_target_buff;
        buff_node->clean_buff = axe_e_clean_target_buff;
        buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

        /* 效果不叠加，需要清理原来的buff，假如有的话 */
        (void)clean_buff_byname(*enemy, buff_node->buff_name);
        add_buff_node(buff_node, &((*enemy)->buff_list));
        enemy++;
    }
    DOTA_FREE(all_enemy);
    return DOTA_SUCCESS;
}


/* 受到30点/秒的伤害，持续16秒 */
int axe_r_buff(LIFE_S *self, LIFE_S *owner)
{
    unsigned short damage = 0;
    unsigned short spell  = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    DOTA_RETURN_IF_NULL(owner, ERR_NULL_POINTER);

    spell = self->ms.spell_rst;
    damage = HERO_MAGIC_DAMAGE(AXE_R_DAMAGE_PER_SECOND, spell);
    TRACE_BATTLE("%s's buff AXE_R make %u damage to %s.\n",
                 owner->name, damage, self->name);

    if (self->cur_hmaa.health <= damage) {
        self->life_state = LIFE_ZOMBIE;
        self->murderer = owner;
        queue_in(owner->kill_queue, self);
        return DOTA_SUCCESS;
    }
    self->cur_hmaa.health -= damage;
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: axe_r
* Skill name: 战斗饥渴
* Describe: 受到30点/秒的伤害，持续16秒。
* Mana Cost: 105点
* Cold Down: 19秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int axe_r(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = self->skills[AXE_R].attr.name;
    buff_node->duration = self->skills[AXE_R].attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = buff_node->start;
    buff_node->do_buff = axe_r_buff;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_DEFAULT;

    /* 效果不叠加，必须清理原来的战斗饥渴buff，假如有的话 */
    (void)clean_buff_byname(target, buff_node->buff_name);
    add_buff_node(buff_node, &(target->buff_list));
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: axe_x
* Skill name: 反击螺旋
* Describe: 在受到攻击时，斧王有20%的几率抓住敌人，粗暴地甩开，对附近300范围内的
*           敌方单位造成205点伤害，伤害无视魔法免疫。
* Mana Cost: 0点
* Cold Down: 0.3秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int axe_x(LIFE_S *self)
{
    int rand_num = 0;
    unsigned short damage = 0;
    unsigned short armor = 0;
    LIFE_S **enemy = NULL;
    LIFE_S **all_enemy = NULL;
    unsigned int dis = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    rand_num = (rand() % 100) + 1;
    if (rand_num > AXE_X_POSSIBILITY)
        return ERR_TRIGGER_FAILED;

    dis = self->skills[AXE_X].attr.dis;
    all_enemy = find_enemy_area(self, dis);
    if (!all_enemy)
        return DOTA_SUCCESS;

    enemy = all_enemy;
    while (*enemy) {
        armor = (*enemy)->cur_hmaa.armor;
        damage = HERO_PHYSICAL_DAMAGE(AXE_X_DAMAGE, armor);
        TRACE_BATTLE("%s use AXE_X make %u damage to %s.\n",
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
    DOTA_FREE(all_enemy);
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name: Axe_C
* Skill name: 淘汰之刃
* Describe: 清除斧王视线范围内的弱者，伤害并不高，但能瞬间斩杀生命值不足的敌人。
*           瞬间斩杀判断无视魔法免疫。造成300点伤害，瞬间斩杀生命值少于400的敌人。
* Mana Cost: 180点
* Cold Down: 55秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int axe_c(LIFE_S *self)
{
    unsigned short damage = 0;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    /* 血量低于400的话，瞬间斩杀，不管是不是魔法免疫 */
    if (target->cur_hmaa.health <= AXE_C_DAMAGE_UPPER_LIMIT) {
        TRACE_BATTLE("%s use AXE_C make %u damage to %s.\n",
                     self->name,
                     AXE_C_DAMAGE_UPPER_LIMIT,
                     target->name);
        target->life_state = LIFE_ZOMBIE;
        target->murderer = self;
        queue_in(self->kill_queue, target);
        return SUC_IGNORE_CD;
    }

    /* 
     * 若对方血量很多不能斩杀，并且对方处于魔法免疫的状态，
     * 则不会造成伤害，但技能也会CD，相当于已经放出技能
     */
    if (IS_MAIGC_IMMUNITY(target)) {
        TRACE_BATTLE("%s use AXE_C make %u damage to %s.\n",
                     self->name, 0, target->name);
        return DOTA_SUCCESS;
    }

    /* 血量高于400，且不处于魔免状态时，会造成300的伤害 */
    damage = HERO_PHYSICAL_DAMAGE(AXE_C_DAMAGE_LOWER_LIMIT,
                                  target->cur_hmaa.armor);
    TRACE_BATTLE("%s use AXE_C make %u damage to %s.\n",
                 self->name, damage, target->name);
    target->cur_hmaa.health -= damage;
    return DOTA_SUCCESS;
}


/* 这个函数会按照一定的策略释放物品和技能 */
int axe_policy(LIFE_S *self)
{
    int ret = DOTA_INVALID;
    int tmp = DOTA_INVALID;
    OPERATE_S *self_ops;
    LIFE_S *target;
    BUFF_NODE_S *node;

    self_ops = self->ops;

    target = self->target;
    /* 目标的血量是否低于400，若低于则释放淘汰之刃，瞬间斩杀 */
    if (target->cur_hmaa.health <= AXE_C_DAMAGE_UPPER_LIMIT) {
        ret = self_ops->skill(self, AXE_C);
        DOTA_RETURN_IF_SUCCESS(ret);
    }

    /* 
     * 是否处于物理攻击下，若是则会触发反击螺旋，攻击斧王的人越多，
     * 触发的概率越大，physical attack的buff是不会主动清除的，这里
     * 根据该buff的数量得知某一时刻攻击斧王的人数。
     */
    while (NULL != (node = find_buff_byname(self, NAME_PHY_ATK))) {
        (void)clean_buff(self, node);
        tmp = self_ops->skill(self, AXE_X);
        if (DOTA_SUCCESS == tmp)
            ret = DOTA_SUCCESS;
    }
    DOTA_RETURN_IF_SUCCESS(ret);

    /* 
     * 当目标没有羊刀效果才释放邪恶蛋蛋，不然放了白放，这才是聪明的
     * AI该干的事
     */
    if ((NULL == find_buff_byname(target, NAME_GSV))
        && (NULL == find_buff_byname(target, "AXE_E"))) {
        ret = self_ops->equip(self, EQUIPMENT_1);
        DOTA_RETURN_IF_SUCCESS(ret);
    }

    /* 释放羊刀策略，没有被吼住且没有邪恶蛋蛋效果时，释放羊刀 */
    if ((NULL == find_buff_byname(target, "AXE_E"))
        && (NULL == find_buff_byname(target, NAME_XMS))) {
        ret = self_ops->equip(self, EQUIPMENT_0);
        DOTA_RETURN_IF_SUCCESS(ret);
    }

    /* 
     * 目标没有被羊住，且没有被释放邪恶蛋蛋时，释放狂战士怒吼
     * 收益最大
     */
    if ((NULL == find_buff_byname(target, NAME_GSV))
        && (NULL == find_buff_byname(target, NAME_XMS))) {
        ret = self_ops->skill(self, AXE_E);
        DOTA_RETURN_IF_SUCCESS(ret);
    }

    /* 这个战斗饥渴的BUFF还是挺管用的，能烧掉不少血 */
    ret = self_ops->skill(self, AXE_R);
    DOTA_RETURN_IF_SUCCESS(ret);

    /* 物理攻击 */
    ret = self_ops->attack(self);
    DOTA_RETURN_IF_SUCCESS(ret);

    return ret;
}


int axe_init(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 初始化链表头 */
    INIT_LIST_HEAD(&(self->buff_list));
    INIT_LIST_HEAD(&(self->life_list));

    self->kill_queue = queue_init(TMP_SIZE);
    if (!self->kill_queue)
        return ERR_MALLOC_FAILED;

    /* 把道具真正的装备上 */
    init_box(self);

    self->life_state = LIFE_RUNNING;
    return DOTA_OK;
}


FSM_NODE_S axe_fsm_nodes[] = {
    {RUN_START, default_run_start,
     RUN_EXEC_BUFF, RUN_OVER},

    {RUN_EXEC_BUFF, default_run_buff,
     RUN_FIND_TARGET, RUN_OVER},

    {RUN_FIND_TARGET, default_find_target,
     RUN_EXEC_POLICY, RUN_OVER},

    {RUN_EXEC_POLICY, axe_policy,
     RUN_EXIT, RUN_EXIT},

    {RUN_EXIT, NULL,
     RUN_EXIT, RUN_EXIT},

    {RUN_OVER, NULL,
     RUN_OVER, RUN_OVER},
};


FSM_S axe_fsm = {
    RUN_START,
    LIFE_PHASE,
    axe_fsm_nodes,
};


int axe_run(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    return fsm_run(&axe_fsm, self);
}



SKILL_S axe_skill[] = {
    {{NAME_PHY_ATK, SKILL_NORMAL_IMI, 0, 0,
     AXE_ATTACK_INTERVAL, 2, 128, NULL}, 0},

    {{"AXE_E", SKILL_NORMAL_IMI, 0, 110, 100, 32, 275, axe_e}, 0},
    {{"AXE_R", SKILL_NORMAL_IMI, 0, 75, 190, 160, 900, axe_r}, 0},
    {{"AXE_X", SKILL_PASSIVE_IMI, 0, 0, 3, 0, 275, axe_x}, 0},
    {{"AXE_C", SKILL_NORMAL_IMI, 0, 200, 550, 0, 150, axe_c}, 0},
};

/* 斧王装备了羊刀和小毛施的邪恶蛋蛋 */
EQUIP_LIFE_S axe_eqp[EQUIP_MAX] = {
    {&g_shop[EQP_GUINSOOS_SCYTHE_OF_VYSE], 0},
    {&g_shop[EQP_XIAOMAOSHI_EGGS_OF_EVIL], 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
    {NULL, 0},
};

OPERATE_S axe_op = {
    default_death,
    default_rebirth,
    default_kill,
    default_attack,
    default_skill,
    default_equip,
    default_execbuff,
    default_clean,

    axe_init,
    axe_run,
};

/*
 * miss designated initializer so much :(
 * 斧王的生命体，初始化很麻烦。
 */
LIFE_S axe_life = {
    AXE_NAME,
    AXE_LEVEL,
    HERO_STATE,
    HERO_STATE,
    SRG_HERO,

    0,
    0,
    HERO_ORGINAL_MONEY,
    {28, 28},
    NULL,

    {NULL, NULL},
    {NULL, NULL},

    {0, HERO_SPELL_RESISTANCE},
    {AXE_ORG_HEALTH, AXE_ORG_MANA, AXE_ORG_ARMOR, AXE_ORG_ATTACK},
    {AXE_ORG_HEALTH, AXE_ORG_MANA, AXE_ORG_ARMOR, AXE_ORG_ATTACK},
    {AXE_ORG_AGILITY, AXE_ORG_STRENGTH, AXE_ORG_MENTAL},
    {AXE_ORG_AGILITY, AXE_ORG_STRENGTH, AXE_ORG_MENTAL},

    /* 技能集 */
    axe_skill,

    /* 装备集 */
    axe_eqp,

    /* 操作集 */
    &axe_op,

    NULL,
    NULL,
    NULL,
    LIFE_BIRTH,
};

/* 注册函数 */
int axe_register()
{
    return register_life(&axe_life);
}
