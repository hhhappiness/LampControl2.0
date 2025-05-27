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

//��ʱ��20Hz, 50mS,ʵ��40mS,ÿ��25.  LSIʱ��ģʽ�¡�
//����ĳ���HSE/128��Ƶ���Ƚ�׼Щ��ʵ��50mSһ�Σ��Ƚ�׼
#define TimeGainSec	20
#define TimeGainMin	1200 //20*60
#define ScanDlyTime	3*TimeGainSec   //�����Զ�ɨ������ʱ3��
//�����ֶ�����Ƶ�ʺ����ʱ3��
//���ֶ�ɨ����ʱ3��Ĺ����У����������ε���ģʽ�Ĳ���
#define MainScanDlyTime 3*TimeGainSec	


u32 nWorkTime;
//��Դ����������
u16 PwrKey_Cnt1 = 0;
u16 PwrKey_Cnt0 = 0;
u16 PwrKey_FreeCnt = 0;
u8 PwrKey_Status = PwrKey_Idle;
/* 
PowerKey���£�����3�ξͿ�ʼ�����
����������ţ������ٴγ���
�����1����־λ���ְ��´��ڵ���3ʱ��ĵ�Դ����״̬
*/
u8 large3 = 0;

//�����������
//����ģʽ�ļ���
volatile u8 PwrPress_FlagON = 0;
volatile  u8 PwrPress_FlagOFF = 0;
#define  KeyPushTimes           2
//����ģʽ��,WorkEn = 1,����һ�ι����Ŀ�ʼ
volatile u8 WorkEn = 0;
//���Զ���Ƶģʽ�£���ֹ��Դ�����Ĺ���
volatile u8 PwrKeyDisableFlag = 0;
u32	PwrFreeCnt = 0;
//����ģʽ��״̬������
u8 PwrHitFlag = PwrHit_STALL;
volatile u32 PwrHitCnt = 0; //���ڵ�Դ������ģʽ�µĳ�ʱ����

//�ػ�����
//������ͣ״̬�µģ���ʱ�ػ�������ʾ�ۻ�����
volatile u32 CloseCnt = 0;	

//������ʾ����
//�����״̬+ ��ʱ����
volatile u32 BlkCtrlCnt = 0;
volatile u8 BlkStatus = BLK_ON;

//�����ʾ״̬
volatile u8 ChargeFlag = false;

//����ɨƵ�����־
/*
�Զ�ɨƵ��2���׶�
1) �����Զ�ɨƵ�Ľ��棬���ﲻ����رձ���
2) �˳��Զ�ɨƵ���3����ʱ�׶�
EnterScanFlag = 1,����ʼ�����Զ�ɨƵ��=0��ʱ���˳�
ScanDlyFlag = 1�����뿪ʼ��ʱ�׶Σ�ͬʱ��ʼ��ʱ����ScanDlyCounting
*/
volatile u8 EnterScanFlag = 0;
volatile u8 ScanDlyFlag = 0;
u16 ScanDlyCnt = 0;
u8 ScanControlEn = 0; //��ֹ��ν���
volatile u16 ScanDlyCounting = 0;


//������ֱ�ӽ����������
volatile u8 MainScanFlag = 0;
u16 MainScanDlyCnt = 0;
 //��ֹ��ν���MainScanControlEn = 1������һ���ֶ���Ƶ�Ŀ�ʼ��=0ʱ�����
u8 MainScanControlEn = 0;

//��������±�־
//volatile u8 AnyKeyPressedFlag = 0;

//RTC�ķ�Ƶ����
u8 RtcCnt = 0;

