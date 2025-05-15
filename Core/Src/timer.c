#include "timer.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "adc.h"
#include "rtc_int.h"
#include "ctrl.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"
#define MAJOR_VERSION 	1
#define MID_VERSION 	0
#define MINOR_VERSION 	0
//����С����λ����
//�Ƕȵĵ�λ��1�ȣ�����360ת��Բ�ܱ�����Q30�޷���С����
//ƽ���������211��Ȩ�õ�4����ֵ��������λ��
//Բ�ܱ���*ƽ������õ�64λ�����պõ�32��ȥ��

//��1��Ϊ��λ�ĽǶ�ת��Բ�ܱ�����Q30�޷���С����

#define MAX_STEP 	65536	//�趨��һ���Ƚ���ֵʱ�ĵ���ֵ����<=65536������65535�ǰ�ȫֵ�������ж�ʱCNT�϶�����CCRֵ�ˣ�����65536Ҳû���⣬�ȽϺü���
#define CalcAvgPulseWidth() AvgPulseWidth = PulseWidth[0]*2 + PulseWidth[1] + PulseWidth[2] //��Ȩ����ƽ��������


#define MIN_PULSE_FREQ 	1	//�ⴥ��������СƵ��
unsigned int AvgPulseWidth;	//ƽ��������
volatile unsigned int AvgHz, AvgHz_Out;	//ƽ��ת�٣�0.01Hz
int ExternalSpeed=0;
//unsigned int PulseWidth[3];	//�������������
unsigned int PulseWidth[7];
unsigned int CurrPulsePos, LastPulsePos;	//ǰ�����������λ�ã����ڼ���������
//�������Ƕȣ���λ������
unsigned int TrigAngle0=30;
//��һ��ת��Q30С��
unsigned int TrigAngle=AngleCov(30);
int PulseCnt=0;//�������������>4���ܼ����ƽ��ֵ
unsigned int WaitStep;	//�����ع���ʱʱ���Ƚ�������ֵ
unsigned int TrigClks;	//��ʱ��ʱ����
volatile unsigned int NextTrigClks;
u32 TrigLimitWidth;
u32 TrigDiv;
u32 TrigCnt;
//�ڴ���Ƶ��
volatile u32 real_int_freq;

int CapEnable=1;				
int OverflowCnt = 0;	//������������ʾת��̫����ֹͣ

//�ܳ��곤ģʽ�ڴ�������������־
volatile u8 Trig_Perimeter_En = 0;
volatile u8 ExtTrigOnce = 0;

volatile u8 ExtTrigOn = 0; //�ⲿ������Ϊ1��ֹͣ��Ϊ0

//����ģʽ�ı���
volatile u16 GearCnt = 0;
u16 GearTrigInt;
u16 GearTirgFrag;
unsigned int TrigAngleGear=AngleCovGear(1);

//������ģʽ�ı���
volatile u16 aCnt = 0;
volatile u16 zCnt = 0;
volatile u16 EncoderLine = 1024; //������������
u16 EncoderLine_old = 1024;

volatile int tim1_width ; //
volatile int flash_pulse; //

TrigState_t TrigState = Idle;

//������ʱ���ȵĵ�λ��100ns
#define FLASH_DELAY_DEFAULT	1
#define FLASH_WIDTH_DEFAULT	200		//�����strobe������
#define CCR2_WIDTH 2
unsigned short FlashDelay = FLASH_DELAY_DEFAULT;
unsigned short FlashWidth = FLASH_WIDTH_DEFAULT;


#define CLOCK_US TIM1_Hz2clk(100000000) 



unsigned int LastTrigPos=0;
unsigned int NextTrigPos;





#define LIM_Hz1		100
#define LIM_Hz2		2000

u32 delta_gear ;
u16 GearTrigInt2;
u16 GearTirgFrag2;







//new 
volatile u16  image_num = 1 ; //ÿ��N������
volatile u32 image_period_per ; //ÿ���������


uint8_t encoder_err_cnt =  0 ;
uint8_t  encoder_trig_en = 1;
uint32_t encoder_num_per  ;


//new encoder
volatile u32 t2_up_cnt = 0;
volatile u32 t2_cnt = 0;
volatile uint8_t period_st_flag= 0;
u32 last_cnt_encoder ;
u32 cur_cnt_encoder ;
u32 del_cnt_encoder ;
volatile u8 need_cal_encoder  = 0;
//volatile u32 image_num  ; //����ĸ�����������

