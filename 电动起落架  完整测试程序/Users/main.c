/*****************************************************************************
* 深圳市科比特航空科技有限公司*
* 电动起落架项目软件实现部分*
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
#include "adc.h"

extern int trig_1;
int led_dt=970 ;
float i;
float vtemp;
float temp;

void HardwareInit(void);

void HardwareInit(void)
{
		GPIO_Configuration(PORTA,PTA,GPIO_Pin4,1);
		GPIO_Configuration(PORTA,PTA,GPIO_Pin5,1);
		GPIO_Configuration(PORTA,PTA,GPIO_Pin12,1);
		GPIO_Configuration(PORTA,PTA,GPIO_Pin13,1);
		
		GPIO_Configuration(PORTB,PTB,GPIO_Pin0,0);
		GPIO_Configuration(PORTB,PTB,GPIO_Pin2,1);
		
		GPIO_Configuration(PORTC,PTC,GPIO_Pin3,1);
		
		GPIO_Configuration(PORTD,PTD,GPIO_Pin6,1);
		
		GPIO_Configuration(PORTE,PTE,GPIO_Pin0,1);
		GPIO_Configuration(PORTE,PTE,GPIO_Pin1,1);
		/*B2开关输出电压*/
		GPIO_Set(PTB,GPIO_Pin2);  	
}

int main(void)
{	
	  Systick_Initial();     
		HardwareInit();
		PIT_QuickInit(HW_PIT_CH0, 20*1000);
		FTM_PWM_init(FTMn0, CH2, 50, 970);  
		GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPD);  
		exti_init(PORTxB,1, 3 ,either_up);

	  ADC_InitTypeDef AD_InitStruct1;
		AD_InitStruct1.instance = HW_ADC0;  
		AD_InitStruct1.clockDiv = kADC_ClockDiv2;
		AD_InitStruct1.resolutionMode = kADC_SingleDiff10or11;
		AD_InitStruct1.triggerMode = kADC_TriggerSoftware; 
		AD_InitStruct1.singleOrDiffMode = kADC_Single; 
		AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; 
		AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable;
		ADC_Init(&AD_InitStruct1);
		/* 启动一次ADC转换 */
    ADC_StartConversion(HW_ADC0,8, kADC_MuxA);
	  /*检测灯*/
		GPIO_Set(PTD,GPIO_Pin6);
	  //FTM_PWM_Duty(FTMn0, CH2,990);  //1号脚架（970  310）   2号脚架（990  330）  965  295
		while (1)
	{
		while(trig_1==0)
		{
			FTM_PWM_Duty(FTMn0, CH2,970);
			break;
		}
		while(trig_1==1)
		{
			for(;led_dt<970;led_dt++)
			{
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(4); 
			}
      /*过流保护，关断通道*/			
			while(i < 451) 
			{
				GPIO_Set(PTB,GPIO_Pin2); 
				break;
			}
			break;
		}
		/*电流检测*/
		{
			ADC_StartConversion(HW_ADC0, 8,kADC_MuxA);
			while(ADC_IsConversionCompleted(HW_ADC0, kADC_MuxA));
			i=ADC_ReadValue(HW_ADC0, kADC_MuxA);
			/*开启通道*/
			while(i > 450)
			{
				GPIO_Reset(PTB,GPIO_Pin2); 
				break;
			}
	  }
			
		while(trig_1==2)
		{
			for(;led_dt>310;led_dt--)
			{	
				FTM_PWM_Duty(FTMn0, CH2,led_dt);
				Delay_ms(4); 													
			}	
			/*过流保护，关断通道*/	
			while(i < 451)
			{
				GPIO_Set(PTB,GPIO_Pin2); 
				break;
			}
			break;
		}
		/*电流检测*/
		{
			ADC_StartConversion(HW_ADC0, 8,kADC_MuxA);
			while(ADC_IsConversionCompleted(HW_ADC0, kADC_MuxA));
			i=ADC_ReadValue(HW_ADC0, kADC_MuxA);
			/*开启通道*/	
			while(i > 450)
			{
				GPIO_Reset(PTB,GPIO_Pin2); 
				break;
			}
	  }
	}
}


