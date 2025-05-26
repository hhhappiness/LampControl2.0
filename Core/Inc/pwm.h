/**
 * @file pwm.h
 * @brief 级联定时器PWM控制函数头文件
 */

#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

/**
 * @brief 启动PWM输出
 * @param pulse_width_us: 脉宽，单位为微秒
 * @param period_us: 周期，单位为微秒
 * @return HAL_StatusTypeDef: 操作状态
 * 
 * @note 此函数配置并启动级联定时器PWM输出
 *       TIM2控制周期，TIM3控制脉宽
 */
void PWM_Start(u32 pulse_width_us, u32 period_us);

/**
 * @brief 调整PWM参数
 * @param pulse_width_us: 新的脉宽，单位为微秒
 * @param period_us: 新的周期，单位为微秒
 * @return HAL_StatusTypeDef: 操作状态
 * 
 * @note 此函数动态调整PWM的脉宽和周期
 *       如果只需调整其中一个参数，可以将另一个参数设为0保持不变
 */
void PWM_Adjust(u32 pulse_width_us, u32 period_us);

/**
 * @brief 停止PWM输出
 * @note 此函数停止TIM2和TIM3定时器
 */
void PWM_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */
