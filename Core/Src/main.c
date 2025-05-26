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
#include "type.h"
#include "ctrl.h"
#include "stm32.h"
#include "Lcd.h"
#include "AppParaCommon.h"
#include "rtc_int.h"
#include "SysParaCommon.h"
#include "SysPara.h"
#include "Encoder.h"
//include for test
#include <stdio.h>
#include "key.h"
#include "stm32g4xx_hal_tim.h"
// 创建一个 error_code 类型的变量
enum error_code my_error;

ADC_HandleTypeDef hadc2;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi2;
WWDG_HandleTypeDef hwwdg;
TIM_HandleTypeDef htim2,htim3,htim4;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC2_Init(void);
static void MX_WWDG_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);

void SysInit(void);
void Init(void);
HAL_StatusTypeDef PWM_ExternalTrigger_Init(uint32_t pulse_width_us, uint32_t max_period_us)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  /* 参数检查 */
  if (pulse_width_us > max_period_us || max_period_us == 0) {
      return HAL_ERROR;
  }
  
  /* 计算定时器参数 */
  uint32_t tim_period = max_period_us - 1;
  uint32_t tim_pulse = pulse_width_us - 1;
  
  /* 启用时钟 */
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();  // TIM3 CH2输出引脚
  __HAL_RCC_GPIOB_CLK_ENABLE();  // PB3触发输入引脚
  
  /* 配置PA7为TIM3 CH2输出引脚 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* 配置PB3为TIM3外部触发输入引脚 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;         // 复用推挽
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_TIM3;      // TIM3的复用功能
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* TIM3初始化配置 */
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 41;                      // 预分频器：42MHz/(41+1)=1MHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;    // 向上计数模式
  htim3.Init.Period = tim_period;                 // 最大周期
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;  // 启用自动重载预加载
  
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
      return HAL_ERROR;
  }
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
      return HAL_ERROR;
  }
  
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
      return HAL_ERROR;
  }
  
  /* 配置TIM3为从模式，使用外部触发输入 */
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;   // 重置模式：触发信号重置计数器
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;      // 触发源：TI2过滤后的信号
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;  // 上升沿触发
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter = 0x0;               // 无滤波
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK) {
      return HAL_ERROR;
  }
  
  /* 配置TIM3 CH2为PWM输出 */
  sConfigOC.OCMode = TIM_OCMODE_PWM1;             // PWM模式1
  sConfigOC.Pulse = tim_pulse;                    // 脉宽
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;     // 高电平有效
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
      return HAL_ERROR;
  }
  
  HAL_TIM_MspPostInit(&htim3); // 配置GPIO引脚和中断
  /* 配置One Pulse Mode */
  htim3.Instance->CR1 |= TIM_CR1_OPM;             // 启用One Pulse Mode
  
  
  return status;
}

HAL_StatusTypeDef PWM_ExternalTrigger_Start(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  /* 停止定时器（如果已经运行） */
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
  
  /* 清除所有标志位 */
  htim3.Instance->SR = 0;
  
  /* 重置计数器 */
  __HAL_TIM_SET_COUNTER(&htim3, 0);
  
  /* 确保One Pulse Mode配置正确 */
  if (!(htim3.Instance->CR1 & TIM_CR1_OPM)) {
    htim3.Instance->CR1 |= TIM_CR1_OPM;
    printf("启用TIM3 One Pulse Mode\r\n");
  }
  
  /* 确保从模式配置正确 */
  uint32_t sms = (htim3.Instance->SMCR & TIM_SMCR_SMS) >> TIM_SMCR_SMS_Pos;
  uint32_t ts = (htim3.Instance->SMCR & TIM_SMCR_TS) >> TIM_SMCR_TS_Pos;
  
  if (sms != 4 || ts != 6) {  // 6对应TIM_TS_TI2FP2
      // 重新配置从模式
      TIM_SlaveConfigTypeDef sSlaveConfig = {0};
      sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
      sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
      sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
      sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
      sSlaveConfig.TriggerFilter = 0x0;
      HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig);
      printf("重新配置TIM3从模式\r\n");
  }
  
  /* 启动PWM输出 */
  status = HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  if (status != HAL_OK) {
      printf("TIM3 PWM启动失败\r\n");
      return status;
  }
  
  /* 验证启动状态 */
  printf("PWM启动状态：\r\n");
  printf("- TIM3 CEN: %d\r\n", (htim3.Instance->CR1 & TIM_CR1_CEN) ? 1 : 0);
  printf("- TIM3等待外部触发信号...\r\n");
  
  return status;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  Init();
#if 0          // stc例程移植调试
  lcd_init();
	while(1)
	{
		display();
	}
