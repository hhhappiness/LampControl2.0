#ifndef _GUI_DISPLAY_H_
#define _GUI_DISPLAY_H_

#include "GUI_Object.hpp"
#include "lcd.h"
#include "fonts.h"
namespace gui {
class GUI_DisplayBuf : public Bitmap
{
public:
	GUI_DisplayBuf(Pos_t w, Pos_t h, u8 * buf) ;
	GUI_DisplayBuf(const Bitmap & bitmap);
	GUI_DisplayBuf(const GUI_DisplayBuf & dispbuf);
		
	//记录当前字符输出结束位置，用于连续输出文本
	//Pos_t CurrX,CurrY;
private:
	bool LineWrapEnable;
public:	
		///当前ASCII字体，缺省为16x8
	const Font_t *pFontASCII;
	///当前Hz字体，缺省为16x16
	const Font_t *pFontHZ;

	u8 ifShortDotWidth = 0, dotNum = 0; //是否缩短“.”的宽度

	///设置页面所用ASCII字体
	void SetFontASCII(const Font_t *p = &DEFAULT_ASCII_FONT){
		if(p!=NULL) pFontASCII = p;
	}
	///设置页面所用汉字字体
	void SetFontHZ(const Font_t *p = &DEFAULT_HANZI_FONT){
		if(p!=NULL) pFontHZ = p;
	}
	
	//打印字符串时是否可以自动换行。缺省不可以。		
	int DispStr8(u8 x, u8 y, const char * str, u8 color=0);
	void DispStr8Align(u8 x, u8 y, u8 w, u8 h, Align_t align, const char * str, u8 color=0);
	int GetStrPixWidth(const char * str);
	
	void DispBmp8(u8 x, u8 y, const Bitmap& Bmp, u8 color=0);
	void DispBmp8(u8 x, u8 y, const Bitmap& Bmp, Rect8_t rect, u8 color=0);	
	void DispBmp (u8 x, u8 y, const Bitmap& Bmp, u8 color=0);
	///更新显示到Lcd，缺省为全屏
	inline void Update(u8 x=0,u8 y=0)
	{
		LcmPutBmp(x+4,y, pPix, Width, Height);
	}
	inline void Update(Rect8_t &rect, u8 x,u8 y)
	{
		LcmPutBmpRect(x+4,y, pPix,Width, &rect);
	}
	inline void Update(Rect8_t &rect)
	{
		LcmPutBmpRect(rect.x+4,rect.y, pPix,Width, &rect);
	}	
	inline void Update(u8 x,u8 y,u8 w, u8 h)
	{
		Rect8_t rect={x,y,w,h};
		LcmPutBmpRect(rect.x+4,rect.y, pPix,Width, &rect);
	}	
	void Fill8(u8 x, u8 y, u8 w, u8 h, u8 color);
	void Fill(u8 x, u8 y, u8 w, u8 h, u8 color);
	void Invert8(u8 x, u8 y, u8 w, u8 h);
	void Invert(u8 x, u8 y, u8 w, u8 h);
	///全屏填充某个颜色
	inline void Fill(u8 color)
	{
		memset(pPix, color, Height*Width/8);
	}
	///清屏为白色
	inline void Clear(void)
	{
		memset(pPix, 0, Height*Width/8);
	}
	void LineH(u8 x, u8 y, u8 len);
	void LineHWhite(u8 x, u8 y, u8 len);
	void LineV(u8 x, u8 y, u8 len);
	void Rectangle(u8 x, u8 y, u8 w, u8 h);
	///设置自动换行属性
	inline void SetLineWrap(bool val){
		LineWrapEnable = val;
	}

};

extern GUI_DisplayBuf MainDispBuf;
extern GUI_DisplayBuf SecondDispBuf;

}//namespace gui {



#endif//_GUI_DISPLAY_H_

