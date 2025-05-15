#include "SysParaCommon.h"
#include "AppParaCommon.h"
#include "utility.h"
//#include "stm32g4xx_hal_flash.h"
#include "string.h"
#include "bsp.h"

volatile u8 NeedSaveSysFlag = 0;


__inline u32 CheckSumSysPara(void)
{
	if(SysPara.LightType != ~SysPara.nLightType)
		return false;
	if(SysPara.ACoff != ~SysPara.nACoff)
		return false;
	if(SysPara.CCoff != ~SysPara.nCCoff)
		return false;
	if(SysPara.UID != ~SysPara.nUID)
		return false;
	return true;
}

/*
void UpdateSysPara(void)
{
	SysPara.nLightType = ~SysPara.LightType;
	SysPara.nACoff = ~SysPara.ACoff;
	SysPara.nCCoff = ~SysPara.CCoff;
	SysPara.nUID= ~SysPara.UID;
}
*/

void SaveSysConfig(void){

	SysPara.nLightType = ~SysPara.LightType;
	SysPara.nACoff = ~SysPara.ACoff;
	SysPara.nCCoff = ~SysPara.CCoff;
	SysPara.nUID= ~SysPara.UID;

	SaveUserData(SYS_PARA_ADDRESS,(u32 *)&SysPara,sizeof(SysPara));	
	NeedSaveSysFlag = 0;
}

#define UID_BASE 0x1FFFF7E8
void LoadDefaultSysConfig(void)
{
	u32 EncryptCode;
	u32 CpuID[3];	
	//don't load UID's value
	memcpy(&SysPara,&SysParaDefault,sizeof(SysPara));
	//Get UID
        //获取CPU唯一的ID  
        CpuID[0]=*(vu32*)(UID_BASE);  
        CpuID[1]=*(vu32*)(UID_BASE+4);  
        CpuID[2]=*(vu32*)(UID_BASE+8);          
  
        //加密算法,很简单的加密算法  
        EncryptCode=(CpuID[0]>>3)+(CpuID[1]>>2)+(CpuID[2]>>1);    
	SysPara.UID = EncryptCode;
	SysPara.nUID = ~SysPara.UID;
}

void SaveDefaultSysConfig(void)
{	
	LoadDefaultSysConfig();
	SaveSysConfig();
}

void LoadSysConfig(void)
{
	memcpy(&SysPara,(void *)SYS_PARA_ADDRESS,sizeof(SysPara));
	
	do{
		if(SysPara.MagicNum != SYS_PARA_MAGIC_NUM || (!CheckSumSysPara())){
		//if(SysPara.MagicNum != SYS_PARA_MAGIC_NUM) {
			SaveDefaultSysConfig();
			break;
		}
		if(SysPara.Version != SYS_PARA_VERSION){
			SysParaConvert(SysPara.Version);
		}
		
		VerifySysParas();
		if(NeedSaveSysFlag){
			SaveSysConfig();
		}
	}while(0);

	//OnLoadSysConfig();
}





