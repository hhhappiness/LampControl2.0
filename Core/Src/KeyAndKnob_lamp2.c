//#include "stm32f10x_it.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_rtc_ex.h"
#include "bsp.h"
#include "key_config.h"
#include "RTC.h"
#include "rtc_int.h"
#include "Key_Base.h"
#include "encoder.h"
#include "key.h"
#include <stdio.h>

extern int encoder_val;
//按键状态机
typedef enum{
    KS_Idle,
    KS_Pressed1,
    KS_WaitDbClick,
    KS_Long_Pressed,
}KeyState_t;

#ifndef REPEAT_TURN_POINT1	
	#define REPEAT_TURN_POINT1 25
#endif

#ifndef REPEAT_TURN_POINT2	
	#define REPEAT_TURN_POINT2 40
#endif

//参数值校验
#if KeyPushTimes < 2
    #error KeyPushTimes must >= 2
#endif

#if KeyLongPushTimes < 5 || KeyLongPushTimes<(KeyPushTimes+KeyRunTimes)
    #error KeyLongPushTimes must >= 5 and KeyLongPushTimes must >=(KeyPushTimes+KeyRunTimes)
#endif

#if KeyRunTimes < 1
    #error KeyRunTimes must >= 1
#endif

#if KeyScanNum > 1	//有按键扫描
	int      KeyScanId =      0;
#else	//无按键扫描
	#define KeyScanId 	0
#endif
s16 Encode;
s16 EncodeLast;






KEY_BUF KeyBuf;
#define KeyOutBuf KeyBuf.Key[3] //键码输出字节
static u8   PressCount[KeyScanNum][KeyRowNum];
static u8   ReleaseCount[KeyScanNum][KeyRowNum];
static u8   KeyState[KeyScanNum][KeyRowNum];
KEY_BUF     KeyNum;
 u8   		KeyBufNum;    //缓冲区内存有的按键数
volatile u8 AnyKeyPressedFlag = 0;
u8   ShiftState=0;  //shift键状态，0或KEY_SHIFT
//重复键移到应用层处理。
// u8   		KeyRepeatNum; //重复键数
//u16         KeyRepeatNumRead;    //已读重复键数
//u16         KeyRepeatNumReadSum; //累计已读重复键数

u32 getKeyBuf()
{
    return KeyBuf.Word;  
}
//键值存入缓冲区
//0=>1=>2=>3
 void InKeyBuf(u8 x){
    switch (KeyBufNum) {
    case 1:
        KeyBuf.Key[2]=x;
        KeyBufNum=2;
        break;
    case 2:
        KeyBuf.Key[1]=x;
        KeyBufNum=3;
        break;
    case 3:
        KeyBuf.Key[0]=x;
        KeyBufNum=4;
        break;
    case 4:
        KeyBuf.Word=KeyBuf.Word<<8; //按键移位进入缓冲区, 如果有键未读会移出  ori = " >> "
		KeyBuf.Key[0]=x;
    default:
        KeyBuf.Key[3]=x;
        KeyBufNum=1;
        break;
    }
}

#if KeyScanNum > 1	//有按键扫描
  //扫描下一列
    inline static void ScanNextCol(){
    	KeyScanId++;
    	if(KeyScanId==KeyScanNum) KeyScanId = 0;
    };
#else   //没有就定义一个空宏
    #define ScanNextCol()
#endif


//初始化按键缓冲区
void InitKey(void)
{
	int i,j;
    for (j=0;j<KeyScanNum;j++) {
        for(i=0;i<KeyRowNum;i++) {
            PressCount[j][i]=0;//清空按键计数缓冲区
            ReleaseCount[j][i]=0;
			KeyState[j][i]=0;
        }
    }
    ShiftState = 0;
    //清除按键缓冲区
    ClearKey();
}

//TIM4 50Hz for Keyboard
//频闪灯是用的RTC中断，节省一个TIM

//清空按键缓冲区
void ClearKey(void){
    DisableKeyTimInt(); 
    KeyBuf.Word=0;
    KeyBufNum =0;
   // KeyRepeatNum=0;
    EnableKeyTimInt();
}


//读取当前缓冲区的键码
int GetKey(){
    s16 tmp;
    DisableKeyTimInt();//禁按键定时器中断

    //频闪灯改用了不同的算法，KeyRepeatNum一直累计，不清0，
    //KeyRepeatNumRead保存上次读的值。根据KeyRepeatNum值的大小换算成不同的值
    if (KeyBufNum==0) {
        tmp=KEY_NULL;
        if(encoder_buff_num > 0) { 
            encoder_val = GetEncoder(); //读取编码器值
            tmp = KEY_ENCODER; 
        }
    }else {
        tmp=KeyOutBuf;
        KeyBuf.Word=KeyBuf.Word<<8 ; //按键移出缓冲区

        KeyBufNum--;
    }

    EnableKeyTimInt();//开按键定时器中断
    return tmp;
}

#define InKey_Press()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+ShiftState+1); \
                       \
}





