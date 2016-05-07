/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_life.c
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/07/20
* History: 
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_list.h"
#include "dota_enum.h"
#include "dota_life.h"

extern LIST_HEAD_S all_life;

/*
 * 英雄单位优先考虑攻击处于攻击范围内最近的敌方英雄，
 * 其他单位优先攻击离自己最近的敌方单位。
 */
LIFE_S *find_enemy(LIFE_S *self, unsigned int dis)
{
    LIFE_S      *life = NULL;
    LIST_HEAD_S *item = NULL;
    LIFE_S      *enemy_hero = NULL;
    LIFE_S      *enemy_unit = NULL;
    unsigned int X1 = 0;
    unsigned int Y1 = 0;
    unsigned int X2 = 0;
    unsigned int Y2 = 0;
    unsigned int tmp_dis = 0;
    unsigned int hero_dis = 0;
    unsigned int unit_dis = 0;
    unsigned int sqr_dis = SQAURE(dis);
    unsigned char id = 0;

    id = GET_CAMP(self);
    X1 = self->location.x;
    Y1 = self->location.y;

    LIST_FOR_EACH(item, &all_life) {
        life = LIST_ENTRY(item, LIFE_S, life_list);

        if (IS_DEAD(life))
            continue;

        if (id == GET_CAMP(life))
            continue;

        X2 = life->location.x;
        Y2 = life->location.y;
        if ((tmp_dis = DISTANCE(X1, Y1, X2, Y2)) > sqr_dis)
            continue;

        if (!unit_dis || tmp_dis < unit_dis) {
            enemy_unit = life;
            unit_dis = tmp_dis;
        }

        if (IS_HERO(life)) {
            if (!hero_dis || tmp_dis < hero_dis) {
                enemy_hero = life;
                hero_dis = tmp_dis;
            }
        }
    }

    if (IS_HERO(self))
        return (enemy_hero) ? enemy_hero : enemy_unit;

    return enemy_unit;
}

/*
 * 寻找self周围dis距离内的所有敌方单位。
 */
LIFE_S **find_enemy_area(LIFE_S *self, unsigned int dis)
{
    LIFE_S      *life = NULL;
    LIFE_S     **store = NULL;
    LIST_HEAD_S *item = NULL;
    unsigned int X1 = 0;
    unsigned int Y1 = 0;
    unsigned int X2 = 0;
    unsigned int Y2 = 0;
    unsigned int i  = 0;
    unsigned int sz = 16;
    unsigned int tmp_sz = 0;
    unsigned int sqr_dis = SQAURE(dis);
    unsigned char id = 0;

    id = GET_CAMP(self);
    X1 = self->location.x;
    Y1 = self->location.y;

    store = (LIFE_S **)malloc((sz + 1) * sizeof(LIFE_S *));
    if (!store)
        return NULL;

    LIST_FOR_EACH(item, &all_life) {
        life = LIST_ENTRY(item, LIFE_S, life_list);

        if (IS_DEAD(life))
            continue;

        if (id == GET_CAMP(life))
            continue;

        X2 = life->location.x;
        Y2 = life->location.y;
        if (DISTANCE(X1, Y1, X2, Y2) > sqr_dis)
            continue;

        if (i >= sz) {
            sz <<= 1;
            tmp_sz = (sz + 1) * sizeof(LIFE_S *);
            store = (LIFE_S **)realloc(store, tmp_sz);
        }
        store[i++] = life;
    }
    store[i] = NULL;
    return store;
}


/* 注册并初始化生命 */
int register_life(LIFE_S *life)
{
    DOTA_RETURN_IF_NULL(life, ERR_NULL_POINTER);

    if (life->ops->init)
        life->ops->init(life);

    list_add(&(life->life_list), &all_life);
    return DOTA_SUCCESS;
}
