#include "rtc_int.h"
#include "stm32.h"
#include "bsp.h"
#include "board_config.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "timer.h"
#include "adc.h"
#include "ctrl.h"
//#include "RTT\Segger_RTT.h"
//#include "utility.h"

//定时器20Hz, 50mS,实测40mS,每秒25.  LSI时钟模式下。
//后面改成了HSE/128分频，比较准些。实测50mS一次，比较准
#define TimeGainSec	20
#define TimeGainMin	1200 //20*60
#define ScanDlyTime	3*TimeGainSec   //用于自动扫描后的延时3秒
//用于手动调整频率后的延时3秒
//在手动扫描延时3秒的过程中，无论是屏蔽单击模式的操作
#define MainScanDlyTime 3*TimeGainSec	


u32 nWorkTime;
//电源按键检测控制
u16 PwrKey_Cnt1 = 0;
u16 PwrKey_Cnt0 = 0;
u16 PwrKey_FreeCnt = 0;
u8 PwrKey_Status = PwrKey_Idle;
/* 
PowerKey按下，大于3次就开始输出，
如果长按不放，则不能再次出发
必须加1个标志位区分按下大于等于3时候的电源键的状态
*/
u8 large3 = 0;

//闪光输出控制
//长按模式的计数
volatile u8 PwrPress_FlagON = 0;
volatile  u8 PwrPress_FlagOFF = 0;
#define  KeyPushTimes           2
//单击模式下,WorkEn = 1,代表一次工作的开始
volatile u8 WorkEn = 0;
//在自动调频模式下，禁止电源按键的功能
volatile u8 PwrKeyDisableFlag = 0;
u32	PwrFreeCnt = 0;
//单击模式的状态及计数
u8 PwrHitFlag = PwrHit_STALL;
volatile u32 PwrHitCnt = 0; //用于电源键单击模式下的超时计数

//关机控制
//用于暂停状态下的，延时关机，关显示累积计数
volatile u32 CloseCnt = 0;	

//背光显示控制
//背光的状态+ 延时计数
volatile u32 BlkCtrlCnt = 0;
volatile u8 BlkStatus = BLK_ON;

//充电显示状态
volatile u8 ChargeFlag = false;

//进入扫频界面标志
/*
自动扫频的2个阶段
1) 进入自动扫频的界面，这里不允许关闭背光
2) 退出自动扫频后的3秒延时阶段
EnterScanFlag = 1,代表开始进入自动扫频，=0的时候退出
ScanDlyFlag = 1，代码开始延时阶段，同时开始延时计数ScanDlyCounting
*/
volatile u8 EnterScanFlag = 0;
volatile u8 ScanDlyFlag = 0;
u16 ScanDlyCnt = 0;
u8 ScanControlEn = 0; //防止多次进入
volatile u16 ScanDlyCounting = 0;


//主界面直接进入闪光调整
volatile u8 MainScanFlag = 0;
u16 MainScanDlyCnt = 0;
 //防止多次进入MainScanControlEn = 1，代表一次手动调频的开始，=0时才完成
u8 MainScanControlEn = 0;

//任意键按下标志
//volatile u8 AnyKeyPressedFlag = 0;

//RTC的分频计数
u8 RtcCnt = 0;

//#ifdef RTT
extern u8 _MainDispBuf[];
extern u8 _SecondDispBuf[];
extern  u8   KeyRepeatNum; //重复键数
extern RTC_HandleTypeDef hrtc;
//#endif


//extern void KeyInput(void);
extern void EnterStandby(void);
extern unsigned char code_HZ[];
extern void SwitchStrobeAFIO(void);


//__inline static void GPIOTest(void);
#ifdef RTT
__inline static void rtt_printf(void) ;
#endif
void ForceToStop(void) ;
int GetSpeedHz(void) ;


/**
  * @brief This function handles RTC wake-up interrupt through EXTI line 20.
  */
void RTC_WKUP_IRQHandler(void)  //50ms为周期
{
  static u8 encoder_wakeup_flag = 0;
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);   //clear RTC wake up counter flag
//电源按键检测
	PwrKey_Detector();
	//组合按键Enter + Mode + Power 进入Standby模式
	if((PwrKey_Status == PwrKey_Pressed)&&(!GPI_KEY_ENTER)){
			ShutDown();
	}
  KeyInput(); //按键输入检测
  
  AnyKeyPressed_Control();//检测到任意键按下需要的处理
  
  CloseDelay_Handler();//无操作状态下，延时关机处理
  
  Blk_Control();//背光控制处理
  //encoder_test_pwmAdjust(); //编码器测试函数
  //只有所有的系统初始化完毕后，才允许输出
	if(Status_MCU != Status_idle) {
		//控制闪光输出的处理
		if((EnterScanFlag == 0)&&(MainScanFlag == 0)) {
		//if(IsTrigMode(Trig_Internal)||IsTrigMode(Trig_SinglePulse)||(IsTrigMode(Trig_Perimeter)))
		{
			switch(AppPara.PowerKey)
				{
					case PwrKey_Hit:
						PwrKeyHit_Handler();
						break;
					case PwrKey_Press:
						PwrKeyPress_Handler();
						break;
					default:
						break;
				}				
			}

		}
		//处理进入自动扫描模式
		Scan_Control();
		//处理主界面频率调整模式
		MainScan_Control();
	}

}

