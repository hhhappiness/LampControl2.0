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

///本模块定义在GUI中调用的应用程序接口，大多是参数表值的变化后执行的操作

void OnSysIdle(void)
{

}

///主页空闲操作(与显示无关的部分)
void OnIdleMainPage(u32 &TReg)
{
		if(AdcFlag == 1)
		{
			AdcFlag = 0;
			BatLevel = GetBatLevel();
		}
}

///菜单页面空闲操作(与显示无关的部分)
void OnIdleMenuPage(u32 &TReg)
{
}

///子页面空闲操作(与显示无关的部分)
void OnIdleSubPage(u32 &TReg)
{
	if(IsTimeOut_ms(TReg,100)){
		TReg = GetTimerCount();
		wdg();
	}
}

///编辑页面空闲操作(与显示无关的部分)
void OnIdleEditPage(u32 &TReg)
{

}

/**
List控件是确认立即修改了参数值，需要在在OnChangeXXX()函数里做相应动作
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
数值编译控件的响应有两种:
一种是即时生效，用bool OnChangeXXX(int new_val)这类函数，赋给GUI_Num控件的成员OnChange函数指针，每次加减乘除都会调用一次。
一种是编辑完确认后才生效，用bool OnOkXXX(int new_val)这类函数，赋给GUI_Num控件的成员OnOk函数指针，只在确认退出编辑界面时调用一次。
这两个函数指针只能用一个。
new_val是控件编辑时得到的新值。
控件里只做了最小值、最大值的检查，没有直接更改绑定的值，因为有些值不能随便改
在这里要进一步做有效性检查，无效则返回false。有效则赋新值给对应参数，执行一定动作，再返回true;
**/
bool OnChangeContrast(int new_val)
{
	if(LcdSetContrast(new_val)){
		AppPara.Contrast = new_val;
		return true;
	}else
		return false;
}


//所有的频率均向Hz看齐
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
		//清除长按的状态标记
		PwrPress_FlagON = 0;
		PwrPress_FlagOFF = 0;
		//开始工作
		if(PwrHitFlag == PwrHit_STALL)
		{
			PwrHitFlag = PwrHit_WORK; //可以开始工作，并且开始计时
			WorkEn = 1;
			PwrHitCnt = 0;
			StartToFlash();
			
		}
	}
	else
		{
			//清除单击模式的标志
			PwrHitFlag = PwrHit_STALL; //可以开始工作，并且开始计时
			WorkEn = 0;
			PwrHitCnt = 0;
			//根据当前的电源键的状态是否打开闪光
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
				//重新置位用于超时的计数和标志
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

//todo 加上换算
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
///更改单脉冲的触发延时
bool OnChange_Delay_SinglePulse(int new_val)
{
	AppPara.Delay_SinglePulse = new_val;
	TrigAngle = AngleCov(AppPara.Delay_SinglePulse);
	return true;
}


///更改周长标长
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

	//更新频率
	AppPara.LineSpeed =( old_speed * new_val + old_len/2)/old_len;
	LineSpeed2Clocks();
	AppPara.Rpm =( (AppPara.LineSpeed*10 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
	AppPara.LampFreq = Rpm2Hz(AppPara.Rpm);
	return true;
}














