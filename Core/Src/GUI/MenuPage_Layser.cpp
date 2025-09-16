#include "MenuPage.hpp"
#include "GUI_Object.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "timer.h"
extern int thresholdFactor;
namespace gui {
const char *AlgorithmStr_Cn[AlgNum]={
	"FFT"
	// "FR ¬À≤®",
};

const char *AlgorithmStr_En[AlgNum]={
	"FFT"
	// "FR Filter",
};

MenuPage_Layser::MenuPage_Layser()
: MenuPage(MaxObjNum)
{
	if(AppPara.Language == Lang_Chinese)
	{
		ObjList.Append(new GUI_List(48,16,&AppPara.Algorithm,AlgNum,AlgTypeList,AlgorithmStr_Cn));
		ObjList.Append(new GUI_Num(&thresholdFactor, 1, 10,2));
	}
	else
		ObjList.Append(new GUI_List(72,16,&AppPara.Algorithm,AlgNum,AlgTypeList,AlgorithmStr_En));
	ObjList[iAlgorithm]->SetPos(2+2*16+2,16);
}

void MenuPage_Layser::Init()
{
	pCurrPage = this;	
	SetFocus(iAlgorithm,false);
	ObjList[iThreshold]->SetPos(32,32);
}

void MenuPage_Layser::Show()
{
	MenuPage::Show();
	
	SetFontASCII();
	if(AppPara.Language == Lang_Chinese) {
        DispStr8(2,16,"À„∑®:");
		DispStr8(2,32,"„–÷µ:");
	}
	else {
		DispStr8(2,16,"Alg:  ");
	}
	GUI_Page::Show();
	Update();
}

void MenuPage_Layser::OnValChange()
{
	switch(FocusId){
		case iAlgorithm:
			OnChangeAlgorithm();
			Show();
			break;
		default:
			break;
	}
}

void MenuPage_Layser::OnIdle() {
	if(IsTimeOut_ms(TIdle,500)){  
		TIdle = GetTimerCount();
		ResetTimeOut(TIdle);
        GUI_Page::Show();
	
    }
}
}//namespace gui {
