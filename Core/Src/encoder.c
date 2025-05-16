/* 包含必要的头文件 */
#include "main.h"
#include "stm32g4xx_hal_tim.h"
#include "encoder.h"


EncoderState_t encoderState = {0};

/* 编码器初始化函数 */
void Encoder_Init(void)
{
  /* 启动编码器模式 */
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  
  /* 初始化状态变量 */
  encoderState.counter = 0;
  encoderState.lastCounter = 0;
  encoderState.totalSteps = 0;
  encoderState.difference = 0;
  encoderState.direction = 0;
  
  /* 设置初始计数值 */
  if (IS_TIM_32B_COUNTER_INSTANCE(htim4.Instance)) {  // 32位定时器
    __HAL_TIM_SET_COUNTER(&htim4, 0x7FFFFFFF);
    encoderState.totalSteps = -(s32)0x7FFFFFFF;  // 设置初始值以抵消diffrence初始值
  } else {                                            // 16位定时器
    __HAL_TIM_SET_COUNTER(&htim4, 0x7FFF);
    encoderState.totalSteps = -(s32)0x7FFF;  
  }
}

/* 更新编码器状态 */
void Encoder_Update(void)
{
  uint32_t counterMax;
  
  /* 确定计数器最大值 */
  if (IS_TIM_32B_COUNTER_INSTANCE(htim4.Instance)) {  // 32位定时器
    counterMax = 0xFFFFFFFF;
  } else {                                            // 16位定时器
    counterMax = 0xFFFF;
  }
  
  /* 获取当前计数值 */
  encoderState.counter = __HAL_TIM_GET_COUNTER(&htim4);
  
  /* 计算变化量（处理溢出） */
  encoderState.difference = encoderState.counter - encoderState.lastCounter;
  
  /* 处理溢出情况 */
  if (encoderState.difference > (int32_t)(counterMax / 2)) {
    encoderState.difference -= (counterMax + 1);
  } else if (encoderState.difference < -(int32_t)(counterMax / 2)) {
    encoderState.difference += (counterMax + 1);
  }
  
  /* 更新总步数 */
  encoderState.totalSteps += encoderState.difference ;  // 0x7FFF是编码器的初始值
  
  /* 确定方向 */
  if (encoderState.difference > 0)
    encoderState.direction = 1;
  else if (encoderState.difference < 0)
    encoderState.direction = -1;
  else
    encoderState.direction = 0;
  
  /* 更新上次计数值 */
  encoderState.lastCounter = encoderState.counter;
}


/* 获取编码器总步数 */
int32_t Encoder_GetTotalSteps(void)
{
  return encoderState.totalSteps;
}

/* 获取编码器变化量 */
int32_t Encoder_GetDifference(void)
{
  return encoderState.difference;
}

/* 获取编码器方向 */
int8_t Encoder_GetDirection(void)
{
  return encoderState.direction;
}

/* 重置编码器总步数 */
void Encoder_ResetTotalSteps(void)
{
  encoderState.totalSteps = 0;
}

/* 主函数 */
int encode_main(void)
{
  /* MCU初始化代码 */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();  // 假设使用UART输出调试信息
  
  /* 初始化编码器 */
  Encoder_Init();
  
  /* 主循环 */
  while (1)
  {
    /* 更新编码器状态 */
    Encoder_Update();
    
    /* 打印编码器状态（调试用） */
    printf("Counter: %ld, Total Steps: %ld, Difference: %ld, Direction: %d\n",
           encoderState.counter,
           encoderState.totalSteps,
           encoderState.difference,
           encoderState.direction);
    
    HAL_Delay(100);  // 延时100ms
  }
}

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
