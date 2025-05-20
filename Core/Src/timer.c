#include "timer.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "adc.h"
#include "rtc_int.h"
#include "ctrl.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"

extern TIM_HandleTypeDef htim2, htim3, htim4;
#define MAJOR_VERSION 	1
#define MID_VERSION 	0
#define MINOR_VERSION 	0
//关于小数点位数。
//角度的单位是1度，除以360转成圆周比例，Q30无符号小数。
//平均间隔由于211加权得到4倍的值，多了两位。
//圆周比例*平均间隔得到64位数，刚好低32舍去。

//将1度为单位的角度转成圆周比例，Q30无符号小数。

#define MAX_STEP 	65536	//设定下一个比较器值时的递增值必须<=65536。本来65535是安全值，但进中断时CNT肯定超过CCR值了，所以65536也没问题，比较好计算
#define CalcAvgPulseWidth() AvgPulseWidth = PulseWidth[0]*2 + PulseWidth[1] + PulseWidth[2] //加权计算平均脉冲宽度


#define MIN_PULSE_FREQ 	1	//外触发检测的最小频率
unsigned int AvgPulseWidth;	//平均脉冲间隔
volatile unsigned int AvgHz, AvgHz_Out;	//平均转速，0.01Hz
int ExternalSpeed=0;
//unsigned int PulseWidth[3];	//最近三次脉冲宽度
unsigned int PulseWidth[7];
unsigned int CurrPulsePos, LastPulsePos;	//前后两次脉冲的位置，用于计算脉冲间隔
//各触发角度，单位可以是
unsigned int TrigAngle0=30;
//归一划转成Q30小数
unsigned int TrigAngle=AngleCov(30);
int PulseCnt=0;//脉冲计数，至少>4才能计算出平均值
unsigned int WaitStep;	//计算曝光延时时，比较器递增值
unsigned int TrigClks;	//延时总时钟数
volatile unsigned int NextTrigClks;
u32 TrigLimitWidth;
u32 TrigDiv;
u32 TrigCnt;
//内触发频率
volatile u32 real_int_freq;

int CapEnable=1;				
int OverflowCnt = 0;	//脉冲间隔超宽，表示转速太慢或停止

//周长标长模式内触发触发启动标志
volatile u8 Trig_Perimeter_En = 0;
volatile u8 ExtTrigOnce = 0;

volatile u8 ExtTrigOn = 0; //外部触发有为1，停止了为0

//齿轮模式的变量
volatile u16 GearCnt = 0;
u16 GearTrigInt;
u16 GearTirgFrag;
unsigned int TrigAngleGear=AngleCovGear(1);

//编码器模式的变量
volatile u16 aCnt = 0;
volatile u16 zCnt = 0;
volatile u16 EncoderLine = 1024; //编码器的线数
u16 EncoderLine_old = 1024;

volatile int tim1_width ; //
volatile int flash_pulse; //

TrigState_t TrigState = Idle;

//闪光延时与宽度的单位是100ns
#define FLASH_DELAY_DEFAULT	1
#define FLASH_WIDTH_DEFAULT	200		//输出的strobe的脉宽
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
volatile u16  image_num = 1 ; //每版N个画面
volatile u32 image_period_per ; //每个版的周期


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
//volatile u32 image_num  ; //画面的个数，都公用

volatile u8 NeedRfReInit = 0 ;



//外触发模式的启动标志
volatile u8 Trig_Ext_En = 0;
volatile u8 Trig_Ext_On = 0;

volatile u8 Trig_Ext_En_Wifi = 0;
volatile u8 Trig_Ext_On_Wifi = 0;

#define	MAX_STROB_POWER	1000000 
//MOS管打开有个延时，暂时固定一个7us的偏移量
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



__inline void ActiveTrigPoint(void)
{
	//设定当TIM2_CH3当CNT=CCR3时输出变高，然后在中断里拉低。
	// TIM2->CCMR2 = (TIM_OCMode_Timing | TIM_OCPreload_Disable)<<8 //通道4的模式
	// 			| (TIM_OCMode_Active | TIM_OCPreload_Disable)<<0; //通道3的模式	
}

__inline void ActiveCCR3Int(void)
{
	// TIM2->SR = (uint16_t)~TIM_IT_CC3; //清除中断标志 
	// TIM2->DIER |= TIM_IT_CC3;	
}
void DeactiveTrigPoint(void)
{
	//设定当TIM2_CH3当CNT=CCR3时输出变高，然后在中断里拉低。
	// TIM2->CCMR2 = (TIM_OCMode_Timing | TIM_OCPreload_Disable)<<8 //通道4的模式
	// 			| (TIM_ForcedAction_InActive | TIM_OCPreload_Disable)<<0; //通道3的模式	
	// TIM2->DIER &= (uint16_t)~TIM_IT_CC3;	
}

