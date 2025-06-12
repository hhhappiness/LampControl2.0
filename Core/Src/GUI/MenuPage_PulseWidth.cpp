#include "MenuPage.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "timer.h"
#include "ctrl.h"

/**
本界面只在LED版有，用于设置Led闪光的脉冲宽度。
**/
namespace gui {
MenuPage_PulseWidth::MenuPage_PulseWidth()
: MenuPage(MaxObjNum)
{
	SecretKey = 0x55;
}


const u32 PulseWidthStepList[GUI_Num::DefaultStepNum]={
	100,500,1000
};



void MenuPage_PulseWidth::Init() {
	pCurrPage = (GUI_Page*)this;
	GUI_Num *p = NULL;
	ObjList.Delete(0);
	if(!SecretMode){
		ObjList.Append(new GUI_Num(&AppPara.PulseWidth_LED, AppParaMin.PulseWidth_LED, AppParaMax.PulseWidth_LED,3));
		if(IsLanguageCh()) {
			ObjList[iWidthSlider]	->SetPos(32,16);
		}
		else {
			ObjList[iWidthSlider]	->SetPos(11*DEFAULT_ASCII_WIDTH,16);
		}
		ObjList[iWidthSlider]->OnChange = OnChangePulseWidth_LED;
		SetFocus(0,false);
		}
	else {
		if(ObjList[iMaxPwr] == NULL) {
			p = new GUI_Num(&AppPara.MaxPower, AppParaMin.MaxPower, AppParaMax.MaxPower,5);
			ObjList.Set(p,iMaxPwr);
			p->StepList = PulseWidthStepList ;
			ObjList[iMaxPwr]->SetPos(32,16);
			ObjList[iMaxPwr]->OnChange = OnChangeMaxPower;
			
			SetFocus(0,false);
			}
		}
}

void MenuPage_PulseWidth::OnIdle() {
	if(IsTimeOut_ms(TIdle,500)){
		TIdle = GetTimerCount();
		ResetTimeOut(TIdle);
		if(!SecretMode){
			
			GUI_Page::Show();
		}else{
			}
		}
	
}

void MenuPage_PulseWidth::Show()
{
	//先画与主页面相关的
	if(!SecretMode){
		MenuPage::Show();
		pCurrPage = this;
		
		SetFontASCII();
		if(IsLanguageCh()) {
			//显示静态文本
			//DispStr8(2,16,"自动");
			DispStr8(2,16,"脉宽:");
			//DispStr8(2,48,"增加脉冲宽度会增加亮度，也会增加拖影"
		}
		else {
			//DispStr8(2,16,"Automatic:");
			DispStr8(2,16,"PulseWidth:");
		}
	}
	else {
			Clear();
			//设置脉宽
			DispStr8(2,0,"参数设置");
			DispStr8(2,16,"功率:");
			SetFocus(iMaxPwr,true);
		}
	wdg();
	//显示控件
	GUI_Page::Show();
	//刷新到Lcd
	Update();
	wdg();
}




void MenuPage_PulseWidth::OnValChange()
{
	switch(FocusId){

		/*
		case iWidthAuto:
			OnChangePulseWidth_Auto();
			break;
		*/	
		case iWidthSlider:
//			pMainPage->Init();
			Updata_OutPusle();
			Show();
			break;
		default:
			break;
	}
	
}
}//namespace gui {
