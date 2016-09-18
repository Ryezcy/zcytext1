#include "FTM.h"
#include "MK20D5.h"
u32 get_bus_clk_kzh(void);


void FTM_PWM_init(FTMn ftmn, CHn ch, u32 freq, u32 duty)
{
    u32 clk_hz = (PeriphBusClock) >> 1;       //bus频率/2
    u32 mod;
    u8 sc_ps;
    u32 cv;

//   ASSERT( (ftmn == FTM0) || ( (ftmn == FTM1 ) && (ch <= CH1))   ); //检查传递进来的通道是否正确  //FIXME 完善断言
//   ASSERT( freq <= (clk_hz >> 1) );              //用断言检测 频率 是否正常 ,频率必须小于时钟二分之一

    /*       计算频率设置        */
    mod = (clk_hz >> 16 ) / freq ;
    for(sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++);
    if(freq < 1000)sc_ps++;
    mod = (clk_hz >> sc_ps) / freq;
    cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;

    switch(ftmn)
    {
    case FTMn0:
        SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //使能FTM0时钟
        switch(ch)
        {
        case CH0:																											//FIXME 以后再动态分配通道引脚 ,现在默认ch0 PTC1
						{
								SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
							  PORTC->PCR[1] = PORT_PCR_MUX(4);  // PTC1
								//PORT_PCR_REG(PORTC_BASE_PTR, 1) = PORT_PCR_MUX(4);  // PTC1
						}
       
            break;
        case CH1:    
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[4] = PORT_PCR_MUX(3);  // PTA4  
							  // SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
                //PORTC->PCR[2] = PORT_PCR_MUX(4);  // PTC2
            }
            break;
        case CH2:
            {
								//SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								//PORTA->PCR[5] = PORT_PCR_MUX(3);  // PTA5  
								SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
                PORTC->PCR[3] = PORT_PCR_MUX(4);  // PTC3
            }         
            break;

        case CH3:          
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
                PORTC->PCR[4] = PORT_PCR_MUX(4);  // PTC4
            }         
            break;

        case CH4:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[4] = PORT_PCR_MUX(4); // PTD4
            }
            break;

        case CH5:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[5] = PORT_PCR_MUX(4);  // PTD5
            }
            break;

        case CH6:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[6] = PORT_PCR_MUX(4);  // PTD6
            }
            break;

        case CH7:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[7] = PORT_PCR_MUX(4);  // PTD7
            }
            break;
        default:
            return;
        }
        break;

    case FTMn1:
        SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;                           //使能FTM1时钟
        switch(ch)
        {
        case CH0:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[12] = PORT_PCR_MUX(3);  // PTA12
            }          
            break;
        case CH1:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[13] = PORT_PCR_MUX(3);  // PTA13          
            }
            break;
					

        default:
            return;
        }
        break;

    default:
        break;
    }
    /******************** 选择输出模式为 边沿对齐PWM *******************/
    //通道状态控制，根据模式来选择 边沿或电平
	  switch(ftmn)
    {
			case FTMn0:
				FTM0->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
				FTM0->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
			
			 /******************** 配置时钟和分频 ********************/
			  FTM0->MOD = mod;                                                    //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
				FTM0->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
			  FTM0->CNTIN =0;                                                      // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
			 
			  FTM0->CNTIN = 0;																										 //Counter Initial Value 计数器初始化值
			  FTM0->CONTROLS[ch].CnV = cv;
				FTM0->CNT =0;
				break;
			case FTMn1:
				FTM1->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
				FTM1->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
			
			
				 /******************** 配置时钟和分频 ********************/
				FTM1->MOD = mod;                                                    //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
				FTM1->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
			  FTM1->CNTIN =0;                                                      // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
			  FTM1->CNTIN = 0;																										 //Counter Initial Value 计数器初始化值
			  FTM1->CONTROLS[ch].CnV = cv;
				FTM1->CNT =0;
				break;
			default:
				break;
		}
}

void FTM_PWM_Duty(FTMn ftmn, CHn ch, u32 duty)
{
    u32 cv;
		u32 mod;
	
 //   ASSERT( (ftmn == FTM0) || ( (ftmn == FTM1 || ftmn == FTM2 ) && (ch <= CH1)) ); //检查传递进来的通道是否正确
 //   ASSERT(duty <= FTM_PRECISON);     //用断言检测 占空比是否合理

    //占空比 = (CnV-CNTIN)/(MOD-CNTIN+1)

		switch(ftmn)
		{
			case FTMn0:
					mod = FTM0->MOD;
					cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;
					// 配置FTM通道值
					FTM0->CONTROLS[ch].CnV = cv;
			
				
				break;
			case FTMn1:
					mod = FTM1->MOD;
					cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;
					// 配置FTM通道值
					FTM1->CONTROLS[ch].CnV = cv;
		
				break;
			default :
				break;
		}
}

