#include "AppParaCommon.h"
#include "type.h"
#include "lcd.h"
#include "timer.h"
#include "SysParaCommon.h"

///ת�ٵ�λ����ֵ�б�
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

///���԰汾����ֵ�б�
const int LanguageList[LangNum] =
{
	Lang_Chinese,
	Lang_English
};


APP_PARA 	AppPara;				///<ʹ���еĲ�����
APP_PARA 	AppPara_Backup;			///<���ݲ�����

///������ȱʡֵ
const APP_PARA AppParaDefault=
{	
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//����ģʽ
	0,				//int SpeedUnit; 	//�ٶ�ģʽ��Ƶ��/ת��
	6000,			//int LampFreq;	//�ڴ���Ƶ��
	36000,			//int Rpm;		//�ڴ���ת��

	60000,			//int LineSpeed;	//���ٶ�,0.01
	300,			//int PlateLen;	//�泤mm
	1,				//int ImagePerPlate;	//ÿ���ж���ٻ���

	//��������ز���
	10,				//int Delay_SinglePulse;		//��ʱ����ʱ��
	360,			//int Div_SinglePulse;	// ��ʱϸ������10,100,360
	//�곤�ܳ���ز���
	1000,			//int Perimeter; 	//�ܳ�
	20,				//int PrintLength;//�곤
	//���ִ���������
	72,				//int GearNum;	//���ֳ���
	10,				//int Delay_Gear;	//0.1��
	//��������ز���
	1000,			//int EncodeNum;	//����������
	10,				//int Delay_Encoder;
	//wifi����
	8,
	50,
	//���������
	1,//int PulseWidth_Auto;	//0:�ֶ����� 1:�Զ�����
	50,				//int PulseWidth_Led;   //LED���������
	200,			//int PulseWidth_Tube;   //�ƹ����������
	/* ϵͳ����  */
	Lang_Chinese,		//int Language;	 //����
	CONTRAST_DEFAULT,	//int Contrast;	 //�Աȶ�����
	//���ܲ���
	PwrKey_Hit,		//int PowerKey;	//��Դ������
	10,				//int PowerOffDly; //�ػ���ʱ
	30,				//int BackLightDelay;	 //������ʱʱ��
	30,				//int PowerDelay;	 //��Դ������ʱ
	120,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LEDƵ������
	2000,			//int FreqLimit_Tube;	 //�ƹ�Ƶ������

	0,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum
};

///��������Сֵ
const APP_PARA AppParaMin=		//��Сֵ
{
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//����ģʽ
	0,				//int SpeedUnit; 	//�ٶ�ģʽ��Ƶ��/ת��
	100,				//int LampFreq;	//�ڴ���Ƶ��

	600,				//int Rpm;		//�ڴ���ת��
	100,			//int LineSpeed;	//���ٶ�,0.01
	30,				//int PlateLen;	//�泤mm		
	1,				//int ImagePerPlate;	//ÿ���ж���ٻ���
	
	//��������ز���
	5,				//int Delay_SinglePulse;		//��ʱ����ʱ��
	360,			//int Div_SinglePulse;	// ��ʱϸ������10,100,360
	//�곤�ܳ���ز���
	100,			//int Perimeter; 	//�ܳ�
	10,				//int PrintLength;//�곤
	//���ִ���������
	10,				//int GearNum;	//���ֳ���
	10,				//int Delay_Gear;	//0.1��
	//��������ز���
	20,			//int EncodeNum;	//����������
	1,				//int Delay_Encoder;
	//wifi����
	1,
	10,
	//���������
	0,//int PulseWidth_Auto;	//0:�ֶ����� 1:�Զ�����
	5,				//int PulseWidth_Led;   //LED���������
	100,			//int PulseWidth_Tube;   //�ƹ����������
	/* ϵͳ����  */
	0,				//int Language;	 //����
	CONTRAST_MIN,	//int Contrast;	 //�Աȶ�����
	//���ܲ���
	0,				//int PowerKey;	//��Դ������
	5,				//int PowerOffDly; //�ػ���ʱ
	10,				//int BackLightDelay;	 //������ʱʱ��
	30,				//int PowerDelay;	 //��Դ������ʱ
	5,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LEDƵ������
	2000,			//int FreqLimit_Tube;	 //�ƹ�Ƶ������

	0,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum	
};

///���������ֵ
const APP_PARA AppParaMax=		//���ֵ
{
	APP_PARA_MAGIC_NUM,	//u32 MagicNum;	//����
	APP_PARA_VERSION,	//u32 Version;	

	Trig_Internal, 	//int TrigMode;	//����ģʽ
	2,				//int SpeedUnit; 	//�ٶ�ģʽ��Ƶ��/ת��


	MAX_FREQ_LED*100,			//int LampFreq;	//�ڴ���Ƶ��
	MAX_FREQ_LED*600,			//int Rpm;		//�ڴ���ת��
	MAX_FREQ_LED*300,			//int LineSpeed;	//���ٶ�,0.01
	2000,			//int PlateLen;	//�泤mm	
	20,			//int ImagePerPlate;	//ÿ���ж���ٻ���
	
	//��������ز���
	355,				//int Delay_SinglePulse;		//��ʱ����ʱ��
	360,			//int Div_SinglePulse;	// ��ʱϸ������10,100,360
	//�곤�ܳ���ز���
	9999,			//int Perimeter; 	//�ܳ�
	999,				//int PrintLength;//�곤
	//���ִ���������
	100,				//int GearNum;	//���ֳ���
	999,				//int Delay_Gear;	//0.1��
	//��������ز���
	2500,			//int EncodeNum;	//����������
	2049,				//int Delay_Encoder;
	//wifi����
	16,
	100,
	//���������
	1,//int PulseWidth_Auto;	//0:�ֶ����� 1:�Զ�����
	100,				//int PulseWidth_Led;   //LED���������
	200,			//int PulseWidth_Tube;   //�ƹ����������
	/* ϵͳ����  */
	LangNum-1,		//int Language;	 //����
	CONTRAST_MAX,	//int Contrast;	 //�Աȶ�����
	//���ܲ���
	PwrKeyNum-1,	//int PowerKey;	//��Դ������
	99,				//int PowerOffDly; //�ػ���ʱ
	1000,				//int BackLightDelay;	 //������ʱʱ��
	30,				//int PowerDelay;	 //��Դ������ʱ
	MAX_WORKTIME_LED,				//int WorkTime;
	
	/* project params set */
	200,			//int FreqLimit_Led;	 //LEDƵ������
	3000,			//int FreqLimit_Tube;	 //�ƹ�Ƶ������

	1,				//int LightType;
	//u32 Reserved[APP_PARA_SIZE/4-26];
	//u32 Sum;		 //check sum};
};

///�汾����ʱ������������仯�ô˺�������
void AppParaConvert(u32 ver)
{
	LoadDefaultConfig();//û���ҵ�������������ʱֱ�Ӽ���ȱʡֵ
}

///У�����
void VerifyParas(void)
{
	//��У���ͺţ����ֲ������ͺ����
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

///���ֲ�����Ҫת����ʹ�ã����ｫ��������Ĳ���ת����ʵ��ʹ�õı���
void OnLoadConfig(void)
{	
	image_num = AppPara.ImagePerPlate ;

	max_strobe_power = 100*AppPara.MaxPower;

}
