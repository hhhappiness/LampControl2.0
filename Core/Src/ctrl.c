#include "ctrl.h"
#include "stm32.h"
#include "bsp.h"
#include "rtc_int.h"
#include "lcd.h"
#include "rtc.h"
#include "spimaster.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"
#include "board_config.h"
#include "adc.h"
#include "timer.h"
#include "string.h"
#include "Reset.h"
#include "share.h"
extern void StartInternalTrig();
//int Status_MCU = Status_WorkStall;

Status_MCU_t Status_MCU;
extern TIM_HandleTypeDef htim2, htim3, htim4;
#define ENCRYPT_BASE 0x0800FC00


//volatile const static u32 CPUIDEncrypt = 0xFFFFFFFF;
#define CPUIDEncrypt *(vu32 *)ENCRYPT_BASE
//#define UID_BASE 0x1FFFF7E8


/*
_TimeStruct RunTime_Cur ;
_TimeStruct RunTime_Old = 0;
#define RunTime_TH	8*3600 //8 hours
*/

//���ٶ�תƵ�ʣ�0.01HzΪ��λ
//��ʽ��Freq=LineSpeed*N*10*100/(PlateLen*60)������LineSpeed*10��ת��mm��泤��Ӧ,��ĸ��60�ǽ���ת���룬*100�ǽ�Hzת��0.01Hz
//�˷�ʽ�����ڴ�ӡ���������ڴ�����LineSpeed2Clocks()������һ������
u32 LineSpeed2Hz001(void)
{
	u32 Freq;//0.01Hz
	u32 Denominator = AppPara.PlateLen*60;		//��ĸ
	//u32 Numerator = (u32)(AppPara.LineSpeed)*10*100; //����
	u32 Numerator = (u32)(AppPara.LineSpeed)*10000;
	Freq =(Numerator + Denominator/2)/ Denominator ;
	return Freq;	
}

//���ٶ�תʱ����
//Ƶ�ʣ�Freq=LineSpeed*ImagePerPlate*10/(PlateLen*60)������LineSpeed*10��ת��mm��泤��Ӧ,��ĸ��60�ǽ���ת���룬�õ���Freq��Hz
//ת��ʱ������NextTrigClks=TIMXCLK/Freq��
//����NextTrigClks=TIMXCLK*PlateLen*60/(LineSpeed*N*10)=TIMXCLK*PlateLen*6/(LineSpeed*ImagePerPlate)
void LineSpeed2Clocks(void)
{
	uint32_t Denominator ;
	long long Numerator ;
//	u32 Linespeed2Rpm,tmp;	
//	u8 div_flg = 0 ;

	//int Denominator = AppPara.LineSpeed*AppPara.ImagePerPlate;		//��ĸ
	Denominator = AppPara.LineSpeed*AppPara.ImagePerPlate*10;
	Numerator =(long long)TIMXCLK*(AppPara.PlateLen*6);	//���ӣ���long long��ֹ���

	/*
	��Ȼ: TrigClks = VerifyInterFreq(NextTrigClks,TrigLimitWidth);�����Ƶ�����ƣ���Ȼ�����ٶ�ģʽ�£���Ƶ��Ҳ�в��Եĵط�
	���Է��֣�
	���ٶ�ģʽ���泤 240mm�����ٶ�2560.0 m/min, 16/�����ʱ��
	���ۼ��㣺177.7Hz�� LED������200HzƵ��
	ʵ����� 189.0Hz�� 1280.0m/min��ʱ����177.7Hz
	
	50625 -160
	25313 x2 -320
	12656 x4  - 640
	6328 x8 - 1280
	3164 x15 - 2560
	1582  x 29 - 5120

	���������⣺ֻҪ�����������У���һ����Ҫ��2����
	*/
	NextTrigClks = (Numerator + Denominator/2)/Denominator;	//��������

	#if 0
	//1. ���ٶ���ת��ΪƵ��
	//���ٶ�a�� ��λ: 0.1m/min, a*100 = mm/min
	//���ٶ� ���� ת�٣� a*100 /�泤�� =  ����ת/min
	Linespeed2Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
	//2. ��Ƶ�ʽ�������
	//ת�ٻ���Ƶ��b, ��λ0.01Hz��  b = a*100/60s = a*5/3 ;
	tmp = Linespeed2Rpm*5/3  ;
	//���ٶȺͰ峤�йأ�ÿ�弸�����棬�൱�����˱�Ƶ
	tmp = tmp * AppPara.ImagePerPlate ;
	//�����ٶ�ģʽ�£��Գ������Ƶ�ʽ������ƣ����յ������Ƶ��&&��Ƶ����
	
	do {
		if(tmp > AppParaMax.LampFreq)
			//tmp = (tmp+1)/2 ;
			tmp >>= 1 ;
		else
			div_flg = 1 ;
	}while( !div_flg ) ;

	//3. ����Ƶ�ʼ���NextTrigClks
	NextTrigClks = TIM2_Hz2clk(tmp);
	#endif
}
inline void ClosePwmTimer(void)
{

	  /* Disable the TIM Update interrupt */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);

    TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_DISABLE);  //�ر�ͨ��2�����

	/* Disable the Peripheral CCER and CEN �Ĵ�����λ*/
	__HAL_TIM_DISABLE(&htim2);
	__HAL_TIM_DISABLE(&htim3);
	//״̬�Ĵ�����λ
	TIM2->SR = 0;
	TIM3->SR = 0;

	RCC->APB1ENR1 &= ~(RCC_APB1ENR1_TIM2EN|RCC_APB1ENR1_TIM3EN);  //�ر�TIM2,3ʱ��
}


