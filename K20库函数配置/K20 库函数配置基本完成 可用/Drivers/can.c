 
#include "can.h"
#include "gpio.h"

#if defined(CAN0)

#define CAN_MB_MAX      (ARRAY_SIZE(CAN0->MB))

#if (!defined(CAN_BASES))
#ifdef CAN0
#define CAN_BASES   {CAN0}
#elif  CAN1
#define CAN_BASES   {CAN0, CAN1}
#endif

#endif

/* global vars */
CAN_Type * const CANBase[] = CAN_BASES;

static const Reg_t ClkTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN0_MASK},
#ifdef CAN1
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FLEXCAN1_MASK},
#endif
};

static const IRQn_Type CAN_IRQnTable[] = 
{
    CAN0_ORed_Message_buffer_IRQn,
#ifdef CAN1
    CAN1_ORed_Message_buffer_IRQn,
#endif
};

/* callback function  */
static CAN_CallBackType CAN_CallBackTable[ARRAY_SIZE(CANBase)] = {NULL};

#define CAN_GET_MB_CODE(cs)         (((cs) & CAN_CS_CODE_MASK)>>CAN_CS_CODE_SHIFT)
#define CAN_GET_FRAME_LEN(cs)       (((cs) & CAN_CS_DLC_MASK)>>CAN_CS_DLC_SHIFT)

static uint32_t CAN_SetBaudrate(CAN_Type *CANx, uint32_t baudrate)
{
    uint32_t ps ,p1, p2, pd;
    uint32_t sclock = GetClock(kBusClock)/baudrate;
    uint32_t time_seg1, time_seg2, total_tq;

    for(pd=0xFF; pd>0; pd--)
    {
        for(ps=1; ps<8; ps++)
        {
            for(p1=1; p1<8; p1++)
            {
                for(p2=1; p2<8; p2++)
                {
                    time_seg1 = ps+p1+2;
                    time_seg2 = p2+1;
                    total_tq = time_seg1+time_seg2+1;

                    if(ABS((int32_t)(total_tq*(pd+1) - sclock)) < 2)
                    {
                        if((time_seg1 < (time_seg2+8)) && ((time_seg2+2) > time_seg1))
                        {
                            CANx->CTRL1 &= ~(CAN_CTRL1_PROPSEG_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PRESDIV_MASK);
                            CANx->CTRL1 |=  CAN_CTRL1_PROPSEG(ps) 
                                            | CAN_CTRL1_RJW(2)
                                            | CAN_CTRL1_PSEG1(p1) 
                                            | CAN_CTRL1_PSEG2(p2)
                                            | CAN_CTRL1_PRESDIV(pd);
                            //printf("Get baudrate param! pd %d ps %d p1 %d p2 %d\r\n", pd, ps,p1,p2);
                            return 1;
                        }
                    }
                }
            }
        }
    }
	return 0;
}

static uint32_t set_id(uint32_t instance, uint32_t mb, uint32_t id)
{
    if(id > 0x7FF)
    {
        CANBase[instance]->MB[mb].ID = (id & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));  /* ID [28-0]*/
        CANBase[instance]->MB[mb].CS |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);  
    }
    else
    {
        CANBase[instance]->MB[mb].ID = CAN_ID_STD(id);  /* ID[28-18] */
        CANBase[instance]->MB[mb].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK); 
    }
    return 0;
}

void CAN_SetRxFilterMask(uint32_t instance, uint32_t mb, uint32_t mask)
{
    CANBase[instance]->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANBase[instance]->MCR))) {}; 
    if(mask > 0x7FF)
    {	 
        CANBase[instance]->RXIMR[mb] = CAN_ID_EXT(mask); 
    }
    else
    {
        CANBase[instance]->RXIMR[mb] = CAN_ID_STD(mask); 
    }
    
    CANBase[instance]->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANBase[instance]->MCR)));
}

