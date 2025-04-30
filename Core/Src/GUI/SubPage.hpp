#ifndef _SUB_PAGE_HPP_
#define _SUB_PAGE_HPP_

#include "GUI_Page.hpp"

namespace gui {
/**
通用子页面因按键操作一致归到一个类里
**/
class SubPage : public GUI_Page
{
public:
	///构造函数，延用GUI_Page(int max_num)
	SubPage(int max_num);
	
	///返回值无用
	virtual int Loop();
	virtual void OnIdle();
	virtual void OnClose(int x){};
};

class SystemSetupPage : public SubPage
{
public:
	SystemSetupPage();
//	GUI_List Language;
//	GUI_Num Contrast;
	enum{
		iLanguage,
		iContrast,
		ObjNum
	};
	virtual void Init();
	virtual void Show();
	//virtual void OnIdle(); //使用缺省的，未重载
	virtual void OnValChange();
};

class EngineeringSetupPage : public SubPage
{
public:
	EngineeringSetupPage();

	enum{
		iLightType,
		MaxObjNum
	};
    bool NeedReboot;
	virtual void Init();
	virtual void Show();
    virtual void OnClose(int x);
	//virtual void OnIdle(); //使用缺省的，未重载
	virtual void OnValChange();
//    virtual void OnClick(int i);
};

class PowerSetupPage : public SubPage
{
public:
	PowerSetupPage();
	enum{
		iPowerKey,
		iWorkTime,
		iPowerOffDelay,
		iBackLightDelay,
		ObjNum
	};
	virtual void Init();
	virtual void Show();	
	//virtual void OnIdle();//使用缺省的，未重载
//	virtual void OnValChange();
};

class AboutPage : public SubPage
{
public:
	AboutPage();

private:	
	//控件标号
	enum{
		iModel,
//		iManufacture,
		iAppVersion,
		iBootVersion,
		iContact
	};
	u32 TScroll;///<滚动显示联系方式的计时器
	char ModelStr[LcmXPixel/8+1];
	char VersionStr[LcmXPixel/8+1];
	void GetModelStr();	///<生成型号的字符串
	int ContactId;
public:	
	virtual void Init();
	virtual void OnIdle();//重载做滚动显示
	virtual void Show();
};

}
#endif
