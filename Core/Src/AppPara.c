#include "AppParaCommon.h"
#include "type.h"
#include "lcd.h"
#include "timer.h"
#include "SysParaCommon.h"

///转速单位参数值列表
const int SpeedUnitList[SpeedUnitNum]=
{
	Unit_Hz,	//Hz
  Unit_rpm, 	//Rpm
	Unit_mpmin,	//m/min
};

const char * const SpeedUnitStr[SpeedUnitNum]=
{
	"Hz",
  "rpm",
	"m/min"
};

///语言版本参数值列表
const int LanguageList[LangNum] =
{
	Lang_Chinese,
	Lang_English
};


APP_PARA 	AppPara;				///<使用中的参数表
APP_PARA 	AppPara_Backup;			///<备份参数表

///参数表缺省值
const APP_PARA AppParaDefault=
{	
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//触发模式
	0,				//int SpeedUnit; 	//速度模式，频率/转速
	6000,			//int LampFreq;	//内触发频率
	36000,			//int Rpm;		//内触发转速

	60000,			//int LineSpeed;	//线速度,0.01
	300,			//int PlateLen;	//版长mm
	1,				//int ImagePerPlate;	//每版有多个少画面

	//单脉冲相关参数
	10,				//int Delay_SinglePulse;		//延时闪光时间
	360,			//int Div_SinglePulse;	// 延时细分数，10,100,360
	//标长周长相关参数
	1000,			//int Perimeter; 	//周长
	20,				//int PrintLength;//标长
	//齿轮传感器参数
	72,				//int GearNum;	//齿轮齿数
	10,				//int Delay_Gear;	//0.1齿
	//编码器相关参数
	1000,			//int EncodeNum;	//编码器线数
	10,				//int Delay_Encoder;
	//wifi参数
	8,
	50,
	//输出脉冲宽度
	1,//int PulseWidth_Auto;	//0:手动调节 1:自动调节
	50,				//int PulseWidth_Led;   //LED输出脉冲宽度
	200,			//int PulseWidth_Tube;   //灯管输出脉冲宽度
	/* 系统参数  */
	Lang_Chinese,		//int Language;	 //语言
	CONTRAST_DEFAULT,	//int Contrast;	 //对比度设置
	//节能参数
	PwrKey_Hit,		//int PowerKey;	//电源键功能
	10,				//int PowerOffDly; //关机延时
	30,				//int BackLightDelay;	 //背光延时时间
	30,				//int PowerDelay;	 //电源按键延时
	120,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LED频率上限
	2000,			//int FreqLimit_Tube;	 //灯管频率上限

	0,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum
};

///参数表最小值
const APP_PARA AppParaMin=		//最小值
{
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//触发模式
	0,				//int SpeedUnit; 	//速度模式，频率/转速
	100,				//int LampFreq;	//内触发频率

	600,				//int Rpm;		//内触发转速
	100,			//int LineSpeed;	//线速度,0.01
	30,				//int PlateLen;	//版长mm		
	1,				//int ImagePerPlate;	//每版有多个少画面
	
	//单脉冲相关参数
	5,				//int Delay_SinglePulse;		//延时闪光时间
	360,			//int Div_SinglePulse;	// 延时细分数，10,100,360
	//标长周长相关参数
	100,			//int Perimeter; 	//周长
	10,				//int PrintLength;//标长
	//齿轮传感器参数
	10,				//int GearNum;	//齿轮齿数
	10,				//int Delay_Gear;	//0.1齿
	//编码器相关参数
	20,			//int EncodeNum;	//编码器线数
	1,				//int Delay_Encoder;
	//wifi参数
	1,
	10,
	//输出脉冲宽度
	0,//int PulseWidth_Auto;	//0:手动调节 1:自动调节
	5,				//int PulseWidth_Led;   //LED输出脉冲宽度
	100,			//int PulseWidth_Tube;   //灯管输出脉冲宽度
	/* 系统参数  */
	0,				//int Language;	 //语言
	CONTRAST_MIN,	//int Contrast;	 //对比度设置
	//节能参数
	0,				//int PowerKey;	//电源键功能
	5,				//int PowerOffDly; //关机延时
	10,				//int BackLightDelay;	 //背光延时时间
	30,				//int PowerDelay;	 //电源按键延时
	5,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LED频率上限
	2000,			//int FreqLimit_Tube;	 //灯管频率上限

	0,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum	
};

