#ifndef _APP_PARA_COMMON_H_
#define _APP_PARA_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
#include "type.h"

//模块定义保存在flash里的参数表。做如下规定
//1.参数表占用一整个page，实际大小不超过一个Page。因此擦除时可以擦除整个扇区，不考虑有其它数据。
//2.

//部分具体实现定义在各应用项目的AppPara.h
#include "AppPara.h"

//在AppPara.h需要定义的
//typedef struct{
//	int MagicNum;	//幻数，做为参数表的标记，不同应用不同，低8位可以做参数表版本号，便于升级应用时复制旧版本参数到新版本。
//	。。。。。		//参数表内容
//	u32 Reserved[N];//占位，保证参数表发生变化时，结构体大小不变
//	u32 Sum;		 //最后一项是校验和
//}APP_PARA;

extern APP_PARA AppPara;				//使用中的参数表
extern APP_PARA AppPara_Backup;			//备份参数表，可有可无
extern const APP_PARA AppParaDefault;	//缺省值
extern const APP_PARA AppParaMin;		//最小值
extern const APP_PARA AppParaMax;		//最大值
extern volatile u8 NeedSaveFlag ;		//标记参数表是否要保存，可以用这个Flag，也可与备份值比较再决定是否保存

//检验参数值,超出设成缺省值
#define VerifyParaToDefault(x) {								\
		if(AppPara.x>AppParaMax.x || AppPara.x<AppParaMin.x) {	\
			AppPara.x = AppParaDefault.x;	NeedSaveFlag = 1;	\
		} 														\
	}
//检验参数值,超出设成边界值
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
