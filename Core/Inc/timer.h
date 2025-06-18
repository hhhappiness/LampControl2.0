#ifndef __TIME_H_
#define __TIME_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32.h"
#include "bsp.h"

typedef enum{
	Idle,			//未启动触发。
	TrigWait,
	TrigPrepared,	//设置好触发位
	Triged,			//已触发 
}TrigState_t;

#define AngleCov(a) ((((a)<<16)/100)<<14)
#define AngleCovGear(a) ((((a)<<16)/10)<<14)

extern unsigned int TrigAngle;
extern volatile int tim1_width ; //定时器1的输出脉冲宽度的最大值，2%
extern volatile int flash_pulse;
extern volatile unsigned int AvgHz;
extern volatile unsigned int AvgHz_Out;
extern volatile unsigned int NextTrigClks;
extern u32 TrigLimitWidth;
extern int ExternalSpeed;
extern volatile u8 Trig_Perimeter_En;

extern volatile u16	 tim4_psc ;
extern volatile u8 NeedRfReInit  ;

//外触发模式的启动标志
extern volatile u8 Trig_Ext_En ;
extern volatile u8 Trig_Ext_On ;

extern volatile u8 Trig_Ext_En_Wifi;
extern volatile u8 Trig_Ext_On_Wifi;

extern TrigState_t TrigState  ;

extern void DeactiveTrigPoint(void) ;

//将频率转换为时钟周期数
#define TIM1_Hz2clk(hz) ((TIMxCLK*10+hz/20)/(hz/10))	 //0.01Hz

//频率转化为要存入counter寄存器的值
#define TIM2_Hz2clk(hz) ((100*TIMXCLK/(TIM2->PSC+1)+hz/2)/(hz))	//输入的hz是原先100倍：输入的10000实则为100hz
#define TIM2_rpm2clk(rpm) TIM2_Hz2clk(rpm/600)	//0.1rmp


#define TIM2_clk2Hz(clk)	((TIMXCLK*100+clk/2)/(clk))


#define TIM4CLK  9000000
#define TIM234CLK 9000000

#define TIM4_Hz2clk(hz)	(((TIM4CLK/(tim4_psc + 1))*100+hz/2)/(hz))


#define MAX_OUT_FREQ	20000

#define MAX_PULSE_WIDTH 1000000
#define TIM2_0p2Hz_CNT	1000



extern volatile u16  image_num  ; //每版N个画面
extern volatile u32 image_period_per ; //每个版的周期
extern volatile u32	max_strobe_power;


void TIM_Reset(TIM_TypeDef * TIM);
inline void SetFlash_PulseWidth(u16 width_clk);
// void StartInternalTrig(void);
void Updata_OutPusle(void);
void TIM6_DAC_IRQHandler(void);


#ifdef __cplusplus
}
#endif 
#endif
