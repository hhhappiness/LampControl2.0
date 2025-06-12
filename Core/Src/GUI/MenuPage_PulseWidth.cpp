#include "MenuPage.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "timer.h"
#include "ctrl.h"

/**
������ֻ��LED���У���������Led����������ȡ�
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
	//�Ȼ�����ҳ����ص�
	if(!SecretMode){
		MenuPage::Show();
		pCurrPage = this;
		
		SetFontASCII();
		if(IsLanguageCh()) {
			//��ʾ��̬�ı�
			//DispStr8(2,16,"�Զ�");
			DispStr8(2,16,"����:");
			//DispStr8(2,48,"���������Ȼ��������ȣ�Ҳ��������Ӱ"
		}
		else {
			//DispStr8(2,16,"Automatic:");
			DispStr8(2,16,"PulseWidth:");
		}
	}
	else {
			Clear();
			//��������
			DispStr8(2,0,"��������");
			DispStr8(2,16,"����:");
			SetFocus(iMaxPwr,true);
		}
	wdg();
	//��ʾ�ؼ�
	GUI_Page::Show();
	//ˢ�µ�Lcd
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