void ReInitSysTick(u8 freq)
{
	u32 tmp;
	tmp = SysTick->VAL;
	
    SysTick->CTRL = 0;
    SysTick->LOAD  =  SYSTICK_MAXCOUNT;                    /* set reload register */
    SysTick->VAL   =  tmp;                              /* Load the SysTick Counter Value */
    SysTick->CTRL = (freq << SysTick_CTRL_CLKSOURCE_Pos) //0: AHB clock/8, 1: AHB clock
                | (0<<SysTick_CTRL_TICKINT_Pos)  
                | (1<<SysTick_CTRL_ENABLE_Pos);   /* Enable SysTick IRQ and SysTick Timer */
}



//已经在初始化GPIO的时候设置了，这个函数没有用
void SwitchStrobeHz(void)
{
#if 0
	//配置Strobe引脚为高阻抗输入模式
	GPIOB->CRL  &= ~0xF;
	GPIOB->CRL |= GPI_IF;
#endif
}



#define FREQ_LV1		2000 //0~20Hz
#define FREQ_LV2			4500 //20~45Hz
#define Clks_FREQ_LV1	TIM2_Hz2clk(FREQ_LV1)
#define Clks_FREQ_LV2	TIM2_Hz2clk(FREQ_LV2)


//VSET1,VSET2设定的高压等级
#define LEVEL1	0	//370V  (0~20Hz)
#define LEVEL2	1	//430V  (20~45Hz)
#define LEVEL3	2	//550V  (45Hz+)



int UpdateVout(void)
{
	// no need for led
	return -1;
}

__inline void PwrKeyHit_Handler(void)
{
	//if((PwrKey_Status == PwrKey_Pressed)&&(!PwrKeyDisableFlag ))
	if((PwrKey_FreeCnt >= 1)&&(!PwrKeyDisableFlag )&&(large3 == 0))
	{
			large3 = 1;
			//PwrKey_FreeCnt = 0;
			if(PwrHitFlag == PwrHit_STALL)
			{
					PwrHitFlag = PwrHit_WORK; //可以开始工作，并且开始计时
					WorkEn = 1;
					StartToFlash();
					
			}
			else if(PwrHitFlag == PwrHit_WORK)
			{
					PwrHitFlag = PwrHit_STALL; //停止工作
					WorkEn = 0;
					StopToFlash();
			}
			
	}
	if(WorkEn == 1)
	{
		PwrHitCnt++;  //50mS 一次RTC, 20次就是一分钟，AppPara.WorkTime = n 分钟
		nWorkTime  = TimeGainMin*AppPara.WorkTime;
				//if(PwrHitCnt >= TimeGainMin*AppPara.WorkTime)
		if(PwrHitCnt >= nWorkTime)
		{
			//单击模式下，一次工作超时时间到
			PwrHitCnt = 0;
			WorkEn = 0;
			StopToFlash();
			PwrHitFlag = PwrHit_STALL; 
			//EnterStandby();
			Status_MCU = Status_WorkStall;
		}
	}
	else
	{
		PwrHitCnt = 0;// 如果按下了电源键，就清0
	}
}
__inline void PwrKeyPress_Handler(void)
{
	//if(PwrKeyPressedCnt >= 2*KeyPushTimes)//检测到Pwr键按下
	if(PwrKey_Status == PwrKey_Pressed)
		{
			PwrPress_FlagOFF = 0;
			if(PwrPress_FlagON == 0)
				{
					PwrPress_FlagON = 1;
					StartToFlash();
				}
		}
	else //if(PwrKeyReleasedCnt >= 2*KeyPushTimes)
		{
			PwrPress_FlagON = 0;
			if(PwrPress_FlagOFF == 0)
			{
			PwrPress_FlagOFF = 1;
			//重新置位用于超时的计数和标志
			StopToFlash();
			}
			
						}
}
__inline void PwrKey_Detector(void)
{
	if(POWER_PRESSED)  	//PwrKey Pressed
	{
			PwrKey_Cnt1++;
			if(PwrKey_FreeCnt < 3)
			{
				PwrKey_FreeCnt++;
			}
			if(PwrKey_Cnt1 >= KeyPushTimes)
			{
				//PwrKey_Cnt0 = 0;
				PwrKey_Status = PwrKey_Pressed;
				PwrKey_Cnt1 = 0;
			
				//电源键按下的时候，关机延时的处理
				//不管什么状态，直接清零
				CloseCnt = 0;

				//电源键按下的时候，背光的处理
				//如果是 开的，清零计数
				//如果是关闭的，重新打开
				if(BlkStatus == BLK_OFF) 
				{
					BackLightOn();
					BlkCtrlCnt = 0;
					BlkStatus = BLK_ON;
				}
				else
				{
					BlkCtrlCnt = 0;
				}
			}
			
	}
	else
	{
			if(PwrKey_Status == PwrKey_Release)
			{
				PwrKey_Status = PwrKey_Idle;
			}
			PwrKey_Cnt0++;
			if(PwrKey_Cnt0 >= KeyPushTimes) 
			{
				//PwrKey_Cnt1 = 0;
				PwrKey_Cnt0 = 0;
				if(PwrKey_Status == PwrKey_Pressed) 
				{
					PwrKey_Status = PwrKey_Release;
				}
			}
			PwrKey_FreeCnt = 0;
			large3 = 0;
	}
}
__inline void CloseDelay_Handler(void)
{
	//暂停状态开始计数，延时关机
	if(Status_MCU == Status_WorkStall)
	{
		//需要在电源键和任意键按下的时候清零
		CloseCnt++;
		if(CloseCnt >= TimeGainMin*AppPara.PowerOffDly)
			{
				//关闭系统
				ShutDown();
			}
	}
	else
		CloseCnt = 0;
}

