#include "ScanAdcPage.hpp"
#include "Icons.hpp"
#include "ctrl.h"
#include "AppParaCommon.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include <arm_math.h>
#include "arm_const_structs.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim7, htim15;
extern DMA_HandleTypeDef hdma_adc1;
extern DAC_HandleTypeDef hdac1;
typedef float float32_t;
extern volatile char time_trig;

typedef struct{
    float frequencies[4]={};  // 四个主要频率
    float magnitudes[4]={};   // 对应的幅值
}FrequencyPeaks;

FrequencyPeaks fft_peaks; // 用于存储FFT计算结果

#define Sampling_CNT 2048
#define FFT_LENGTH 2048
#define BUFFER_SIZE (FFT_LENGTH * 2)

volatile int ADC_DONE_FLAG = 0;
float fft_inputbuf[FFT_LENGTH * 2] = {0};
float fft_magnitude[FFT_LENGTH] ={0};
// float fft_magnitude[FFT_LENGTH] ={0};

u8 first_time = 1, DMA_flag=0; // 用于标记是否第一次运行
u8 algStrLenth_CH[2] = {3, 5};
u8 algStrLenth_EN[2] = {3, 7};
// void DMA1_Channel1_IRQHandler(void);
int* compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N);
float* Hanning_Win(float* Array, int len);
float* Blackman_Win(float* Array, int len);
void DO_FFT( unsigned int SampleRate, float* freq_found);

namespace gui {
	
#define DIGITAL_X	2+2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH
#define DIGITAL_Y	48
#define RESULT_NUM  6
	

#define UP_ARROW_X		2
#define DOWN_ARROW_X	(LcmXPixel-15)
#define OBJECT_ALG 0


static const char *AlgorithmStr_Cn[AlgNum]={
	"FFT",
	// "FR 滤波",
};
static const char *AlgorithmStr_En[AlgNum]={
	"FFT",
	// "FR Filter",
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

        #if TEST1
        DispStr8((LcmXPixel-4*DEFAULT_HANZI_WIDTH-RESULT_NUM*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"计算频率:");

        #else
        DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-algStrLenth_CH[AppPara.Algorithm]*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"算法:");
        	//显示算法
	    DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-algStrLenth_CH[AppPara.Algorithm]*DEFAULT_ASCII_WIDTH)/2 + \
            2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH,DIGITAL_Y,AlgorithmStr_Cn[AppPara.Algorithm]);	
        #endif
	}else{
		DispStr8((LcmXPixel-17*DEFAULT_ASCII_WIDTH)/2,0,"Collecting Data...");
        DispStr8((LcmXPixel-10*DEFAULT_ASCII_WIDTH- algStrLenth_EN[AppPara.Algorithm]*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"Algorithm:");
        	//显示算法
	    DispStr8((LcmXPixel-10*DEFAULT_ASCII_WIDTH- algStrLenth_EN[AppPara.Algorithm]*DEFAULT_ASCII_WIDTH)/2 + \
            10*DEFAULT_ASCII_WIDTH,DIGITAL_Y,AlgorithmStr_Cn[AppPara.Algorithm]);	
	}
	
	//显示控件
	GUI_Page::Show(0);

	Update();
}

