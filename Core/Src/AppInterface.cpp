#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "SysParaCommon.h"

#include "stm32.h"
#include "bsp.h"
#include "rtc_int.h"
#include "lcd.h"
#include "rtc.h"
#include "spimaster.h"
#include "AppParaCommon.h"
#include "board_config.h"
#include "ctrl.h"
#include "adc.h"
#include "timer.h"

///��ģ�鶨����GUI�е��õ�Ӧ�ó���ӿڣ�����ǲ�����ֵ�ı仯��ִ�еĲ���

void OnSysIdle(void)
{

}

///��ҳ���в���(����ʾ�޹صĲ���)
void OnIdleMainPage(u32 &TReg)
{
		if(AdcFlag == 1)
		{
			AdcFlag = 0;
			BatLevel = GetBatLevel();
		}
}

///�˵�ҳ����в���(����ʾ�޹صĲ���)
void OnIdleMenuPage(u32 &TReg)
{
}

///��ҳ����в���(����ʾ�޹صĲ���)
void OnIdleSubPage(u32 &TReg)
{
	if(IsTimeOut_ms(TReg,100)){
		TReg = GetTimerCount();
		wdg();
	}
}

///�༭ҳ����в���(����ʾ�޹صĲ���)
void OnIdleEditPage(u32 &TReg)
{

}

/**
List�ؼ���ȷ�������޸��˲���ֵ����Ҫ����OnChangeXXX()����������Ӧ����
**/
void OnChangePulseWidth_Auto(void)
{
	
}

void OnChangePulseWidth_LED(void)
{
	
}

void OnChangeTrigMode(void)
//void OnChangeTrigMode(int new_val)
{
		//AppPara.TrigMode = new_val ;
			if(AppPara.PowerKey == PwrKey_Hit) 
			StartToFlash();
}

/**
��ֵ����ؼ�����Ӧ������:
һ���Ǽ�ʱ��Ч����bool OnChangeXXX(int new_val)���ຯ��������GUI_Num�ؼ��ĳ�ԱOnChange����ָ�룬ÿ�μӼ��˳��������һ�Ρ�
һ���Ǳ༭��ȷ�Ϻ����Ч����bool OnOkXXX(int new_val)���ຯ��������GUI_Num�ؼ��ĳ�ԱOnOk����ָ�룬ֻ��ȷ���˳��༭����ʱ����һ�Ρ�
����������ָ��ֻ����һ����
new_val�ǿؼ��༭ʱ�õ�����ֵ��
�ؼ���ֻ������Сֵ�����ֵ�ļ�飬û��ֱ�Ӹ��İ󶨵�ֵ����Ϊ��Щֵ��������
������Ҫ��һ������Ч�Լ�飬��Ч�򷵻�false����Ч����ֵ����Ӧ������ִ��һ���������ٷ���true;
**/
bool OnChangeContrast(int new_val)
{
	if(LcdSetContrast(new_val)){
		AppPara.Contrast = new_val;
		return true;
	}else
		return false;
}


//���е�Ƶ�ʾ���Hz����
bool OnChangeFreq(int new_val)
{
	AppPara.LampFreq = new_val;
	NextTrigClks = TIM2_Hz2clk(AppPara.LampFreq);	
	UpdateVout();
	Updata_OutPusle();
	AppPara.Rpm = Hz2Rpm(AppPara.LampFreq);

	AppPara.LineSpeed =((uint64_t)( AppPara.LampFreq * AppPara.PlateLen*6 + 500))/1000;
	if( AppPara.LineSpeed > 9999)
		AppPara.LineSpeed = 9999 ;
	return true;
}

bool OnChangeRpm(int new_val)
{
	AppPara.Rpm = new_val;
	NextTrigClks = TIM2_rpm2clk(AppPara.Rpm);
	UpdateVout();
	//conver to hz
	AppPara.LampFreq = Rpm2Hz(AppPara.Rpm);

	AppPara.LineSpeed =((uint64_t)( AppPara.LampFreq * AppPara.PlateLen*6 + 500))/1000;
	if( AppPara.LineSpeed > 9999)
		AppPara.LineSpeed = 9999 ;

	Updata_OutPusle();
	return true;	
}

