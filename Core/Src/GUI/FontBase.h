#ifndef _FONT_BASE_H_
#define _FONT_BASE_H_

#include "type.h"
#ifdef __cplusplus
 extern "C" {
#endif 

//
typedef struct {
	u8 WidthMin;	//非等宽字体WidthMin!=WidthMax
	u8 WidthMax;	//  等宽字体WidthMin =WidthMax
	u8 WidthDefault;	//缺省宽度，用于简单计算字符串总宽度
	u8 Height;		//所有字符高度相同
	u8 Width;		//当前查找的字符宽度，在Find函数里更新
	u8 * (*Find)(u16 code);	//查找点阵起始位置的函数
}Font_t;
 
#ifdef __cplusplus
 }
#endif 

#endif//_FONT_BASE_H_

