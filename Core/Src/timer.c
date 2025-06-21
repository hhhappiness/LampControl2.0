#include "timer.h"
#include "encoder.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "adc.h"
#include "rtc_int.h"
#include "ctrl.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"

extern TIM_HandleTypeDef htim2, htim3, htim4,htim6;
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
#define MaxCounterLimit TIM2_Hz2clk(AppParaMin.LampFreq)//TIM2 period���ޣ���Փ���������ֵ��hzΪ��Сֵ1ʱ������Ϊ1000000us
#define MinCounterLimit TIM2_Hz2clk(AppParaMax.LampFreq) //TIM2 period����

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

void TIM_Reset(TIM_TypeDef * TIM);
void StartInternalTrig(void);



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

u32 VerifyInterFreq(u32 counter)
{
	//�����Ƶ�ʽ�������
	/* ������� */
    if (counter > MaxCounterLimit) 
		return MaxCounterLimit ; //������������ֵ������Ϊ���
	if (counter < MinCounterLimit) 
		return MinCounterLimit ; //���С����Сֵ������Ϊ��С
	return counter;
}

void StartInternalTrig()
{
	/* ����TIM2���� */
	TrigClks = VerifyInterFreq(NextTrigClks);
    __HAL_TIM_SET_AUTORELOAD(&htim2, TrigClks-1);  //period= TrigClks-1
    TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE); //����ͨ��2�����
    __HAL_TIM_ENABLE(&htim2); //TIM3trigger mode, TIM2��Ϊ������
	/* ����TIM2��ʱ�����Զ��޸�PB3��GPIOģʽ�������ֶ��ָ�PB3Ϊ����ģʽ */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM6_DAC_IRQHandler(void)
{
	static u32 flag_1ms = 0;
	HAL_TIM_IRQHandler(&htim6);//��־λ֮���
	HAL_IncTick();  //���systick�жϽ���ÿms����
	flag_1ms++;   //1000/s��32λ�ܹ����Լ�����2^32-1
		
	if(flag_1ms%50 == 0) { //ÿ50msִ��һ��
		Encoder_Update();
		KeyInput(); //����������
	}else if(flag_1ms%200 == 0){
		if(IsTrigMode(Trig_Internal)) StartInternalTrig();   //�ڴ���ģʽ,���ڸ��µ��ڴ���Ƶ��
	}
	
}

inline void SetFlash_PulseWidth(u16 width_clk)
{
	#if 0
	//��ȷ���Ƿ����Ҫ��ֹͣ�����������������
	TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_DISABLE);
    __HAL_TIM_DISABLE(&htim2);
	#endif
	/* ����TIM3����*/
    // ��������TIM3���ڣ����one pulse mode���}���Y���᲻�þ�һֱ����ƽ�ˣ����ó�ʼ�����ڼ���
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, width_clk-1); //CCR2= PwmPulseWidth_us-1
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



//MAX Power:16W
//800W*2% = 16W

void Updata_OutPusle(void)
{
	//from Min=5us to Max 50us
	static int max_pulse;
	u32 Linespeed2Rpm,tmp;	
//	u8 div_flg = 0 ;
	u32 tmp_nextclks,real_nextclks;

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

			//���ٶ�a�� ��λ: dm/min, a*100 = mm/min
			//���ٶ� ���� ת�٣� a*100 /�泤�� =  ����ת/min
			Linespeed2Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
			//ת�ٻ���Ƶ��b, ��λ0.01Hz��  b = a*100/60s = a*5/3 ;
			tmp = Linespeed2Rpm*5/3  ;
			//���ٶȺͰ峤�йأ�ÿ�弸�����棬�൱�����˱�Ƶ
			tmp = tmp * AppPara.ImagePerPlate ;

			//���ʵ������Ƶ�ʺ����ʵ���Ƶ��,Ȼ������������
			tmp_nextclks = TIM2_Hz2clk(tmp);
			real_nextclks = VerifyInterFreq(tmp_nextclks);
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
	

