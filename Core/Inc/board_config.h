#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32.h"
#include "main.h"
#include "type.h"
#include "key_base.h"
#include "core_config.h"

#define APP_ADDRESS  	0x8004000
#define BOOT_ADDRESS	0x8000000

//进入HardFault中断后按哪个键复位，这里设成Enter键
#define FaultResetCondition() 	(GPI_KEY_ENTER == 0)

// My GPIO function define

//#define GPIOA->ODR(i) (GPIOA->ODR & ~(1<<i)) //GPIOA->ODR(i) = 0
#define GPA_O(i,x) ((x==1) ? (GPIOA->ODR |= (1<<i)):(GPIOA->ODR &= ~(1<<i)))
#define GPB_O(i,x) ((x==1) ? (GPIOB->ODR |= (1<<i)):(GPIOB->ODR &= ~(1<<i)))


//输出管脚

#define POWER_SW(x) 	GPB_O(0,x)
#define BKLT_SW(x)		GPA_O(15,x)
#define LCD_RSTB(x)	GPA_O(8,x) //RSTB=1 or 0	
#define LCD_A0(x) 	GPB_O(14,x) //A0=1 or 0
#define LCD_CSB(x) 	GPB_O(12,x) //CSB=0 or 1

//输入管脚	 
#define CHARGE_PIN	GPB_I(12)
#define GPI_KEY_POWER GPA_I(0)
//Mode键和Enter键除做通用按键使用外还有特殊功能，在此定义管脚	 
#define GPI_KEY_MODE 	GPA_I(15)
#define GPI_KEY_ENTER 	GPC_I(13)		 

__inline void PowerOn(void )  {POWER_SW(1);}
__inline void PowerOff(void ) {POWER_SW(0);}
__inline void BackLightOn(void )  {BKLT_SW(1);}
__inline void BackLightOff(void ) {BKLT_SW(0);}
__inline void LcdResetOn(void )  {LCD_RSTB(0);}
__inline void LcdResetOff(void ) {LCD_RSTB(1);}

#define GetChargePin	0

void EnterStandby(void);

#ifdef __cplusplus
}
#endif 
	
#endif
