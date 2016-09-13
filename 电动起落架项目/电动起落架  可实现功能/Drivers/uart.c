/*****************************************************************************
* Copyright (C), 2013 杭州安米电子有限公司 www.mcuzone.com
* All rights reserved.
*
* 文件名: uart.c
* 内容简述: 配置初始化UART0,相关的UART0的相关发送与接收驱动函数
*
* 当前版本：Rev1.0
* 作者：Mcuzone-freescale
* 完成日期：2013.01.08
******************************************************************************/

#include "uart.h"



/******************************************************************************
函数名称：UART0_Configuration
函数功能：串口0初始化配置为：波特率9600(模块时钟48MHz),8位数据位,1位停止位,无奇偶校验,
					无硬件流控制,传输时低位在前(LSB First)。
入口参数：无
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void UART0_Configuration(void)
{	
		SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;	      //使能uart0时钟模块
	
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;       //与uart0相关的引脚的时钟使能(PTB16,PTB17)
	
		PORTB->PCR[16] |= PORT_PCR_MUX(0x3);       //在PTB16上使能UART0_RXD功能
	
		PORTB->PCR[17] |= PORT_PCR_MUX(0x3);       //在PTB17上使能UART0_TXD功能
		
		
		UART0->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);      //配置串口基本参数之前,先禁止发送和接收
		
		UART0->C1 &= ~(UART_C1_M_MASK);                         //配置UART为8位数据传输模式
	
		UART0->C1 &= ~(UART_C1_PE_MASK);                        //配置UART无奇偶校验
	
		UART0->S2 &= ~UART_S2_MSBF_MASK;                         //配置UART传输数据低位在前(LSB First)
		
		//配置波特率发生器,Baud Rate=UART Module Clock/(16*(SBR[0:12]+BRFD)) = 48MHz/(16*(312+0.5)) = 9600bit/s;
		//按照下面的配置,UART0模块时钟48MHz,计算出波特率为9600bit/s
		UART0->BDH = 0x01;
	
		UART0->BDL = 0x38;                                     //波特率发生器整数部分312(0x138)
	
		UART0->C4 |= UART_C4_BRFA(0x10);                        //波特率发生器小数部分补偿0x10,计算BRFD的值,BRFD=16/32=0.5

		UART0->RWFIFO = UART_RWFIFO_RXWATER(1);
		
		UART0->TWFIFO = UART_TWFIFO_TXWATER(0);
		
		UART0->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;         //使能发送器与接收器
}



/******************************************************************************
函数名称：UART0_SendOneByte
函数功能：从串口0向外发送一字节数据
入口参数：buffer：待发送的一字节数据
出口参数：无
作者：Mcuzone-freescale
******************************************************************************/
void UART0_SendOneByte(uint8_t buffer)
{
		while (!(UART0->S1 & UART_S1_TDRE_MASK));               //等待发送缓冲区清空(即等待发送缓冲区的数据移入移位寄存器中发送出去)
	
		UART0->D = (uint8_t)buffer;                             //向数据寄存器中送入数据
}


/******************************************************************************
函数名称：UART0_ReadOneByte
函数功能：从串口0读取接收的一字节数据并返回
入口参数：无
出口参数：返回接收的一字节数据
作者：Mcuzone-freescale
******************************************************************************/
uint8_t UART0_ReadOneByte(void)
{
		uint8_t buffer = 0;
	
		if (UART0->S1 & UART_S1_RDRF_MASK)                      //判断接收缓冲区是否有有效数据
		{
				buffer = UART0->D;
			
				return (buffer);
		}
		else
		{
				return 0;
		}
}



/******************************************************************************
函数名称：fputc
函数功能：软件重定义fputc函数,这样可以利用printf函数从串口打印输出信息
入口参数：
出口参数：
作者：Mcuzone-freescale
******************************************************************************/
int fputc(int ch, FILE *f)
{
		UART0_SendOneByte((uint8_t)ch);
	
		while ((UART0->S1 & UART_S1_TC_MASK) == 0);       //等待数据发送完成
	
		return ch;
}



/******************************************************************************
函数名称：fgetc
函数功能：软件重定义fgetc函数,这样可以利用scanf函数从串口输入数据
入口参数：
出口参数：
作者：Mcuzone-freescale
******************************************************************************/
int fgetc(FILE *f)
{
		while ((UART0->S1 & UART_S1_RDRF_MASK) == 0);     //等待从串口接收一字节数据
	
		return ((int)(UART0_ReadOneByte()));               
}



