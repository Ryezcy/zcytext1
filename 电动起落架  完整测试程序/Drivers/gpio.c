#include "gpio.h"
#include "systick.h"
#include "common.h"

#if (!defined(GPIO_BASES))

#ifdef PTF
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE, PTF}
#define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE, PORTF}
#else
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
#define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
#endif

#endif

static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};

static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};

static const IRQn_Type GPIO_IRQnTable[] = 
{
    PORTA_IRQn,
    PORTB_IRQn,
    PORTC_IRQn,
    PORTD_IRQn,
    PORTE_IRQn,
};


void GPIO_Configuration(PORT_Type* port ,GPIO_Type * gpio,GPIO_PinNumber pin, uint8_t dir )
{
		if(port == PORTA)
		{
			SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK ;         //��PORTA��ʱ��
		}
		else if(port == PORTB)
		{
			SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK ;         //��PORTB��ʱ��
		}
			else if(port == PORTC)
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK ;         //��PORTC��ʱ��
			}
				else if(port == PORTD)
				{
					SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK ;         //��PORTD��ʱ��
				}
					else if(port == PORTE)
					{
						SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK ;         //��PORTE��ʱ��
					}
		else
		{
			;
		}
	port->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	port->PCR[pin] |= PORT_PCR_MUX(1);           //���ó�GPIOģʽ
	
	if(dir){
		//���
		port->PCR[pin] |= PORT_PCR_DSE_MASK;         //ǿ���ʹ��
		port->PCR[pin] &= ~PORT_PCR_PE_MASK;         //�ڲ��������������ֹʹ��
		gpio->PDDR |= 1 << pin; 
		gpio->PSOR |= 1 << pin;
	}else {
		//����
		port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;   //ʹ���ڲ���������
		gpio->PDDR &= ~(1 << pin);                         //���ö˿ڷ���Ĵ���Ϊ���뷽��
	}
	
}

void GPIO_Reset(GPIO_Type *gpio, GPIO_PinNumber pin)
{
		gpio->PSOR |= (1 << pin);
}

void GPIO_Set(GPIO_Type *gpio, GPIO_PinNumber pin)
{
		gpio->PCOR |= (1 << pin);
}

void GPIO_Toggle(GPIO_Type *gpio, GPIO_PinNumber pin)
{
		gpio->PTOR |= (1 << pin);
}


uint8_t Read_InputDataBit(GPIO_Type *gpio, GPIO_PinNumber pin)
{
		if (gpio == PTD)
		{
				if (((PTD->PDIR) >> pin) & 0x01)         //�ж��Ƿ��а�������
				{
						return 0;                            //û��,�򷵻�0
				}
				else
				{
						return 1;                            //�а��������򷵻�1
				}
		}
		else 
		{
				if (((PTE->PDIR) >> pin) & 0x01)
				{
						return 0;
				}
				else
				{
						return 1;
				}
		}
}

void GPIO_PinConfig(uint32_t instance, uint8_t pin, GPIO_PinConfig_Type mode)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (mode == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pin)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pin));
}

void GPIO_WriteBit(uint32_t instance, uint8_t pin, uint8_t data)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pin)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pin));
}

uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pin)
{
    /* input or output */
    if(((GPIO_InstanceTable[instance]->PDDR) >> pin) & 0x01)
    {
        return ((GPIO_InstanceTable[instance]->PDOR >> pin) & 0x01);
    }
    else
    {
        return ((GPIO_InstanceTable[instance]->PDIR >> pin) & 0x01);
    }
}

void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    /* config state */
    switch(GPIO_InitStruct->mode)
    {
        case kGPIO_Mode_IFT:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLED);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLED);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLED);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, ENABLED);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLED);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    /* config pinMux */
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pin, bool status)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (status) ? (PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_ODE_MASK);
}

void PORT_PinMuxConfig(uint32_t instance, uint8_t pin, PORT_PinMux_Type pinMux)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pin] &= ~(PORT_PCR_MUX_MASK);
    PORT_InstanceTable[instance]->PCR[pin] |=  PORT_PCR_MUX(pinMux);
}

uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
}

void PORT_PinPullConfig(uint32_t instance, uint8_t pin, PORT_Pull_Type pull)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    switch(pull)
    {
        case kPullDisabled:
            PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_PE_MASK;
            break;
        case kPullUp:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PS_MASK;
            break;
        case kPullDown:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
    }
}

void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun)
{
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pin, GPIO_ITDMAConfig_Type config, bool status)
{
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(GPIO_IRQnTable[instance]);
        return;
    }
    
    switch(config)
    {
        case kGPIO_DMA_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_High:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        default:
            break;
    }
}
