#ifndef _STM32_BSP_H_
#define _STM32_BSP_H_

#include "stm32.h"
#include "board_config.h"

#ifndef false
    #define false 0
#endif

#ifndef true
    #define true 1
#endif

#ifdef __cplusplus
 extern "C" {
#endif 
   
void InitUart(USART_TypeDef * uart,uint32_t BaudRate);

void GPIO_Configuration(void);//��Ӳ����أ���bsp��Ӧ����ʵ��
void NVIC_Configuration(void);//��Ӧ����ʵ��
void RCC_Configuration(void);//��Ӧ����ʵ��

//Pin assignment 
//assignments for lcd are moved corsponding header file

void InitTIM4(void);
void InitTIM2(void);

int GetKey(void);
void ClearKey(void);
extern s16 Encode;
extern s16 EncodeLast;
void InitEncoder(void);
void TestKey(void);
int GetKnob2(void);
extern u32 TrigDelay;
extern u8 LockTrigDelay;


extern u16 KeyRepeatNumRead;  //�Ѷ��ظ�����

int GetKeyRepeat(void);
void InitKey(void);

#define KEY_NULL            0
#define KEY_1               1
#define KEY_2               2
#define KEY_3               3
#define KEY_4               4
#define KEY_5               5
#define KEY_6               6
#define KEY_7               7
#define KEY_8               8
#define KEY_9               9
#define KEY_10              10
#define KEY_11              11
#define KEY_12              12
#define KEY_13              13
#define KEY_14              14
#define KEY_15              15
#define KEY_RELEASE         0x20

#define KEY_REPEATE         0x40

//���̼��벻�ᳬ��16����ť�ʹ�17��ʼ��
#define KEY_RB              17  //��ť
#define KEY_RA              18  //��ť

#define KEY_RB2             19 //��������ť
#define KEY_RA2             20 //��������ť


#define IsKeyMoveFocus()


// __INLINE void sio1_putch(u8 dat)  {while(!USART1_TC);USART1->DR=dat;}

// __INLINE void sio2_putch(u8 dat)  {while(!USART2_TC);USART2->DR=dat;}

// __INLINE void sio3_putch(u8 dat) {while(!USART3_TC);USART3->DR=dat;}

void sio1_puts(const char *pDat,u8 len);
void sio1_putstr(const char *pDat);

void sio2_puts(const char *pDat,u8 len);
void sio2_putstr(const char *pDat);

void sio3_puts(const char *pDat,u8 len);
void sio3_putstr(const char *pDat);

void sio1_readdummy(void);
u8 sio1_getch(u8 *pDat);
u8 sio2_getch(u8 *pDat);
u8 sio3_getch(u8 *pDat);
u8 sio1_getch_cancel(u8 *pDat);
u8 sio2_getch_cancel(u8 *pDat);
u8 sio3_getch_cancel(u8 *pDat);
void SetUartTimeOut(u16 ms);
u8 sio1_getch_timeout(u8 *pDat);
u8 sio2_getch_timeout(u8 *pDat);
u8 sio3_getch_timeout(u8 *pDat);
void InitUart1DmaT(void);
void InitUart2DmaT(void);
///DMA���ͣ������֮ǰ��DMA�Ƿ����
__inline void _sio1_write_dma(void *pDat,u16 len){
    DMA1_Channel4->CMAR = (u32)(pDat);  //�趨���ݵ�ַ
    DMA1_Channel4->CNDTR = len;         //�趨���ݳ���
    DMA1_Channel4->CCR |= ENABLE;       //ʹ��
}
///�ȴ�֮ǰ��DMA�Ƿ����
__inline void sio1_write_dma_wait(void){
    if(DMA1_Channel4->CCR & ENABLE) {//����Ѿ�����DMA
        while(!(DMA1->ISR & DMA_ISR_TCIF4));//�ȷ������
        DMA1->IFCR |= DMA_IFCR_CTCIF4;  //��״̬λ
        DMA1_Channel4->CCR &= ~ENABLE;//��DMA
    }
}
///DMA���ͣ������֮ǰ��DMA�Ƿ����
// __inline void _sio2_write_dma(void *pDat,u16 len){
//     DMA1_Channel7->CMAR = (u32)(pDat);  //�趨���ݵ�ַ
//     DMA1_Channel7->CNDTR = len;         //�趨���ݳ���
//     DMA1_Channel7->CCR |= ENABLE;       //ʹ��
// }
// ///�ȴ�֮ǰ��DMA�Ƿ����
// __inline void sio2_write_dma_wait(void)
// {
//     if(DMA1_Channel7->CCR & ENABLE) {//����Ѿ�����DMA
//         while(!(DMA1->ISR & DMA_ISR_TCIF7));//�ȷ������
//         DMA1->IFCR |= DMA_IFCR_CTCIF7;  //��״̬λ
//         DMA1_Channel7->CCR &= ~ENABLE;//��DMA
//     }	
// }
void sio1_write_dma(void *pDat,u16 len);
void sio2_write_dma(void *pDat,u16 len);


extern char gTmpStr[100];//LCD_COL_NUM+1];        //���ڴ�ӡ����ת�ַ�����ʱ����ַ�����
extern u8 gTmpStrIndex;
void printStrNum(char * str,u16 num,u8 num_len,	u8 row,u8 width);

#ifdef __cplusplus
 }
#endif 
#endif//_STM32_BSP_H_

