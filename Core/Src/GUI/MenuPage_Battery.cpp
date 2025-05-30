#include "MenuPage.hpp"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "AppInterface.hpp"
#include "adc.h"
#include "ctrl.h"
#include "rtc_int.h"

namespace gui {


MenuPage_Battery::MenuPage_Battery()
: MenuPage(MaxObjNum)
, Voltage(0)
, Duration(0)
, AdVal1(0), AdVal2(0), V1(6500), V2(15000), AorC(0)
, AdcFlag(0)
{
	ObjList.Append(new GUI_NumText(&Voltage,4,1));
	SecretKey = 0x55;
	
}

void MenuPage_Battery::Show()
{
	if(!SecretMode){
		//先画菜单
		MenuPage::Show();
		if(IsLanguageCh())
			DispStr8(2,16,"电池电压:     V");
			//DispStr8(2,32,"可用时间:     小时");
		else
			DispStr8(2,16,"Voltage:     V");
	}else{
		Clear();
		DispStr8(0,0,"ADC:     V:");
		DispStr8(9*DEFAULT_ASCII_WIDTH,16,"V1:");
		DispStr8(9*DEFAULT_ASCII_WIDTH,32,"V2:");
		DispStr8(0*DEFAULT_ASCII_WIDTH,16,"AD1:");
		DispStr8(0*DEFAULT_ASCII_WIDTH,32,"AD2:");
		DispStr8(8*DEFAULT_ASCII_WIDTH,48,"A:");


	}
	GUI_Page::Show();
	Update();
}

void MenuPage_Battery::Init()
{
	pCurrPage = (GUI_Page*)this;
	ObjList.Delete(1,MaxObjNum-1);//删除两个界面不同的控件
	if(!SecretMode){
		//ObjList.Append(new GUI_NumText(&Duration,4,1));
		ObjList[iVoltage]->SetPos(4*DEFAULT_HANZI_WIDTH + DEFAULT_ASCII_WIDTH+2,16);
		//ObjList[iDuration]->SetPos(4*DEFAULT_HANZI_WIDTH + DEFAULT_ASCII_WIDTH+2,32);
		//ObjList[iDuration]->Align = AlignMid;a
		SetFocus(-1, false);
		Voltage = (GetVoltage() +50)/100;//3位小数转成1位
		//todo计算可用时间Duration
	}else{
		//电压改成3位小数显示
		GUI_NumText* p = (GUI_NumText* )ObjList[iVoltage];
		p->DigitalNum = 6;//15.000，5位数字加个小数点
		p->Decimal =3;		//3位小数
		p->Align = AlignRight;//改成右对齐
		//添加其它控件
		ObjList.Append(new GUI_NumText(&AdcVal,4,0));
		ObjList.Append(new GUI_NumText(&AdVal1,4,0));
		ObjList.Append(new GUI_Num(&V1,6000,7000,6,3));
		ObjList.Append(new GUI_NumText(&AdVal2,4,0));
		ObjList.Append(new GUI_Num(&V2,13000,15050,6,3));
		ObjList.Append(new GUI_TextButton(28,16,"保存"));
		ObjList.Append(new GUI_NumText(&SysPara.ACoff,8,16));
		pObjCoff = (GUI_NumText *)ObjList[iCoff];
		//设置控件位置
		ObjList[iVoltage]->SetPos(12*DEFAULT_ASCII_WIDTH,0);
		ObjList[iADC]->SetPos(4*DEFAULT_ASCII_WIDTH,0);
		
		ObjList[iAdVal1]->SetPos(4*DEFAULT_ASCII_WIDTH,16);
		ObjList[iV1]->SetPos(12*DEFAULT_ASCII_WIDTH,16);
		ObjList[iAdVal2]->SetPos(4*DEFAULT_ASCII_WIDTH,32);
		ObjList[iV2]->SetPos(12*DEFAULT_ASCII_WIDTH,32);	
		
		ObjList[iBtnSave]->SetPos(0*DEFAULT_ASCII_WIDTH,48);
		ObjList[iCoff]->SetPos(10*DEFAULT_ASCII_WIDTH,48);	
		ObjList[iCoff]->Align = AlignRight;
		AdcVal = GetAvgAdc();
		Voltage = CalcVoltage(AdcVal);
		bakACoff = SysPara.ACoff;
		bakCCoff = SysPara.CCoff;
		SetFocus(iV1, false);
	}
}

void MenuPage_Battery::OnClick(int i)
{
	switch(i){
		case iBtnSave:
			SaveSysConfig();//暂时保存在AppPara，以后换到单独的扇区
			//更新备份值
			bakACoff = SysPara.ACoff;
			bakCCoff = SysPara.CCoff;
			break;
		default:
			return;
	}
	//恢复本页面
	Show();
}

void MenuPage_Battery::OnValChange()
{
	switch(FocusId){
		case iV1:
			AdVal1 = GetAvgAdc();	//采集ADC值
			AdcFlag |= 0x01;		//更新Flag
			break;
		case iV2:
			AdVal2 = GetAvgAdc();	//采集ADC值
			AdcFlag |= 0x02;		//更新Flag
			break;
		default:
			return;
	}
	if(AdcFlag == 0x03){ //两个AD值都采了，计算系数
		CalcAdcCoff(AdVal1, AdVal2, V1, V2);
	}
	//刷新本页面
	Show();
}

void MenuPage_Battery::OnIdle()
{
	if(IsTimeOut_ms(TIdle,500)){
		TIdle = GetTimerCount();
		ResetTimeOut(TIdle);
		
		if(!SecretMode){
			Voltage = (GetVoltage() +50)/100;//3位小数转成1位
			//todo:计算可用时间Duration
			GUI_Page::Show();
		}else{
			BlkCtrlCnt = 0;//清背计数器保持背光常开
			AdcVal = GetAvgAdc();
			Voltage = CalcVoltage(AdcVal);
			//切换显示A或C参数
			AorC = (AorC+1)&0x07;
			if(AorC==0){
				DispStr8(8*DEFAULT_ASCII_WIDTH,48,"A:");
				pObjCoff->pVal = &SysPara.ACoff;
			}else if(AorC==4){
				DispStr8(8*DEFAULT_ASCII_WIDTH,48,"C:");
				pObjCoff->pVal = &SysPara.CCoff;
			}			
			GUI_Page::Show();
		}
		Update();//todo:暂时刷新全部，以后改成刷新控件
	}
}

///退出前恢复系数的备份值
void MenuPage_Battery::OnClose(int x)
{
	SetAdcCoff(bakACoff, bakCCoff);
}

}//namespace gui {
