#include "MenuPage.hpp"
#include "GUI_Object.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "timer.h"

namespace gui {
const char *AlgorithmStr_Cn[AlgNum]={
	"FFT"
	// "FR 滤波",
};

const char *AlgorithmStr_En[AlgNum]={
	"FFT"
	// "FR Filter",
};

MenuPage_Layser::MenuPage_Layser()
: MenuPage(MaxObjNum)
{
	#if 0
	if(AppPara.Language == Lang_Chinese)
	{
		ObjList.Append(new GUI_List(48,16,&AppPara.Algorithm,AlgNum,AlgTypeList,AlgorithmStr_Cn));
	}
	else
	{
		ObjList.Append(new GUI_List(72,16,&AppPara.Algorithm,AlgNum,AlgTypeList,AlgorithmStr_En));
	}
	#endif
	ObjList.Append(new GUI_Num(&AppPara.Threshold, 1, 10,2));
	ObjList.Append(new GUI_Num(&AppPara.minLimit, 1, 200,3));
	ObjList.Append(new GUI_Num(&AppPara.maxLimit, 1, 200,3));
}

void MenuPage_Layser::Init()
{
	pCurrPage = this;	
	ObjList[iThreshold]->SetPos(32,16);
	#if 0
	ObjList[iAlgorithm]->SetPos(2+2*16+2,16);
	#endif
	ObjList[iMinLimit]->SetPos(60,32);
	ObjList[iMaxLimit]->SetPos(60,48);
	SetFocus(iThreshold,false);
}

void MenuPage_Layser::Show()
{
	MenuPage::Show();
	
	SetFontASCII();
	if(AppPara.Language == Lang_Chinese) {
		#if 0
        DispStr8(2,16,"算法:");
		#endif
		DispStr8(2,16,"阈值:");
		DispStr8(2,32,"最小频率:");
		DispStr8(2,48,"最大频率:");
	}
	else {
		#if 0
		DispStr8(2,16,"Alg:  ");
		#endif
		DispStr8(2,16,"Thr:  ");
		DispStr8(2,32,"MinFreq:");
		DispStr8(2,48,"MaxFreq:");
	}
	GUI_Page::Show();
	Update();
}

void MenuPage_Layser::OnValChange()
{
	switch(FocusId){	
		#if 0
		case iAlgorithm:
			OnChangeAlgorithm();
			Show();
			break;
		#endif
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
	MenuPage::OnIdle();
}
}//namespace gui {
