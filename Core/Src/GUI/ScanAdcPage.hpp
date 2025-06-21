#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_
#include "ByteConstSymbol.h"

#include "GUI_page.hpp"
#include "GUI_Speed.hpp"

typedef struct {
    float frequencies[4]={};  // �ĸ���ҪƵ��
    float magnitudes[4]={};   // ��Ӧ�ķ�ֵ
} FrequencyPeaks;
extern FrequencyPeaks fft_peaks; // ���ڴ洢FFT������


void DMA1_Channel1_IRQHandler(void);
FrequencyPeaks compute_fft_peak_frequencies(uint16_t *adc_data, uint32_t sample_rate, uint32_t N);
namespace gui {
class ScanAdcPage : public GUI_Page
{

private:  
    ScanAdcPage();   //���캯����˽�е�  

    ScanAdcPage(const ScanAdcPage &);  
    ScanAdcPage & operator = (const ScanAdcPage &);  

	GUI_Page * bakPage;

	enum{
		iProgress,
		iFreq2,
		iFreq3,
		iFreq4,
		MaxObjNum
	};

	GUI_Speed &SpeedCtrl;
	int Val;
	int BakX,BakY;

	int Step;	///����ֵ�����ƽ������ƶ��ٶ�
	
	void OnStep(int step);
	void StopScan();
	void StartScan();

public:	
    static ScanAdcPage & GetInstance()  
    {  
        static ScanAdcPage instance;   //�ֲ���̬����  
        return instance;  
    } 	
	virtual void Init();
	void UnInit();
	void ShowResults(int* freqs);
	virtual void Show();
	virtual int Loop();
};


}

#endif