//#ifdef RTT
extern u8 _MainDispBuf[];
extern u8 _SecondDispBuf[];
extern  u8   KeyRepeatNum; //�ظ�����
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
void RTC_WKUP_IRQHandler(void)  //50msΪ����
{
  static u8 encoder_wakeup_flag = 0;
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);   //clear RTC wake up counter flag
//��Դ�������
	PwrKey_Detector();
	//��ϰ���Enter + Mode + Power ����Standbyģʽ
	if((PwrKey_Status == PwrKey_Pressed)&&(!GPI_KEY_ENTER)){
			ShutDown();
	}
  KeyInput(); //����������
  
  AnyKeyPressed_Control();//��⵽�����������Ҫ�Ĵ���
  
  CloseDelay_Handler();//�޲���״̬�£���ʱ�ػ�����
  
  Blk_Control();//������ƴ���
  //encoder_test_pwmAdjust(); //���������Ժ���
  //ֻ�����е�ϵͳ��ʼ����Ϻ󣬲��������
	if(Status_MCU != Status_idle) {
		//������������Ĵ���
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
		//��������Զ�ɨ��ģʽ
		Scan_Control();
		//����������Ƶ�ʵ���ģʽ
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



//�Ѿ��ڳ�ʼ��GPIO��ʱ�������ˣ��������û����
void SwitchStrobeHz(void)
{
#if 0
	//����Strobe����Ϊ���迹����ģʽ
	GPIOB->CRL  &= ~0xF;
	GPIOB->CRL |= GPI_IF;
#endif
}



#define FREQ_LV1		2000 //0~20Hz
#define FREQ_LV2			4500 //20~45Hz
#define Clks_FREQ_LV1	TIM2_Hz2clk(FREQ_LV1)
#define Clks_FREQ_LV2	TIM2_Hz2clk(FREQ_LV2)


//VSET1,VSET2�趨�ĸ�ѹ�ȼ�
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
					PwrHitFlag = PwrHit_WORK; //���Կ�ʼ���������ҿ�ʼ��ʱ
					WorkEn = 1;
					StartToFlash();
					
			}
			else if(PwrHitFlag == PwrHit_WORK)
			{
					PwrHitFlag = PwrHit_STALL; //ֹͣ����
					WorkEn = 0;
					StopToFlash();
			}
			
	}
	if(WorkEn == 1)
	{
		PwrHitCnt++;  //50mS һ��RTC, 20�ξ���һ���ӣ�AppPara.WorkTime = n ����
		nWorkTime  = TimeGainMin*AppPara.WorkTime;
				//if(PwrHitCnt >= TimeGainMin*AppPara.WorkTime)
		if(PwrHitCnt >= nWorkTime)
		{
			//����ģʽ�£�һ�ι�����ʱʱ�䵽
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
		PwrHitCnt = 0;// ��������˵�Դ��������0
	}
}
__inline void PwrKeyPress_Handler(void)
{
	//if(PwrKeyPressedCnt >= 2*KeyPushTimes)//��⵽Pwr������
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
			//������λ���ڳ�ʱ�ļ����ͱ�־
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
			
				//��Դ�����µ�ʱ�򣬹ػ���ʱ�Ĵ���
				//����ʲô״̬��ֱ������
				CloseCnt = 0;

				//��Դ�����µ�ʱ�򣬱���Ĵ���
				//����� ���ģ��������
				//����ǹرյģ����´�
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
	//��ͣ״̬��ʼ��������ʱ�ػ�
	if(Status_MCU == Status_WorkStall)
	{
		//��Ҫ�ڵ�Դ������������µ�ʱ������
		CloseCnt++;
		if(CloseCnt >= TimeGainMin*AppPara.PowerOffDly)
			{
				//�ر�ϵͳ
				ShutDown();
			}
	}
	else
		CloseCnt = 0;
}

