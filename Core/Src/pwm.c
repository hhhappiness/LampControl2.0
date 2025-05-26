/**
 * @file pwm_control.c
 * @brief 级联定时器PWM控制函数
 */
#include "bsp.h"
#include "pwm.h"
#include "main.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_tim.h"


/* 外部变量声明 */
extern TIM_HandleTypeDef htim2,htim3;  // 主定时器句柄
extern enum error_code my_error; // 错误代码
u8 pwm_start_flag = 0; // PWM启动标志

/**
 * @brief 启动PWM输出
 * @param pulse_width_us: 脉宽，单位为微秒
 * @param period_us: 周期，单位为微秒
 * @return HAL_StatusTypeDef: 操作状态
 * 
 * @note 此函数配置并启动级联定时器PWM输出
 *       TIM2控制周期，TIM3控制脉宽
 */
void PWM_Start(u32 pulse_width_us, u32 period_us)
{    
    /* 参数检查 */
    if (pulse_width_us > period_us || (period_us-1) > 0xFFFFFFFF)
    {
        Error_Handler();
    }
    
    /* 停止定时器（如果已经运行） */
    HAL_TIM_Base_Stop(&htim2);
    //HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    
    /* 计算TIM2周期值 */
    u32 tim2_period = period_us - 1;
    
    /* 计算TIM3脉宽值 */
    u32 tim3_pulse = pulse_width_us - 1;
    
    /* 配置TIM2周期 */
    __HAL_TIM_SET_AUTORELOAD(&htim2, tim2_period);
    
    /* 配置TIM3脉宽和周期 */
    // 固定初始周期100us，因為one pulse mode，脈寬結束後就一直為低電平了
    //__HAL_TIM_SET_AUTORELOAD(&htim3, tim2_period);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim3_pulse);
    
    /* 启动定时器 */
    
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_Base_Start(&htim2);
    pwm_start_flag = 1; // 设置PWM启动标志
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
void PWM_Adjust(u32 pulse_width_us, u32 period_us)
{
    if(pwm_start_flag == 0)  // 如果PWM未启动，则启动
    {
        PWM_Start(pulse_width_us,period_us);
        return;
    }
    u32 current_period,current_pulse;
    
    /* 参数检查 脉宽不能大于周期;检查是否超出16位定时器范围   */
    if (pulse_width_us > period_us || (period_us - 1)> 0xFFFFFFFF){
        my_error = ERROR_PWM;
        Error_Handler();
    }
    /* 获取当前参数 */
    current_period = __HAL_TIM_GET_AUTORELOAD(&htim2) + 1;
    current_pulse = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) + 1;
    /* 计算新的参数值 */
    u32 tim2_period,tim3_pulse;
    /* 如果参数为0，保持当前值不变 */
    tim2_period = (period_us == 0) ? (current_period) :  (period_us - 1);
    
    tim3_pulse = (pulse_width_us == 0) ? (current_pulse) : (pulse_width_us - 1);
    
    /* 判断是否需要重启定时器 */    
    /* 如果周期改变，需要重启定时器 */
    if (period_us != current_period)
    {
        /* 停止定时器 */
        HAL_TIM_Base_Stop(&htim2);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
        
        /* 配置新参数 */
        __HAL_TIM_SET_AUTORELOAD(&htim2, tim2_period);  //只设置tim2的周期
        /* 配置TIM3脉宽 */
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
    pwm_start_flag = 0; // 清除PWM启动标志
}