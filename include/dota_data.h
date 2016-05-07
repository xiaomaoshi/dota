/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_data.h
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/
#ifndef __DOTA_DATA_H__
#define __DOTA_DATA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dota_list.h"
#include "dota_queue.h"
#include "dota_base.h"
#include "dota_enum.h"
#include "dota_time.h"

/*
bit 0: 是否魔法免疫，例如BKB
bit 1: 是否物理免疫，例如绿杖
bit 2: 是否可以进行魔法攻击，沉默大招了或者被紫苑了，都是不能施法的
bit 3: 是否可以进行物理攻击，例如被虚灵刀或者骨法C了
bit 4: 是否可以使用物品
bit 5: 是否能成为攻击目标
bit 6: 是否正被物理攻击
bit 7: 是否可以移动
*/

#define MAGIC_IMMUNITY_BIT  0
#define MAGIC_IMMUNITY_MASK 1         //00000001B

#define PHYSICAL_IMMUNITY_BIT  1
#define PHYSICAL_IMMUNITY_MASK 2      //00000010B

#define MAGIC_ATTACK_BIT  2
#define MAGIC_ATTACK_MASK 4           //00000100B

#define PHYSICAL_ATTACK_BIT  3
#define PHYSICAL_ATTACK_MASK 8        //00001000B

#define EQUIP_BTI  4
#define EQUIP_MASK 16                 //00010000B

#define CAN_BE_ATTACKED_BIT  5
#define CAN_BE_ATTACKED_MASK 32       //00100000B

#define UNDER_PHYSICAL_ATTACK_BIT  6
#define UNDER_PHYSICAL_ATTACK_MASK 64 //01000000B

#define MOVE_BIT  7
#define MOVE_MASK 128      //10000000B

#define HERO_STATE           0xBC      //10111100B
#define WILD_STATE           0xAC      //10101100B
//#define NORMAL_STATE         0xAC      //10101100B
#define MAGIC_IMMUNITY_STATE 0xA9      //10101001B

#define GIDDINESS_MASK       0x9C      //10011100B

#define GSV_MASK             0x1C      //00011100B

#define EUL_MASK             0xBC      //10111100B

#define XMS_MASK             0x14      //00010100B

/*
bit 0: 是否是机械单位
bit 1: 是否是建筑单位
bit 2: 是否是远古单位
bit 3: 是否是无敌单位
bit 4: 保留
bit 5 & bit 6:
              00 ROSHAN
              10 近卫单位
              01 天灾单位
              11 中立单位野怪
bit 7: 是否是英雄
*/

#define MACHINE_BIT  0
#define MACHINE_MASK 0x1           //00000001B

#define BUILDDING_BIT  1
#define BUILDDING_MASK 0x2         //00000010B

#define ACIENT_BIT  2
#define ACIENT_MASK 0x4            //00000100B

#define CAMP_BIT  5
#define CAMP_MASK 0x60             //01100000B

#define HERO_BIT  7
#define HERO_MASK 0x80             //10000000B


#define SEN_HERO         0xA0      //10100000B
#define SRG_HERO         0xC0      //11000000B

#define SEN_SOLDIER      0x20      //00100000B
#define SRG_SOLDIER      0x40      //01000000B

#define SEN_CATAPULT     0x21      //00100001B
#define SRG_CATAPULT     0x41      //01000001B

#define NEUTRAL_WILD     0x60      //01100000B
#define ROSHAN_IDENTITY  0x8       //00000100B

typedef struct tagLocation
{
    unsigned int x, y;
}LOCATION_S;

typedef struct tag_ms
{
    unsigned short move_speed;
    unsigned short spell_rst;
}MS_S;

typedef struct tag_asm
{
    unsigned short agility;
    unsigned short strength;
    unsigned short mental;
}ASM_S;

#define MINUS_PER(d, p) \
    (d) = (((d) * (100 - (p))) / 100)

#define ADD_PER(d, p) \
    (d) = (((d) * 100) / (100 - (p)))

#define PER(d, dp) \
    (((dp) * 100) / (d))

#define ADD_ASM(p, pm)                   \
    do {                                 \
        (p)->agility  += (pm)->agility;  \
        (p)->strength += (pm)->strength; \
        (p)->mental   += (pm)->mental;   \
    } while(0)