//计算触发点相对于脉冲边缘的时钟数
__inline void CalcTrigPoint(void)
{
	
	//对于单脉冲，最大频率来自于外部频率的计算
	//对于标长周长，最大频率来自于外部频率的计算* 标长/周长比值

	//*4 是因为AvgPulseWidth没有除4
	TrigDiv = 4*TrigLimitWidth/AvgPulseWidth + 1;
	TrigClks = ((unsigned long long)AvgPulseWidth*TrigAngle)>> 32;//触发角度换算成时钟延时
}

#define  TIM1_100ns2clk(ns) ((ns)*(TIMxCLK/1000000)/10)
#define  TIM1_us2clk(us) 	((us)*(TIMxCLK/1000000))
#define  Hz2Us(hz)	(1000*1000*100 + hz/2)/hz





//频率过高时强制分频
__inline int VerifyInterval(void)
{
	TrigCnt++;
	if(TrigCnt >= TrigDiv){
		TrigCnt = 0;
		return 1;
	}else
		return 0; //可以触发
}

//设定触发点
__inline void SetTrigPoint(void)
{
	//如果TrigClks<=65535,可以直接设曝光输出位置，否则按<=65536的步进中断，直到最后一步再设曝光输出位置
	//WaitStep = TrigClks/(TrigClks/MAX_STEP+1);	
	WaitStep = TrigClks/((TrigClks+MAX_STEP-1)/MAX_STEP)+1;
	if(TrigClks<=WaitStep){
		//允许输出和中断
		TIM2->CCR3 = TIM2->CCR2 + (uint16_t)TrigClks;
		ActiveTrigPoint();
		ActiveCCR3Int();
		TrigState = TrigPrepared;
	}else{
		//设置中断位，下次再判断
		TIM2->CCR3 = TIM2->CCR2 + (uint16_t)WaitStep;
		TrigClks -= WaitStep;
		ActiveCCR3Int();
		TrigState = TrigWait;
	}
}

__inline void SetTrigPointInternal(void)
{
	//如果TrigClks<=65535,可以直接设曝光输出位置，否则按<=65536的步进中断，直到最后一步再设曝光输出位置
	//WaitStep = TrigClks/(TrigClks/MAX_STEP+1);	
	WaitStep = TrigClks/((TrigClks+MAX_STEP-1)/MAX_STEP)+1;
	if(TrigClks<=WaitStep){
		//允许输出和中断
		TIM2->CCR3 = TIM2->CCR3 + (uint16_t)TrigClks;
		ActiveTrigPoint();
		TrigState = TrigPrepared;
		ActiveCCR3Int();
		
	}else{
		//设置中断位，下次再判断
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
	//对输出频率进行限制
	TrigClks = VerifyInterFreq(NextTrigClks,TrigLimitWidth);

	real_int_freq = TIM2_clk2Hz(TrigClks);

	SetTrigPointInternal();
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
	TIM->CR1 = 0;//停计数器
	TIM->DIER = 0;//禁中断
	TIM->CCER = 0; //放在清其它之前，否则某些寄存器的变化会导致输出变化。
	TIM->SMCR = 0 ;
	TIM->PSC  = 0 ;
	TIM->ARR = 0;

	TIM->CNT = 0; //这个必须放在SR之前清。
				//如果CNT!=0,CCR1=0，清CNT会导致CC1IF=1，影响后面的的toggle
	TIM->SR =0;	//清中断状态位
}


int TestFreq = 60;//1
#define TIM4_CLK 60000	





void CloseAllTimer(void)
{

	  /* Disable the TIM Update interrupt */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
	

	/* Disable the Peripheral CCER and CEN 寄存器复位*/
	__HAL_TIM_DISABLE(&htim2);
	__HAL_TIM_DISABLE(&htim3);
	__HAL_TIM_DISABLE(&htim4);
	//状态寄存器复位
	TIM2->SR = 0;
	TIM3->SR = 0;
	TIM4->SR = 0;

	RCC->APB1ENR1 &= ~(RCC_APB1ENR1_TIM2EN|RCC_APB1ENR1_TIM3EN|RCC_APB1ENR1_TIM4EN);  //关闭TIM2,3,4时钟
}

void OpenAllTimer(void)
{

#if 0
	RCC->APB2ENR |= RCC_APB2Periph_TIM1;
	RCC->APB1ENR |= RCC_APB1Periph_TIM2;
	RCC->APB1ENR |= RCC_APB1Periph_TIM3;
#endif
__HAL_RCC_TIM3_CLK_ENABLE();
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

			//线速度a： 单位: 0.1m/min, a*100 = mm/min
			//线速度 折算 转速： a*100 /版长度 =  多少转/min
			Linespeed2Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
			//转速换成频率b, 单位0.01Hz，  b = a*100/60s = a*5/3 ;
			tmp = Linespeed2Rpm*5/3  ;
			//线速度和板长有关，每板几个画面，相当于做了倍频
			tmp = tmp * AppPara.ImagePerPlate ;

			//求出实际限制频率后的真实输出频率,然后做功率限制
			tmp_nextclks = TIM2_Hz2clk(tmp);
			real_nextclks = VerifyInterFreq(tmp_nextclks,TrigLimitWidth);
			//实际的输出频率
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


