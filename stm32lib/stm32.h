#ifndef _STM_32_H_
#define _STM_32_H_
#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32g4xx.h"
#include "core_config.h"
#include "stdint.h"
#include "type.h"


#define __NOP2() {__NOP;__NOP;}
#define __NOP3() {__NOP;__NOP;__NOP;}
#define __NOP4() {__NOP;__NOP;__NOP;__NOP;}
#define __NOP5() {__NOP;__NOP;__NOP;__NOP;__NOP;}

#ifndef NULL
    #define NULL 0
#endif

#if AHBClk == APB1Clk
    #define TIMXCLK (APB1Clk*1) //to TIM2,3,4,5,6,7,12,13,14
#else
    #define TIMXCLK (APB1Clk*2) //to TIM2,3,4,5,6,7,12,13,14
#endif
//用systick做通用延时器
#define SYSTICK_MAXCOUNT 	SysTick_LOAD_RELOAD_Msk
__INLINE void InitSysTick(){
    SysTick->LOAD  =  SYSTICK_MAXCOUNT;                    /* set reload register */
    SysTick->VAL   =  (0x00);                              /* Load the SysTick Counter Value */
    #if AHBClk == 72000000 || AHBClk == 70000000 || AHBClk == 168000000
    //主频较高时用ABH clock/8
    SysTick->CTRL = (0 << SysTick_CTRL_CLKSOURCE_Pos) //0: AHB clock/8, 1: AHB clock
                | (0 << SysTick_CTRL_TICKINT_Pos)   /* disable SysTick interrupt 原先在systick中断实现的转移到别处*/
                | (1<<SysTick_CTRL_ENABLE_Pos);   /* Enable SysTick IRQ and SysTick Timer */
    #else
        #error please define the ABH clock
    #endif
}

//不同总线时钟下的延时换算系数

#define SYSTICK_us      (70+4)/8       //systick与us的换算系数
#define SYSTICK_ms      8750    //systick与ms的换算系数
#define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计3728ms


#if AHBClk == APB1Clk
    #define TIMXCLK (APB1Clk*1) //to TIM2,3,4,5,6,7,12,13,14
#else
    #define TIMXCLK (APB1Clk*2) //to TIM2,3,4,5,6,7,12,13,14
#endif
 
#if AHBClk == APB2Clk
    #define TIMxCLK (APB2Clk*1) //to TIM1,8,9,10,11
#else
    #define TIMxCLK (APB2Clk*2) //to TIM1,8,9,10,11
#endif 



#define SYSTICK_COUNTFLAG  0x10000 //倒计到0时变1, 对SysTick->CTRL写任何值清0
//延时计数器, systick自由跑, 每个延时计数器需要一个变量记录开始值, 与当前值比较是否超时
#define GetTimerCount()     (SysTick->VAL)
#define _IsTimeOut(TReg,Count)     (((TReg+SYSTICK_MAXCOUNT+1-GetTimerCount())& SYSTICK_MAXCOUNT)>(Count))
#define IsTimeOut_ms(TReg,ms)     (_IsTimeOut(TReg,ms*SYSTICK_ms))
#define IsTimeOut_us(TReg,us)     (_IsTimeOut(TReg,us*SYSTICK_us))
#define Delay_ms(ms)     {u32 TReg=GetTimerCount();    while(!IsTimeOut_ms(TReg,ms));}
#define Delay_us(us)     {u32 TReg=GetTimerCount();    while(!IsTimeOut_us(TReg,us));}
#define delay_ms(ms) Delay_ms(ms);
#define delay_us(us) Delay_us(us);
#define Delay_100ms(i) Delay_ms(i*100) 
#define ResetTimeOut(TReg) 	{TReg=GetTimerCount();}


//Bit Banding
#define BB_Periph_addr(addr,bit) (PERIPH_BB_BASE + ((addr)-PERIPH_BASE)*32 + (bit)*4)
#define BB_Periph(addr,bit) *(volatile uint32_t*)(BB_Periph_addr(addr,bit))
#define BB_SRAM(addr,bit) *((volatile uint32_t*)(SRAM_BB_BASE + ((addr-SRAM_BASE)*32) + (bit*4)))
#define BitBanding_NVIC


