/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_equipment.c
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/

#include "dota_equipment.h"
#include "dota_operate.h"
#include "dota_data.h"
#include "dota_log.h"

#if 1

/* 羊刀的效果函数 */
static int gsv_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 设置羊刀影响的bit位 */
    SET_GSV_EFFECT(self);
    return DOTA_OK;
}

/* 清除羊刀的效果函数 */
static int clean_gsv_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 清理羊刀影响的bit位 */
    CLEAN_GSV_EFFECT(self);
    return DOTA_OK;
}


/* 风杖的效果函数 */
static int eul_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    SET_EUL_EFFECT(self);
    TRACE_BATTLE("%s is under EUL effect.\n", self->name);
    return DOTA_OK;
}

/* 风杖对敌方单位造成的伤害 */
#define EUL_DAMAGE 50

/* 清除风杖的效果函数 */
static int clean_eul_buff(LIFE_S *self, LIFE_S *owner)
{
    unsigned short damage = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    DOTA_RETURN_IF_NULL(owner, ERR_NULL_POINTER);

    CLEAN_EUL_EFFECT(self);
    TRACE_BATTLE("%s is clean EUL effect.\n", self->name);

    /* 若不是对自己释放，而是对敌人释放，则落地后会有50点伤害 */
    if (self == owner)
        return DOTA_OK;

    damage = HERO_MAGIC_DAMAGE(EUL_DAMAGE, self->ms.spell_rst);
    TRACE_BATTLE("%s's EUL make %hu damage to %s.\n",
                 owner->name, damage, self->name);

    if (self->cur_hmaa.health <= damage) {
        self->life_state = LIFE_ZOMBIE;
        self->murderer = owner;
        queue_in(owner->kill_queue, self);
        return DOTA_OK;
    }
    self->cur_hmaa.health -= damage;
    return DOTA_OK;
}

/* 蛋疼效果的函数 */
static int xms_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    /* 砸蛋，攻击自己 */
    self->target = self;
    self->ops->attack(self);
    SET_XMS_EFFECT(self);
    return DOTA_OK;
}

/* 清除蛋疼效果的函数 */
static int clean_xms_buff(LIFE_S *self, LIFE_S *owner)
{
    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    self->target = NULL;
    CLEAN_XMS_EFFECT(self);
    return DOTA_OK;
}
#endif

