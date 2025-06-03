#include "adc.h"
#include "AppParaCommon.h"
#include "SysParaCommon.h"

extern RTC_HandleTypeDef hrtc;
extern ADC_HandleTypeDef hadc2;
#define Nsamp RTC_FREQ/2 //10
u16 adc_val ;
volatile u16 adc_idx = 0;
u16 adc_buf[Nsamp] = {0};
volatile u16 adc_result = BAT_LVL4;	//电池电压，单位mV
volatile u8 AdcFlag = 0;
volatile int BatLevel = 5;

u16 StartAdcSample(void)
{
	uint16_t adcValue = 0;

	// 设置要转换的通道
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_4;  // VBAT_ACQ_Pin - PA7
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);

	// 开始转换
	HAL_ADC_Start(&hadc2);

	// 等待转换完成
	if(HAL_ADC_PollForConversion(&hadc2, 100) == HAL_OK)
	{
		adcValue = HAL_ADC_GetValue(&hadc2);
	}

	HAL_ADC_Stop(&hadc2);

	return adcValue;
}



#if 1
u16 adc_average(u16 *buf, u32 len)
{
	u32 i;
	u32 sum = 0;
	u16 *p = buf+2;
	for(i=0;i<len-2;i++)
	{
			sum += *p++;
	}
	sum = (sum + (len-2)/2)/(len-2);

	return (u16)sum;
}
#endif

#if 0
u16 adc_average(u16 *buf, u32 len)
{
	u32 i;
	u32 sum = 0;
	u16 *p = buf;
	for(i=0;i<len;i++)
		{
			sum += *p++;
		}
	sum = (sum + (len)/2)/(len);
		
	return (u16)sum;
}
#endif

u16 adc_bubble(u16 *buf, u32 len)
{
	int i,j;
	u16 tmp;
	u16 *p;
	for(i=0;i<len;i++) {
		for(j=i;j<len;j++) {
			if(buf[i] > buf[j])
				tmp = buf[i];
				buf[i] = buf[j];
				buf[j]= tmp;
		}
	}
	p =( buf + len - 1) - 48;
	//丢弃最大数值16个，取32个做平均
	tmp = adc_average(p, 32);
	return tmp;
}

u16 adc_calc(u16 code)
{
	u32 temp;
	temp = code;
	temp = (temp*36381 - 2216);
	temp >>= 12;
	return (u16)(temp & 0x0000FFFF);
}

//AD采样开始后, 前面ad采样的1-3个数可能是乱的
u8 ad_en= 0;
u16 ad_data;

void AdcSamp(void)
{
	ad_data = StartAdcSample();
	adc_buf[adc_idx++] = ad_data;
	if(adc_idx >= 10)
	{
		ad_en = 1;
	}

	if(ad_en == 1)
		{
			if(ad_data < BAT_LVSHDN/10)
				{
					//PowerOff();
				}
		}

	if(adc_idx >= Nsamp)
		{
			adc_idx = 0;
			adc_val = adc_average(adc_buf, Nsamp);
			adc_result = adc_calc(adc_val);
			AdcFlag = 1;
			//BatLevel = GetBatLevel();
		}
}

int GetBatLevel(void)
{
	int level;
	int adval;
	adval = (adc_result/100)*100;
	if(adval >= BAT_LVL4)
	{
		if(BatLevel==4)
		{
			level = (adval>=(BAT_LVL4 + BAT_LVOFFSET)) ? 5 : BatLevel;
		}
		else
			level = 5;
	}
	else if((adval < BAT_LVL4) && (adval >= BAT_LVL3))
	{
		if(BatLevel==5)
		{
			level = (adval< (BAT_LVL4 - BAT_LVOFFSET)) ? 4 : BatLevel;
		}
		else if(BatLevel==3)
		{
			level = (adval>=(BAT_LVL3 + BAT_LVOFFSET)) ? 4 : BatLevel;
		}
		else
			level = 4;
	}
	else if((adval < BAT_LVL3) && (adval >= BAT_LVL2))
	{
		if(BatLevel==4)
		{
			level = (adval< (BAT_LVL3 - BAT_LVOFFSET)) ? 3 : BatLevel;
		}
		else if(BatLevel==2)
		{
			level = (adval>=(BAT_LVL2 + BAT_LVOFFSET)) ? 3 : BatLevel;
		}
		else
			level = 3;
	}
	else if((adval < BAT_LVL2) && (adval >= BAT_LVL1))
	{
		if(BatLevel==3)
		{
			level = (adval< (BAT_LVL2 - BAT_LVOFFSET)) ? 2 : BatLevel;
		}
		else if(BatLevel==1)
		{
			level = (adval>=(BAT_LVL1 + BAT_LVOFFSET)) ? 2 : BatLevel;
		}
		else
			level = 2;
	}
	else if((adval < BAT_LVL1) && (adval >= BAT_LVL0))
	{
		if(BatLevel==2)
		{
			level = (adval< (BAT_LVL1 - BAT_LVOFFSET)) ? 1 : BatLevel;
		}
		else if(BatLevel==0)
		{
			level = (adval>=(BAT_LVL0 + BAT_LVOFFSET)) ? 1 : BatLevel;
		}
		else
			level = 1;
	}
	else
	{
		if(BatLevel==1)
		{
			level = (adval< (BAT_LVL0 - BAT_LVOFFSET)) ? 0 : BatLevel;
		}
		else
			level = 0;
	}

	return level;
}


//ADC参数校正相关

///获取一个ADC的平均值
int GetAvgAdc(void)
{
	return adc_val;
}

///计算ADC校正系统
void CalcAdcCoff(int ad1, int ad2, int v1, int v2)
{
	//SysPara.ACoff = ...
	//SysPara.CCoff = ...
	int delta_ad;
	int delta_v;
	int v1_t,v2_t;
	v1_t = v1<<16;
	v2_t = v2<<16;
	delta_ad = ad2 - ad1;
	if(delta_ad < 0)
		delta_ad *= -1;
	delta_v = v2_t - v1_t;
	if(delta_v < 0)
		delta_v*= -1;
	SysPara.ACoff =(delta_v + delta_ad/2)/delta_ad;
	SysPara.CCoff = v2_t - SysPara.ACoff*ad2;

}

///设置一个缺省值或由备份值恢复
void SetAdcCoff(int a_coff, int c_coff)
{
	//SysPara.ACoff = a_coff;
	//SysPara.CCoff = c_coff;
}

///由ADC采样值计算电压值
u16 CalcVoltage(u16 adc_val)
{
	/*
	int x;
	u16 y;
	x = GetAvgAdc();
	y = (SysPara.ACoff*x + SysPara.CCoff + 32768) >> 16;
	return y;
	*/
	return ((SysPara.ACoff*adc_val + SysPara.CCoff + 32768) >> 16);
}

///获取当前电压值
u16 GetVoltage(void)
{
	//暂时返回以前算法的结果
	//return adc_result;
	u16 ret;
	int avg = GetAvgAdc();
	ret = CalcVoltage(avg);
	return ret;
}
