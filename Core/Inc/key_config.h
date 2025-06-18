#ifndef _KEY_CONFIG_H_
#define _KEY_CONFIG_H_
#pragma once
#ifdef __cplusplus
 extern "C" {
#endif 
#include "board_config.h"

/**
\file key_config.h
本头文件配置通用按键程序中需要的宏定义的变量。
**/

#define  KeyPushTimes           2 //     //确定按下、释放的检测次数，至少是2次
#define  KeyLongPushTimes      (600/KEY_DETECT_PERIOD) //长压键盘时间间隔次数 12
#define  KeyRunTimes           (250/KEY_DETECT_PERIOD) //自动触发时间间隔次数 5
#define  DbKeyPressTime        (180/KEY_DETECT_PERIOD) //双击功能启用时，第一次按压的时间最大值，160-200ms换算
#define  DbKeyReleaseTime      (180/KEY_DETECT_PERIOD) //双击功能启用时，第一次释放到第2次压下的时间最大值，跟上一个值差不多

//使用4个键的缓冲区, 用union便于移位和清空
//Stm32对于字节处理有优化，变量可以使用字节而不影响性能
typedef union{
    u32 Word;
    u8 Key[4];
}KEY_BUF;

extern RTC_HandleTypeDef hrtc;
//不同板子用得不同定时器
#define DisableKeyTimInt() HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) //禁按键定时器中断
#define EnableKeyTimInt()  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 63, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) Error_Handler();//开按键定时器中断



//按键数量定义
#define KeyScanNum       1
#define KeyRowNum 		(6) 


//按键输入脚的位地址

static inline u32 KeyI(u8 KeyRow) {
    switch (KeyRow) {
        case 0:
            return GPB_I(4); // PB3  KEY_DIV/key_right
        case 1:
            return GPB_I(5); // PB5  KEY_ENTER
        case 2:
            return GPB_I(7); // PB6  KEY_MULT/KEY_LEFT  
        case 3:
            return GPB_I(6); // PB4  KEY_DOWN
        case 4:
            return GPB_I(3); // PB7  KEY_UP
        case 5:
            return GPB_I(0); // PB8  KEY_POWER
    }
    return 0;
};

//按键功能模式定义。
static const u8 KeyModeCtrl[KeyScanNum][KeyRowNum]={
	{
		KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        //KEY_SHIFT_EN,     已经没有mode键了
        KEY_REPEATE_EN,
       // KEY_DBCLICK_EN
	}
};



#define KeyPressLed(i)
#define KeyReleaseLed(i)


//判断按键电平。如果所有按键是相同的电平好办，定义一个宏判断高低电平就行。
//如果有些按键是高电平，有此是低电平，就要写个函数来判断了
#define IsKeyPressed ((InP)==0) //这里将电平保存在变量InP里，低电平按下，参数i未使用


#ifdef __cplusplus
}
#endif 
#endif

