#include "MenuTitlePage.hpp"
#include "Icons.hpp"
#include "SysPara.h"

namespace gui {
	
///�˵���֮��ļ����������Ϊ��λ
#define MENU_SPACE 		6
#define BOX_TOP_HEIGH	16
//��ͷͼ��Ŀ��
#define ARROW_WIDTH 	4	
	
///�˵�����-����
const char * MenuName_Cn_Led[MenuTitlePage::MenuNum_Led]={
	"����",
#ifdef LAYSER
	"��Ƶ",
#endif
	"��λ",
	"ѡ��",
	"���",
//	"����"
};

///�˵�����-Ӣ��
const char * MenuName_En_Led[MenuTitlePage::MenuNum_Led]={
	"Lum",
#ifdef LAYSER
        "Layser",
#endif
	"Unit",
	"Option",
	"Battery",
//	"Help"
};


///���캯�����ǰ���ʵ�������ģ�ֻ�����һ��
MenuTitlePage::MenuTitlePage() 
: GUI_Page(MenuNum_Led) , MenuNum(MenuNum_Led)
{	//�������ؼ�����ʱΪ��
	for(int i = 0; i< MenuNum; i++){
			ObjList.Append(new GUI_Text(8,16,NULL));
		}
}

///��MenuPage���õ���ʾ�˵���ĺ���
void MenuTitlePage::SelectMenu(int i)
{
	static int LastLang = -1;

	//��������ѡ��˵�����
	if(LastLang != AppPara.Language){
		const char ** StrList;
		if(AppPara.Language == Lang_Chinese){
				StrList = MenuName_Cn_Led;
		}else{
				StrList = MenuName_En_Led;
		}
		GUI_Text * p;
		
		for(int i = 0; i< MenuNum; i++){
			p = (GUI_Text *)ObjList[i];
			p->Str = StrList[i];
			p->Width = GetStrPixWidth(StrList[i]);
		}	
		
			
		LastLang = AppPara.Language;
	}
	
	
	LayoutMenu(i);
	
	ShowMenuPageBox(i);
}




///���ݵ�ǰѡ�в˵���������ʾ��Щ�˵��Ͳ���
void MenuTitlePage::LayoutMenu(const int i)
{
	//�Ȱ���ѡ��
	int x=0;
	int j;
	int MenuStart,MenuEnd;//<��ǰ��ʾ����ֹ�˵���
	if(i==0 || i==1 || MenuNum <=3){//�ڿ�ͷ��������������ٻ���ʾ����,������3��ʱȫ����ʾ
		x=0;
		MenuStart =0;
		for(j=MenuStart;j<MenuNum;j++){
			MenuEnd = j;
			if(i==MenuEnd) x+=MENU_SPACE;
			ObjList[MenuEnd]->x = x;
			x += MENU_SPACE + ObjList[MenuEnd]->Width;
			if(i==MenuEnd) x+=MENU_SPACE;
			if(x>LcmXPixel-ARROW_WIDTH) {
				MenuEnd --;
				break;
			}
		}		
	}else if(i==MenuNum-1 || i==MenuNum-2){//��δβ�����ұ��������ٻ���ʾ����
		MenuEnd = MenuNum -1;
		x = LcmXPixel;
		for(j = MenuEnd; j>=0; j--){
			MenuStart = j;
			if(i== MenuStart) x-= MENU_SPACE;
			x-= ObjList[MenuStart]->Width;
			ObjList[MenuStart]->x = x;
			x-=MENU_SPACE;
			if(i== MenuStart) x-= MENU_SPACE;
			if(x<0){
				MenuStart++;
				break;
			}
		}
	}else{//���м䣬�ӵ�ǰ������Ǹ��������ٻ���ʾ����
		MenuStart = i-1;
		x+=ARROW_WIDTH;//�������ͷ��λ��
		for(j=MenuStart; j<=i+1; j++){
			MenuEnd = j;
			if(i==MenuEnd) x+=MENU_SPACE;
			x+=MENU_SPACE;
			ObjList[MenuEnd]->x = x;
			x+= ObjList[MenuEnd]->Width;
			if(i==MenuEnd) x+=MENU_SPACE;
			if(x>LcmXPixel-ARROW_WIDTH) {
				break;
			}
		}

		//������ʾ�������ܿ��
		u8 w = 0;
		for(int j = MenuStart; j<=MenuEnd; j++){
			w+= MENU_SPACE + ObjList[j]->Width;
		}
		w += 2*MENU_SPACE + 2*ARROW_WIDTH;
		//��������ƶ���
		x=(LcmXPixel-w)/2;
		for(int j = MenuStart; j<=MenuEnd; j++){
			ObjList[j]->x+=x;
		}		
	}

	if(MenuStart!=0){//�����ͷ
		DispBmp8(0,0, Icon_More[0]);
	}
	//���˵���
	GUI_Page::Show(MenuStart,MenuEnd);
	//���Ҽ�ͷ
	if(MenuEnd!=MenuNum-1){
		DispBmp8(LcmXPixel-4,0, Icon_More[1]);
	}
}


///���˵�ҳ������
void MenuTitlePage::ShowMenuPageBox(int i)
{
	const GUI_Object *p = ObjList[i];
	u8 x1 = p->x-MENU_SPACE;
	u8 w = p->Width+MENU_SPACE*2;
	u8 x2 = x1+ w -1;
	
	LineH(x1,0,w);//��������
	Update();
	LineV(x1,0,BOX_TOP_HEIGH);		//�������
	Update();
	LineV(x2,0,BOX_TOP_HEIGH);//�Ҳ�����
	Update();
	if(x1!=0){//
		LineH(0,BOX_TOP_HEIGH-1,x1+1);
	}
	Update();
	LineH(x2,BOX_TOP_HEIGH-1,(LcmXPixel-x2));//
	Update();
	LineH(0,LcmYPixel-1,LcmXPixel);//����
	Update();
	LineV(0,BOX_TOP_HEIGH-1,LcmYPixel-BOX_TOP_HEIGH);
	Update();
	LineV(LcmXPixel-1,BOX_TOP_HEIGH-1,LcmYPixel-BOX_TOP_HEIGH);
	Update();
        return;
}

}//namespace gui {
