
#include "spi.h"
#include "gpio.h"
#include "common.h"


#if (!defined(SPI_BASES))

#ifdef      SPI1
#define     SPI_BASES {SPI0, SPI1}
#elif       SPI2
#define     SPI_BASES {SPI0, SPI1, SPI2}
#elif       SPI3
#define     SPI_BASES {SPI0, SPI1, SPI2, SPI3}
#else
#define     SPI_BASES {SPI0}
#endif

#endif

SPI_Type * const SPI_InstanceTable[] = SPI_BASES;
static SPI_CallBackType SPI_CallBackTable[ARRAY_SIZE(SPI_InstanceTable)] = {NULL};

static const Reg_t SIM_SPIClockGateTable[] =
{
#ifdef SIM_SCGC6_DSPI0_MASK
    {(void*)&(SIM->SCGC6), SIM_SCGC6_DSPI0_MASK},
#else
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI0_MASK},
#endif
    
#ifdef SPI1
#ifdef SIM_SCGC6_DSPI1_MASK
    {(void*)&(SIM->SCGC6), SIM_SCGC6_DSPI1_MASK},
#else
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI1_MASK},
#endif
#endif
    
#ifdef SPI2
#ifdef SIM_SCGC3_DSPI2_MASK
    {(void*)&(SIM->SCGC3), SIM_SCGC3_DSPI2_MASK},
#else
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SPI2_MASK},
#endif
#endif
};
static const IRQn_Type SPI_IRQnTable[] = 
{
    SPI0_IRQn,
#ifdef SPI1
    SPI1_IRQn,
#endif
#ifdef SPI2
    SPI2_IRQn,
#endif
};

static const uint32_t s_baudratePrescaler[] = { 2, 3, 5, 7 };
static const uint32_t s_baudrateScaler[] = { 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
                                           4096, 8192, 16384, 32768 };

static uint32_t dspi_hal_set_baud(uint32_t instance, uint8_t whichCtar, uint32_t bitsPerSec, uint32_t sourceClockInHz)                    
{
    uint32_t prescaler, bestPrescaler;
    uint32_t scaler, bestScaler;
    uint32_t dbr, bestDbr;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t baudrate = bitsPerSec;

    if(!(SPI_InstanceTable[instance]->MCR & SPI_MCR_MSTR_MASK))
    {
        return 0;
    }
    min_diff = 0xFFFFFFFFU;
    bestPrescaler = 0;
    bestScaler = 0;
    bestDbr = 1;
    bestBaudrate = 0; 

    for (prescaler = 0; (prescaler < 4) && min_diff; prescaler++)
    {
        for (scaler = 0; (scaler < 16) && min_diff; scaler++)
        {
            for (dbr = 1; (dbr < 3) && min_diff; dbr++)
            {
                realBaudrate = ((sourceClockInHz * dbr) /
                                (s_baudratePrescaler[prescaler] * (s_baudrateScaler[scaler])));

                if (baudrate >= realBaudrate)
                {
                    diff = baudrate-realBaudrate;
                    if (min_diff > diff)
                    {
                        min_diff = diff;
                        bestPrescaler = prescaler;
                        bestScaler = scaler;
                        bestBaudrate = realBaudrate;
                        bestDbr = dbr;
                    }
                }
            }
        }
    }

    uint32_t temp;
    temp = SPI_InstanceTable[instance]->CTAR[whichCtar];
    temp &= ~(SPI_CTAR_DBR_MASK| SPI_CTAR_PBR_MASK | SPI_CTAR_BR_MASK);
    if((bestDbr-1))
    {
        temp |= SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    else
    {
        temp |= SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    SPI_InstanceTable[instance]->CTAR[whichCtar] = temp;
    LIB_TRACE("bestBaudrate:%d\r\n", bestBaudrate);
    return bestBaudrate;
}

void SPI_Init(SPI_InitTypeDef * SPI_InitStruct)
{
    SPI_Type *SPIx;
    
    SPIx = SPI_InstanceTable[SPI_InitStruct->instance];
    
    *(uint32_t*)SIM_SPIClockGateTable[SPI_InitStruct->instance].addr |= SIM_SPIClockGateTable[SPI_InitStruct->instance].mask;
    
    SPIx->MCR |= SPI_MCR_HALT_MASK;
    
    switch(SPI_InitStruct->mode)
    {
        case kSPI_Master:
            SPIx->MCR |= SPI_MCR_MSTR_MASK;
            break;
        case kSPI_Slave:
            SPIx->MCR &= ~SPI_MCR_MSTR_MASK;
            break;
        default:
            break;
    }
    
    SPIx->MCR &= ~SPI_MCR_MDIS_MASK;
    
    SPIx->MCR |= 
        SPI_MCR_PCSIS_MASK |
        SPI_MCR_HALT_MASK  |
        SPI_MCR_CLR_TXF_MASK|
        SPI_MCR_CLR_RXF_MASK|
        SPI_MCR_DIS_TXF_MASK|
        SPI_MCR_DIS_RXF_MASK;
    
    SPI_CTARConfig(SPI_InitStruct->instance, SPI_InitStruct->ctar, SPI_InitStruct->frameFormat, SPI_InitStruct->dataSize, SPI_InitStruct->bitOrder, SPI_InitStruct->baudrate);
    
    SPIx->SR = 0xFFFFFFFF;
    
    SPIx->MCR &= ~SPI_MCR_HALT_MASK;
}

void SPI_CTARConfig(uint32_t instance, uint32_t ctar, SPI_FrameFormat_Type frameFormat, uint8_t dataSize, uint8_t bitOrder, uint32_t baudrate)
{
    SPI_Type *SPIx;
    uint32_t clock;
    
    SPIx = SPI_InstanceTable[instance];
    
    SPIx->CTAR[ctar] &= ~SPI_CTAR_FMSZ_MASK;
    SPIx->CTAR[ctar] |= SPI_CTAR_FMSZ(dataSize-1);
    
    switch(bitOrder)
    {
        case kSPI_MSB:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_LSBFE_MASK;
            break;
        case kSPI_LSB:
            SPIx->CTAR[ctar] |= SPI_CTAR_LSBFE_MASK;
            break;
        default:
            break;
    }
    
    switch(frameFormat)
    {
        case kSPI_CPOL0_CPHA0:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPHA_MASK;
            break;
        case kSPI_CPOL0_CPHA1:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] |= SPI_CTAR_CPHA_MASK;
            break;   
        case kSPI_CPOL1_CPHA0:
            SPIx->CTAR[ctar] |= SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPHA_MASK;
            break;  
        case kSPI_CPOL1_CPHA1:
            SPIx->CTAR[ctar] |= SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] |= SPI_CTAR_CPHA_MASK;  
            break;  
        default:
            break;
    }
    
    clock = PeriphBusClock;
    dspi_hal_set_baud(instance, ctar, baudrate, clock);
    
    SPIx->CTAR[ctar] |= SPI_CTAR_ASC(1)|SPI_CTAR_CSSCK(1)|SPI_CTAR_PASC(1)|SPI_CTAR_PCSSCK(1);  
}

