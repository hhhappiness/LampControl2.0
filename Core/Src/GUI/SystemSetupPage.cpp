#include "SubPage.hpp"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "AppInterface.hpp"
#include "ctrl_common.h"

namespace gui {
	
static const char * const LanguageStrList[LangNum]={
	"简体中文",
	"English",
};

SystemSetupPage::SystemSetupPage()
: SubPage(ObjNum)
{

	ObjList.Append(new GUI_List(56,16,&AppPara.Language, LangNum, LanguageList, (const char **) LanguageStrList));
	GUI_Num *pConstrast = new GUI_Num(&AppPara.Contrast,AppParaMin.Contrast,AppParaMax.Contrast, 2);
	ObjList.Append(pConstrast);
	
	//ObjList[iLanguage]->SetPos(38,0);
	//ObjList[iContrast]->SetPos(48,16);
	pConstrast->OnChange = OnChangeContrast;
	
	SetFocus(iLanguage);
}
		
void SystemSetupPage::Init()
{
	pCurrPage = (GUI_Page * )this;
}

void SystemSetupPage::Show()
{
	//清屏
	Clear();
	//显示静态文本
	if(AppPara.Language == Lang_Chinese){
		DispStr8(0,0, "语  言:");
		DispStr8(0,16,"对比度:");
		
		ObjList[iLanguage]->SetPos(7*7+2,0);
		ObjList[iContrast]->SetPos(7*7+2,16);

		
	}else{
		DispStr8(0,0, "Language:");
		DispStr8(0,16,"Contrast:");		
		ObjList[iLanguage]->SetPos(7*9+2,0);
		ObjList[iContrast]->SetPos(7*9+2,16);

	}
	//显示控件
	GUI_Page::Show();
	//刷新到Lcd
	Update();
	
}

void SystemSetupPage::OnValChange()
{
	switch(FocusId){
		case iLanguage:
			Show();//更新显示
			break;
		case iContrast:
			//Todo:OnChangeContrast();
			break;
		default:
			break;
	}
}

}
