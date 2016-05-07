/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_enum.h
* Describe: all enum codes in dota were described at here.
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#ifndef __DOTA_ENUM_H__
#define __DOTA_ENUM_H__

#define DOTA_OK            0
#define DOTA_FAILED        1

#define DOTA_SUCCESS_START 0x1000 // 成功开始
#define DOTA_SUCCESS_END   0x1FFF // 成功结束

#define DOTA_ERROR_START   0x2000 // 错误开始
#define DOTA_ERROR_END     0x2FFF // 错误开始

#define DOTA_INVALID       0xFFFF // 无效值

typedef enum tag_success
{
    DOTA_SUCCESS = DOTA_SUCCESS_START,   // 释放成功
    SUC_IGNORE_CD,                       // 成功并且忽略冷却

    SUC_BUTT,
}SUCCESS_E;

typedef enum tag_error
{
    ERR_NULL_POINTER = DOTA_ERROR_START, // 空指针
    ERR_MALLOC_FAILED,                   // 申请内存失败
    ERR_INVALID_PARAMETER,               // 无效的参数

    ERR_OUT_OF_HEALTH,                   // 生命值不够
    ERR_OUT_OF_MANA,                     // 魔法值不够
    ERR_COLD_DOWN,                       // 冷却中
    ERR_WRONG_OBJ,                       // 错误的对象
    ERR_MAGIC_IMMUNITY,                  // 魔法免疫
    ERR_TRIGGER_FAILED,                  // 触发失败

    ERR_CANNOT_BE_ATTACKED,              // 目标不能被攻击
    ERR_CANNOT_MAGIC_ATTACK,             // 不能进行魔法攻击
    ERR_CANNOT_PHYSICAL_ATTACK,          // 不能进行物理攻击
    ERR_CANNOT_USE_EQUIPMENT,            // 不能使用物品

    ERR_UNKNOW_SKILL,                    // 未知的技能
    ERR_INVALID_SKILL,                   // 无效的技能
    ERR_NO_ENEMY,                        // 没有找到敌方单位
    ERR_UNKNOW_EQP,                      // 未知的物品
    ERR_INVALID_EQP,                     // 无效的物品
    ERR_INVALID_BUFF,                    // 无效的BUFF

    ERR_SHOULD_OVER,                     // 表示某个单位应该结束

    ERROR_BUTT,                          // 不要再此后面添加错误码
}ERROR_E;

typedef enum
{
    LIFE_BIRTH,
    LIFE_RUNNING,
    LIFE_ZOMBIE,
    LIFE_DEAD,

    STATE_BUTT,
}LIFE_STATE_E;
#endif
