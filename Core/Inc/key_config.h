#ifndef _KEY_CONFIG_H_
#define _KEY_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif 
#include "board_config.h"

/**
\file key_config.h
��ͷ�ļ�����ͨ�ð�����������Ҫ�ĺ궨��ı�����
**/


//��ͬ�����õò�ͬ��ʱ��
#define DisableKeyTimInt() DisableINT(RTC_IRQn) //��������ʱ���ж�
#define EnableKeyTimInt()  EnableINT(RTC_IRQn) //��������ʱ���ж�


//������������
#define KeyScanNum       1
#define KeyRowNum 		(6) 

//��������ŵ�λ��ַ
u32 KeyI[KeyScanNum][KeyRowNum]={
	{
    GPA_I_Addr(11),//PA11  KEY_DIV  //GPIOx_IDR��ƫ������0x08
    GPC_I_Addr(13),//PC13  KEY_ENTER
    GPC_I_Addr(14),//PC14  KEY_MULT
    GPA_I_Addr(12),//PA12  KEY_DOWN
    GPA_I_Addr(15),//PA15  KEY_MODE
    GPC_I_Addr(15),//PC15	KEY_UP	
	}
};



//��������ģʽ���塣
static const u8 KeyModeCtrl[KeyScanNum][KeyRowNum]={
	{
		KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_REPEATE_EN,
        KEY_SHIFT_EN,
        KEY_REPEATE_EN,
       // KEY_DBCLICK_EN
	}
};



#define KeyPressLed(i)
#define KeyReleaseLed(i)


//�жϰ�����ƽ��������а�������ͬ�ĵ�ƽ�ð죬����һ�����жϸߵ͵�ƽ���С�
//�����Щ�����Ǹߵ�ƽ���д��ǵ͵�ƽ����Ҫд���������ж���
#define IsKeyPressed(i) ((InP)==0) //���ｫ��ƽ�����ڱ���InP��͵�ƽ���£�����iδʹ��


#ifdef __cplusplus
}
#endif 
#endif

