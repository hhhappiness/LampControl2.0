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
int encoder_val; //读取的编码器值

namespace gui {
#define RUN_ICON_X (0)
#define RUN_CH_X (18)
#define BATTERY_ICON_X	(128-16-1-5)
#define CHARGE_ICON_X	(128-5)
	
const u8 SpeedX[SpeedUnitNum]={32,20,16};	///Speed控件的x坐标
#define SpeedY 	28							///Speed控件的y坐标
#define SPEED_STEP_NUM 	3	
///Speed控件的步进值列表
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
	//缺省第一个控件，不可删除
	ObjList.Append(&SpeedCtrl);
}	
	

void CMainPage::OnMeasureMode()
{
	
	ScanAdcPage* MeasurePage = new ScanAdcPage;
	MeasurePage->Init();
	MeasurePage->Show();
	int Ret = MeasurePage->Loop();	 
	delete MeasurePage; //删除测量页面对象
	//返回值则为用户选择的算法计算值，直接将SpeedCtrl的值设置为此值
	SpeedCtrl.OnSetVal(Ret*100); //设置SpeedCtrl的值
	Init(); //重新初始化页面
	Show();	//显示
}

///MainPage主循环
int CMainPage::Loop()
{
	
	TKey = GetTimerCount();
	TIdle = GetTimerCount();
	while(1){
		if(IsTimeOut_ms(TKey,50)){
			wdg();
			Key = GetKey();
			if(Key != KEY_NULL){
				if(IsTrigMode(Trig_Internal)){//内触发只调频率
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
						case KEY_DIV2_SHOT ://除2
							MainScanFlag = 1;
							SpeedCtrl.Div2();
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_MULT_SHOT ://乘2
							MainScanFlag = 1;
							SpeedCtrl.Mul2();
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_UP_SHOT : 	
							MainScanFlag = 1;
							SpeedCtrl.OnStep(SpeedCtrl.StepList[0]);//最小步进值增加
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_DOWN_SHOT : 
							MainScanFlag = 1;
							SpeedCtrl.OnStep(-SpeedCtrl.StepList[0]);//最小步进值减小
							//SpeedCtrl.Loop(Key); 
							SpeedCtrl.Display();
							SpeedCtrl.Update();
							break;
						case KEY_UP_LONG : 		
						
								//SpeedCtrl.OnStep(SpeedCtrl.StepList[1]);//最小自动步进值增加
								SpeedCtrl.Loop(Key); 
								SpeedCtrl.Display();
								SpeedCtrl.Update();
						
							break;
						case KEY_DOWN_LONG : 	
							
								//SpeedCtrl.OnStep(-SpeedCtrl.StepList[1]);//最小自动步进值减小
								SpeedCtrl.Loop(Key); 
								SpeedCtrl.Display();
								SpeedCtrl.Update();
							
							break;		
						case KEY_ENTER_SHOT : 
							
								//进入扫频界面
								
								EnterScanFlag = 1;
								ScanDlyFlag = 0;
								
								OnEnter();	
								
								EnterScanFlag = 0;
								ScanDlyCnt = 0;
								ScanDlyFlag = 1;
								ScanDlyCounting = 1;								
							
							break;
							
					}
				}else{//外触发跟普通界面一样
					GUI_Num * p = (GUI_Num *)ObjList[FocusId]; 
					//GUI_Num * p = NULL ;
					switch(Key){
						case KEY_UP_RELEASE:     //返回键，外触发暂时不管
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

///按Mode键进入菜单
void CMainPage::OnKeyMode()
{
	MenuPage::ShowMenuPage();
	Init(); //重新初始化页面
	Show();	//显示
}


///初始化页面中的所有对像，包括菜单、控件
void CMainPage::Init()
{
	pCurrPage = (GUI_Page * )this;
	Clear();	//清屏
	
	switch(AppPara.TrigMode){
		case Trig_Internal:		PageInit_Internal();	break;
	}
	SpeedCtrl.OnSetSpeedUnit();//根据单位更新显示
}


///显示MainPage
void CMainPage::Show()
{
	//显示单位
	int i=AppPara.SpeedUnit;
	int x = LcmXPixel-Song_Width16_ASCII.Width*6-11*2; //6个数字+2个单位
	SpeedCtrl.SetPos(x/2,SpeedY);	//设置位置
	SpeedCtrl.SetStepList(SpeedStepList[i], KeepList, SPEED_STEP_NUM);//设置步进列表
	DispStr8Font(x/2 + Song_Width16_ASCII.Width*6, SpeedY+8, SpeedUnitStr[i], 0, &Song_ASCII_Special);//显示单位
	Update();
	//内触发显示按键提示
	if(IsTrigMode(Trig_Internal)){
		if(IsLanguageCh()) {
			// DispStr8(0,48,"● 自动");
			//if(IsTrigMode(Trig_Internal)) DispStr8(0,48,"M 设置");
			}
		else {
			// DispStr8(10*8,48,"● Auto");
			// if(IsTrigMode(Trig_Internal))
			// 	DispStr8(0,48,"M Setup");
			}
	}else{
		//外触发有别的控件要显示
		//重新设置速度控件的位置
		//没有办法显示4行，Speed占的比较大	
	}
	GUI_Page::Show();		//显示控件
	ShowRunIcon((int)Status_MCU);		
	ShowBatteryIcon(BatLevel);		
	//ShowChargeIcon(ChargeFlag);	
	//ShowTrigModeIcon();             //显示触发模式，暂时关闭
#ifdef DEBUG //显示调试选项，防止出货前刷错版本
	DispStr8(BATTERY_ICON_X-8,0,"D");
#endif	
	//显示security bit是否加上
	if(!FLASH_GetReadOutProtectionStatus()){ //无读保护则生效  
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

///内触发相关控件初始化
inline void CMainPage::PageInit_Internal()
{
	ObjList.Delete(1,ObjList.MaxNum-1);
	FocusId = 0; //焦点在频率调节上，但不反显
}

///单脉冲相关控件初始化



//显示运行状态的图标
void CMainPage::ShowRunIcon(int mode){
	switch(AppPara.Language){
		case Lang_Chinese:
			DispBmp(RUN_ICON_X,0, Icon_LED[2]);  //led icon
			Update(RUN_ICON_X, 0, Icon_LED[0].Width, Icon_LED[0].Height*2);
			DispStr8(RUN_CH_X,0,(mode==Status_WorkStall) ? "关" : "开");  //led 开/关
			//Update(RUN_CH_X, 8, Icon_LED[0].Width,  Icon_LED[0].Height);
			Update(RUN_CH_X, 0, 2*DEFAULT_HANZI_WIDTH, 16);

			break;
		case Lang_English:
			DispStr8(RUN_ICON_X, 0, (mode==Status_WorkStall) ? "STOP" : "RUN ");
			Update(RUN_ICON_X, 0, 3*DEFAULT_ASCII_WIDTH, 16);
			break;
	}
}

//显示电池状态的图标
//Todo:原来只做了5级图标，需要更多的话再做
void CMainPage::ShowBatteryIcon(int level)
{
	if(level <0 || level > ARRAY_SIZE(Icon_Bat)) return;
	DispBmp(BATTERY_ICON_X,4, Icon_Bat[level]);
	//Update(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2);
}

//显示充电状态的图标
//Todo：充电状态一种方案是画动态的图标，一种方案是另画一个充电标记，后一个方案简单一些。图标还没画
void CMainPage::ShowChargeIcon(bool enable)
{
	if(enable){//画Icon
		DispBmp8(CHARGE_ICON_X, 0, Icon_Charge_Small);
	}else{//清Icon
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
				pTrigStr = "内触发";
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

///主界面空闲时的操作
void CMainPage::OnIdle()
{
	static int Last_Status_MCU = -1;	//上次显示的运行状态
	static int Last_TrigMode = -1;		//上次显示的触发模式
	static int BlinkChargeIcon = 0;		//充电标志闪烁控制
	static int BlinkBatIcon = 0;		//电池标志闪烁控制
	
	//立即更新的状态显示，不在超时里做，让人感觉响应快一些
	//更新运行状态显示
	if(Last_Status_MCU != Status_MCU)
	{
		ShowRunIcon(Status_MCU);
		Last_Status_MCU = Status_MCU;
	}
	//更新触发模式的显示
	if(Last_TrigMode != AppPara.TrigMode)
	{
		//ShowTrigModeIcon();
		Last_TrigMode = AppPara.TrigMode;
	}
	//定时刷新的显示
	if(IsTimeOut_ms(TIdle,500))
	{
		TIdle = GetTimerCount();
		OnIdleMainPage(TIdle);

		ChargeFlag = GetChargePin;
		//充电与电池压状态
		if(ChargeFlag)	// 充电插头插入，闪动显示充电标志
		{
			if(BlinkChargeIcon==1)	//清除充电标志
			{
				ShowChargeIcon(false);
				BlinkChargeIcon = 0;
			}
			else					//显示充电标志
			{
				ShowChargeIcon(true);
				BlinkChargeIcon = 1;
			}
			//正常显示电池标志
			ShowBatteryIcon(BatLevel);
			BlinkBatIcon = 1;
		}
		else 						//清除充电标志
		{
			ShowChargeIcon(false);
			BlinkChargeIcon = 0;
			//电池状态
			if(BatLevel <=0)		//欠压时闪动显示
			{
				if(BlinkBatIcon==1)//清除电池标志
				{
					Fill8(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2, 0x00);
					Update(BATTERY_ICON_X, 0, Icon_Bat[0].Width, Icon_Bat[0].Height*2);;
					BlinkBatIcon = 0;
				}
				else				//显示电池标志
				{
					ShowBatteryIcon(BatLevel);
					BlinkBatIcon = 1;
				}			
			}
			else//正常显示电池标志
			{
				ShowBatteryIcon(BatLevel);
				BlinkBatIcon = 1;
			}			
		}
		
		//todo:外触发时判断频率/转速值变化了再更新显示，内触发由编辑器更新显示
		GUI_Page::Show();
		Update();		
	}

}

}//namespace gui {