#define MINUS_ASM(p, pm)                 \
    do {                                 \
        (p)->agility  -= (pm)->agility;  \
        (p)->strength -= (pm)->strength; \
        (p)->mental   -= (pm)->mental;   \
    } while(0)

typedef struct tag_hmaa
{
    unsigned short health;
    unsigned short mana;
    unsigned short armor;
    unsigned short attack;
}HMAA_S;

#define MINUS_HAMM(p, pm)                \
    do {                                 \
        (p)->health   -= (pm)->health;   \
        (p)->mana     -= (pm)->mana;     \
        (p)->armor    -= (pm)->armor;    \
        (p)->attack   -= (pm)->attack;   \
    } while(0)

#define MINUS_HAMM_PER(pc, po, pm)                                 \
    do {                                                           \
        MINUS_PER((pc)->health, PER((po)->health, (pm)->health)); \
        MINUS_PER((pc)->mana, PER((po)->mana, (pm)->mana));       \
        MINUS_PER((pc)->armor, PER((po)->armor, (pm)->armor));    \
        MINUS_PER((pc)->attack, PER((po)->attack, (pm)->attack)); \
    } while(0)

#define ADD_HAMM(p, pm)                  \
    do {                                 \
        (p)->health   += (pm)->health;   \
        (p)->mana     += (pm)->mana;     \
        (p)->armor    += (pm)->armor;    \
        (p)->attack   += (pm)->attack;   \
    } while(0)

#define ADD_HAMM_PER(pc, po, pm)                                   \
    do {                                                           \
        ADD_PER((pc)->health, PER((po)->health, (pm)->health));   \
        ADD_PER((pc)->mana, PER((po)->mana, (pm)->mana));         \
        ADD_PER((pc)->armor, PER((po)->armor, (pm)->armor));      \
        ADD_PER((pc)->attack, PER((po)->attack, (pm)->attack));   \
    } while(0)

#define HEALTH_PER_STRENGTH      19
#define MANA_PER_MENTAL          13
#define AGILITY_PER_ARMOR        7
#define ATTACK_PER_STRENGTH      1

#define SET_HAMM(p, h, m, ar, ak)              \
    do {                                       \
        (p)->health = (h);                     \
        (p)->mana = (m);                       \
        (p)->armor = (ar);                     \
        (p)->attack = (ak);                    \
    } while(0)
#define RESET_HAMM(p)       SET_HAMM(p, 0, 0, 0, 0)

#define COPY_HAMM(pS, pD)              \
    do {                               \
        (pD)->health = (pS)->health;   \
        (pD)->mana = (pS)->mana;       \
        (pD)->armor = (pS)->armor;     \
        (pD)->attack = (pS)->attack;   \
    } while(0)


#define ASM_TO_HMAA(asm, hmaa)                                      \
    do {                                                            \
        (hmaa)->health += (HEALTH_PER_STRENGTH * (asm)->strength);  \
        (hmaa)->mana   += (MANA_PER_MENTAL * (asm)->mental);        \
        (hmaa)->armor  += ((asm)->agility / AGILITY_PER_ARMOR);     \
        (hmaa)->attack += (ATTACK_PER_STRENGTH * (asm)->strength);  \
    } while(0)

#define NAME_LEN  32
#define SKILL_MAX 4
#define EQUIP_MAX 6
typedef struct dota_life * LIFE_PST;
typedef int (* SKILL_PF)(LIFE_PST self, LIFE_PST target);

typedef int (* OPER0_PF)(void);
typedef int (* OPER1_PF)(LIFE_PST self);
typedef int (* OPER2_PF)(LIFE_PST self, unsigned int id);


typedef struct skill_attr
{
    const char *name;
    unsigned char state;
    unsigned short health;
    unsigned short mana;
    unsigned short cold_down;
    unsigned short duration;
    unsigned int dis;
    OPER1_PF skill;
}SKILL_ATTR_S;

typedef struct tag_skill
{
    SKILL_ATTR_S attr;
    unsigned long long used_time;
}SKILL_S;

/*
bit 0: 是否无视魔法免疫
bit 1: 是否是被动技能
bit 2: 是否只能对敌方单位使用
bit 3: 是否只能对英雄释放
bit 4 - 7: reserved.
*/

/* IMI = IGNORE MAGIC IMMUNITY */
#define SKILL_IMI_BIT                      0
#define SKILL_IMI_MASK                     1 //00000001B

