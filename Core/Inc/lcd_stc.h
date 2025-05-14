#ifndef _LCD_STC_H_
#define _LCD_STC_H_

//#include <STC15F2K60S2.H>

typedef unsigned char uchar;
typedef unsigned int	uint;

void delay(void);
void delay_ms(uint ms);
void write_byte(uchar byte,uchar a);
void lcd_init(void);
void dianzhen(uchar dat1,uchar dat2);
void display_zimu(uchar line,uchar column,uchar *dat,uchar length);
void display(void);


extern const uchar zimu8[][8];
extern const uchar zimu16[][16];
extern const uchar zimu64[][64];

#endif
