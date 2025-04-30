#include "SubPage.hpp"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "AppInterface.hpp"
#include "ctrl_common.h"

namespace gui {
	
static const char * const LanguageStrList[LangNum]={
	"��������",
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
	//����
	Clear();
	//��ʾ��̬�ı�
	if(AppPara.Language == Lang_Chinese){
		DispStr8(0,0, "��  ��:");
		DispStr8(0,16,"�Աȶ�:");
		
		ObjList[iLanguage]->SetPos(7*7+2,0);
		ObjList[iContrast]->SetPos(7*7+2,16);

		
	}else{
		DispStr8(0,0, "Language:");
		DispStr8(0,16,"Contrast:");		
		ObjList[iLanguage]->SetPos(7*9+2,0);
		ObjList[iContrast]->SetPos(7*9+2,16);

	}
	//��ʾ�ؼ�
	GUI_Page::Show();
	//ˢ�µ�Lcd
	Update();
	
}

void SystemSetupPage::OnValChange()
{
	switch(FocusId){
		case iLanguage:
			Show();//������ʾ
			break;
		case iContrast:
			//Todo:OnChangeContrast();
			break;
		default:
			break;
	}
}

}
