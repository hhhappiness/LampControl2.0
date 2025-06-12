#ifndef _MENU_PAGE_HPP_
#define _MENU_PAGE_HPP_

#include "GUI_Page.hpp"
#include "MenuTitlePage.hpp"

#include "key.h"
#include "AppParaCommon.h"

namespace gui {
	
/**
菜单页面的基类。顶部的Menu部分由MainPage画好，只绘框线内部
**/
class MenuPage : public GUI_Page
{
public:
	///构造函数，延用GUI_Page(int max_num)
	MenuPage(int max_num);
	
	///返回值，-1表示左边一个，0表示关闭，1表示右边一个Menu
	virtual int Loop();
	virtual void Show();
	virtual void OnIdle();

	virtual void OnClose(int x){}

	///菜单相关
	MenuTitlePage & MenuTitle;
	static int CurrMenu;		///<当前选中菜单项
	static int LastMenu;		///<最后一次打开的菜单		
	static int ShowMenuPage(int i);
	static int ShowMeasurePage();
	static int ShowMenuPage();

	//隐藏功能
	u16 KeySeq;			///<读入的按键序列，UP为0，down为1
	u16 SecretKey;		///<检测的按键序列值
	bool SecretMode;	///<判断是否进入了隐藏模式
	
	
	void CheckSecretMode(int key);
	virtual void OnSecretMode();
};

class MenuPage_Option : public MenuPage
{
public:
	MenuPage_Option();
	///控件索引
	enum{
		iPowerSetup,	
		iSystemSetup,
		iAbout,
//		iSavePara,
//		iStandby,
		MaxObjNum
	};
	virtual void Init();
	virtual void Show();
	virtual void OnClick(int i);
	
};


class MenuPage_PulseWidth : public MenuPage
{
public:
	MenuPage_PulseWidth();

	///控件索引
	enum{
		iWidthSlider = 0,	///<Slider控件
		iMaxPwr = 0,
		MaxObjNum
	};

	/*
	enum {
			iMaxPwr,
			MaxObjNum
		}
*/
	virtual void Show();
	virtual void Init();
	virtual void OnIdle();
	
	virtual void OnValChange();
};

class MenuPage_Layser : public MenuPage
{
public:
	MenuPage_Layser();

	///控件索引
	enum{
		iAlgorithm, 
		MaxObjNum
	};
	virtual void Show();
	virtual void Init();
	virtual void OnIdle();
	
	virtual void OnValChange();
};

class MenuPage_SpeedUnit : public MenuPage
{
public:
	MenuPage_SpeedUnit();
	enum{
		iSpeedUnit,
		
		iImagePerPlate,
		iPlateLen,
		MaxObjNum
	};
	int BakUnit;
	virtual void Show();
	virtual void Init();
	virtual void OnValChange();
};


///电池菜单页，正常时显示电压值和剩余可工作时间，进入隐藏模式可进入ADC校正
class MenuPage_Battery : public MenuPage
{
public:
	MenuPage_Battery();
	enum{
		iVoltage,
		
		iDuration=1,
		
		iADC=1,
		
		iAdVal1,
		iV1,
		
		iAdVal2,
		iV2,
		
		iBtnSave,
		iCoff,
		MaxObjNum
	};
	
	int Voltage;	///<当前计算电压值
	int Duration;	///<可用时间
	int AdcVal;		///<ADC采集值
	int AdVal1,AdVal2;
	int V1,V2;
	int bakACoff, bakCCoff;
	int AorC;	//系数显示不下，一次只显示一个
	int AdcFlag;	///<采集了几个ADC值, bit 0 adc1, bit 1 adc1;
	GUI_NumText *pObjCoff;
	
	virtual void Init();
	virtual void Show();
	virtual void OnIdle();
	virtual void OnClick(int i);
	virtual void OnValChange();
	virtual void OnClose(int x);
};
}//namespace gui {
#endif

