/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define TEST1
#define DEBUG
/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "bsp.h"

extern TIM_HandleTypeDef htim3,htim4;
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SNSR_ADC_Pin GPIO_PIN_0
#define SNSR_ADC_GPIO_Port GPIOA
#define SNSR_PWR_Pin GPIO_PIN_1
#define SNSR_PWR_GPIO_Port GPIOA
#define POWER_ALL_ON_Pin GPIO_PIN_2
#define POWER_ALL_ON_GPIO_Port GPIOA
#define SNSR_GPIO_Pin GPIO_PIN_3
#define SNSR_GPIO_GPIO_Port GPIOA
#define DRIVER_STROBE_Pin GPIO_PIN_4
#define DRIVER_STROBE_GPIO_Port GPIOA
#define PA5_DAC_Pin GPIO_PIN_5
#define PA5_DAC_GPIO_Port GPIOA
#define BAT_NTC_Pin GPIO_PIN_6
#define BAT_NTC_GPIO_Port GPIOA
#define VBAT_ACQ_Pin GPIO_PIN_7
#define VBAT_ACQ_GPIO_Port GPIOA
#define KEY_POWER_Pin GPIO_PIN_0
#define KEY_POWER_GPIO_Port GPIOB
#define CHG_STS_Pin GPIO_PIN_1
#define CHG_STS_GPIO_Port GPIOB
#define DRIVER_ON_N_Pin GPIO_PIN_10
#define DRIVER_ON_N_GPIO_Port GPIOB
#define LCD_CSB_Pin GPIO_PIN_12
#define LCD_CSB_GPIO_Port GPIOB
#define LCD_SCL_Pin GPIO_PIN_13
#define LCD_SCL_GPIO_Port GPIOB
#define LDC_A0_Pin GPIO_PIN_14
#define LDC_A0_GPIO_Port GPIOB
#define LCD_SDA_Pin GPIO_PIN_15
#define LCD_SDA_GPIO_Port GPIOB
#define LCD_RSTB_Pin GPIO_PIN_8
#define LCD_RSTB_GPIO_Port GPIOA
#define KEY_CHA_Pin GPIO_PIN_11
#define KEY_CHA_GPIO_Port GPIOA
#define KEY_CHB_Pin GPIO_PIN_12
#define KEY_CHB_GPIO_Port GPIOA
#define BKLT_ON_Pin GPIO_PIN_15
#define BKLT_ON_GPIO_Port GPIOA
#define KEY_RIGHT_Pin GPIO_PIN_3
#define KEY_RIGHT_GPIO_Port GPIOB
#define KEY_DOWN_Pin GPIO_PIN_4
#define KEY_DOWN_GPIO_Port GPIOB
#define KEY_ENTER_Pin GPIO_PIN_5
#define KEY_ENTER_GPIO_Port GPIOB
#define KEY_LEFT_Pin GPIO_PIN_6
#define KEY_LEFT_GPIO_Port GPIOB
#define KEY_UP_Pin GPIO_PIN_7
#define KEY_UP_GPIO_Port GPIOB

#define LCD_CSB_Pin_O(x)		((x==0) ? (GPIOB->ODR & ~LCD_CSB_Pin) : (GPIOB->ODR | LCD_CSB_Pin))
#define LCD_SCL_Pin_O(x)		((x==0) ? (GPIOB->ODR & ~LCD_SCL_Pin) : (GPIOB->ODR | LCD_SCL_Pin))
#define LCD_SDA_Pin_O(x)		((x==0) ? (GPIOB->ODR & ~LCD_SDA_Pin) : (GPIOB->ODR | LCD_SDA_Pin))
#define LCD_RSTB_Pin_O(x)		((x==0) ? (GPIOA->ODR & ~LCD_RSTB_Pin) : (GPIOA->ODR | LCD_RSTB_Pin))
#define LCD_A0_Pin_O(x)		((x==0) ? (GPIOB->ODR & ~LDC_A0_Pin) : (GPIOB->ODR | LDC_A0_Pin))

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
