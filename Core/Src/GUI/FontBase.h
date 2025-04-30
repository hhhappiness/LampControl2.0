#ifndef _FONT_BASE_H_
#define _FONT_BASE_H_

#include "type.h"
#ifdef __cplusplus
 extern "C" {
#endif 

//
typedef struct {
	u8 WidthMin;	//�ǵȿ�����WidthMin!=WidthMax
	u8 WidthMax;	//  �ȿ�����WidthMin =WidthMax
	u8 WidthDefault;	//ȱʡ��ȣ����ڼ򵥼����ַ����ܿ��
	u8 Height;		//�����ַ��߶���ͬ
	u8 Width;		//��ǰ���ҵ��ַ���ȣ���Find���������
	u8 * (*Find)(u16 code);	//���ҵ�����ʼλ�õĺ���
}Font_t;
 
#ifdef __cplusplus
 }
#endif 

#endif//_FONT_BASE_H_

