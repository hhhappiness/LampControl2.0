/**************************************************************
RTC模块，两种工作模式， 
1.仅用它来做定时器，使用其second中断来通用定时 
2.用作实时时种，获取年/月/日，需要定义宏RTC_REAL_TIME 
时钟源可以是LSE和HSE/128。暂时没有考虑使用不精确的LSI 
需要定义宏RTCCLK_SOURCE和RTC_FREQ， 
缺省值在stm.h里定义为RCC_RTCCLKSource_LSE、1Hz 
如果使用了LSE，并且频率不是32768Hz，也需要定义宏LSEClk 
 ***************************************************************/
#include "stm32f10x.h"
//#include "platform_config.h"
//#include "app.h"
#include <stdlib.h>
#include "bsp.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rcc.h"
#include "RTC.h"

#define RCC_LSE_RDY 0x0002
#define RCC_RTC_EN  0x8000

//是否使用RTC的宏定义，缺省不定义，需要是在编译参数中定义
//#define RTC_REAL_TIME

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC->APB1ENR |= RCC_APB1Periph_PWR | RCC_APB1Periph_BKP;  

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
	
	/* enable OSC32_IN/OUT as GPIO */
	RCC_LSEConfig(RCC_LSE_OFF);

  /* Reset Backup Domain */
  BKP_DeInit();
  #if (RTCCLK_SOURCE) == (RTCCLKSource_LSE) //通常一般用32.576的外部晶体
  /* Enable LSE */
    if(!(RCC->BDCR& (RCC_LSE_ON | RCC_LSE_RDY))) {
        RCC->BDCR |= RCC_LSE_ON;
        while(!(RCC->BDCR & RCC_LSE_RDY));
    }
    RCC->BDCR |= RCC_RTCCLKSource_LSE|RCC_RTC_EN;/* Select LSE as RTC Clock Source */
  #elif RTCCLK_SOURCE == RTCCLKSource_HSE_Div128 //频闪灯的RTC用HSE/128做时钟源，4M/128=31.25k
    RCC->BDCR |= RCC_RTCCLKSource_HSE_Div128|RCC_RTC_EN;/* Select LSE as RTC Clock Source */
//  #endif
  #elif RTCCLK_SOURCE == RTCCLKSource_LSI //频闪灯的RTC用HSE/128做时钟源，4M/128=31.25k
    RCC->BDCR |= RTCCLKSource_LSI|RCC_RTC_EN;/* Select LSE as RTC Clock Source */
    RCC->CSR  |= 0x01;
  #endif

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  #if RTCCLK_SOURCE == RTCCLKSource_LSE //通常一般用32.576的外部晶体
    RTC_SetPrescaler(LSEClk/RTC_FREQ-1); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
  #elif RTCCLK_SOURCE == RTCCLKSource_HSE_Div128
    RTC_SetPrescaler(HSEClk/(128*RTC_FREQ)-1); //(4M/128)/250=125
   #elif RTCCLK_SOURCE == RTCCLKSource_LSI
    RTC_SetPrescaler(LSIClk/RTC_FREQ-1); //(4M/128)/250=125
  #endif

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


#ifdef RTC_REAL_TIME //如果使用RTC来记时(年月日)

_TimeStruct Time;
u8 TimeDisplay=0;

//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

int InitRTC(void)
{
	int FirstInit=0;	//是否第一次初始化RTC
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		FirstInit= 1;
		/* Backup data register value is not correct or not yet programmed (when
		   the first time the program is executed) */

		/* RTC Configuration */
		RTC_Configuration();

		//printf("\r\n RTC configured....");

		/* Adjust time by values entred by the user on the hyperterminal */
		//Time_Adjust();
		Time.Year=2009;
		Time.Month=1;
		Time.Day=1;
		Time.Hour=0;
		Time.Minute=0;
		Time.Second=0;

		RTC_WaitForLastTask();
		/* Change the current time */
		//RTC_SetCounter(TimeStruct2Time(&Time));
		RTC_SetCounter(0);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	} else {
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		  //printf("\r\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
		  //printf("\r\n\n External Reset occurred....");
		}

		//printf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
		//这句在103VE会卡住
		RTC_WaitForSynchro();
		Time2TimeStrut(RTC_GetCounter(),&Time);
		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

	#ifdef RTCClockOutput_Enable
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
								 functionality must be disabled */

	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif

	/* Clear reset flags */
	RCC_ClearFlag();
	return FirstInit;
}


#if 0
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);
    /* Enable time update */
   TimeDisplay = 1;
  }
	if (RTC_GetITStatus(RTC_IT_OW) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_OW);
  }
}
#endif

