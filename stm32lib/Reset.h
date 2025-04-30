#ifndef _RESET_H_
#define _RESET_H_

#include "type.h"
#include "board_config.h"

///注意必须在board_config定义BootAddress和AppAddress

#ifdef __cplusplus
 extern "C" {
#endif 
 
void ResetAt(u32 reset_addr);
#define Reset() 	ResetAt(BOOT_ADDRESS)
#define ResetApp() 	ResetAt(APP_ADDRESS)
 
static inline void SoftReset(void)
{
	__set_FAULTMASK(1);  // 关闭所有中端
	NVIC_SystemReset();// 复位
}

 #ifdef __cplusplus
 }
#endif 

#endif//_RESET_H_

