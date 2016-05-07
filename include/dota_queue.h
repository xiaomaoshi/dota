/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_queue.h
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History:
***********************************************************************************************************************/

#ifndef __DOTA_QUEUE_H__
#define __DOTA_QUEUE_H__

#if defined(WIN32) && !defined(__cplusplus)
#define inline __inline
#endif

#include "dota_base.h"

typedef struct
{
    void **data;
    unsigned int size;
    unsigned int head, tail;
}QUEUE_S;

static inline void queue_init(QUEUE_S *queue, unsigned int size)
{
    queue->data = (void **)malloc((size + 1) * sizeof(void *));
    if (!queue->data)
        return;

    queue->head = queue->tail = 0;
    queue->size = size;
}

static inline void queue_free(QUEUE_S *queue)
{
    if (!queue)
        return;

    DOTA_FREE(queue->data);
    DOTA_FREE(queue);    
}

static inline int queue_empty(QUEUE_S *queue)
{
    return queue->head == queue->tail;
}

static inline int queue_full(QUEUE_S *queue)
{
   return ((queue->tail + 1) % queue->size) == queue->head;
}

static inline void queue_in(QUEUE_S *queue, void *data)
{
    if (queue_full(queue))
        return;

    queue->data[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->size;
}

static inline void *queue_out(QUEUE_S *queue)
{
    void *data = NULL;
    if (queue_empty(queue))
        return NULL;

    data = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    return data;
}
#endif
