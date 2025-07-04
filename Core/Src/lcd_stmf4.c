#include "lcd_stmf4.h"
#include "stdlib.h"
#include "stm32.h"
#include "main.h"
#include "ctrl.h"
//#include "delay.h"
u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色
// u8 const ComTable[]={7,6,5,4,3,2,1,0,};
u8 Key=1;
u16 t;  
u16 len;	
u16 times=0;
u8 i;
u8 contrast=40;                    //对比度=48
void DelayKey(unsigned int Second , unsigned int MS100)
{
		unsigned int i;
		for(i=0;i<Second*100+MS100*10;i++)
		{
				if(Key==0)
				{
    //向LCD写入命令
						delay_ms(20);
	//向LCD写入数据
						while(Key==0) {delay_ms(20);}
						break;
				}
				else delay_ms(10);
		}
}

void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CSB_Pin|LCD_A0_Pin|LCD_SDA_Pin|LCD_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RSTB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_CSB_Pin LCD_A0_Pin LCD_RSTB_Pin */
  GPIO_InitStruct.Pin = LCD_CSB_Pin|LCD_A0_Pin|LCD_SCL_Pin|LCD_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA_Pin SCL_Pin */
  GPIO_InitStruct.Pin = LCD_RSTB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}
#if 0
void LCD_display(){
  	LCD_ShowBmp(bmp3);
	DelayKey(1,0);
	for(i=(contrast-5);i<(contrast+5);i++)
	{
			LCD_WR_REG(0x81);                                 //Sets V0
			LCD_WR_REG(0x3F&i);                               //内部电位器调节对比度
			LCD_ShowNum(2,0,i);
			DelayKey(1,0);
	}
	LCD_WR_REG(0x81);                                     //Sets V0
	LCD_WR_REG(contrast);                                 //恢复对比度
	//LCD_ShowNum(2,0,contrast);

	DelayKey(1,0);
	delay_ms(100);
}
#endif
void LCD_Writ_Bus(char dat)   //串行数据写入
{	
	u8 i;	
 	OLED_CS_Clr();	//片选使能
  //delay_us(1);       //确保片选有效
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();  //时钟线拉低
		if(dat&0x80)
			OLED_SDIN_Set();
		else 
			OLED_SDIN_Clr();
		delay_us(1);   
		OLED_SCLK_Set();  //时钟线拉高，此时读取SDIN线数据
		delay_us(1);
		dat<<=1;
	}
	//OLED_SDIN_Clr();  //每传输一个完整的字节，SDIN置低
  OLED_CS_Set();	//片选失能
}

#if 1
void LCD_WR_DATA8(char da) //发送8位数据
{	//OLED_CS_Clr();
    OLED_DC_Set();
		LCD_Writ_Bus(da);  
	//OLED_CS_Set();
}  
 void LCD_WR_DATA(int da) //发送16位数据
{//	OLED_CS_Clr();
    OLED_DC_Set();
	LCD_Writ_Bus(da>>8);
    LCD_Writ_Bus(da);
	//OLED_CS_Set();
}	  
void LCD_WR_REG(char da)	 //发送8位命令
{	//	OLED_CS_Clr();
    OLED_DC_Clr();
	LCD_Writ_Bus(da);
	//OLED_CS_Set();
}
 void LCD_WR_REG_DATA(char reg,int da)
{	//OLED_CS_Clr();
    LCD_WR_REG(reg);
	LCD_WR_DATA(da);
	//OLED_CS_Set();
}
#endif
void LCD_FullFill( uint8_t FillData )
{
		uint8_t i,j;
		for(i=0;i<9;i++)
		{
				LCD_WR_REG(i|0xB0);
				LCD_WR_REG(0x10);
				LCD_WR_REG(0x00);
			  for(j=0;j<132;j++)
				{
					  LCD_WR_DATA8(FillData);
				}
		}
}

