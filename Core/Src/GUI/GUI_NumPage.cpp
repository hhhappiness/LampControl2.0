#include "GUI_NumPage.hpp"
#include "Icons.hpp"
#include "ctrl.h"

namespace gui {

#define DIGITAL_X	16
#define DIGITAL_Y	16
	
#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)

#define INV_PLUS	(1<<0)
#define INV_MINUS	(1<<1)

void GUI_NumPage::ChangeStep(int delta)
{
	int new_id = StepS32(StepId, delta, MinStepId, MaxStepId); 
	
	if(StepId != new_id){
		InvertDigital(CurrDigital);
		StepId = new_id;
		Step = pNumObj->StepList[new_id];
		CurrDigital = U32ToDigitalNum(Step);
		InvertDigital(CurrDigital);
	}
}

///���캯��
GUI_NumPage::GUI_NumPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, InvertState(0)
, InvertDigitalState(0)
{

}


///��ʼ�������ݵ�ԭPageָ�룬�����˳�ʱ�ָ���ʾ
void GUI_NumPage::Attach(GUI_Num *pN)
{
	bakPage = pCurrPage;
	pCurrPage = this;
	
	//������һ����ͬ�ģ�����Щ����Ҫ��
	pNumObj = new GUI_Num(*pN);
	//BakpVal = pNumObj->pVal;//�����ָ��
	Val = *(pNumObj->pVal);//�õ���ֵ
	if(pNumObj->OnChange == NULL){//�������ʱ��Ч
		pNumObj->pVal = &Val;//�󶨵��ڲ�����
	}
	pNumObj->DigitalNum = MaxDigitalNum;
	pNumObj->Font = &Song24_ASCII;
	pNumObj->Width =MaxDigitalNum * pNumObj->Font->WidthDefault+1;
	pNumObj->Height =  pNumObj->Font->Height;
	pNumObj->SelectMode = NotSelected;
	pNumObj->Align = AlignRight;
	
	ObjList.Append(pNumObj);
	ObjList[iValue]->SetPos(DIGITAL_X,DIGITAL_Y);

	MinStepId = 0;
	MaxStepId = pNumObj->StepNum-1;		
	StepId = MinStepId;
	Step = pNumObj->StepList[StepId];
	CurrDigital = U32ToDigitalNum(Step);
	
	InvertState = 0;
	InvertDigitalState = 0;
	AutoMode = GUI_Num::Mode_None;
}

void GUI_NumPage::UnInit()
{
	ObjList.Delete(iValue);
	pNumObj = NULL;
	
	pCurrPage = bakPage;//�˳�ǰ�ָ�pCurrPageָ��
	pCurrPage->Update();//�ָ��ڸǲ��ֵ���ʾ		
	ClearKey();//�������ĳ�����
}


///��ʾһ����򣬸���ťͼ�꣬����ʾ�ؼ�
void GUI_NumPage::Show()
{
	Clear();
	Rectangle(0, 0, LcmXPixel, LcmYPixel);

	DispBmp(UP_ARROW_X,DIGITAL_Y+8,Icon_Up);
	DispBmp(DOWN_ARROW_X,DIGITAL_Y+8,Icon_Down);	
	
	GUI_Page::Show();
	InvertDigital(CurrDigital);
	Update();
}

#define TOGGLE_STATE(x)  {if(InvertState & (x)) 	InvertState &= ~(x);else InvertState |= (x);}

void GUI_NumPage::InvertPlus()
{
	TOGGLE_STATE(INV_PLUS);
	Invert(UP_ARROW_X-1, DIGITAL_Y+8-4, Icon_Up.Width+2, Icon_Up.Height+8);
	Update(UP_ARROW_X-1, DIGITAL_Y+8-8, Icon_Up.Width+2, Icon_Up.Height+16);
}


void GUI_NumPage::InvertMinus()
{
	TOGGLE_STATE(INV_MINUS);
	Invert(DOWN_ARROW_X-1, DIGITAL_Y+8-4, Icon_Down.Width+2, Icon_Down.Height+8);
	Update(DOWN_ARROW_X-1, DIGITAL_Y+8-8, Icon_Down.Width+2, Icon_Down.Height+16);	
}

void GUI_NumPage::ClearInv()
{
	if(InvertState & INV_PLUS) 	InvertPlus();
	if(InvertState & INV_MINUS) InvertMinus();
}

#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///����ѭ�������Ҽ��ƶ���꣬ȷ��ѡ���˳�
int GUI_NumPage::Loop()
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
							InvertPlus();
							break;
						case GUI_Num::Mode_AutoPlus://
							break;
						case GUI_Num::Mode_AutoMinus:
							ClearInv();
							break;
					}				
					break;
				case KEY_UP_LONG : 
				
						switch(AutoMode){
							case GUI_Num::Mode_None://����ת�Զ�+
								StartAutoPlus(true);
								break;
							case GUI_Num::Mode_AutoPlus://
								AutoPlus();
								break;
							case GUI_Num::Mode_AutoMinus://�л����Զ���
								StartAutoPlus(false);
								break;
						}
					
					break;
				case KEY_UP_RELEASE : 	
					switch(AutoMode){
						case GUI_Num::Mode_None://������
							ClearInv();
							OnStep(Step);
							break;
						case GUI_Num::Mode_AutoPlus://��1
							StopScan();
							OnStep(Step);
							break;
						case GUI_Num::Mode_AutoMinus://��1
							StopScan();
							OnStep(-Step);
							break;
					}
					break;			
				case KEY_DOWN_SHOT : 
					switch(AutoMode){
						case GUI_Num::Mode_None:
							InvertMinus();
							break;
						case GUI_Num::Mode_AutoPlus:
							ClearInv();
							break;
						case GUI_Num::Mode_AutoMinus:
							break;
					}					
					break;					
				case KEY_DOWN_LONG : 	
									
						switch(AutoMode){
							case GUI_Num::Mode_None://��ʼ�Զ�
								StartAutoMinus(true);
								break;
							case GUI_Num::Mode_AutoPlus://�л����Զ���
								StartAutoMinus(false);
								break;
							case GUI_Num::Mode_AutoMinus:
								AutoMinus();
								break;
							
						}
				
					break;	
				case KEY_DOWN_RELEASE : 	
					switch(AutoMode){
						case GUI_Num::Mode_None://������
							ClearInv();
							OnStep(-Step);
							break;
						case GUI_Num::Mode_AutoPlus://��1��
							StopScan();
							OnStep(Step);
							break;
						case GUI_Num::Mode_AutoMinus://�ӵ���
							StopScan();
							OnStep(-Step);
							break;
					}
					break;			
				case KEY_MULT_SHOT : 	
					break;	
				case KEY_MULT_LONG : 
					
						OnStep(*pNumObj->pVal);
					
					break;	
				case KEY_MULT_RELEASE : 	
					OnStep(*pNumObj->pVal);
					break;	
				case KEY_DIV2_SHOT : 	
					break;	
				case KEY_DIV2_LONG : 
					
						OnStep(-(*pNumObj->pVal)/2);
					
					break;	
				case KEY_DIV2_RELEASE : 	
					OnStep(-(*pNumObj->pVal)/2);
					break;				
				case KEY_MODE_LONG:
				case KEY_MODE_RELEASE:
				case KEY_ENTER_RELEASE:
				case KEY_ENTER_LONG:
					UnInit();
					return Val;					
				}

			}else{//no key
				TQuit++;
				if(TQuit == NO_KEY_QUIT/KEY_PERIOD) {
					UnInit();
					return Val;	
				}
			}
		}
		if(AutoMode!=GUI_Num::Mode_None){
			if(IsTimeOut_ms(TAutoMode,1000)){
				if(Step>0){
					ChangeStep(-1);
					ResetTimeOut(TAutoMode);
				}else{
					StopScan();
				}
				
			}		
		}
		
	}
}

