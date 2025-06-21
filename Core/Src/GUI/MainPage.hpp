#ifndef _MAIN_PAGE_HPP_
#define _MAIN_PAGE_HPP_

#include "GUI_Page.hpp"
#include "MenuPage.hpp"
#include "GUI_Speed.hpp"

namespace gui {

class CMainPage : public GUI_Page
{
private:  
    CMainPage();   //构造函数是私有的  
    CMainPage(const CMainPage &);  
    CMainPage & operator = (const CMainPage &);	
	
	void OnKeyMode();
	
	
	inline void PageInit_Internal();
	inline void PageInit_SinglePulse();
	inline void PageInit_Perimeter();
	inline void PageInit_Gear();
	inline void PageInit_Encoder();
	void ShowRunIcon(int mode);				///<显示运行状态的图标
	void ShowBatteryIcon(int level);		///<显示电池状态的图标
	void ShowChargeIcon(bool enable=true);	///<显示充电状态的图标
	void ShowTrigModeIcon();				///<显示触发模式的图标	
public:
	void OnMeasureMode();
	/*
	enum{
		iSpeed,
		//单脉冲
		iDelay = 1,
		//周长标长
		iPerimeter = 1,
		iPrintLength,
		//齿轮
		iGear = 1,
		iGearDelay,
		//编码器
		iEncoder = 1,
		iEncoderDelay
	};
	*/
		enum{
		iSpeed = 0,
		//RF
		iRf_Ch = 1,
		//
		iImage_Num = 2

	};

	//重载的虚函数
	virtual int Loop();
	virtual void Init();
	virtual void Show();
	virtual void OnIdle();
	virtual void OnValChange();
	GUI_Speed & SpeedCtrl;

	static CMainPage & GetInstance()  
    {  
        static CMainPage instance;   //局部静态变量  
        return instance;  
    } 
};

}//namespace gui {
#endif//_MAIN_PAGE_HPP_
