#include "ScanSpeedPage.hpp"
#include "Icons.hpp"
#include "GUI_Speed.hpp"
#include "ctrl.h"

namespace gui {
	
#define DIGITAL_X	8
#define DIGITAL_Y	48
	

#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)
	
GUI_Progress Progress(LcmXPixel,16);
	
///���캯��
ScanSpeedPage::ScanSpeedPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, SpeedCtrl(GUI_Speed::GetInstance())
{
	//ȱʡ��һ���ؼ�������ɾ��
	ObjList.Append(&SpeedCtrl);
	ObjList.Append(&Progress);
	
}


///��ʼ�������ݵ�ԭPageָ�룬�����˳�ʱ�ָ���ʾ
void ScanSpeedPage::Init()
{
	bakPage = pCurrPage;
	pCurrPage = this;
	//�ٶȿؼ�����
	//����ԭ����
	BakX = SpeedCtrl.x;
	BakY = SpeedCtrl.y;
	//��������
	SpeedCtrl.Font = &Song16_ASCII_10p;
	SpeedCtrl.Height = 16;
	//����������
	SpeedCtrl.SetDefaultWidth();
	SpeedCtrl.SetPos(SpeedCtrl.x,DIGITAL_Y);

	//����������
	Progress.SetPos(0,24);
	Progress.SetRange(SpeedCtrl.Min, SpeedCtrl.Max);
	Progress.SetValue(*SpeedCtrl.pVal);
	Step = SpeedCtrl.StepList[2];
	AutoMode = GUI_Num::Mode_None;
}

void ScanSpeedPage::UnInit()
{
	//�˳�ǰ�ָ�pCurrPageָ��
	pCurrPage = bakPage;
	//�ָ��ٶȿؼ�������
	SpeedCtrl.SetPos(BakX, BakY);
	//�ָ�����
	SpeedCtrl.Font = &DEFAULT_SPEED_FONT;
	SpeedCtrl.Height = 24;
	pCurrPage->Update();//�ָ��ڸǲ��ֵ���ʾ		
	ClearKey();//�������ĳ�����
}

///��ʾһ����򣬸���ťͼ�꣬����ʾ�ؼ�
void ScanSpeedPage::Show()
{
	Clear();
	//Rectangle(0, 0, LcmXPixel, LcmYPixel);
	//��ʾ���ڱ���
	if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-4*DEFAULT_HANZI_WIDTH)/2,0,"�Զ�ɨƵ");
	}else{
		DispStr8((LcmXPixel-9*DEFAULT_ASCII_WIDTH)/2,0,"Auto Scan");
	}
	
	//��ʾ�ؼ�
	GUI_Page::Show();
	//��ʾ��λ
	DispStr8(SpeedCtrl.x + SpeedCtrl.Font->Width*SpeedCtrl.DigitalNum,SpeedCtrl.y,SpeedUnitStr[AppPara.SpeedUnit]);	
	Update();
}

#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///����ѭ�������Ҽ��ƶ���꣬ȷ��ѡ���˳�
int ScanSpeedPage::Loop()
{
	HoldRepeatKey = 0;
	TQuit = 0;
	ResetTimeOut(TKey);
	ResetTimeOut(TAutoMode);
	AutoTimes = 0;
	ReleaseTimes = 0;
	
	while(1){
		if(IsTimeOut_ms(TKey,KEY_PERIOD)){
			ResetTimeOut(TKey);
			wdg();
			TKey = GetTimerCount();
			Key = GetKey();
			if(Key!=KEY_NULL){
				TQuit = 0;
				if(Key !=KEY_DOWN_LONG && Key != KEY_UP_LONG){
					HoldRepeatKey = 0;
				}
				switch(Key){				
				case KEY_UP_SHOT : 
					switch(AutoMode){
						case GUI_Num::Mode_None:
							StartAutoPlus();
							break;
						case GUI_Num::Mode_AutoPlus://
							break;
						case GUI_Num::Mode_AutoMinus:
							StartAutoPlus();
							break;
					}				
					break;
			
				case KEY_DOWN_SHOT : 
					switch(AutoMode){
						case GUI_Num::Mode_None:
							StartAutoMinus();
							break;
						case GUI_Num::Mode_AutoPlus:
							StartAutoMinus();
							break;
						case GUI_Num::Mode_AutoMinus:
							break;
					}					
					break;						
				case KEY_MULT_SHOT : 	
					StopScan();
					OnStep(*SpeedCtrl.pVal);
					break;	
				case KEY_DIV2_SHOT : 	
					StopScan();
					OnStep(-(*SpeedCtrl.pVal)/2);
					break;			
				case KEY_ENTER_SHOT:
					UnInit();
					return Val;
				default:
					break;
				}

			}else{//no key
				if(AutoMode==GUI_Num::Mode_None){
					TQuit++;
					if(TQuit == NO_KEY_QUIT/KEY_PERIOD) {
						UnInit();
						return Val;	
					}
				}else{
					TQuit = 0;
				}
			}
		}
		if(AutoMode!=GUI_Num::Mode_None){
			if(IsTimeOut_ms(TAutoMode,500)){
				ResetTimeOut(TAutoMode);
				switch(AutoMode){
					case GUI_Num::Mode_AutoPlus:
						AutoPlus();
						break;
					case GUI_Num::Mode_AutoMinus:
						AutoMinus();
						break;
				}
			}		
		}
		
	}
}

void ScanSpeedPage::StopScan()
{
	AutoMode = GUI_Num::Mode_None;
//	Step =0;
	AutoTimes = 0;
}

///�����Զ���
void ScanSpeedPage::StartAutoPlus()
{
	AutoMode = GUI_Num::Mode_AutoPlus;
//	if(FromNone) 	ChangeStep(1);//�ӵ����л�������1��
	OnStep(Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanSpeedPage::AutoPlus()
{
	OnStep(Step);//����һ��
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
//		ChangeStep(1);
		AutoTimes = 0;
	}
}
void ScanSpeedPage::AutoMinus()
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
void ScanSpeedPage::StartAutoMinus()
{
	AutoMode = GUI_Num::Mode_AutoMinus;
	OnStep(-Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void ScanSpeedPage::OnStep(int step)
{
	int bak = *SpeedCtrl.pVal;
	SpeedCtrl.OnStep(step);
	if(bak != *SpeedCtrl.pVal){
		SpeedCtrl.Display();
		SpeedCtrl.Update();		
		Progress.SetValue(*SpeedCtrl.pVal);
	}else{//˵��ֵ�Ѿ�����߽�
		//StopScan();
	}
}

}//namespace gui {