void GUI_NumPage::StopScan()
{
	ClearInv();
	AutoMode = GUI_Num::Mode_None;
	ChangeStep(0 - StepId);
	AutoTimes = 0;
}

///�����Զ���
void GUI_NumPage::StartAutoPlus(bool FromNone)
{
	ClearInv();
	InvertPlus();
	AutoMode = GUI_Num::Mode_AutoPlus;
	if(FromNone) 	ChangeStep(1);//�ӵ����л�������1��
	OnStep(Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void GUI_NumPage::AutoPlus()
{
	OnStep(Step);//����һ��
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
		ChangeStep(1);
		AutoTimes = 0;
	}
}
void GUI_NumPage::AutoMinus()
{
	OnStep(-Step);
	ResetTimeOut(TAutoMode);
	AutoTimes++;
	if(AutoTimes==5){
		ChangeStep(1);
		AutoTimes = 0;
	}	
}

///�����Զ���
void GUI_NumPage::StartAutoMinus(bool FromNone)
{
	ClearInv();
	InvertMinus();
	AutoMode = GUI_Num::Mode_AutoMinus;
	if(FromNone) 	ChangeStep(1);//�ӵ����л�������1��
	OnStep(-Step);//����һ��
	AutoTimes = 1;
	ResetTimeOut(TAutoMode);
}

void GUI_NumPage::OnStep(int step)
{
	int bak = *pNumObj->pVal;
	pNumObj->OnStep(step);
	if(bak != *pNumObj->pVal){
		pNumObj->Display();
		InvertDigital(CurrDigital);
		pNumObj->Update();		
	}else{//˵��ֵ�Ѿ�����߽�
		//StopScan();
	}
}


///i���ұ�������ʼ1
void GUI_NumPage::InvertDigital(u32 i)
{
	
	u8 x;
	if(pNumObj->Decimal>0){//�����С����
		if(i>MaxDigitalNum-1) return;
		if(i>pNumObj->Decimal){//С����֮ǰ
			x = DIGITAL_X + (7-i)*FontWidth;
		}else{//С����֮��
			x = DIGITAL_X + (8-i)*FontWidth;
		}
	}else{//û��С��
		if(i>MaxDigitalNum) return;
		x = DIGITAL_X + (8-i)*FontWidth;
	}
	x++;
	Invert8(x,DIGITAL_Y,FontWidth,24);
	Update(x,DIGITAL_Y,FontWidth,24);
	u32 mask =  (1<<i);
	if(InvertDigitalState & mask){
		InvertDigitalState &= ~mask;
	}else{
		InvertDigitalState |= mask;
	}
}


}//namespace gui {
