#include "MenuPage.hpp"
#include "SubPage.hpp"
#include "AppParaCommon.h"
#include "ctrl.h"
namespace gui {
MenuPage_Option::MenuPage_Option()
: MenuPage(MaxObjNum)
{
	SecretKey = 0x55;
	
}

#define ITEM_WIDTH 	56
#define ITEM_X		((LcmXPixel-ITEM_WIDTH)/2)

void MenuPage_Option::Init()
{
	int i;
	ObjList.Clear();//重新初始化时清掉原有控件
	if(IsLanguageCh()) {
		if (!SecretMode) {
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"节能设置"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"系统设置"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"版本信息"));
		}else{
            ObjList.Append(new GUI_TextButton(ITEM_WIDTH, 16, "工程参数"));
        }
		for(i=0;i<ObjList.Num;i++){
			ObjList[i]->SetPos(ITEM_X,(i+1)*16);
			ObjList[i]->SetAligh(AlignMid);
			
		}
	}
	else {
		if (!SecretMode) {
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"Energy"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"System"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"Version"));
		}else{
            ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"Engineer"));
        }
		for(i=0;i<ObjList.Num;i++){
			ObjList[i]->SetPos(ITEM_X,(i+1)*16);
			ObjList[i]->SetAligh(AlignMid);
		}
	}
	FocusId = -1;//重新调用Init()可能FoucusId就在iSystemSetup上，导致下一条SetFocus没执行
	SetFocus(iPowerSetup,false);		
}

void MenuPage_Option::Show()
{
	//先画与主页面相关的
	MenuPage::Show();
	pCurrPage = (GUI_Page * )this;
	GUI_Page::Show();//再画控件
	Update();
	
}

void MenuPage_Option::OnClick(int i)
{
	GUI_Page * p=NULL;
	int BakLang = AppPara.Language;
	wdg();
	switch(i){
		case iPowerSetup:
			if (!SecretMode) {
				p = (GUI_Page *)(new PowerSetupPage);
			}else {
				p = (GUI_Page *)(new EngineeringSetupPage);
			}
			break;
		case iSystemSetup:
			p = (GUI_Page *)(new SystemSetupPage);
			break;			
		case iAbout:		//新建AboutPage
			p = (GUI_Page *)(new AboutPage);
			break;
//		case iSavePara:
//			SaveConfig();
//			break;
//		case iStandby:
//			EnterStandby();
//			break;
		default:
			return;
	}
	//显示并时入界面循环
	wdg();
	p->Init();
	p->Show();
	wdg();
	p->Loop();
	wdg();
	if(p!=NULL) delete p; //删除页面 
	//恢复本页面
	if(BakLang != AppPara.Language){//如果语言发生变化
		ObjList.Clear();//清掉原语言版本的控件
		Init();//重新初始化
		wdg();
	}
	Show();
	wdg();
}
}//namespace gui {
