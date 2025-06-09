#include "GUI_Display.hpp"
#include "GUI_Object.hpp"
#include "Lcd.h"
#include "FontBase.h"
#include "Fonts.h"

//#ifdef RTT
u8 _MainDispBuf[LcmXPixel*LcmYPixel/8];
u8 _SecondDispBuf[LcmXPixel*LcmYPixel/8];
//#endif

namespace gui {
/*
#ifndef RTT
u8 _MainDispBuf[LcmXPixel*LcmYPixel/8];
u8 _SecondDispBuf[LcmXPixel*LcmYPixel/8];
#endif
*/

GUI_DisplayBuf MainDispBuf(LcmXPixel, LcmYPixel, _MainDispBuf);
GUI_DisplayBuf SecondDispBuf(LcmXPixel, LcmYPixel, _SecondDispBuf);

	
GUI_DisplayBuf::GUI_DisplayBuf(Pos_t w, Pos_t h, u8 * buf) 
: Bitmap(w,h,buf)
,LineWrapEnable(0)
{
	SetFontASCII();
	SetFontHZ();
}

GUI_DisplayBuf::GUI_DisplayBuf(const Bitmap & bitmap) 
: Bitmap(bitmap)
, LineWrapEnable(0)
{
	SetFontASCII();
	SetFontHZ();		
}

GUI_DisplayBuf::GUI_DisplayBuf(const GUI_DisplayBuf & dispbuf) 
: Bitmap(dispbuf.Width,dispbuf.Height,dispbuf.pPix )
, LineWrapEnable(0)
{
	SetFontASCII();
	SetFontHZ();		
}


//显示字符串，纵向坐标是8的倍数
//返回字符串占的像素宽度，不过如果自动换行了宽度就没什么意义了
int GUI_DisplayBuf::DispStr8(u8 x, u8 y, const char * str, u8 color)
{	
	int StrLen=0;
	u8 *pDot;
	u8 *pDst,*p;
	u8 FontWidth,FontHeight,FontInterval;
	const u8 *ustr = (const u8 *) str; //转成unsigned char
	int page,page_end,col;
	if(x>=Width) return 0;
	if(y>=Height) return 0;
//	CurrX = x;
//	CurrY = y;
	while(*ustr != 0) //判断字符串时候显示完毕
	{	
		if(*ustr<128){
			pDot 	= pFontASCII->Find(*ustr);
			FontWidth 	= pFontASCII->Width;
			FontHeight 	= pFontASCII->Height;
			FontInterval = pFontASCII->WidthMax - pFontASCII->Width;
			ustr++;
		}else{
			pDot 	= pFontHZ->Find(*ustr<<8|*(ustr+1));
			FontWidth 	= pFontHZ->Width;
			FontHeight 	= pFontHZ->Height;
			FontInterval = 0;
			ustr+=2;
		}
		if((x+FontWidth)>Width){//自动换行
			if(LineWrapEnable){
				y+=FontHeight;
				x=0;
			}else{
				return StrLen;
			}
		}
		if(y>=Height) return StrLen;
		StrLen+=FontWidth;
		page = y/8;
		page_end =(y+FontHeight)/8;
		pDst = pPix + page*Width + x;
		
		for(; page <page_end && page < Height/8 ; page++){
			p=pDst;
			for(col=0;col<FontWidth;col++){
				*p++ = color ^ (*pDot++);
			}
			pDst+=Width;
			pDot+=FontInterval;
		}
		x+=FontWidth;
//		CurrX += FontWidth;
//		CurrY += FontHeight;
	}
	return StrLen;
}

///获取字符串在当前字体下占用的像素宽度
int GUI_DisplayBuf::GetStrPixWidth(const char * str)
{	
	int StrLen=0;
	//转成unsigned char，否则跟128比大小时会warning
	const u8 *ustr = (const u8 *) str; 

	while(*ustr != 0) //判断字符串时候显示完毕
	{	
		if(*ustr<128){
			pFontASCII->Find(*ustr);
			StrLen += pFontASCII->Width;
			ustr++;
			
		}else{
			pFontHZ->Find(*ustr<<8|*(ustr+1));
			StrLen += pFontHZ->Width;
			ustr+=2;
		}
	}
	return StrLen;
}

///调用时注意当前显示缓冲区自动换行的设置，在限宽输出时一般不会自动换行。
void GUI_DisplayBuf::DispStr8Align(u8 x, u8 y, u8 w, u8 h, Align_t align, const char * str, u8 color)
{
	//获得实际像素宽度
	int PixWidth = GetStrPixWidth(str);
	int dx;
	//如果超宽了只能按左对齐
	if(PixWidth>=w){
		align = AlignLeft;
	}
	//重新设置x坐标
	if(align == AlignMid){
		dx=(w-PixWidth)/2;
	}else if(align == AlignRight){
		dx=(w-PixWidth);
	}else{
		dx=0;
	}
	//新坐标输出文字。Todo：反显时前后空档也反显
	if(dx!=0){
		Fill8(x,y,dx,h,color);
	}
	x+=dx;
	DispStr8(x,y,str,color);
	dx = w-PixWidth-dx;
	if(dx>0){
		Fill8(x+PixWidth,y,dx,h,color);
	}
	
}

//显示字符串，纵向坐标是不限于8的倍数
int DispStr(Bitmap& Buf, u8 x, u8 y, const char * str, u8 color)
{
	int StrLen=0;
	return StrLen;
	
}

//显示整个位图，纵向坐标是8的倍数
void GUI_DisplayBuf::DispBmp8(u8 x, u8 y, const Bitmap& Bmp,u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	//判断x,y坐标是否超界
	if(x>=Width) return;
	if(y>=Height) return;
	//判断输出是否超界
	EndX = x + Bmp.Width;
	if(EndX > Width) EndX = Width;
	EndY =  y + Bmp.Height;
	if(EndY > Height) EndY = Height;
	pSrc = Bmp.pPix;
	pDst = pPix + (y/8)*Width + x;
	for(;y<EndY;y+=8){
		u8 *pd = pDst;
		u8 *ps = pSrc;
		for(i=x;i<EndX;i++){
			*pd++ = color ^ (*ps++);
		}
		pDst += Width;
		pSrc += Bmp.Width;
	}
}

//显示部分位图，纵向坐标是8的倍数
void GUI_DisplayBuf::DispBmp8(u8 x, u8 y, const Bitmap& Bmp, Rect8_t rect, u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	//判断x,y坐标是否超界
	if(x>=Width) return;
	if(y>=Height) return;
	//判断输出是否超界
	EndX = x + rect.w;
	if(EndX > Width) EndX = Width;
	EndY =  y + rect.h;
	if(EndY > Height) EndY = Height;
	pSrc = Bmp.pPix+(rect.y/8)*Bmp.Width + rect.x;
	pDst = pPix + (y/8)*Width + x;
	for(;y<EndY;y+=8){
		u8 *pd = pDst;
		u8 *ps = pSrc;
		for(i=x;i<EndX;i++){
			*pd++ = color ^ (*ps++);
		}
		pDst += Width;
		pSrc += Bmp.Width;
	}
}

//显示整个位图，纵向坐标是不限于8的倍数
void GUI_DisplayBuf::DispBmp(u8 x, u8 y, const Bitmap& Bmp, u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	u8 ybit;
	u8 RemainHeight = Bmp.Height;//剩余未显示的高度
	
	//判断x,y坐标是否超界
	if(x>=Width) return;
	if(y>=Height) return;
	//判断输出是否超界
	EndX = x + Bmp.Width;
	if(EndX > Width) EndX = Width;
	EndY =  y + Bmp.Height;
	if(EndY > Height) EndY = Height;
	//源缓冲区指针
	pSrc = Bmp.pPix;
	//目标缓冲区指针
	pDst = pPix + (y/8)*Width + x;	
	//剩余未显示的高度
	RemainHeight = Bmp.Height;
	//y坐标取8的余数
	ybit = y&0x7;
	if(ybit!=0){//顶部不对齐8像素，可能性比较多，限定两种最常用的
		if((RemainHeight &0x7)==0 && Bmp.StartBit==0){//要么位图是8的整数倍
			u8 LeadBit = 8 - ybit;
			{
				u8 *pd = pDst;
				u8 *ps = pSrc;			
				for(i=x;i<EndX;i++){
					u8 tmp = *pd;
					tmp &= (0xFF>>LeadBit); //清掉原字节的高位
					tmp |= (color ^ (*ps++)) << ybit; //或上新字节的低位
					*pd++ = tmp;			//写回
				}
				y+= LeadBit;				
			}
			//缓冲区下移一行
			//pSrc += Bmp.Width;
			pDst += Width;						

			while(RemainHeight > 8){//还剩多行
				u8 *pd = pDst;
				u8 *ps1 = pSrc;		
				u8 *ps2 = pSrc + Bmp.Width;	
				for(i=x;i<EndX;i++){
					*pd++ = (color ^ (*ps1++))>> ybit | (color ^ (*ps2++)) << LeadBit ; //第一行的高位 | 第二行的低位
				}
				y+= 8;
				//缓冲区下移一行
				pSrc += Bmp.Width;
				pDst += Width;					
			}
			//剩下单行
			{
				u8 *pd = pDst;
				u8 *ps = pSrc;
				for(i=x;i<EndX;i++){
					u8 tmp = *pd;
					tmp &= (0xFF<<ybit); //清掉原字节的低位
					tmp |= (color ^ (*ps++)) >> LeadBit; //或上新字节的高位
					*pd++ = tmp;			//写回
				}
				return;//画完退出
			}
		
			
		}else if( ybit == Bmp.StartBit){//要么顶部是互补的
			u8 FirstBit = (8-ybit);//第一行字节高字节有效，低字节无用
			u8 *pd = pDst;
			u8 *ps = pSrc;
			u8 mask = (0xFF>>FirstBit);
			for(i=x;i<EndX;i++){
				u8 tmp = *pd;
				tmp &= mask; //清掉原字节的高位
				tmp |= (color ^ (*ps++)) & ~mask; //或上新字节的高位，新字节的低位是无效字节
				*pd++ = tmp;			//写回
			}
			RemainHeight = Bmp.Height - FirstBit;
			y+=FirstBit;
			pSrc += Bmp.Width;
			pDst += Width;			
			//剩下可以按顶部对齐的方案做
		}else{
			while(1);//错误陷阱
		}
			

	}
	
	//顶部对对齐8像素，后面都简单一些
	//先绘整Page的
	while(RemainHeight >=8){
		u8 *pd = pDst;
		u8 *ps = pSrc;			
		for(i=x;i<EndX;i++){
			*pd++ = color ^ (*ps++);
		}			
		RemainHeight -= 8;
		pSrc += Bmp.Width;
		pDst += Width;
		
	}
	//如果还有不足Page的
	if(RemainHeight>0){
		u8 *pd = pDst;
		u8 *ps = pSrc;			
		for(i=x;i<EndX;i++){
			u8 tmp = *pd;
			tmp &= 0xFF<<RemainHeight;//保留高位
			tmp |= (color ^ (*ps++)) & (0xFF >> (8-RemainHeight));//新字节取低位，与tmp相或
			*pd++ = tmp;
		}				
	}

}

///填充指定矩形，y和高度是8的倍数
void GUI_DisplayBuf::Fill8(u8 x, u8 y, u8 w, u8 h, u8 color)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 EndY = y+h;
	if(EndY > LcmYPixel) EndY = LcmYPixel;
//	if(x+w > LcmXPixel) w = LcmXPixel-x;
	for(;y<EndY;y+=8){
		u8 *p = pDst;
		for(int i=0;i<w;i++){
			*p++ = color;
		}	
		pDst += Width;
	}
}

