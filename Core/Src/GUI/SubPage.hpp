#ifndef _SUB_PAGE_HPP_
#define _SUB_PAGE_HPP_

#include "GUI_Page.hpp"

namespace gui {
/**
ͨ����ҳ���򰴼�����һ�¹鵽һ������
**/
class SubPage : public GUI_Page
{
public:
	///���캯��������GUI_Page(int max_num)
	SubPage(int max_num);
	
	///����ֵ����
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
	//virtual void OnIdle(); //ʹ��ȱʡ�ģ�δ����
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
	//virtual void OnIdle(); //ʹ��ȱʡ�ģ�δ����
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
	//virtual void OnIdle();//ʹ��ȱʡ�ģ�δ����
//	virtual void OnValChange();
};

class AboutPage : public SubPage
{
public:
	AboutPage();

private:	
	//�ؼ����
	enum{
		iModel,
//		iManufacture,
		iAppVersion,
		iBootVersion,
		iContact
	};
	u32 TScroll;///<������ʾ��ϵ��ʽ�ļ�ʱ��
	char ModelStr[LcmXPixel/8+1];
	char VersionStr[LcmXPixel/8+1];
	void GetModelStr();	///<�����ͺŵ��ַ���
	int ContactId;
public:	
	virtual void Init();
	virtual void OnIdle();//������������ʾ
	virtual void Show();
};

}
#endif