#define EnableINT(x)   NVIC->ISER[(x) >> 0x05] = (uint32_t)0x01 << (x & (uint8_t)0x1F)
#define DisableINT(x)  NVIC->ICER[(x) >> 0x05] = (uint32_t)0x01 << (x & (uint8_t)0x1F)

#define GPIO_IDR 0x10 //GPIO寄存器IDR的偏移地址
#define GPIO_ODR 0x14 //GPIO寄存器ODR的偏移地址

//GPIO位地址定义
#define GPA_I_Addr(i) 	BB_Periph_addr(GPIOA_BASE+GPIO_IDR,i)
#define GPA_O_Addr(i) 	BB_Periph_addr(GPIOA_BASE+GPIO_ODR,i)
#define GPB_I_Addr(i) 	BB_Periph_addr(GPIOB_BASE+GPIO_IDR,i)
#define GPB_O_Addr(i) 	BB_Periph_addr(GPIOB_BASE+GPIO_ODR,i)
#define GPC_I_Addr(i) 	BB_Periph_addr(GPIOC_BASE+GPIO_IDR,i)
#define GPC_O_Addr(i) 	BB_Periph_addr(GPIOC_BASE+GPIO_ODR,i)
#define GPD_I_Addr(i) 	BB_Periph_addr(GPIOD_BASE+GPIO_IDR,i)
#define GPD_O_Addr(i) 	BB_Periph_addr(GPIOD_BASE+GPIO_ODR,i)
#define GPE_I_Addr(i) 	BB_Periph_addr(GPIOE_BASE+GPIO_IDR,i)
#define GPE_O_Addr(i) 	BB_Periph_addr(GPIOE_BASE+GPIO_ODR,i)

//GPIO位直接访问
// #define GPA_I(i)		(*(volatile uint32_t*)GPA_I_Addr(i))
// #define GPA_O(i)		(*(volatile uint32_t*)GPA_O_Addr(i))
// #define GPB_I(i)		(*(volatile uint32_t*)GPB_I_Addr(i))
// #define GPB_O(i)		(*(volatile uint32_t*)GPB_O_Addr(i))
#define GPC_I(i)		(*(volatile uint32_t*)GPC_I_Addr(i))
#define GPC_O(i)		(*(volatile uint32_t*)GPC_O_Addr(i))
#define GPD_I(i)		(*(volatile uint32_t*)GPD_I_Addr(i))
#define GPD_O(i)		(*(volatile uint32_t*)GPD_O_Addr(i))
#define GPE_I(i)		(*(volatile uint32_t*)GPE_I_Addr(i))
#define GPE_O(i)		(*(volatile uint32_t*)GPE_O_Addr(i))


//IIC CR1
#define I2C1_CR1_bit(i) BB_Periph(I2C1_BASE+0x00,i)
#define I2C1_PE         I2C1_CR1_bit(0)//Peripheral enable
#define I2C1_SMBUS      I2C1_CR1_bit(1)//0: I2C mode 1: SMBus mode
#define I2C1_SMBTYPE    I2C1_CR1_bit(3)//0: SMBus Device 1: SMBus Host
#define I2C1_ENARP      I2C1_CR1_bit(4)//ARP enable
#define I2C1_ENPEC      I2C1_CR1_bit(5)//PEC calculation enable
#define I2C1_ENGC       I2C1_CR1_bit(6)//0: General call disabled. Address 00h is NACKed.
#define I2C1_NOSTRETCH  I2C1_CR1_bit(7)//PEC calculation enable
#define I2C1_START      I2C1_CR1_bit(8)//Start generation
#define I2C1_STOP       I2C1_CR1_bit(9)//Stop generation
#define I2C1_ACK        I2C1_CR1_bit(10)//Acknowledge enable
#define I2C1_POS        I2C1_CR1_bit(11)//Acknowledge/PEC Position (for data reception)
#define I2C1_PEC        I2C1_CR1_bit(12)//Packet error checking
#define I2C1_ALERT      I2C1_CR1_bit(13)//SMBus alert
#define I2C1_SWRST      I2C1_CR1_bit(15)//Software reset