///反显指定矩形，y和高度是8的倍数
void GUI_DisplayBuf::Invert8(u8 x, u8 y, u8 w, u8 h)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 EndY = y+h;
	if(EndY>LcmYPixel) EndY = LcmYPixel;
//	if(x+w>LcmXPixel) w = LcmXPixel-x;	
	for(;y<EndY;y+=8){
		u8 *p = pDst;
		for(int i=0;i<w;i++){
			*p = ~(*p);
			p++;
		}	
		pDst += Width;
	}	
}

///反显指定矩形，y和高度不限8的倍数
void GUI_DisplayBuf::Invert(u8 x, u8 y, u8 w, u8 h)
{
	u8 *pDst = pPix + (y/8)*Width + x;
	u8 *p;
	u8 mask;
	
	u8 LowBits=y & 0x07;//低位保留的位
	//头一个不完整的Page
	if(LowBits !=0){
		u8 HighBits=8-LowBits;
		if(h<HighBits){
			//一般不带这样玩的
			return;
		}
		p = pDst;
		mask = 0xFF<< LowBits;
		for(int i=0;i<w;i++){
			u8 tmp = *p & ~mask;//保留低位
			*p = tmp | (~(*p) & mask);//高位取反
			p++;
		}
		pDst += Width;
		h -= HighBits;
		
	}
	
	
	//中间完整的Page
	while(h>=8){
		p = pDst;
		for(int i=0;i<w;i++){
			*p = ~(*p);//取反
			p++;
		}		
		pDst += Width;
		h -= 8;
	}
	//如果后面还有不完整的page
	if(h>0){
		u8 HighBits = 8-h;
		mask = 0xFF >> HighBits;
		p = pDst;
		for(int i=0;i<w;i++){
			u8 tmp = *p & ~mask;//保留高位
			*p = tmp | (~(*p) & mask);//低位取反
			p++;
		}			
	}	
}

