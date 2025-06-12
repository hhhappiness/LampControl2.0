#ifndef _MENU_TITLE_PAGE_HPP_
#define _MENU_TITLE_PAGE_HPP_

#include "GUI_Page.hpp"
#include "key.h"
#include "AppParaCommon.h"

namespace gui {
/**
�˵�ҳ��Ļ��ࡣ������Menu������MainPage���ã�ֻ������ڲ�
**/
class MenuTitlePage : public GUI_Page
{
private:  
    MenuTitlePage();   //���캯����˽�е�  
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
        static MenuTitlePage instance;   //�ֲ���̬����  
        return instance;  
    }  
	
};  
}//namespace gui {
#endif

