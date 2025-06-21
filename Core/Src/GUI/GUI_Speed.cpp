#include "GUI_Speed.hpp"
#include "ScanSpeedPage.hpp"
#include "timer.h"
#include "AppInterface.hpp"
#include "MainPage.hpp"
#include "ctrl.h"
#include "key.h"
#include "SysParaCommon.h"

namespace gui {

#define SPEED_WIDTH 	80
#define SPEED_HEIHT		23
#define SPEED_X			36
#define SPEED_Y 		9

#define FREQ_DECIMAL	2
#define RPM_DECIMAL		1
#define MPMIN_DECIMAL	1

u16 Steps[]={
    10,20,30,40,50,60,70,80,90,100,120,140,180,200,150,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000
};


GUI_Speed::GUI_Speed()
: GUI_Num(NULL,0,0,6,0, &Song24_ASCII) 
, DispBuf(MainDispBuf)
{
	x = SPEED_X;
	y = SPEED_Y;
	OnSetSpeedUnit();
	DispBuf.SetFontASCII(Font);
}

inline void GUI_Speed::SetTo_Freq()
{
	if(IsTrigMode(Trig_Internal)){
		pVal = &AppPara.LampFreq;
		Enable = 1;
	}else{
		pVal = &ExternalSpeed;
		Enable = 0;
	}
	
	Min = AppParaMin.LampFreq;
//	if(!IsWKS100(WKS100_XI)){
		Max = AppParaMax.LampFreq;
//	}else{
//		Max = MAX_FREQ_TUBE*100;
//	}
	DigitalNum = 6;
	Decimal = FREQ_DECIMAL;
	OnChange = OnChangeFreq;
	SetDefaultWidth();
	
}

inline void GUI_Speed::SetTo_rpm(){
	if(IsTrigMode(Trig_Internal)){
		pVal = &AppPara.Rpm;
		Enable = 1;
	}else {
		pVal = &ExternalSpeed;
		Enable = 0;
	}
	Min = AppParaMin.Rpm;
//	if(!IsWKS100(WKS100_XI)){
		Max = AppParaMax.Rpm;
//	}else{
//		Max = MAX_FREQ_TUBE*600;
//	}
	DigitalNum = 7;
	Decimal = RPM_DECIMAL;
	OnChange = OnChangeRpm;
	SetDefaultWidth();
}
inline void GUI_Speed::SetTo_mpmin(){
	if(IsTrigMode(Trig_Internal)){
		pVal = &AppPara.LineSpeed;
		Enable = 1;
	} else {
		pVal = &ExternalSpeed;
		Enable = 0;
	}
	Min = AppParaMin.LineSpeed;
//	if(!IsWKS100(WKS100_XI)){
		Max = AppParaMax.LineSpeed;
//	}else{
//		Max = MAX_FREQ_TUBE*300;
//	}
	DigitalNum = 6;
	Decimal = MPMIN_DECIMAL;	
	OnChange = OnChangeLineSpeed;
	SetDefaultWidth();
}

void GUI_Speed::OnSetSpeedUnit()
{
	switch(AppPara.SpeedUnit){
		case Unit_Hz:  SetTo_Freq(); break;
		case Unit_rpm:  SetTo_rpm(); break;
		case Unit_mpmin:  SetTo_mpmin(); break;
	}
}

void GUI_Speed::_OnKeyLongUpDown(int key)
{
    if (key == KEY_UP_LONG) {
        OnStep(Steps[StepId]*StepList[0]);
    }else{
        OnStep(-Steps[StepId]*StepList[0]);
    }
    Display();
	Update();
    if (StepId < ARRAY_SIZE(Steps)-1) {
        StepId++;
    }
}


///重载了OnEditPage()，替换缺省数值编辑界面为扫频界面
int GUI_Speed::OnEditPage()
{

	ScanSpeedPage & NumPage = ScanSpeedPage::GetInstance();
	NumPage.Init();
	NumPage.Show();
	
	return NumPage.Loop();	
}

void GUI_Speed::Loop(int key)
{
    StepId = 0;
    _OnKeyLongUpDown(key);
    CMainPage & MainPage= CMainPage::GetInstance();
    while (1) {
        if(IsTimeOut_ms(MainPage.TKey,50)){
			wdg();
			MainPage.GetKeyAndKnob();
			if(MainPage.Key != KEY_NULL){
                switch (MainPage.Key) {
                case KEY_UP_LONG_RELEASE:
                case KEY_DOWN_LONG_RELEASE:
                    //长按释放时直接返回
                    return;
                case KEY_UP_LONG:
                case KEY_DOWN_LONG:
                     _OnKeyLongUpDown(key);
                     break;
                default://不明按键退出
                    return;
                }
            }

	}
}
		}


}
