/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_hero.c
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_equipment.h"
#include "dota_operate.h"
#include "dota_time.h"
#include "dota_log.h"

/* 处于被物理攻击的状态，该buff持续200毫秒 */
int physical_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    return DOTA_OK;
}

/* 清理物理攻击buff */
int clean_physical_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    return DOTA_OK;
}


int default_death(LIFE_S *self)
{
    unsigned int money = 0;
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    DOTA_RETURN_IF((LIFE_ZOMBIE != self->life_state), ERR_WRONG_OBJ);
    /* 清理所有的BUFF */
    clean_all_buff(self);

    /* 清0血量、魔法等属性，毕竟单位死亡了 */
    RESET_HAMM(&(self->cur_hmaa));

    /* 相等为自杀 */
    if (self == self->murderer) {
        money = HERO_LOST_MONEY(0);
        TRACE_BATTLE("%s is suicide!\n", self->name);
    } else {
        money = HERO_LOST_MONEY(self->serial_kill);
        TRACE_BATTLE("%s is dead!\n", self->name);
    }

    if (self->money > money)
        self->money -= money;
    else
        self->money = 0;

    self->life_state = LIFE_DEAD;
    return DOTA_SUCCESS;
}


int default_rebirth(LIFE_S *self)
{
    HMAA_S        *pstOrgHmaa = NULL;
    HMAA_S        *pstCurHmaa = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    pstOrgHmaa = &(self->org_hmaa);
    pstCurHmaa = &(self->cur_hmaa);

    /* 若不是自杀，则需要把连续杀人数清零 */
    if (self != self->murderer)
        self->serial_kill = 0;

    /* 置空凶手 */
    self->murderer = NULL;

    /* 重置属性，英雄重生 */
    COPY_HAMM(pstOrgHmaa, pstCurHmaa);
    self->state = self->org_state;

    TRACE_BATTLE("%s is rebirth!\n", self->name);
    self->life_state = LIFE_RUNNING;
    return DOTA_SUCCESS;
}


int default_kill(LIFE_S *self)
{
    LIFE_S *target = NULL;
    QUEUE_S *queue = NULL;
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    queue = self->kill_queue;
    while (!queue_empty(queue)) {
        target = (LIFE_S *)queue_out(queue);

        if (!target || self == target)
            continue;

        TRACE_BATTLE("%s killed %s!\n",
                     self->name,
                     target->name);

        if (!IS_HERO(target)) {
            self->money += target->money;
            continue;
        }

        if (self->serial_kill < 10)
            self->serial_kill += 1;

        self->money += HERO_GET_MONEY(target->serial_kill);
    }
    return DOTA_SUCCESS;
}

/* 默认的物理攻击动作 */
int default_attack(LIFE_S *self)
{
    unsigned short     damage    = 0;
    unsigned short     cold_down  = 0;
    unsigned long long cur_time  = 0;
    unsigned long long used_time = 0;
    BUFF_NODE_S       *buff_node = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    /* 判断自身是否可以进行物理攻击 */
    if (!CAN_PHYSICAL_ATTACK(self)) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot physical attack!\n",
                     self->name);
        return ERR_CANNOT_PHYSICAL_ATTACK;
    }

    /* 判断目标是否可以被攻击 */
    if (!IS_CAN_BE_ATTACKED(target)) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot be attacked!\n",
                     target->name);
        return ERR_CANNOT_BE_ATTACKED;
    }

    cold_down = self->skills[PY_ATTK].attr.cold_down;
    used_time = self->skills[PY_ATTK].used_time;
    cur_time = get_current_time();
    /* 
     * 若不是第一次物理攻击，则判断物理攻击是否冷却，因
     * 为物理攻击也是有间隔的，即攻速
     */
    if (used_time && (used_time + cold_down > cur_time)) {
        TRACE_BATTLE(INFO_FORMAT "%s use %s is cold down"
                     "(LAST USE: %04llu, interval: %hu)!\n",
                     self->name, self->skills[PY_ATTK].attr.name,
                     used_time, cold_down);
        return ERR_COLD_DOWN;
    }

    /* 创建一个BUFF节点 */
    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = self->skills[PY_ATTK].attr.name;
    buff_node->duration = self->skills[PY_ATTK].attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = physical_buff;
    buff_node->clean_buff = clean_physical_buff;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

    /* 
     * physical buff是叠加的，会根据某个单位身上的该种buff的数量
     * 来判断此时有多少敌人正在攻击该单位，该buff只会持续200毫秒。
     */
    add_buff_node(buff_node, &(target->buff_list));

    /* 记录本次物理攻击的时间 */
    self->skills[PY_ATTK].used_time = cur_time;

    /* 计算本次物理攻击造成的伤害 */
    damage = HERO_PHYSICAL_DAMAGE(self->cur_hmaa.attack,
                                  target->cur_hmaa.armor);
    TRACE_BATTLE("%s physical attack %s, make %hu damage.\n",
                 self->name, target->name, damage);

    /* 判断是否会有人头掉落 */
    if (target->cur_hmaa.health <= damage) {
        target->life_state = LIFE_ZOMBIE;
        /* 如果杀人了，则记得填上真凶 */
        target->murderer = self;
        queue_in(self->kill_queue, target);
        return DOTA_SUCCESS;
    }
    target->cur_hmaa.health -= damage;
    return DOTA_SUCCESS;
}


