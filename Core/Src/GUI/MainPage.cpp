#include "MainPage.hpp"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "Icons.hpp"
#include "key.h"
#include "MenuPage.hpp"
#include "GUI_Speed.hpp"
#include "adc.h"
#include "AppInterface.hpp"
#include "ctrl.h"
#include "rtc_int.h"
#include "timer.h"
#include "SysPara.h"
#include "encoder.h"
#include "flash.h"
#include "ScanAdcPage.hpp"

extern u32 time_on;
int encoder_val; //��ȡ�ı�����ֵ

namespace gui {
#define RUN_ICON_X (0)
#define RUN_CH_X (18)
#define BATTERY_ICON_X	(128-16-1-5)
#define CHARGE_ICON_X	(128-5)
	
const u8 SpeedX[SpeedUnitNum]={32,20,16};	///Speed�ؼ���x����
#define SpeedY 	28							///Speed�ؼ���y����
#define SPEED_STEP_NUM 	3	
///Speed�ؼ��Ĳ���ֵ�б�
const u32 SpeedStepList[SpeedUnitNum][SPEED_STEP_NUM]={
	{1,10,100},
	{1,10,100},
	{1,10,100},
};
const u32 KeepList[SPEED_STEP_NUM]={1,1,2};

CMainPage::CMainPage() 
: GUI_Page(4)
, SpeedCtrl(GUI_Speed::GetInstance())
{
	//ȱʡ��һ���ؼ�������ɾ��
	ObjList.Append(&SpeedCtrl);
}	
	

void CMainPage::OnMeasureMode()
{
	
	ScanAdcPage* MeasurePage = new ScanAdcPage;
	MeasurePage->Init();
	MeasurePage->Show();
	int Ret = MeasurePage->Loop();	 
	delete MeasurePage; //ɾ������ҳ�����
	//����ֵ��Ϊ�û�ѡ����㷨����ֵ��ֱ�ӽ�SpeedCtrl��ֵ����Ϊ��ֵ
	SpeedCtrl.OnSetVal(Ret*100); //����SpeedCtrl��ֵ
	Init(); //���³�ʼ��ҳ��
	Show();	//��ʾ
}

///MainPage��ѭ��
int CMainPage::Loop()
{
	
	TKey = GetTimerCount();
	TIdle = GetTimerCount();
	while(1){
		if(IsTimeOut_ms(TKey,50)){
			wdg();
			Key = GetKey();
			if(Key != KEY_NULL){
				if(IsTrigMode(Trig_Internal)){//�ڴ���ֻ��Ƶ��
					switch(Key){
						case KEY_POWER_LONG:
							OnMeasureMode();
							break;
						case KEY_ENCODER:
							MainScanFlag = 1;
							SpeedCtrl.OnStep(encoder_val);
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_ENTER_LONG:
							OnKeyMode();
							break;
						case KEY_DIV2_SHOT ://��2
							MainScanFlag = 1;
							SpeedCtrl.Div2();
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_MULT_SHOT ://��2
							MainScanFlag = 1;
							SpeedCtrl.Mul2();
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_UP_SHOT : 	
							MainScanFlag = 1;
							SpeedCtrl.OnStep(SpeedCtrl.StepList[0]);//��С����ֵ����
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_DOWN_SHOT : 
							MainScanFlag = 1;
							SpeedCtrl.OnStep(-SpeedCtrl.StepList[0]);//��С����ֵ��С
							//SpeedCtrl.Loop(Key); 
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_UP_LONG : 		
						
								//SpeedCtrl.OnStep(SpeedCtrl.StepList[1]);//��С�Զ�����ֵ����
								SpeedCtrl.Loop(Key); 
								SpeedCtrl.Display();
								SpeedCtrl.Update();
						
							break;
						case KEY_DOWN_LONG : 	
							
								//SpeedCtrl.OnStep(-SpeedCtrl.StepList[1]);//��С�Զ�����ֵ��С
								SpeedCtrl.Loop(Key); 
								SpeedCtrl.Display();
								SpeedCtrl.Update();
							
							break;		
						case KEY_ENTER_SHOT : 
							
								//����ɨƵ����
								
								EnterScanFlag = 1;
								ScanDlyFlag = 0;
								
								OnEnter();	
								
								EnterScanFlag = 0;
								ScanDlyCnt = 0;
								ScanDlyFlag = 1;
								ScanDlyCounting = 1;								
							
							break;
							
					}
				}else{//�ⴥ������ͨ����һ��
					GUI_Num * p = (GUI_Num *)ObjList[FocusId]; 
					//GUI_Num * p = NULL ;
					switch(Key){
						case KEY_UP_RELEASE:     //���ؼ����ⴥ����ʱ����
							OnKeyMode();
							break;
						case KEY_DIV2_SHOT:
							/*
							p->OnStep(-(*p->pVal/2));
							p->Display();
							p->Update();
							*/
							p =  (GUI_Num *)ObjList[iRf_Ch];
							p->OnStep(-1);
							p->Update();
							break;
						case KEY_DIV2_LONG:
							p =  (GUI_Num *)ObjList[iRf_Ch];
							p->OnStep(-5);
							p->Update();
							break;
						case KEY_MULT_SHOT:
							/*
							p->OnStep(*p->pVal);
							p->Display();
							p->Update();	
							*/
							p =  (GUI_Num *)ObjList[iRf_Ch];
							p->OnStep(1);
							p->Update();
							break;
						case KEY_MULT_LONG:
							p =  (GUI_Num *)ObjList[iRf_Ch];
							p->OnStep(5);
							p->Update();
							break;
						case KEY_UP_SHOT : 	
							/*
							p->OnStep(p->StepList[0]);
							p->Display();
							p->Update();	
							*/
							p =  (GUI_Num *)ObjList[iImage_Num];
							p->OnStep(1);
							p->Update();
							break;
						case KEY_DOWN_SHOT : 	
							/*
							p->OnStep(-p->StepList[0]);
							p->Display();
							p->Update();	
							*/
							p =  (GUI_Num *)ObjList[iImage_Num];
							p->OnStep(-1);
							p->Update();
							break;
						case KEY_UP_LONG : 		
								/*
								p->OnStep(p->StepList[1]);
								p->Display();
								p->Update();
								*/
								p =  (GUI_Num *)ObjList[iImage_Num];
							p->OnStep(5);
							p->Update();
							
							break;
						case KEY_DOWN_LONG : 	
								/*
								p->OnStep(-p->StepList[1]);
								p->Display();
								p->Update();
								*/
								p =  (GUI_Num *)ObjList[iImage_Num];
							p->OnStep(-5);
							p->Update();
							break;
						case KEY_ENTER_RELEASE : 
							//OnEnter();	
							break;
					}					
				}
			}
		}

		OnIdle();

	}
}

///��Mode������˵�
void CMainPage::OnKeyMode()
{
	MenuPage::ShowMenuPage();
	Init(); //���³�ʼ��ҳ��
	Show();	//��ʾ
}


///��ʼ��ҳ���е����ж��񣬰����˵����ؼ�
void CMainPage::Init()
{
	pCurrPage = (GUI_Page * )this;
	Clear();	//����
	
	switch(AppPara.TrigMode){
		case Trig_Internal:		PageInit_Internal();	break;
	}
	SpeedCtrl.OnSetSpeedUnit();//���ݵ�λ������ʾ
}


///��ʾMainPage
void CMainPage::Show()
{
	//��ʾ��λ
	int i=AppPara.SpeedUnit;
	int x = LcmXPixel-Song_Width16_ASCII.Width*6-11*2; //6������+2����λ
	SpeedCtrl.SetPos(x/2,SpeedY);	//����λ��
	SpeedCtrl.SetStepList(SpeedStepList[i], KeepList, SPEED_STEP_NUM);//���ò����б�
	DispStr8Font(x/2 + Song_Width16_ASCII.Width*6, SpeedY+8, SpeedUnitStr[i], 0, &Song_ASCII_Special);//��ʾ��λ
	Update();
	//�ڴ�����ʾ������ʾ
	if(IsTrigMode(Trig_Internal)){
		if(IsLanguageCh()) {
			// DispStr8(0,48,"�� �Զ�");
			//if(IsTrigMode(Trig_Internal)) DispStr8(0,48,"M ����");
			}
		else {
			// DispStr8(10*8,48,"�� Auto");
			// if(IsTrigMode(Trig_Internal))
			// 	DispStr8(0,48,"M Setup");
			}
	}else{
		//�ⴥ���б�Ŀؼ�Ҫ��ʾ
		//���������ٶȿؼ���λ��
		//û�а취��ʾ4�У�Speedռ�ıȽϴ�	
	}
	GUI_Page::Show();		//��ʾ�ؼ�
	ShowRunIcon((int)Status_MCU);		
	ShowBatteryIcon(BatLevel);		
	//ShowChargeIcon(ChargeFlag);	
	//ShowTrigModeIcon();             //��ʾ����ģʽ����ʱ�ر�
#ifdef DEBUG //��ʾ����ѡ���ֹ����ǰˢ��汾
	DispStr8(BATTERY_ICON_X-8,0,"D");
#endif	
	//��ʾsecurity bit�Ƿ����
	if(!FLASH_GetReadOutProtectionStatus()){ //�޶���������Ч  
		DispStr8(BATTERY_ICON_X-15,0,"!");   
	}
	Update();
}

inline void CMainPage::OnValChange()
{
	switch(FocusId){
//		case iDelay:
//			break;
		default:
			break;
	}
}

///�ڴ�����ؿؼ���ʼ��
inline void CMainPage::PageInit_Internal()
{
	ObjList.Delete(1,ObjList.MaxNum-1);
	FocusId = 0; //������Ƶ�ʵ����ϣ���������
}

///��������ؿؼ���ʼ��



//��ʾ����״̬��ͼ��
void CMainPage::ShowRunIcon(int mode){
	switch(AppPara.Language){
		case Lang_Chinese:
			DispBmp(RUN_ICON_X,0, Icon_LED[2]);  //led icon
			Update(RUN_ICON_X, 0, Icon_LED[0].Width, Icon_LED[0].Height*2);
			DispStr8(RUN_CH_X,0,(mode==Status_WorkStall) ? "��" : "��");  //led ��/��
			//Update(RUN_CH_X, 8, Icon_LED[0].Width,  Icon_LED[0].Height);
			Update(RUN_CH_X, 0, 2*DEFAULT_HANZI_WIDTH, 16);

			break;
		case Lang_English:
			DispStr8(RUN_ICON_X, 0, (mode==Status_WorkStall) ? "STOP" : "RUN ");
			Update(RUN_ICON_X, 0, 3*DEFAULT_ASCII_WIDTH, 16);
			break;
	}
}

//��ʾ���״̬��ͼ��
//Todo:ԭ��ֻ����5��ͼ�꣬��Ҫ����Ļ�����
void CMainPage::ShowBatteryIcon(int level)
{
	if(level <0 || level > ARRAY_SIZE(Icon_Bat)) return;
	DispBmp(BATTERY_ICON_X,4, Icon_Bat[level]);
	//Update(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2);
}

//��ʾ���״̬��ͼ��
//Todo�����״̬һ�ַ����ǻ���̬��ͼ�꣬һ�ַ�������һ������ǣ���һ��������һЩ��ͼ�껹û��
void CMainPage::ShowChargeIcon(bool enable)
{
	if(enable){//��Icon
		DispBmp8(CHARGE_ICON_X, 0, Icon_Charge_Small);
	}else{//��Icon
		Fill8(CHARGE_ICON_X,0, Icon_Charge_Small.Width, Icon_Charge_Small.Height,0);
	}
}

#define TRIGMODE_ICON_X_CN	3*DEFAULT_HANZI_WIDTH
#define TRIGMODE_ICON_X_EN	5*DEFAULT_ASCII_WIDTH

void CMainPage::ShowTrigModeIcon()
{
	const char * pTrigStr;
	u8 x,w;
//	if(IsTrigMode(Trig_Internal))
	{
		switch(AppPara.Language){
			case Lang_Chinese:
				pTrigStr = "�ڴ���";
				x = TRIGMODE_ICON_X_CN;
				w= 3*DEFAULT_HANZI_WIDTH;
				break;
			case Lang_English:
				pTrigStr = "Int.";
				x = TRIGMODE_ICON_X_EN;
				w = 4*DEFAULT_ASCII_WIDTH;
				break;			
			default:
				return;
		}
	}

	DispStr8(x, 0, pTrigStr);
	
	Update(x, 0, w, 16);		
}

///���������ʱ�Ĳ���
void CMainPage::OnIdle()
{
	static int Last_Status_MCU = -1;	//�ϴ���ʾ������״̬
	static int Last_TrigMode = -1;		//�ϴ���ʾ�Ĵ���ģʽ
	static int BlinkChargeIcon = 0;		//����־��˸����
	static int BlinkBatIcon = 0;		//��ر�־��˸����
	
	//�������µ�״̬��ʾ�����ڳ�ʱ���������˸о���Ӧ��һЩ
	//��������״̬��ʾ
	if(Last_Status_MCU != Status_MCU)
	{
		ShowRunIcon(Status_MCU);
		Last_Status_MCU = Status_MCU;
	}
	//���´���ģʽ����ʾ
	if(Last_TrigMode != AppPara.TrigMode)
	{
		//ShowTrigModeIcon();
		Last_TrigMode = AppPara.TrigMode;
	}
	//��ʱˢ�µ���ʾ
	if(IsTimeOut_ms(TIdle,500))
	{
		TIdle = GetTimerCount();
		OnIdleMainPage(TIdle);

		ChargeFlag = GetChargePin;
		//�������ѹ״̬
		if(ChargeFlag)	// ����ͷ���룬������ʾ����־
		{
			if(BlinkChargeIcon==1)	//�������־
			{
				ShowChargeIcon(false);
				BlinkChargeIcon = 0;
			}
			else					//��ʾ����־
			{
				ShowChargeIcon(true);
				BlinkChargeIcon = 1;
			}
			//������ʾ��ر�־
			ShowBatteryIcon(BatLevel);
			BlinkBatIcon = 1;
		}
		else 						//�������־
		{
			ShowChargeIcon(false);
			BlinkChargeIcon = 0;
			//���״̬
			if(BatLevel <=0)		//Ƿѹʱ������ʾ
			{
				if(BlinkBatIcon==1)//�����ر�־
				{
					Fill8(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2, 0x00);
					Update(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2);;
					BlinkBatIcon = 0;
				}
				else				//��ʾ��ر�־
				{
					ShowBatteryIcon(BatLevel);
					BlinkBatIcon = 1;
				}			
			}
			else//������ʾ��ر�־
			{
				ShowBatteryIcon(BatLevel);
				BlinkBatIcon = 1;
			}			
		}
		
		//todo:�ⴥ��ʱ�ж�Ƶ��/ת��ֵ�仯���ٸ�����ʾ���ڴ����ɱ༭��������ʾ
		GUI_Page::Show();
		Update();		
	}

}

}//namespace gui {


