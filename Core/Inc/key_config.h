#ifndef _KEY_CONFIG_H_
#define _KEY_CONFIG_H_
#pragma once
#ifdef __cplusplus
 extern "C" {
#endif 
#include "board_config.h"

/**
\file key_config.h
��ͷ�ļ�����ͨ�ð�����������Ҫ�ĺ궨��ı�����
**/

#define  KeyPushTimes           2 //     //ȷ�����¡��ͷŵļ�������������2��
#define  KeyLongPushTimes      (600/KEY_DETECT_PERIOD) //��ѹ����ʱ�������� 12
#define  KeyRunTimes           (250/KEY_DETECT_PERIOD) //�Զ�����ʱ�������� 5
#define  DbKeyPressTime        (180/KEY_DETECT_PERIOD) //˫����������ʱ����һ�ΰ�ѹ��ʱ�����ֵ��160-200ms����
#define  DbKeyReleaseTime      (180/KEY_DETECT_PERIOD) //˫����������ʱ����һ���ͷŵ���2��ѹ�µ�ʱ�����ֵ������һ��ֵ���

//ʹ��4�����Ļ�����, ��union������λ�����
//Stm32�����ֽڴ������Ż�����������ʹ���ֽڶ���Ӱ������
typedef union{
    u32 Word;
    u8 Key[4];
}KEY_BUF;

extern RTC_HandleTypeDef hrtc;
//��ͬ�����õò�ͬ��ʱ��
#define DisableKeyTimInt() HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) //��������ʱ���ж�
#define EnableKeyTimInt()  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 63, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) Error_Handler();//��������ʱ���ж�



//������������
#define KeyScanNum       1
#define KeyRowNum 		(6) 


//��������ŵ�λ��ַ

static inline u32 KeyI(u8 KeyRow) {
    switch (KeyRow) {
        case 0:
            return GPB_I(4); // PB3  KEY_DIV/key_right
        case 1:
            return GPB_I(5); // PB5  KEY_ENTER
        case 2:
            return GPB_I(7); // PB6  KEY_MULT/KEY_LEFT  
        case 3:
            return GPB_I(6); // PB4  KEY_DOWN
        case 4:
            return GPB_I(3); // PB7  KEY_UP
        case 5:
            return GPB_I(0); // PB8  KEY_POWER
    }
    return 0;
};

//��������ģʽ���塣
static const u8 KeyModeCtrl[KeyScanNum][KeyRowNum]={
	{
		KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        //KEY_SHIFT_EN,     �Ѿ�û��mode����
        KEY_REPEATE_EN,
       // KEY_DBCLICK_EN
	}
};



#define KeyPressLed(i)
#define KeyReleaseLed(i)


//�жϰ�����ƽ��������а�������ͬ�ĵ�ƽ�ð죬����һ�����жϸߵ͵�ƽ���С�
//�����Щ�����Ǹߵ�ƽ���д��ǵ͵�ƽ����Ҫд���������ж���
#define IsKeyPressed ((InP)==0) //���ｫ��ƽ�����ڱ���InP��͵�ƽ���£�����iδʹ��


#ifdef __cplusplus
}
#endif 
#endif

