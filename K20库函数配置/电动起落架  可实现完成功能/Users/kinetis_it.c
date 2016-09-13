
#include <stdint.h>
#include <stdio.h>

#include "Kinetis_it.h"
#include "systick.h"
#include "gpio.h"
#include "FTM.h"
#include "pit.h"


int a_start;
int a_end;
int a_value;

int trig_1;

void SysTick_Handler(void)
{
		if ((SysTick->CTRL) & (1 << 16))   //读取COUNTFLAG位是否置1来判断是否发生systick异常中断
		{
				TimingDelay_Decrement();
		}
}

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
		trig_1=1;
		break;
	}
	while(a_value>96000&&a_value<97000)
	{
		trig_1=2;
		break;
	}
		PORTB->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}


void PORTD_IRQHandler(void)
{
		
		PORTD->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}

void PORTE_IRQHandler(void)
{
		
		
		PORTE->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}


