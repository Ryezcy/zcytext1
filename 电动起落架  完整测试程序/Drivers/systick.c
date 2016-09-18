/*****************************************************************************
* Copyright (C), 2013 杭州安米电子有限公司 www.mcuzone.com
* All rights reserved.
*
* 文件名: systick.c
* 内容简述: 实现系统嘀嗒定时器systcik初始化,并利用SYSTICK定时器实现微秒级
*           和毫秒级的延时
* 当前版本：Rev1.0
* 作者：Mcuzone-freescale
* 完成日期：2013.01.08
******************************************************************************/


#include "systick.h"


static __IO uint32_t TimingDelay = 0;


__IO uint32_t  g_iRunTime = 0;            //时间计数器



/******************************************************************************
函数名称：Systick_Initial
函数功能：配置系统滴答定时器每1us发生定时中断一次
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void Systick_Initial(void)
{
		SystemCoreClockUpdate();
		
		//SystemCoreClock  / 1000     1ms中断一次
		//SystemCoreClock  / 100000	  10us中断一次
		//SystemCoreClock  / 1000000  1us中断一次
	
		if(SysTick_Config(SystemCoreClock / 1000))
		{
				while(1);
		}
}


/******************************************************************************
函数名称：Delay_10us
函数功能：实现10微秒级延时函数
入口参数：ustime：需要延时的微秒数,systick实现每10us发生中断一次,该参数是10us的倍数
出口参数：无
作者：Mcuzone-freescale
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
函数名称：Delay_ms
函数功能：通过调用微秒级延时函数实现毫秒级延时函数
入口参数：mstime：需要延时的毫秒数。
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void Delay_ms(uint32_t mstime)
{
		TimingDelay = mstime;
	
		while(TimingDelay != 0);
}



/******************************************************************************
函数名称：TimingDelay_Decrement
函数功能：获取节拍程序,在 SysTick 中断函数 SysTick_Handler()调用
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void TimingDelay_Decrement(void)
{
		if(TimingDelay != 0x00)
		{ 
				TimingDelay--;
		}
}



/******************************************************************************
函数名称：SysTick_ISR
函数功能：systick定时中断服务程序,系统定时器systick每隔1ms就发生一次中断
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void SysTick_ISR(void)
{
		g_iRunTime++;                 //时间计数器每隔1ms自加1
	
		if(g_iRunTime == 0x80000000)  //因为该计数器变量定义为无符号的32位整数,所以其计数的最大值为0x7FFFFFFF
		{
				g_iRunTime = 0;           //计数达到最大值后,计数器清0
		}
}



/******************************************************************************
函数名称：Systick_GetRunTime
函数功能：读取systick定时器当前计数值
入口参数：无
出口参数：返回systick定时器当前的计数值
作者：Mcuzone-freescale
******************************************************************************/
uint32_t Systick_GetRunTime(void)
{
		uint32_t runtime;
	
		__set_PRIMASK(1);  		/* 关中断 */
	
		runtime = g_iRunTime;
	
		__set_PRIMASK(0);  		/* 开中断 */
	
		return runtime;
}



