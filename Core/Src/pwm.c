/**
 * @file pwm_control.c
 * @brief 级联定时器PWM控制函数
 */

#include "pwm.h"
#include "main.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"
/* 外部变量声明 */
extern TIM_HandleTypeDef htim2;  // 主定时器句柄
extern TIM_HandleTypeDef htim3;  // 从定时器句柄


/**
 * @brief 启动PWM输出
 * @param pulse_width_us: 脉宽，单位为微秒
 * @param period_us: 周期，单位为微秒
 * @return HAL_StatusTypeDef: 操作状态
 * 
 * @note 此函数配置并启动级联定时器PWM输出
 *       TIM2控制周期，TIM3控制脉宽
 */
void PWM_Start(uint32_t pulse_width_us, uint32_t period_us)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t timer_clock = 1000000; // 定时器时钟频率1MHz (42MHz/42)
    
    /* 参数检查 */
    if (pulse_width_us > period_us || period_us == 0)
    {
        Error_Handler();
    }
    
    /* 停止定时器（如果已经运行） */
    HAL_TIM_Base_Stop(&htim2);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    
    /* 计算TIM2周期值 */
    uint32_t tim2_period = period_us - 1;
    if (tim2_period > 0xFFFF) // 检查是否超出16位定时器范围
    {
        Error_Handler();
    }
    
    /* 计算TIM3脉宽值 */
    uint32_t tim3_pulse = pulse_width_us - 1;
    if (tim3_pulse > tim2_period)
    {
        tim3_pulse = tim2_period;
    }
    
    /* 配置TIM2周期 */
    __HAL_TIM_SET_AUTORELOAD(&htim2, tim2_period);
    
    /* 配置TIM3脉宽和周期 */
    __HAL_TIM_SET_AUTORELOAD(&htim3, tim2_period); // 与TIM2相同的周期
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim3_pulse);
    
    /* 启动定时器 */
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

/**
 * @brief 调整PWM参数
 * @param pulse_width_us: 新的脉宽，单位为微秒
 * @param period_us: 新的周期，单位为微秒
 * @return HAL_StatusTypeDef: 操作状态
 * 
 * @note 此函数动态调整PWM的脉宽和周期
 *       如果只需调整其中一个参数，可以将另一个参数设为0保持不变
 */
void PWM_Adjust(uint32_t pulse_width_us, uint32_t period_us)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t current_period,current_pulse;
    /* 参数检查 */
    if (pulse_width_us > period_us)
    {
        Error_Handler();
    }
    /* 获取当前参数 */
    current_period = __HAL_TIM_GET_AUTORELOAD(&htim2) + 1;
    current_pulse = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) + 1;
    /* 计算新的参数值 */
    uint32_t tim2_period,tim3_pulse;
    /* 如果参数为0，保持当前值不变 */
    tim2_period = (period_us == 0) ? (current_period) :  (period_us - 1);
    
    tim3_pulse = (pulse_width_us == 0) ? (current_pulse) : (pulse_width_us - 1);

    if (tim2_period > 0xFFFF) // 检查是否超出16位定时器范围
    {
        Error_Handler();
    }
    
    /* 判断是否需要重启定时器 */    
    /* 如果周期改变，需要重启定时器 */
    if (period_us != current_period)
    {
        /* 停止定时器 */
        HAL_TIM_Base_Stop(&htim2);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
        
        /* 配置新参数 */
        __HAL_TIM_SET_AUTORELOAD(&htim2, tim2_period);
        __HAL_TIM_SET_AUTORELOAD(&htim3, tim2_period);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim3_pulse);
        
        /* 重启定时器 */
        HAL_TIM_Base_Start(&htim2);
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    }
    else
    {
        /* 只更新脉宽，不需要重启 */
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim3_pulse);
    }

}

void PWM_Stop(void)
{
    /* 停止定时器 */
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    HAL_TIM_Base_Stop(&htim2);
}