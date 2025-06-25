
#ifndef _SYS_PARA_H_
#define _SYS_PARA_H_

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "type.h"
#define SYS_PARA_MAGIC_NUM 		FOURCHAR2INT('L','A','M','2')	//Lamp Control 2
#define SYS_PARA_VERSION		(1<<16 | 1)	//1.1
//#define SYS_PARA_ADDRESS		0x0800FC00
#define SYS_PARA_ADDRESS		0x0801F800
#define LAYSER




#define 		Type_LIGHT_SHIFT		0			//bit: 0
#define 		Type_LIGHT_MASK		0x01
#define 		Type_TRIG_SHIFT	1			//bit: 3:1
#define 		Type_TRIG_MASK	0x02
#define 		Type_BAT_SHIFT	4			//bit: 4
#define		Type_BAT_MASK		0x10
#define 		Type_WIFI_SHIFT	5
#define 		Type_WIFI_MASK	0x20
#define		Type_UV_SHIFT		6
#define 		Type_UV_MASK		0x40
#define		Type_LED19_SHIFT	7
#define		Type_LED19_MASK	0x80

#define 		TYPE_LIGHT_LED 	0
#define 		TYPE_LIGHT_TUBE	1
#define		TYPE_TRIG_INT	0
#define		TYPE_TRIG_EXT	1
#define		TYPE_BAT_2000	0
#define		TYPE_BAT_2600 	1

#define 		TYPE_WIFI_NO	0
#define		TYPE_WIFI_YES	1
#define		TYPE_UV_NO	0
#define		TYPE_UV_YES 	1
#define		TYPE_LED19_NO	0
#define		TYPE_LED19_YES 	1

#define LIGHT_TYPE_NUM  3   //当前支持的型号数量

#define 		WKS100_LI0  ((TYPE_LIGHT_LED << Type_LIGHT_SHIFT) | (TYPE_TRIG_INT << Type_TRIG_SHIFT) | (TYPE_BAT_2000 << Type_BAT_SHIFT))
#define 		WKS100_LI1  ((TYPE_LIGHT_LED << Type_LIGHT_SHIFT) | (TYPE_TRIG_INT << Type_TRIG_SHIFT) | (TYPE_BAT_2600 << Type_BAT_SHIFT))
#define		WKS100_LI     ((TYPE_LED19_YES << Type_LED19_SHIFT)|(TYPE_LIGHT_LED << Type_LIGHT_SHIFT) | (TYPE_TRIG_INT << Type_TRIG_SHIFT) | (TYPE_BAT_2000 << Type_BAT_SHIFT))

//#define IsTypeLed()	((((SysPara.LightType & Type_LIGHT_MASK) >> Type_LIGHT_SHIFT) == TYPE_LIGHT_LED) ? 1 : 0)
//#define IsTrigInt()	((((SysPara.LightType & Type_TRIG_MASK) >> Type_TRIG_SHIFT) == TYPE_TRIG_INT) ? 1 : 0)
//#define IsBAT2000()	((((SysPara.LightType & Type_BAT_MASK) >> Type_BAT_SHIFT) == TYPE_BAT_2000) ? 1 : 0)

//#define IsWKS100(x)	(((SysPara.LightType & 0xff) == x )?1:0)

//#define IsWKSWIFI()	(((SysPara.LightType & Type_WIFI_MASK)  ==  Type_WIFI_MASK) ? 1 : 0)




///参数表结构体
typedef struct{
	u32 MagicNum;	///<幻数
	u32 Version;	///<参数表版本

	int LightType;
	int nLightType;
	int ACoff;
	int nACoff;
	int CCoff;
	int nCCoff;
	u32 UID;
	u32 nUID;
	u32 DesByte1;
	u32 nDesByte1;
	u32 DesByte2;
	u32 nDesByte2;
	u32 SnByte1;
	u32 SnByte2;
	u32 SnByte3;

}SYS_PARA;

void VerifySysParas(void);
void SysParaConvert(u32 ver);
extern const char * LightType_StrList[LIGHT_TYPE_NUM];
extern const int LightTypeList[LIGHT_TYPE_NUM];
extern int LightType2Id(void);
#ifdef __cplusplus
 }
#endif 
 
#endif
