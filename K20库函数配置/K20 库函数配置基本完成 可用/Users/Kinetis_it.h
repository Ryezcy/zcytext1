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


#ifndef  __KINETIS_IT_H__
#define  __KINETIS_IT_H__


#include <stdint.h>

#include "MK20D5.h"




/******************************************************************************
函数名称：SysTick_Handler
函数功能：systick定时中断服务程序
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void SysTick_Handler(void);
void PORTA_IRQHandler(void);
void PORTB_IRQHandler(void);
void PORTC_IRQHandler(void);
void PORTD_IRQHandler(void);
void PORTE_IRQHandler(void);

#endif





