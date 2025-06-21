#ifndef _MAIN_PAGE_HPP_
#define _MAIN_PAGE_HPP_

#include "GUI_Page.hpp"
#include "MenuPage.hpp"
#include "GUI_Speed.hpp"

namespace gui {

class CMainPage : public GUI_Page
{
private:  
    CMainPage();   //���캯����˽�е�  
    CMainPage(const CMainPage &);  
    CMainPage & operator = (const CMainPage &);	
	
	void OnKeyMode();
	
	
	inline void PageInit_Internal();
	inline void PageInit_SinglePulse();
	inline void PageInit_Perimeter();
	inline void PageInit_Gear();
	inline void PageInit_Encoder();
	void ShowRunIcon(int mode);				///<��ʾ����״̬��ͼ��
	void ShowBatteryIcon(int level);		///<��ʾ���״̬��ͼ��
	void ShowChargeIcon(bool enable=true);	///<��ʾ���״̬��ͼ��
	void ShowTrigModeIcon();				///<��ʾ����ģʽ��ͼ��	
public:
	void OnMeasureMode();
	/*
	enum{
		iSpeed,
		//������
		iDelay = 1,
		//�ܳ��곤
		iPerimeter = 1,
		iPrintLength,
		//����
		iGear = 1,
		iGearDelay,
		//������
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

	//���ص��麯��
	virtual int Loop();
	virtual void Init();
	virtual void Show();
	virtual void OnIdle();
	virtual void OnValChange();
	GUI_Speed & SpeedCtrl;

	static CMainPage & GetInstance()  
    {  
        static CMainPage instance;   //�ֲ���̬����  
        return instance;  
    } 
};

}//namespace gui {
#endif//_MAIN_PAGE_HPP_
