#ifndef _SYS_PARA_COMMON_H_
#define _SYS_PARA_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
#include "type.h"

//模块定义保存在flash里的参数表。做如下规定
//1

//部分具体实现定义在各应用项目的AppPara.h
#include "SysPara.h"


extern SYS_PARA SysPara;				//使用中的参数表
extern const SYS_PARA SysParaDefault;	//缺省值
extern const SYS_PARA SysParaMin;		//最小值
extern const SYS_PARA SysParaMax;		//最大值
extern volatile u8 NeedSaveSysFlag ;		//标记参数表是否要保存，可以用这个Flag，也可与备份值比较再决定是否保存

//检验参数值,超出设成缺省值
#define VerifySysParaToDefault(x) {								\
		if(SysPara.x>SysParaMax.x || SysPara.x<SysParaMin.x) {	\
			SysPara.x = SysParaDefault.x;	NeedSaveSysFlag = 1;	\
		} 														\
	}
//检验参数值,超出设成边界值
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

