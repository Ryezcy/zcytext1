
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
}

int main(void)
{
	Systick_Initial();                            //初始化systick定时器,实现1ms定时中断	
	HardwareInit();
	
	PIT_QuickInit(HW_PIT_CH0, 20*1000);
	FTM_PWM_init(FTMn0, CH2, 50, 1300);           //FTM1_CH0 PTA12
	
	GPIO_Set(PTB,GPIO_Pin2);     
  	
	GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPD);  //按键	
	exti_init(PORTxB,1, 3 ,either_up);
	
	/*GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_IPD);  //按键	
	exti_init(PORTxD,7, 3 ,either_up);*/
	
	while(1)
	{
		
		/*GPIO_Reset(PTD,GPIO_Pin6);
		DelayUs(300000);
		GPIO_Set(PTD,GPIO_Pin6);
		DelayUs(300000);*/
		//FTM_PWM_Duty(FTMn0, CH2,1000);	
	}
}


