#ifndef __CTRL_COMMON_H_
#define __CTRL_COMMON_H_
#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "Type.h"
#include "stm32g4xx_hal.h"



///长关机的时间门限，以分钟为单位。缺省2小时，测试时可设成1分钟
#define LONG_SHUT_DOWN_TIME 	(2*60)	 
int CheckPowerKey(int ms);

extern WWDG_HandleTypeDef hwwdg;
#define wdg() HAL_WWDG_Refresh(&hwwdg)

#define BOOTLOADER_VERSION_ADDR 	(0x20005000-16)
#define BOOTLOADER_VERSION_SIZE		16
#ifdef BOOTLOADER
extern char BootloaderVersionStr[BOOTLOADER_VERSION_SIZE];
#else
extern char *BootloaderVersionStr;
#endif

#ifdef __cplusplus
}
#endif 
#endif//__CTRL_COMMON_H_

