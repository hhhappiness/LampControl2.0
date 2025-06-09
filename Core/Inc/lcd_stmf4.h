    //  版 本 号   : v1.0
//  作    者   : Joyee
//  生成日期   : 2019-08-17
//  最近修改   : 
//  功能描述   : OLED演示例程(STM32系列)
//              说明: 
//              ----------------------------------------------------------------
//							CS     接PD6
//              RST    接PD8
//              A0     接PD10
//              SCL    接PD12
//              SDA    接PD14
//              VCC    3.3v电源
//              GND    电源地
//              ----------------------------------------------------------------
//******************************************************************************/
#ifndef __OLED_H
#define __OLED_H
#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32g4xx.h"
#include "type.h"
#include "board_config.h"

#define LCD_W 128
#define LCD_H 64

#define CSB(x)		GPB_O(12,x)		//片选端，低电平使能	
#define	RST(x)		GPA_O(8,x)			//复位端，低电平复位
#define A0(x)		  GPB_O(14,x)			//数据、指令选择端
#define SDA(x)		GPB_O(15,x)			//串行数据端
#define SCL(x)		GPB_O(13,x)			//串行时钟端
			  
//-----------------OLED端口定义----------------  					   
#define OLED_CS_Clr()  CSB(0)//CS
#define OLED_CS_Set()  CSB(1)

#define OLED_RST_Clr() RST(0)//RES
#define OLED_RST_Set() RST(1)

#define OLED_DC_Clr() A0(0)//DC
#define OLED_DC_Set() A0(1)
 		     
#define OLED_SCLK_Clr() SCL(0)//CLK
#define OLED_SCLK_Set() SCL(1)

#define OLED_SDIN_Clr() SDA(0)//DIN
#define OLED_SDIN_Set() SDA(1)

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define RED           	 0xF800
#define GREEN         	 0x07E0
#define YELLOW        	 0xFFE0
					  		 
extern  u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void LCD_WR_DATA8(char da); //发送数据-8位参数
void LCD_WR_DATA(int da);
void LCD_WR_REG(char da);
void LCD_display();
void LCD_GPIO_Init(void);
void LCD_FullFill( uint8_t FillData );
void LCD_init(void);

void display(u8 dat1,u8 dat2);
void displaychar(u8 const *p);

void LCD_ShowChar(u8 col,u8 page,u8 Order);
void LCD_ShowStr(u8 col,u8 page,u8 *puts);
void LCD_ShowNum(u8 col,u8 page,u8 Num);
void LCD_ShowBmp( u8 const *puts );

#ifdef __cplusplus
}
#endif
#endif  
	 
	 