#define I2C2_CR1_bit(i) BB_Periph(I2C2_BASE+0x00,i)
#define I2C2_PE         I2C2_CR1_bit(0)//Peripheral enable
#define I2C2_SMBUS      I2C2_CR1_bit(1)//0: I2C mode 1: SMBus mode
#define I2C2_SMBTYPE    I2C2_CR1_bit(3)//0: SMBus Device 1: SMBus Host
#define I2C2_ENARP      I2C2_CR1_bit(4)//ARP enable
#define I2C2_ENPEC      I2C2_CR1_bit(5)//PEC calculation enable
#define I2C2_ENGC       I2C2_CR1_bit(6)//0: General call disabled. Address 00h is NACKed.
#define I2C2_NOSTRETCH  I2C2_CR1_bit(7)//PEC calculation enable
#define I2C2_START      I2C2_CR1_bit(8)//Start generation
#define I2C2_STOP       I2C2_CR1_bit(9)//Stop generation
#define I2C2_ACK        I2C2_CR1_bit(10)//Acknowledge enable
#define I2C2_POS        I2C2_CR1_bit(11)//Acknowledge/PEC Position (for data reception)
#define I2C2_PEC        I2C2_CR1_bit(12)//Packet error checking
#define I2C2_ALERT      I2C2_CR1_bit(13)//SMBus alert
#define I2C2_SWRST      I2C2_CR1_bit(15)//Software reset
//IIC CR2

//IIC SR1
//
//IIC SR2

#define DEBUGMCU_CR  *((volatile uint32_t*)0xE0042004)
#define AFIO_MAPR    *((volatile uint32_t*)0x40010004)

//TIM input filter
//用于CCMR的ICxF位域和SMCR的ETF位域
#define TIM_INPUT_FILTER_fDTS_1         0   //0000: No filter, sampling is done at fDTS.
#define TIM_INPUT_FILTER_fCK_INT_2      1   //0001: fSAMPLING=fCK_INT , N=2.
#define TIM_INPUT_FILTER_fCK_INT_4      2   //0010: fSAMPLING=fCK_INT , N=4.
#define TIM_INPUT_FILTER_fCK_INT_8      3   //0011: fSAMPLING=fCK_INT , N=8.
#define TIM_INPUT_FILTER_fDTS2_6        4   //0100: fSAMPLING=fDTS/2, N=6.
#define TIM_INPUT_FILTER_fDTS2_8        5   //0101: fSAMPLING=fDTS/2, N=8.
#define TIM_INPUT_FILTER_fDTS4_6        6   //0110: fSAMPLING=fDTS/4, N=6.
#define TIM_INPUT_FILTER_fDTS4_8        7   //0111: fSAMPLING=fDTS/4, N=8.
#define TIM_INPUT_FILTER_fDTS8_6        8   //1000: fSAMPLING=fDTS/8, N=6.
#define TIM_INPUT_FILTER_fDTS8_8        9   //1001: fSAMPLING=fDTS/8, N=8.
#define TIM_INPUT_FILTER_fDTS16_5      10   //1010: fSAMPLING=fDTS/16, N=5.
#define TIM_INPUT_FILTER_fDTS16_6      11   //1011: fSAMPLING=fDTS/16, N=6.
#define TIM_INPUT_FILTER_fDTS16_8      12   //1100: fSAMPLING=fDTS/16, N=8.
#define TIM_INPUT_FILTER_fDTS32_5      13   //1101: fSAMPLING=fDTS/32, N=5.
#define TIM_INPUT_FILTER_fDTS32_6      14   //1110: fSAMPLING=fDTS/32, N=6.
#define TIM_INPUT_FILTER_fDTS32_8      15   //1111: fSAMPLING=fDTS/32, N=8.

#define TIM_ONE_PULSE_MODE  (1<<3)

//以下在stm32f20x.tim.c里定义过，复制过来的
#define CR1_ARPE_Set                ((uint16_t)0x0080) 
#define CR1_CEN_Set                 ((uint16_t)0x0001)

