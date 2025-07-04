#ifndef _LCD_H_
#define _LCD_H_

#ifdef __cplusplus
 extern "C" {
#endif 
#include "type.h"
#include "stm32.h"
#include "lcd_stmf4.h"

#define LcmXPixel 	128 //横向宽度
#define LcmYPixel 	64 //纵向高度
#define LcmYPage 	(LcmYPixel/8)

#define CONTRAST_DEFAULT 	5
#define CONTRAST_MIN		1
#define CONTRAST_MAX		10


typedef unsigned char Uchar;
typedef unsigned int Uint;


void LcmInit( void );
void LcdTest();
void Lcd_init( void );
void LcdFullFill(u8 data);
void LcmPutBmp(u8 x,u8 y, const u8 *bmp,u8 w, u8 h);
void LcmPutBmpRect(u8 x,u8 y, const u8 *bmp,u8 w, const Rect8_t * rect);
void LcmPutChar(Uchar col,Uchar page,Uchar Order);
void LcmPutChar_r(Uchar col,Uchar page,Uchar Order);
void LcmPutStr(Uchar col,Uchar page,char *puts);
void LcmPutStr_r(Uchar col,Uchar page,char *puts);
void LcmPutNum(Uchar col,Uchar page,Uchar Num);
BOOL LcdSetContrast(int val);

void LcmFill( u8 x, u8 y, u8 w, u8 h, u8 color );
__inline void LcmClear(u8 color)
{
	LcmFill(0,0, LcmXPixel, LcmYPixel, color);
}
void OutRectFill(u16 x1,u16 y1, u16 x2, u16 y2, u8 FillData);

void LcmPutStr24(Uchar col,Uchar page,char *puts);
#ifdef __cplusplus
}
#endif 
#endif
