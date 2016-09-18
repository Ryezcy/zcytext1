
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

int i;
float vtemp;
float temp;
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
		Systick_Initial();                            //��ʼ��systick��ʱ��,ʵ��1ms��ʱ�ж�	
		HardwareInit();
		
		//PIT_QuickInit(HW_PIT_CH0, 20*1000);
	  ADC_InitTypeDef AD_InitStruct1;
    AD_InitStruct1.instance = HW_ADC0;  
    AD_InitStruct1.clockDiv = kADC_ClockDiv2;
    AD_InitStruct1.resolutionMode = kADC_SingleDiff10or11;
    AD_InitStruct1.triggerMode = kADC_TriggerSoftware; /* �������ת�� */
    AD_InitStruct1.singleOrDiffMode = kADC_Single; /*����ģʽ */
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; 
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable;
    ADC_Init(&AD_InitStruct1);
    
    /* ��ʼ����Ӧ���� */
    /* 26ͨ�� оƬ�ڲ����ӵ��� Ƭ���¶ȼ��� �������� */
    
    /* ����һ��ADCת�� */
    ADC_StartConversion(HW_ADC0, 0, kADC_MuxA);
    
    while(1)
    {
        ADC_StartConversion(HW_ADC0, 0,kADC_MuxA);
        while(ADC_IsConversionCompleted(HW_ADC0, kADC_MuxA));
			  i=ADC_ReadValue(HW_ADC0, kADC_MuxA);
        vtemp = ((float)ADC_ReadValue(HW_ADC0, kADC_MuxA)/4096)*3.3;
			
        if(vtemp >= 0.7012)
        {
            temp = (vtemp-0.7012)/.001646;
        }
        else
        {
            temp = (vtemp-0.7012)/.001769;
        }
        temp=25 - temp;
       
    }
    
}


