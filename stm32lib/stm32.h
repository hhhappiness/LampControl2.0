#ifndef _STM_32_H_
#define _STM_32_H_
#include "stm32g4xx.h"
#include "core_config.h"
#include "core_cm4.h"

#ifdef __cplusplus
 extern "C" {
#endif 

#define __NOP2() {__NOP;__NOP;}
#define __NOP3() {__NOP;__NOP;__NOP;}
#define __NOP4() {__NOP;__NOP;__NOP;__NOP;}
#define __NOP5() {__NOP;__NOP;__NOP;__NOP;__NOP;}

#ifndef NULL
    #define NULL 0
#endif

//原来的时钟初始化使用的是system_stm32f10x.c里的SystemInit()和SetSysClock()，比较麻烦
//实际频率一般固定，方案也不会太多。暂时不考虑时钟频率、时钟源发生变化的情况
//主时钟选择，可定义RCC_CFGR_SW_HSI,RCC_CFGR_SW_HSE,RCC_CFGR_SW_PLL
//如果用PLL，则PLL source固定为HSE，暂不考虑HSI/2和HSE/2的情况
//以下定义等同于RCC_CFGR_SW_xxx，只去掉了(uint32_t)类型转换
#define  SYS_CLOCK_SOURCE_HSI                     (0x00000000)        /*!< HSI selected as system clock */
#define  SYS_CLOCK_SOURCE_HSE                     (0x00000001)        /*!< HSE selected as system clock */
#define  SYS_CLOCK_SOURCE_PLL                     (0x00000002)        /*!< PLL selected as system clock */

#ifndef SYS_CLOCK_SOURCE
	#define  SYS_CLOCK_SOURCE		SYS_CLOCK_SOURCE_PLL
#endif

//根据不同的时钟源选择时钟初始化程序
#if SYS_CLOCK_SOURCE == SYS_CLOCK_SOURCE_HSI
	#define ClockInit()  ClockSwith_HSI()
#elif SYS_CLOCK_SOURCE == SYS_CLOCK_SOURCE_HSE
	#define ClockInit()  ClockInit_HSE()
#else
	#define ClockInit()  ClockInit_HSE_PLL() 
#endif

//以下宏定义与stm32f10x_rcc.h里的相应宏的值一样，只去掉了(uint32_t)类型转换，
//原定义在条件编译时会报错，如"#if RTCCLK_SOURCE == RCC_RTCCLKSource_LSE"会报错
//为了不改动库，在这里重新定义一下
#define RTCCLKSource_LSE             (0x00000100) //RCC_RTCCLKSource_LSE
#define RTCCLKSource_LSI             (0x00000200) //RCC_RTCCLKSource_LSI
#define RTCCLKSource_HSE_Div128      (0x00000300) //RCC_RTCCLKSource_HSE_Div128

#ifndef LSEClk  //没使用LSE时缺省为32768，有些地方计算要用
    #define LSEClk  32768 
#endif

#ifndef LSIClk  //没使用LSE时缺省为32768，有些地方计算要用
    #define LSIClk  40000 
#endif

#ifndef RTCCLK_SOURCE //未使用RTC的设成LSE, RTC Second中断的频率1Hz
    #define RTCCLK_SOURCE   RTCCLKSource_LSE //未使用的就设成LSE
    #define RTC_FREQ        1                   //RTC second 中断的频率
#endif

//如果是HSE+PLL，检查倍率是否正确
#if (SYS_CLOCK_SOURCE == SYS_CLOCK_SOURCE_PLL)
	#if SysClk/HSEClk >= 2 && SysClk/HSEClk <= 16
		#define RCC_CFGR_PLLMULLx ((SysClk/HSEClk-2)<<18)
	#else
		#error Invalid SysClk/HSEClk
	#endif
#else //非PLL的设个0
	#define RCC_CFGR_PLLMULLx 	0 
#endif

#if SysClk/AHBClk==1
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV1   /*!< SYSCLK not divided */
#elif SysClk/AHBClk==2
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV2   /*!< SYSCLK divided by 2 */
#elif SysClk/AHBClk==4
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV4   /*!< SYSCLK divided by 4 */
#elif SysClk/AHBClk==8
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV8   /*!< SYSCLK divided by 8 */
#elif SysClk/AHBClk==16
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV16  /*!< SYSCLK divided by 16 */
#elif SysClk/AHBClk==64
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV64  /*!< SYSCLK divided by 64 */
#elif SysClk/AHBClk==128
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV128 /*!< SYSCLK divided by 128 */
#elif SysClk/AHBClk==256
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV256 /*!< SYSCLK divided by 256 */
#elif SysClk/AHBClk==512
    #define  RCC_CFGR_HPRE_DIV  RCC_CFGR_HPRE_DIV512 /*!< SYSCLK divided by 512 */
#else
    #error      Invalid SysClk/APBClk rate
#endif

#if AHBClk/APB1Clk==1
    #define     RCC_CFGR_PPRE1_DIV  RCC_CFGR_PPRE1_DIV1  /*!< HCLK not divided */
#elif AHBClk/APB1Clk==2
    #define     RCC_CFGR_PPRE1_DIV  RCC_CFGR_PPRE1_DIV2  /*!< HCLK divided by 2 */
#elif AHBClk/APB1Clk==4
    #define     RCC_CFGR_PPRE1_DIV  RCC_CFGR_PPRE1_DIV4  /*!< HCLK divided by 4 */
#elif AHBClk/APB1Clk==8
    #define     RCC_CFGR_PPRE1_DIV  RCC_CFGR_PPRE1_DIV8  /*!< HCLK divided by 8 */
#elif AHBClk/APB1Clk==16
    #define     RCC_CFGR_PPRE1_DIV  RCC_CFGR_PPRE1_DIV16 /*!< HCLK divided by 16 */
#else
    #error      Invalid SysClk/APB1Clk rate
#endif


#if AHBClk/APB2Clk==1
    #define     RCC_CFGR_PPRE2_DIV  RCC_CFGR_PPRE1_DIV1  /*!< HCLK not divided */
#elif AHBClk/APB2Clk==2
    #define     RCC_CFGR_PPRE2_DIV  RCC_CFGR_PPRE1_DIV2  /*!< HCLK divided by 2 */
#elif AHBClk/APB2Clk==4
    #define     RCC_CFGR_PPRE2_DIV  RCC_CFGR_PPRE1_DIV4  /*!< HCLK divided by 4 */
#elif AHBClk/APB2Clk==8
    #define     RCC_CFGR_PPRE2_DIV  RCC_CFGR_PPRE1_DIV8  /*!< HCLK divided by 8 */
#elif AHBClk/APB2Clk==16
    #define     RCC_CFGR_PPRE2_DIV  RCC_CFGR_PPRE1_DIV16 /*!< HCLK divided by 16 */
#else
    #error      Invalid SysClk/APB2Clk rate
#endif

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
 
void ClockSwith_HSI(void);
void ClockInit_HSE_PLL (void);
void ClockInit_HSE (void);
void ClockInit_HSI (void);
void ClockInit_HSEBypass_PLL (void);
int ClockSwitch_HSI272M(void);
void ClockSwitch_72M2HSI(void);

/////////////////////////////////////////////////////////////
///

//GPIO类型码. ST的库弄的太复杂了
#define GPO_10M     0x1u
#define GPO_02M     0x2u
#define GPO_50M     0x3u
#define GPO_OD_10M  0x5u
#define GPO_OD_02M  0x6u
#define GPO_OD_50M  0x7u
#define AFO_10M     0x9u
#define AFO_02M     0xAu
#define AFO_50M     0xBu
#define AFO_OD_10M  0xDu
#define AFO_OD_02M  0xEu
#define AFO_OD_50M  0xFu
#define GPI_AI      0x0u
#define GPI_IF      0x4u
#define GPI_IP      0x8u //包括PULL-down和pull-up, 要设ODR

//用于设置GPIO_CR1/GPIO_CR2的宏, 高位在前，低位在后
#define GPIO_CR(IO7,IO6,IO5,IO4,IO3,IO2,IO1,IO0) \
    ((IO7)<<28 | (IO6)<<24| (IO5)<<20| (IO4)<<16| (IO3)<<12| (IO2)<<8| (IO1)<<4 | (IO0))

//用systick做通用延时器
#define SYSTICK_MAXCOUNT 	SysTick_LOAD_RELOAD_Msk
__INLINE void InitSysTick(){
    SysTick->LOAD  =  SYSTICK_MAXCOUNT;                    /* set reload register */
    SysTick->VAL   =  (0x00);                              /* Load the SysTick Counter Value */
    #if AHBClk == 72000000 || AHBClk == 70000000
    //主频较高时用ABH clock/8
    SysTick->CTRL = (0 << SysTick_CTRL_CLKSOURCE_Pos) //0: AHB clock/8, 1: AHB clock
                | (0<<SysTick_CTRL_TICKINT_Pos)  
                | (1<<SysTick_CTRL_ENABLE_Pos);   /* Enable SysTick IRQ and SysTick Timer */
    #elif AHBClk == 36000000 || AHBClk == 32000000
    SysTick->CTRL = (0 << SysTick_CTRL_CLKSOURCE_Pos) //0: AHB clock/8, 1: AHB clock
                | (0<<SysTick_CTRL_TICKINT_Pos)  
                | (1<<SysTick_CTRL_ENABLE_Pos);   /* Enable SysTick IRQ and SysTick Timer */
    #elif AHBClk == 8000000 || AHBClk ==16000000
    //主频较低时用ABH clock/8
    SysTick->CTRL = (1 << SysTick_CTRL_CLKSOURCE_Pos) //0: AHB clock/8, 1: AHB clock
                | (0<<SysTick_CTRL_TICKINT_Pos)  
                | (1<<SysTick_CTRL_ENABLE_Pos);   /* Enable SysTick IRQ and SysTick Timer */
    #else
        #error please define the ABH clock
    #endif
}

//不同总线时钟下的延时换算系数
#if AHBClk == 72000000
    #define SYSTICK2us      9       //systick与us的换算系数
    #define SYSTICK2ms      9000    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计1864ms
#elif AHBClk == 36000000
    #define SYSTICK2us      9/2       //systick与us的换算系数，这个值不准
    #define SYSTICK2ms      4500    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计3728ms
#elif AHBClk == 32000000
    #define SYSTICK2us      4       //systick与us的换算系数，这个值不准
    #define SYSTICK2ms      4000    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计4196ms
#elif AHBClk == 8000000
    #define SYSTICK2us      8       //systick与us的换算系数
    #define SYSTICK2ms      8000    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计2097ms
#elif AHBClk ==16000000
    #define SYSTICK2us      16       //systick与us的换算系数
    #define SYSTICK2ms      16000    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计1048ms
#elif AHBClk == 70000000
    #define SYSTICK2us      9       //systick与us的换算系数
    #define SYSTICK2ms      8750    //systick与ms的换算系数
    #define MAX_DELAY_MS    (0x1000000/SYSTICK2ms) //最大能计1864ms
#else
    #error please define the ABH clock
#endif


#define SYSTICK_COUNTFLAG  0x10000 //倒计到0时变1, 对SysTick->CTRL写任何值清0
//延时计数器, systick自由跑, 每个延时计数器需要一个变量记录开始值, 与当前值比较是否超时
#define GetTimerCount()     (SysTick->VAL)
#define _IsTimeOut(TReg,Count)     (((TReg+SYSTICK_MAXCOUNT+1-GetTimerCount())& SYSTICK_MAXCOUNT)>(Count))
#define IsTimeOut_ms(TReg,ms)     (_IsTimeOut(TReg,ms*SYSTICK2ms))
#define IsTimeOut_us(TReg,us)     (_IsTimeOut(TReg,us*SYSTICK2us))
#define Delay_ms(ms)     {u32 TReg=GetTimerCount();    while(!IsTimeOut_ms(TReg,ms));}
#define Delay_us(us)     {u32 TReg=GetTimerCount();    while(!IsTimeOut_us(TReg,us));}
#define ResetTimeOut(TReg) 	{TReg=GetTimerCount();}

void Delay_100ms(int i);

//Bit Banding

#define BB_Periph_addr(addr,bit) (PERIPH_BB_BASE + ((addr)-PERIPH_BASE)*32 + (bit)*4)
#define BB_Periph(addr,bit) *(volatile uint32_t*)(BB_Periph_addr(addr,bit))
#define BB_SRAM(addr,bit) *((volatile uint32_t*)(SRAM_BB_BASE + ((addr-SRAM_BASE)*32) + (bit*4)))
#define BitBanding_NVIC
int GetKey(void);
void ClearKey(void);


#define EnableINT(x)   NVIC->ISER[(x) >> 0x05] = (uint32_t)0x01 << (x & (uint8_t)0x1F)
#define DisableINT(x)  NVIC->ICER[(x) >> 0x05] = (uint32_t)0x01 << (x & (uint8_t)0x1F)
void InitINT(uint8_t NVIC_IRQChannel, uint8_t Priority, uint8_t SubPriority);
void InitINTDisabled(uint8_t NVIC_IRQChannel, uint8_t Priority, uint8_t SubPriority);

#define GPIO_IDR 0x08 //GPIO寄存器IDR的偏移地址
#define GPIO_ODR 0x08 //GPIO寄存器IDR的偏移地址

//USART1状态位
#define USART1_CTS    BB_Periph(USART1_BASE+0,9) //This bit is set by hardware when the nCTS input toggles,
#define USART1_LBD    BB_Periph(USART1_BASE+0,8) //LIN break detection flag
#define USART1_TXE    BB_Periph(USART1_BASE+0,7) //Transmit data register empty
#define USART1_TC     BB_Periph(USART1_BASE+0,6) //Transmission complete
#define USART1_RXNE   BB_Periph(USART1_BASE+0,5) //Read data register not empty
#define USART1_IDLE   BB_Periph(USART1_BASE+0,4) //IDLE line detected
#define USART1_ORE    BB_Periph(USART1_BASE+0,3) //Overrun error
#define USART1_NE     BB_Periph(USART1_BASE+0,3) //Noise error flag
#define USART1_FE     BB_Periph(USART1_BASE+0,2) //Framing error
#define USART1_PE     BB_Periph(USART1_BASE+0,1) //Parity error


//USART2状态位
#define USART2_CTS    BB_Periph(USART2_BASE+0,9) //This bit is set by hardware when the nCTS input toggles,
#define USART2_LBD    BB_Periph(USART2_BASE+0,8) //LIN break detection flag
#define USART2_TXE    BB_Periph(USART2_BASE+0,7) //Transmit data register empty
#define USART2_TC     BB_Periph(USART2_BASE+0,6) //Transmission complete
#define USART2_RXNE   BB_Periph(USART2_BASE+0,5) //Read data register not empty
#define USART2_IDLE   BB_Periph(USART2_BASE+0,4) //IDLE line detected
#define USART2_ORE    BB_Periph(USART2_BASE+0,3) //Overrun error
#define USART2_NE     BB_Periph(USART2_BASE+0,3) //Noise error flag
#define USART2_FE     BB_Periph(USART2_BASE+0,2) //Framing error
#define USART2_PE     BB_Periph(USART2_BASE+0,1) //Parity error


//USART3状态位
#define USART3_CTS    BB_Periph(USART3_BASE+0,9) //This bit is set by hardware when the nCTS input toggles,
#define USART3_LBD    BB_Periph(USART3_BASE+0,8) //LIN break detection flag
#define USART3_TXE    BB_Periph(USART3_BASE+0,7) //Transmit data register empty
#define USART3_TC     BB_Periph(USART3_BASE+0,6) //Transmission complete
#define USART3_RXNE   BB_Periph(USART3_BASE+0,5) //Read data register not empty
#define USART3_IDLE   BB_Periph(USART3_BASE+0,4) //IDLE line detected
#define USART3_ORE    BB_Periph(USART3_BASE+0,3) //Overrun error
#define USART3_NE     BB_Periph(USART3_BASE+0,3) //Noise error flag
#define USART3_FE     BB_Periph(USART3_BASE+0,2) //Framing error
#define USART3_PE     BB_Periph(USART3_BASE+0,1) //Parity error

//SPI1控制位
#define SPI1_BIDIOE    BB_Periph(SPI1_BASE+0,14) //This bit is set by hardware when the nCTS input toggles,
//SPI2控制位
#define SPI2_BIDIOE    BB_Periph(SPI2_BASE+0,14) //This bit is set by hardware when the nCTS input toggles,

//GPIO位地址定义
#define GPA_I_Addr(i) 	BB_Periph_addr(GPIOA_BASE+0x08,i)
#define GPA_O_Addr(i) 	BB_Periph_addr(GPIOA_BASE+0x0C,i)
#define GPB_I_Addr(i) 	BB_Periph_addr(GPIOB_BASE+0x08,i)
#define GPB_O_Addr(i) 	BB_Periph_addr(GPIOB_BASE+0x0C,i)
#define GPC_I_Addr(i) 	BB_Periph_addr(GPIOC_BASE+0x08,i)
#define GPC_O_Addr(i) 	BB_Periph_addr(GPIOC_BASE+0x0C,i)
#define GPD_I_Addr(i) 	BB_Periph_addr(GPIOD_BASE+0x08,i)
#define GPD_O_Addr(i) 	BB_Periph_addr(GPIOD_BASE+0x0C,i)
#define GPE_I_Addr(i) 	BB_Periph_addr(GPIOE_BASE+0x08,i)
#define GPE_O_Addr(i) 	BB_Periph_addr(GPIOE_BASE+0x0C,i)

//GPIO位直接访问
#define GPA_I(i)		*(volatile uint32_t*)GPA_I_Addr(i)
#define GPA_O(i)		*(volatile uint32_t*)GPA_O_Addr(i)
#define GPB_I(i)		*(volatile uint32_t*)GPB_I_Addr(i)
#define GPB_O(i)		*(volatile uint32_t*)GPB_O_Addr(i)
#define GPC_I(i)		*(volatile uint32_t*)GPC_I_Addr(i)
#define GPC_O(i)		*(volatile uint32_t*)GPC_O_Addr(i)
#define GPD_I(i)		*(volatile uint32_t*)GPD_I_Addr(i)
#define GPD_O(i)		*(volatile uint32_t*)GPD_O_Addr(i)
#define GPE_I(i)		*(volatile uint32_t*)GPE_I_Addr(i)
#define GPE_O(i)		*(volatile uint32_t*)GPE_O_Addr(i)


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

#ifdef __cplusplus
 }
#endif 
#endif//_STM_32_H_
