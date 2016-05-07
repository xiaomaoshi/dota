/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_life.h
* Describe: 
* Author: xiaomaoshi
* Action: create file
* Date: 2015/07/20
* History:
***********************************************************************************************************************/
#ifndef __DOTA_LIFE_H__
#define __DOTA_LIFE_H__

#include "dota_data.h"

LIFE_S *find_enemy(LIFE_S *self, unsigned int dis);
LIFE_S **find_enemy_area(LIFE_S *self, unsigned int dis);
int register_life(LIFE_S *life);
#endif