/**************************************************************
时间转换函数
时间基数为2001,最多可以表示68年
***************************************************************/
#define TIME_BASE_YEAR 2009 //时间基准必须是4N+1,四年周期里各年分别365,365,365,366天
                            // 
const s16 DayBeforeMonth[]={//平常年每个月之前的天数
0	,31	,59	,90	,120	,151	,181	,212	,243	,273	,304	,334	,
};
const s16 DayBeforeMonth2[]={//闰年每个月之前的天数
0	,31	,60	,91	,121	,152	,182	,213	,244	,274	,305	,335	,
};

int DaysOfThisMonth(_TimeStruct *t){
      switch (t->Month) {
      case 2:
          if(t->Year%4==0) return 29;
          else return 28;
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
          return 31;
      default: return 30;
      }
}
/**************************************************************
时间结构体转换成s32型
***************************************************************/
int TimeStruct2Time(_TimeStruct *T)
{
    int DayBeforeThisYear;
    int DayBeforeThisMonth;
    int Days;
    int IsYunNian;
    int Year;
    int t;
    Year=T->Year-TIME_BASE_YEAR;
    IsYunNian=((Year & 0x03)==0x03);
    DayBeforeThisYear=(Year)*365+ (Year/4);
    DayBeforeThisMonth=(IsYunNian) ? DayBeforeMonth2[T->Month-1] : DayBeforeMonth[T->Month-1];
    
    Days=DayBeforeThisYear+DayBeforeThisMonth+T->Day-1;
    t=(((u32)Days*24 + T->Hour)*60 + T->Minute)*60 + T->Second;//3个32bit*8bit乘法
    return t;
}
/**************************************************************
s32型时间转换成结构体
***************************************************************/
void Time2TimeStrut(int t,_TimeStruct *T)
{
    div_t  DivResult;
    int Days;
    int QYear;
    int i;
    int IsYunNian;

    DivResult=div(t,60);
    T->Second=DivResult.rem;
    DivResult=div(DivResult.quot,60);
    T->Minute=DivResult.rem;
    DivResult=div(DivResult.quot,24);
    T->Hour=DivResult.rem;
    DivResult=div(DivResult.quot,365*4+1);//四年一个周期
    QYear=DivResult.quot;
    Days=DivResult.rem;

    if(Days==365*4){//闰年的最后一天如果按后面的方法会得算到下一年的第一天所以单独计算
        T->Year=QYear*4+3+TIME_BASE_YEAR;
        T->Month=12;
        T->Day=31;
        return;
    }else{
        DivResult=div(Days,365);//计算剩下的天数还有几个整年
        T->Year=QYear*4+DivResult.quot;//总的整年数
        Days=DivResult.rem;//剩下的天数
    
        IsYunNian=((T->Year & 0x03)==0x03);
        if (IsYunNian) {
            for(i=11;i>=0;i--) {
                if(Days>=DayBeforeMonth2[i]) break;//比较查找当前月份
            }
           T->Month=i+1;
           T->Day=Days-DayBeforeMonth2[T->Month-1]+1;
        }
        else{
            for(i=11;i>=0;i--) {
                if(Days>=DayBeforeMonth[i]) break;
            }
           T->Month=i+1;
           T->Day=Days-DayBeforeMonth[T->Month-1]+1;
        }
        T->Year+=TIME_BASE_YEAR;//加上年的基数
    }
}

/**************************************************************
将_TimeStruct结构体转成字符串，buf长度19
***************************************************************/
void Time2Str(_TimeStruct *t,char *buf)
{
    u16 tmp;
    u8 t8;
    tmp=t->Year;
    buf[3]=tmp%10+'0';    tmp/=10;
    buf[2]=tmp%10+'0';    tmp/=10;
    buf[1]=tmp%10+'0';    tmp/=10;
    buf[0]=tmp%10+'0';
    buf[4]='-';
    t8=t->Month;
    buf[6]=t8%10+'0';    t8/=10;
    buf[5]=t8%10+'0';
    buf[7]='-';
    t8=t->Day;
    buf[9]=t8%10+'0';    t8/=10;
    buf[8]=t8%10+'0';

    buf[10]=' ';
    t8=t->Hour;
    buf[12]=t8%10+'0';    t8/=10;
    buf[11]=t8%10+'0';

    buf[13]=':';
    t8=t->Minute;
    buf[15]=t8%10+'0';    t8/=10;
    buf[14]=t8%10+'0';

    buf[16]=':';
    t8=t->Second;
    buf[18]=t8%10+'0';    t8/=10;
    buf[17]=t8%10+'0';
}

#else //不用RTC来记时，只使用其中断功能

int InitRTC(void){
  RTC_Configuration();

  /* Clear reset flags */
  RCC_ClearFlag();
	return 0;

}

#endif


