
#include <stdint.h>

#include "Kinetis_it.h"
#include "systick.h"
#include "gpio.h"
#include "FTM.h"
#include "pit.h"

int led_fx ;
int led_dt ;
int a_start;
int a_end;
int a_value;

/******************************************************************************
systick定时中断服务程序
******************************************************************************/
void SysTick_Handler(void)
{
		if ((SysTick->CTRL) & (1 << 16))   //读取COUNTFLAG位是否置1来判断是否发生systick异常中断
		{
				TimingDelay_Decrement();
		}
}


/******************************************************************************
PORTE中断服务程序
******************************************************************************/

void PORTB_IRQHandler(void)
{
	//GPIO_Set(PTD,GPIO_Pin6);
	GPIO_Toggle(PTA,GPIO_Pin13);
	a_end = a_start;
	a_start=PIT_GetCounterValue(HW_PIT_CH0);
	if(a_end != a_start)
	{
		a_value=a_end - a_start;
	}
	else
	{
		a_value=0;
	}
	if(a_value < 0)
	{
		a_value=-a_value;
	}
	else
	{
		;
	}
	while(a_value>48000&&a_value<49000)
	{
		
		for(;led_dt>1100;led_dt--)
			{	
				led_fx = 6000;
				led_fx = led_fx - 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(led_fx); 													
			}				
		for(;led_dt>720;led_dt--)
			{	
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(3000); 													
			}		
		for(;led_dt>620;led_dt--)
			{	
				led_fx = 3000;
				led_fx = led_fx + 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(led_fx); 													
			}			
			
		break;
	}
	while(a_value>96000&&a_value<97000)
	{
		for(;led_dt<720;led_dt++)
			{
				led_fx = 6000;
				led_fx = led_fx - 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(led_fx); 
			}		
		for(;led_dt<1200;led_dt++)
			{
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(3000); 
			}		
		for(;led_dt<1300;led_dt++)
			{
				led_fx = 3000;
				led_fx = led_fx + 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				DelayUs(led_fx); 
			}	
			break;
	}
		PORTB->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}



/*********************************************************************************************/
void PORTA_IRQHandler(void)
{
	GPIO_Toggle(PTA,GPIO_Pin4);
	FTM_PWM_Duty(FTMn0, CH1, 100);           //FTM1_CH0 PTA12
	PORTA->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}

void PORTC_IRQHandler(void)
{
		if (((PORTC->ISFR >> 6) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin4);				
		}
		
		if (((PORTC->ISFR >> 7) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin5);				
		}
		
		PORTC->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}

void PORTD_IRQHandler(void)
{
		if (((PORTD->ISFR >> 6) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin4);				
		}
		
		if (((PORTD->ISFR >> 7) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin5);				
		}
		
		PORTD->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}

void PORTE_IRQHandler(void)
{
		if (((PORTE->ISFR >> 6) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin4);				
		}
		
		if (((PORTE->ISFR >> 7) & 0x01) != 0)
		{
				GPIO_Toggle(PTA,GPIO_Pin5);				
		}
		
		PORTE->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}