void ScanAdcPage::ShowResults(int* freqs){
    Clear(); // 清除显示区域
    if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-4*DEFAULT_HANZI_WIDTH-DEFAULT_ASCII_WIDTH)/2,0,"算法结果:");
        DispStr8((LcmXPixel-3*DEFAULT_HANZI_WIDTH)/2,DIGITAL_Y,"请确认");
	}else{
		DispStr8((LcmXPixel-17*DEFAULT_ASCII_WIDTH)/2,0,"Algorithm Results:");
        DispStr8((LcmXPixel-13*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"Please confirm");
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

volatile int glob_cnt;   
///按键循环，左右键移动光标，确定选中退出
float ScanAdcPage::fLoop()
{
    uint32_t remaining = BUFFER_SIZE,completion_percentage=0; // 剩余传输数量
    int collectTimes = 1;
    float frequency = -1;
    int last_quarter = -1;

    Rect8_t Rect;

    StartScan(); // 启动ADC采集条件
    glob_cnt = 0;
    ADC_DONE_FLAG = 0;
    memset((void*)fft_inputbuf, 0, sizeof(fft_inputbuf));
        Rect = {(u8)((LcmXPixel-4*DEFAULT_HANZI_WIDTH - RESULT_NUM*DEFAULT_ASCII_WIDTH )/2 + \
            4*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH),DIGITAL_Y, RESULT_NUM*DEFAULT_ASCII_WIDTH, DEFAULT_ASCII_FONT.Height};
   #ifdef TEST_FFT
    while(ENTER_PRESSED){
        #else
    while(HALF_POWER_PRESSED){ // 用户按电源轻按键时进行数据采集
#endif
#ifdef DMA_ADC
        DispStr8( Rect.x, Rect.y ,&signalVal[0]);	
        LcmPutBmpRect(Rect.x+4,Rect.y, pCurrPage->pPix,Width, &Rect); // 更新显示

        completion_percentage = 19900 * (BUFFER_SIZE - remaining) / BUFFER_SIZE + 100;  // 计算已完成百分比
        remaining -= __HAL_DMA_GET_COUNTER(&hdma_adc1);         // 获取剩余传输数量
        Progress->SetValue(completion_percentage);              //更新进度条
        delay_ms(1000);
#else
    if(time_trig == 1&& !ADC_DONE_FLAG)
    {
        unsigned short adc_value = 114;
        HAL_ADC_Start(&hadc1);     // 启动ADC转换
        HAL_ADC_PollForConversion(&hadc1, 2);   // 等待ADC转换完成，超时时间为2ms
        if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
            adc_value = HAL_ADC_GetValue(&hadc1);   // 获取ADC转换结果
        fft_inputbuf[glob_cnt] = (float)adc_value;  // 将ADC结果存入FFT输入缓冲区
        fft_inputbuf[glob_cnt + 1] = 0;  // 虚部设为0
        glob_cnt = glob_cnt + 2;  // 缓冲区索引后移
        time_trig = 0;  // 清除触发标志
        completion_percentage = 19900 * glob_cnt / BUFFER_SIZE + 100;  // 计算完成百分比
        // 每完成四分之一时更新一次进度条
        int current_quarter = completion_percentage / 5000; // 每 5000 为四分之一（19900+100=20000，20000/4=5000）
        if (current_quarter != last_quarter) {
            Progress->SetValue(completion_percentage);  // 更新进度条
            last_quarter = current_quarter;
        }

        if(glob_cnt == FFT_LENGTH * 2) // 采集完成,进行计算FFT
        {
            ADC_DONE_FLAG = 1;
        }   
    }
    if(ADC_DONE_FLAG)
    //if(0)
    {
        #ifdef TEST_FFT
        // 将采集到的fft_input数据存到本地的txt文件中
        FILE *fp = fopen("fft_input_data.txt", "w");
        if (fp != NULL) {
            for (int i = 0; i < FFT_LENGTH * 2; i++) {
                fprintf(fp, "%f\n", fft_inputbuf[i]);
            }
            fclose(fp);
        }
        #endif
        DO_FFT( 1000, &frequency);
        // 将frequency转换为字符串
        char freq_str[6];
        snprintf(freq_str, sizeof(freq_str), "%f", frequency);
        DispStr8( Rect.x, Rect.y , freq_str);  // 显示平均值
        //再次进入采集
        ADC_DONE_FLAG = 0;
        glob_cnt=0;
        collectTimes++;
        char collectTimes_str[20];  // 增大缓冲区大小以确保能容纳格式化后的字符串
        snprintf(collectTimes_str, sizeof(collectTimes_str), "第%d次采集中...", collectTimes);
        DispStr8((LcmXPixel-6*DEFAULT_HANZI_WIDTH-4*DEFAULT_ASCII_WIDTH)/2,0,collectTimes_str);
        Update();
    }

    #if 0 //观察信号强度
    if (glob_cnt >= 20) {
        uint32_t sum = 0;
        // 计算最近20个FFT输入缓冲区数据的总和
        for (int i = glob_cnt - 20; i < glob_cnt; i += 1) {
            sum += (uint32_t)fft_inputbuf[i];
        }
        uint16_t average = sum / 10;  // 计算平均值
                
        // 将平均值转换为字符串
        char avg_str[4];
        snprintf(avg_str, sizeof(avg_str), "%d", average);
        DispStr8( Rect.x, Rect.y , avg_str);  // 显示平均值
        LcmPutBmpRect(Rect.x+4,Rect.y, pCurrPage->pPix,Width, &Rect); // 更新显示区域
    }
    #endif
    
    #endif
        if(POWER_PRESSED)
            break;
            #ifdef TEST_FFT
        if(!ENTER_PRESSED)
            break;
            #endif
    }
    StopScan();
    if(frequency > 0 && POWER_PRESSED){
        return frequency;
    }else
    {
        return (float)*SpeedCtrl.pVal/100;  //返回原值
    }
#if 0
    int* freqs = compute_fft_peak_frequencies(0, 500, BUFFER_SIZE); // 计算FFT峰值频率
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
    #endif

}

// 停止激光传感器的驱动、ADC采集
void ScanAdcPage::StopScan()
{
    HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2); // 停止PWM输出定时器触发
    SNSR_PWR(0); // 关闭测频模块电源
    HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2); // 停止DAC
    #ifdef DMA_ADC
    HAL_TIM_Base_Stop(&htim7);    // 停止定时器触发
    HAL_ADC_Stop_DMA(&hadc1);     // 停止ADC和DMA	
    #endif
}

///启动自动加
void ScanAdcPage::StartScan()  //开启adc定时采集并通过DMA传输到adc_buffer
{
    SNSR_PWR(1); //开启测频模块电源
    StopToFlash();   //开启测频后停止LED输出
    HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2); // 启动pwm输出定时器，455hz方波控制激光器
    
    if(first_time) {  // 如果是第一次运行，进行 ADC 校准
        HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
        first_time = 0;
    }

    //用单片机的DAC输出一个模拟电压，来控制测频模块模拟部分的增益(有效控制范围约为2.3V-2.7V，电压越高增益越大)，使其尽量接近单片机ADC的满量程。
   
    HAL_DAC_Start(&hdac1,DAC1_CHANNEL_2); // 启动DAC
    HAL_DAC_SetValue(&hdac1,DAC1_CHANNEL_2,DAC_ALIGN_12B_R,4095); // 设置DAC输出为2047，即1.65V
#ifdef DMA_ADC
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);   //传输数据个数，目前看手册感觉是以adc每次采集的16位数据为单位，所以还是buffersize
    // 启用DMA传输完成中断
    //__HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
    HAL_TIM_Base_Start(&htim7);      //启动adc触发定时器
