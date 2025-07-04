#include "stm32.h"
#include "bsp.h"
#include "board_config.h"
#include "Lcd.h"
#include "fonts.h"
#include "type.h"
#include "share.h"
#include "spi_io.h"
#include "core_config.h"
#include "ctrl_common.h"
#include "GUI_Page.hpp"
#include "stm32g4xx_hal_spi.h"
using namespace gui;

extern SPI_HandleTypeDef hspi2;

#define MIN(A,B) ((A)<(B)?(A):(B))
// #define WriteCommand(data) LCD_WR_REG(data)
// #define WriteData(data) LCD_WR_DATA8(data)


#ifndef BOOTLOADER		//bootloader程序不输出汉字
	#define OUTPUT_HANZI
#endif

//串口模式下只能写不能读,也不能查忙,因此用户要控制好速度不要太快
#ifdef	BOOTLOADER
static void Spi_WriteByte(unsigned char dat)
{
	LCD_CSB = 0;
	SPIIO_RW8(dat);
	LCD_CSB = 1;
}
#else

u8 spi_dat;

static inline void Spi_WriteByte(SPI_HandleTypeDef *hspi,const u8 *dat)
{
	LCD_CSB(0);
#if 0
	/* Set the transaction information */
	hspi->State       = HAL_SPI_STATE_BUSY_TX;
	hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr  = (const uint8_t *)dat;
	hspi->TxXferSize  = 1;
	hspi->TxXferCount = 1;

	/*Init field not used in handle to zero */
	hspi->pRxBuffPtr  = (uint8_t *)NULL;
	hspi->RxXferSize  = 0U;
	hspi->RxXferCount = 0U;
	hspi->TxISR       = NULL;
	hspi->RxISR       = NULL;

	*((__IO uint8_t *)&hspi->Instance->DR) = *((const uint8_t *)hspi->pTxBuffPtr);
	hspi->pTxBuffPtr ++;
	hspi->TxXferCount--;

	    while (hspi->TxXferCount > 0U)
    {
      /* Wait until TXE flag is set to send data */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
      {
        if (hspi->TxXferCount > 1U)
        {
          /* write on the data register in packing mode */
          hspi->Instance->DR = *((const uint16_t *)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr += sizeof(uint16_t);
          hspi->TxXferCount -= 2U;
        }
        else
        {
          *((__IO uint8_t *)&hspi->Instance->DR) = *((const uint8_t *)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr++;
          hspi->TxXferCount--;
        }
      }
    }

	__HAL_SPI_CLEAR_OVRFLAG(hspi);
#endif
	HAL_SPI_Transmit(&hspi2, dat, 1, HAL_MAX_DELAY);
	LCD_CSB(1);

}
#endif


static __inline void WriteCommand( u8 CommandByte )
{
	_A0_0;
	spi_dat = CommandByte;
	Spi_WriteByte(&hspi2, &spi_dat);
}
static __inline void WriteData( u8 DataByte )
{
	_A0_1;
	spi_dat = DataByte;
	Spi_WriteByte(&hspi2, &spi_dat);
}

static __inline void _SetPos(u8 page, u8 col)
{
	WriteCommand(page|0xB0); //Set Page Address
	WriteCommand( ((col)>>4) | 0x10); //Set Column Address High Byte
	WriteCommand( (col)&0x0F ); //Low Byte Colum from S128 -> S1 auto add
}
void LcdTest()
{
	WriteCommand(0xA5);
}
void LcdFullFill(u8 data){
  	uint8_t i,j;
	if(data==0 || data==0x01){
		for(i=0;i<9;i++)
		{
			WriteCommand(i|0xB0);
			WriteCommand(0x10);
			WriteCommand(0x00);
			for(j=0;j<132;j++)
			{
				WriteData(data);
			}
		}
	}else{
		WriteCommand(0xa5);
	}
	
}
void LcmFill( u8 x, u8 y, u8 w, u8 h, u8 color )
{
	u8 i,j;
	for(i=y;i<y+h;i+=8){
		_SetPos(i/8,x);
		for(j=0;j<w;j++){
			WriteData( color );
		}
	}
}

void LcmInit( void )
{
	WriteCommand(0xAE); //Display OFF
	WriteCommand(0xA2); //1/64 Duty 1/9 Bias
	WriteCommand(0xA0); //ADC select S0->S131(玻璃设计用 S1-S128)
	WriteCommand(0xC0); //com64 --> com1
	WriteCommand(0x24); //对某些模块没用,用的外部 Rb/Ra
	WriteCommand(0x81); //Sets V0
	WriteCommand(48); //内部电位器调节对比度
	WriteCommand(0x2F); //voltage follower ON regulator ON booster ON
	WriteCommand(0xA6); //Normal Display (not reverse dispplay)
	WriteCommand(0xA4); //Entire Display Disable
	WriteCommand(0x40); //Set Display Start Line = com0
	WriteCommand(0xB0); //Set Page Address = 0
	WriteCommand(0x10); //Set Column Address 4 higher bits = 0
	WriteCommand(0x00); //Set Column Address 4 lower bits = 1 , from IC SEG1 -> SEG128
	LcmClear(0);
	WriteCommand(0xAF); //Display ON
	

}

void Lcd_init(void)
{
	OLED_CS_Clr();  //打开片选使能
	OLED_RST_Clr();
	wdg();
	delay_ms(20);
	wdg();
	OLED_RST_Set();
	delay_ms(20);
	wdg();
	OLED_CS_Set();

	WriteCommand(0xE2);               //initialize interal function  
	WriteCommand(0x2F);               //power control(VB,VR,VF=1,1,1)     
	WriteCommand(0x23);               //Regulator resistor select(RR2,RR1,VRR0=0,1,1) 
	WriteCommand(0xA2);               //set LCD bias=1/9(BS=0)        
	WriteCommand(0x81);               //set reference voltage        
	WriteCommand(0x25);               //Set electronic volume (EV) level        
	WriteCommand(0xC0);               //COM:C8上下颠倒；C0 正常显示    
	WriteCommand(0xa1);               //SEG:A1 左右颠倒；A0 正常显示
	WriteCommand(0x40);               //Initial Display Line        
	WriteCommand(0xA6);               //set reverse display OFF        
	WriteCommand(0xA4);               //set all pixels OFF
	LCD_FullFill(0x00);             //full clear        
	WriteCommand(0xAF);               //turns the display ON
}


#if 1
void LcmPutChar(Uchar col,Uchar page,Uchar Order)
{
	Uchar i;
	Uchar *p = ShareInfo.DefaultAsciiFont->Find(Order);
	u8 CharWidth = ShareInfo.DefaultAsciiFont->Width;
	_SetPos(page,col);
	for(i=0;i<CharWidth;i++){
		WriteData( *p++ );
	}
	page++; //下半字符 page+1
	_SetPos(page,col);
	for(i=0;i<CharWidth;i++){
		WriteData( *p++);
	}
}
#endif

void LcmPutChar_r(Uchar col,Uchar page,Uchar Order)
{
	Uchar i;
	Uchar *p = ShareInfo.DefaultAsciiFont->Find(Order);
	u8 CharWidth = ShareInfo.DefaultAsciiFont->Width;
	_SetPos(page,col);
	for(i=0;i<CharWidth;i++){
		WriteData( ~(*p++) );
	}
	page++; //下半字符 page+1
	_SetPos(page,col);
	for(i=0;i<CharWidth;i++){
		WriteData( ~(*p++) );
	}
}

#if 0
void LcmPutChar24(Uchar col,Uchar page,Uchar Order)
{
	Uchar i,j;
	Uchar *p = FindAsciiDot24(Order);
	for(j=0;j<3;j++){
		_SetPos(page++,col);
		for(i=0;i<12;i++){
			WriteData( *p++ );
		}
	}
}


void LcmPutStr24(Uchar col,Uchar page,char *puts)
{
	while(*puts != '\0') //判断字符串时候显示完毕
	{
		if(*puts<58){
			LcmPutChar24(col,page,*puts);
			puts++;
			col=col+12; //下一个字符 8 列之后
		}else{
			/*
			if(col>(LcmXPixel-24)){ //判断行末空间是否足够放一个字符,自动换行
				page=page+3;
				col=0;
			}
			if(page>(LcmYPixel/12-3)){ //到了屏幕最下角,自动返回左上角
				page=0;
				col=0;
			}*/
			LcmPutChar(col,page+1,*puts);
			puts+=1;
			col=col+8; 
		
		}
	}
}
#endif

#ifdef OUTPUT_HANZI
//输出汉字
void LcmPutHZ(Uchar col,Uchar page,u16 Order)
{
	Uchar i;
	Uchar *	p = DEFAULT_HANZI_FONT.Find(Order); //查找字符位置
	_SetPos(page,col);
	for(i=0;i<16;i++){
		WriteData( *p++ );
	}
	page++; //下半字符 page+1
	_SetPos(page,col);
	for(i=0;i<16;i++){
		WriteData( *p++);
	}
}

void LcmPutHZ_r(Uchar col,Uchar page,u16 Order)
{
	Uchar i;
	Uchar *	p = DEFAULT_HANZI_FONT.Find(Order); //查找字符位置
	_SetPos(page,col);
	for(i=0;i<16;i++){
		WriteData( ~(*p++) );
	}
	page++; //下半字符 page+1
	_SetPos(page,col);
	for(i=0;i<16;i++){
		WriteData( ~(*p++));
	}
}
#endif

//显示字符串的函数
void LcmPutStr(Uchar col,Uchar page,char *puts)
{
	while(*puts != '\0') //判断字符串时候显示完毕
	{
		if(*puts<128){
			if(col>(LcmXPixel-8)){ //判断行末空间是否足够放一个字符,自动换行
				page=page+2;
				col=0;
			}
			if(page>(LcmYPixel/8-2)){ //到了屏幕最下角,自动返回左上角
				page=0;
				col=0;
			}
			LcmPutChar(col,page,*puts);
			puts++;
			col=col+8; //下一个字符 8 列之后
		}
	
		else{
			if(col>(LcmXPixel-16)){ //判断行末空间是否足够放一个字符,自动换行
				page=page+2;
				col=0;
			}
			if(page>(LcmYPixel/8-2)){ //到了屏幕最下角,自动返回左上角
				page=0;
				col=0;
			}
#ifdef OUTPUT_HANZI				
			LcmPutHZ(col,page,*puts<<8|*(puts+1));
#endif				
			puts+=2;
			col=col+16; //下一个字符 16 列之后			
		}
	
	}
}


void LcmPutStr_r(Uchar col,Uchar page,char *puts)
{
	while(*puts != '\0') //判断字符串时候显示完毕
	{
		if(*puts<128){
			if(col>(LcmXPixel-8)){ //判断行末空间是否足够放一个字符,自动换行
				page=page+2;
				col=0;
			}
			if(page>(LcmYPixel/8-2)){ //到了屏幕最下角,自动返回左上角
				page=0;
				col=0;
			}
			LcmPutChar_r(col,page,*puts);
			puts++;
			col=col+8; //下一个字符 8 列之后
		}else{
			if(col>(LcmXPixel-16)){ //判断行末空间是否足够放一个字符,自动换行
				page=page+2;
				col=0;
			}
			if(page>(LcmYPixel/8-2)){ //到了屏幕最下角,自动返回左上角
				page=0;
				col=0;
			}
#ifdef OUTPUT_HANZI				
			LcmPutHZ_r(col,page,*puts<<8|*(puts+1));
#endif			
			puts+=2;
			col=col+16; //下一个字符 16 列之后			
		}
	}
}
//显示 3 位数的数值(0-255)
void LcmPutNum(Uchar col,Uchar page,Uchar Num)
{
	Uchar a,b,c;
	a=Num/100;
	b=(Num%100)/10;
	c=Num%10;
	if(a==0) ; //也不写空格,直接跳过去//PutChar(col,page,0x20);
	else LcmPutChar(col,page,a+0x30);
	if(a==0 && b==0) ; //也不写空格,直接跳过去//LcmPutChar(col,page,0x20);
	else LcmPutChar(col+8,page,b+0x30);
	LcmPutChar(col+16,page,c+0x30);
}

//显示整个位图，纵向坐标是8的倍数，未做超界检查
void LcmPutBmp(u8 x,u8 y, const u8 *bmp,u8 w, u8 h)
{
	Uchar i,j;
	for(i=y;i<(y+h);i+=8)
	{
		WriteCommand(0xB0|(i/8)); //Set Page Address
		WriteCommand(((x)>>4) |0x10); //Set Column Address 高位
		WriteCommand((x)&0x0F); //colunm address 低位
		for(j=0;j<w;j++){
			WriteData( *bmp++ );
		}
	}
}

//显示部分位图，纵向坐标是8的倍数，未做超界检查
//函数LcmPutBmpRect用于在LCD上显示一个位图矩形区域
void LcmPutBmpRect(u8 x,u8 y, const u8 *bmp,u8 w, const Rect8_t * rect)
{
	//定义变量i,j
	Uchar i,j, page_end;
	//定义指针pSrc，指向位图矩形区域的起始位置
	const u8 * pSrc = bmp + rect->y/8 * w + rect->x;
	//定义指针p
	const u8 * p;
	//循环，从y开始，到y+rect->h结束，每次增加8
	page_end = (y+rect->h);
	if(pCurrPage->if8RowShow == 0) page_end += 4; //使得总体页数+1
	for(i=y;i<page_end;i+=8)
	{
		WriteCommand(0xB0|(i/8)); //Set Page Address
		WriteCommand(((x)>>4) |0x10); //Set Column Address = 0
		WriteCommand((x)&0x0F); //Colum from S1 -> S128 auto add
		p= pSrc;
		for(j=0;j<rect->w;j++){
			WriteData( *p++ );
		}
		pSrc += w;
	}
}


//add by gongj
void OutRectFill(u16 x1,u16 y1, u16 x2, u16 y2, u8 FillData)
{
	u32 i,j;
	//u8 Page ;
	u16 col;
	//Page = y1;//(y2 - y1)/8;
	for(j=y1;j<=y2;j++)
	{
		WriteCommand(0xB0|j); //Set Page Address
		col = x1;
		//WriteCommand(0x10); //Set Column Address = 0
		//WriteCommand(0x00); //Colum from S1 -> S128 auto add
		WriteCommand( ((col)>>4) | 0x10); //Set Column Address High Byte
		WriteCommand( (col)&0x0F ); //Low Byte Colum from S128 -> S1 auto add
		for(i=x1;i<x2;i++)
		{
		WriteData( FillData );
		}
	}
}

#define CONTRAST_RAW_DEFAULT 48 //原始缺省值
BOOL LcdSetContrast(int val)
{
	if(val<CONTRAST_MIN || CONTRAST_MIN>CONTRAST_MAX) return FALSE;
	val = CONTRAST_RAW_DEFAULT-CONTRAST_DEFAULT + val;
	WriteCommand(0x81); //Sets V0
	WriteCommand(0x3F&val); //内部电位器调节对比度
	return TRUE;
}
