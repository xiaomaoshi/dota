/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_frame.c
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/07/20
* History: 
***********************************************************************************************************************/

#include "dota_data.h"
#include "dota_list.h"
#include "dota_enum.h"
#include "dota_time.h"
#include <time.h>

extern void time_run(void);
extern int axe_register(void);
extern int es_register(void);
extern int centaur_register(void);

LIST_HEAD_S all_life;

OPER0_PF all_register[] = {
    axe_register,
    es_register,
    centaur_register,
    NULL,
};

int dota_schedule(void)
{
    int result = DOTA_INVALID;
    int over   = 0;
    LIFE_S      *life = NULL;
    LIST_HEAD_S *item = NULL;

    while (!over) {
        /*
         * 首先假定战争会结束，如果该标记后面被置为0，
         * 说明假定不成立，继续循环。
         */
        over = 1;

        /*
         * 根据注册的顺序，依次调用每个life的驱动函数run，有点类似
         * 回合制的游戏，调用某个run就相当于进入该life的回合，其他
         * 单位此时不能做任何动作。所有的run都执行一遍相当于一轮回
         * 合结束。
         */
        LIST_FOR_EACH(item, &all_life) {
            life = LIST_ENTRY(item, LIFE_S, life_list);
            life->ops->kill(life);

            if (LIFE_DEAD == life->life_state)
                continue;

            if (LIFE_ZOMBIE == life->life_state) {
                life->ops->death(life);
                continue;
            }

            result = life->ops->run(life);
            if (ERR_SHOULD_OVER == result)
                continue;

            /*
             * 到这一步说明战争还未结束，战斗结束的条件是，敌人全部
             * 死亡，或者所有单位全部死亡。
             */
            over = 0;
        }

        /* 
         * 一轮循环结束，时间流逝100毫秒，在每个100毫秒里，调用是有
         * 先后顺序的(根据注册的顺序)，这就跟单核CPU顺序调度进程是
         * 一样的，微观串行，宏观并行。其实说成一轮回合结束更合适。
         */
        time_run();
    }
    return DOTA_SUCCESS;
}


int dota_finish(void)
{
    LIFE_S      *life = NULL;
    LIST_HEAD_S *item = NULL;

    LIST_FOR_EACH(item, &all_life) {
        life = LIST_ENTRY(item, LIFE_S, life_list);
        if (life->ops->clean)
            life->ops->clean(life);
    }
    return DOTA_SUCCESS;
}


int dota_register(void)
{
    OPER0_PF *reg_func = all_register;

    INIT_LIST_HEAD(&all_life);

    while (*reg_func)
        (*reg_func++)();

    srand((unsigned)time(NULL));
    return DOTA_SUCCESS;
}
