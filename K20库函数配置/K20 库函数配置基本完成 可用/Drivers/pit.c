
#include "pit.h"
#include "common.h"
#include "MK20D5.h"
#define NULL 0

static u32 fac_us; 

static PIT_CallBackType PIT_CallBackTable[4] = {NULL};
static const IRQn_Type PIT_IRQnTable[] = 
{
    PIT0_IRQn,
    PIT1_IRQn,
    PIT2_IRQn,
    PIT3_IRQn,
};

 
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    
    fac_us = PeriphBusClock;
    fac_us /= 1000000;
    PIT->CHANNEL[PIT_InitStruct->chl].LDVAL = fac_us * PIT_InitStruct->timeInUs;
    PIT->CHANNEL[PIT_InitStruct->chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
}


void PIT_QuickInit(uint8_t chl, uint32_t timeInUs)
{
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = chl;
    PIT_InitStruct1.timeInUs = timeInUs;
    PIT_Init(&PIT_InitStruct1);
}


void PIT_ITDMAConfig(uint8_t chl, PIT_ITDMAConfig_Type config,u32 flag)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    if(flag)
    {
        NVIC_EnableIRQ(PIT_IRQnTable[chl]);
    }

    (flag)?
    (PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK):
    (PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK);
    
    if(!flag)
    {
        PIT->CHANNEL[chl].TFLG |= PIT_TFLG_TIF_MASK;
    }
}


void PIT_ResetCounter(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}
 
uint32_t PIT_GetCounterValue(uint8_t chl)
{
    return PIT->CHANNEL[chl].CVAL;
}



void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    if(AppCBFun != NULL)
    {
        PIT_CallBackTable[chl] = AppCBFun;
    }
}


static void PIT_IRQHandler(uint32_t instance)
{
    PIT->CHANNEL[instance].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[instance])
    {
        PIT_CallBackTable[instance]();
    }
}


void PIT0_IRQHandler(void)
{
    PIT_IRQHandler(0);
}

void PIT1_IRQHandler(void)
{
    PIT_IRQHandler(1);
}

void PIT2_IRQHandler(void)
{
    PIT_IRQHandler(2);
}

void PIT3_IRQHandler(void)
{
    PIT_IRQHandler(3);
}
