#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"

typedef struct {
    float frequencies[4]={};  // 四个主要频率
    float magnitudes[4]={};   // 对应的幅值
} FrequencyPeaks;
extern FrequencyPeaks fft_peaks; // 用于存储FFT计算结果


void DMA1_Channel1_IRQHandler(void);
FrequencyPeaks compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N);
namespace gui {
class ScanAdcPage : public GUI_Page
{

public:  
    ScanAdcPage();   //构造函数是私有的  

	GUI_Page * bakPage;
	GUI_Speed &SpeedCtrl;
	GUI_Progress* Progress;
	GUI_NumText* Freq[4];

	enum{
		iProgress,
		iFreq1,
		iFreq2,
		iFreq3,
		iFreq4,
		MaxObjNum
	};

	int Val;
	int BakX,BakY;

	
	void OnStep(int step);
	void StopScan();
	void StartScan();
	virtual void Init();
	void UnInit();
	void ShowResults(int* freqs);
	virtual void Show();
	virtual int Loop();
};


}

#endif