///参数表最大值
const APP_PARA AppParaMax=		//最大值
{
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//幻数
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//触发模式
	2,				//int SpeedUnit; 	//速度模式，频率/转速


	MAX_FREQ_LED*100,			//int LampFreq;	//内触发频率
	MAX_FREQ_LED*600,			//int Rpm;		//内触发转速
	MAX_FREQ_LED*300,			//int LineSpeed;	//线速度,0.01
	2000,			//int PlateLen;	//版长mm	
	20,			//int ImagePerPlate;	//每版有多个少画面
	
	//单脉冲相关参数
	355,				//int Delay_SinglePulse;		//延时闪光时间
	360,			//int Div_SinglePulse;	// 延时细分数，10,100,360
	//标长周长相关参数
	9999,			//int Perimeter; 	//周长
	999,				//int PrintLength;//标长
	//齿轮传感器参数
	100,				//int GearNum;	//齿轮齿数
	999,				//int Delay_Gear;	//0.1齿
	//编码器相关参数
	2500,			//int EncodeNum;	//编码器线数
	2049,				//int Delay_Encoder;
	//wifi参数
	16,
	100,
	//输出脉冲宽度
	1,//int PulseWidth_Auto;	//0:手动调节 1:自动调节
	100,				//int PulseWidth_Led;   //LED输出脉冲宽度
	200,			//int PulseWidth_Tube;   //灯管输出脉冲宽度
	/* 系统参数  */
	LangNum-1,		//int Language;	 //语言
	CONTRAST_MAX,	//int Contrast;	 //对比度设置
	//节能参数
	PwrKeyNum-1,	//int PowerKey;	//电源键功能
	99,				//int PowerOffDly; //关机延时
	1000,				//int BackLightDelay;	 //背光延时时间
	30,				//int PowerDelay;	 //电源按键延时
	MAX_WORKTIME_LED,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LED频率上限
	3000,			//int FreqLimit_Tube;	 //灯管频率上限

	1,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum};
};

///版本升级时如果参数表发生变化用此函数升级
void AppParaConvert(u32 ver)
{
	LoadDefaultConfig();//没有找到对齐升级方案时直接加载缺省值
}

///校验参数
void VerifyParas(void)
{
	//先校验型号，部分参数与型号相关
	//VerifyParaToDefault(LightType);
	AppPara.TrigMode = Trig_Internal;
	VerifyParaToDefault(SpeedUnit);
	VerifyParaToDefault(LampFreq);
	VerifyParaToDefault(Rpm);
	VerifyParaToDefault(Delay_SinglePulse);
	VerifyParaToDefault(Div_SinglePulse);
	VerifyParaToDefault(Perimeter);
	VerifyParaToDefault(PrintLength);
	VerifyParaToDefault(ImagePerPlate);
	VerifyParaToDefault(rfeq);
	VerifyParaToDefault(Delay_Wifi);
	
	VerifyParaToDefault(GearNum);
	VerifyParaToDefault(Delay_Gear);
	VerifyParaToDefault(EncodeNum);
	VerifyParaToDefault(Delay_Encoder);
	VerifyParaToDefault(PulseWidth_Auto);
	VerifyParaToDefault(PulseWidth_LED);
	VerifyParaToDefault(PulseWidth_Tube);
	VerifyParaToDefault(Language);
	VerifyParaToDefault(Contrast);

	VerifyParaToDefault(PowerKey);
	VerifyParaToDefault(PowerOffDly);
	VerifyParaToDefault(BackLightDelay);
	VerifyParaToDefault(PowerDelay);
	VerifyParaToDefault(WorkTime);
	VerifyParaToDefault(FreqLimit_Led);
	VerifyParaToDefault(MaxPower);
	
	
}

///部分参数需要转换后使用，这里将参数表里的参数转换成实际使用的变量
void OnLoadConfig(void)
{	
	image_num = AppPara.ImagePerPlate ;

	max_strobe_power = 100*AppPara.MaxPower;

}
