#include "exti.h"

void  exti_init(PORTx portx ,u8 pin, u8 priority ,exti_cfg cfg)
{
	  switch(portx)
		{
			case  PORTxA:
					NVIC_ClearPendingIRQ(PORTA_IRQn);    
					NVIC_SetPriority(PORTA_IRQn, priority); 
					PORTA->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); 
					NVIC_EnableIRQ(PORTA_IRQn);       
				break;
			case PORTxB:
					NVIC_ClearPendingIRQ(PORTB_IRQn);    
					NVIC_SetPriority(PORTB_IRQn, priority); 
					PORTB->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); 
					NVIC_EnableIRQ(PORTB_IRQn);    
				break;
			case  PORTxC:
					NVIC_ClearPendingIRQ(PORTC_IRQn);    
					NVIC_SetPriority(PORTC_IRQn, priority); 
					PORTC->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); 
					NVIC_EnableIRQ(PORTC_IRQn);    
				break;
			case PORTxD:
					NVIC_ClearPendingIRQ(PORTD_IRQn);    
					NVIC_SetPriority(PORTD_IRQn, priority); 
					PORTD->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); 
					NVIC_EnableIRQ(PORTD_IRQn);    
				break;
			case  PORTxE:
					NVIC_ClearPendingIRQ(PORTE_IRQn);    
					NVIC_SetPriority(PORTE_IRQn, priority); 
					PORTE->PCR[pin] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); 
					NVIC_EnableIRQ(PORTE_IRQn);    
				break;
			default:
				break;
		}
}

void System_NVIC_Initial(void)
{	
		NVIC_ClearPendingIRQ(PIT0_IRQn);        //���PIT0�Ĺ���Ĵ���,����жϹ���λ 
	
		NVIC_SetPriority(PIT0_IRQn, 3);         //����PIT0�ж����ȼ�Ϊ3
	
		NVIC_EnableIRQ(PIT0_IRQn);              //ʹ��PIT0�Ķ�ʱ�ж�
		
	
		NVIC_ClearPendingIRQ(PIT1_IRQn);        //���PIT0�Ĺ���Ĵ���,����жϹ���λ 
	
		NVIC_SetPriority(PIT1_IRQn, 4);         //����PIT0�ж����ȼ�Ϊ4
	
		NVIC_EnableIRQ(PIT1_IRQn);              //ʹ��PIT0�Ķ�ʱ�ж�
		
	
		NVIC_ClearPendingIRQ(PIT2_IRQn);        //���PIT0�Ĺ���Ĵ���,����жϹ���λ 
	
		NVIC_SetPriority(PIT2_IRQn, 5);         //����PIT0�ж����ȼ�Ϊ5
		
		NVIC_EnableIRQ(PIT2_IRQn);              //ʹ��PIT0�Ķ�ʱ�ж�
		
	
		NVIC_ClearPendingIRQ(PIT3_IRQn);        //���PIT0�Ĺ���Ĵ���,����жϹ���λ 
	
		NVIC_SetPriority(PIT3_IRQn, 6);         //����PIT0�ж����ȼ�Ϊ6
		
		NVIC_EnableIRQ(PIT3_IRQn);              //ʹ��PIT0�Ķ�ʱ�ж�
}
