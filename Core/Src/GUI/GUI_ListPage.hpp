#ifndef _GUI_LIST_PAGE_HPP_
#define _GUI_LIST_PAGE_HPP_

#include "GUI_page.hpp"
namespace gui {
class GUI_ListPage : public GUI_Page
{
public:
	GUI_ListPage(GUI_List &L);
	enum{
		MaxItemNum = 3
	};

	GUI_Page * bakPage;
	int Val[MaxItemNum];
	int ItemNum;
	const int *ValList;
	u8 Pop_x, Pop_y, Pop_w, Pop_h;
	u8 Inner_x;
	int StartId,EndId,CurrId;
	void GetVisibleItem();
	void NextItem(int dir);
	virtual void Init();
	virtual void Show();
	virtual int Loop();
};

}
#endif
