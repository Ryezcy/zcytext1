/*****************************************************************************
* Copyright (C), 2013 ���ݰ��׵������޹�˾ www.mcuzone.com
* All rights reserved.
*
* �ļ���: systick.c
* ���ݼ���: ʵ��ϵͳ��શ�ʱ��systcik��ʼ��,������SYSTICK��ʱ��ʵ��΢�뼶
*           �ͺ��뼶����ʱ
* ��ǰ�汾��Rev1.0
* ���ߣ�Mcuzone-freescale
* ������ڣ�2013.01.08
******************************************************************************/


#include "systick.h"


static __IO uint32_t TimingDelay = 0;


__IO uint32_t  g_iRunTime = 0;            //ʱ�������



/******************************************************************************
�������ƣ�Systick_Initial
�������ܣ�����ϵͳ�δ�ʱ��ÿ1us������ʱ�ж�һ��
��ڲ�������
���ڲ�������
���ߣ�Mcuzone-freescale
******************************************************************************/
void Systick_Initial(void)
{
		SystemCoreClockUpdate();
		
		//SystemCoreClock  / 1000     1ms�ж�һ��
		//SystemCoreClock  / 100000	  10us�ж�һ��
		//SystemCoreClock  / 1000000  1us�ж�һ��
	
		if(SysTick_Config(SystemCoreClock / 1000))
		{
				while(1);
		}
}


/******************************************************************************
�������ƣ�Delay_10us
�������ܣ�ʵ��10΢�뼶��ʱ����
��ڲ�����ustime����Ҫ��ʱ��΢����,systickʵ��ÿ10us�����ж�һ��,�ò�����10us�ı���
���ڲ�������
���ߣ�Mcuzone-freescale
******************************************************************************/
void Delay_10us(uint32_t ustime)
{
		TimingDelay = ustime;
	
		while(TimingDelay != 0);
}

void DWT_DelayUs(uint32_t us)
{
    uint32_t startts, endts, ts;
    startts = DWT->CYCCNT;
    ts =  us * (SystemCoreClock /(1000*1000) ); 
    endts = startts + ts;      
    if(endts > startts)  
    {
        while(DWT->CYCCNT < endts);       
    }
    else
    {
        while(DWT->CYCCNT > endts);
        while(DWT->CYCCNT < endts);
    }
}

void DelayUs(uint32_t us)
{
    DWT_DelayUs(us);
}

/******************************************************************************
�������ƣ�Delay_ms
�������ܣ�ͨ������΢�뼶��ʱ����ʵ�ֺ��뼶��ʱ����
��ڲ�����mstime����Ҫ��ʱ�ĺ�������
���ڲ�������
���ߣ�Mcuzone-freescale
******************************************************************************/
void Delay_ms(uint32_t mstime)
{
		TimingDelay = mstime;
	
		while(TimingDelay != 0);
}



/******************************************************************************
�������ƣ�TimingDelay_Decrement
�������ܣ���ȡ���ĳ���,�� SysTick �жϺ��� SysTick_Handler()����
��ڲ�������
���ڲ�������
���ߣ�Mcuzone-freescale
******************************************************************************/
void TimingDelay_Decrement(void)
{
		if(TimingDelay != 0x00)
		{ 
				TimingDelay--;
		}
}



/******************************************************************************
�������ƣ�SysTick_ISR
�������ܣ�systick��ʱ�жϷ������,ϵͳ��ʱ��systickÿ��1ms�ͷ���һ���ж�
��ڲ�������
���ڲ�������
���ߣ�Mcuzone-freescale
******************************************************************************/
void SysTick_ISR(void)
{
		g_iRunTime++;                 //ʱ�������ÿ��1ms�Լ�1
	
		if(g_iRunTime == 0x80000000)  //��Ϊ�ü�������������Ϊ�޷��ŵ�32λ����,��������������ֵΪ0x7FFFFFFF
		{
				g_iRunTime = 0;           //�����ﵽ���ֵ��,��������0
		}
}



/******************************************************************************
�������ƣ�Systick_GetRunTime
�������ܣ���ȡsystick��ʱ����ǰ����ֵ
��ڲ�������
���ڲ���������systick��ʱ����ǰ�ļ���ֵ
���ߣ�Mcuzone-freescale
******************************************************************************/
uint32_t Systick_GetRunTime(void)
{
		uint32_t runtime;
	
		__set_PRIMASK(1);  		/* ���ж� */
	
		runtime = g_iRunTime;
	
		__set_PRIMASK(0);  		/* ���ж� */
	
		return runtime;
}