volatile u8 NeedRfReInit = 0 ;



//�ⴥ��ģʽ��������־
volatile u8 Trig_Ext_En = 0;
volatile u8 Trig_Ext_On = 0;

volatile u8 Trig_Ext_En_Wifi = 0;
volatile u8 Trig_Ext_On_Wifi = 0;

#define	MAX_STROB_POWER	1000000 
//MOS�ܴ��и���ʱ����ʱ�̶�һ��7us��ƫ����
#define PULSE_WIDTH_OFFSET  1
volatile u32	max_strobe_power = MAX_STROB_POWER ;



void TIM_Init(void);
void TIM_Reset(TIM_TypeDef * TIM);
unsigned int GetPulsePos(void);
void SetFlash(void);
void NVIC_Configuration(void);
void StartInternalTrig(void);
void gpio_test(void) ;




void TIM_ResetAll(void)
{
	TIM_Reset(TIM1);
	TIM_Reset(TIM2);
	TIM_Reset(TIM3);
	TIM_Reset(TIM4);
}




#if 0
void TIM_Init_Ori(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	
	TIM_ResetAll();
	
	//��TIM������ʼ��
	TIM_TimeBaseStructure.TIM_Prescaler=0;             //����Ƶ
	TIM_TimeBaseStructure.TIM_Period=0xFFFF;                 //��ʱȫF��SetFlash�����þ���ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	//TIM1��һ��������ģʽ
	TIM1->CR1 |= TIM_OPMode_Single;				

	//TIM2_CH2��������
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
	TIM_ICInit(TIM2,&TIM_ICInitStructure);
	//TIM3_CH1��������
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM3,&TIM_ICInitStructure);	

	/*
	if(IsTrigMode(Trig_Encoder)) {
		//TIM3_CH2��������
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
		TIM_ICInit(TIM3,&TIM_ICInitStructure);	
		//TIM3_CH2���ж�ʹ��
		TIM3->SR &= ~TIM_IT_CC2; 
		TIM3->DIER |= TIM_IT_CC2;	
	}
	*/
	
	//TIM2_CH1�������->TRGO
	TIM_OCInitStructure.TIM_OCMode= TIM_OCMode_PWM2;		//PWMģʽ2
	TIM_OCInitStructure.TIM_Pulse=0xFFFF; 					//
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;	 //ʹ������Ƚ�
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//�������
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);			
	
	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref);
	//TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);

	//TIM2_CH3���ع����
	TIM_OCInitStructure.TIM_OCMode = TIM_ForcedAction_InActive; //��Force Inactive����Ҫʱ�����Active
	TIM_OCInitStructure.TIM_Pulse=0xFF; 					//���ڿ��������
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;	 //���
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//�������
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	//TIM2->TIM3 Gated
	TIM3->SMCR = TIM_SlaveMode_Gated //TIM_SlaveMode_External1
				| TIM_TS_ITR1;//TIM2->TIM3
	//TIM3_CH1��������
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM3,&TIM_ICInitStructure);

	//TIM2->TIM1 Triger
	TIM1->SMCR = TIM_SlaveMode_Trigger
				| TIM_TS_TI1FP1;// TI1 Edge Detector 

	//TIM1_CH2N���
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;	
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;	 //ʹ������Ƚ�
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//�������
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState =TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState =TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_Pulse = CCR2_WIDTH;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	
	TIM1->BDTR = 0x8000;//Main output enable

//	if(!IsTrigInt()) { //�ⴥ�����͵��豸����
//		if(!IsTrigMode(Trig_Internal)){
//			TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_CC2,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
//		}
//	}
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

//	TIM_ARRPreloadConfig(TIM3,ENABLE);					
	TIM_Cmd(TIM2,ENABLE);								//ʹ��
	TIM_Cmd(TIM3,ENABLE);								//ʹ��

	TrigState = Idle;
	TrigCnt = 0;
	PulseCnt = 0;
}

void TIM_Init(void)
{
	TIM_Init_Ori();

}
#endif

__inline void ActiveTrigPoint(void)
{
	//�趨��TIM2_CH3��CNT=CCR3ʱ�����ߣ�Ȼ�����ж������͡�
	// TIM2->CCMR2 = (TIM_OCMode_Timing | TIM_OCPreload_Disable)<<8 //ͨ��4��ģʽ
	// 			| (TIM_OCMode_Active | TIM_OCPreload_Disable)<<0; //ͨ��3��ģʽ	
}

