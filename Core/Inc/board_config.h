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
#define GPA_I(i) ((GPIOA->IDR & (1<<i)) ? 1 : 0)
#define GPB_I(i) ((GPIOB->IDR & (1<<i)) ? 1 : 0)

// GPIO模式设置宏定义
// 模式定义: 0-输入模式, 1-输出模式, 2-复用功能模式, 3-模拟模式
#define GPA_MODE(i,m) ((GPIOA->MODER) = ((GPIOA->MODER & ~(0x03 << (2*i))) | ((m & 0x03) << (2*i))))
#define GPB_MODE(i,m) ((GPIOB->MODER) = ((GPIOB->MODER & ~(0x03 << (2*i))) | ((m & 0x03) << (2*i))))

// GPIO输出类型设置宏定义
// 输出类型: 0-推挽输出, 1-开漏输出
#define GPA_OTYPE(i,t) ((t==1) ? (GPIOA->OTYPER |= (1<<i)):(GPIOA->OTYPER &= ~(1<<i)))
#define GPB_OTYPE(i,t) ((t==1) ? (GPIOB->OTYPER |= (1<<i)):(GPIOB->OTYPER &= ~(1<<i)))

// GPIO上拉/下拉设置宏定义
// 上拉/下拉: 0-无上拉下拉, 1-上拉, 2-下拉
#define GPA_PUPD(i,p) ((GPIOA->PUPDR) = ((GPIOA->PUPDR & ~(0x03 << (2*i))) | ((p & 0x03) << (2*i))))
#define GPB_PUPD(i,p) ((GPIOB->PUPDR) = ((GPIOB->PUPDR & ~(0x03 << (2*i))) | ((p & 0x03) << (2*i))))

// GPIO模式常量定义
#define GPIO_MODE_INPUT        0x00  // 输入模式
#define GPIO_MODE_OUTPUT       0x01  // 输出模式
#define GPIO_MODE_ALTERNATE    0x02  // 复用功能模式
#define GPIO_MODE_ANALOG       0x03  // 模拟模式

// GPIO输出类型常量定义
#define GPIO_OTYPE_PP          0x00  // 推挽输出
#define GPIO_OTYPE_OD          0x01  // 开漏输出

// GPIO上拉/下拉常量定义
#define GPIO_PUPD_NONE         0x00  // 无上拉下拉
#define GPIO_PUPD_PULLUP       0x01  // 上拉
#define GPIO_PUPD_PULLDOWN     0x02  // 下拉

//输出管脚

#define POWER_PRESSED 	(GPB_I(0)==0) //电源按键按下
#define HALF_POWER_PRESSED 	(GPB_I(11)==0) //电源轻按键按下
#define ENTER_PRESSED 	(GPB_I(5)==0) //Enter键按下
#define BKLT_SW(x)		GPA_O(15,x)
#define LCD_RSTB(x)	GPA_O(8,x) //RSTB=1 or 0	
#define LCD_A0(x) 	GPB_O(14,x) //A0=1 or 0
#define LCD_CSB(x) 	GPB_O(12,x) //CSB=0 or 1
#define SNSR_PWR(x) 	GPA_O(1,x) //SNSR_PWR=1 or 0

#define _A0_1 		GPB_O(14,1) //LCD_A0=1
#define _A0_0 		GPB_O(14,0) //LCD_A0=0
#define _BLK_1 		BKLT_SW(1)
#define _BLK_0 		BKLT_SW(0)
#define _RSTB_0 	GPA_O(8,0) //LCD_RSTB=0
#define _RSTB_1 	GPA_O(8,1) //LCD_RSTB=1

//输入管脚	 
#define CHARGE_PIN	GPB_I(12)
//Mode键和Enter键除做通用按键使用外还有特殊功能，在此定义管脚	 
#define GPI_KEY_ENTER 	GPB_I(5)		 

    __inline void PowerOn(void) { GPA_MODE(2, GPIO_MODE_OUTPUT); GPA_O(2, 1); }
    __inline void PowerOff(void) { GPA_O(2, 0); GPA_MODE(2, GPIO_MODE_INPUT); }
    __inline void BackLightOn(void) { GPA_MODE(15, GPIO_MODE_OUTPUT); BKLT_SW(1); }
    __inline void BackLightOff(void) { BKLT_SW(0); GPA_MODE(15, GPIO_MODE_INPUT); }
    __inline void LcdResetOn(void) { GPA_MODE(8, GPIO_MODE_OUTPUT); LCD_RSTB(0); }
    __inline void LcdResetOff(void) { LCD_RSTB(1); GPA_MODE(8, GPIO_MODE_INPUT); }

#define GetChargePin	0

#ifdef __cplusplus
}
#endif 

#endif
