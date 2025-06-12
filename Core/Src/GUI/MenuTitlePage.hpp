#ifndef _MENU_TITLE_PAGE_HPP_
#define _MENU_TITLE_PAGE_HPP_

#include "GUI_Page.hpp"
#include "key.h"
#include "AppParaCommon.h"

namespace gui {
/**
菜单页面的基类。顶部的Menu部分由MainPage画好，只绘框线内部
**/
class MenuTitlePage : public GUI_Page
{
private:  
    MenuTitlePage();   //构造函数是私有的  
    MenuTitlePage(const MenuTitlePage &);  
    MenuTitlePage & operator = (const MenuTitlePage &);  
	
	void ShowMenuPageBox(const int i);
	void LayoutMenu(int i);
	
public:  
	enum {
		iPulseWidth_Led,
		iMeasureMenu,
		iSpeedUnit_Led,
		iOption_Led,
		iBaterry_Led,
		
		MenuNum_Led
	};	

	int MenuNum;

	void SelectMenu(int i);
	
    static MenuTitlePage & GetInstance()  
    {  
        static MenuTitlePage instance;   //局部静态变量  
        return instance;  
    }  
	
};  
}//namespace gui {
#endif

