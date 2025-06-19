#include "TestPage.hpp"
#include "Icons.hpp"
#include "GUI_Speed.hpp"
#include "ctrl.h"

namespace gui {
	
#define DIGITAL_X	8
#define DIGITAL_Y	48
	

#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)
	
GUI_Progress adc_Progress(LcmXPixel,16);
	
///构造函数
ScanAdcPage::ScanAdcPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, SpeedCtrl(GUI_Speed::GetInstance())
{
	//缺省第一个控件，不可删除
	ObjList.Append(&adc_Progress);
	
}


void ScanAdcPage::Init()
	// 将当前页保存到bakPage中
{
	// 将当前页设置为ScanAdcPage
	bakPage = pCurrPage;
	// 设置adc_Progress的位置为0，宽度为24
	pCurrPage = this;
	// 设置adc_Progress的范围为SpeedCtrl.Min到SpeedCtrl.Max
	adc_Progress.SetPos(0,24);
	// 设置adc_Progress的值为SpeedCtrl.pVal
	adc_Progress.SetRange(SpeedCtrl.Min, SpeedCtrl.Max);
	adc_Progress.SetValue(*SpeedCtrl.pVal);

}

void ScanAdcPage::UnInit()
{
	//退出前恢复pCurrPage指针
	pCurrPage = bakPage;
	pCurrPage->Update();//恢复遮盖部分的显示		
	ClearKey();//清掉后面的长按键
}

///显示一个外框，各按钮图标，再显示控件
void ScanAdcPage::Show()
{
	Clear();
	DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"正在采集数据...");
	int x = 0;
	int y = 0;
	DispStr8(x,y,"正在采集数据...");

		
        // DispStr8(2,DIGITAL_Y,"算法:");
}
#if 0
#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///按键循环，左右键移动光标，确定选中退出
int ScanAdcPage::Loop()
{

}

void ScanAdcPage::StopScan()
{
	AutoMode = GUI_Num::Mode_None;
//	Step =0;
	AutoTimes = 0;
}

///启动自动加
void ScanAdcPage::StartAutoPlus()
{
	AutoMode = GUI_Num::Mode_AutoPlus;
//	if(FromNone) 	ChangeStep(1);//从单击切换过来升1档
	OnStep(Step);//先走一步
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanAdcPage::AutoPlus()
{
	OnStep(Step);//先走一步
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
//		ChangeStep(1);
		AutoTimes = 0;
	}
}
void ScanAdcPage::AutoMinus()
{
	OnStep(-Step);
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
//		ChangeStep(1);
		AutoTimes = 0;
	}	
}

///启动自动减
void ScanAdcPage::StartAutoMinus()
{
	AutoMode = GUI_Num::Mode_AutoMinus;
	OnStep(-Step);//先走一步
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanAdcPage::OnStep(int step)
{
	int bak = *SpeedCtrl.pVal;
	SpeedCtrl.OnStep(step);
	if(bak != *SpeedCtrl.pVal){
		SpeedCtrl.Display();
		SpeedCtrl.Update();		
		adc_Progress.SetValue(*SpeedCtrl.pVal);
	}else{//说明值已经到达边界
		//StopScan();
	}
}
#endif
}//namespace gui {
