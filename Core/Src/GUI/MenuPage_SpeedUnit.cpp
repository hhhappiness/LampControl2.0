#include "MenuPage.hpp"
#include "GUI_Object.hpp"
#include "AppParaCommon.h"
#include "AppInterface.hpp"
#include "timer.h"

namespace gui {
const char *SpeedUnitStr_Cn[SpeedUnitNum]={
	"Ƶ��",
	"ת��",
	"���ٶ�"
};

const char *SpeedUnitStr_En[SpeedUnitNum]={
	"Frequnce",
	"rpm",
	"LineSpeed"
};

MenuPage_SpeedUnit::MenuPage_SpeedUnit()
: MenuPage(MaxObjNum), BakUnit(AppPara.SpeedUnit)
{
	if(AppPara.Language == Lang_Chinese)
		ObjList.Append(new GUI_List(48,16,&AppPara.SpeedUnit,SpeedUnitNum,SpeedUnitList,SpeedUnitStr_Cn));
	else
		ObjList.Append(new GUI_List(72,16,&AppPara.SpeedUnit,SpeedUnitNum,SpeedUnitList,SpeedUnitStr_En));
	ObjList[iSpeedUnit]->SetPos(2+2*16+2,16);
	
}

void MenuPage_SpeedUnit::Init()
{
	pCurrPage = this;	
	if(AppPara.SpeedUnit == Unit_mpmin){//���ٶȶ�泤��ÿ�滭����������
		if(ObjList[iPlateLen] == NULL)
			{
			GUI_Num * p = new GUI_Num(&AppPara.PlateLen, AppParaMin.PlateLen, AppParaMax.PlateLen,4);
			ObjList.Set(p,iPlateLen);
			p->OnOk = OnOkPlateLen;//�¼�������
			if(AppPara.Language == Lang_Chinese) {
				p->SetPos(2+2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH,48);
			}else {
				p->SetPos(2+9*DEFAULT_ASCII_WIDTH,48);
			}
		}
		if(ObjList[iImagePerPlate] == NULL)
			{
			GUI_Num * p = new GUI_Num(&AppPara.ImagePerPlate, AppParaMin.ImagePerPlate, AppParaMax.ImagePerPlate,2);
			if(p != NULL){
				ObjList.Set(p,iImagePerPlate);
				p->OnOk = OnOkImagePerPlate;//�¼�������
				if(AppPara.Language == Lang_Chinese) {
					p->SetPos(2+2*DEFAULT_HANZI_WIDTH + 6,32);
				} else {
					p->SetPos(2+6*7,32);
					ObjList[iImagePerPlate]->SetAligh(AlignLeft);
				}
			}else{
				//while(1);
			}
		}
	}else{
		ObjList.Delete(iPlateLen);
		if(ObjList[iImagePerPlate] == NULL)
			{
				
			GUI_Num * p = new GUI_Num(&AppPara.ImagePerPlate, AppParaMin.ImagePerPlate, AppParaMax.ImagePerPlate,2);
			ObjList.Set(p,iImagePerPlate);
			p->OnOk = OnOkImagePerPlate;//�¼�������
			if(AppPara.Language == Lang_Chinese) {
				p->SetPos(2+2*DEFAULT_HANZI_WIDTH + 6,32);
			} else {
				p->SetPos(2+6*7,32);
				ObjList[iImagePerPlate]->SetAligh(AlignLeft);
			}
			
		}
		
	}
	SetFocus(iSpeedUnit,false);
}

void MenuPage_SpeedUnit::Show()
{
	
	//�Ȼ�����ҳ����ص�
	MenuPage::Show();
	
	SetFontASCII();
	if(AppPara.Language == Lang_Chinese) {
		DispStr8(2,16,"��λ:");
		if(AppPara.SpeedUnit == Unit_mpmin){
			
			DispStr8(2,32,"����:	 ��/ÿ��");
			DispStr8(2,48,"�泤:     mm");
		}
		else {
			DispStr8(2,32,"����:	 ��/ÿ��");
			DispStr8(2,48,"        ");
		}
	}
	else {
		DispStr8(2,16,"Unit:  ");
		if(AppPara.SpeedUnit == Unit_mpmin){
			DispStr8(2,48,"PlateLen:     mm");
			DispStr8(2,32,"Image:");
		}
		else {
				DispStr8(2,48,"                        ");
				DispStr8(2,32,"Image:");
			}
	}
	GUI_Page::Show();
	Update();
}

void MenuPage_SpeedUnit::OnValChange()
{
	switch(FocusId){
		case iSpeedUnit:
			OnChangeSpeedUnit();
			Init();
			Show();//������ʾ
			//BakUnit = AppPara.SpeedUnit;
			break;
		case iPlateLen:
			//OnChangePlateLen();
			//OnOkPlateLen();
			break;
	}
	
}
}//namespace gui {