__inline void Blk_Control(void)
{
	AppPara.BackLightDelay = 10;
		//系统初始化完毕后，才使能背光的控制
		if(Status_MCU != Status_idle)
		{
			if(BlkStatus == BLK_ON) {BlkCtrlCnt++;}
			if((BlkCtrlCnt > TimeGainSec*AppPara.BackLightDelay)&&(BlkStatus == BLK_ON)) 
				{
					//close blacklight
					BackLightOff();
					BlkCtrlCnt = 0;
					BlkStatus = BLK_OFF;
				}
			//任意键按下的处理
			//电源键按下的处理

		}

}


__inline void Scan_Control(void)
{
	if((EnterScanFlag == 1) &&(ScanControlEn == 0)){
		if(IsTrigMode(Trig_Internal))
		{
			ScanControlEn = 1;
			switch(AppPara.PowerKey)
				{
					case PwrKey_Hit:
						if(PwrHitFlag == PwrHit_STALL)
							{
							PwrHitFlag = PwrHit_WORK;
							StartToFlash();
							}
						break;
					case PwrKey_Press:
						if(PwrKey_Status != PwrKey_Pressed)
							StartToFlash();
						break;
					default:
						break;
				}
				
			}
	}

	//if((EnterScanFlag == 0) &&(ScanDlyCounting == 0)){
	if((EnterScanFlag == 0) &&(ScanDlyCounting == 0)&&(ScanControlEn == 1)){
		ScanControlEn = 0;
		switch(AppPara.PowerKey)
		{
			case PwrKey_Hit:
				PwrHitCnt = 0;
				break;
			case PwrKey_Press:
				if(PwrKey_Status != PwrKey_Pressed)
					StopToFlash();
				break;
			default:
				break;
		}
	}

	//从Enter()返回后开始延时计数
	if(ScanDlyFlag == 1)
	{
		ScanDlyCnt++;
		if(ScanDlyCnt  >= ScanDlyTime) {
			ScanDlyCnt = 0;
			ScanDlyFlag = 0;
			ScanDlyCounting = 0;
		}
			
	}
}

__inline void MainScan_Control(void)
{
	//如果检测到主页面按下4个键(+,-,*,/)
	//如果没有开始闪光，就要打开
	//打开之后，设定一个Dly 时间
	//Dly时间内，检测到任意键，清零计数
	//Dly时间到了
	//如果单击模式，清零单击计数器，单击一次的工作时间到停机
	//如果是连续模式，直接关闭输出，停止闪光
 	if((MainScanFlag == 1) &&(MainScanControlEn == 0)){
		if(IsTrigMode(Trig_Internal))
		{
			//开始一次延时闪光调整过程
			MainScanControlEn = 1;
			//延时计数清零
			MainScanDlyCnt =0;
			switch(AppPara.PowerKey)
				{
					case PwrKey_Hit:
						//如果当前是单击模式，并且输出没有打开，开始闪光
						if(PwrHitFlag == PwrHit_STALL)
							{
							PwrHitFlag = PwrHit_WORK;
							StartToFlash();
							}
						break;
					case PwrKey_Press:
						//如果当前是长按模式，并且没有长按，开始直接闪光
						if(PwrKey_Status != PwrKey_Pressed)
							StartToFlash();
						break;
					default:
						break;
				}
				
			}
	}

	//如果开始了一次延时闪光调整过程，开始计数延时时间
	if(MainScanControlEn == 1) {
			MainScanDlyCnt++;
			if(MainScanDlyCnt >= MainScanDlyTime){
				//延时时间到了,清零计数
				MainScanDlyCnt = 0;
				//一次延时调整完成
				MainScanControlEn = 0;
				MainScanFlag = 0;
				//防止Main界面调整快要退出的时候，又进入了自动扫描模式，不能关闭
				if((EnterScanFlag != 1)&&(ScanDlyCounting != 1)) {
					//对当前的不同操作模式进行处理
					switch(AppPara.PowerKey)
					{
						case PwrKey_Hit:
							//单击模式，清零单击计数，一直延时到一次工作时间
							PwrHitCnt = 0;
							break;
						case PwrKey_Press:
							//长按模式，如果没有长按，就停止
							if(PwrKey_Status != PwrKey_Pressed)
								StopToFlash();
							break;
						default:
							break;
					}
				}
				
			}
	}
}

