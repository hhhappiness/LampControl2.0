#include "SubPage.hpp"
#include "version.h"
#include "string.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "SysPara.h"
#include "ctrl.h"
#include "ctrl_common.h"

//要用到的C函数声明
extern "C"{
void GetLinkTimeStr_YYMMDD(char * LinkTimeStr);
}

namespace gui {

///Todo: 不同OEM版本
///生产厂家中英文名称

char ManufactureSn[16] ={
	"SN:2015888"
};


///联系方式种类
#define CONTACT_NUM  3

#ifdef OEM_kimmin
const char WuhanKesaiStr_Cn[]="金明科技";
const char WuhanKesaiStr_En[]="Kimminh Machine";
///切换联系方式的字符串
static const char *ContactStr[CONTACT_NUM]=
{
	WuhanKesaiStr_Cn,
	"www.whkesai.com",
	"01235888858",
	"kimminh.machine@gmail.com",
	//最后一条打印bootloader版本号
};
#else
const char WuhanKesaiStr_Cn[]="武汉科赛智能电子";
const char WuhanKesaiStr_En[]="Wuhan Kesai";
///切换联系方式的字符串
static const char *ContactStr[CONTACT_NUM]=
{
	WuhanKesaiStr_Cn,
	"www.whkszn.com",
	"18086662932",
	//最后一条打印bootloader版本号，已移到第二行
};
#endif


///构造函数：添加4个GUI_Text型控件
AboutPage ::AboutPage()
: SubPage(4)
, ContactId(0)
{
	
	if(IsLanguageCh()){
		ContactStr[0] = WuhanKesaiStr_Cn;
	}else{
		
		ContactStr[0] = WuhanKesaiStr_En;
	}
	//从flash获取SN信息
	if(SysPara.SnByte1 != 0xffffffff)
		memcpy(&ManufactureSn[3],&(SysPara.SnByte1), 7);
		
	
	ObjList.Append(new GUI_Text(128,16,ModelStr));
//	ObjList.Append(new GUI_Text(128,16,ManufactureSn)); //sn
	ObjList.Append(new GUI_Text(128,16,VersionStr));
	ObjList.Append(new GUI_Text(128,16,BootloaderVersionStr));
	ObjList.Append(new GUI_Text(128,16,ContactStr[ContactId]));
	//每个占一满行，居中对齐
	for(int i=0;i<ObjList.Num;i++){
		ObjList[i]->SetPos(0,16*i);
		ObjList[i]->Align = AlignMid;
	}
	GetModelStr();
	//生成版本的字符串
	sprintf(VersionStr, "%d.%d.%d ", MAJOR_VERSION, MID_VERSION, MINOR_VERSION);
	GetLinkTimeStr_YYMMDD(VersionStr+strlen(VersionStr));//应该不会溢出
	
//	ContactStr[CONTACT_NUM-1]=BootloaderVersionStr;
}

void AboutPage ::OnIdle()
{
	//联系方式滚动显示
	if(IsTimeOut_ms(TScroll,1800)){//注意不超过MAX_DELAY_MS
		TScroll = GetTimerCount();
		if(ContactId == CONTACT_NUM-1){
			ContactId = 0;
		}else{
			ContactId++;
		}
		GUI_Text * p= (GUI_Text * )ObjList[iContact];
		p->Str = ContactStr[ContactId];
		p->Display();
		//p->Update();
		pCurrPage->Update();
	}
	//调用父类的OnIdle()
	SubPage::OnIdle();
}
void AboutPage ::Init()
{
	pCurrPage = (GUI_Page * )this;
}

///显示界面
void AboutPage ::Show()
{
	Clear();
	GUI_Page::Show();
	Update();
}

///生成型号的字符串
///Todo: 生成实际型号
void AboutPage ::GetModelStr()
{
	int Id =LightType2Id();
    if (Id<0) {//型号错误
		strcpy(ModelStr,"Unkown");
    }else{
		strcpy(ModelStr,LightType_StrList[Id]);
	}
}

}//namespace gui {