/***********************************************************************************************************************
* Function name:  EQP_GuinsoosScytheofVyse
* Equipment name: 羊刀
* Describe: 激活后（消耗100魔法，冷却时间35秒）将目标变成一只绵羊，期间只能保
*           留部分被动技能效果且不能攻击，只有缓慢的移动速度，持续3.5秒
* Mana Cost: 100点
* Cold Down: 35秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int eqp_guinsoos_sv(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    EQUIPMENT_S *eqp = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    eqp = &(g_shop[EQP_GUINSOOS_SCYTHE_OF_VYSE]);

    TRACE_BATTLE("%s use %s to %s.\n",
                 self->name,
                 eqp->skl_attr.name,
                 target->name);

    /* 新建一个buff节点 */
    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = eqp->skl_attr.name;
    buff_node->duration = eqp->skl_attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = gsv_buff;
    buff_node->clean_buff = clean_gsv_buff;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

    /* 效果不用叠加，清理原来的gsv buff，假如有的话 */
    (void)clean_buff_byname(target, buff_node->buff_name);
    add_buff_node(buff_node, &(target->buff_list));
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name:  EQP_EulScepterofDivinity
* Equipment name: 风杖
* Describe: 使对方悬空3秒，可以对自己使用，施法距离700，冷却时间30秒。被龙卷风吹起的敌方单位落地时将受到50点伤害
　　        龙卷风可对处于魔免状态下的自身施放
* Mana Cost: 75点
* Cold Down: 30秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int eqp_eul_sd(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    EQUIPMENT_S *eqp = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    eqp = &(g_shop[EQP_EUL_SCEPTER_OF_DIVINITY]);

    /* 若不是对自己释放，需要判断目标是否魔法免疫 */
    if (self != target) {
        if (GET_CAMP(self) == GET_CAMP(target)) {
            TRACE_BATTLE("EUL only work to enemy or self.\n");
            return ERR_WRONG_OBJ;
        }

        if (IS_MAIGC_IMMUNITY(target)) {
            TRACE_BATTLE("use EUL failed, target magic immunity.\n");
            return ERR_MAGIC_IMMUNITY;
        }
    }

    TRACE_BATTLE("%s use %s to %s.\n",
                 self->name,
                 eqp->skl_attr.name,
                 target->name);

    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = eqp->skl_attr.name;
    buff_node->duration = eqp->skl_attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = eul_buff;
    buff_node->clean_buff = clean_eul_buff;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

    /* 添加到target的buff链表中 */
    add_buff_node(buff_node, &(target->buff_list));
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name:  eqp_icefrog
* Equipment name: 冰蛙的物品
* Describe: 
*
* Mana Cost:
* Cold Down:
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int eqp_icefrog(LIFE_S *self)
{
    return DOTA_SUCCESS;
}


/***********************************************************************************************************************
* Function name:  EQP_Xiaomaoshi
* Equipment name: 小毛施的邪恶蛋蛋
* Describe: 激活后，会让目标蛋疼，持续3秒。蛋疼期间，目标会物理攻击自己即蛋疼砸蛋。
* Mana Cost: 50点
* Cold Down: 40秒
* Author: xiaomaoshi
* Date: 2015/05/20
* History:
***********************************************************************************************************************/
int eqp_xiaomaoshi_ee(LIFE_S *self)
{
    BUFF_NODE_S *buff_node = NULL;
    EQUIPMENT_S *eqp = NULL;
    LIFE_S *target = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    target = self->target;
    DOTA_RETURN_IF_NULL(target, ERR_NULL_POINTER);

    eqp = &(g_shop[EQP_XIAOMAOSHI_EGGS_OF_EVIL]);

    TRACE_BATTLE("%s use %s to %s.\n",
                 self->name,
                 eqp->skl_attr.name,
                 target->name);

    buff_node = create_buff_node();
    DOTA_RETURN_IF_NULL(buff_node, ERR_MALLOC_FAILED);

    buff_node->buff_name = eqp->skl_attr.name;
    buff_node->duration = eqp->skl_attr.duration;
    buff_node->start = get_current_time();
    buff_node->last_exec = 0;
    buff_node->do_buff = xms_buff;
    buff_node->clean_buff = clean_xms_buff;
    buff_node->owner = self;
    buff_node->exec_interval = HERO_BUFF_INTERVAL_MIN;

    /* 清理掉原来的小毛施邪恶蛋蛋效果 */
    (void)clean_buff_byname(target, buff_node->buff_name);
    add_buff_node(buff_node, &(target->buff_list));
    return DOTA_SUCCESS;
}


/* 商店 */
EQUIPMENT_S g_shop[] = {
    {{10, 10, 35}, {NAME_GSV, SKILL_NORMAL,
     0, 100, 350, 35, 800, eqp_guinsoos_sv}},

    {{0,  0,  10}, {NAME_EUL, SKILL_EUL_LIKE,
     0, 75, 300, 25, 700, eqp_eul_sd}},

    {{1,  1,  1 }, {NAME_XMS, SKILL_NORMAL,
     0, 50,  400, 30, 750, eqp_xiaomaoshi_ee}},
};


/*
 * 英雄扔掉一件装备，box_id为哪一个格子
 */
int takeoff_equip(LIFE_S *self, int box_id)
{
    EQUIPMENT_S *eqp = NULL;
    HMAA_S tmp = {0, 0, 0, 0};
    unsigned long long time = 0;
    unsigned long long used_time = 0;
    unsigned short cd = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    if (box_id >= EQUIP_MAX)
        return ERR_UNKNOW_EQP;

    if (!IS_HERO(self))
        return ERR_WRONG_OBJ;

    eqp = self->equips[box_id].eqp;
    if (!eqp)
        return DOTA_OK;

    /*
     * 处于冷却状态下的装备是不能扔掉的
     */
    time = get_current_time();
    used_time = self->equips[box_id].used_time;
    cd = eqp->skl_attr.cold_down;
    if (used_time && (used_time + cd < time))
        return ERR_COLD_DOWN;

    ASM_TO_HMAA(&(eqp->bsc_attr), &tmp);

    /*
     * 测试发现，在扔掉装备扣除血量时，是根据百分比来扣除的。比如
     * 羊刀给某英雄增加了100点血量，假设英雄装备羊刀后满血的血量
     * 为500点，那么在某一时刻，假设英雄的血量为300点，此时若扔掉
     * 羊刀这件物品，英雄的血量不是300 - 100 = 200，而是300*(4/5)
     * 等于240点，HMAA的其他成员也应该遵守此规矩。
     */
    MINUS_HAMM_PER(&(self->cur_hmaa), &(self->org_hmaa), &tmp);
    MINUS_HAMM(&self->org_hmaa, &tmp);

    MINUS_ASM(&(self->org_asm), &(eqp->bsc_attr));
    MINUS_ASM(&(self->cur_asm), &(eqp->bsc_attr));

    /* 扔掉 */
    self->equips[box_id].eqp = NULL;
    self->equips[box_id].used_time = 0;
    return DOTA_OK;
}


int takeon_equip(LIFE_S *self, int box_id, EQUIPMENT_S *eqp)
{
    HMAA_S tmp = {0, 0, 0, 0};
    int ret = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    if (!eqp || box_id >= EQUIP_MAX)
        return ERR_UNKNOW_EQP;

    if (DOTA_OK != (ret = takeoff_equip(self, box_id)))
        return ret;

    ASM_TO_HMAA(&(eqp->bsc_attr), &tmp);

    /*
     * 在装备一件物品的时候，同样也会遵循一个百分比的法则。继续拿
     * 上面扔掉物品时的例子做解释，若此时英雄的血量为240点，装备
     * 上羊刀后的血量不是240 + 100 = 340点，而是240/(4/5)=300点。
     * 这个百分比(4/5)是羊刀增加的血量(100)与增加后满血状态下血量
     * (500)的比值相对于100%的补码。所以需要先调用ADD_HAMM来得到满
     * 血的血量，与扔掉装备时相反。
     */
    ADD_HAMM(&(self->org_hmaa), &tmp);
    ADD_HAMM_PER(&(self->cur_hmaa), &(self->org_hmaa), &tmp);

    ADD_ASM(&(self->org_asm), &(eqp->bsc_attr));
    ADD_ASM(&(self->cur_asm), &(eqp->bsc_attr));

    /* 
     * 初始化的时候其实没必要做这一步。这里这么做的目的
     * 是保证其他时候调用这个函数都是OK的。
     */
    self->equips[box_id].eqp = eqp;
    self->equips[box_id].used_time = 0;
    return DOTA_OK;
}


int init_box(LIFE_S *self)
{
    int id = 0;
    EQUIPMENT_S * eqp = NULL;
    EQUIP_LIFE_S *equips = NULL;

    DOTA_RETURN_IF((!self), ERR_NULL_POINTER);

    for (equips = self->equips; !equips; equips++, id++) {
        eqp = equips->eqp;
        (void)takeon_equip(self, id, eqp);
    }
    return DOTA_OK;
}