#endif
  // PWM_ExternalTrigger_Init(50,100); // 初始化PWM，脉宽50us，最大周期100us
  // PWM_ExternalTrigger_Start(); // 启动PWM
  PWM_Start(50,10000); // 启动PWM
  //HAL_TIM_Base_Start_IT(&htim2);
  //htim3.Instance->CR1 &= ~TIM_CR1_OPM; // 停用OPM模式
  while(1)
  {
    
    Delay_ms(50);  // 延时100ms
    
  }
}


/** 
  * @brief  硬件初始化
  * @param  None
  * @retval None
  */
void SysInit()
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
  //MX_WWDG_Init();             //看门狗初始化
  
  MX_SPI2_Init();           //SPI初始化

  MX_TIM3_Init();             //TIM3初始化
  MX_TIM4_Init();             //TIM4初始化
  MX_TIM2_Init();
  MX_RTC_Init();           //RTC初始化
  __disable_irq(); //禁止所有中断
}

void Init(void){
  SysInit();        //CubeMX配置的系统初始化  
  InitSysTick();         //初始化SysTick定时器
  InitKey();        //按键初始化
  Encoder_Init(); //编码器初始化
  __enable_irq(); //使能所有中断
#ifdef TEST
  if(CheckPowerKey(1000))//电源键按至少2s
  {
    PowerSeq(); //上电缓启动
  }
  else
  {

    ShutDown(); // 关机

  }		
  LcmReset();          //LCD复位
  LcmInit();            //LCD初始化(按照数据手册)
#endif
  Status_MCU = Status_idle;
  LoadSysConfig();//先加载SysPara，因为部分AppPara的参数转换需要知道灯管类型
  LoadConfig();
  BackLightOn();       //背光打开

  //初始化完毕，允许输出
	Status_MCU =  Status_WorkStall;
  {
		if(AppPara.PowerKey == PwrKey_Hit)
		{
			//StopToFlash();    //初始化暂停闪烁
			WorkEn = 0;
			PwrHitFlag = PwrHit_WORK;
		}
	}
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 15;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
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
  /* 使能PWR和BKP外设时钟 */
// __HAL_RCC_PWR_CLK_ENABLE();
// __HAL_RCC_BKP_CLK_ENABLE();

//   /* 允许访问备份域 */
// HAL_PWR_EnableBkUpAccess();
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
/** Enable the WakeUp*/
if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 99, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) //设置中断，50ms一次
{
  Error_Handler();
}
  /* USER CODE END RTC_Init 2 */

}
/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;//双线但只用发送或单线
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
  WRITE_REG(hspi2.Instance->CR1,hspi2.Instance->CR1 |= SPI_CR1_BIDIOE); //设置输出开启
  __HAL_SPI_ENABLE(&hspi2);

  /* USER CODE END SPI2_Init 2 */

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
   hwwdg.Init.Prescaler = WWDG_PRESCALER_128;
   hwwdg.Init.Window = 110;
   hwwdg.Init.Counter = 110;
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
 HAL_GPIO_WritePin(GPIOA, POWER_ALL_ON_Pin|SNSR_GPIO_Pin|LCD_RSTB_Pin|BKLT_ON_Pin, GPIO_PIN_RESET);

 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOB, DRIVER_ON_N_Pin|LCD_CSB_Pin|LDC_A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SNSR_ADC_Pin PA5_DAC_Pin */
  GPIO_InitStruct.Pin = SNSR_ADC_Pin|PA5_DAC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*Configure GPIO pins : POWER_ALL_ON_Pin SNSR_GPIO_Pin LCD_RSTB_Pin BKLT_ON_Pin */
  GPIO_InitStruct.Pin = POWER_ALL_ON_Pin|SNSR_GPIO_Pin|LCD_RSTB_Pin|BKLT_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*Configure GPIO pins : KEY_POWER_Pin KEY_RIGHT_Pin KEY_DOWN_Pin KEY_ENTER_Pin
                           KEY_LEFT_Pin KEY_UP_Pin */
  GPIO_InitStruct.Pin = KEY_POWER_Pin|KEY_RIGHT_Pin|KEY_DOWN_Pin|KEY_ENTER_Pin
                          |KEY_LEFT_Pin|KEY_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  /*Configure GPIO pins : DRIVER_ON_N_Pin LDC_A0_Pin */
  GPIO_InitStruct.Pin = DRIVER_ON_N_Pin|LCD_CSB_Pin|LDC_A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}
/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 41;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 99;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim3, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR1;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 49;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

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
    #ifdef DEBUG
    switch (my_error)
    {
      case ERROR_PWM:

        printf("PWM error\n");
        break;
      default:
        break;
    }
    #endif
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