/* 默认的释放技能的操作 */
int default_skill(LIFE_S *self, unsigned int id)
{
    unsigned short     mana  = 0;
    unsigned short     cold_down  = 0;
    unsigned long long cur_time  = 0;
    unsigned long long used_time = 0;
    int                ret        = 0;
    const char        *skl_name   = NULL;
    OPER1_PF           skill     = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    /* 技能ID只能是从0到4，其中0代表物理攻击 */
    if (id > SKILL_MAX) {
        TRACE_BATTLE(ERROR_FORMAT "%s cannot use the skill,"
                     "skill id is %u!\n",
                     self->name, id);
        return ERR_UNKNOW_SKILL;
    }
    
    skl_name = self->skills[id].attr.name;
    /*
     * 判断当前目标能否成为攻击对象，例如被风杖吹起来了，
     * 就不能成为攻击对象。
     */
    if (!IS_CAN_BE_ATTACKED(target)) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot be attacked!\n",
                     target->name);
        return ERR_CANNOT_BE_ATTACKED;
    }

    /*
     * 判断技能是否只能对敌方单位释放
     */
    if (IS_SKILL_ENEMY_ONLY(&(self->skills[id].attr))
        && (GET_CAMP(self) == GET_CAMP(target))) {
        TRACE_BATTLE(INFO_FORMAT "%s just only work to enemy!\n",
                     skl_name);
        return ERR_WRONG_OBJ;
    }
 
    /*
     * 判断自己是否可以释放技能，如果不能施法再判断该技能是否
     * 是被动技能，被动技能只要条件满足即可释放，不受施法标记
     * 位的限制
     */
    if (!CAN_MAGIC_ATTACK(self)
        && !IS_SKILL_PASSIVE(&(self->skills[id].attr))) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot magic attack!\n",
                     self->name);
        return ERR_CANNOT_MAGIC_ATTACK;
    }

    /*
     * 判断目标是否魔法免疫，并判断本技能是否无视魔免，若目标
     * 正处于魔法免疫状态且该技能不能无视魔法免疫，则返回
     */
    if (IS_MAIGC_IMMUNITY(target)
        && !IS_SKILL_IMI(&(self->skills[id].attr))) {
        TRACE_BATTLE(INFO_FORMAT "%s is magic immunity!\n",
                     target->name);
        return ERR_MAGIC_IMMUNITY;
    }

    /* 获取释放该技能需要消耗的魔法值 */
    mana = self->skills[id].attr.mana;
    /* 如果魔法值不够，则记录日志后返回魔法值不够的错误 */
    if (self->cur_hmaa.mana < mana) {
        TRACE_BATTLE(INFO_FORMAT "%s use %s is out of mana"
                     "(MUST: %hu, CUR: %hu)!\n",
                     self->name,
                     skl_name,
                     mana,
                     self->cur_hmaa.mana);
        return ERR_OUT_OF_MANA;
    }

    cold_down = self->skills[id].attr.cold_down;
    used_time = self->skills[id].used_time;
    cur_time = get_current_time();

    /*
     * 判断技能是否处于CD之中，如果使用时间为0说明该技能是第一
     * 次使用。否则，若上一次的使用时间加上该技能的冷却时间已经
     * 小于当前时间，说明技能已冷却OK了
     */
    if (used_time && (used_time + cold_down > cur_time)) {
        TRACE_BATTLE(INFO_FORMAT "%s use %s is cold down"
                     "(LAST USE: %04llu, CD: %hu)!\n",
                     self->name,
                     skl_name,
                     used_time,
                     cold_down);
        return ERR_COLD_DOWN;
    }

    /*
     * 经过上述的一些判断，基本保证了施法的有效性，
     * 这里再判断技能本身是否有效
     */
    skill = self->skills[id].attr.skill;
    if (NULL == skill) {
        TRACE_BATTLE(ERROR_FORMAT "%s use %s is invalid!\n",
                     self->name,
                     skl_name);
        return ERR_INVALID_SKILL;
    }

    /* 通过注册的函数指针，来释放对应的技能 */
    ret = skill(self);

    switch(ret) {
        /*
         * 某些技能释放后在某些条件下不会进入冷却状态，仅扣除魔法值，
         * 不记录施法时间，仍然可用。
         */
        case SUC_IGNORE_CD:
            TRACE_BATTLE("%s use %s OK.\n",
                         self->name,
                         skl_name);
            TRACE_BATTLE("ignore the cd of skill %s.\n", skl_name);
            self->cur_hmaa.mana -= mana;
        break;

        /*
         * 如果释放成功，则需要扣除相应的魔法，并且记录施法时间
         */
        case DOTA_SUCCESS:
            TRACE_BATTLE("%s use %s OK.\n",
                         self->name,
                         skl_name);
            self->cur_hmaa.mana -= mana;
            self->skills[id].used_time = cur_time;
        break;

        /*
         * 技能释放失败，打印失败错误码
         */
        default:
            TRACE_BATTLE("%s use %s faild, ERROR CODE: %d.\n",
                         self->name, skl_name, ret);
        break;
    }
    return ret;
}


