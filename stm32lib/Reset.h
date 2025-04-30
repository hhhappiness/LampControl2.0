#ifndef _RESET_H_
#define _RESET_H_

#include "type.h"
#include "board_config.h"

///ע�������board_config����BootAddress��AppAddress

#ifdef __cplusplus
 extern "C" {
#endif 
 
void ResetAt(u32 reset_addr);
#define Reset() 	ResetAt(BOOT_ADDRESS)
#define ResetApp() 	ResetAt(APP_ADDRESS)
 
static inline void SoftReset(void)
{
	__set_FAULTMASK(1);  // �ر������ж�
	NVIC_SystemReset();// ��λ
}

 #ifdef __cplusplus
 }
#endif 

#endif//_RESET_H_

