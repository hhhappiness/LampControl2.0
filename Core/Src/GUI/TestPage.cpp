#include "TestPage.hpp"
#include "Icons.hpp"
#include "GUI_Speed.hpp"
#include "ctrl.h"

namespace gui {
	
#define DIGITAL_X	8
#define DIGITAL_Y	48
	

#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)
	
GUI_Progress adc_Progress(LcmXPixel,16);
	
///���캯��
ScanAdcPage::ScanAdcPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, SpeedCtrl(GUI_Speed::GetInstance())
{
	//ȱʡ��һ���ؼ�������ɾ��
	ObjList.Append(&adc_Progress);
	
}


void ScanAdcPage::Init()
	// ����ǰҳ���浽bakPage��
{
	// ����ǰҳ����ΪScanAdcPage
	bakPage = pCurrPage;
	// ����adc_Progress��λ��Ϊ0�����Ϊ24
	pCurrPage = this;
	// ����adc_Progress�ķ�ΧΪSpeedCtrl.Min��SpeedCtrl.Max
	adc_Progress.SetPos(0,24);
	// ����adc_Progress��ֵΪSpeedCtrl.pVal
	adc_Progress.SetRange(SpeedCtrl.Min, SpeedCtrl.Max);
	adc_Progress.SetValue(*SpeedCtrl.pVal);

}

void ScanAdcPage::UnInit()
{
	//�˳�ǰ�ָ�pCurrPageָ��
	pCurrPage = bakPage;
	pCurrPage->Update();//�ָ��ڸǲ��ֵ���ʾ		
	ClearKey();//�������ĳ�����
}

///��ʾһ����򣬸���ťͼ�꣬����ʾ�ؼ�
void ScanAdcPage::Show()
{
	Clear();
	DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"���ڲɼ�����...");
	int x = 0;
	int y = 0;
	DispStr8(x,y,"���ڲɼ�����...");

		
        // DispStr8(2,DIGITAL_Y,"�㷨:");
}
#if 0
#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///����ѭ�������Ҽ��ƶ���꣬ȷ��ѡ���˳�
int ScanAdcPage::Loop()
{

}

void ScanAdcPage::StopScan()
{
	AutoMode = GUI_Num::Mode_None;
//	Step =0;
	AutoTimes = 0;
}

///�����Զ���
void ScanAdcPage::StartAutoPlus()
{
	AutoMode = GUI_Num::Mode_AutoPlus;
//	if(FromNone) 	ChangeStep(1);//�ӵ����л�������1��
	OnStep(Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanAdcPage::AutoPlus()
{
	OnStep(Step);//����һ��
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
//		ChangeStep(1);
		AutoTimes = 0;
	}
}
void ScanAdcPage::AutoMinus()
{
	OnStep(-Step);
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
//		ChangeStep(1);
		AutoTimes = 0;
	}	
}

///�����Զ���
void ScanAdcPage::StartAutoMinus()
{
	AutoMode = GUI_Num::Mode_AutoMinus;
	OnStep(-Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanAdcPage::OnStep(int step)
{
	int bak = *SpeedCtrl.pVal;
	SpeedCtrl.OnStep(step);
	if(bak != *SpeedCtrl.pVal){
		SpeedCtrl.Display();
		SpeedCtrl.Update();		
		adc_Progress.SetValue(*SpeedCtrl.pVal);
	}else{//˵��ֵ�Ѿ�����߽�
		//StopScan();
	}
}
#endif
}//namespace gui {
