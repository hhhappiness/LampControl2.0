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


//��ʾ�ַ���������������8�ı���
//�����ַ���ռ�����ؿ�ȣ���������Զ������˿�Ⱦ�ûʲô������
int GUI_DisplayBuf::DispStr8(u8 x, u8 y, const char * str, u8 color)
{	
	int StrLen=0;
	u8 *pDot;
	u8 *pDst,*p;
	u8 FontWidth,FontHeight,FontInterval;
	const u8 *ustr = (const u8 *) str; //ת��unsigned char
	int page,page_end,col;
	if(x>=Width) return 0;
	if(y>=Height) return 0;
//	CurrX = x;
//	CurrY = y;
	while(*ustr != 0) //�ж��ַ���ʱ����ʾ���
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
		if((x+FontWidth)>Width){//�Զ�����
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

///��ȡ�ַ����ڵ�ǰ������ռ�õ����ؿ��
int GUI_DisplayBuf::GetStrPixWidth(const char * str)
{	
	int StrLen=0;
	//ת��unsigned char�������128�ȴ�Сʱ��warning
	const u8 *ustr = (const u8 *) str; 

	while(*ustr != 0) //�ж��ַ���ʱ����ʾ���
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

///����ʱע�⵱ǰ��ʾ�������Զ����е����ã����޿����ʱһ�㲻���Զ����С�
void GUI_DisplayBuf::DispStr8Align(u8 x, u8 y, u8 w, u8 h, Align_t align, const char * str, u8 color)
{
	//���ʵ�����ؿ��
	int PixWidth = GetStrPixWidth(str);
	int dx;
	//���������ֻ�ܰ������
	if(PixWidth>=w){
		align = AlignLeft;
	}
	//��������x����
	if(align == AlignMid){
		dx=(w-PixWidth)/2;
	}else if(align == AlignRight){
		dx=(w-PixWidth);
	}else{
		dx=0;
	}
	//������������֡�Todo������ʱǰ��յ�Ҳ����
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

//��ʾ�ַ��������������ǲ�����8�ı���
int DispStr(Bitmap& Buf, u8 x, u8 y, const char * str, u8 color)
{
	int StrLen=0;
	return StrLen;
	
}

//��ʾ����λͼ������������8�ı���
void GUI_DisplayBuf::DispBmp8(u8 x, u8 y, const Bitmap& Bmp,u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	//�ж�x,y�����Ƿ񳬽�
	if(x>=Width) return;
	if(y>=Height) return;
	//�ж�����Ƿ񳬽�
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

//��ʾ����λͼ������������8�ı���
void GUI_DisplayBuf::DispBmp8(u8 x, u8 y, const Bitmap& Bmp, Rect8_t rect, u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	//�ж�x,y�����Ƿ񳬽�
	if(x>=Width) return;
	if(y>=Height) return;
	//�ж�����Ƿ񳬽�
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

//��ʾ����λͼ�����������ǲ�����8�ı���
void GUI_DisplayBuf::DispBmp(u8 x, u8 y, const Bitmap& Bmp, u8 color)
{
	int EndX,EndY;
	u8 * pSrc, *pDst;
	int i; 
	u8 ybit;
	u8 RemainHeight = Bmp.Height;//ʣ��δ��ʾ�ĸ߶�
	
	//�ж�x,y�����Ƿ񳬽�
	if(x>=Width) return;
	if(y>=Height) return;
	//�ж�����Ƿ񳬽�
	EndX = x + Bmp.Width;
	if(EndX > Width) EndX = Width;
	EndY =  y + Bmp.Height;
	if(EndY > Height) EndY = Height;
	//Դ������ָ��
	pSrc = Bmp.pPix;
	//Ŀ�껺����ָ��
	pDst = pPix + (y/8)*Width + x;	
	//ʣ��δ��ʾ�ĸ߶�
	RemainHeight = Bmp.Height;
	//y����ȡ8������
	ybit = y&0x7;
	if(ybit!=0){//����������8���أ������ԱȽ϶࣬�޶�������õ�
		if((RemainHeight &0x7)==0 && Bmp.StartBit==0){//Ҫôλͼ��8��������
			u8 LeadBit = 8 - ybit;
			{
				u8 *pd = pDst;
				u8 *ps = pSrc;			
				for(i=x;i<EndX;i++){
					u8 tmp = *pd;
					tmp &= (0xFF>>LeadBit); //���ԭ�ֽڵĸ�λ
					tmp |= (color ^ (*ps++)) << ybit; //�������ֽڵĵ�λ
					*pd++ = tmp;			//д��
				}
				y+= LeadBit;				
			}
			//����������һ��
			//pSrc += Bmp.Width;
			pDst += Width;						

			while(RemainHeight > 8){//��ʣ����
				u8 *pd = pDst;
				u8 *ps1 = pSrc;		
				u8 *ps2 = pSrc + Bmp.Width;	
				for(i=x;i<EndX;i++){
					*pd++ = (color ^ (*ps1++))>> ybit | (color ^ (*ps2++)) << LeadBit ; //��һ�еĸ�λ | �ڶ��еĵ�λ
				}
				y+= 8;
				//����������һ��
				pSrc += Bmp.Width;
				pDst += Width;					
			}
			//ʣ�µ���
			{
				u8 *pd = pDst;
				u8 *ps = pSrc;
				for(i=x;i<EndX;i++){
					u8 tmp = *pd;
					tmp &= (0xFF<<ybit); //���ԭ�ֽڵĵ�λ
					tmp |= (color ^ (*ps++)) >> LeadBit; //�������ֽڵĸ�λ
					*pd++ = tmp;			//д��
				}
				return;//�����˳�
			}
		
			
		}else if( ybit == Bmp.StartBit){//Ҫô�����ǻ�����
			u8 FirstBit = (8-ybit);//��һ���ֽڸ��ֽ���Ч�����ֽ�����
			u8 *pd = pDst;
			u8 *ps = pSrc;
			u8 mask = (0xFF>>FirstBit);
			for(i=x;i<EndX;i++){
				u8 tmp = *pd;
				tmp &= mask; //���ԭ�ֽڵĸ�λ
				tmp |= (color ^ (*ps++)) & ~mask; //�������ֽڵĸ�λ�����ֽڵĵ�λ����Ч�ֽ�
				*pd++ = tmp;			//д��
			}
			RemainHeight = Bmp.Height - FirstBit;
			y+=FirstBit;
			pSrc += Bmp.Width;
			pDst += Width;			
			//ʣ�¿��԰���������ķ�����
		}else{
			while(1);//��������
		}
			

	}
	
	//�����Զ���8���أ����涼��һЩ
	//�Ȼ���Page��
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
	//������в���Page��
	if(RemainHeight>0){
		u8 *pd = pDst;
		u8 *ps = pSrc;			
		for(i=x;i<EndX;i++){
			u8 tmp = *pd;
			tmp &= 0xFF<<RemainHeight;//������λ
			tmp |= (color ^ (*ps++)) & (0xFF >> (8-RemainHeight));//���ֽ�ȡ��λ����tmp���
			*pd++ = tmp;
		}				
	}

}

///���ָ�����Σ�y�͸߶���8�ı���
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

///����ָ�����Σ�y�͸߶���8�ı���
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

///����ָ�����Σ�y�͸߶Ȳ���8�ı���
void GUI_DisplayBuf::Invert(u8 x, u8 y, u8 w, u8 h)
{
	u8 *pDst = pPix + (y/8)*Width + x;
	u8 *p;
	u8 mask;
	
	u8 LowBits=y & 0x07;//��λ������λ
	//ͷһ����������Page
	if(LowBits !=0){
		u8 HighBits=8-LowBits;
		if(h<HighBits){
			//һ�㲻���������
			return;
		}
		p = pDst;
		mask = 0xFF<< LowBits;
		for(int i=0;i<w;i++){
			u8 tmp = *p & ~mask;//������λ
			*p = tmp | (~(*p) & mask);//��λȡ��
			p++;
		}
		pDst += Width;
		h -= HighBits;
		
	}
	
	
	//�м�������Page
	while(h>=8){
		p = pDst;
		for(int i=0;i<w;i++){
			*p = ~(*p);//ȡ��
			p++;
		}		
		pDst += Width;
		h -= 8;
	}
	//������滹�в�������page
	if(h>0){
		u8 HighBits = 8-h;
		mask = 0xFF >> HighBits;
		p = pDst;
		for(int i=0;i<w;i++){
			u8 tmp = *p & ~mask;//������λ
			*p = tmp | (~(*p) & mask);//��λȡ��
			p++;
		}			
	}	
}

///�����Σ���ɫΪ��
void GUI_DisplayBuf::Rectangle(u8 x, u8 y, u8 w, u8 h)
{
	LineH(x,y,w);
	LineH(x,y+h-1,w);
	LineV(x,y,h);
	LineV(x+w-1,y,h);
}

///�����ߣ���ɫΪ��
void GUI_DisplayBuf::LineH(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 LowBits=y & 0x07;
	u8 mask = 1<<LowBits;//λ����
	for(int i=0;i<len;i++){
		*pDst |= mask;
		pDst++;
	}
}

///�����ߣ���ɫΪ��
void GUI_DisplayBuf::LineHWhite(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;	
	u8 LowBits=y & 0x07;
	u8 mask = ~(1<<LowBits);//λ����
	for(int i=0;i<len;i++){
		*pDst &= mask;
		pDst++;
	}
}

///�����ߣ���ɫΪ��
void GUI_DisplayBuf::LineV(u8 x, u8 y, u8 len)
{
	u8 *pDst = pPix + (y/8)*Width + x;
	u8 mask;
	
	u8 LowBits=y & 0x07;//��λ������λ
	//ͷһ����������Page
	if(LowBits !=0){
		u8 HighBits=8-LowBits;
		if(len<=HighBits){//��̫���ˣ�û��Page�׾ͽ�����
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
	

	//�м�������Page
	while(len>=8){
		*pDst = 0xFF;
		pDst += Width;
		len -= 8;
	}
	//������滹�в�������page
	if(len>0){
		u8 HighBits = 8-len;
		mask = 0xFF >> HighBits;
		*pDst |= mask;
	}
}



}//namespace gui {





