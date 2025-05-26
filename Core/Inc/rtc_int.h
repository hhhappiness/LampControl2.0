#ifndef __RTC_INT_H
#define __RTC_INT_H

#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32.h"
#include "type.h"

enum {
	PwrKey_Idle,
	PwrKey_Pressed,
	PwrKey_Release
};
extern u8 PwrKey_Status ;


enum {
	PwrHit_WORK,
	PwrHit_STALL
};

extern volatile u8 WorkEn ;


extern volatile u8 WorkEn ;
extern volatile u8 PwrKeyDisableFlag;
extern u8 PwrHitFlag ;
extern volatile u32 PwrHitCnt ;

extern volatile u8 EnterScanFlag ;
extern volatile u8 ScanDlyFlag;
extern volatile u16 ScanDlyCounting;
extern u16 ScanDlyCnt ;

//背光显示控制
#define BLK_ON	0
#define BLK_OFF	1
extern volatile u32 BlkCtrlCnt ;
extern volatile u8 BlkStatus ;


extern volatile u8 PwrPress_FlagON ;
extern volatile  u8 PwrPress_FlagOFF ;


//主界面直接进入调整频率的标志及其计数
extern volatile u8 MainScanFlag ;
extern u16 MainScanDlyCnt ;
extern u8 MainScanControlEn ; //防止多次进入
//任意键按下标志
extern volatile u8 AnyKeyPressedFlag ;

//充电显示状态
extern volatile u8 ChargeFlag ;
u32 getKeyBuf();
__inline void PwrKeyHit_Handler(void);
__inline void PwrKeyPress_Handler(void);
__inline void PwrKey_Detector(void);
__inline void CloseDelay_Handler(void);
__inline void Blk_Control(void);
__inline void Scan_Control(void);
__inline void MainScan_Control(void);
__inline void AnyKeyPressed_Control(void);

void ReInitSysTick(u8 freq);

void SwitchStrobeGPIO(void);

void SwitchStrobeHz(void);


void SwitchStrobeAFIO(void);

int UpdateVout(void);


#ifdef __cplusplus
}
#endif 
#endif
