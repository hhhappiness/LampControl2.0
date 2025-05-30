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
		//�Ȼ��˵�
		MenuPage::Show();
		if(IsLanguageCh())
			DispStr8(2,16,"��ص�ѹ:     V");
			//DispStr8(2,32,"����ʱ��:     Сʱ");
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
	ObjList.Delete(1,MaxObjNum-1);//ɾ���������治ͬ�Ŀؼ�
	if(!SecretMode){
		//ObjList.Append(new GUI_NumText(&Duration,4,1));
		ObjList[iVoltage]->SetPos(4*DEFAULT_HANZI_WIDTH + DEFAULT_ASCII_WIDTH+2,16);
		//ObjList[iDuration]->SetPos(4*DEFAULT_HANZI_WIDTH + DEFAULT_ASCII_WIDTH+2,32);
		//ObjList[iDuration]->Align = AlignMid;a
		SetFocus(-1, false);
		Voltage = (GetVoltage() +50)/100;//3λС��ת��1λ
		//todo�������ʱ��Duration
	}else{
		//��ѹ�ĳ�3λС����ʾ
		GUI_NumText* p = (GUI_NumText* )ObjList[iVoltage];
		p->DigitalNum = 6;//15.000��5λ���ּӸ�С����
		p->Decimal =3;		//3λС��
		p->Align = AlignRight;//�ĳ��Ҷ���
		//��������ؼ�
		ObjList.Append(new GUI_NumText(&AdcVal,4,0));
		ObjList.Append(new GUI_NumText(&AdVal1,4,0));
		ObjList.Append(new GUI_Num(&V1,6000,7000,6,3));
		ObjList.Append(new GUI_NumText(&AdVal2,4,0));
		ObjList.Append(new GUI_Num(&V2,13000,15050,6,3));
		ObjList.Append(new GUI_TextButton(28,16,"����"));
		ObjList.Append(new GUI_NumText(&SysPara.ACoff,8,16));
		pObjCoff = (GUI_NumText *)ObjList[iCoff];
		//���ÿؼ�λ��
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
			SaveSysConfig();//��ʱ������AppPara���Ժ󻻵�����������
			//���±���ֵ
			bakACoff = SysPara.ACoff;
			bakCCoff = SysPara.CCoff;
			break;
		default:
			return;
	}
	//�ָ���ҳ��
	Show();
}

void MenuPage_Battery::OnValChange()
{
	switch(FocusId){
		case iV1:
			AdVal1 = GetAvgAdc();	//�ɼ�ADCֵ
			AdcFlag |= 0x01;		//����Flag
			break;
		case iV2:
			AdVal2 = GetAvgAdc();	//�ɼ�ADCֵ
			AdcFlag |= 0x02;		//����Flag
			break;
		default:
			return;
	}
	if(AdcFlag == 0x03){ //����ADֵ�����ˣ�����ϵ��
		CalcAdcCoff(AdVal1, AdVal2, V1, V2);
	}
	//ˢ�±�ҳ��
	Show();
}

void MenuPage_Battery::OnIdle()
{
	if(IsTimeOut_ms(TIdle,500)){
		TIdle = GetTimerCount();
		ResetTimeOut(TIdle);
		
		if(!SecretMode){
			Voltage = (GetVoltage() +50)/100;//3λС��ת��1λ
			//todo:�������ʱ��Duration
			GUI_Page::Show();
		}else{
			BlkCtrlCnt = 0;//�屳���������ֱ��ⳣ��
			AdcVal = GetAvgAdc();
			Voltage = CalcVoltage(AdcVal);
			//�л���ʾA��C����
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
		Update();//todo:��ʱˢ��ȫ�����Ժ�ĳ�ˢ�¿ؼ�
	}
}

///�˳�ǰ�ָ�ϵ���ı���ֵ
void MenuPage_Battery::OnClose(int x)
{
	SetAdcCoff(bakACoff, bakCCoff);
}

}//namespace gui {
