#include "MenuTitlePage.hpp"
#include "Icons.hpp"
#include "SysPara.h"

namespace gui {
	
///菜单项之间的间隔，以像素为单位
#define MENU_SPACE 		6
#define BOX_TOP_HEIGH	16
//箭头图标的宽度
#define ARROW_WIDTH 	4	
	
///菜单文字-中文
const char * MenuName_Cn_Led[MenuTitlePage::MenuNum_Led]={
	"亮度",
#ifdef LAYSER
	"测频",
#endif
	"单位",
	"选项",
	"电池",
//	"帮助"
};

///菜单文字-英文
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


///构造函数，是按单实例类做的，只会调用一次
MenuTitlePage::MenuTitlePage() 
: GUI_Page(MenuNum_Led) , MenuNum(MenuNum_Led)
{	//创建各控件，暂时为空
	for(int i = 0; i< MenuNum; i++){
			ObjList.Append(new GUI_Text(8,16,NULL));
		}
}

///给MenuPage调用的显示菜单项的函数
void MenuTitlePage::SelectMenu(int i)
{
	static int LastLang = -1;

	//根据语言选择菜单标题
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




///跟据当前选中菜单项设置显示哪些菜单和布局
void MenuTitlePage::LayoutMenu(const int i)
{
	//先按不选中
	int x=0;
	int j;
	int MenuStart,MenuEnd;//<当前显示的起止菜单项
	if(i==0 || i==1 || MenuNum <=3){//在开头，从左边排起，至少会显示两项,不超过3项时全部显示
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
	}else if(i==MenuNum-1 || i==MenuNum-2){//在未尾，从右边排起，至少会显示两项
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
	}else{//在中间，从当前项左边那个排起，至少会显示两项
		MenuStart = i-1;
		x+=ARROW_WIDTH;//留出左箭头的位置
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

		//居中显示，计算总宽度
		u8 w = 0;
		for(int j = MenuStart; j<=MenuEnd; j++){
			w+= MENU_SPACE + ObjList[j]->Width;
		}
		w += 2*MENU_SPACE + 2*ARROW_WIDTH;
		//算算可以移多少
		x=(LcmXPixel-w)/2;
		for(int j = MenuStart; j<=MenuEnd; j++){
			ObjList[j]->x+=x;
		}		
	}

	if(MenuStart!=0){//画左箭头
		DispBmp8(0,0, Icon_More[0]);
	}
	//画菜单项
	GUI_Page::Show(MenuStart,MenuEnd);
	//画右箭头
	if(MenuEnd!=MenuNum-1){
		DispBmp8(LcmXPixel-4,0, Icon_More[1]);
	}
}


///画菜单页面的外框
void MenuTitlePage::ShowMenuPageBox(int i)
{
	const GUI_Object *p = ObjList[i];
	u8 x1 = p->x-MENU_SPACE;
	u8 w = p->Width+MENU_SPACE*2;
	u8 x2 = x1+ w -1;
	
	LineH(x1,0,w);//顶部横线
	Update();
	LineV(x1,0,BOX_TOP_HEIGH);		//左侧竖线
	Update();
	LineV(x2,0,BOX_TOP_HEIGH);//右侧竖线
	Update();
	if(x1!=0){//
		LineH(0,BOX_TOP_HEIGH-1,x1+1);
	}
	Update();
	LineH(x2,BOX_TOP_HEIGH-1,(LcmXPixel-x2));//
	Update();
	LineH(0,LcmYPixel-1,LcmXPixel);//底线
	Update();
	LineV(0,BOX_TOP_HEIGH-1,LcmYPixel-BOX_TOP_HEIGH);
	Update();
	LineV(LcmXPixel-1,BOX_TOP_HEIGH-1,LcmYPixel-BOX_TOP_HEIGH);
	Update();
        return;
}

}//namespace gui {