#define SKILL_PASSIVE_BIT                  1
#define SKILL_PASSIVE_MASK                 2 //00000010B

#define SKILL_ENEMY_ONLY_BIT               2
#define SKILL_ENEMY_ONLY_MASK              4 //00000100B

#define SKILL_HERO_ONLY_BIT                3
#define SKILL_HERO_ONLY_MASK               8 //00001000B

#define SKILL_EUL_LIKE                   0x1 //00000001B
#define SKILL_NORMAL                     0x4 //00000100B
#define SKILL_NORMAL_IMI                 0x5 //00000101B
#define SKILL_PASSIVE                    0x6 //00000110B
#define SKILL_PASSIVE_IMI                0x7 //00000111B

/* 无视魔法免疫 */
#define IS_SKILL_IMI(attr)        \
    ((attr)->state & SKILL_IMI_MASK)

/* 被动技能 */
#define IS_SKILL_PASSIVE(attr)    \
    ((attr)->state & SKILL_PASSIVE_MASK)

/* 只能对敌人释放 */
#define IS_SKILL_ENEMY_ONLY(attr) \
    ((attr)->state & SKILL_ENEMY_ONLY_MASK)

/* 只能对英雄释放 */
#define IS_SKILL_HERO_ONLY(attr)  \
    ((attr)->state & SKILL_HERO_ONLY_MASK)

typedef struct tag_equipment
{
    ASM_S bsc_attr;
    SKILL_ATTR_S skl_attr;
}EQUIPMENT_S;

typedef struct equip_life
{
    EQUIPMENT_S *eqp;
    unsigned long long used_time;
}EQUIP_LIFE_S;

typedef struct tag_operate
{
    OPER1_PF death;
    OPER1_PF rebirth;
    OPER1_PF kill;
    OPER1_PF attack;
    OPER2_PF skill;
    OPER2_PF equip;
    OPER1_PF buff;
    OPER1_PF clean;

    OPER1_PF init;
    OPER1_PF run;
}OPERATE_S;

typedef struct dota_life
{
    const char *name;
    unsigned char level;
    unsigned char state;
    unsigned char org_state;
    unsigned char identity;

    unsigned char serial_kill;
    unsigned int serial_num;
    unsigned int money;
    LOCATION_S location;
    void *p_data;

    LIST_HEAD_S buff_list;
    LIST_HEAD_S life_list;

    MS_S ms;
    HMAA_S org_hmaa;
    HMAA_S cur_hmaa;
    ASM_S org_asm;
    ASM_S cur_asm;

    SKILL_S *skills;
    EQUIP_LIFE_S *equips;
    OPERATE_S *ops;

    struct dota_life *target;
    struct dota_life *murderer;
    QUEUE_S *kill_queue;
    LIFE_STATE_E life_state;
}LIFE_S;

typedef struct buff_node
{
    const char *buff_name;
    LIST_HEAD_S buff_list;
    unsigned long long start;
    unsigned long long last_exec;
    unsigned short duration;
    unsigned short exec_interval;
    SKILL_PF do_buff;
    SKILL_PF clean_buff;
    LIFE_S *owner;
}BUFF_NODE_S;

#define EQUIPMENT_0      0
#define EQUIPMENT_1      1
#define EQUIPMENT_2      2
#define EQUIPMENT_3      3
#define EQUIPMENT_4      4
#define EQUIPMENT_5      5

#define PY_ATTK          0
#define TMP_SIZE         16

#define IS_MAIGC_IMMUNITY(life)           ((life)->state & MAGIC_IMMUNITY_MASK)
#define IS_PHYSICAL_IMMUNITY(life)        ((life)->state & PHYSICAL_IMMUNITY_MASK)

#define CAN_MAGIC_ATTACK(life)            ((life)->state & MAGIC_ATTACK_MASK)
#define SET_MAGIC_ATTACK_BIT(life)        (((life)->state) |= MAGIC_ATTACK_MASK)
#define CLEAN_MAGIC_ATTACK_BIT(life)      (((life)->state) &= (~MAGIC_ATTACK_MASK))


#define CAN_PHYSICAL_ATTACK(life)         ((life)->state & PHYSICAL_ATTACK_MASK)
#define SET_PHYSICAL_ATTACK_BIT(life)     (((life)->state) |= PHYSICAL_ATTACK_MASK)
#define CLEAN_PHYSICAL_ATTACK_BIT(life)   (((life)->state) &= (~PHYSICAL_ATTACK_MASK))


