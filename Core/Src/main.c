/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

ADC_HandleTypeDef hadc2;
RTC_HandleTypeDef hrtc;

WWDG_HandleTypeDef hwwdg;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC2_Init(void);
static void MX_WWDG_Init(void);
static void MX_RTC_Init(void);


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

/**
 * HAL_Init()
 * 配置闪存缓存
 * 设置中断优先级分组：NVIC_PriorityGroup_4
 * 配置Systick定时器
 * 底层硬件初始化
**/
  HAL_Init();    
  SystemClock_Config();        //RCC配置振荡器和时钟

  MX_GPIO_Init();             //GPIO初始化
  MX_ADC2_Init();             //ADC初始化
  MX_WWDG_Init();
  MX_RTC_Init();

  while (1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 28;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_DIFFERENTIAL_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
 static void MX_RTC_Init(void)
 {
 
   /* USER CODE BEGIN RTC_Init 0 */
 
   /* USER CODE END RTC_Init 0 */
 
   /* USER CODE BEGIN RTC_Init 1 */
 
   /* USER CODE END RTC_Init 1 */
 
   /** Initialize RTC Only
   */
   hrtc.Instance = RTC;
   hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
   hrtc.Init.AsynchPrediv = 127;
   hrtc.Init.SynchPrediv = 255;
   hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
   hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
   hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
   hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
   hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
   if (HAL_RTC_Init(&hrtc) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE BEGIN RTC_Init 2 */
 
   /* USER CODE END RTC_Init 2 */
 
 }
 
/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
 static void MX_WWDG_Init(void)
 {
 
   /* USER CODE BEGIN WWDG_Init 0 */
 
   /* USER CODE END WWDG_Init 0 */
 
   /* USER CODE BEGIN WWDG_Init 1 */
 
   /* USER CODE END WWDG_Init 1 */
   hwwdg.Instance = WWDG;
   hwwdg.Init.Prescaler = WWDG_PRESCALER_1;
   hwwdg.Init.Window = 64;
   hwwdg.Init.Counter = 64;
   hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
   if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE BEGIN WWDG_Init 2 */
 
   /* USER CODE END WWDG_Init 2 */
 
 }

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, POWER_ALL_ON_Pin|SNSR_GPIO_Pin|BKLT_ON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DRIVER_ON_N_Pin|LCD_CSB_Pin|LDC_A0_Pin|LCD_RSTB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SNSR_ADC_Pin PA5_DAC_Pin */
  GPIO_InitStruct.Pin = SNSR_ADC_Pin|PA5_DAC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SNSR_PWR_Pin */
  GPIO_InitStruct.Pin = SNSR_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(SNSR_PWR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : POWER_ALL_ON_Pin SNSR_GPIO_Pin BKLT_ON_Pin */
  GPIO_InitStruct.Pin = POWER_ALL_ON_Pin|SNSR_GPIO_Pin|BKLT_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DRIVER_STROBE_Pin */
  GPIO_InitStruct.Pin = DRIVER_STROBE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(DRIVER_STROBE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_POWER_Pin KEY_RIGHT_Pin KEY_DOWN_Pin KEY_ENTER_Pin
                           KEY_LEFT_Pin KEY_UP_Pin */
  GPIO_InitStruct.Pin = KEY_POWER_Pin|KEY_RIGHT_Pin|KEY_DOWN_Pin|KEY_ENTER_Pin
                          |KEY_LEFT_Pin|KEY_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : CHG_STS_Pin */
  GPIO_InitStruct.Pin = CHG_STS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(CHG_STS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DRIVER_ON_N_Pin LCD_CSB_Pin LDC_A0_Pin LCD_RSTB_Pin */
  GPIO_InitStruct.Pin = DRIVER_ON_N_Pin|LCD_CSB_Pin|LDC_A0_Pin|LCD_RSTB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_SDA_Pin LCD_SCL_Pin */
  GPIO_InitStruct.Pin = LCD_SDA_Pin|LCD_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_CHA_Pin KEY_CHB_Pin */
  GPIO_InitStruct.Pin = KEY_CHA_Pin|KEY_CHB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
