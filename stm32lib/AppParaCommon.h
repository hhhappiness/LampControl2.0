#ifndef _APP_PARA_COMMON_H_
#define _APP_PARA_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
#include "type.h"

//ģ�鶨�屣����flash��Ĳ����������¹涨
//1.������ռ��һ����page��ʵ�ʴ�С������һ��Page����˲���ʱ���Բ����������������������������ݡ�
//2.

//���־���ʵ�ֶ����ڸ�Ӧ����Ŀ��AppPara.h
#include "AppPara.h"

//��AppPara.h��Ҫ�����
//typedef struct{
//	int MagicNum;	//��������Ϊ������ı�ǣ���ͬӦ�ò�ͬ����8λ������������汾�ţ���������Ӧ��ʱ���ƾɰ汾�������°汾��
//	����������		//����������
//	u32 Reserved[N];//ռλ����֤���������仯ʱ���ṹ���С����
//	u32 Sum;		 //���һ����У���
//}APP_PARA;

extern APP_PARA AppPara;				//ʹ���еĲ�����
extern APP_PARA AppPara_Backup;			//���ݲ��������п���
extern const APP_PARA AppParaDefault;	//ȱʡֵ
extern const APP_PARA AppParaMin;		//��Сֵ
extern const APP_PARA AppParaMax;		//���ֵ
extern volatile u8 NeedSaveFlag ;		//��ǲ������Ƿ�Ҫ���棬���������Flag��Ҳ���뱸��ֵ�Ƚ��پ����Ƿ񱣴�

//�������ֵ,�������ȱʡֵ
#define VerifyParaToDefault(x) {								\
		if(AppPara.x>AppParaMax.x || AppPara.x<AppParaMin.x) {	\
			AppPara.x = AppParaDefault.x;	NeedSaveFlag = 1;	\
		} 														\
	}
//�������ֵ,������ɱ߽�ֵ
#define VerifyParaToLimt(x) {						\
	if(AppPara.x>AppParaMax.x) { 					\
		AppPara.x = AppParaMax.x; NeedSaveFlag = 1;	\
	} else if(AppPara.x<AppParaMin.x) {				\
		AppPara.x = AppParaMin.x; NeedSaveFlag = 1;	\
	}												\
}
void LoadDefaultConfig(void);
void SaveDefaultConfig(void);
void FlashPara(u32 addr,u32 *pDat,u32 len);
void LoadConfig(void);
void SaveConfig(void);
void VerifyParas(void);
void AppParaConvert(u32 ver);
BOOL CmpAppParam(void);	
void OnLoadConfig(void);

void VerifyParaList(int * pVal, const int * pValList, int ListNum, int DefaultId);

#ifdef __cplusplus
 }
#endif 
#endif