#define CAN_USE_EQUIPMENT(life)           ((life)->state & EQUIP_MASK)
#define SET_USE_EQUIPMENT_BIT(life)       (((life)->state) |= EQUIP_MASK)
#define CLEAN_USE_EQUIPMENT_BIT(life)     (((life)->state) &= (~EQUIP_MASK))

#define IS_CAN_BE_ATTACKED(life)          (((life)->state) & CAN_BE_ATTACKED_MASK)

#define SET_CAN_BE_ATTACKED_BIT(life)  \
    (((life)->state) |= CAN_BE_ATTACKED_MASK)

#define CLEAN_CAN_BE_ATTACKED(life) \
    (((life)->state) &= (~CAN_BE_ATTACKED_MASK))

#define SET_UNDER_PHYSICAL_ATTACK_BIT(life)  \
    (((life)->state) |= UNDER_PHYSICAL_ATTACK_MASK)

#define CLEAN_UNDER_PHYSICAL_ATTACK_BIT(life) \
    (((life)->state) &= (~UNDER_PHYSICAL_ATTACK_MASK))


#define CAN_MOVE(life)                     (((life)->state) & MOVE_MASK)
#define SE_MOVE_BIT(life)                  (((life)->state) |= MOVE_MASK)
#define CLEAN_MOVE_BIT(life)               (((life)->state) &= (~MOVE_MASK))

#define GET_HERO_BIT(life)                 ((((life)->identity) & HERO_MASK) >> HERO_BIT)
#define IS_HERO(life)                      (((life)->identity) & HERO_MASK)

#define GET_CAMP(life)                     ((((life)->identity) & CAMP_MASK) >> CAMP_BIT)

#define IS_DEAD(life) \
    (LIFE_ZOMBIE == (life)->life_state || LIFE_DEAD == (life)->life_state)

#define SET_EQUIP_BIT(life)                (((life)->state) |= EQUIP_MASK)
#define CLEAN_EQUIP_BIT(life)              (((life)->state) &= (~EQUIP_MASK))


#define SET_GIDDINESS_EFFECT(life)         ((life)->state &= (~GIDDINESS_MASK))
#define CLEAN_GIDDINESS_EFFECT(life)       ((life)->state |= GIDDINESS_MASK)
#define GIDDINESS_NAME "GIDDINESS"

#define SET_GSV_EFFECT(life)               ((life)->state &= (~GSV_MASK))
#define CLEAN_GSV_EFFECT(life)             ((life)->state |= GSV_MASK)

#define SET_EUL_EFFECT(life)               ((life)->state &= (~EUL_MASK))
#define CLEAN_EUL_EFFECT(life)             ((life)->state |= EUL_MASK)

#define SET_XMS_EFFECT(life)               ((life)->state &= (~XMS_MASK))
#define CLEAN_XMS_EFFECT(life)             ((life)->state |= XMS_MASK)

#define SQAURE(X) ((X)*(X))
#define DISTANCE(X1, Y1, X2, Y2) \
    (SQAURE((X1)-(X2)) + SQAURE((Y1)-(Y2)))

#define DOTA_RETURN_IF(p, ret)        \
    do {                              \
        if (p)                        \
            return (ret);             \
    }while(0)

#define DOTA_RETURN_IF_NULL(p, Ret)   \
    do {                              \
        if (NULL == (p))              \
            return (Ret);             \
    }while(0)

#define DOTA_RETURN_IF_SUCCESS(Ret)          \
    do {                                     \
        if (((Ret) >= DOTA_SUCCESS_START) && \
            ((Ret) <= DOTA_SUCCESS_END))     \
            return DOTA_OK;                  \
    }while(0)

#define DOTA_RETURN_IF_DEAD(life)     \
    do {                              \
        if (IS_DEAD(life))            \
            return DOTA_OK;           \
    }while(0)

#define NAME_PHY_ATK "physical attack"

BUFF_NODE_S *create_buff_node(void);
int add_buff_node(BUFF_NODE_S *buff_node, LIST_HEAD_S *head);
int clean_buff_byname(LIFE_S *self, const char *buff_name);
int clean_all_buff(LIFE_S *self);
BUFF_NODE_S *find_buff_byname(LIFE_S *self, const char *buff_name);
int clean_buff(LIFE_S *self, BUFF_NODE_S *node);

#endif
