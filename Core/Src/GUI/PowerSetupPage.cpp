#include "SubPage.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "ctrl.h"
#include "SysParaCommon.h"

const char * const PowerKeyStrList_Ch[2]={
	"���ž��� �ɿ���ͣ",
	"��һ���� ��һ��ͣ"
};

const char * const PowerKeyStrList_En[2]={
	"Press Key Always",
	"Press Key Once"
};

namespace gui {
PowerSetupPage::PowerSetupPage()
: SubPage(ObjNum)
{
	if(IsLanguageCh()) {
		ObjList.Append(new GUI_Bool(120,16,&AppPara.PowerKey, (const char **)PowerKeyStrList_Ch));
		ObjList.Append(new GUI_Num(&AppPara.WorkTime, AppParaMin.WorkTime, AppParaMax.WorkTime,3));
		ObjList.Append(new GUI_Num(&AppPara.PowerOffDly, AppParaMin.PowerOffDly, AppParaMax.PowerOffDly,3));
		ObjList[iPowerKey]->SetPos(0,0);
		ObjList[iWorkTime]->SetPos(2*DEFAULT_HANZI_WIDTH,16);
		ObjList[iPowerOffDelay]->SetPos(2*DEFAULT_HANZI_WIDTH,32);

	}
	else {
		ObjList.Append(new GUI_Bool(128,16,&AppPara.PowerKey, (const char **)PowerKeyStrList_En));
		ObjList.Append(new GUI_Num(&AppPara.WorkTime, AppParaMin.WorkTime, AppParaMax.WorkTime,3));
		ObjList.Append(new GUI_Num(&AppPara.PowerOffDly, AppParaMin.PowerOffDly, AppParaMax.PowerOffDly,3));
		ObjList[iPowerKey]->SetPos(0,0);
		ObjList[iWorkTime]->SetPos(11*DEFAULT_ASCII_WIDTH,16);
		ObjList[iPowerOffDelay]->SetPos(11*DEFAULT_ASCII_WIDTH,32);
	}

	SetFocus(0,false);
	SetLineWrap(true);
	ObjList[iPowerKey]->ClearBeforeShow= false;
	ObjList[iPowerKey]->OnOk = OnOkPowerKey;
	ObjList[iWorkTime]->OnOk = OnOkWorkTime;
	ObjList[iPowerOffDelay]->OnOk = OnOkPowerOffDly;
	ObjList[iWorkTime]->Align = AlignMid;
	ObjList[iPowerOffDelay]->Align = AlignMid;	
}
		
void PowerSetupPage::Init()
{
	pCurrPage = (GUI_Page * )this;
}


void PowerSetupPage::Show()
{
	//����
	Clear();
	//��ʾ��̬�ı�
	if(IsLanguageCh()) {
		//DispStr8(8*DEFAULT_HANZI_WIDTH+7,0,",");
		DispStr8(0,16,"����   ���Ӻ���ͣ");
		DispStr8(0,32,"��ͣ   ���Ӻ�ػ�");
	}
	else {
		//DispStr8(8*DEFAULT_HANZI_WIDTH+7,0,",");
		DispStr8(0,16,"Stop After:    Min");
		DispStr8(0,32,"Off  After:    Min");
	}
	//��ʾ�ؼ�
	GUI_Page::Show();
	//ˢ�µ�Lcd
	Update();
}


}//namespace gui {
