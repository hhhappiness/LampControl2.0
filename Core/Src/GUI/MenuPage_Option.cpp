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
	ObjList.Clear();//���³�ʼ��ʱ���ԭ�пؼ�
	if(IsLanguageCh()) {
		if (!SecretMode) {
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"��������"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"ϵͳ����"));
			ObjList.Append(new GUI_TextButton(ITEM_WIDTH,16,"�汾��Ϣ"));
		}else{
            ObjList.Append(new GUI_TextButton(ITEM_WIDTH, 16, "���̲���"));
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
	FocusId = -1;//���µ���Init()����FoucusId����iSystemSetup�ϣ�������һ��SetFocusûִ��
	SetFocus(iPowerSetup,false);		
}

void MenuPage_Option::Show()
{
	//�Ȼ�����ҳ����ص�
	MenuPage::Show();
	pCurrPage = (GUI_Page * )this;
	GUI_Page::Show();//�ٻ��ؼ�
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
		case iAbout:		//�½�AboutPage
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
	//��ʾ��ʱ�����ѭ��
	wdg();
	p->Init();
	p->Show();
	wdg();
	p->Loop();
	wdg();
	if(p!=NULL) delete p; //ɾ��ҳ�� 
	//�ָ���ҳ��
	if(BakLang != AppPara.Language){//������Է����仯
		ObjList.Clear();//���ԭ���԰汾�Ŀؼ�
		Init();//���³�ʼ��
		wdg();
	}
	Show();
	wdg();
}
}//namespace gui {
