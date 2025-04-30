#ifndef _SYS_PARA_COMMON_H_
#define _SYS_PARA_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
#include "type.h"

//ģ�鶨�屣����flash��Ĳ����������¹涨
//1

//���־���ʵ�ֶ����ڸ�Ӧ����Ŀ��AppPara.h
#include "SysPara.h"


extern SYS_PARA SysPara;				//ʹ���еĲ�����
extern const SYS_PARA SysParaDefault;	//ȱʡֵ
extern const SYS_PARA SysParaMin;		//��Сֵ
extern const SYS_PARA SysParaMax;		//���ֵ
extern volatile u8 NeedSaveSysFlag ;		//��ǲ������Ƿ�Ҫ���棬���������Flag��Ҳ���뱸��ֵ�Ƚ��پ����Ƿ񱣴�

//�������ֵ,�������ȱʡֵ
#define VerifySysParaToDefault(x) {								\
		if(SysPara.x>SysParaMax.x || SysPara.x<SysParaMin.x) {	\
			SysPara.x = SysParaDefault.x;	NeedSaveSysFlag = 1;	\
		} 														\
	}
//�������ֵ,������ɱ߽�ֵ
#define VerifySysParaToLimt(x) {						\
	if(SysPara.x>SysParaMax.x) { 					\
		SysPara.x = SysParaMax.x; NeedSaveSysFlag = 1;	\
	} else if(SysPara.x<SysParaMin.x) {				\
		SysPara.x = SysParaMin.x; NeedSaveSysFlag = 1;	\
	}												\
}



void LoadDefaultSysConfig(void);
void SaveDefaultSysConfig(void);
void LoadSysConfig(void);
void SaveSysConfig(void);
//void VerifySysParas(void);




#ifdef __cplusplus
 }
#endif 
#endif

