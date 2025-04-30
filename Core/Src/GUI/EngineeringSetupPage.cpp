#include "SubPage.hpp"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "AppInterface.hpp"
#include "ctrl_common.h"
#include "Reset.h"

namespace gui {
	
EngineeringSetupPage::EngineeringSetupPage()
: SubPage(MaxObjNum)
, NeedReboot(false)
{
	ObjList.Append(new GUI_List(64, 16, &SysPara.LightType, LIGHT_TYPE_NUM, LightTypeList, (const char **)LightType_StrList)); 

	ObjList[iLightType]->SetPos(64,0);
}
		
void EngineeringSetupPage::Init()
{
	pCurrPage = (GUI_Page * )this;
    SetFocus(iLightType, false);
}

void EngineeringSetupPage::Show()
{
	//清屏
	Clear();
	//显示静态文本

    DispStr8(0,0, " 型  号 :");
	
	//显示控件
	GUI_Page::Show();
	//刷新到Lcd
	Update();
	
}

void EngineeringSetupPage::OnValChange()
{
	switch(FocusId){
    case iLightType:
        //NeedSaveSysFlag = 1;
		SaveSysConfig();
        NeedReboot = true;
        break;
    default:
        break;
	}
}

// void EngineeringSetupPage::OnClick(int i)
// {
	// FLASH_Unlock(); /* Flash 解锁 */
	// /* ENABLE the ReadOut Protection */
	// FLASH_ReadOutProtection(ENABLE);
	// FLASH_Lock();
	// NeedReboot = true;

// }

void EngineeringSetupPage::OnClose(int x)
{
    if (NeedSaveFlag) {
        SaveConfig();
    }
    if (NeedSaveSysFlag) {
        SaveSysConfig();
    }
    if (NeedReboot) {
        Clear();
        DispStr8(0,0,"Rebooting...");
        Update();
		int i;
		for(i=0;i<10;i++){
			wdg(); Delay_ms(100);
		}
        wdg();
		ResetApp();

    }
}

}//namespace gui
