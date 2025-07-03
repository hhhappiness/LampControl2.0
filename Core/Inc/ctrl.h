#ifndef __CTRL_H_
#define __CTRL_H_
#ifdef __cplusplus
 extern "C" {
#endif 
	 
	 
#include "Type.h"
#include "AppPara.h"
#include "ctrl_common.h"

//#define Status_WorkFlash 1
//#define Status_WorkStall 0
typedef enum {
	Status_idle,
	Status_WorkFlash,
	Status_WorkStall
}Status_MCU_t;

extern  Status_MCU_t Status_MCU;

#define IsLanguageCh() ((AppPara.Language == Lang_Chinese) ? 1:0)
//#define Hz2Rpm(x) 	((x*6 + 5)/10) 	//rpm = hz*60/100 =( hz*6/10 = hz*6 + 5)/10
//#define Rpm2Hz(x)	((x*10 + 3)/6)	//hz = (rpm/60)*100 = rpm*10/6

//0.01Hzת0.1rpm
#define Hz2Rpm(x) 	((x)*6) 	//rpm = (hz/100)*60*10 = hz*6
#define Rpm2Hz(x)	((x + 3)/6)	//hz = (rpm/60/10)*100 = rpm/6


//extern  int  Status_MCU;

u32 LineSpeed2Hz001(void);
void LineSpeed2Clocks(void);
void StartToFlash(void);
void StopToFlash(void);

void WriteEncrypt(void)  ;
BOOL JudgeEncrypt(void)  ;
BOOL DecodeDES(void);
BOOL TestLongShutDown(int minutes);
void ReBoot(void);
void PowerSeq(void);



#ifdef __cplusplus
}
#endif 
#endif

