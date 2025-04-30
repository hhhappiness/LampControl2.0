#ifndef _KEY_CONFIG_H_
#define _KEY_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 
#include "board_config.h"

/**
\file key_config.h
本头文件配置通用按键程序中需要的宏定义的变量。
**/


//不同板子用得不同定时器
#define DisableKeyTimInt() DisableINT(RTC_IRQn) //禁按键定时器中断
#define EnableKeyTimInt()  EnableINT(RTC_IRQn) //开按键定时器中断


//按键数量定义
#define KeyScanNum       1
#define KeyRowNum 		(6) 

//按键输入脚的位地址
u32 KeyI[KeyScanNum][KeyRowNum]={
	{
    GPA_I_Addr(11),//PA11  KEY_DIV  //GPIOx_IDR的偏移量是0x08
    GPC_I_Addr(13),//PC13  KEY_ENTER
    GPC_I_Addr(14),//PC14  KEY_MULT
    GPA_I_Addr(12),//PA12  KEY_DOWN
    GPA_I_Addr(15),//PA15  KEY_MODE
    GPC_I_Addr(15),//PC15	KEY_UP	
	}
};



//按键功能模式定义。
static const u8 KeyModeCtrl[KeyScanNum][KeyRowNum]={
	{
		KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_SHIFT_EN,
        KEY_REPEATE_EN,
       // KEY_DBCLICK_EN
	}
};



#define KeyPressLed(i)
#define KeyReleaseLed(i)


//判断按键电平。如果所有按键是相同的电平好办，定义一个宏判断高低电平就行。
//如果有些按键是高电平，有此是低电平，就要写个函数来判断了
#define IsKeyPressed(i) ((InP)==0) //这里将电平保存在变量InP里，低电平按下，参数i未使用


#ifdef __cplusplus
}
#endif 
#endif

