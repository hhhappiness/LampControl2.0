// 定时器初始化（TIM7）
#include <arm_math.h>

#define BUFFER_SIZE 8192

uint16_t adc_buffer[BUFFER_SIZE]; // 存储采样数据
typedef struct {
    float frequencies[4];  // 四个主要频率
    float magnitudes[4];   // 对应的幅值
} FrequencyPeaks;
int first_time = 1; // 用于标记是否第一次运行
//主程序逻辑
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();
    MX_DMA_Init();

    // 启动 ADC + DMA 采样
    if(first_time) {  // 如果是第一次运行，进行 ADC 校准
        HAL_ADCEx_Calibration_Start();
        first_time = 0;
    }
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);

    while (1) {
        // 等待采样完成（可使用标志位或中断）
        // 执行 FFT 分析
        FrequencyPeaks freq = compute_fft_peak_frequency(adc_buffer, 2000, BUFFER_SIZE);
        printf("主频: %.2f Hz\n", freq.frequencies[0]);
        printf("幅值: %.2f\n", freq.magnitudes[0]);
    }
}



FrequencyPeaks compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N) {
    arm_rfft_fast_instance_f32 S;
    float32_t real[N], imag[N/2];
    float32_t magnitude[N/2];
    
    // 归一化ADC数据并应用汉宁窗
    for (uint32_t i = 0; i < N; i++) {
        // 汉宁窗系数
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (N - 1)));
        // 应用窗函数到归一化数据
        real[i] = ((float)(adc_data[i] - 2048) / 2048.0f) * window;
    }

    // 执行FFT
    arm_rfft_fast_init_f32(&S, N);
    arm_rfft_fast_f32(&S, real, real, 0);

    // 计算幅值谱
    for (uint32_t i = 0; i < N/2; i++) {
        // 实部和虚部在CMSIS-DSP的rfft结果中的排列方式
        float re = real[2*i];
        float im = real[2*i+1];
        magnitude[i] = sqrtf(re*re + im*im) / (N/2); // 归一化
    }

    // 寻找最大的四个幅值对应的频率
    uint32_t max_indices[4] = {0, 0, 0, 0};
    float max_mags[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    // 从第2个元素开始（跳过DC分量）
    for (uint32_t i = 1; i < N/2; i++) {
        if (magnitude[i] > max_mags[3]) {
            // 检查是否是局部峰值（比相邻点大）
            if (i > 0 && i < N/2 - 1) {
                if (magnitude[i] < magnitude[i-1] || magnitude[i] < magnitude[i+1]) {
                    continue; // 不是局部峰值
                }
            }
            
            // 插入新值并保持数组排序
            if (magnitude[i] > max_mags[0]) {
                // 新值是最大的
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = max_mags[1];
                max_indices[2] = max_indices[1];
                max_mags[1] = max_mags[0];
                max_indices[1] = max_indices[0];
                max_mags[0] = magnitude[i];
                max_indices[0] = i;
            } else if (magnitude[i] > max_mags[1]) {
                // 新值是第二大的
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = max_mags[1];
                max_indices[2] = max_indices[1];
                max_mags[1] = magnitude[i];
                max_indices[1] = i;
            } else if (magnitude[i] > max_mags[2]) {
                // 新值是第三大的
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = magnitude[i];
                max_indices[2] = i;
            } else {
                // 新值是第四大的
                max_mags[3] = magnitude[i];
                max_indices[3] = i;
            }
        }
    }

    // 使用抛物线插值提高频率估计精度
    FrequencyPeaks result;
    for (int i = 0; i < 4; i++) {
        uint32_t idx = max_indices[i];
        if (idx > 0 && idx < N/2 - 1) {
            // 抛物线插值
            float alpha = magnitude[idx-1];
            float beta = magnitude[idx];
            float gamma = magnitude[idx+1];
            float delta = 0.5f * (alpha - gamma) / (alpha - 2.0f*beta + gamma);
            
            // 修正频率估计
            float corrected_idx = idx + delta;
            result.frequencies[i] = (float)(corrected_idx * sample_rate) / N;
        } else {
            // 直接计算频率（无法插值的情况）
            result.frequencies[i] = (float)(idx * sample_rate) / N;
        }
        
        result.magnitudes[i] = max_mags[i];
    }
    
    return result;
}

void MX_TIM7_Init(void) {
    TIM_HandleTypeDef htim7;
    htim7.Instance = TIM7;
    htim7.Init.Prescaler = 84 - 1;        // 84MHz / 84 = 1MHz
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.Period = 499;              // 1MHz / 500 = 2000Hz  配置到四倍目标频率
    htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim7);
    HAL_TIM_Base_Start(&htim7);
}

// ADC 初始化
//单通道连续采样，用dma循环模式
void MX_ADC1_Init(void) {
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 84MHz / 4 = 21MHz
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE; // 禁用连续转换
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T2_TRGO; // 定时器触发
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING; // 上升沿触发
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE; // 禁用连续DMA请求
    
    HAL_ADC_Init(&hadc1);
    
    // 配置通道 0（模拟输入引脚）
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // 低阻抗信号
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

//dma配置
void MX_DMA_Init(void) {
    __HAL_RCC_DMA2_CLK_ENABLE();

    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_NORMAL; // 单次传输模式
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    HAL_DMA_Init(&hdma_adc1);

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    // 配置DMA中断
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    // 启用DMA传输完成中断
    __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
}

// DMA中断处理函数
void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

// ADC转换完成回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        // 设置采样完成标志
        adc_conversion_complete = 1;
    }
}