void CAN_SetRxMB(uint32_t instance, uint32_t mb, uint32_t id)
{
    set_id(instance, mb, id);
    CANBase[instance]->MB[mb].CS &= ~CAN_CS_CODE_MASK; 
	CANBase[instance]->MB[mb].CS |= CAN_CS_CODE(kFlexCanRX_Empty);
}

void CAN_Init(CAN_InitTypeDef* Init)
{
    uint32_t i;
    CAN_Type *CANx;
    
    /* enable clock gate */
    IP_CLK_ENABLE(Init->instance);
  
    CANx = CANBase[Init->instance];
    
    /* set clock source is bus clock */
    CANx->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    /* enable module */
    CANx->MCR &= ~CAN_MCR_MDIS_MASK;
    
    /* software reset */
	CANx->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (CANx->MCR)) {}; 
        
    /* halt mode */
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
        
    /* init all mb */
    for(i = 0; i < CAN_MB_MAX; i++)
	{
		CANx->MB[i].CS = 0x00000000;
		CANx->MB[i].ID = 0x00000000;
		CANx->MB[i].WORD0 = 0x00000000;
		CANx->MB[i].WORD1 = 0x00000000;
        CANx->RXIMR[i] = 0x00000000; /* received all frame */
        CANx->IMASK1 = 0x00000000;
        CANx->IFLAG1 = 0xFFFFFFFF;
	}
	/* set all masks */
	//CANx->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
   // CANx->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
   // CANx->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK);
    /* use indviual mask, do not use RXMGMASK, RX14MASK and RX15MASK */
    CANx->MCR |= CAN_MCR_IRMQ_MASK;
    CANx->MCR &= ~CAN_MCR_IDAM_MASK;
    
    /* setting baudrate */
	CAN_SetBaudrate(CANx, Init->baudrate);
    
    /* bypass the frame sended by itself */
    CANx->MCR |= CAN_MCR_SRXDIS_MASK; 
    
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
}

uint32_t CAN_QuickInit(uint32_t CANxMAP, uint32_t baudrate)
{
	uint32_t i;
    map_t * pq = (map_t*)&(CANxMAP); 
    CAN_InitTypeDef CAN_InitSturct1;
    CAN_InitSturct1.instance = pq->ip;
    CAN_InitSturct1.baudrate = baudrate;
    CAN_Init(&CAN_InitSturct1);
    /* init pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux); 
    }
    return pq->ip;
}

static bool is_mb_idle(uint32_t instance, uint32_t mb)
{
    uint32_t code;
    code = CAN_GET_MB_CODE(CANBase[instance]->MB[mb].CS);
    if((code == kFlexCanTX_Inactive) || (code == kFlexCanRX_Inactive) || (code == kFlexCanRX_Empty))
    {
        return true;
    }
    return false;
}

uint32_t CAN_WriteData(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
    uint32_t i;
	uint32_t word[2] = {0};
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    
    while(is_mb_idle(instance, mb) == false);

    /* setting data */
	for(i = 0; i < len; i++)
	{
        if(i<4)
            word[0] |= (*(buf+i)<<((3-i)*8));
        else
            word[1] |= (*(buf+i)<<((7-i)*8));  
	}
    CANx->MB[mb].WORD0 = word[0];
    CANx->MB[mb].WORD1 = word[1];
    
    /* DLC field */
    CANx->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CANx->MB[mb].CS |= CAN_CS_DLC(len);
    
    /* clear RTR */
    CANx->MB[mb].CS &= ~CAN_CS_RTR_MASK;
    
    /* ID and IDE */
    set_id(instance, mb, id);

    CANx->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CANx->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Data);
    return 0;
}

uint32_t CAN_WriteRemote(uint32_t instance, uint32_t mb, uint32_t id, uint8_t len)
{
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    
    uint32_t ret;
    ret = is_mb_idle(instance, mb);
    if(ret)
    {
        return 2;
    }
    /* DLC field, remote frame still has DLC filed */
    CANx->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CANx->MB[mb].CS |= CAN_CS_DLC(len);
    
    CANx->MB[mb].CS |= CAN_CS_RTR_MASK;
    
    /* ID and IDE */
    set_id(instance, mb, id);
    
    CANx->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CANx->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Remote);
    return 0;
}

