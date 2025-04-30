#include "Reset.h"

///软件复位到指定地址
void ResetAt(u32 reset_addr)
{
	typedef  void (*pFunction)(void);
	pFunction Jump_To_Application;

	Jump_To_Application = (pFunction) (*(__IO uint32_t*) (reset_addr + 4));
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) reset_addr);
	Jump_To_Application();	
}
