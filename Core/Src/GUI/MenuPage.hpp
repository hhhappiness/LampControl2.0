#ifndef _MENU_PAGE_HPP_
#define _MENU_PAGE_HPP_

#include "GUI_Page.hpp"
#include "MenuTitlePage.hpp"

#include "key.h"
#include "AppParaCommon.h"

namespace gui {
	
/**
�˵�ҳ��Ļ��ࡣ������Menu������MainPage���ã�ֻ������ڲ�
**/
class MenuPage : public GUI_Page
{
public:
	///���캯��������GUI_Page(int max_num)
	MenuPage(int max_num);
	
	///����ֵ��-1��ʾ���һ����0��ʾ�رգ�1��ʾ�ұ�һ��Menu
	virtual int Loop();
	virtual void Show();
	virtual void OnIdle();

	virtual void OnClose(int x){}

	///�˵����
	MenuTitlePage & MenuTitle;
	static int CurrMenu;		///<��ǰѡ�в˵���
	static int LastMenu;		///<���һ�δ򿪵Ĳ˵�		
	static int ShowMenuPage(int i);
	static int ShowMeasurePage();
	static int ShowMenuPage();

	//���ع���
	u16 KeySeq;			///<����İ������У�UPΪ0��downΪ1
	u16 SecretKey;		///<���İ�������ֵ
	bool SecretMode;	///<�ж��Ƿ����������ģʽ
	
	
	void CheckSecretMode(int key);
	virtual void OnSecretMode();
};

class MenuPage_Option : public MenuPage
{
public:
	MenuPage_Option();
	///�ؼ�����
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

	///�ؼ�����
	enum{
		iWidthSlider = 0,	///<Slider�ؼ�
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

	///�ؼ�����
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


///��ز˵�ҳ������ʱ��ʾ��ѹֵ��ʣ��ɹ���ʱ�䣬��������ģʽ�ɽ���ADCУ��
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
	
	int Voltage;	///<��ǰ�����ѹֵ
	int Duration;	///<����ʱ��
	int AdcVal;		///<ADC�ɼ�ֵ
	int AdVal1,AdVal2;
	int V1,V2;
	int bakACoff, bakCCoff;
	int AorC;	//ϵ����ʾ���£�һ��ֻ��ʾһ��
	int AdcFlag;	///<�ɼ��˼���ADCֵ, bit 0 adc1, bit 1 adc1;
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

