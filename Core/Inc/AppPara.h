#ifndef _APP_PARA_H_
#define _APP_PARA_H_

#include "type.h"
#define APP_PARA_MAGIC_NUM 		FOURCHAR2INT('L','A','M','2')	//Lamp Control 2
#define APP_PARA_VERSION		(1<<16 | 4)	//1.2
#define APP_PARA_ADDRESS		0x0800F800
//#define APP_PARA_ADDRESS		0x0801F800

#define APP_PARA_SIZE			128

///����ģʽ���塣��Щ����ģʽ�²���������ͬ����λ�������ͬʱ�Ƚ϶��еʽ��
#define Trig_Internal 		0x01
/*
#define Trig_SinglePulse	0x02
#define Trig_Perimeter		0x04
#define Trig_Gear			0x08
#define Trig_Encoder		0x10
*/
//#define Trig_Wifi			0x02

#define TrigModeNum			1

///����TrigMode�Ƿ���������磺IsTrigMode(Trig_Internal|Trig_Perimeter)
#define IsTrigMode(x) ((AppPara.TrigMode &(x))!=0)

///��������ö�٣�ȱʡ������
enum{
	Lang_Chinese,///<����
	Lang_English,///<Ӣ��
	LangNum		///<��ǰ֧�ֵ���������
};

///���������ö��
/*
enum {
	Light_LED,	///<LED
	Light_Tube,	///<�ƹ�
	LightTypeNum
};
*/

#define MAX_FREQ_LED	200		//LED���Ƶ��
#define MAX_WORKTIME_LED	600 //LED�ͺŹ���ʱ������

///��Դ������ö��
enum {
	PwrKey_Press,
	PwrKey_Hit,
	PwrKeyNum
};

///�ٶȵĵ�λ����ö��
typedef enum {
    Unit_Hz,	//Ƶ�ʣ���HzΪ��λ
    Unit_rpm, 	//ת�٣���rpmΪ��λ
	Unit_mpmin,	//���ٶȣ���m/min��λ
	SpeedUnitNum ///<֧�ֵĵ�λ������
}SpeedUnit_t;

///������ṹ��
typedef struct{
	u32 MagicNum;	///<����
	u32 Version;	///<������汾
	int TrigMode;	///<����ģʽ
	int SpeedUnit; 	///<�ٶ�ģʽ��Ƶ��/ת��
	int LampFreq;		//<�ڴ���Ƶ��
	int Rpm;		//�ڴ���ת��
	int LineSpeed;	//���ٶ�,0.01
	int PlateLen;	//�泤mm
	int ImagePerPlate;	//ÿ���ж���ٻ���
	//��������ز���
	int Delay_SinglePulse;		//��ʱ����ʱ��
	int Div_SinglePulse;	// ��ʱϸ������10,100,360
	//�곤�ܳ���ز���
	int Perimeter; 	//�ܳ�
	int PrintLength;//�곤
	//���ִ���������
	int GearNum;	//���ֳ���
	int Delay_Gear;	//0.1��
	//��������ز���
	int EncodeNum;	//����������
	int Delay_Encoder;
	//wifi����
	int rfeq;
	int Delay_Wifi;
	//���������
	int PulseWidth_Auto;	//0:�ֶ����� 1:�Զ�����
	int PulseWidth_LED;   //LED���������
	int PulseWidth_Tube;   //�ƹ����������
	/* ϵͳ����  */
	int Language;	 //����
	int Contrast;	 //�Աȶ�����
	//���ܲ���
	int PowerKey;			//��Դ�����ž���,�ɿ���ͣ/��һ����,��һ��ͣ
	int PowerOffDly; 		//��������ʱxx���ӹػ�
	int BackLightDelay;	 	//�ް�������xx���Ӻ�رձ���
	int PowerDelay;	 		//��Դ������ʱ
	int WorkTime;			//������xxx���Ӻ���ͣ��
	
	/* project params set */
	int FreqLimit_Led;	 //LEDƵ������
	int MaxPower;	 //�ƹ�Ƶ������
	
	u32 Reserved[APP_PARA_SIZE/4-29];
	u32 Sum;		 //check sum
}APP_PARA;

extern const int SpeedUnitList[SpeedUnitNum];
extern const char * const SpeedUnitStr[SpeedUnitNum];
extern const int LanguageList[LangNum];
//extern int LightTypeList[LightTypeNum];
#endif
