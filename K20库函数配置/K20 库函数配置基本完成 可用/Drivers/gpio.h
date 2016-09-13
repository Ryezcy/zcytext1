#ifndef  __GPIO_H__
#define  __GPIO_H__

#include <stdint.h>
#include "MK20D5.h"
#include "common.h"

/* GPIO???? */
#define HW_GPIOA  (0x00U) /*GPIO??A,????*/  
#define HW_GPIOB  (0x01U)
#define HW_GPIOC  (0x02U)
#define HW_GPIOD  (0x03U)
#define HW_GPIOE  (0x04U)
#define HW_GPIOF  (0x05U)

/* ?????????? */
typedef void (*GPIO_CallBackType)(uint32_t pinxArray);

typedef enum
{
		GPIO_Pin0 = 0,
		GPIO_Pin1,	
		GPIO_Pin2,	
		GPIO_Pin3,	
		GPIO_Pin4,
		GPIO_Pin5,
		GPIO_Pin6,
		GPIO_Pin7,
	
		GPIO_Pin8,
		GPIO_Pin9,	
		GPIO_Pin10,	
		GPIO_Pin11,	
		GPIO_Pin12,
		GPIO_Pin13,
		GPIO_Pin14,
		GPIO_Pin15,
	
		GPIO_Pin16,
		GPIO_Pin17,	
		GPIO_Pin18,	
		GPIO_Pin19,	
		GPIO_Pin20,
		GPIO_Pin21,
		GPIO_Pin22,
		GPIO_Pin23,
		
		GPIO_Pin24,
		GPIO_Pin25,	
		GPIO_Pin26,	
		GPIO_Pin27,	
		GPIO_Pin28,
		GPIO_Pin29,
		GPIO_Pin30,
		GPIO_Pin31
}GPIO_PinNumber;

typedef enum 
{
    PORTxA = 0,
    PORTxB,
    PORTxC,
    PORTxD,
    PORTxE
}PORTx;

/**
 * \enum GPIO_ITDMAConfig_Type
 * \brief ?????DMA????
 */
typedef enum
{
    kGPIO_DMA_RisingEdge,	      /**<?????DMA*/
    kGPIO_DMA_FallingEdge,        /**<?????DMA*/
    kGPIO_DMA_RisingFallingEdge,  /**<?????????DMA*/
    kGPIO_IT_Low,                 /**<???????*/
    kGPIO_IT_RisingEdge,          /**<???????*/
    kGPIO_IT_FallingEdge,         /**<???????*/
    kGPIO_IT_RisingFallingEdge,   /**<???????????*/
    kGPIO_IT_High,                /**<???????*/
}GPIO_ITDMAConfig_Type;


typedef enum
{
    kGPIO_Mode_IFT = 0x00,       /**< ???? */
    kGPIO_Mode_IPD = 0x01,       /**< ???? */
    kGPIO_Mode_IPU = 0x02,       /**< ???? */
    kGPIO_Mode_OOD = 0x03,       /**< ???? */
    kGPIO_Mode_OPP = 0x04,       /**< ???? */
}GPIO_Mode_Type;

typedef enum
{
    kPullDisabled,  /**<?????????*/
    kPullUp,        /**<????????*/
    kPullDown,      /**<????????*/
}PORT_Pull_Type;

typedef enum
{
    kInput,                  /**< ?????? */
    kOutput,                 /**< ?????? */
}GPIO_PinConfig_Type;

typedef enum
{
    kPinAlt0,  /**<0????*/
    kPinAlt1,  /**<1????*/
    kPinAlt2,  /**<2????*/
    kPinAlt3,  /**<3????*/
    kPinAlt4,  /**<4????*/
    kPinAlt5,  /**<5????*/
    kPinAlt6,  /**<6????*/
    kPinAlt7,  /**<7????*/
}PORT_PinMux_Type;

typedef struct
{
    uint8_t                instance;    ///<????HW_GPIOA~HW_GPIOF
    GPIO_Mode_Type         mode;        ///<????
    uint32_t               pinx;        ///<???0~31
}GPIO_InitTypeDef;

typedef enum 
{
    DISABLED = 0,        /**< ??? */
    ENABLED = !DISABLED,  /**< ?? */
}FunctionalState;

void GPIO_Configuration(PORT_Type* port ,GPIO_Type * gpio,GPIO_PinNumber pin, uint8_t dir );
void GPIO_OutPut_Configuration(void);
void GPIO_Input_Configuration(void);
void GPIO_Set(GPIO_Type *gpio, GPIO_PinNumber pin);
void GPIO_Reset(GPIO_Type *gpio, GPIO_PinNumber pin);
void GPIO_Toggle(GPIO_Type *gpio, GPIO_PinNumber pin);
uint8_t Read_InputDataBit(GPIO_Type *gpio, GPIO_PinNumber pin);
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode);
void GPIO_PinConfig(uint32_t instance, uint8_t pin, GPIO_PinConfig_Type mode);
void GPIO_WriteBit(uint32_t instance, uint8_t pin, uint8_t data);
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pin);
void PORT_PinPullConfig(uint32_t instance, uint8_t pin, PORT_Pull_Type pull);

void PORT_PinMuxConfig(uint32_t instance, uint8_t pin, PORT_PinMux_Type pinMux);
void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pin, bool status);
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun);
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pin, GPIO_ITDMAConfig_Type config, bool status);

#endif



