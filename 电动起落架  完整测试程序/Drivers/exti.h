#ifndef _EXTI_H_
#define _EXTI_H_

#include "common.h"
#include "gpio.h"



typedef enum 
{
    zero_down     = 0x08u,     //�͵�ƽ�������ڲ�����
    rising_down   = 0x09u,     //�����ش������ڲ�����
    falling_down  = 0x0Au,     //�½��ش������ڲ�����
    either_down   = 0x0Bu,     //�����ش������ڲ�����
    one_down      = 0x0Cu,     //�ߵ�ƽ�������ڲ�����

    //�����λ��־����������
    zero_up       = 0x88u,     //�͵�ƽ�������ڲ�����
    rising_up     = 0x89u,     //�����ش������ڲ�����
    falling_up    = 0x8Au,     //�½��ش������ڲ�����
    either_up     = 0x8Bu,     //�����ش������ڲ�����
    one_up        = 0x8Cu      //�ߵ�ƽ�������ڲ�����
} exti_cfg;

void  exti_init(PORTx  ,u8 pin, u8 priority ,exti_cfg );


#endif
