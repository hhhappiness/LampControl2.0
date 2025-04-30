
#include "stm32.h"
#include "bsp.h"
#include "board_config.h"

void SPIMasterInit(void)
{
	SPI_InitTypeDef SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;;
	SPI_Cmd(SPI1,DISABLE);     //失能

	SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;//设置SPI单向或者双向的数据模式:SPI设置为TX only
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//设置SPI工作模式:设置为从SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收16位帧结构
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High; //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//数据捕获于第2个时钟沿（上升沿）
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;  //定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	//SPI_InitStructure.SPI_CRCPolynomial=7;//CRC值计算的多项式
	SPI_Init(SPI1,&SPI_InitStructure);//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
#if 0	
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);//使能接收缓存非空中断屏蔽
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
#endif	
	SPI_Cmd(SPI1,ENABLE);//使能SPI1外设	
}


void SPIMasterReInit(uint16_t psc)
{
	SPI_InitTypeDef SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;;
	SPI_Cmd(SPI1,DISABLE);     //失能

	SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;//设置SPI单向或者双向的数据模式:SPI设置为TX only
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//设置SPI工作模式:设置为从SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收16位帧结构
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High; //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//数据捕获于第2个时钟沿（上升沿）
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）
	SPI_InitStructure.SPI_BaudRatePrescaler=psc;  //定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	//SPI_InitStructure.SPI_CRCPolynomial=7;//CRC值计算的多项式
	SPI_Init(SPI1,&SPI_InitStructure);//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI1,ENABLE);//使能SPI1外设	
}

