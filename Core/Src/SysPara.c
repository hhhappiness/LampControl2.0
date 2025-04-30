#include "SysParaCommon.h"
#include "type.h"
#include "lcd.h"


SYS_PARA 	SysPara;				///<使用中的参数表

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

///检测TrigModeList查找id号
int LightType2Id(void)
{
	int i;
	for(i=0;i<LIGHT_TYPE_NUM;i++){
		if(LightTypeList[i] == SysPara.LightType) return i;
	}
	return -1;
}


///参数表缺省值
const SYS_PARA SysParaDefault={	
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	SYS_PARA_VERSION,	//u32 Version;	

	WKS100_LI, 			//灯管类型
	~WKS100_LI,
	0x0008DC74,				//系数A
	~(0x0008DC74),
	0xFFF86320,			//系数C
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

///参数表最小值
const SYS_PARA SysParaMin={		//最小值
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	SYS_PARA_VERSION,	//u32 Version;	

	0, 			//灯管类型
	0,
	0,				//系数A
	0,
	0,			//系数C
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

///参数表最大值
const SYS_PARA SysParaMax={		//最大值
	SYS_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	SYS_PARA_VERSION,	//u32 Version;	

	0x7fffffff , 					//灯管类型
	0xffffffff,
	0xffffffff,					//系数A
	0xffffffff,
	0xffffffff,					//系数C
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

///版本升级时如果参数表发生变化用此函数升级
void SysParaConvert(u32 ver)
{
	LoadDefaultSysConfig();//没有找到对齐升级方案时直接加载缺省值
}

///校验参数
void VerifySysParas(void)
{
	//先校验型号，部分参数与型号相关
	//VerifySysParaToDefault(LightType);
	//VerifySysParaToDefault(ACoff);
	//VerifySysParaToDefault(CCoff);
}

///部分参数需要转换后使用，这里将参数表里的参数转换成实际使用的变量
/*
void OnLoadConfig(void)
{
}
*/




