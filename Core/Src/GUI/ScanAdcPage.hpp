#ifndef _GUI_SCAN_ADC_PAGE_HPP_
#define _GUI_SCAN_ADC_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"


namespace gui {
class ScanAdcPage : public GUI_Page
{

private:  
    ScanAdcPage();   //构造函数是私有的  

    ScanAdcPage(const ScanAdcPage &);  
    ScanAdcPage & operator = (const ScanAdcPage &);  

	GUI_Page * bakPage;

	enum{
		iProgress,
		iFreq2,
		iFreq3,
		iFreq4,
		MaxObjNum
	};

	GUI_Speed &SpeedCtrl;
	int Val;
	int BakX,BakY;

	int Step;	///步进值，控制进度条移动速度
	
	void OnStep(int step);
	void StopScan();
	void StartScan();

public:	
    static ScanAdcPage & GetInstance()  
    {  
        static ScanAdcPage instance;   //局部静态变量  
        return instance;  
    } 	
	virtual void Init();
	void UnInit();
	void ShowResults(int* freqs);
	virtual void Show();
	virtual int Loop();
};


}

#endif
