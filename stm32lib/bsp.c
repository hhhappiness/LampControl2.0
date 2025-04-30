#include "bsp.h"
#include "utility.h"
#include "stm32f10x_dma.h"

void InitUart(USART_TypeDef * uart,uint32_t BaudRate)
{
  USART_InitTypeDef USART_InitStructure;
  USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_Init(uart, &USART_InitStructure);
	USART_Cmd(uart, ENABLE);	
}

/******************************************************************************
Function:
    Get one byte data from serial port,
return 0: Timeout 
       1: success 
******************************************************************************/
u8 sio1_getch(u8 *pDat)
{
  u32 T;
  T=GetTimerCount();
  while(!USART1_RXNE)
	{
    if(IsTimeOut_ms(T,1)) return 0;
  }
  *pDat = (u8)(USART1->DR & 0xFF);
  return 1;
}

u8 sio2_getch(u8 *pDat)
{
  u32 T;
  T=GetTimerCount();
  while(!USART2_RXNE)
	{
    if(IsTimeOut_ms(T,1)) return 0;
  }
  *pDat = (u8)(USART2->DR & 0xFF);
  return 1;
}

u8 sio3_getch(u8 *pDat)
{
  u32 T;
  T=GetTimerCount();
  while(!USART3_RXNE)
	{
    if(IsTimeOut_ms(T,1)) return 0;
  }
  *pDat = (u8)(USART3->DR & 0xFF);;
  return 1;
}

void sio1_readdummy()
{
	if(USART3_RXNE) 
	{
		volatile u16 tmp;
    tmp = USART3->DR;
  }
}

void sio1_puts(const char *pDat,u8 len)
{
  for(;len>0;len--) 
	{
		sio1_putch(*pDat++);
  }
}

void sio2_puts(const char *pDat,u8 len)
{
	for(;len>0;len--) 
	{
		sio2_putch(*pDat++);
	}
}

void sio3_puts(const char *pDat,u8 len)
{
  for(;len>0;len--) 
	{
    sio3_putch(*pDat++);
  }
}

void sio1_putstr(const char *pDat)
{
  while(*pDat) 
	{
    sio1_putch(*pDat++);
  }
}

void sio2_putstr(const char *pDat)
{
  while(*pDat) 
	{
    sio2_putch(*pDat++);
  }
}

void sio3_putstr(const char *pDat)
{
  while(*pDat)
	{
    sio3_putch(*pDat++);
  }
}

//初始化uart1的DMA发送，未启动
//uart1 TX对应DMA1_CH4
void InitUart1DmaT(void)
{
  DMA1_Channel4->CPAR = (u32)(&(USART1->DR));

  DMA1_Channel4->CCR = DMA_DIR_PeripheralDST 
                  | DMA_PeripheralInc_Disable 
                  | DMA_MemoryInc_Enable 
                  | DMA_PeripheralDataSize_Byte 
                  | DMA_MemoryDataSize_Byte 
                  | DMA_Mode_Normal
                  | DMA_Priority_VeryHigh
                  | DMA_M2M_Disable
                  | DMA_IT_TC
                  | DISABLE;
  USART1->CR3 |= USART_DMAReq_Tx;
}

//uart1使用DMA_CH4发送数据
void sio1_write_dma(void *pDat,u16 len)
{
  sio1_write_dma_wait();
  _sio1_write_dma(pDat,len);
}

//初始化uart2的DMA发送，未启动
//uart2 TX对应DMA1_CH7
void InitUart2DmaT(void)
{
  DMA1_Channel7->CPAR = (u32)(&(USART2->DR));

  DMA1_Channel7->CCR = DMA_DIR_PeripheralDST 
                  | DMA_PeripheralInc_Disable 
                  | DMA_MemoryInc_Enable 
                  | DMA_PeripheralDataSize_Byte 
                  | DMA_MemoryDataSize_Byte 
                  | DMA_Mode_Normal
                  | DMA_Priority_VeryHigh
                  | DMA_M2M_Disable
                  | DMA_IT_TC
                  | DISABLE;
  USART2->CR3 |= USART_DMAReq_Tx;
}

//uart2使用DMA1_CH7发送数据
void sio2_write_dma(void *pDat,u16 len)
{
  sio2_write_dma_wait();
  _sio2_write_dma(pDat,len);
}

char gTmpStr[100];//LCD_COL_NUM+1];        //用于打印数字转字符串临时存放字符串。
u8 gTmpStrIndex;        //用于打印数字转字符串临时存放字符串。
