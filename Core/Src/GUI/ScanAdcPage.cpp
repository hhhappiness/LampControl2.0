#include "ScanAdcPage.hpp"
#include "Icons.hpp"
#include "ctrl.h"
#include "AppParaCommon.h"
#include "stm32g4xx_hal.h"
// #include <arm_math.h>
#include "math.h"

#define BUFFER_SIZE 500 // ������������С

uint16_t adc_buffer[600]; // �洢��������
FrequencyPeaks fft_peaks;

u8 first_time = 1, DMA_flag=0; // ���ڱ���Ƿ��һ������


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
	"FR �˲�",
};
static const char *AlgorithmStr_En[AlgNum]={
	"FFT",
	"FR Filter",
};

///���캯��
ScanAdcPage::ScanAdcPage()
: GUI_Page(MaxObjNum, SecondDispBuf)
, SpeedCtrl(GUI_Speed::GetInstance())
{
    Progress = new GUI_Progress(LcmXPixel,16);
	ObjList.Append(Progress);
}


///��ʼ�������ݵ�ԭPageָ�룬�����˳�ʱ�ָ���ʾ
void ScanAdcPage::Init()
{
	bakPage = pCurrPage;
	pCurrPage = this;
	//����������
	Progress->SetPos(0,24);
	Progress->SetRange(SpeedCtrl.Min, SpeedCtrl.Max);
	Progress->SetValue(0); //��ʼֵ
}

void ScanAdcPage::UnInit()
{
    ObjList.Delete(0,MaxObjNum); // ɾ������New�����Ŀؼ�
	//�˳�ǰ�ָ�pCurrPageָ��
	pCurrPage = bakPage;
	pCurrPage->Update();//�ָ��ڸǲ��ֵ���ʾ	
    StopScan(); // ֹͣ�ɼ�
	ClearKey();//�������ĳ�����
}
///��ʾһ����򣬸���ťͼ�꣬����ʾ�ؼ�
void ScanAdcPage::Show()
{
	Clear();
	//Rectangle(0, 0, LcmXPixel, LcmYPixel);
	//��ʾ���ڱ���
	if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-6*DEFAULT_HANZI_WIDTH-3*DEFAULT_ASCII_WIDTH)/2,0,"���ڲɼ�����...");
        DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-4*DEFAULT_ASCII_WIDTH)/2,DIGITAL_Y,"�㷨:");
	}else{
		DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"Collecting Data...");
        DispStr8(2,DIGITAL_Y,"Algorithm:");
	}
	
	//��ʾ�ؼ�
	GUI_Page::Show(0);
	//��ʾ�㷨
	DispStr8((LcmXPixel-2*DEFAULT_HANZI_WIDTH-4*DEFAULT_ASCII_WIDTH)/2 + \
            2*DEFAULT_HANZI_WIDTH+DEFAULT_ASCII_WIDTH,DIGITAL_Y,AlgorithmStr_Cn[AppPara.Algorithm]);	
	Update();
}

void ScanAdcPage::ShowResults(int* freqs){
    Clear(); // �����ʾ����
    if(AppPara.Language == Lang_Chinese){
		DispStr8((LcmXPixel-4*DEFAULT_HANZI_WIDTH-DEFAULT_ASCII_WIDTH)/2,0,"�㷨���:");
        DispStr8((LcmXPixel-3*DEFAULT_HANZI_WIDTH)/2,DIGITAL_Y,"��ȷ��");
	}else{
		DispStr8((LcmXPixel-8*DEFAULT_HANZI_WIDTH)/2,0,"Algorithm Results:");
        DispStr8(2,DIGITAL_Y,"Please confirm:");
	}
    //��ʾ�ĸ�����ֵ�����Ƶ��
    Freq[0] = new GUI_NumText(freqs, 3, 0, &Song_Width9_ASCII); // �����ĸ�Ƶ���ı��ؼ�
    Freq[1] = new GUI_NumText(freqs+1, 3, 0, &Song_Width9_ASCII);    
    Freq[2] = new GUI_NumText(freqs+2, 3, 0, &Song_Width9_ASCII);
    Freq[3] = new GUI_NumText(freqs+3, 3, 0, &Song_Width9_ASCII);
    for(int i = 0; i < 4; i++){
        Freq[i]->SetPos(2 + (i) * (3 * 9+4), 24);  //setÿ���ؼ���λ��
        Freq[i]->Enable = true; // ���ý��㹦��
        Freq[i]->Align = AlignRight; // �Ҷ���
        ObjList.Append(Freq[i]);
    }
    SetFocus(iFreq1,false);
    GUI_Page::Show(1, 4); //��ʾ���˽��������㷨�ؼ�֮��������ؼ�
    Update(); // ������ʾ

}

#define KEY_PERIOD 		50
#define NO_KEY_QUIT 	10000

#define HOLD_REPEATE_NUM	4