__inline void ActiveCCR3Int(void)
{
	// TIM2->SR = (uint16_t)~TIM_IT_CC3; //����жϱ�־ 
	// TIM2->DIER |= TIM_IT_CC3;	
}
void DeactiveTrigPoint(void)
{
	//�趨��TIM2_CH3��CNT=CCR3ʱ�����ߣ�Ȼ�����ж������͡�
	// TIM2->CCMR2 = (TIM_OCMode_Timing | TIM_OCPreload_Disable)<<8 //ͨ��4��ģʽ
	// 			| (TIM_ForcedAction_InActive | TIM_OCPreload_Disable)<<0; //ͨ��3��ģʽ	
	// TIM2->DIER &= (uint16_t)~TIM_IT_CC3;	
}

//���㴥��������������Ե��ʱ����
__inline void CalcTrigPoint(void)
{
	
	//���ڵ����壬���Ƶ���������ⲿƵ�ʵļ���
	//���ڱ곤�ܳ������Ƶ���������ⲿƵ�ʵļ���* �곤/�ܳ���ֵ

	//*4 ����ΪAvgPulseWidthû�г�4
	TrigDiv = 4*TrigLimitWidth/AvgPulseWidth + 1;
	TrigClks = ((unsigned long long)AvgPulseWidth*TrigAngle)>> 32;//�����ǶȻ����ʱ����ʱ
}

#define  TIM1_100ns2clk(ns) ((ns)*(TIMxCLK/1000000)/10)
#define  TIM1_us2clk(us) 	((us)*(TIMxCLK/1000000))
#define  Hz2Us(hz)	(1000*1000*100 + hz/2)/hz





//Ƶ�ʹ���ʱǿ�Ʒ�Ƶ
__inline int VerifyInterval(void)
{
	TrigCnt++;
	if(TrigCnt >= TrigDiv){
		TrigCnt = 0;
		return 1;
	}else
		return 0; //���Դ���
}

//�趨������
__inline void SetTrigPoint(void)
{
	//���TrigClks<=65535,����ֱ�����ع����λ�ã�����<=65536�Ĳ����жϣ�ֱ�����һ�������ع����λ��
	//WaitStep = TrigClks/(TrigClks/MAX_STEP+1);	
	WaitStep = TrigClks/((TrigClks+MAX_STEP-1)/MAX_STEP)+1;
	if(TrigClks<=WaitStep){
		//����������ж�
		TIM2->CCR3 = TIM2->CCR2 + (uint16_t)TrigClks;
		ActiveTrigPoint();
		ActiveCCR3Int();
		TrigState = TrigPrepared;
	}else{
		//�����ж�λ���´����ж�
		TIM2->CCR3 = TIM2->CCR2 + (uint16_t)WaitStep;
		TrigClks -= WaitStep;
		ActiveCCR3Int();
		TrigState = TrigWait;
	}
}

__inline void SetTrigPointInternal(void)
{
	//���TrigClks<=65535,����ֱ�����ع����λ�ã�����<=65536�Ĳ����жϣ�ֱ�����һ�������ع����λ��
	//WaitStep = TrigClks/(TrigClks/MAX_STEP+1);	
	WaitStep = TrigClks/((TrigClks+MAX_STEP-1)/MAX_STEP)+1;
	if(TrigClks<=WaitStep){
		//����������ж�
		TIM2->CCR3 = TIM2->CCR3 + (uint16_t)TrigClks;
		ActiveTrigPoint();
		TrigState = TrigPrepared;
		ActiveCCR3Int();
		
	}else{
		//�����ж�λ���´����ж�
		TIM2->CCR3 = TIM2->CCR3 + (uint16_t)WaitStep;
		TrigClks -= WaitStep;
		TrigState = TrigWait;
		ActiveCCR3Int();
		
	}
}

#if 0
void StartInternalTrig()
{
	TrigClks = NextTrigClks;
	SetTrigPointInternal();
}
#endif

u32 VerifyInterFreq(u32 clk, u32 minclk)
{
//	int i;
	if(clk < minclk) {
		int rate = (minclk +clk-1)/clk;
		
		return clk*rate;
	}
	else 
		return clk;
}

u32 VerifyExtHz(u32 hz, u32 maxhz)
{
	int i;
	if(hz > maxhz)
	{
		for(i=2;i<10000;i++)
		{
			if(maxhz*i > hz)
			{
				return	(( hz+i/2)/i);
			}
		}
		return hz;
	}
	else 
		return hz;
}