/* 默认的使用物品的操作 */
int default_equip(LIFE_S *self, unsigned int id)
{
    unsigned short     mana  = 0;
    unsigned short     cold_down  = 0;
    unsigned long long cur_time  = 0;
    unsigned long long used_time = 0;
    int                ret        = 0;
    const char        *eqp_name   = NULL;
    OPER1_PF           skill     = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    if (id >= EQUIP_MAX) {
        TRACE_BATTLE(ERROR_FORMAT "%s cannot use the equipment,"
                     "index is %u!\n", self->name, id);
        return ERR_UNKNOW_EQP;
    }
    eqp_name = self->equips[id].eqp->skl_attr.name;

    if (!IS_CAN_BE_ATTACKED(target)) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot be attacked!\n",
                     self->name);
        return ERR_CANNOT_BE_ATTACKED;
    }

    if (!CAN_USE_EQUIPMENT(self)) {
        TRACE_BATTLE(INFO_FORMAT "%s cannot use equipment.\n",
                     self->name);
        return ERR_CANNOT_USE_EQUIPMENT;
    }

    /*
     * 判断物品是否只能对敌方单位释放
     */
    if (IS_SKILL_ENEMY_ONLY(&(self->equips[id].eqp->skl_attr))
        && (GET_CAMP(self) == GET_CAMP(target))) {
        TRACE_BATTLE(INFO_FORMAT "%s just only work to enemy!\n",
                     eqp_name);
        return ERR_WRONG_OBJ;
    }

    if (IS_MAIGC_IMMUNITY(target) 
        && !IS_SKILL_IMI(&(self->equips[id].eqp->skl_attr))) {
        TRACE_BATTLE(INFO_FORMAT "%s is magic immunity!\n",
                     target->name);
        return ERR_MAGIC_IMMUNITY;
    }

    mana = self->equips[id].eqp->skl_attr.mana;
    if (self->cur_hmaa.mana < mana) {
        TRACE_BATTLE(INFO_FORMAT "%s use %s is out of mana"
                     "(MUST: %hu, CUR: %hu)!\n",
                     self->name, eqp_name,
                     mana, self->cur_hmaa.mana);
        return ERR_OUT_OF_MANA;
    }

    cold_down = self->equips[id].eqp->skl_attr.cold_down;
    used_time = self->equips[id].used_time;
    cur_time = get_current_time();
    if (used_time && (used_time + cold_down > cur_time)) {
        TRACE_BATTLE(INFO_FORMAT "%s use %s is cold down"
                     "(LAST USE: %04llu, CD: %hu)!\n",
                     self->name,
                     eqp_name,
                     used_time,
                     cold_down);
        return ERR_COLD_DOWN;
    }

    skill = self->equips[id].eqp->skl_attr.skill;
    if (!skill) {
        TRACE_BATTLE(ERROR_FORMAT "%s use %s is invalid!\n",
                     self->name, eqp_name);
        return ERR_INVALID_EQP;
    }

    ret = skill(self);

    switch(ret) {
        /*
         * 如果物品释放成功，则需要扣除相应的魔法，并且记录施法时间
         */
        case DOTA_SUCCESS:
            TRACE_BATTLE("%s use %s OK.\n",
                         self->name,
                         eqp_name);
            self->cur_hmaa.mana -= mana;
            self->equips[id].used_time = cur_time;
        break;

        /*
         * 物品释放失败，打印失败错误码
         */
        default:
            TRACE_BATTLE("%s use %s faild, ERROR: %d.\n",
                         self->name,
                         eqp_name,
                         ret);
        break;
    }
    return ret;
}