#endif
}

}

int* compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N) {
    int *freqs;
     freqs[0] = (int)(100 + 0.5f);
     freqs[1] = (int)(50 + 0.5f);
     freqs[2] = (int)(25 + 0.5f);
     freqs[3] = (int)(12.5f + 0.5f);
    return freqs;
}
#if 1
int thresholdFactor = 4;
void DO_FFT( unsigned int SampleRate, float *freq_found)
{
   // Blackman_Win(fft_inputbuf,  Sampling_CNT);
    arm_cfft_f32(&arm_cfft_sR_f32_len2048, fft_inputbuf, 0, 1); 
    arm_cmplx_mag_f32(fft_inputbuf, fft_magnitude, FFT_LENGTH);
    
    #ifdef TEST_FFT
    // 将采集到的fft_input数据存到本地的txt文件中
    FILE *fp = fopen("fft_magnitude.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < FFT_LENGTH; i++) {
            fprintf(fp, "%f\n", fft_magnitude[i]);
        }
        fclose(fp);
    }
    #endif
    //计算平均功率
    double avg_power = 0;
    for(int i = 2; i < 102; i++) 
    {
        avg_power += (double)fft_magnitude[i];
      //  printf("%d %.0f\n", i, fft_magnitude[i]);
    }
    //printf("\ntotal power %f\n", avg_power);
    avg_power = avg_power / (100);
    
    //printf("avg_power %f\n", avg_power);
    double threshold = avg_power * thresholdFactor;
    
    //搜索1-50Hz
    int peak_found = 0;
    for(int i=1;i<102;i++)
    {
      if( fft_magnitude[i]> threshold && fft_magnitude[i]> fft_magnitude[i-1] && fft_magnitude[i] > fft_magnitude[i+1] )  
      {
        double fre_resu =  (double)SampleRate / (FFT_LENGTH);  // 频率分辨率
        
        // 使用二次抛物线插值法提高频率估计精度
        // 获取峰值点及其左右两个点的幅度
        double y1 = fft_magnitude[i-1];
        double y2 = fft_magnitude[i];
        double y3 = fft_magnitude[i+1];
        
        // 计算插值偏移量
        // 使用二次抛物线插值公式: delta = (y3 - y1) / (2 * (2*y2 - y1 - y3))
        double delta = (y3 - y1) / (2 * (2*y2 - y1 - y3));
        
        // 限制偏移量在[-0.5, 0.5]范围内，确保插值结果在相邻两个频率点之间
        if (delta > 0.5) delta = 0.5;
        if (delta < -0.5) delta = -0.5;
        
        // 计算插值后的频率
        double freq_find = (i  + delta) * fre_resu;
        
        //printf("First Freq below 50Hz: %.3f Hz, Magnitude: %f, Interpolated delta: %.3f\n", 
        //       freq_find, fft_magnitude[i], delta);
        peak_found = 1;
        *freq_found = freq_find;
        break;
      }
    }
    
    if (!peak_found) 
    {
        //printf("No significant peak found above threshold in 1-50Hz range\n");
    }
    
}
// Array: 实部、虚部交替存储的 FFT 输入数组
// len  : FFT 点数（实部数量）
float* Hanning_Win(float *Array, int len)
{
    for(int n = 0; n < len; n++)
    {
        // 汉宁窗系数
        float w = 0.5f - 0.5f * cosf(2.0f * 3.1415926 * n / (len - 1));

        // 乘窗（只处理实部）
        Array[2 * n] *= w;      // 实部
        Array[2 * n + 1] *= w;  // 虚部（如果需要窗虚部，通常为0也乘一下保持一致）
    }
    return Array;
}

// 布莱克曼窗
float* Blackman_Win(float *Array, int len)
{
    for(int n = 0; n < len; n++)
    {
        float w = 0.42f 
                - 0.5f * cosf(2.0f * 3.1415926 * n / (len - 1))
                + 0.08f * cosf(4.0f * 3.1415926 * n / (len - 1));

        Array[2 * n]     *= w; // 实部
        Array[2 * n + 1] *= w; // 虚部（如果虚部全 0，也可直接保留乘法以保持一致）
    }
    return Array;
}

#endif