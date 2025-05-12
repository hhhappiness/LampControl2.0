#ifndef _CORE_CONFIG_H_
#define _CORE_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#ifdef BOOTLOADER	//bootloader
	#define HSEClk       8000000
	#define SysClk       8000000
	#define AHBClk       8000000
	#define APB1Clk      8000000
	#define APB2Clk      8000000
	  //
	#define  SYS_CLOCK_SOURCE		SYS_CLOCK_SOURCE_HSI
	
	#define RTCCLK_SOURCE   RTCCLKSource_LSI //RTCCLKSource_HSE_Div128
	#define RTC_FREQ    1000 //20Hz���ڰ���ɨ�裬ͨ�ö�ʱ�ж�
#else				//App
	//clock config
	#define HSEClk      12000000
	#define SysClk      168000000
	#define AHBClk      168000000
	#define APB1Clk      2100000
	#define APB2Clk     168000000
	#define  SYS_CLOCK_SOURCE		SYS_CLOCK_SOURCE_PLL
	#define RTCCLK_SOURCE   RTCCLKSource_LSI //RTCCLKSource_HSE_Div128 ����Ҫ��standby��ʱ�䣬ֻ����LSI
	#define RTC_FREQ   1000//���ڰ���ɨ�裬ͨ�ö�ʱ�ж�
#endif

//����������ڣ���msΪ��λ
#define KEY_DETECT_PERIOD   (1000/(RTC_FREQ/50))

#ifdef __cplusplus
}
#endif 
	
#endif
