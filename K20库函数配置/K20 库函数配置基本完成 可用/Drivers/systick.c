
#include "systick.h"


static __IO uint32_t TimingDelay = 0;


__IO uint32_t  g_iRunTime = 0;            //时间计数器

void Systick_Initial(void)
{
		SystemCoreClockUpdate();
		
		//SystemCoreClock  / 1000     1ms中断一次
		//SystemCoreClock  / 100000	  10us中断一次
		//SystemCoreClock  / 1000000  1us中断一次
	
//		if(SysTick_Config(SystemCoreClock / 1000))
//		{
//				while(1);
//		}
}

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

void Delay_ms(uint32_t mstime)
{
		TimingDelay = mstime;
	
		while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
		if(TimingDelay != 0x00)
		{ 
				TimingDelay--;
		}
}


void SysTick_ISR(void)
{
		g_iRunTime++;                 //时间计数器每隔1ms自加1
	
		if(g_iRunTime == 0x80000000)  //因为该计数器变量定义为无符号的32位整数,所以其计数的最大值为0x7FFFFFFF
		{
				g_iRunTime = 0;           //计数达到最大值后,计数器清0
		}
}

uint32_t Systick_GetRunTime(void)
{
		uint32_t runtime;
	
		__set_PRIMASK(1);  		/* 关中断 */
	
		runtime = g_iRunTime;
	
		__set_PRIMASK(0);  		/* 开中断 */
	
		return runtime;
}



