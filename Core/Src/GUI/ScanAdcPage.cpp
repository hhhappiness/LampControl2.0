#include "ScanAdcPage.hpp"
#include "Icons.hpp"
#include "ctrl.h"
#include "AppParaCommon.h"
#include "stm32g4xx_hal.h"
// #include <arm_math.h>
#include "math.h"

#define BUFFER_SIZE 500 // 采样缓冲区大小

uint16_t adc_buffer[600]; // 存储采样数据
FrequencyPeaks fft_peaks;

u8 first_time = 1, DMA_flag=0; // 用于标记是否第一次运行


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim7;
extern DMA_HandleTypeDef hdma_adc1;


namespace gui {
	
#define DIGITAL_X	2+2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH
#define DIGITAL_Y	48
	

#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)
#define OBJECT_ALG 0


static const char *AlgorithmStr_Cn[AlgNum]={
	"FFT",
	"FR 滤波",
};
static const char *AlgorithmStr_En[AlgNum]={
	"FFT",
	"FR Filter",
};

///构造函数
ScanAdcPage::ScanAdcPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, SpeedCtrl(GUI_Speed::GetInstance())
{
    Progress = new GUI_Progress(LcmXPixel,16);
	ObjList.Append(Progress);
}


///初始化，备份当原Page指针，用于退出时恢复显示
void ScanAdcPage::Init()
{
	bakPage = pCurrPage;
	pCurrPage = this;
	//进度条设置
	Progress->SetPos(0,24);
	Progress->SetRange(SpeedCtrl.Min, SpeedCtrl.Max);
	Progress->SetValue(0); //初始值
}

void ScanAdcPage::UnInit()
{
    ObjList.Delete(0,MaxObjNum); // 删除所有New创建的控件
	//退出前恢复pCurrPage指针
	pCurrPage = bakPage;
	pCurrPage->Update();//恢复遮盖部分的显示	
    StopScan(); // 停止采集
	ClearKey();//清掉后面的长按键
}
///显示一个外框，各按钮图标，再显示控件
void ScanAdcPage::Show()
{
	Clear();
	//Rectangle(0, 0, LcmXPixel, LcmYPixel);
	//显示窗口标题
	if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-6*DEFAULT_HANZI_WIDTH-3*DEFAULT_ASCII_WIDTH)/2,0,"正在采集数据...");
        DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-4*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"算法:");
	}else{
		DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"Collecting Data...");
        DispStr8(2,DIGITAL_Y,"Algorithm:");
	}
	
	//显示控件
	GUI_Page::Show(0);
	//显示算法
	DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-4*DEFAULT_ASCII_WIDTH)/2 + \
            2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH,DIGITAL_Y,AlgorithmStr_Cn[AppPara.Algorithm]);	
	Update();
}

void ScanAdcPage::ShowResults(int* freqs){
    Clear(); // 清除显示区域
    if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-4*DEFAULT_HANZI_WIDTH-DEFAULT_ASCII_WIDTH)/2,0,"算法结果:");
        DispStr8((LcmXPixel-3*DEFAULT_HANZI_WIDTH)/2,DIGITAL_Y,"请确认");
	}else{
		DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"Algorithm Results:");
        DispStr8(2,DIGITAL_Y,"Please confirm:");
	}
    //显示四个最大幅值代表的频率
    Freq[0] = new GUI_NumText(freqs, 3, 0, &Song_Width9_ASCII); // 创建四个频率文本控件
    Freq[1] = new GUI_NumText(freqs+1, 3, 0, &Song_Width9_ASCII);    
    Freq[2] = new GUI_NumText(freqs+2, 3, 0, &Song_Width9_ASCII);
    Freq[3] = new GUI_NumText(freqs+3, 3, 0, &Song_Width9_ASCII);
    for(int i = 0; i < 4; i++){
        Freq[i]->SetPos(2 + (i) * (3 * 9+4), 24);  //set每个控件的位置
        Freq[i]->Enable = true; // 启用焦点功能
        Freq[i]->Align = AlignRight; // 右对齐
        ObjList.Append(Freq[i]);
    }
    SetFocus(iFreq1,false);
    GUI_Page::Show(1, 4); //显示除了进度条和算法控件之外的其他控件
    Update(); // 更新显示

}

#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///按键循环，左右键移动光标，确定选中退出
int ScanAdcPage::Loop()
{
    #if 1
    uint32_t remaining = BUFFER_SIZE,completion_percentage=0; // 剩余传输数量
    // StartScan(); // 启动ADC采集
    while(remaining>0){ // 等待DMA传输完成
        // 获取剩余传输数量
        // remaining = __HAL_DMA_GET_COUNTER(&hdma_adc1);
        
         
        // 计算已完成百分比
        completion_percentage = 19900 * (BUFFER_SIZE - remaining) / BUFFER_SIZE + 100;
        remaining -= BUFFER_SIZE/5;
        Progress->SetValue(completion_percentage); //更新进度条
        delay_ms(1000);

    }
    int* freqs = compute_fft_peak_frequencies(adc_buffer, 500, BUFFER_SIZE); // 计算FFT峰值频率
    #endif
    ShowResults(freqs); // 显示结果
    
    TKey = GetTimerCount();
	TIdle = GetTimerCount(); 
	while(1){
		if(IsTimeOut_ms(TKey,50)){
			TKey = GetTimerCount();
			Key = GetKey();
			wdg();
			if(Key != KEY_NULL){
				switch(Key){
                    case KEY_DIV2_SHOT : 	FocusPrevious();  break;
					case KEY_MULT_SHOT : 	FocusNext();  break;
					case KEY_ENTER_SHOT : 
						int ChosenFreq = OnEnter();
                        UnInit();
						return ChosenFreq;
					default:break;
				}
			}
		}
	}

}




void ScanAdcPage::StopScan()
{
    HAL_TIM_Base_Stop(&htim7);    // 停止定时器触发
    HAL_ADC_Stop_DMA(&hadc1);     // 停止ADC和DMA	
}

///启动自动加
void ScanAdcPage::StartScan()  //开启adc定时采集并通过DMA传输到adc_buffer
{
    if(first_time) {  // 如果是第一次运行，进行 ADC 校准
        HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
        first_time = 0;
    }
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);
    // 启用DMA传输完成中断
    //__HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
    HAL_TIM_Base_Start(&htim7);
}


}//namespace gui {

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_adc1);  // 处理DMA中断
  DMA_flag = 1; 
}
typedef float float32_t;
FrequencyPeaks compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N) {
    #if 0
    arm_rfft_fast_instance_f32 S;
#endif
    float32_t *real = new float32_t[N];
    float32_t *magnitude = new float32_t[N/2];
    
    // 归一化ADC数据并应用汉宁窗
    for (uint32_t i = 0; i < N; i++) {
        // 汉宁窗系数
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (N - 1)));
        // 应用窗函数到归一化数据
        real[i] = ((float)(adc_data[i] - 2048) / 2048.0f) * window;
    }

    #if 0
    // 执行FFT
    arm_rfft_fast_init_f32(&S, N);
    arm_rfft_fast_f32(&S, real, real, 0);
#endif
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
    
    delete[] real;
    delete[] magnitude;
    return result;
}