inline void OpenPwmTimer(void)
{

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN|RCC_APB1ENR1_TIM3EN; //��TIM2,3ʱ�ӣ�Enable��ͨ��2�����ں�������������

}

void StartToFlash(void)
{
		
		// �����ж�ʱ��
		OpenPwmTimer();
		// �����������
		Updata_OutPusle();
		if(IsTrigMode(Trig_Internal)) {
			if(AppPara.SpeedUnit == Unit_Hz) {
				NextTrigClks = TIM2_Hz2clk(AppPara.LampFreq);
			} else if(AppPara.SpeedUnit == Unit_rpm) {
				NextTrigClks = TIM2_rpm2clk(AppPara.Rpm);
			} else {
				LineSpeed2Clocks();
			}

			StartInternalTrig();
		}
		Status_MCU = Status_WorkFlash; // ����״̬Ϊ����״̬

}
	// ���ö�ʱ��1

	// ���ö�ʱ��2
void StopToFlash(void)
	// ���ö�ʱ��3
{
	/* ֹͣ��ʱ��������Ѿ����У� */
	ClosePwmTimer();
	Status_MCU = Status_WorkStall;
	//ֹͣ�����ʱ����Ҫ����Strobe��״̬��Led�͵ƹܵĲ�ͬ
	GPA_O(4,0); //�ر�Strobe���
	WorkEn = 0;
}

void ShutDown(void)
{

	// if(CmpAppParam())
	// 	SaveConfig();
	//if(CmpSysParam())
		//SaveSysConfig();


	BackLightOff();
	LcdResetOn();
	LCD_A0(0);
	LCD_CSB(0);
	PowerOff();
	while(1); //�ϵ��������δֹͣ������ѭ��

}
#if 0

void WriteEncrypt(void)  
{   
    //��һ����д:��UIDд�뵽Flash��  
    if(CPUIDEncrypt==0xFFFFFFFF)  
   {  
        u32 CpuID[3];     
				u32 EncryptCode;	   
        //��ȡCPUΨһ��ID  
        CpuID[0]=*(vu32*)(UID_BASE);  
        CpuID[1]=*(vu32*)(UID_BASE+4);  
        CpuID[2]=*(vu32*)(UID_BASE+8);          
  
        //�����㷨,�ܼ򵥵ļ����㷨  
        EncryptCode=(CpuID[0]>>3)+(CpuID[1]>>2)+(CpuID[2]>>1);     
        FLASH_Unlock();  
        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);  
        FLASH_ProgramWord((u32)&CPUIDEncrypt, EncryptCode);  
        FLASH_Lock();  
    }  
}  


//�жϼ���  
BOOL JudgeEncrypt(void)  
{         
    u32 CpuID[4];         
    //��ȡCPUΨһ��ID  
    CpuID[0]=*(vu32*)(UID_BASE);  
    CpuID[1]=*(vu32*)(UID_BASE+4);  
    CpuID[2]=*(vu32*)(UID_BASE+8);      
    //�����㷨,�ܼ򵥵ļ����㷨  
    CpuID[3]=(CpuID[0]>>3)+(CpuID[1]>>2)+(CpuID[2]>>1);     
    //���Flash�е�UID�Ƿ�Ϸ�   
   // return (CPUIDEncrypt == CpuID[3]);  
   return (SysPara.UID == CpuID[3]);
}     


void Check_Flash(void)
{
	 FlagStatus status = RESET;
	status = FLASH_GetReadOutProtectionStatus();

	if(status != SET)
	{ 
		FLASH_Unlock(); /* Flash ���� */
		/* ENABLE the ReadOut Protection */
		FLASH_ReadOutProtection(ENABLE);
		//������ʹ��
		FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);
	}
} 

#endif
BOOL DecodeDES(void)
{
	int i;
	unsigned char key[16] = { 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xFE, 0x1B, 0x63, 0xCB, 0x49, 0xE3, 0xB9, 0x0A};
	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];	
	ShareInfo.setkey(key,16);
	memcpy(&encoded[0],&SysPara.DesByte1,4);
	memcpy(&encoded[4],&SysPara.DesByte2,4);
	memcpy(&src[0],&SysPara.SnByte1,4);
	memcpy(&src[4],&SysPara.SnByte2,4);
	ShareInfo.decrypt(decoded,encoded);//����
	for(i=0;i<4;i++) {
		decoded[i] -= 1;
		decoded[1+i] += 1;
	}
	for(i=0;i<8;i++) {
		if(src[i] != decoded[i])
			return FALSE;
	}
	return TRUE;
		
}


void ReBoot(void)
{
	if (((*(__IO uint32_t*)BOOT_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{ 
		__disable_irq();
		Reset();
	}	
}




