/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Email: mym_cocacola@163.com
*
* File name: dota_list.h
* Describe: see linux kernel code -- list.h
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/


#ifndef __DOTA_LIST_H__
#define __DOTA_LIST_H__

#if defined(WIN32) && !defined(__cplusplus)
#define inline __inline
#endif

typedef struct list_head
{
    struct list_head *next, *prev;
}LIST_HEAD_S;


#define INIT_LIST_HEAD(ptr)                       \
    do                                            \
    {                                             \
        (ptr)->next = (ptr);                      \
        (ptr)->prev = (ptr);                      \
    } while (0)


static inline int list_empty(LIST_HEAD_S *head)
{
    return head->next == head;
}

static inline void list_del(LIST_HEAD_S *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}


static inline void list_add(LIST_HEAD_S *item, LIST_HEAD_S *where)
{
    item->next = where->next;
    item->prev = where;
    where->next->prev = item;
    where->next = item;
}

#define LIST_ENTRY(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define LIST_FOR_EACH_SAFE(pos, temp, head)    \
    for (pos = (head)->next, temp = pos->next; \
         pos != (head);                        \
         pos = temp, temp = temp->next)

#endif
