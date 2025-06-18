#ifndef __TIME_H_
#define __TIME_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32.h"
#include "bsp.h"

typedef enum{
	Idle,			//δ����������
	TrigWait,
	TrigPrepared,	//���úô���λ
	Triged,			//�Ѵ��� 
}TrigState_t;

#define AngleCov(a) ((((a)<<16)/100)<<14)
#define AngleCovGear(a) ((((a)<<16)/10)<<14)

extern unsigned int TrigAngle;
extern volatile int tim1_width ; //��ʱ��1����������ȵ����ֵ��2%
extern volatile int flash_pulse;
extern volatile unsigned int AvgHz;
extern volatile unsigned int AvgHz_Out;
extern volatile unsigned int NextTrigClks;
extern u32 TrigLimitWidth;
extern int ExternalSpeed;
extern volatile u8 Trig_Perimeter_En;

extern volatile u16	 tim4_psc ;
extern volatile u8 NeedRfReInit  ;

//�ⴥ��ģʽ��������־
extern volatile u8 Trig_Ext_En ;
extern volatile u8 Trig_Ext_On ;

extern volatile u8 Trig_Ext_En_Wifi;
extern volatile u8 Trig_Ext_On_Wifi;

extern TrigState_t TrigState  ;

extern void DeactiveTrigPoint(void) ;

//��Ƶ��ת��Ϊʱ��������
#define TIM1_Hz2clk(hz) ((TIMxCLK*10+hz/20)/(hz/10))	 //0.01Hz

//Ƶ��ת��ΪҪ����counter�Ĵ�����ֵ
#define TIM2_Hz2clk(hz) ((100*TIMXCLK/(TIM2->PSC+1)+hz/2)/(hz))	//�����hz��ԭ��100���������10000ʵ��Ϊ100hz
#define TIM2_rpm2clk(rpm) TIM2_Hz2clk(rpm/600)	//0.1rmp


#define TIM2_clk2Hz(clk)	((TIMXCLK*100+clk/2)/(clk))


#define TIM4CLK  9000000
#define TIM234CLK 9000000

#define TIM4_Hz2clk(hz)	(((TIM4CLK/(tim4_psc + 1))*100+hz/2)/(hz))


#define MAX_OUT_FREQ	20000

#define MAX_PULSE_WIDTH 1000000
#define TIM2_0p2Hz_CNT	1000



extern volatile u16  image_num  ; //ÿ��N������
extern volatile u32 image_period_per ; //ÿ���������
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