__inline void Blk_Control(void)
{
	AppPara.BackLightDelay = 10;
		//ϵͳ��ʼ����Ϻ󣬲�ʹ�ܱ���Ŀ���
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
			//��������µĴ���
			//��Դ�����µĴ���

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

	//��Enter()���غ�ʼ��ʱ����
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
	//�����⵽��ҳ�水��4����(+,-,*,/)
	//���û�п�ʼ���⣬��Ҫ��
	//��֮���趨һ��Dly ʱ��
	//Dlyʱ���ڣ���⵽��������������
	//Dlyʱ�䵽��
	//�������ģʽ�����㵥��������������һ�εĹ���ʱ�䵽ͣ��
	//���������ģʽ��ֱ�ӹر������ֹͣ����
 	if((MainScanFlag == 1) &&(MainScanControlEn == 0)){
		if(IsTrigMode(Trig_Internal))
		{
			//��ʼһ����ʱ�����������
			MainScanControlEn = 1;
			//��ʱ��������
			MainScanDlyCnt =0;
			switch(AppPara.PowerKey)
				{
					case PwrKey_Hit:
						//�����ǰ�ǵ���ģʽ���������û�д򿪣���ʼ����
						if(PwrHitFlag == PwrHit_STALL)
							{
							PwrHitFlag = PwrHit_WORK;
							StartToFlash();
							}
						break;
					case PwrKey_Press:
						//�����ǰ�ǳ���ģʽ������û�г�������ʼֱ������
						if(PwrKey_Status != PwrKey_Pressed)
							StartToFlash();
						break;
					default:
						break;
				}
				
			}
	}

	//�����ʼ��һ����ʱ����������̣���ʼ������ʱʱ��
	if(MainScanControlEn == 1) {
			MainScanDlyCnt++;
			if(MainScanDlyCnt >= MainScanDlyTime){
				//��ʱʱ�䵽��,�������
				MainScanDlyCnt = 0;
				//һ����ʱ�������
				MainScanControlEn = 0;
				MainScanFlag = 0;
				//��ֹMain���������Ҫ�˳���ʱ���ֽ������Զ�ɨ��ģʽ�����ܹر�
				if((EnterScanFlag != 1)&&(ScanDlyCounting != 1)) {
					//�Ե�ǰ�Ĳ�ͬ����ģʽ���д���
					switch(AppPara.PowerKey)
					{
						case PwrKey_Hit:
							//����ģʽ�����㵥��������һֱ��ʱ��һ�ι���ʱ��
							PwrHitCnt = 0;
							break;
						case PwrKey_Press:
							//����ģʽ�����û�г�������ֹͣ
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

//���������ʱ����Ҫִ�еĲ���
__inline void AnyKeyPressed_Control(void)
{
	//�����˰��������
	if(AnyKeyPressedFlag == 1) {
		AnyKeyPressedFlag = 0;
		//�ֶ���Ƶ����ʱ����Ҫ����
		MainScanDlyCnt = 0;
		//�ػ���ʱ����������ʲô״̬��ֱ������
		CloseCnt = 0;

		//��������µ�ʱ�򱳹�Ĵ���
		if(BlkStatus == BLK_ON) 
			{BlkCtrlCnt = 0;}
		else {
			//��������ѱ���
			BlkStatus = BLK_ON;
			BackLightOn();
			BlkCtrlCnt = 0;
		}

	}

	//2016.1.8
	//��Ϊ�޸��˰�����û����KeyRepeatNum
	//û��ע��MainScanDlyCnt = 0;
	//�����޸���Ƶ�ʺ���ͣ������������
		
	//�����������Ҳ����
	//if(KeyRepeatNum > 0)
		{
			//�ֶ���Ƶģʽ����ʱ����Ҳ���㣬�����������
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
		 	
		
		 SEGGER_RTT_Write(0,&tmp,1);//����
		 SEGGER_RTT_Write(0,&ch,1);//����
		 SEGGER_RTT_Write(0,&tmp,1);//����
		 for(i=0;i<8*8;i++){
			 for(j=0;j<128/8;j++){
				 U8ToHEXStr(*p++, buf);
				 SEGGER_RTT_Write(0, buf, 2);
			 }
			 SEGGER_RTT_Write(0,&tmp,1);//����
		 }
		 SEGGER_RTT_Write(0,&tmp,1);//����
		 SEGGER_RTT_Write(0,&tmp,1);//����
		 SEGGER_RTT_Write(0,&tmp,1);//����
		 SEGGER_RTT_Write(0,&tmp,1);//����
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

		//�ⴥ����ʱ�򣬶�������ⲿ��Ƶ��
		//�ں��ʵĵط�������ʵ�ʵ��������Ƶ��
		//ͳһΪAvgHz_Out
		tmp = AvgHz_Out;
		
		/*
		switch(AppPara.TrigMode)
			{
				case Trig_SinglePulse:
					//��Ϊ�ⲿƵ�ʸ��ˣ�����з�Ƶ
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
