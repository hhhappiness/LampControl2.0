#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"



namespace gui {
class ScanAdcPage : public GUI_Page
{

public:  
    ScanAdcPage();   //构造函数是私有的  

	GUI_Page * bakPage;
	GUI_Speed &SpeedCtrl;
	GUI_Progress* Progress;
	GUI_NumText* Freq[4];

	enum{
		iProgress,
		iFreq1,
		iFreq2,
		iFreq3,
		iFreq4,
		MaxObjNum
	};

	int Val;
	int BakX,BakY;

	
	void OnStep(int step);
	void StopScan();
	void StartScan();
	virtual void Init();
	void UnInit();
	void ShowResults(int* freqs);
	virtual void Show();
	virtual int Loop();
};


}

#endif
