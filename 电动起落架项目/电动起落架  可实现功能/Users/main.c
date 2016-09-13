/*****************************************************************************
* Copyright (C), 2013 杭州安米电子有限公司 www.mcuzone.com
* All rights reserved.
*
* 文件名: main.c
* 内容简述: 示例工程主函数,实现K20-EK开发板LED灯闪烁实验
*
* 当前版本：Rev1.0
* 作者：Mcuzone-freescale
* 完成日期：2013.01.08
******************************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "MK20D5.h"
#include "gpio.h"
#include "FTM.h"
#include "systick.h"
#include "uart.h"
#include "exti.h"
#include "pit.h"
#include "common.h"

extern int trig_1;
int led_dt=990 ;

void HardwareInit(void);


void HardwareInit(void)
{
	GPIO_Configuration(PORTA,PTA,GPIO_Pin4,1);
	GPIO_Configuration(PORTA,PTA,GPIO_Pin5,1);
	GPIO_Configuration(PORTA,PTA,GPIO_Pin12,1);
	GPIO_Configuration(PORTA,PTA,GPIO_Pin13,1);
	
	GPIO_Configuration(PORTB,PTB,GPIO_Pin2,1);

	GPIO_Configuration(PORTC,PTC,GPIO_Pin3,1);
	
	GPIO_Configuration(PORTD,PTD,GPIO_Pin6,1);
	//GPIO_Configuration(PORTD,PTD,GPIO_Pin7,1);
	
	GPIO_Configuration(PORTE,PTE,GPIO_Pin0,1);
	GPIO_Configuration(PORTE,PTE,GPIO_Pin1,1);
	GPIO_Set(PTB,GPIO_Pin2);  //B2开关输出电压
}

int main(void)
{
		
	  Systick_Initial();                         //初始化systick定时器,实现1ms定时中断
		HardwareInit();
		PIT_QuickInit(HW_PIT_CH0, 20*1000);
		FTM_PWM_init(FTMn0, CH2, 50, 990);           //FTM1_CH0 PTA12

		     
			
		GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPD);  //按键	
		exti_init(PORTxB,1, 3 ,either_up);
		
	//FTM_PWM_Duty(FTMn0, CH2,310);  //980  300   970  310
	/*while (1)
	{
		GPIO_Reset(PTD,GPIO_Pin6);
		Delay_ms(300);
		GPIO_Set(PTD,GPIO_Pin6);
		Delay_ms(300);
		
		for(;led_dt>310;led_dt--)
			{	
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(5); 													
			}	
			Delay_ms(300);
			
			for(;led_dt<970;led_dt++)
			{
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(4); 
			}	
			Delay_ms(300);
	}*/
		while (1)
	{
		GPIO_Reset(PTD,GPIO_Pin6);
		Delay_ms(300);
		GPIO_Set(PTD,GPIO_Pin6);
		Delay_ms(300);
		
		while(trig_1==1)
		{
			for(;led_dt>330;led_dt--)
			{	
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(4); 													
			}	
			break;
		}
		
		while(trig_1==2)
		{
			for(;led_dt<990;led_dt++)
			{
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(4); 
			}	
			break;
		}
	}
}


