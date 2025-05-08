#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32.h"
#include "key_base.h"
#include "core_config.h"

#define APP_ADDRESS  	0x8004000
#define BOOT_ADDRESS	0x8000000

//����HardFault�жϺ��ĸ�����λ���������Enter��
#define FaultResetCondition() 	(GPI_KEY_ENTER == 0)

//����ܽ�
#define VSET1 		GPB_O(9) 
#define VSET2 		GPB_O(8) 
#define POWER_SW 	GPB_O(7)
#define BKLT_SW		GPB_O(11)
#define LCD_RSTB(x)	LCD_RSTB_Pin_O(x) //RSTB=1 or 0	
#define LCD_A0(x) 	LCD_A0_Pin_O(x) //A0=1 or 0
#define LCD_CSB(x) 	LCD_CSB_Pin_O(x) //CSB=0 or 1

//����ܽ�	 
#define CHARGE_PIN	GPB_I(12)
#define GPI_KEY_POWER GPA_I(0)
//Mode����Enter������ͨ�ð���ʹ���⻹�����⹦�ܣ��ڴ˶���ܽ�	 
#define GPI_KEY_MODE 	GPA_I(15)
#define GPI_KEY_ENTER 	GPC_I(13)		 

#ifdef PLED_V20
	__inline void BoostOn(void )  {VSET1 = 1;}
	__inline void BoostOff(void ) {VSET1 = 0;}
#else
	__inline void BoostOn(void )  {VSET1 = 0;}
	__inline void BoostOff(void ) {VSET1 = 1;}
#endif
__inline void PowerOn(void )  {POWER_SW = 1;}
__inline void PowerOff(void ) {POWER_SW = 0;}
__inline void BackLightOn(void )  {BKLT_SW = 1;}
__inline void BackLightOff(void ) {BKLT_SW = 0;}
__inline void LcdResetOn(void )  {LCD_RSTB(0)       ;}
__inline void LcdResetOff(void ) {LCD_RSTB(1);}

#define GetChargePin	0

void EnterStandby(void);

#ifdef __cplusplus
}
#endif 
	
#endif
