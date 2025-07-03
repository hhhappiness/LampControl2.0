#include "stm32.h"
#include "board_config.h"
#include "type.h"
#include "ctrl_common.h"

///App��Bootloader���õĿ��ƴ���

///bootloader���汾��Ϣд���ַ���BootloaderVersionStr��App���Զ�ȡ
#ifdef BOOTLOADER
char BootloaderVersionStr[BOOTLOADER_VERSION_SIZE] __attribute__((at(BOOTLOADER_VERSION_ADDR)));
#else
char *BootloaderVersionStr = (char *) BOOTLOADER_VERSION_ADDR;
#endif

//mcu���ԼĴ���
#define DBGMCU_CR    (*((volatile unsigned long *)0xE0042004)) 

int CheckPowerKey(int ms)
{
	u32 Treg;
	int Cnt = ms/50;
	if(!POWER_PRESSED)//û�����˳�
	{
		return 0;
	}	
	Treg=GetTimerCount();
	while(1)
	{
		if(IsTimeOut_ms(Treg,50))//û�����˳�
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