void CAN_CallbackInstall(uint32_t instance, CAN_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        CAN_CallBackTable[instance] = AppCBFun;
    }
}

void CAN_ITDMAConfig(uint32_t instance, uint32_t mb, CAN_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kCAN_IT_Tx_Disable:
            CANBase[instance]->IMASK1 &= ~(1 << mb);
            break;
        case kCAN_IT_Tx:
            CANBase[instance]->IMASK1 |= (1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        case kCAN_IT_Rx_Disable:
            CANBase[instance]->IMASK1 &= ~(1 << mb);
            break;
        case kCAN_IT_RX:
            CANBase[instance]->IMASK1 |= (1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        default:
            break;
    }
}

uint32_t CAN_ReadData(uint32_t instance, uint32_t mb, uint32_t *id, uint8_t *buf, uint8_t *len)
{
	uint32_t code, i;
	uint32_t word[2] = {0};
    code = CAN_GET_MB_CODE(CANBase[instance]->MB[mb].CS);
    if((code & 0x01))
    { 
        return 1; /* MB is busy and controlled by hardware */
    }
    
    if(CANBase[instance]->IFLAG1 & (1<<mb))
    {
        /* clear IT pending bit */
        CANBase[instance]->IFLAG1 = (1 << mb);
        /* read content */
        *len = CAN_GET_FRAME_LEN(CANBase[instance]->MB[mb].CS);
        word[0] = CANBase[instance]->MB[mb].WORD0;
        word[1] = CANBase[instance]->MB[mb].WORD1;
        for(i = 0; i < *len; i++)
        {  
            if(i < 4)
            (*(buf + i))=(word[0]>>((3-i)*8));
            else							
            (*(buf + i))=(word[1]>>((7-i)*8));
        }
        *id = (CANBase[instance]->MB[mb].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
        i = CANBase[instance]->TIMER; /* unlock MB */
        return 0;
    }
    i = CANBase[instance]->TIMER; /* unlock MB */
    return 1;
}

uint32_t CAN_ReadFIFO(uint32_t instance, uint32_t *id, uint8_t *buf, uint8_t *len)
{
	uint32_t i;
	uint32_t word[2] = {0};

    /* read content */
    *len = CAN_GET_FRAME_LEN(CANBase[instance]->MB[0].CS);
    word[0] = CANBase[instance]->MB[0].WORD0;
    word[1] = CANBase[instance]->MB[0].WORD1;
    for(i = 0; i < *len; i++)
    {
        if(i < 4)
        (*(buf + i))=(word[0]>>((3-i)*8));
        else							
        (*(buf + i))=(word[1]>>((7-i)*8));
    }
    *id = (CANBase[instance]->MB[0].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
    i = CANBase[instance]->TIMER; /* unlock MB */
    CANBase[instance]->IFLAG1 = (1 << CAN_RX_FIFO_MB);
    return 0;
}

bool CAN_IsRxFIFOEnable(uint32_t instance)
{
    return (CANBase[instance]->MCR & CAN_MCR_RFEN_MASK);
}

void CAN_SetRxFIFO(uint32_t instance)
{
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    /* halt mode */
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
        
    CANx->MCR |= CAN_MCR_RFEN_MASK;
    CANx->CTRL2 |= CAN_CTRL2_RFFN(0);
    
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
}

void CAN_IRQHandler(uint32_t instance)
{
    if(CAN_CallBackTable[instance])
    {
        CAN_CallBackTable[instance]();
    }
}

void CAN0_ORed_Message_buffer_IRQHandler(void) {CAN_IRQHandler(HW_CAN0);}
void CAN1_ORed_Message_buffer_IRQHandler(void) {CAN_IRQHandler(HW_CAN1);}


#endif
