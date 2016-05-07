/***********************************************************************************************************************
*  FOR DOTA, FOR ICEFROG!  
*
*  Copyright (c) xiaomaoshi, all rights reserved.
*
*  Email: mym_cocacola@163.com
*
* File name: dota_base.h
* Describe:
* Author: xiaomaoshi
* Action: create file
* Date: 2015/05/20
* History: 
***********************************************************************************************************************/

#ifndef __DOTA_BASE_H__
#define __DOTA_BASE_H__

#define DOTA_FREE(p)       \
    do                     \
    {                      \
        if (NULL != (p)) { \
            free(p);       \
            (p) = NULL;    \
        }                  \
    } while(0)

#endif
