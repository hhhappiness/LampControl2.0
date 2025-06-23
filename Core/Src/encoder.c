/* 包含必要的头文件 */
#include "main.h"
#include "stm32g4xx_hal_tim.h"
#include "encoder.h"
#include "stdio.h"
#include "bsp.h"

#define EncoderOutBuf encoderState.encoder_diff.diff[3] //编码器缓冲区输出
#define MaxEncoderDiff 0x36 //编码器差值最大值


// 优化后的整数幂函数，防止b为负数且更安全
static inline int pow_int(int base, int exp) {
  int result = 1;
  if (exp < 0) return 0; // 不支持负指数
  while (exp--) result *= base;
  return result;
}
EncoderState_t encoderState = {0};
u8 encoder_buff_num = 0; // 缓冲区数据个数
uint32_t counterMax;

float currentHz = 100,exponent_result = 0;
u32 currentPeriod_us;
u16 key = 0;
u8 flag=1;
/* 编码器初始化函数 */
void Encoder_Init(void)
{
  /* 启动编码器模式 */
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  /* 设置初始计数值 */
  if (IS_TIM_32B_COUNTER_INSTANCE(htim4.Instance)) {  // 32位定时器
    __HAL_TIM_SET_COUNTER(&htim4, 0x7FFFFFFF);
    counterMax = 0xFFFFFFFF;
  } else {                                            // 16位定时器
    __HAL_TIM_SET_COUNTER(&htim4, 0x7FFF);
    counterMax = 0xFFFF;
  }
}
u8 encoderUpdate_Enable = 1;

/* 更新编码器状态 */
void Encoder_Update(void)
{
  if(!encoderUpdate_Enable) return; // 如果编码器更新被禁用，直接返回
  static s32 lastCounter=0x7fff; //counter的初始值
  static u32 lastDiff=0;  //初始化lastcounter值
  s8 diff=0;
  /* 获取当前计数值 */
  encoderState.counter = __HAL_TIM_GET_COUNTER(&htim4);
  if(encoderState.counter == lastCounter) {
    
    return; //如果计数值没有变化，直接返回
  }
  /* 计算变化量（包含方向） */
  diff = encoderState.counter - lastCounter;
  /* 更新总步数 */
  encoderState.totalSteps += diff ;  // 0x7FFF是编码器的初始值

  /* 更新上次计数值 */
  lastCounter = encoderState.counter;
  InEncoderBuf(diff); //存入缓冲区
}

int GetEncoder()
{
  encoderUpdate_Enable = 1; // 是否禁止编码器更新
  /* 返回当前差值 */
  int ret = 0;
  s8 difference,direction;
  u8 abs_diff,exp;
  u32 factor;
  u32 table[9] = {1,2,4,8,16,32,64,128,256}; // 2的幂表
  
  if(encoder_buff_num>0){
    difference = EncoderOutBuf;
    abs_diff = difference>0?difference:-difference;
    direction = difference > 0 ? 1 : -1; // 判断方向
    encoderState.encoder_diff.diff_buffer <<= 8; // 左移8位
    encoder_buff_num--; // 减少缓冲区数据个数

    if(abs_diff<=5){
      ret = (int)difference * pow_int(2,0);  //小数点后两位开始加
    // }else{
    //   exp = ((abs_diff - 5)* 8+ MaxEncoderDiff/2)/MaxEncoderDiff;
    //   factor = table[exp];
    //   ret = (int)(direction  *factor * 10);  //0.1~25.6
    }else if(abs_diff <=10){
      ret =  (int)direction*(abs_diff-5)*50 ;  //小数点后一位开始加
    }else{
        ret =  (int)direction*(abs_diff-10) *1000;  
    }
  }else{
    return 0; //如果没有数据，直接返回0
  }

    return ret;

  

}

