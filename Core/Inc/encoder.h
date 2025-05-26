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
  u32 difference;    // 变化量
  u32 delta_diff; // 变化量的变化速率
  s8 direction;      // 旋转方向
} EncoderState_t;

/* 函数声明 */
void Encoder_Init(void);
void Encoder_Update(void);
void encoder_test_pwmAdjust();
s32 Encoder_GetTotalSteps(void);
s32 Encoder_GetDifference(void);
s8 Encoder_GetDirection(void);
void Encoder_ResetTotalSteps(void);
int encode_main(void);
void Encoder_Update_WithThreshold(void);
void Encoder_Update_TimeWindow(void);
void Encoder_Update_Median(void);

//extern EncoderState_t encoderState; // 声明编码器状态变量

#endif /* __ENCODER_H_ */