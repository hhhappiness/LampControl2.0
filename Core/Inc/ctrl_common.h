#ifndef __CTRL_COMMON_H_
#define __CTRL_COMMON_H_
#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "Type.h"

///���ػ���ʱ�����ޣ��Է���Ϊ��λ��ȱʡ2Сʱ������ʱ�����1����
#define LONG_SHUT_DOWN_TIME 	(2*60)	 
BOOL TestLongShutDown(int minutes);
void PowerSeq(void);
void WatchdogInit(void);
void wdg(void);
void dbgmcu_cr(void);

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