//任意键按下时候需要执行的操作
__inline void AnyKeyPressed_Control(void)
{
	//发生了按下任意键
	if(AnyKeyPressedFlag == 1) {
		AnyKeyPressedFlag = 0;
		//手动调频的延时计数要清零
		MainScanDlyCnt = 0;
		//关机延时计数，不管什么状态，直接清零
		CloseCnt = 0;

		//任意键按下的时候背光的处理
		if(BlkStatus == BLK_ON) 
			{BlkCtrlCnt = 0;}
		else {
			//任意键唤醒背光
			BlkStatus = BLK_ON;
			BackLightOn();
			BlkCtrlCnt = 0;
		}

	}

	//2016.1.8
	//因为修改了按键，没有了KeyRepeatNum
	//没有注释MainScanDlyCnt = 0;
	//导致修改了频率后暂停按键不起作用
		
	//如果长按不放也清零
	//if(KeyRepeatNum > 0)
		{
			//手动调频模式的延时计数也清零，长按的情况下
			//MainScanDlyCnt = 0;
		}

}

#ifdef RTT
__inline static void rtt_printf(void) {
		 int ret;
		 int i,j;
		 char tmp = 0x0A;
		 char ch = 0x3E;
		 char buf[3];
		 u8 *p = NULL;
		 ret = SEGGER_RTT_GetKey() ;
		 if(ret == 0x70)
		 	p = &_MainDispBuf[0];
		 else if(ret == 0x73)
		 	p =  &_SecondDispBuf[0];
		 else
		 	return;
		 	
		
		 SEGGER_RTT_Write(0,&tmp,1);//换行
		 SEGGER_RTT_Write(0,&ch,1);//换行
		 SEGGER_RTT_Write(0,&tmp,1);//换行
		 for(i=0;i<8*8;i++){
			 for(j=0;j<128/8;j++){
				 U8ToHEXStr(*p++, buf);
				 SEGGER_RTT_Write(0, buf, 2);
			 }
			 SEGGER_RTT_Write(0,&tmp,1);//换行
		 }
		 SEGGER_RTT_Write(0,&tmp,1);//换行
		 SEGGER_RTT_Write(0,&tmp,1);//换行
		 SEGGER_RTT_Write(0,&tmp,1);//换行
		 SEGGER_RTT_Write(0,&tmp,1);//换行
}
#endif

void ForceToStop(void) {
	if(Status_MCU != Status_WorkStall) {
		PwrHitCnt = 0;
		WorkEn = 0;
		StopToFlash();
		PwrHitFlag = PwrHit_STALL; 
		//EnterStandby();
		Status_MCU = Status_WorkStall;
	}
}


int GetSpeedHz(void) {
	int tmp;
	if(IsTrigMode(Trig_Internal)) {
		switch(AppPara.SpeedUnit)
			{
				case Unit_Hz:
					tmp = AppPara.LampFreq;
					break;
				case Unit_rpm:
					tmp = Rpm2Hz(AppPara.Rpm);
					break;
				case Unit_mpmin:
					tmp = TIM2_clk2Hz(NextTrigClks);
					break;
				default:
					break;
			}
		}
	else {

		//外触发的时候，都会计数外部的频率
		//在合适的地方，计算实际的输出闪光频率
		//统一为AvgHz_Out
		tmp = AvgHz_Out;
		
		/*
		switch(AppPara.TrigMode)
			{
				case Trig_SinglePulse:
					//因为外部频率高了，会进行分频
					AvgHz_Out = AvgHz/TrigCnt;
					break;
				case Trig_Perimeter:
					break;
				case Trig_Gear:
					break;
				case Trig_Encoder:
					break;
				default:
					break;
			}
			*/

		}
	
	return tmp;
}
