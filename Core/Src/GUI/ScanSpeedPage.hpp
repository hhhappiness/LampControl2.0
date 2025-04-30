#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"

namespace gui {
class ScanSpeedPage : public GUI_Page
{

private:  
    ScanSpeedPage();   //构造函数是私有的  

    ScanSpeedPage(const ScanSpeedPage &);  
    ScanSpeedPage & operator = (const ScanSpeedPage &);  

	GUI_Page * bakPage;

	enum{
		iValue,
		iProgress,
		MaxObjNum
	};

	GUI_Speed &SpeedCtrl;
	int Val;
	int BakX,BakY;

	u32 Step;	///<步进值，以绑定GUI_Num的Step值为初值，随操作变化
	GUI_Num::Mode_t AutoMode;
	int HoldRepeatKey;
	int AutoTimes;
	int ReleaseTimes;
	u32 TAutoMode;	//自动模式的计时器
	u32 TQuit; 
	
	void OnStep(int step);
	inline void ShowStepMark();
	void StopScan();
	void StartAutoPlus();
	void StartAutoMinus();
	void AutoPlus();
	void AutoMinus();

public:	
    static ScanSpeedPage & GetInstance()  
    {  
        static ScanSpeedPage instance;   //局部静态变量  
        return instance;  
    } 	
	virtual void Init();
	void UnInit();
	virtual void Show();
	virtual int Loop();
};


}

#endif
