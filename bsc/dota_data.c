/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_data.c
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#include "dota_data.h"

BUFF_NODE_S *create_buff_node(void)
{
    BUFF_NODE_S *buff_node = NULL;

    buff_node = (BUFF_NODE_S *)malloc(sizeof(BUFF_NODE_S));
    DOTA_RETURN_IF_NULL(buff_node, NULL);

    memset(buff_node, 0, sizeof(BUFF_NODE_S));

    INIT_LIST_HEAD(&(buff_node->buff_list));
    return buff_node;
}

int add_buff_node(BUFF_NODE_S *buff_node, LIST_HEAD_S *head)
{
    LIST_HEAD_S *item     = NULL;
    BUFF_NODE_S *pstBuffTemp = NULL;

    DOTA_RETURN_IF_NULL(head, ERR_NULL_POINTER);
    DOTA_RETURN_IF_NULL(buff_node, ERR_NULL_POINTER);

    LIST_FOR_EACH(item, head) {
        pstBuffTemp = LIST_ENTRY(item, BUFF_NODE_S, buff_list);
        /*
         * 按照BUFF到期的顺序插入节点，即到期时间
         * 越晚的BUFF，越靠近链表的尾部
         */
        if (buff_node->start + buff_node->duration <
            pstBuffTemp->start + pstBuffTemp->duration)
            break;
    }
    list_add(&(buff_node->buff_list), item->prev);
    return DOTA_OK;
}


BUFF_NODE_S *find_buff_byname(LIFE_S *self, const char *buff_name)
{
    LIST_HEAD_S *item = NULL;
    LIST_HEAD_S *head = NULL;
    BUFF_NODE_S *buff_node = NULL;
    unsigned int str_len  = 0;

    DOTA_RETURN_IF_NULL(self, NULL);
    DOTA_RETURN_IF_NULL(buff_name, NULL);

    head = &(self->buff_list);
    str_len = strlen(buff_name);
    LIST_FOR_EACH(item, head) {
        buff_node = LIST_ENTRY(item, BUFF_NODE_S, buff_list);

        /* 若名称一样，则说明找到了该buff，返回OK */
        if (!strncmp(buff_node->buff_name, buff_name, str_len))
            return buff_node;
    }
    return NULL;
}


int clean_buff(LIFE_S *self, BUFF_NODE_S *node)
{
    SKILL_PF clean;
    LIST_HEAD_S *entry;

    DOTA_RETURN_IF_NULL(node, ERR_NULL_POINTER);

    entry = &(node->buff_list);
    list_del(entry);

    clean = node->clean_buff;
    if (NULL != clean)
        clean(self, node->owner);

    DOTA_FREE(node);
    return DOTA_OK;
}


int clean_buff_byname(LIFE_S *self, const char *buff_name)
{
    LIST_HEAD_S  *item      = NULL;
    LIST_HEAD_S  *temp      = NULL;
    LIST_HEAD_S  *head      = NULL;
    BUFF_NODE_S  *buff_node = NULL;
    SKILL_PF      clean     = NULL;
    unsigned int  str_len   = 0;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);
    DOTA_RETURN_IF_NULL(buff_name, ERR_NULL_POINTER);

    head = &(self->buff_list);
    str_len = strlen(buff_name);
    LIST_FOR_EACH_SAFE(item, temp, head) {
        buff_node = LIST_ENTRY(item, BUFF_NODE_S, buff_list);

        /* 若名称一样，则清理该buff */
        if (!strncmp(buff_node->buff_name, buff_name, str_len)) {
            list_del(item);
            clean = buff_node->clean_buff;
            if (NULL != clean)
                clean(self, buff_node->owner);

            DOTA_FREE(buff_node);
            return DOTA_OK;
        }
    }
    return ERR_INVALID_BUFF;
}


int clean_all_buff(LIFE_S *self)
{
    LIST_HEAD_S *item      = NULL;
    LIST_HEAD_S *temp      = NULL;
    LIST_HEAD_S *head      = NULL;
    BUFF_NODE_S *buff_node = NULL;

    DOTA_RETURN_IF_NULL(self, ERR_NULL_POINTER);

    head = &(self->buff_list);
    LIST_FOR_EACH_SAFE(item, temp, head) {
        buff_node = LIST_ENTRY(item, BUFF_NODE_S, buff_list);
        list_del(item);
        DOTA_FREE(buff_node);
    }
    return DOTA_OK;
}