void LCD_init(void)
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

	LCD_WR_REG(0xE2);               //initialize interal function  
	LCD_WR_REG(0x2F);               //power control(VB,VR,VF=1,1,1)     
	LCD_WR_REG(0x23);               //Regulator resistor select(RR2,RR1,VRR0=0,1,1) 
	LCD_WR_REG(0xA2);               //set LCD bias=1/9(BS=0)        
	LCD_WR_REG(0x81);               //set reference voltage        
	LCD_WR_REG(0x25);               //Set electronic volume (EV) level        
	LCD_WR_REG(0xC0);               //COM:C8上下颠倒；C0 正常显示    
	LCD_WR_REG(0xa1);               //SEG:A1 左右颠倒；A0 正常显示
	LCD_WR_REG(0x40);               //Initial Display Line        
	LCD_WR_REG(0xA6);               //set reverse display OFF        
	LCD_WR_REG(0xA4);               //set all pixels OFF
	LCD_FullFill(0x00);             //full clear        
	LCD_WR_REG(0xAF);               //turns the display ON
}

#if 0
void display(u8 dat1,u8 dat2)
{
   	u8 row,col;

   	for (row=0xb0; row<0xb8; row++)  
   	{
      		LCD_WR_REG(row);//set page address
      		LCD_WR_REG(0x10);//set column address
      		LCD_WR_REG(0x00);
      		for(col=0;col<128;col++)
      		{
							 LCD_WR_DATA8(dat1);
							 LCD_WR_DATA8(dat2);
      		}
  	}

}

void displaychar(u8 const *p)
{
   	u8 row,col;

   	for (row=0xb0; row<0xb8; row++)
   	{
					LCD_WR_REG(row);//set page address
      		LCD_WR_REG(0x10);//set column address
      		LCD_WR_REG(0x00);
      		for(col=0;col<128;col++)
      		LCD_WR_DATA8(*p++);
   	}

}


//显示字符
void LCD_ShowChar(u8 col,u8 page,u8 Order)
{
		u8 i;
		u16 x;
		x = (Order-0x20)*0x10;                   //ASCII字符从0x20开始,每个16 byte
		LCD_WR_REG(ComTable[page&0x07]|0xB0);    //Set Page Address
		LCD_WR_REG( ((col+1)>>4) | 0x10);        //Set Column Address High Byte
		LCD_WR_REG( (col+1)&0x0F );              //Low Byte Colum from S128 -> S1 auto add
		for(i=0;i<8;i++)
		{
				LCD_WR_DATA8( ASCIIchardot[x] );
				x++;
		}
		page++;                                  //下半字符page+1
		LCD_WR_REG(ComTable[page&0x07]|0xB0);    //Set Page Address
		LCD_WR_REG( ((col+1)>>4) | 0x10);        //Set Column Address High Byte
		LCD_WR_REG( (col+1)&0x0F );              //Low Byte Colum from S128 -> S1 auto add
		for(i=0;i<8;i++)
		{
				LCD_WR_DATA8( ASCIIchardot[x] );
				x++;
		}
		page--;                                  //写完一个字符page还原
}


//显示字符串
void LCD_ShowStr(u8 col,u8 page,u8 *puts)
{
		while(*puts != '\0')                     //判断字符串是否显示完毕
		{
				if(col>(LCD_W-8))                  //判断行末是否够放一个字符，否则换行
				{
						page=page+2;
						col=0;
				}
				if(page>(LCD_H/8-2))               //判断屏末是否够放一个字符，否则返回
				{
						page=0;
						col=0;
				}
				LCD_ShowChar(col+3,page,*puts);
				puts++;
				col=col+8;                           //下一个字符
		}
}

//显示三位数(0-255)
void LCD_ShowNum(u8 col,u8 page,u8 Num)
{
		u8 a,b,c;
		a=Num/100;
		b=(Num%100)/10;
		c=Num%10;
		if(a==0) ;                               //不写空格,直接跳过//PutChar(col,page,0x20);
		else LCD_ShowChar(col,page,a+0x30);
		if(a==0 && b==0) ;                       //不写空格,直接跳过//LCD_ShowChar(col,page,0x20);
		else LCD_ShowChar(col+8,page,b+0x30);
		LCD_ShowChar(col+16,page,c+0x30);
}

//显示图片
void LCD_ShowBmp( u8 const *puts )
{
		u8 i,j;
		u16 X=0;
		for(i=0;i<(LCD_H/8);i++)
		{
				LCD_WR_REG(0xB0|ComTable[i]);        //Set Page Address
				LCD_WR_REG(0x10);                    //Set Column Address = 0
				LCD_WR_REG(0x04);                    //Colum from S1 -> S128 auto add
				for(j=0;j<LCD_W;j++)
				{
						LCD_WR_DATA8( puts[X] );
						X++;
				}
		}
}


#endif






















