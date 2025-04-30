#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"

namespace gui {
class ScanSpeedPage : public GUI_Page
{

private:  
    ScanSpeedPage();   //���캯����˽�е�  

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

	u32 Step;	///<����ֵ���԰�GUI_Num��StepֵΪ��ֵ��������仯
	GUI_Num::Mode_t AutoMode;
	int HoldRepeatKey;
	int AutoTimes;
	int ReleaseTimes;
	u32 TAutoMode;	//�Զ�ģʽ�ļ�ʱ��
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
        static ScanSpeedPage instance;   //�ֲ���̬����  
        return instance;  
    } 	
	virtual void Init();
	void UnInit();
	virtual void Show();
	virtual int Loop();
};


}

#endif
