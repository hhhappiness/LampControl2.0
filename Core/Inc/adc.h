#ifndef __ADC_H_
#define __ADC_H_
#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32.h"
#include "bsp.h"
#include "board_config.h"

#define BAT_LVL4		11600
#define BAT_LVL3		11000
#define BAT_LVL2		10600
#define BAT_LVL1		10200
#define BAT_LVL0		 9800
#define BAT_LVSHDN   8000
#define BAT_LVOFFSET	 50

extern volatile u8 AdcFlag ;
extern volatile int BatLevel ;

void AdcModeConfig(void);

u16 StartAdcSample(void);

void AdcSamp(void);

u16 adc_bubble(u16 *buf, u32 len);

int GetBatLevel(void);

int GetAvgAdc(void);
void CalcAdcCoff(int ad1, int ad2, int v1, int v2);
void SetAdcCoff(int a_coff, int c_coff);
u16 CalcVoltage(u16 adc_val);
u16 GetVoltage(void);
#ifdef __cplusplus
}
#endif 
#endif
