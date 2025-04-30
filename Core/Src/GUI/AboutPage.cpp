#include "SubPage.hpp"
#include "version.h"
#include "string.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "SysPara.h"
#include "ctrl.h"
#include "ctrl_common.h"

//Ҫ�õ���C��������
extern "C"{
void GetLinkTimeStr_YYMMDD(char * LinkTimeStr);
}

namespace gui {

///Todo: ��ͬOEM�汾
///����������Ӣ������

char ManufactureSn[16] ={
	"SN:2015888"
};


///��ϵ��ʽ����
#define CONTACT_NUM  3

#ifdef OEM_kimmin
const char WuhanKesaiStr_Cn[]="�����Ƽ�";
const char WuhanKesaiStr_En[]="Kimminh Machine";
///�л���ϵ��ʽ���ַ���
static const char *ContactStr[CONTACT_NUM]=
{
	WuhanKesaiStr_Cn,
	"www.whkesai.com",
	"01235888858",
	"kimminh.machine@gmail.com",
	//���һ����ӡbootloader�汾��
};
#else
const char WuhanKesaiStr_Cn[]="�人�������ܵ���";
const char WuhanKesaiStr_En[]="Wuhan Kesai";
///�л���ϵ��ʽ���ַ���
static const char *ContactStr[CONTACT_NUM]=
{
	WuhanKesaiStr_Cn,
	"www.whkszn.com",
	"18086662932",
	//���һ����ӡbootloader�汾�ţ����Ƶ��ڶ���
};
#endif


///���캯�������4��GUI_Text�Ϳؼ�
AboutPage ::AboutPage()
: SubPage(4)
, ContactId(0)
{
	
	if(IsLanguageCh()){
		ContactStr[0] = WuhanKesaiStr_Cn;
	}else{
		
		ContactStr[0] = WuhanKesaiStr_En;
	}
	//��flash��ȡSN��Ϣ
	if(SysPara.SnByte1 != 0xffffffff)
		memcpy(&ManufactureSn[3],&(SysPara.SnByte1), 7);
		
	
	ObjList.Append(new GUI_Text(128,16,ModelStr));
//	ObjList.Append(new GUI_Text(128,16,ManufactureSn)); //sn
	ObjList.Append(new GUI_Text(128,16,VersionStr));
	ObjList.Append(new GUI_Text(128,16,BootloaderVersionStr));
	ObjList.Append(new GUI_Text(128,16,ContactStr[ContactId]));
	//ÿ��ռһ���У����ж���
	for(int i=0;i<ObjList.Num;i++){
		ObjList[i]->SetPos(0,16*i);
		ObjList[i]->Align = AlignMid;
	}
	GetModelStr();
	//���ɰ汾���ַ���
	sprintf(VersionStr, "%d.%d.%d ", MAJOR_VERSION, MID_VERSION, MINOR_VERSION);
	GetLinkTimeStr_YYMMDD(VersionStr+strlen(VersionStr));//Ӧ�ò������
	
//	ContactStr[CONTACT_NUM-1]=BootloaderVersionStr;
}

void AboutPage ::OnIdle()
{
	//��ϵ��ʽ������ʾ
	if(IsTimeOut_ms(TScroll,1800)){//ע�ⲻ����MAX_DELAY_MS
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
	//���ø����OnIdle()
	SubPage::OnIdle();
}
void AboutPage ::Init()
{
	pCurrPage = (GUI_Page * )this;
}

///��ʾ����
void AboutPage ::Show()
{
	Clear();
	GUI_Page::Show();
	Update();
}

///�����ͺŵ��ַ���
///Todo: ����ʵ���ͺ�
void AboutPage ::GetModelStr()
{
	int Id =LightType2Id();
    if (Id<0) {//�ͺŴ���
		strcpy(ModelStr,"Unkown");
    }else{
		strcpy(ModelStr,LightType_StrList[Id]);
	}
}

}//namespace gui {

