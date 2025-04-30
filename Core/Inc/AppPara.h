#ifndef _APP_PARA_H_
#define _APP_PARA_H_

#include "type.h"
#define APP_PARA_MAGIC_NUM 		FOURCHAR2INT('L','A','M','2')	//Lamp Control 2
#define APP_PARA_VERSION		(1<<16 | 4)	//1.2
#define APP_PARA_ADDRESS		0x0800F800
//#define APP_PARA_ADDRESS		0x0801F800

#define APP_PARA_SIZE			128

///触发模式定义。有些触发模式下部分设置相同，用位定义便于同时比较多个械式。
#define Trig_Internal 		0x01
/*
#define Trig_SinglePulse	0x02
#define Trig_Perimeter		0x04
#define Trig_Gear			0x08
#define Trig_Encoder		0x10
*/
//#define Trig_Wifi			0x02

#define TrigModeNum			1

///测试TrigMode是否相符，例如：IsTrigMode(Trig_Internal|Trig_Perimeter)
#define IsTrigMode(x) ((AppPara.TrigMode &(x))!=0)

///界面语言枚举，缺省是中文
enum{
	Lang_Chinese,///<中文
	Lang_English,///<英文
	LangNum		///<当前支持的语言数量
};

///闪光灯类型枚举
/*
enum {
	Light_LED,	///<LED
	Light_Tube,	///<灯管
	LightTypeNum
};
*/

#define MAX_FREQ_LED	200		//LED最大频率
#define MAX_WORKTIME_LED	600 //LED型号工作时间上限

///电源键功能枚举
enum {
	PwrKey_Press,
	PwrKey_Hit,
	PwrKeyNum
};

///速度的单位种类枚举
typedef enum {
    Unit_Hz,	//频率，以Hz为单位
    Unit_rpm, 	//转速，以rpm为单位
	Unit_mpmin,	//线速度，以m/min单位
	SpeedUnitNum ///<支持的单位种类数
}SpeedUnit_t;

///参数表结构体
typedef struct{
	u32 MagicNum;	///<幻数
	u32 Version;	///<参数表版本
	int TrigMode;	///<触发模式
	int SpeedUnit; 	///<速度模式，频率/转速
	int LampFreq;		//<内触发频率
	int Rpm;		//内触发转速
	int LineSpeed;	//线速度,0.01
	int PlateLen;	//版长mm
	int ImagePerPlate;	//每版有多个少画面
	//单脉冲相关参数
	int Delay_SinglePulse;		//延时闪光时间
	int Div_SinglePulse;	// 延时细分数，10,100,360
	//标长周长相关参数
	int Perimeter; 	//周长
	int PrintLength;//标长
	//齿轮传感器参数
	int GearNum;	//齿轮齿数
	int Delay_Gear;	//0.1齿
	//编码器相关参数
	int EncodeNum;	//编码器线数
	int Delay_Encoder;
	//wifi参数
	int rfeq;
	int Delay_Wifi;
	//输出脉冲宽度
	int PulseWidth_Auto;	//0:手动调节 1:自动调节
	int PulseWidth_LED;   //LED输出脉冲宽度
	int PulseWidth_Tube;   //灯管输出脉冲宽度
	/* 系统参数  */
	int Language;	 //语言
	int Contrast;	 //对比度设置
	//节能参数
	int PowerKey;			//电源键按着就闪,松开就停/按一下闪,按一下停
	int PowerOffDly; 		//不工作延时xx分钟关机
	int BackLightDelay;	 	//无按键操作xx分钟后关闭背光
	int PowerDelay;	 		//电源按键延时
	int WorkTime;			//持续闪xxx分钟后暂停，
	
	/* project params set */
	int FreqLimit_Led;	 //LED频率上限
	int MaxPower;	 //灯管频率上限
	
	u32 Reserved[APP_PARA_SIZE/4-29];
	u32 Sum;		 //check sum
}APP_PARA;

extern const int SpeedUnitList[SpeedUnitNum];
extern const char * const SpeedUnitStr[SpeedUnitNum];
extern const int LanguageList[LangNum];
//extern int LightTypeList[LightTypeNum];
#endif