void StartInternalTrig()
{
	//TrigClks = NextTrigClks;
	//�����Ƶ�ʽ�������
	TrigClks = VerifyInterFreq(NextTrigClks,TrigLimitWidth);

	real_int_freq = TIM2_clk2Hz(TrigClks);

	SetTrigPointInternal();
}


//��ʱ��2�жϷ������
void TIM2_IRQHandler_Int(void)   
{
//	unsigned long long tmp;
//	static unsigned int num = 4;
#if 0
	if ((TIM2->DIER & TIM_IT_CC2) && (TIM2->SR & TIM_IT_CC2))  //���CC2�жϷ�������ⴥ������
	{
		TIM2->SR = (uint16_t)~TIM_IT_CC2; //���CC2�жϱ�־ 
		
	}	
	if((TIM2->DIER & TIM_IT_CC3) &&  (TIM2->SR & TIM_IT_CC3)){//���ⴥ������
		TIM2->SR = (uint16_t)~TIM_IT_CC3; //���CC3�жϱ�־ 
		if(TIM2->DIER & TIM_IT_CC3){
			if(TrigState == TrigWait){
				if(TrigClks<=WaitStep){
					TIM2->CCR3 += (uint16_t)TrigClks;
					ActiveTrigPoint();
					TrigState = TrigPrepared;
				}else {//�����ж�λ���´����ж�
					TIM2->CCR3 += (uint16_t)WaitStep;
					TrigClks -= WaitStep;
				}				
			}else if(TrigState == TrigPrepared){
				DeactiveTrigPoint();//��TIM2_CH3���������
				//if(IsTrigMode(Trig_Internal)){
				if(IsTrigMode(Trig_Internal) ){
					StartInternalTrig();
				}
				else{
					TrigState = Triged;
				}
			}	
		}
	}
	if ((TIM2->DIER & TIM_IT_Update) &&  (TIM2->SR & TIM_IT_Update))  //����жϷ�������ⴥ������
	{
		TIM2->SR = (uint16_t)~TIM_IT_Update; //����жϱ�־ 
		
	}	
#endif
}


void TIM2_IRQHandler (void)   
{

	TIM2_IRQHandler_Int();
}



unsigned int GetPulsePos(void)
{
	return (TIM3->CCR1 << 16 | TIM2->CCR2);
}




void SetFlash_PulseWidth(u16 width_clk)
{
	TIM1->ARR = width_clk;
	TIM1->CCR2 = CCR2_WIDTH;
}

void TIM_Reset(TIM_TypeDef * TIM)
{
	TIM->CR1 = 0;//ͣ������
	TIM->DIER = 0;//���ж�
	TIM->CCER = 0; //����������֮ǰ������ĳЩ�Ĵ����ı仯�ᵼ������仯��
	TIM->SMCR = 0 ;
	TIM->PSC  = 0 ;
	TIM->ARR = 0;

	TIM->CNT = 0; //����������SR֮ǰ�塣
				//���CNT!=0,CCR1=0����CNT�ᵼ��CC1IF=1��Ӱ�����ĵ�toggle
	TIM->SR =0;	//���ж�״̬λ
}


int TestFreq = 60;//1
#define TIM4_CLK 60000	





void CloseAllTimer(void)
{
	#if 0
	TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE); 
	TIM2->SR = 0;
	TIM_Cmd(TIM2,DISABLE);

	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); 
	TIM3->SR = 0;
	TIM_Cmd(TIM3,DISABLE);

	TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE); 
	TIM1->SR = 0;
	TIM_Cmd(TIM1,DISABLE);

	
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE); 
	TIM4->SR = 0;
	TIM_Cmd(TIM4,DISABLE);

#if 0
	RCC->APB2ENR &= ~RCC_APB2Periph_TIM1;
	RCC->APB1ENR &= ~RCC_APB1Periph_TIM2;
	RCC->APB1ENR &= ~RCC_APB1Periph_TIM3;
	//RCC->APB1ENR &= ~RCC_APB1Periph_TIM4;
#endif
	SwitchStrobeGPIO();
	#endif
}

void OpenAllTimer(void)
{

#if 0
	RCC->APB2ENR |= RCC_APB2Periph_TIM1;
	RCC->APB1ENR |= RCC_APB1Periph_TIM2;
	RCC->APB1ENR |= RCC_APB1Periph_TIM3;
	#endif
	SwitchStrobeAFIO();
}




