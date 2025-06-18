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

int MenuPage::CurrMenu;		///<��ǰѡ�в˵���
int MenuPage::LastMenu;		///<���һ�δ򿪵Ĳ˵�
	
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
	Clear();			//����
	MenuTitle.SelectMenu(CurrMenu);
	wdg();
}


void MenuPage::OnIdle()
{
	//�������200msû�в�������ִ�����²���
	if(IsTimeOut_ms(TIdle,200)){
		//����wdg()����
		wdg();
		//����OnIdleMenuPage()����
		OnIdleMenuPage(TIdle);
		//����TIdle��ֵ
		TIdle = GetTimerCount();	
		//���Ƶ���
		LineH(0,LcmYPixel-1,LcmXPixel);//����
		Update(0,LcmYPixel-8,LcmXPixel,8);//�����������page
	}
}

///��ʾ�˵�ҳ�档
int MenuPage::ShowMenuPage(int i)
{
	int Ret;
	MenuPage * pMenuPage;	///<�˵�ҳ���ʵ��ָ��
	//�����˵�ҳ���ʵ��
	switch(i){
		case MenuTitlePage::iPulseWidth_Led: 	pMenuPage = (MenuPage * )(new MenuPage_PulseWidth); break;
		case MenuTitlePage::iMeasureMenu: 		pMenuPage = (MenuPage * )(new MenuPage_Layser); break;
		case MenuTitlePage::iSpeedUnit_Led: 	pMenuPage = (MenuPage * )(new MenuPage_SpeedUnit); break;
		case MenuTitlePage::iOption_Led: 		pMenuPage = (MenuPage * )(new MenuPage_Option); break;
		case MenuTitlePage::iBaterry_Led: 		pMenuPage = (MenuPage * )(new MenuPage_Battery); break;
		
	}
	//��ʾ�˵�ҳ������
	pCurrPage = (GUI_Page * )pMenuPage;
	pMenuPage->Init();
	wdg();
	pMenuPage->Show();
	pMenuPage->Update();
	wdg();
	//���ò˵�ҳ�����ѭ��
	Ret = pMenuPage->Loop();
	wdg();
	//ɾ���˵�ҳ�����
	delete pMenuPage;
	//�л���ǰҳ��
	
	return Ret;
}

int MenuPage::ShowMenuPage()
{
	int Ret;
//���ϴδ򿪵Ĳ˵������û�оʹ򿪵�1��
	CurrMenu = (LastMenu<0) ? 0 : LastMenu; 
	LastMenu = CurrMenu;
	Ret=ShowMenuPage(CurrMenu);///�򿪲˵���ֱ���˵�ҳ���˳�������ֵ�����ǽ��Ŵ���һ���˵������˳�
	while(Ret!=0){
		if(Ret<0){//ǰһ���˵�
			CurrMenu --;
			if(CurrMenu <0) CurrMenu = MenuTitlePage::MenuNum_Led-1;
			//if(CurrMenu <0) CurrMenu = 5-1;
		}else {//��һ���˵�
			CurrMenu ++;
			if(CurrMenu >=MenuTitlePage::MenuNum_Led) CurrMenu = 0;	
			//if(CurrMenu >= 5) CurrMenu = 0;	
		}
		LastMenu = CurrMenu;
		Ret = ShowMenuPage(CurrMenu);
	}
	//�رղ˵����ָ���ҳ�����ʾ
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
	Init();//���³�ʼ��
	Show();//������ʾ
}
}//namespace gui {
