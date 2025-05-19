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

//int Status_MCU = Status_WorkStall;

Status_MCU_t Status_MCU;

#define ENCRYPT_BASE 0x0800FC00


//volatile const static u32 CPUIDEncrypt = 0xFFFFFFFF;
#define CPUIDEncrypt *(vu32 *)ENCRYPT_BASE
//#define UID_BASE 0x1FFFF7E8


/*
_TimeStruct RunTime_Cur ;
_TimeStruct RunTime_Old = 0;
#define RunTime_TH	8*3600 //8 hours
*/

//线速度转频率，0.01Hz为单位
//公式：Freq=LineSpeed*N*10*100/(PlateLen*60)，其中LineSpeed*10是转到mm与版长对应,分母的60是将分转到秒，*100是将Hz转成0.01Hz
//此分式仅用于打印换算结果，内触发用LineSpeed2Clocks()，少做一个除法
u32 LineSpeed2Hz001(void)
{
	u32 Freq;//0.01Hz
	u32 Denominator = AppPara.PlateLen*60;		//分母
	//u32 Numerator = (u32)(AppPara.LineSpeed)*10*100; //分子
	u32 Numerator = (u32)(AppPara.LineSpeed)*10000;
	Freq =(Numerator + Denominator/2)/ Denominator ;
	return Freq;	
}

//线速度转时钟数
//频率：Freq=LineSpeed*ImagePerPlate*10/(PlateLen*60)，其中LineSpeed*10是转到mm与版长对应,分母的60是将分转到秒，得到的Freq是Hz
//转成时钟数：NextTrigClks=TIMXCLK/Freq，
//于是NextTrigClks=TIMXCLK*PlateLen*60/(LineSpeed*N*10)=TIMXCLK*PlateLen*6/(LineSpeed*ImagePerPlate)
void LineSpeed2Clocks(void)
{
	uint32_t Denominator ;
	long long Numerator ;
//	u32 Linespeed2Rpm,tmp;	
//	u8 div_flg = 0 ;

	//int Denominator = AppPara.LineSpeed*AppPara.ImagePerPlate;		//分母
	Denominator = AppPara.LineSpeed*AppPara.ImagePerPlate*10;
	Numerator =(long long)TIMXCLK*(AppPara.PlateLen*6);	//分子，用long long防止溢出

	/*
	虽然: TrigClks = VerifyInterFreq(NextTrigClks,TrigLimitWidth);有输出频率限制，仍然在线速度模式下，倍频有也有不对的地方
	测试发现：
	线速度模式，版长 240mm，线速度2560.0 m/min, 16/画面的时候
	理论计算：177.7Hz， LED限制在200Hz频率
	实际输出 189.0Hz， 1280.0m/min的时候是177.7Hz
	
	50625 -160
	25313 x2 -320
	12656 x4  - 640
	6328 x8 - 1280
	3164 x15 - 2560
	1582  x 29 - 5120

	理解的有问题：只要是整数倍就行，不一定非要是2的幂
	*/
	NextTrigClks = (Numerator + Denominator/2)/Denominator;	//四舍五入

	#if 0
	//1. 线速度先转换为频率
	//线速度a： 单位: 0.1m/min, a*100 = mm/min
	//线速度 折算 转速： a*100 /版长度 =  多少转/min
	Linespeed2Rpm =( (AppPara.LineSpeed*100 + AppPara.PlateLen/2)/ AppPara.PlateLen) ;
	//2. 对频率进行限制
	//转速换成频率b, 单位0.01Hz，  b = a*100/60s = a*5/3 ;
	tmp = Linespeed2Rpm*5/3  ;
	//线速度和板长有关，每板几个画面，相当于做了倍频
	tmp = tmp * AppPara.ImagePerPlate ;
	//在线速度模式下，对超过最大频率进行限制，按照低于最大频率&&分频处理
	
	do {
		if(tmp > AppParaMax.LampFreq)
			//tmp = (tmp+1)/2 ;
			tmp >>= 1 ;
		else
			div_flg = 1 ;
	}while( !div_flg ) ;

	//3. 根据频率计算NextTrigClks
	NextTrigClks = TIM2_Hz2clk(tmp);
	#endif
}

void StartToFlash(void)
{
		/*
		if(IsTrigMode(Trig_SinglePulse)) {
			TrigAngle = AngleCov(AppPara.Delay_SinglePulse);
		}
		*/
		//BoostOn();
		OpenAllTimer();
		Updata_OutPusle();
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
		if(IsTrigMode(Trig_Internal)) {
			if(AppPara.SpeedUnit == Unit_Hz) {
				NextTrigClks = TIM2_Hz2clk(AppPara.LampFreq);
			} else if(AppPara.SpeedUnit == Unit_rpm) {
				NextTrigClks = TIM2_rpm2clk(AppPara.Rpm);
			} else {
				LineSpeed2Clocks();
			}
		}
			
		if(IsTrigMode(Trig_Internal)) {
			StartInternalTrig();
		}
		/*
		else if(IsTrigMode(Trig_Wifi)) {
				if(wifi_recv_flag ) {
					NextTrigClks = TIM2_Hz2clk(wifi_freq);
					StartInternalTrig();
				}
			}
		else if(IsTrigMode(Trig_Perimeter)) {
			if(Trig_Perimeter_En == 1) {
				StartInternalTrig();
			}

		}

		*/
		Status_MCU = Status_WorkFlash;

}

void StopToFlash(void)
{
	//BoostOff();
	CloseAllTimer();
	Status_MCU = Status_WorkStall;
	//停止输出的时候，需要控制Strobe的状态，Led和灯管的不同
	SwitchStrobeGPIO();
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

}
#if 0

void WriteEncrypt(void)  
{   
    //第一次烧写:将UID写入到Flash中  
    if(CPUIDEncrypt==0xFFFFFFFF)  
   {  
        u32 CpuID[3];     
				u32 EncryptCode;	   
        //获取CPU唯一的ID  
        CpuID[0]=*(vu32*)(UID_BASE);  
        CpuID[1]=*(vu32*)(UID_BASE+4);  
        CpuID[2]=*(vu32*)(UID_BASE+8);          
  
        //加密算法,很简单的加密算法  
        EncryptCode=(CpuID[0]>>3)+(CpuID[1]>>2)+(CpuID[2]>>1);     
        FLASH_Unlock();  
        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);  
        FLASH_ProgramWord((u32)&CPUIDEncrypt, EncryptCode);  
        FLASH_Lock();  
    }  
}  


//判断加密  
BOOL JudgeEncrypt(void)  
{         
    u32 CpuID[4];         
    //获取CPU唯一的ID  
    CpuID[0]=*(vu32*)(UID_BASE);  
    CpuID[1]=*(vu32*)(UID_BASE+4);  
    CpuID[2]=*(vu32*)(UID_BASE+8);      
    //加密算法,很简单的加密算法  
    CpuID[3]=(CpuID[0]>>3)+(CpuID[1]>>2)+(CpuID[2]>>1);     
    //检查Flash中的UID是否合法   
   // return (CPUIDEncrypt == CpuID[3]);  
   return (SysPara.UID == CpuID[3]);
}     


void Check_Flash(void)
{
	 FlagStatus status = RESET;
	status = FLASH_GetReadOutProtectionStatus();

	if(status != SET)
	{ 
		FLASH_Unlock(); /* Flash 解锁 */
		/* ENABLE the ReadOut Protection */
		FLASH_ReadOutProtection(ENABLE);
		//读保护使能
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
	ShareInfo.decrypt(decoded,encoded);//解密
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