void InEncoderBuf(s8 x){        //最早数据在diff[3]，最新数据在diff[0]
    switch (encoder_buff_num) {
    case 1:
      encoderState.encoder_diff.diff[2]=x;
      encoder_buff_num=2;
      break;
    case 2:
      encoderState.encoder_diff.diff[1]=x;
      encoder_buff_num=3;
      break;
    case 4:
      encoderState.encoder_diff.diff[0]=x;
      encoder_buff_num=4;
    break;
    case 3: 
      encoderState.encoder_diff.diff_buffer<<= 8; //按键移位进入缓冲区, 如果有键未读会移出  ori = " >> "
		  encoderState.encoder_diff.diff[0]=x;
    
      break;
    default:
      encoderState.encoder_diff.diff[3]=x; 
      encoder_buff_num=1;
      break;
    }
}
#if 0
/* 阈值滤波 - 忽略短时间内的小变化 */
void Encoder_Update_WithThreshold(void)
{
  static int32_t accumulatedDiff = 0;
  const int32_t THRESHOLD = 4;  // 设置阈值
  
  // 获取当前计数值
  int32_t currentCount = __HAL_TIM_GET_COUNTER(&htim4);
  
  // 计算差值
  int32_t diff = currentCount - encoderState.lastCounter;
  
  // 处理溢出
  if (diff > 32767) diff -= 65536;
  else if (diff < -32767) diff += 65536;
  
  // 累积差值
  accumulatedDiff += diff;
  
  // 只有当累积差值超过阈值时才更新
  if (abs(accumulatedDiff) >= THRESHOLD)
  {
    // 更新总步数，方向等
    encoderState.totalSteps += accumulatedDiff;
    encoderState.direction = (accumulatedDiff > 0) ? 1 : -1;
    encoderState.difference = accumulatedDiff;
    
    // 重置累积差值
    accumulatedDiff = 0;
  }
  else
  {
    // 差值小于阈值，视为无变化
    encoderState.difference = 0;
    encoderState.direction = 0;
  }
  
  // 更新上次计数值
  encoderState.lastCounter = currentCount;
}

/* 时间窗口滤波 - 在固定时间窗口内累积变化 */
void Encoder_Update_TimeWindow(void)
{
  static uint32_t lastUpdateTime = 0;
  static int32_t windowDiff = 0;
  const uint32_t UPDATE_INTERVAL = 50;  // 50ms更新窗口
  
  // 获取当前计数值
  int32_t currentCount = __HAL_TIM_GET_COUNTER(&htim4);
  
  // 计算差值
  int32_t diff = currentCount - encoderState.lastCounter;
  
  // 处理溢出
  if (diff > 32767) diff -= 65536;
  else if (diff < -32767) diff += 65536;
  
  // 累积窗口内的差值
  windowDiff += diff;
  
  // 更新上次计数值
  encoderState.lastCounter = currentCount;
  
  // 检查是否达到更新时间
  uint32_t currentTime = HAL_GetTick();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL)
  {
    // 更新编码器状态
    encoderState.difference = windowDiff;
    encoderState.totalSteps += windowDiff;
    
    if (windowDiff > 0)
      encoderState.direction = 1;
    else if (windowDiff < 0)
      encoderState.direction = -1;
    else
      encoderState.direction = 0;
    
    // 重置窗口差值和时间
    windowDiff = 0;
    lastUpdateTime = currentTime;
  }
  else
  {
    // 未到更新时间，不报告变化
    encoderState.difference = 0;
    encoderState.direction = 0;
  }
}

/* 中值滤波 - 取多次读数的中值 */
void Encoder_Update_Median(void)
{
  #define FILTER_SIZE 5
  static int32_t readings[FILTER_SIZE];
  static uint8_t readIndex = 0;
  
  // 获取当前计数值
  int32_t currentCount = __HAL_TIM_GET_COUNTER(&htim4);
  
  // 存储当前读数
  readings[readIndex] = currentCount;
  readIndex = (readIndex + 1) % FILTER_SIZE;
  
  // 复制数组用于排序
  int32_t sortedReadings[FILTER_SIZE];
  for (int i = 0; i < FILTER_SIZE; i++)
    sortedReadings[i] = readings[i];
  
  // 简单的冒泡排序
  for (int i = 0; i < FILTER_SIZE-1; i++) {
    for (int j = 0; j < FILTER_SIZE-i-1; j++) {
      if (sortedReadings[j] > sortedReadings[j+1]) {
        int32_t temp = sortedReadings[j];
        sortedReadings[j] = sortedReadings[j+1];
        sortedReadings[j+1] = temp;
      }
    }
  }
  
  // 取中值
  int32_t medianValue = sortedReadings[FILTER_SIZE/2];
  
  // 计算差值
  int32_t diff = medianValue - encoderState.lastCounter;
  
  // 处理溢出
  if (diff > 32767) diff -= 65536;
  else if (diff < -32767) diff += 65536;
  
  // 更新编码器状态
  encoderState.difference = diff;
  encoderState.totalSteps += diff;
  
  if (diff > 0)
    encoderState.direction = 1;
  else if (diff < 0)
    encoderState.direction = -1;
  else
    encoderState.direction = 0;
  
  // 更新上次计数值
  encoderState.lastCounter = medianValue;
}
#endif