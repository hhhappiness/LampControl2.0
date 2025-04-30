#ifndef _RTC_H_
#define _RTC_H_
#ifdef __cplusplus
 extern "C" {
#endif 
#include "type.h"

typedef struct{
    u16 Year;  //2009->
    u8 Month; //1-12
    u8 Day;   //1-31
    u8 Hour;  //0-23
    u8 Minute;//0-59
    u8 Second;//0-59
}_TimeStruct;

extern u8 TimeDisplay;
extern _TimeStruct Time;

extern volatile u32 CloseCnt;
extern volatile u8 DispCntFlag ;
extern volatile u8 DispFastCntFlag ;

int InitRTC(void);
void Time2Str(_TimeStruct *t,char *buf);
void Time2TimeStrut(int t,_TimeStruct *T);
int TimeStruct2Time(_TimeStruct *T);
int DaysOfThisMonth(_TimeStruct *t);

#ifdef __cplusplus
 }
#endif 
#endif //_RTC_H_
