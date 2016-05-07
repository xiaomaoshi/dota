/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_equipment.h
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/
#ifndef __DOTA_EQUIPMENT_H__
#define __DOTA_EQUIPMENT_H__

#include "dota_data.h"

/* 道具在商店里的顺序 */
typedef enum
{
    EQP_GUINSOOS_SCYTHE_OF_VYSE = 0,
    EQP_EUL_SCEPTER_OF_DIVINITY,
    EQP_XIAOMAOSHI_EGGS_OF_EVIL,
    EQP_ICEFROG,

    EQP_BUTT,
}EQP_E;

#define NAME_GSV "Guinsoos Scythe of Vyse"
#define NAME_EUL "Eul Scepter of Divinity"
#define NAME_XMS "Xiaomaoshi Eggs of Evil"

/* 各物品的初始化信息 */
extern EQUIPMENT_S g_shop[];
int takeon_equip(LIFE_S *self, int box_id, EQUIPMENT_S *eqp);
int takeoff_equip(LIFE_S *self, int box_id);
int init_box(LIFE_S *self);
#endif