///����ѭ�������Ҽ��ƶ���꣬ȷ��ѡ���˳�
int ScanAdcPage::Loop()
{
    #if 1
    uint32_t remaining = BUFFER_SIZE,completion_percentage=0; // ʣ�ഫ������
    // StartScan(); // ����ADC�ɼ�
    while(remaining>0){ // �ȴ�DMA�������
        // ��ȡʣ�ഫ������
        // remaining = __HAL_DMA_GET_COUNTER(&hdma_adc1);
        
         
        // ��������ɰٷֱ�
        completion_percentage = 19900 * (BUFFER_SIZE - remaining) / BUFFER_SIZE + 100;
        remaining -= BUFFER_SIZE/5;
        Progress->SetValue(completion_percentage); //���½�����
        delay_ms(1000);

    }
    int* freqs = compute_fft_peak_frequencies(adc_buffer, 500, BUFFER_SIZE); // ����FFT��ֵƵ��
    #endif
    ShowResults(freqs); // ��ʾ���
    
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
    HAL_TIM_Base_Stop(&htim7);    // ֹͣ��ʱ������
    HAL_ADC_Stop_DMA(&hadc1);     // ֹͣADC��DMA	
}

///�����Զ���
void ScanAdcPage::StartScan()  //����adc��ʱ�ɼ���ͨ��DMA���䵽adc_buffer
{
    if(first_time) {  // ����ǵ�һ�����У����� ADC У׼
        HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
        first_time = 0;
    }
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);
    // ����DMA��������ж�
    //__HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
    HAL_TIM_Base_Start(&htim7);
}


}//namespace gui {

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_adc1);  // ����DMA�ж�
  DMA_flag = 1; 
}
typedef float float32_t;
FrequencyPeaks compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N) {
    #if 0
    arm_rfft_fast_instance_f32 S;
#endif
    float32_t *real = new float32_t[N];
    float32_t *magnitude = new float32_t[N/2];
    
    // ��һ��ADC���ݲ�Ӧ�ú�����
    for (uint32_t i = 0; i < N; i++) {
        // ������ϵ��
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (N - 1)));
        // Ӧ�ô���������һ������
        real[i] = ((float)(adc_data[i] - 2048) / 2048.0f) * window;
    }

    #if 0
    // ִ��FFT
    arm_rfft_fast_init_f32(&S, N);
    arm_rfft_fast_f32(&S, real, real, 0);
#endif
    // �����ֵ��
    for (uint32_t i = 0; i < N/2; i++) {
        // ʵ�����鲿��CMSIS-DSP��rfft����е����з�ʽ
        float re = real[2*i];
        float im = real[2*i+1];
        magnitude[i] = sqrtf(re*re + im*im) / (N/2); // ��һ��
    }
    
    // Ѱ�������ĸ���ֵ��Ӧ��Ƶ��
    uint32_t max_indices[4] = {0, 0, 0, 0};
    float max_mags[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    // �ӵ�2��Ԫ�ؿ�ʼ������DC������
    for (uint32_t i = 1; i < N/2; i++) {
        if (magnitude[i] > max_mags[3]) {
            // ����Ƿ��Ǿֲ���ֵ�������ڵ��
            if (i > 0 && i < N/2 - 1) {
                if (magnitude[i] < magnitude[i-1] || magnitude[i] < magnitude[i+1]) {
                    continue; // ���Ǿֲ���ֵ
                }
            }
            
            // ������ֵ��������������
            if (magnitude[i] > max_mags[0]) {
                // ��ֵ������
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = max_mags[1];
                max_indices[2] = max_indices[1];
                max_mags[1] = max_mags[0];
                max_indices[1] = max_indices[0];
                max_mags[0] = magnitude[i];
                max_indices[0] = i;
            } else if (magnitude[i] > max_mags[1]) {
                // ��ֵ�ǵڶ����
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = max_mags[1];
                max_indices[2] = max_indices[1];
                max_mags[1] = magnitude[i];
                max_indices[1] = i;
            } else if (magnitude[i] > max_mags[2]) {
                // ��ֵ�ǵ������
                max_mags[3] = max_mags[2];
                max_indices[3] = max_indices[2];
                max_mags[2] = magnitude[i];
                max_indices[2] = i;
            } else {
                // ��ֵ�ǵ��Ĵ��
                max_mags[3] = magnitude[i];
                max_indices[3] = i;
            }
        }
    }

    // ʹ�������߲�ֵ���Ƶ�ʹ��ƾ���
    FrequencyPeaks result;
    for (int i = 0; i < 4; i++) {
        uint32_t idx = max_indices[i];
        if (idx > 0 && idx < N/2 - 1) {
            // �����߲�ֵ
            float alpha = magnitude[idx-1];
            float beta = magnitude[idx];
            float gamma = magnitude[idx+1];
            float delta = 0.5f * (alpha - gamma) / (alpha - 2.0f*beta + gamma);
            
            // ����Ƶ�ʹ���
            float corrected_idx = idx + delta;
            result.frequencies[i] = (float)(corrected_idx * sample_rate) / N;
        } else {
            // ֱ�Ӽ���Ƶ�ʣ��޷���ֵ�������
            result.frequencies[i] = (float)(idx * sample_rate) / N;
        }
        
        result.magnitudes[i] = max_mags[i];
    }
    
    delete[] real;
    delete[] magnitude;
    return result;
}