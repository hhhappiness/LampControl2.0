#include "SysParaCommon.h"
#include "type.h"
#include "lcd.h"


SYS_PARA 	SysPara;				///<ʹ���еĲ�����

const char * LightType_StrList[LIGHT_TYPE_NUM]={
	"WKS100LI0",
	"WKS100LI1",
	"WKS100_LI",
};

const int LightTypeList[LIGHT_TYPE_NUM]={
	WKS100_LI0,
	WKS100_LI1,
	WKS100_LI,
};

///���TrigModeList����id��
int LightType2Id(void)
{
	int i;
	for(i=0;i<LIGHT_TYPE_NUM;i++){
		if(LightTypeList[i] == SysPara.LightType) return i;
	}
	return -1;
}


///������ȱʡֵ
const SYS_PARA SysParaDefault={	
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	SYS_PARA_VERSION,	//u32 Version;	

	WKS100_LI, 			//�ƹ�����
	~WKS100_LI,
	0x0008DC74,				//ϵ��A
	~(0x0008DC74),
	0xFFF86320,			//ϵ��C
	~(0xFFF86320),
	0xffffffff,			//UID
	0,
	0, 					//des1
	~(0),				
	0,					//des2
	~(0),
	0,					//sn1
	0,					//sn2
	0					//sn3

};

///��������Сֵ
const SYS_PARA SysParaMin={		//��Сֵ
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	SYS_PARA_VERSION,	//u32 Version;	

	0, 			//�ƹ�����
	0,
	0,				//ϵ��A
	0,
	0,			//ϵ��C
	0,
	0,				//UID
	0,
	0, 					//des1
	~(0),				
	0,					//des2
	~(0),
	0,					//sn1
	0,					//sn2
	0					//sn3
};

///���������ֵ
const SYS_PARA SysParaMax={		//���ֵ
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	SYS_PARA_VERSION,	//u32 Version;	

	0x7fffffff , 					//�ƹ�����
	0xffffffff,
	0xffffffff,					//ϵ��A
	0xffffffff,
	0xffffffff,					//ϵ��C
	0xffffffff,	
	0xffffffff,					//UID
	0xffffffff,
	0xffffffff, 					//des1
	0xffffffff,				
	0xffffffff,					//des2
	0xffffffff,
	0xffffffff,					//sn1
	0xffffffff,					//sn2
	0xffffffff					//sn3
};

///�汾����ʱ������������仯�ô˺�������
void SysParaConvert(u32 ver)
{
	LoadDefaultSysConfig();//û���ҵ�������������ʱֱ�Ӽ���ȱʡֵ
}

///У�����
void VerifySysParas(void)
{
	//��У���ͺţ����ֲ������ͺ����
	//VerifySysParaToDefault(LightType);
	//VerifySysParaToDefault(ACoff);
	//VerifySysParaToDefault(CCoff);
}

///���ֲ�����Ҫת����ʹ�ã����ｫ��������Ĳ���ת����ʵ��ʹ�õı���
/*
void OnLoadConfig(void)
{
}
*/




