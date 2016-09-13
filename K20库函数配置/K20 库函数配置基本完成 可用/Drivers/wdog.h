
#ifndef __CH_LIB_WDOG_H__
#define __CH_LIB_WDOG_H__

#include "common.h"
#include <stdint.h>
#include "MK20D5.h"

typedef enum
{
    kWDOG_Mode_Normal,            /**< 普通模式看门狗 */
    kWDOG_Mode_Window,            /**< WDOG 窗体模式  */
}WDOG_Mode_Type;

typedef struct
{
    WDOG_Mode_Type      mode;                   ///<WDOG 工作模式选择
    uint32_t            timeOutInMs;            ///<WDOG 最大喂狗时限
    uint32_t            windowInMs;             ///<WDOG 开窗起始时间
}WDOG_InitTypeDef;

typedef void (*WDOG_CallBackType)(void);

/* API functions */
void WDOG_QuickInit(uint32_t timeInUs);
void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct);
void WDOG_ITDMAConfig(bool status);
void WDOG_CallbackInstall(WDOG_CallBackType AppCBFun);
uint32_t WDOG_GetResetCounter(void);
void WDOG_ClearResetCounter(void);
void WDOG_Refresh(void);
uint32_t WDOG_GetCurrentCounter(void);


#endif



