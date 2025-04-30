#ifndef _KEY_BASE_H_
#define _KEY_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif 
	
void InitKey(void);
int KeyInput(void);
int GetKey(void);
	
#define KEY_NULL            0
//按键码。如果没用Shift，可以有31个，用了Shift，可以有15个。包含旋钮
#define KEY_1               1
#define KEY_2               2
#define KEY_3               3
#define KEY_4               4
#define KEY_5               5
#define KEY_6               6
#define KEY_7               7
#define KEY_8               8
#define KEY_9               9
#define KEY_10              10
#define KEY_11              11
#define KEY_12              12
#define KEY_13              13
#define KEY_14              14
#define KEY_15              15

//按键标志
#define KEY_PRESSED         0x00    //按键按下
#define KEY_SHIFT           0x10    //shift打开
#define KEY_RELEASE         0x20    //短按释放
#define KEY_REPEATE         0x40    //重复键
#define KEY_LONG_RELEASE    0x60    //长按释放
#define KEY_DOUBLE          0x80    //双击

//按键功能定义
#define KEY_REPEATE_EN  0x01    //使能重复键(不影响长按键)
#define KEY_DBCLICK_EN  0x02    //使能双击  
#define KEY_SHIFT_EN    0x04    //当作Shift键

//任意键按下标志
extern volatile unsigned char AnyKeyPressedFlag ;

#ifdef __cplusplus
}
#endif 

#endif //_KEY_BASE_H_
