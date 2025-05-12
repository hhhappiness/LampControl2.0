#include "stm32.h"
#include "board_config.h"
#include "type.h"
#include "ctrl_common.h"

///App和Bootloader共用的控制代码

///bootloader将版本信息写到字符串BootloaderVersionStr，App可以读取
#ifdef BOOTLOADER
char BootloaderVersionStr[BOOTLOADER_VERSION_SIZE] __attribute__((at(BOOTLOADER_VERSION_ADDR)));
#else
char *BootloaderVersionStr = (char *) BOOTLOADER_VERSION_ADDR;
#endif

//mcu调试寄存器
#define DBGMCU_CR    (*((volatile unsigned long *)0xE0042004)) 

#if 0
///上电缓起动流程
void PowerSeq(void)
{
	int i;
	BoostOff();
	for (i=1;i<11;i++)
	{
		PowerOff();
		Delay_ms(50);
		PowerOn();
		Delay_us(20*i);
		wdg();
	}
}


///判断关机是否超过一定时间，以分钟为单位，暂时一直判定为TRUE
BOOL TestLongShutDown(int minutes)
{
	u32 runtime = RTC_GetCounter();
	runtime = (runtime + RTC_FREQ/2)/RTC_FREQ;
	return (runtime > minutes*60);
}


void WatchdogInit(void)
{
#if 0
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256); //40K/256 [ 6.4ms ]
	IWDG_SetReload(200); //6.4ms * 200 = 1.28S 
	IWDG_Enable();
#endif

	//Boot APB1 = 8M
	//App APB1 =4.5M
	//按照8M来计算时间
	//8M/4096/8 = 4.098ms ~= 4.1ms
	//4.1ms * (0x7F - 0x40) = 63*4.1ms = 250ms

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
	WWDG_DeInit();
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(0x7F);
	WWDG_Enable(0x7F);
}


void wdg(void)
{
#if 0
	IWDG_ReloadCounter();
#endif
	
	WWDG_SetCounter(0x7F);
}
#endif
// bit2: debug standby mode
// bit9: debug iwdg 
void dbgmcu_cr(void)
{
	u32 dbg_reg;
	dbg_reg = DEBUGMCU_CR;
	dbg_reg |= ((1<<9)|(1<<2));
	DEBUGMCU_CR = dbg_reg;
}

