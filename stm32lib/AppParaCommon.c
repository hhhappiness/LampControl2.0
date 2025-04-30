#include "AppParaCommon.h"
#include "utility.h"
#include "stm32f10x_flash.h"
#include "string.h"
#include "ctrl_common.h"

volatile u8 NeedSaveFlag = 0;


__inline u32 CheckSumPara(void)
{
	return SumU32((u32*)&AppPara, sizeof(APP_PARA)/4 -1);
}

void SaveConfig(void){
	//FlashPara(0x08000000+(u32)&FlashSave,(u32 *)&AppPara,(sizeof(AppPara)+3)/4);
	//FlashPara(0x03fff000, (u32 *)&AppPara,(sizeof(AppPara)+3)/4);

	AppPara.Sum = CheckSumPara();
	
	FlashPara(APP_PARA_ADDRESS,(u32 *)&AppPara,(sizeof(AppPara)+3)/4);	
	NeedSaveFlag = 0;
}


void LoadDefaultConfig(void)
{
	memcpy(&AppPara,&AppParaDefault,sizeof(AppPara));
}

void SaveDefaultConfig(void)
{	
	LoadDefaultConfig();

	SaveConfig();
}

void LoadConfig(void)
{
//	static u32 tmp_sum = 0;
	memcpy(&AppPara,(void *)APP_PARA_ADDRESS,sizeof(AppPara));
	memcpy(&AppPara_Backup,(void *)APP_PARA_ADDRESS,sizeof(AppPara_Backup));
	do{
		#if 0
		if(AppPara.MagicNum != APP_PARA_MAGIC_NUM || CheckSumPara() != AppPara.Sum){
			SaveDefaultConfig();
			break;
		}
		#endif
		if(AppPara.MagicNum != APP_PARA_MAGIC_NUM ){
			SaveDefaultConfig();
			break;
		}
		
//		tmp_sum = CheckSumPara();
		if( CheckSumPara() != AppPara.Sum){
			//SaveDefaultConfig();
			break;
		}
		if(AppPara.Version != APP_PARA_VERSION){
			AppParaConvert(AppPara.Version);
		}
		
		VerifyParas();
		if(NeedSaveFlag){
			SaveConfig();
		}
	}while(0);

	OnLoadConfig();
}

//比较参数表除Sum之外的值
BOOL CmpAppParam(void)
{
	if(memcmp(&AppPara_Backup,&AppPara,sizeof(APP_PARA)-4)!=0)
		return TRUE;
	else
		return FALSE;
}

//参数表写入Flash，擦除整个Page
void FlashPara(u32 addr,u32 *pDat,u32 len){
	u32 Address;
	volatile FLASH_Status FLASHStatus;
	wdg();
	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();
	

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	
	/* Erase the FLASH pages */
	FLASHStatus = FLASH_ErasePage(addr);
	wdg();
	
	/*  FLASH Word program of data 0x15041979 at addresses defined by StartAddr and EndAddr*/
	
	for(Address = addr;Address < addr + len*4; Address += 4) {
		wdg();
		FLASHStatus = FLASH_ProgramWord(Address, *pDat++);
		while(FLASHStatus!=FLASH_COMPLETE) ;
	}
	wdg();
}

void VerifyParaList(int * pVal, const int * pValList, int ListNum, int DefaultId)
{
	int i;
	for(i=0;i<ListNum;i++){
		if(*pVal == pValList[i]) return;//found in list
	}
	*pVal = pValList[DefaultId];	//set to first
}