#define InKey_Release()  {                \
    AnyKeyPressedFlag = 1;              \
    InKeyBuf(i*KeyScanNum+KeyScanId+1+ShiftState+KEY_RELEASE); \
}
#define InKey_Long()  {                \
    AnyKeyPressedFlag = 1;              \
	InKeyBuf(i*KeyScanNum+KeyScanId+1+KEY_REPEATE); \
} 
#define InKey_LongRelease()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_LONG_RELEASE); \
} 

#define InKey_Repeate()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_REPEATE); \
} 

#define InKey_DbClick()  {                \
    AnyKeyPressedFlag = 1;              \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_DOUBLE); \
}
#define StoreInp 
static inline int ScanKeyPress()
{
    
    return -1;
}
//按键波形与计数器之间的关系
//          HHHLHLLLLLLLLLHLHHHHHHHHH
//Press     000101234567899AAA0000000 //遇低+1, 未稳时遇高清0, 稳定时遇高不变
//Release   0000000000000010120000000 //遇低清0, 按下稳定后才起作用, 用于确认释放
//                x          x  
int KeyInput_1(void)
{
    int i=0;
    u8 PressCnt,ReleaseCnt,State,KeyMode;
    int InP;
    int j=0;
    for(;j<KeyRowNum;j++) {
        InP=KeyI(j);
        if (InP ==0 ){
            i=j;
            break; //有按键按下
        }
    }
    State = KeyState[KeyScanId][i];
    if(IsKeyPressed || State !=KS_Idle) {//有按键按下||状态不是空闲
        PressCnt = PressCount[KeyScanId][i];       //取压键次数
        ReleaseCnt  = ReleaseCount[KeyScanId][i];
        KeyMode = KeyModeCtrl[KeyScanId][i];
        switch (State) {
        case KS_Idle://空闲时必须是
            //注1：必须有一定的释放时间才可以开始状态机，防止某个键死锁，或者按键按下时被Knob清掉了状态机, 必须释放后才重新才始。
            if (IsKeyPressed) {
                if (ReleaseCnt >= KeyPushTimes) { //注1
                    PressCnt++; //压键计数增加
                    if (PressCnt >= KeyPushTimes){//确认压下
						State = KS_Pressed1;
                        if (KeyMode & (KEY_DBCLICK_EN |KEY_SHIFT_EN)) {
                            //双击或Shift时不保存按下键
                        }else{
                            break;//第一次不保存按下键，先进行是否长按判断，如无继续长按，则保存短按按键
                        }
                        KeyPressLed(i);  //点亮LED
						ReleaseCnt = 0;
                    }
                }
            }
            break;
        case KS_Pressed1:
            if (IsKeyPressed) {
                PressCnt++; //压键计数增加，
                ReleaseCnt=0; //清掉可能出现的短暂释放
                if(PressCnt == KeyLongPushTimes){
                    State = KS_Long_Pressed;
                    if (KeyMode & KEY_SHIFT_EN) {//如果是shift键，设置状态，也记按键
                        ShiftState = KEY_SHIFT;
                    }
                    InKey_Repeate();
                }
            }else{
                ReleaseCnt++;
                if(PressCnt>=2){
                    InKey_Press(); //若按压次数大于等于2，则认为按下但没长按，保存按键
                    //printf("pressed \r\n");
                    PressCnt=0;
                }
                if (ReleaseCnt>=KeyPushTimes){//确认释放
                    //使能双击并且，并且压键时间小于DbKeyPressTime
                    if((KeyModeCtrl[KeyScanId][i] & KEY_DBCLICK_EN) && PressCnt < DbKeyPressTime){
                        State = KS_WaitDbClick;//Release1,并等待第2次按下
                    }else{//单击释放
                        State = KS_Idle;
                        InKey_Release();
                    }
                    KeyReleaseLed(i);
                }
            }
            break;
        case KS_WaitDbClick:
            if (IsKeyPressed) {
                PressCnt++; //压键计数增加
                if (PressCnt >= KeyPushTimes) {
                    if (ReleaseCnt < DbKeyReleaseTime) {
                        State = KS_Idle;
                        InKey_DbClick();
                    }
                    ReleaseCnt = 0;
                }
            }else{
                ReleaseCnt++;
                if (ReleaseCnt > DbKeyReleaseTime) {//确认是单击
                    State = KS_Idle;
                    InKey_Release();
                }
                PressCnt = 0;
            }
            break;
        case KS_Long_Pressed:
            if (IsKeyPressed) {
                if(KeyModeCtrl[KeyScanId][i] & KEY_REPEATE_EN){//如果允许重复键
                    PressCnt++; //压键计数增加，释放保持不变
                    if (PressCnt == KeyLongPushTimes+KeyRunTimes) {
                        InKey_Repeate();
                        PressCnt = KeyLongPushTimes;	//按下计数倒退到KeyLongPushTimes，准备下一次长按
                    }
                }
                ReleaseCnt = 0;//清掉可能的毛刺
            }else{
                ReleaseCnt++;
                if (ReleaseCnt>=KeyPushTimes){
                    State = KS_Idle;
                    if (KeyMode & KEY_SHIFT_EN) {//如果是shift键，清状态
                        ShiftState = 0;
                    }
                    InKey_LongRelease();
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        }

        
    }
    else if(State ==KS_Idle){
        if(ReleaseCnt < KeyPushTimes) ReleaseCnt++; //注1
        //如果有按下，次数不够又被释放，清掉。
        PressCnt = 0;
    }
    PressCount[KeyScanId][i] = PressCnt;
    ReleaseCount[KeyScanId][i]=ReleaseCnt;
    KeyState[KeyScanId][i] = State;
	ScanNextCol();
	return 0;
}


int KeyInput(void)
{
    int i=0;
    u8 PressCnt,ReleaseCnt,State,KeyMode;
    int InP;
    for(i=0;i<KeyRowNum;i++) {//每个周期只做一个扫描线
		InP=KeyI(i);  //读按键电平
        PressCnt = PressCount[KeyScanId][i];       //取压键次数
        ReleaseCnt  = ReleaseCount[KeyScanId][i];
        State = KeyState[KeyScanId][i];
        KeyMode = KeyModeCtrl[KeyScanId][i];
        switch (State) {
        case KS_Idle://空闲时必须是
            //注1：必须有一定的释放时间才可以开始状态机，防止某个键死锁，或者按键按下时被Knob清掉了状态机, 必须释放后才重新才始。
            if (IsKeyPressed){
                if (ReleaseCnt >= KeyPushTimes) { //注1
                    PressCnt++; //压键计数增加
                    if (PressCnt >= KeyPushTimes){//确认压下
						State = KS_Pressed1;
                        if (KeyMode & (KEY_DBCLICK_EN |KEY_SHIFT_EN)) {
                            //双击或Shift时不保存按下键
                        }else{    //到下一步判断是长按还是短按
                            break;
                        }
                        KeyPressLed(i);  //点亮LED
						ReleaseCnt = 0;
                    }
                }
            }else{
                if(ReleaseCnt < KeyPushTimes) ReleaseCnt++; //注1
                //如果有按下，次数不够又被释放，清掉。
                PressCnt = 0;
            }
            break;
        case KS_Pressed1:
            if (IsKeyPressed) {
                PressCnt++; //压键计数增加，
                ReleaseCnt=0; //清掉可能出现的短暂释放
                if(PressCnt == KeyLongPushTimes){
                    State = KS_Long_Pressed;
                    if (KeyMode & KEY_SHIFT_EN) {//如果是shift键，设置状态，也记按键
                        ShiftState = KEY_SHIFT;
                    }
                    InKey_Repeate();
                }
            }else{
                ReleaseCnt++;
                if(PressCnt>=2){
                    InKey_Press(); //若按压次数大于等于2，则认为按下但没长按，保存按键
                    PressCnt=0;
                }
                if (ReleaseCnt>=KeyPushTimes){//确认释放
                    //使能双击并且，并且压键时间小于DbKeyPressTime
                    if((KeyModeCtrl[KeyScanId][i] & KEY_DBCLICK_EN) && PressCnt < DbKeyPressTime){
                        State = KS_WaitDbClick;//Release1,并等待第2次按下
                    }else{//单击释放
                        State = KS_Idle;
                        InKey_Release();
                    }
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        case KS_WaitDbClick:
            if (IsKeyPressed) {
                PressCnt++; //压键计数增加
                if (PressCnt >= KeyPushTimes) {
                    if (ReleaseCnt < DbKeyReleaseTime) {
                        State = KS_Idle;
                        InKey_DbClick();
                    }
                    ReleaseCnt = 0;
                }
            }else{
                ReleaseCnt++;
                if (ReleaseCnt > DbKeyReleaseTime) {//确认是单击
                    State = KS_Idle;
                    InKey_Release();
                }
                PressCnt = 0;
            }
            break;
        case KS_Long_Pressed:
            if (IsKeyPressed) {
                if(KeyModeCtrl[KeyScanId][i] & KEY_REPEATE_EN){//如果允许重复键
                    PressCnt++; //压键计数增加，释放保持不变
                    if (PressCnt == KeyLongPushTimes+KeyRunTimes) {
                        InKey_Repeate();
                        PressCnt = KeyLongPushTimes;	//按下计数倒退到KeyLongPushTimes，准备下一次长按
                    }
                }
                ReleaseCnt = 0;//清掉可能的毛刺
            }else{
                ReleaseCnt++;
                if (ReleaseCnt>=KeyPushTimes){
                    State = KS_Idle;
                    if (KeyMode & KEY_SHIFT_EN) {//如果是shift键，清状态
                        ShiftState = 0;
                    }
                    InKey_LongRelease();
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        }

        PressCount[KeyScanId][i] = PressCnt;
        ReleaseCount[KeyScanId][i]=ReleaseCnt;
        KeyState[KeyScanId][i] = State;
    }

	ScanNextCol();
	return 0;
}