/*uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    SPI_InitTypeDef SPI_InitStruct1;
    SPI_InitStruct1.baudrate = baudrate;
    SPI_InitStruct1.frameFormat = (SPI_FrameFormat_Type)frameFormat;
    SPI_InitStruct1.dataSize = 8;
    SPI_InitStruct1.instance = pq->ip;
    SPI_InitStruct1.mode = kSPI_Master;
    SPI_InitStruct1.bitOrder = kSPI_MSB;
    SPI_InitStruct1.ctar = HW_CTAR0;
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux); 
    }
    SPI_Init(&SPI_InitStruct1);
    return pq->ip;
}*/

void SPI_EnableTxFIFO(uint32_t instance, bool status)
{
    SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_MDIS_MASK;
    
    (status)?\
    (SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_DIS_TXF_MASK):\
    (SPI_InstanceTable[instance]->MCR |= SPI_MCR_DIS_TXF_MASK);
}

void SPI_EnableRxFIFO(uint32_t instance, bool status)
{
    SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_MDIS_MASK;
    
    (status)?\
    (SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_DIS_RXF_MASK):\
    (SPI_InstanceTable[instance]->MCR |= SPI_MCR_DIS_RXF_MASK);
}

void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config, bool status)
{
    SPI_Type* SPIx;
    
    SPIx = SPI_InstanceTable[instance];
    
    switch(config)
    {
        case kSPI_IT_TCF: 
            (status)?\
            (SPIx->RSER |= SPI_RSER_TCF_RE_MASK):\
            (SPIx->RSER &= ~SPI_RSER_TCF_RE_MASK);
            NVIC_EnableIRQ(SPI_IRQnTable[instance]);
            break;
        case kSPI_DMA_TFFF:
            (status)?\
            (SPIx->RSER |= SPI_RSER_TFFF_RE_MASK):\
            (SPIx->RSER &= ~SPI_RSER_TFFF_RE_MASK); 
            (status)?\
            (SPIx->RSER |= SPI_RSER_TFFF_DIRS_MASK):\
            (SPIx->RSER &= ~SPI_RSER_TFFF_DIRS_MASK); 
            break;
        case kSPI_DMA_RFDF:
            (status)?\
            (SPIx->RSER |= SPI_RSER_RFDF_RE_MASK):\
            (SPIx->RSER &= ~SPI_RSER_RFDF_RE_MASK); 
            (status)?\
            (SPIx->RSER |= SPI_RSER_RFDF_DIRS_MASK):\
            (SPIx->RSER &= ~SPI_RSER_RFDF_DIRS_MASK);   
            break;
        default:
            break;
    }
}

void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        SPI_CallBackTable[instance] = AppCBFun;
    }
}

uint16_t SPI_ReadWriteByte(uint32_t instance, uint32_t ctar, uint16_t data, uint16_t CSn, SPI_PCS_Type csState)
{
	SPI_InstanceTable[instance]->PUSHR = (((uint32_t)(((csState))<<SPI_PUSHR_CONT_SHIFT))&SPI_PUSHR_CONT_MASK) 
            | SPI_PUSHR_CTAS(ctar)      
            | SPI_PUSHR_PCS(1<<CSn)
            | SPI_PUSHR_TXDATA(data);
    
    if(!(SPI_InstanceTable[instance]->RSER & SPI_RSER_TCF_RE_MASK)) 
    {
        while(!(SPI_InstanceTable[instance]->SR & SPI_SR_TCF_MASK));
        SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK;
    }
    return (uint16_t)SPI_InstanceTable[instance]->POPR;
}

static void SPI_IRQHandler(uint32_t instance)
{
    SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK ;
    if(SPI_CallBackTable[instance])
    {
        SPI_CallBackTable[instance]();
    }
}

void SPI0_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI0);
}

void SPI1_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI1);
}

#if defined(SPI2)

void SPI2_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI2);
}
#endif