//MAX Power:16W
//800W*2% = 16W

#if 0
void Updata_OutPusle(void)
{
	//from Min=5us to Max 50us
	static int max_pulse;
	u32 Linespeed2Rpm;	
	//if(SysPara.LightType == Light_LED)
	if(IsTypeLed())
		{
				if((AppPara.SpeedUnit == Unit_Hz)&&(AppPara.LampFreq > 0)){
					max_pulse =( Hz2Us(AppPara.LampFreq) + 25)/50; // 2 % 
					}
				else if((AppPara.SpeedUnit == Unit_rpm)&&(AppPara.Rpm > 0)){
					max_pulse =( Hz2Us(AppPara.Rpm*60) + 25)/50; // 2 % 
					}
				else if((AppPara.SpeedUnit == Unit_mpmin)&&(AppPara.LineSpeed > 0))
					{
					Linespeed2Rpm =( (AppPara.LineSpeed + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
					max_pulse =( Hz2Us(Linespeed2Rpm) + 25)/50; // 2 % 
					}
				/*
				if(max_pulse < AppParaMax.PulseWidth_LED) {
					AppPara.PulseWidth_LED =MIN(AppPara.PulseWidth_LED, max_pulse);
					}
					*/
				//flash_pulse=TIM1_us2clk(AppPara.PulseWidth_LED);
				flash_pulse = TIM1_us2clk(MIN(AppPara.PulseWidth_LED, max_pulse));
				SetFlash_PulseWidth(flash_pulse);

		}
	else
		{
			flash_pulse = TIM1_us2clk(AppPara.PulseWidth_Tube) ;
			SetFlash_PulseWidth(flash_pulse);
		}
		
}
#endif



void Updata_OutPusle(void)
{
	//from Min=5us to Max 50us
	static int max_pulse;
	u32 Linespeed2Rpm,tmp;	
//	u8 div_flg = 0 ;
	u32 tmp_nextclks,real_nextclks ;

	if(IsTrigMode(Trig_Internal) ) {

		if((AppPara.SpeedUnit == Unit_Hz)&&(AppPara.LampFreq > 0)){
			max_pulse =( max_strobe_power + AppPara.LampFreq/2) /AppPara.LampFreq ;
			}
		else if((AppPara.SpeedUnit == Unit_rpm)&&(AppPara.Rpm > 0)){
			tmp = (AppPara.Rpm + 3)/6  ;
			max_pulse =( max_strobe_power + tmp/2) /tmp ;
			}
		else if((AppPara.SpeedUnit == Unit_mpmin)&&(AppPara.LineSpeed > 0))
			{

			//���ٶ�a�� ��λ: 0.1m/min, a*100 = mm/min
			//���ٶ� ���� ת�٣� a*100 /�泤�� =  ����ת/min
			Linespeed2Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
			//ת�ٻ���Ƶ��b, ��λ0.01Hz��  b = a*100/60s = a*5/3 ;
			tmp = Linespeed2Rpm*5/3  ;
			//���ٶȺͰ峤�йأ�ÿ�弸�����棬�൱�����˱�Ƶ
			tmp = tmp * AppPara.ImagePerPlate ;

			//���ʵ������Ƶ�ʺ����ʵ���Ƶ��,Ȼ������������
			tmp_nextclks = TIM2_Hz2clk(tmp);
			real_nextclks = VerifyInterFreq(tmp_nextclks,TrigLimitWidth);
			//ʵ�ʵ����Ƶ��
			tmp = TIM2_clk2Hz(real_nextclks);	
			max_pulse =( max_strobe_power + tmp/2) /tmp ;
			
			}

		tmp = MIN(max_pulse,AppPara.PulseWidth_LED) + PULSE_WIDTH_OFFSET ;
		//tmp = AppPara.PulseWidth_LED + PULSE_WIDTH_OFFSET ;
		flash_pulse=TIM1_us2clk(tmp);
		SetFlash_PulseWidth(flash_pulse);
		
		}

}

// void gpio_test(void) {
// 	static u8 gpio_flag =  0;
// 	if(gpio_flag) {
// 		gpio_flag = 0;
// 		GPA_O(12) =  0;
// 		}
// 	else {
// 		gpio_flag = 1;
// 		GPA_O(12) =  1;
// 		}
// }