/* 默认的清算buff & debuff的操作 */
int default_execbuff(LIFE_S *self)
{
    LIST_HEAD_S       *item       = NULL;
    LIST_HEAD_S       *temp       = NULL;
    LIST_HEAD_S       *head       = NULL;
    BUFF_NODE_S       *buff_node  = NULL;
    SKILL_PF           clean_buff = NULL;
    SKILL_PF           do_buff    = NULL;
    unsigned long long cur_time   = 0;
    unsigned long long start      = 0;
    unsigned long long last       = 0;
    unsigned short     interval   = 0;
    unsigned short     duration   = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    head = &(self->buff_list);
    cur_time = get_current_time();

    /*
     * 遍历链表中所有的BUFF节点
     */
    LIST_FOR_EACH_SAFE(item, temp, head) {

        buff_node = LIST_ENTRY(item, BUFF_NODE_S, buff_list);
        last = buff_node->last_exec;
        interval = buff_node->exec_interval;

        /*
         * 第一次的话会立即执行buff的do函数，否则
         * 是每当间隔时间到了执行do函数
         */
        if (!last || (last + interval <= cur_time)) {
            do_buff = buff_node->do_buff;
            if (do_buff)
                (void)do_buff(self, buff_node->owner);

            TRACE_BATTLE("%s do %s buff.\n",
                         self->name,
                         buff_node->buff_name);
            buff_node->last_exec = cur_time;
        }

        start = buff_node->start;
        duration = buff_node->duration;
        /*
         * 若某个buff超时或者说是过期了，需要清理移除
         */
        if (start + duration <= cur_time) {
            list_del(item);
            clean_buff = buff_node->clean_buff;
            if (clean_buff)
                (void)clean_buff(self, buff_node->owner);

            TRACE_BATTLE("%s clean %s buff.\n",
                         self->name,
                         buff_node->buff_name);
            DOTA_FREE(buff_node);
        }
    }
    return DOTA_SUCCESS;
}


int default_clean(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    TRACE_BATTLE("Clean %s.\n", self->name);

    /* 清理所有的BUFF节点 */
    clean_all_buff(self);
    queue_free(self->kill_queue);
    return DOTA_SUCCESS;
}


/* 野怪的杀人操作 */
int wilders_kill(LIFE_S *self)
{
    LIFE_S *target = NULL;
    QUEUE_S *queue = NULL;
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    queue = self->kill_queue;
    while (!queue_empty(queue)) {
        target = (LIFE_S *)queue_out(queue);

        if (!target || self == target)
            continue;

        TRACE_BATTLE("%s killed %s!\n",
                     self->name,
                     target->name);
    }
    return DOTA_SUCCESS;
}


int wilders_death(LIFE_S *self)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    DOTA_RETURN_IF((LIFE_ZOMBIE != self->life_state), ERR_WRONG_OBJ);

    clean_all_buff(self);

    RESET_HAMM(&(self->cur_hmaa));

    if (self == self->murderer) {
        TRACE_BATTLE("%s is suicide!\n", self->name);
    } else {
        TRACE_BATTLE("%s is dead!\n", self->name);
    }

    self->life_state = LIFE_DEAD;
    return DOTA_SUCCESS;
}
