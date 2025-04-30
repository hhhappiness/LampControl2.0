
#include "stm32.h"
#include "bsp.h"
#include "board_config.h"

void SPIMasterInit(void)
{
	SPI_InitTypeDef SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;;
	SPI_Cmd(SPI1,DISABLE);     //ʧ��

	SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;//����SPI�������˫�������ģʽ:SPI����ΪTX only
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//����SPI�����ݴ�С:SPI���ͽ���16λ֡�ṹ
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High; //ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//���ݲ����ڵ�2��ʱ���أ������أ�
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ��
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;  //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	//SPI_InitStructure.SPI_CRCPolynomial=7;//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1,&SPI_InitStructure);//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
#if 0	
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);//ʹ�ܽ��ջ���ǿ��ж�����
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
#endif	
	SPI_Cmd(SPI1,ENABLE);//ʹ��SPI1����	
}


void SPIMasterReInit(uint16_t psc)
{
	SPI_InitTypeDef SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;;
	SPI_Cmd(SPI1,DISABLE);     //ʧ��

	SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;//����SPI�������˫�������ģʽ:SPI����ΪTX only
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//����SPI�����ݴ�С:SPI���ͽ���16λ֡�ṹ
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_High; //ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;//���ݲ����ڵ�2��ʱ���أ������أ�
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ��
	SPI_InitStructure.SPI_BaudRatePrescaler=psc;  //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	//SPI_InitStructure.SPI_CRCPolynomial=7;//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1,&SPI_InitStructure);//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI1,ENABLE);//ʹ��SPI1����	
}