//Bits 6:4 MMS: Master mode selection
//These bits allow to select the information to be sent in master mode to slave timers for
//synchronization (TRGO). The combination is as follows:
//000: Reset - the UG bit from the TIMx_EGR register is used as trigger output (TRGO). If the
//reset is generated by the trigger input (slave mode controller configured in reset mode) then
//the signal on TRGO is delayed compared to the actual reset.
#define MMS_RESET  (0<<4)
//001: Enable - the Counter enable signal, CNT_EN, is used as trigger output (TRGO). It is
//useful to start several timers at the same time or to control a window in which a slave timer is
//enabled. The Counter Enable signal is generated by a logic OR between CEN control bit and
//the trigger input when configured in gated mode.
//When the Counter Enable signal is controlled by the trigger input, there is a delay on TRGO,
//except if the master/slave mode is selected (see the MSM bit description in TIMx_SMCR
//register).
#define MMS_ENABLE (1<<4) 
//010: Update - The update event is selected as trigger output (TRGO). For instance a master
//timer can then be used as a prescaler for a slave timer.
#define MMS_UPDATE (2<<4)
//011: Compare Pulse - The trigger output send a positive pulse when the CC1IF flag is to be
//set (even if it was already high), as soon as a capture or a compare match occurred.
#define MMS_CC1IF  (3<<4)
//100: Compare - OC1REF signal is used as trigger output (TRGO).
#define MMS_OC1REF (4<<4) 
//101: Compare - OC2REF signal is used as trigger output (TRGO).
#define MMS_OC2REF (5<<4)
//110: Compare - OC3REF signal is used as trigger output (TRGO).
#define MMS_OC3REF (6<<4)
//111: Compare - OC4REF signal is used as trigger output (TRGO).
#define MMS_OC4REF (7<<4)

#define CCx_DMA_request_when_CCx_event      0 
#define CCx_DMA_requests_when_update_event  1 


#define CCxS_OUTPUT         (0)  //CC1 channel is configured as output.
#define CCxS_INPUT_TIx      (1)  //CC1 channel is configured as input, IC1 is mapped on TI1
#define CCxS_INPUT_TIx_SWAP (2)  //CC1 channel is configured as input, IC1 is mapped on TI2
#define CCxS_INPUT_TRC      (3)  //CC1 channel is configured as input, IC1 is mapped on TRC

#define CC1S_OUTPUT     (0<<0)  //CC1 channel is configured as output.
#define CC1S_INPUT_TI1  (1<<0)  //CC1 channel is configured as input, IC1 is mapped on TI1
#define CC1S_INPUT_TI2  (2<<0)  //CC1 channel is configured as input, IC1 is mapped on TI2
#define CC1S_INPUT_TRC  (3<<0)  //CC1 channel is configured as input, IC1 is mapped on TRC

#define CC2S_OUTPUT     (0<<8)  //CC2 channel is configured as output.
#define CC2S_INPUT_TI2  (1<<8)  //CC2 channel is configured as input, IC1 is mapped on TI2
#define CC2S_INPUT_TI1  (2<<8)  //CC2 channel is configured as input, IC1 is mapped on TI1
#define CC2S_INPUT_TRC  (3<<8)  //CC2 channel is configured as input, IC1 is mapped on TRC


#define CC3S_OUTPUT     (0<<0)  //CC3 channel is configured as output.
#define CC3S_INPUT_TI3  (1<<0)  //CC3 channel is configured as input, IC1 is mapped on TI3
#define CC3S_INPUT_TI4  (2<<0)  //CC3 channel is configured as input, IC1 is mapped on TI4
#define CC3S_INPUT_TRC  (3<<0)  //CC3 channel is configured as input, IC1 is mapped on TRC

#define CC4S_OUTPUT     (0<<8)  //CC4 channel is configured as output.
#define CC4S_INPUT_TI4  (1<<8)  //CC4 channel is configured as input, IC1 is mapped on TI4
#define CC4S_INPUT_TI3  (2<<8)  //CC4 channel is configured as input, IC1 is mapped on TI3
#define CC4S_INPUT_TRC  (3<<8)  //CC4 channel is configured as input, IC1 is mapped on TRC

#ifndef MIN 
	#define MIN(a, b)        (((a) < (b)) ? (a) : (b))
#endif	 

#ifndef MAX
	#define MAX(a, b)        (((a) > (b)) ? (a) : (b))
#endif		 
	 
	 

#ifdef __cplusplus
 }
#endif 
#endif//_STM_32_H_
