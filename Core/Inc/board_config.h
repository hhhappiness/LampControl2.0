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

//����HardFault�жϺ��ĸ�����λ���������Enter��
#define FaultResetCondition() 	(GPI_KEY_ENTER == 0)

// My GPIO function define

//#define GPIOA->ODR(i) (GPIOA->ODR & ~(1<<i)) //GPIOA->ODR(i) = 0
#define GPA_O(i,x) ((x==1) ? (GPIOA->ODR |= (1<<i)):(GPIOA->ODR &= ~(1<<i)))
#define GPB_O(i,x) ((x==1) ? (GPIOB->ODR |= (1<<i)):(GPIOB->ODR &= ~(1<<i)))
#define GPA_I(i) ((GPIOA->IDR & (1<<i)) ? 1 : 0)
#define GPB_I(i) ((GPIOB->IDR & (1<<i)) ? 1 : 0)

//����ܽ�

#define POWER_PRESSED 	(GPB_I(0)==0) //��Դ��������
#define ENTER_PRESSED 	(GPB_I(5)==0) //Enter������
#define BKLT_SW(x)		GPA_O(15,x)
#define LCD_RSTB(x)	GPA_O(8,x) //RSTB=1 or 0	
#define LCD_A0(x) 	GPB_O(14,x) //A0=1 or 0
#define LCD_CSB(x) 	GPB_O(12,x) //CSB=0 or 1

#define _A0_1 		GPB_O(14,1) //LCD_A0=1
#define _A0_0 		GPB_O(14,0) //LCD_A0=0
#define _BLK_1 		BKLT_SW(1)
#define _BLK_0 		BKLT_SW(0)
#define _RSTB_0 	GPA_O(8,0) //LCD_RSTB=0
#define _RSTB_1 	GPA_O(8,1) //LCD_RSTB=1

//����ܽ�	 
#define CHARGE_PIN	GPB_I(12)
//Mode����Enter������ͨ�ð���ʹ���⻹�����⹦�ܣ��ڴ˶���ܽ�	 
#define GPI_KEY_ENTER 	GPB_I(5)		 

__inline void PowerOn(void )  {GPA_O(2,1);}
__inline void PowerOff(void ) {GPA_O(2,0);}
__inline void BackLightOn(void )  {BKLT_SW(1);}
__inline void BackLightOff(void ) {BKLT_SW(0);}
__inline void LcdResetOn(void )  {LCD_RSTB(0);}
__inline void LcdResetOff(void ) {LCD_RSTB(1);}

#define GetChargePin	0

void ShutDown(void);

#ifdef __cplusplus
}
#endif 
	
#endif