void FTM_PWM_freq(FTMn ftmn, u32 freq)             //设置FTM的频率
{
    u32 clk_hz = PeriphBusClock >> 1;        //bus频率/2
    u32 mod;
    u8 sc_ps;
	
   // ASSERT( freq <= (clk_hz >> 1) );              //用断言检测 频率 是否正常 ,频率必须小于时钟二分之一

    /*       计算频率设置        */
    mod = (clk_hz >> 16 ) / freq ;
    for(sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++);
    mod = (clk_hz >> sc_ps) / freq;
	
	  switch(ftmn)
    {
			case FTMn0:
			 /******************** 配置时钟和分频 ********************/
			  FTM0->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
			  FTM0->CNTIN =0;                                                      // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
			  FTM0->MOD = mod;                                                    //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
			  FTM0->CNTIN = 0;																										 //Counter Initial Value 计数器初始化值
				FTM0->CNT =0;
				break;
			case FTMn1:
				 /******************** 配置时钟和分频 ********************/
				FTM1->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
			  FTM1->CNTIN =0;                                                      // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
			  FTM1->MOD = mod;                                                    //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
			  FTM1->CNTIN = 0;																										 //Counter Initial Value 计数器初始化值

				FTM1->CNT =0;
				break;
			default:
				break;
		}
}


#if 0


void FTM_Input_init(FTMn ftmn, CHn ch, Input_cfg cfg)
{
     switch(ftmn)
    {
    case FTMn0:
        SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //使能FTM0时钟
        switch(ch)
        {
        case CH0:																											//FIXME 以后再动态分配通道引脚 ,现在默认ch0 PTC1
						{
								SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
							   PORTC->PCR[1] = PORT_PCR_MUX(4);  // PTC1
								//PORT_PCR_REG(PORTC_BASE_PTR, 1) = PORT_PCR_MUX(4);  // PTC1
						}
       
            break;
        case CH1:    
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
                PORTC->PCR[2] = PORT_PCR_MUX(4);  // PTC2
            }
            break;
        case CH2:
            {
								SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[5] = PORT_PCR_MUX(3);  // PTA5  
							//SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
                //PORTC->PCR[3] = PORT_PCR_MUX(4);  // PTC3
            }         
            break;

        case CH3:          
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
               PORTC->PCR[4] = PORT_PCR_MUX(4);  // PTC4
            }         
            break;

        case CH4:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
               PORTD->PCR[4] = PORT_PCR_MUX(4); // PTD4
            }
            break;

        case CH5:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[5] = PORT_PCR_MUX(4);  // PTD5
            }
            break;

        case CH6:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
                PORTD->PCR[6] = PORT_PCR_MUX(4);  // PTD6
            }
            break;

        case CH7:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
               PORTD->PCR[7] = PORT_PCR_MUX(4);  // PTD7
            }
            break;
        default:
            return;
        }
        break;

    case FTMn1:
        SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;                           //使能FTM1时钟
        switch(ch)
        {
        case CH0:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[12] = PORT_PCR_MUX(3);  // PTA12
            }          
            break;
        case CH1:
            {
                SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
								PORTA->PCR[13] = PORT_PCR_MUX(3);  // PTA13          
            }
            break;
					

        default:
            return;
        }
        break;

    default:
        break;
    }


    /******************* 设置为输入捕捉功能 *******************/
    switch(cfg)
    {
    case Rising:    //上升沿触发
        FTM_CnSC_REG(FTMx[ftmn], 0) |=  ( FTM_CnSC_ELSA_MASK  | FTM_CnSC_CHIE_MASK );                   //置1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_ELSB_MASK  | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK); //清0
        break;

    case Falling:   //下降沿触发
        FTM_CnSC_REG(FTMx[ftmn], 0) |= (FTM_CnSC_ELSB_MASK  | FTM_CnSC_CHIE_MASK );                    //置1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_ELSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK); //清0
        break;

    case Rising_or_Falling: //上升沿、下降沿都触发
        FTM_CnSC_REG(FTMx[ftmn], 0) |=  ( FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK  | FTM_CnSC_CHIE_MASK ); //置1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_MSB_MASK  | FTM_CnSC_MSA_MASK); //清0
        break;
    }

    FTM_SC_REG(FTMx[ftmn]) = FTM_SC_CLKS(0x1);       //System clock

    FTM_MODE_REG(FTMx[ftmn]) |= FTM_MODE_WPDIS_MASK;
    FTM_COMBINE_REG(FTMx[ftmn]) = 0;
    FTM_MODE_REG(FTMx[ftmn]) &= ~FTM_MODE_FTMEN_MASK;
    FTM_CNTIN_REG(FTMx[ftmn]) = 0;

    FTM_STATUS_REG(FTMx[ftmn]) = 0x00;               //清中断标志位

    //开启输入捕捉中断
    enable_irq(78 - 16 + ftmn);
}

#endif
