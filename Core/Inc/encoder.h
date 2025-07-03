#ifndef __ENCODER_H_
#define __ENCODER_H_
#ifdef __cplusplus
 extern "C" {
#endif 

#include "type.h"
/* 定义编码器状态结构体 */

typedef struct {
  s32 counter;       // 当前计数值
  s32 totalSteps;    // 总步数（处理溢出）
  union   //存放历史差值
  {
    s8 diff[4];
    s32 diff_buffer;
  }encoder_diff;
  u8 delta_diff;
} EncoderState_t;

 

/* 函数声明 */
void Encoder_Init(void);
void Encoder_Update(void);
int GetEncoder(void);
void InEncoderBuf(s8 x);
void Encoder_Update_WithThreshold(void);
void Encoder_Update_TimeWindow(void);
void Encoder_Update_Median(void);
// void TIM4_IRQHandler(void);


//extern EncoderState_t encoderState; // 声明编码器状态变量
extern u8 encoder_buff_num; // 缓冲区数据个数

#ifdef __cplusplus
}
#endif
#endif /* __ENCODER_H_ */