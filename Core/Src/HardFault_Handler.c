#include "stm32g4xx_it.h"

void HardFault_Handler()
{
 /* Go to infinite loop when Hard Fault exception occurs */
 while(!AlownReturn);
 
 __asm("BX lr\n");
}