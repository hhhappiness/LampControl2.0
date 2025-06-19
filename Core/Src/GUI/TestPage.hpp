#ifndef _GUI_TEST_HPP_
#define _GUI_TEST_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"

namespace gui {
class ScanAdcPage : public GUI_Page
{

public:  
    ScanAdcPage();   //构造函数是私有的  

	GUI_Page * bakPage;

	enum{
		iValue,
		iProgress,
		MaxObjNum
	};

	GUI_Speed &SpeedCtrl;
	virtual void Init();
	 void UnInit();
	virtual void Show();
	// virtual int Loop();

};


}

#endif
