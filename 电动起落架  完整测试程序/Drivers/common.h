#include <stdint.h>
#include <stddef.h>

#ifndef _COMMON_H_
#define _COMMON_H_
#define IP_CLK_ENABLE(x)        (*((uint32_t*) ClkTbl[x].addr) |= ClkTbl[x].mask)
#define IP_CLK_DISABLE(x)       (*((uint32_t*) ClkTbl[x].addr) &= ~ClkTbl[x].mask)
#define bool _Bool
#define true 1
#define false 0



/*****************************设置数据类型*****************************/
typedef 	unsigned 	char		u8;	  //无符号型
typedef 	unsigned 	short int	u16;
typedef 	unsigned 	long  int	u32;

typedef 	char					s8;	  //有符号型
typedef 	short int				s16;
typedef 	long  int				s32;

typedef struct 
{
    void *      addr;
    uint32_t    mask;
    uint32_t    shift;
}Reg_t;

typedef struct
{
    uint32_t ip:3;
    uint32_t io:3;
    uint32_t mux:3;
    uint32_t pin_start:5;
    uint32_t pin_cnt:5;
    uint32_t chl:5;
    uint32_t reserved:8;
}map_t;

#endif




#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#if defined(LIB_DEBUG)
    #include <stdio.h>
    #define LIB_TRACE	printf
#else
    #define LIB_TRACE(...)
#endif
