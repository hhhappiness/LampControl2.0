#ifndef _FONTS_H_
#define _FONTS_H_
#include "bsp.h"
#ifdef __cplusplus
 extern "C" {
#endif 
#include "FontBase.h"

#define DEFAULT_HANZI_FONT	Song12_GB2312
#define DEFAULT_HANZI_WIDTH	12
#define DEFAULT_ASCII_FONT	Song16_ASCII_10p
#define DEFAULT_ASCII_WIDTH	7
#define DEFAULT_SPEED_FONT	Song_Width13_ASCII
#define DEFAULT_SPEED_WIDTH	13
#define DEFAULT_UNIT_FONT	Song_ASCII_w10_h16
#define DEFAULT_UNIT_WIDTH	10

extern const u8 FontNotFound[16*16/8];

u8 *FindAsciiDot16(u16 c);
u8 *FindAsciiDot24(u16 c);
u8 *FindHZ16(u16 code);


extern const Font_t Song16_ASCII_11p;
extern const Font_t Song16_ASCII_10p;
extern const Font_t Song24_ASCII;
extern const Font_t Song_Width9_ASCII;
extern const Font_t Song_Width13_ASCII;
extern const Font_t Song_ASCII_w10_h16;
extern const Font_t Song12_GB2312;
extern const Font_t *pFont8;
extern const Font_t *pFont16;


extern const u8 AsciiDot16[95][14];

#ifdef __cplusplus
}
#endif 
#endif
