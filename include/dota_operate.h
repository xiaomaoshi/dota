/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_operate.h
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#ifndef __DOTA_OPERATE_H__
#define __DOTA_OPERATE_H__

#include "dota_data.h"

#define HERO_ORGINAL_MONEY 825
#define HERO_HEAD_MONEY    225
#define HERO_SERIAL_KILLING_BONUS  50

#define HERO_SPELL_RESISTANCE      75

#define HERO_BUFF_INTERVAL_ONETIME 0xFFFF
#define HERO_BUFF_INTERVAL_DEFAULT 10
#define HERO_BUFF_INTERVAL_MIN     1

#define HERO_MAGIC_DAMAGE(usD, usSR)     (((usD) * (usSR)) / 100)
#define HERO_PHYSICAL_DAMAGE(usAK, usAR) \
    (((usAK) * 100) / (100 + 6 * (usAR)))

#define HERO_LOST_MONEY(n) \
    (HERO_HEAD_MONEY + (n) * HERO_SERIAL_KILLING_BONUS)

#define HERO_GET_MONEY(n) \
    (HERO_HEAD_MONEY + (n) * HERO_SERIAL_KILLING_BONUS)

int wilders_kill(LIFE_S *self);
int default_attack(LIFE_S *self);
int default_skill(LIFE_S *self, unsigned int id);
int default_equip(LIFE_S *self, unsigned int id);
int default_execbuff(LIFE_S *self);
int default_death(LIFE_S *self);
int default_rebirth(LIFE_S *self);
int default_kill(LIFE_S *self);
int default_clean(LIFE_S *self);
#endif
