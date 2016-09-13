 
#include "common.h"
#include "i2c.h"
#include "gpio.h"
#include "systick.h"

#define SDA_DDR_OUT()       do {GPIO_PinConfig(i2c.instace, i2c.sda_pin, kOutput);}while(0)
#define SDA_DDR_IN()        do {GPIO_PinConfig(i2c.instace, i2c.sda_pin, kInput);}while(0)
#define SDA_H()             do {GPIO_WriteBit(i2c.instace, i2c.sda_pin, 1);}while(0)
#define SDA_L()             do {GPIO_WriteBit(i2c.instace, i2c.sda_pin, 0);}while(0)
#define SCL_H()             do {GPIO_WriteBit(i2c.instace, i2c.scl_pin, 1);}while(0)
#define SCL_L()             do {GPIO_WriteBit(i2c.instace, i2c.scl_pin, 0);}while(0)
#define I2C_DELAY()         DelayUs(1)



static i2c_gpio i2c;


uint8_t I2C_QuickInit(uint32_t MAP, uint32_t baudrate)
{
    uint8_t i;
    map_t * pq = (map_t*)&(MAP);
    
    for(i = 0; i < pq->pin_cnt; i++)
    {
        GPIO_QuickInit(pq->io, pq->pin_start + i, kGPIO_Mode_OPP);
        GPIO_QuickInit(pq->io, pq->pin_start + i, kGPIO_Mode_OPP);
        GPIO_WriteBit(pq->io, pq->pin_start + i, 1);
        PORT_PinPullConfig(pq->io, pq->pin_start + i, kPullUp);
    }

    i2c.instace = pq->io;
    
    switch(MAP)
    {
        case I2C1_SCL_PE01_SDA_PE00:
            i2c.scl_pin = 1;i2c.sda_pin = 0;
            break;
        case I2C0_SCL_PE19_SDA_PE18:
            i2c.scl_pin = 19;i2c.sda_pin = 18;
            break;
        case I2C0_SCL_PF22_SDA_PF23:
            i2c.scl_pin = 22;i2c.sda_pin = 23;
            break;
        case I2C0_SCL_PB00_SDA_PB01:
            i2c.scl_pin = 0;i2c.sda_pin = 1;
            break;
        case I2C0_SCL_PB02_SDA_PB03:
            i2c.scl_pin = 2;i2c.sda_pin = 3;
            break;
        case I2C1_SCL_PC10_SDA_PC11:
            i2c.scl_pin = 10;i2c.sda_pin = 11;
            break;
        case I2C0_SCL_PD08_SDA_PD09:
            i2c.scl_pin = 8;i2c.sda_pin = 9;
            break;
        case I2C0_SCL_PE24_SDA_PE25:
            i2c.scl_pin = 24;i2c.sda_pin = 25;
            break;
        case I2C1_SCL_PC01_SDA_PC02:
            i2c.scl_pin = 1;i2c.sda_pin = 2;
            break;
        case I2Cx_SCL_PC14_SDA_PC15:
            i2c.scl_pin = 14;i2c.sda_pin = 15;
            break;
        default:
            break;
    }
    return pq->ip;
}


void I2C_Init(I2C_InitTypeDef* I2C_InitStruct)
{
    
}

static inline uint8_t SDA_IN(void)
{
    return GPIO_ReadBit(i2c.instace, i2c.sda_pin);
}


static bool I2C_Start(void)
{
    SDA_DDR_OUT();
    SDA_H();
    SCL_H();
    I2C_DELAY();
    SDA_L();
    I2C_DELAY();
    SCL_L();
    return true;
}


static void I2C_Stop(void)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    SDA_H();
    I2C_DELAY();
}

static void I2C_Ack(void)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static void I2C_NAck(void)
{
    SCL_L();
    I2C_DELAY();
    SDA_H();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static bool I2C_WaitAck(void)
{
    uint8_t ack;
    SDA_DDR_IN();
    SCL_L();
    
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    ack = SDA_IN();
    SCL_L();
    SDA_DDR_OUT();
    
    return ack;
}

static void I2C_SendByte(uint8_t data)
{
    volatile uint8_t i;
    
    i = 8;
    while(i--)
    {
        if(data & 0x80) SDA_H();
        else SDA_L();
        data <<= 1;
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        SCL_L();
    }

}

static uint8_t I2C_GetByte(void)
{
    uint8_t i,byte;
    
    i = 8;
    byte = 0;

    SDA_DDR_IN();
    while(i--)
    {
        SCL_L();
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        byte = (byte<<1)|(SDA_IN() & 1);
    }
    SCL_L();
    SDA_DDR_OUT();
    return byte;
}

int I2C_BurstWrite(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&addr;
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err += I2C_WaitAck();

    while(addrLen--)
    {
        I2C_SendByte(*p++);
        err += I2C_WaitAck();
    }
    
    while(len--)
    {
        I2C_SendByte(*buf++);
        err += I2C_WaitAck();  
    }

    I2C_Stop();
    return err;
}

int I2C_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data)
{
    return I2C_BurstWrite(instance, chipAddr, addr, 1, &data, 1);
}

int I2C_BurstRead(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&addr;
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err += I2C_WaitAck();
    
    while(addrLen--)
    {
        I2C_SendByte(*p++);
        err += I2C_WaitAck();
    }
    
    I2C_Start();
    I2C_SendByte(chipAddr+1);
    err += I2C_WaitAck();
    
    while(len--)
    {
        *buf++ = I2C_GetByte();
        if(len)
        {
            I2C_Ack();
        }
    }
    
    I2C_NAck();
    I2C_Stop();
    
    return err;
}

int I2C_Probe(uint32_t instance, uint8_t chipAddr)
{
    uint8_t err;
    
    err = 0;
    chipAddr <<= 1;
    
    I2C_Start();
    I2C_SendByte(chipAddr);
    err = I2C_WaitAck();
    I2C_Stop();
    return err;
}

int I2C_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t *data)
{
    return I2C_BurstRead(instance, chipAddr, addr, 1, data, 1);
}

int SCCB_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* data)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    chipAddr <<= 1;
    
    while(retry--)
    {
        err = 0;
        I2C_Start();
        I2C_SendByte(chipAddr);
        err += I2C_WaitAck();
        
        I2C_SendByte(addr);
        err += I2C_WaitAck();
        
        I2C_Stop();
        I2C_Start();
        I2C_SendByte(chipAddr+1);
        err += I2C_WaitAck();
        
        *data = I2C_GetByte();
       // err += I2C_WaitAck();
        
        I2C_NAck();
        I2C_Stop();
        if(!err)
        {
            break;
        }
    }

    return err;
}

int SCCB_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    
    while(retry--)
    {
        err = I2C_WriteSingleRegister(instance, chipAddr, addr, data);
        if(!err)
        {
            break;
        }
    }
    return err;
}

void I2C_Scan(uint32_t MAP)
{
    uint8_t i;
    uint8_t ret;
    uint32_t instance;
    instance = I2C_QuickInit(MAP, 100*1000);
    for(i = 1; i < 127; i++)
    {
        ret = I2C_Probe(instance , i);
        if(!ret)
        {
            LIB_TRACE("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }
}
