/*****************************************************************************
* Copyright (C), 2013 杭州安米电子有限公司 www.mcuzone.com
* All rights reserved.
*
* 文件名: Kinetis_it.c
* 内容简述: 实现Kinetis微控制器片上各种外设中断服务程序
*
* 当前版本：Rev1.0
* 作者：Mcuzone-freescale
* 完成日期：2013.01.08
******************************************************************************/

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



/******************************************************************************
函数名称：SysTick_Handler
函数功能：systick定时中断服务程序
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void SysTick_Handler(void)
{
		if ((SysTick->CTRL) & (1 << 16))   //读取COUNTFLAG位是否置1来判断是否发生systick异常中断
		{
				TimingDelay_Decrement();
		}
}



/******************************************************************************
函数名称：PORTD_IRQHandler
函数功能：PORTD中断服务程序
入口参数：无
出口参数：无
作者：Mcuzone-freescale
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
		trig_1=1;
		/*for(;led_dt>1100;led_dt--)
			{	
				led_fx = 6000;
				led_fx = led_fx - 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(led_fx); 													
			}				
		for(;led_dt>820;led_dt--)
			{	
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				//Delay_ms(8); 													
			}		
		for(;led_dt>620;led_dt--)
			{	
				led_fx = 3000;
				led_fx = led_fx + 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				//Delay_ms(led_fx); 													
			}			
			*/
		break;
	}
	while(a_value>96000&&a_value<97000)
	{
		trig_1=2;
	/*	for(;led_dt<720;led_dt++)
			{
				led_fx = 6000;
				led_fx = led_fx - 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				//Delay_ms(led_fx); 
			}		
		for(;led_dt<1100;led_dt++)
			{
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				//Delay_ms(8); 
			}		
		for(;led_dt<1300;led_dt++)
			{
				led_fx = 3000;
				led_fx = led_fx + 30;
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				//Delay_ms(led_fx); 
			}	*/
			break;
	}
		PORTB->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}


void PORTD_IRQHandler(void)
{
		
		PORTD->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}



/******************************************************************************
函数名称：PORTE_IRQHandler
函数功能：PORTE中断服务程序
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void PORTE_IRQHandler(void)
{
		
		
		PORTE->ISFR |= PORT_ISFR_ISF_MASK;          //清除中断标志
}


