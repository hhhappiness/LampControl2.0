#include "MenuPage.hpp"
//#include "MainPage.hpp"
#include "AppInterface.hpp"
#include "ctrl.h"
#include "adc.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "SysPara.h"
#include "Icons.hpp"

namespace gui {

int MenuPage::CurrMenu;		///<当前选中菜单项
int MenuPage::LastMenu;		///<最后一次打开的菜单
	
MenuPage::MenuPage(int max_num) 
: GUI_Page(max_num)
, MenuTitle(MenuTitlePage::GetInstance())
, SecretKey(0x55AA)
, SecretMode(false)
{
	
}

int MenuPage::Loop()
{
	TKey = GetTimerCount();
	TIdle = GetTimerCount();
	KeySeq =0;
	while(1){
		if(IsTimeOut_ms(TKey,50)){
			TKey = GetTimerCount();
			Key = GetKey();
			wdg();
			if(Key != KEY_NULL){
				switch(Key){
					case KEY_DIV2_SHOT : 	OnClose(-1); return -1;
					case KEY_MULT_SHOT : 	OnClose(1); return 1;
					case KEY_UP_SHOT : 		
						if(!FocusPrevious()) return 0;
						CheckSecretMode(Key);
						break;
					case KEY_DOWN_SHOT : 	FocusNext(); CheckSecretMode(Key); break;
					case KEY_ENTER_SHOT : 
						OnEnter();
						CheckSecretMode(Key);
						break;
					
				}
			}
		}
		OnIdle();
	}
	
}

void MenuPage::Show()
{
	Clear();			//清屏
	MenuTitle.SelectMenu(CurrMenu);
	wdg();
}


void MenuPage::OnIdle()
{
	//如果超过200ms没有操作，则执行以下操作
	if(IsTimeOut_ms(TIdle,200)){
		//调用wdg()函数
		wdg();
		//调用OnIdleMenuPage()函数
		OnIdleMenuPage(TIdle);
		//更新TIdle的值
		TIdle = GetTimerCount();	
		//绘制底线
		LineH(0,LcmYPixel-1,LcmXPixel);//底线
		Update(0,LcmYPixel-8,LcmXPixel,8);//更新最下面的page
	}
}

///显示菜单页面。
int MenuPage::ShowMenuPage(int i)
{
	int Ret;
	MenuPage * pMenuPage;	///<菜单页面的实例指针
	//创建菜单页面的实例
	switch(i){
		case MenuTitlePage::iPulseWidth_Led: 	pMenuPage = (MenuPage * )(new MenuPage_PulseWidth); break;
		case MenuTitlePage::iMeasureMenu: 		pMenuPage = (MenuPage * )(new MenuPage_Layser); break;
		case MenuTitlePage::iSpeedUnit_Led: 	pMenuPage = (MenuPage * )(new MenuPage_SpeedUnit); break;
		case MenuTitlePage::iOption_Led: 		pMenuPage = (MenuPage * )(new MenuPage_Option); break;
		case MenuTitlePage::iBaterry_Led: 		pMenuPage = (MenuPage * )(new MenuPage_Battery); break;
		
	}
	//显示菜单页面内容
	pCurrPage = (GUI_Page * )pMenuPage;
	pMenuPage->Init();
	wdg();
	pMenuPage->Show();
	pMenuPage->Update();
	wdg();
	//调用菜单页面的主循环
	Ret = pMenuPage->Loop();
	wdg();
	//删除菜单页面对像
	delete pMenuPage;
	//切换当前页面
	
	return Ret;
}

int MenuPage::ShowMenuPage()
{
	int Ret;
//打开上次打开的菜单，如果没有就打开第1个
	CurrMenu = (LastMenu<0) ? 0 : LastMenu; 
	LastMenu = CurrMenu;
	Ret=ShowMenuPage(CurrMenu);///打开菜单，直到菜单页面退出，返回值表明是接着打开下一个菜单还是退出
	while(Ret!=0){
		if(Ret<0){//前一个菜单
			CurrMenu --;
			if(CurrMenu <0) CurrMenu = MenuTitlePage::MenuNum_Led-1;
			//if(CurrMenu <0) CurrMenu = 5-1;
		}else {//后一个菜单
			CurrMenu ++;
			if(CurrMenu >=MenuTitlePage::MenuNum_Led) CurrMenu = 0;	
			//if(CurrMenu >= 5) CurrMenu = 0;	
		}
		LastMenu = CurrMenu;
		Ret = ShowMenuPage(CurrMenu);
	}
	//关闭菜单，恢复主页面的显示
	CurrMenu = -1;
	return 0;
}

void MenuPage::CheckSecretMode(int key)
{
	if(KEY_UP_SHOT == key){
		 KeySeq<<=1;
	}else if(KEY_DOWN_SHOT == key){
		KeySeq<<=1;KeySeq |=1;
	}else{
		KeySeq = 0;
		return;
	}
	if(KeySeq == SecretKey){
		OnSecretMode();
	}
}

void MenuPage::OnSecretMode()
{
	SecretMode = true;
	Init();//重新初始化
	Show();//重新显示
}
}//namespace gui {
