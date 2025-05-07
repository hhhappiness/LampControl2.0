/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32g4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  HAL_PWREx_DisableUCPDDeadBattery();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief ADC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hadc->Instance==ADC2)
  {
    /* USER CODE BEGIN ADC2_MspInit 0 */

    /* USER CODE END ADC2_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA6     ------> ADC2_IN3
    PA7     ------> ADC2_IN4
    */
    GPIO_InitStruct.Pin = BAT_NTC_Pin|VBAT_ACQ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN ADC2_MspInit 1 */

    /* USER CODE END ADC2_MspInit 1 */

  }

}

/**
  * @brief ADC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC2)
  {
    /* USER CODE BEGIN ADC2_MspDeInit 0 */

    /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA6     ------> ADC2_IN3
    PA7     ------> ADC2_IN4
    */
    HAL_GPIO_DeInit(GPIOA, BAT_NTC_Pin|VBAT_ACQ_Pin);

    /* USER CODE BEGIN ADC2_MspDeInit 1 */

    /* USER CODE END ADC2_MspDeInit 1 */
  }

}

/**
  * @brief RTC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer
  * @retval None
  */
 void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
 {
   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
   if(hrtc->Instance==RTC)
   {
     /* USER CODE BEGIN RTC_MspInit 0 */
 
     /* USER CODE END RTC_MspInit 0 */
 
   /** Initializes the peripherals clocks
   */
     PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
     PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
 
     if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
     {
       Error_Handler();
     }
 
     /* Peripheral clock enable */
     __HAL_RCC_RTC_ENABLE();
     __HAL_RCC_RTCAPB_CLK_ENABLE();
     /* USER CODE BEGIN RTC_MspInit 1 */
 
     /* USER CODE END RTC_MspInit 1 */
 
   }
 
 }
 
 /**
   * @brief RTC MSP De-Initialization
   * This function freeze the hardware resources used in this example
   * @param hrtc: RTC handle pointer
   * @retval None
   */
 void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
 {
   if(hrtc->Instance==RTC)
   {
     /* USER CODE BEGIN RTC_MspDeInit 0 */
 
     /* USER CODE END RTC_MspDeInit 0 */
     /* Peripheral clock disable */
     __HAL_RCC_RTC_DISABLE();
     __HAL_RCC_RTCAPB_CLK_DISABLE();
     /* USER CODE BEGIN RTC_MspDeInit 1 */
 
     /* USER CODE END RTC_MspDeInit 1 */
   }
 
 }
 /**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI2)
  {
    /* USER CODE BEGIN SPI2_MspInit 0 */

    /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB12     ------> SPI2_NSS
    PB13     ------> SPI2_SCK
    PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = LCD_CSB_Pin|LCD_SCL_Pin|LCD_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN SPI2_MspInit 1 */

    /* USER CODE END SPI2_MspInit 1 */

  }

}

/**
  * @brief SPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI2)
  {
    /* USER CODE BEGIN SPI2_MspDeInit 0 */

    /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /**SPI2 GPIO Configuration
    PB12     ------> SPI2_NSS
    PB13     ------> SPI2_SCK
    PB15     ------> SPI2_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, LCD_CSB_Pin|LCD_SCL_Pin|LCD_SDA_Pin);

    /* USER CODE BEGIN SPI2_MspDeInit 1 */

    /* USER CODE END SPI2_MspDeInit 1 */
  }

}
/**
  * @brief WWDG MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hwwdg: WWDG handle pointer
  * @retval None
  */
 void HAL_WWDG_MspInit(WWDG_HandleTypeDef* hwwdg)
 {
   if(hwwdg->Instance==WWDG)
   {
     /* USER CODE BEGIN WWDG_MspInit 0 */
 
     /* USER CODE END WWDG_MspInit 0 */
     /* Peripheral clock enable */
     __HAL_RCC_WWDG_CLK_ENABLE();
     /* USER CODE BEGIN WWDG_MspInit 1 */
 
     /* USER CODE END WWDG_MspInit 1 */
 
   }
 
 }
 
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
