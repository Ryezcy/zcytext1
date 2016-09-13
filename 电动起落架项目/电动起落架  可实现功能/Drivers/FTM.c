#include "FTM.h"
#include "MK20D5.h"
u32 get_bus_clk_kzh(void);


void FTM_PWM_init(FTMn ftmn, CHn ch, u32 freq, u32 duty)
{
    u32 clk_hz = (PeriphBusClock) >> 1;       //busƵ��/2
    u32 mod;
    u8 sc_ps;
    u32 cv;

//   ASSERT( (ftmn == FTM0) || ( (ftmn == FTM1 ) && (ch <= CH1))   ); //��鴫�ݽ�����ͨ���Ƿ���ȷ  //FIXME ���ƶ���
//   ASSERT( freq <= (clk_hz >> 1) );              //�ö��Լ�� Ƶ�� �Ƿ����� ,Ƶ�ʱ���С��ʱ�Ӷ���֮һ

    /*       ����Ƶ������        */
    mod = (clk_hz >> 16 ) / freq ;
    for(sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++);
    if(freq < 1000)sc_ps++;
    mod = (clk_hz >> sc_ps) / freq;
    cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;

    switch(ftmn)
    {
    case FTMn0:
        SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //ʹ��FTM0ʱ��
        switch(ch)
        {
        case CH0:																											//FIXME �Ժ��ٶ�̬����ͨ������ ,����Ĭ��ch0 PTC1
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
        SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;                           //ʹ��FTM1ʱ��
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
    /******************** ѡ�����ģʽΪ ���ض���PWM *******************/
    //ͨ��״̬���ƣ�����ģʽ��ѡ�� ���ػ��ƽ
	  switch(ftmn)
    {
			case FTMn0:
				FTM0->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
				FTM0->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
			
			 /******************** ����ʱ�Ӻͷ�Ƶ ********************/
			  FTM0->MOD = mod;                                                    //Modulo valueģ��, EPWM������Ϊ ��MOD - CNTIN + 0x0001
				FTM0->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //û��ʹ������ж�
			  FTM0->CNTIN =0;                                                      // Channel (n) Value  �����������ȣ�(CnV - CNTIN).
			 
			  FTM0->CNTIN = 0;																										 //Counter Initial Value ��������ʼ��ֵ
			  FTM0->CONTROLS[ch].CnV = cv;
				FTM0->CNT =0;
				break;
			case FTMn1:
				FTM1->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
				FTM1->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
			
			
				 /******************** ����ʱ�Ӻͷ�Ƶ ********************/
				FTM1->MOD = mod;                                                    //Modulo valueģ��, EPWM������Ϊ ��MOD - CNTIN + 0x0001
				FTM1->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //û��ʹ������ж�
			  FTM1->CNTIN =0;                                                      // Channel (n) Value  �����������ȣ�(CnV - CNTIN).
			  FTM1->CNTIN = 0;																										 //Counter Initial Value ��������ʼ��ֵ
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
	
 //   ASSERT( (ftmn == FTM0) || ( (ftmn == FTM1 || ftmn == FTM2 ) && (ch <= CH1)) ); //��鴫�ݽ�����ͨ���Ƿ���ȷ
 //   ASSERT(duty <= FTM_PRECISON);     //�ö��Լ�� ռ�ձ��Ƿ����

    //ռ�ձ� = (CnV-CNTIN)/(MOD-CNTIN+1)

		switch(ftmn)
		{
			case FTMn0:
					mod = FTM0->MOD;
					cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;
					// ����FTMͨ��ֵ
					FTM0->CONTROLS[ch].CnV = cv;
			
				
				break;
			case FTMn1:
					mod = FTM1->MOD;
					cv = (duty * (mod - 0 + 1)) / FTM_PRECISON;
					// ����FTMͨ��ֵ
					FTM1->CONTROLS[ch].CnV = cv;
		
				break;
			default :
				break;
		}
}

void FTM_PWM_freq(FTMn ftmn, u32 freq)             //����FTM��Ƶ��
{
    u32 clk_hz = PeriphBusClock >> 1;        //busƵ��/2
    u32 mod;
    u8 sc_ps;
	
   // ASSERT( freq <= (clk_hz >> 1) );              //�ö��Լ�� Ƶ�� �Ƿ����� ,Ƶ�ʱ���С��ʱ�Ӷ���֮һ

    /*       ����Ƶ������        */
    mod = (clk_hz >> 16 ) / freq ;
    for(sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++);
    mod = (clk_hz >> sc_ps) / freq;
	
	  switch(ftmn)
    {
			case FTMn0:
			 /******************** ����ʱ�Ӻͷ�Ƶ ********************/
			  FTM0->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //û��ʹ������ж�
			  FTM0->CNTIN =0;                                                      // Channel (n) Value  �����������ȣ�(CnV - CNTIN).
			  FTM0->MOD = mod;                                                    //Modulo valueģ��, EPWM������Ϊ ��MOD - CNTIN + 0x0001
			  FTM0->CNTIN = 0;																										 //Counter Initial Value ��������ʼ��ֵ
				FTM0->CNT =0;
				break;
			case FTMn1:
				 /******************** ����ʱ�Ӻͷ�Ƶ ********************/
				FTM1->SC  = FTM_SC_CPWMS_MASK | FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //û��ʹ������ж�
			  FTM1->CNTIN =0;                                                      // Channel (n) Value  �����������ȣ�(CnV - CNTIN).
			  FTM1->MOD = mod;                                                    //Modulo valueģ��, EPWM������Ϊ ��MOD - CNTIN + 0x0001
			  FTM1->CNTIN = 0;																										 //Counter Initial Value ��������ʼ��ֵ

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
        SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //ʹ��FTM0ʱ��
        switch(ch)
        {
        case CH0:																											//FIXME �Ժ��ٶ�̬����ͨ������ ,����Ĭ��ch0 PTC1
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
        SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;                           //ʹ��FTM1ʱ��
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


    /******************* ����Ϊ���벶׽���� *******************/
    switch(cfg)
    {
    case Rising:    //�����ش���
        FTM_CnSC_REG(FTMx[ftmn], 0) |=  ( FTM_CnSC_ELSA_MASK  | FTM_CnSC_CHIE_MASK );                   //��1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_ELSB_MASK  | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK); //��0
        break;

    case Falling:   //�½��ش���
        FTM_CnSC_REG(FTMx[ftmn], 0) |= (FTM_CnSC_ELSB_MASK  | FTM_CnSC_CHIE_MASK );                    //��1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_ELSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK); //��0
        break;

    case Rising_or_Falling: //�����ء��½��ض�����
        FTM_CnSC_REG(FTMx[ftmn], 0) |=  ( FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK  | FTM_CnSC_CHIE_MASK ); //��1
        FTM_CnSC_REG(FTMx[ftmn], 0) &= ~( FTM_CnSC_MSB_MASK  | FTM_CnSC_MSA_MASK); //��0
        break;
    }

    FTM_SC_REG(FTMx[ftmn]) = FTM_SC_CLKS(0x1);       //System clock

    FTM_MODE_REG(FTMx[ftmn]) |= FTM_MODE_WPDIS_MASK;
    FTM_COMBINE_REG(FTMx[ftmn]) = 0;
    FTM_MODE_REG(FTMx[ftmn]) &= ~FTM_MODE_FTMEN_MASK;
    FTM_CNTIN_REG(FTMx[ftmn]) = 0;

    FTM_STATUS_REG(FTMx[ftmn]) = 0x00;               //���жϱ�־λ

    //�������벶׽�ж�
    enable_irq(78 - 16 + ftmn);
}

#endif
