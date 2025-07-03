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

int CheckPowerKey(int ms)
{
	u32 Treg;
	int Cnt = ms/50;
	if(!POWER_PRESSED)//没按就退出
	{
		return 0;
	}	
	Treg=GetTimerCount();
	while(1)
	{
		if(IsTimeOut_ms(Treg,50))//没按就退出
		{
			//wdg();
			Treg=GetTimerCount();
			if(!POWER_PRESSED)
			{
				return 0;
			}
			Cnt --;
			if(Cnt==0) return 1;
		}
	}
}

void dbgmcu_cr(void)
{
	u32 dbg_reg;
	dbg_reg = DEBUGMCU_CR;
	dbg_reg |= ((1<<9)|(1<<2));
	DEBUGMCU_CR = dbg_reg;
}