///画矩形，颜色为黑
void GUI_DisplayBuf::Rectangle(u8 x, u8 y, u8 w, u8 h)
{
	LineH(x,y,w);
	LineH(x,y+h-1,w);
	LineV(x,y,h);
	LineV(x+w-1,y,h);
}

///画横线，颜色为黑
void GUI_DisplayBuf::LineH(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 LowBits=y & 0x07;
	u8 mask = 1<<LowBits;//位掩码
	for(int i=0;i<len;i++){
		*pDst |= mask;
		pDst++;
	}
}

///画横线，颜色为白
void GUI_DisplayBuf::LineHWhite(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 LowBits=y & 0x07;
	u8 mask = ~(1<<LowBits);//位掩码
	for(int i=0;i<len;i++){
		*pDst &= mask;
		pDst++;
	}
}

///画竖线，颜色为黑
void GUI_DisplayBuf::LineV(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;
	u8 mask;
	
	u8 LowBits=y & 0x07;//低位保留的位
	//头一个不完整的Page
	if(LowBits !=0){
		u8 HighBits=8-LowBits;
		if(len<=HighBits){//线太短了，没到Page底就结束了
			HighBits -= len;
			mask = (0xFF<< LowBits) & (0xFF>>HighBits);
			*pDst |= mask;
			return;
		}
		mask = 0xFF<< LowBits;
		*pDst |= mask;
		pDst += Width;
		len -= HighBits;		
	}
	

	//中间完整的Page
	while(len>=8){
		*pDst = 0xFF;
		pDst += Width;
		len -= 8;
	}
	//如果后面还有不完整的page
	if(len>0){
		u8 HighBits = 8-len;
		mask = 0xFF >> HighBits;
		*pDst |= mask;
	}
}



}//namespace gui {