bool OnChangeLineSpeed(int new_val)
{
	
	AppPara.LineSpeed = new_val;
	LineSpeed2Clocks();
	AppPara.Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
	AppPara.LampFreq = Rpm2Hz(AppPara.Rpm);
	Updata_OutPusle();
	return true;
}

bool OnChangePulseWidth_LED(int new_val)
{
	AppPara.PulseWidth_LED = new_val;
	Updata_OutPusle();
	//Todo:
	return true;
}

bool OnChangeMaxPower(int new_val)
{
	AppPara.MaxPower= new_val;
	max_strobe_power = 100*AppPara.MaxPower ;
	Updata_OutPusle();
	//Todo:
	return true;
}


bool OnOkPlateLen(int new_val)
{
	AppPara.PlateLen = new_val;
	LineSpeed2Clocks();
	
	return true;
}

bool OnOkImagePerPlate(int new_val)
{
	AppPara.ImagePerPlate = new_val;
	image_num = AppPara.ImagePerPlate ;

	
	
	return true;
}

bool OnOkPowerKey(int new_val)
{
	AppPara.PowerKey = new_val;
	
	if(AppPara.PowerKey == PwrKey_Hit) {
		//���������״̬���
		PwrPress_FlagON = 0;
		PwrPress_FlagOFF = 0;
		//��ʼ����
		if(PwrHitFlag == PwrHit_STALL)
		{
			PwrHitFlag = PwrHit_WORK; //���Կ�ʼ���������ҿ�ʼ��ʱ
			WorkEn = 1;
			PwrHitCnt = 0;
			StartToFlash();
			
		}
	}
	else
		{
			//�������ģʽ�ı�־
			PwrHitFlag = PwrHit_STALL; //���Կ�ʼ���������ҿ�ʼ��ʱ
			WorkEn = 0;
			PwrHitCnt = 0;
			//���ݵ�ǰ�ĵ�Դ����״̬�Ƿ������
			if(POWER_PRESSED)
			{
				PwrPress_FlagOFF = 0;
				if(PwrPress_FlagON == 0)
					{
						PwrPress_FlagON = 1;
						StartToFlash();
					}
			}
			else 
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
	return true;
}

bool OnOkWorkTime(int new_val)
{
	AppPara.WorkTime = new_val;
	//todo
	return true;
}
bool OnOkPowerOffDly(int new_val)
{
	AppPara.PowerOffDly = new_val;
	//todo
	return true;
}

//todo ���ϻ���
bool OnChangeSpeedUnit(void)
{
	switch(AppPara.SpeedUnit){
		case Unit_Hz:
			NextTrigClks = TIM2_Hz2clk(AppPara.LampFreq);	
			UpdateVout();
			break;
		case Unit_rpm:
			NextTrigClks = TIM2_rpm2clk(AppPara.Rpm);
			UpdateVout();
			break;
		case Unit_mpmin:
			LineSpeed2Clocks();
			UpdateVout();
			break;
	}
	return true;
}

bool OnChangeAlgorithm(void){
	return true;
}
///���ĵ�����Ĵ�����ʱ
bool OnChange_Delay_SinglePulse(int new_val)
{
	AppPara.Delay_SinglePulse = new_val;
	TrigAngle = AngleCov(AppPara.Delay_SinglePulse);
	return true;
}


///�����ܳ��곤
bool OnChange_Perimeter(int new_val)
{
	AppPara.Perimeter= new_val;
	//TrigAngle = AngleCov(AppPara.Delay_SinglePulse);
	return true;
}


bool OnChange_PrintLength(int new_val)
{
	uint32_t old_len = AppPara.PrintLength;
	uint32_t old_speed = AppPara.LineSpeed;
	AppPara.PrintLength = new_val;

	//����Ƶ��
	AppPara.LineSpeed =( old_speed * new_val + old_len/2)/old_len;
	LineSpeed2Clocks();
	AppPara.Rpm =( (AppPara.LineSpeed*10 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
	AppPara.LampFreq = Rpm2Hz(AppPara.Rpm);
	return true;
}














