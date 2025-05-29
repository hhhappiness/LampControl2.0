
#include "AppParaCommon.h"
#include "utility.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_flash.h"
#include "string.h"
#include "ctrl_common.h"
#include "flash.h"

volatile u8 NeedSaveFlag = 0;
HAL_StatusTypeDef FlashPara(u32 addr,u32 *pDat,u32 len);
static inline u32 CheckSumPara(void)
{
	return SumU32((u32*)&AppPara, sizeof(APP_PARA)/4 -1);
}

void SaveConfig(void){
	//FlashPara(0x08000000+(u32)&FlashSave,(u32 *)&AppPara,(sizeof(AppPara)+3)/4);
	//FlashPara(0x03fff000, (u32 *)&AppPara,(sizeof(AppPara)+3)/4);
	
	AppPara.Sum = CheckSumPara();
	/* 保存数据 */
    SaveUserData(APP_PARA_ADDRESS,(u32 *)&AppPara,sizeof(AppPara));
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

void Flash_ProgramWord(u32 Address, u32 *Data){
	  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

  /* Set PG bit */
  SET_BIT(FLASH->CR, FLASH_CR_PG);
  
  /* Program first word */
  *(uint32_t *)Address = *Data;
}
//参数表写入Flash，擦除整个Page
void FlashParaOld(u32 addr,u32 *pDat,u32 len){
	#if 0 
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
	#endif
}

HAL_StatusTypeDef FlashPara(u32 addr,u32 *pDat,u32 len){
	u32 Addresses;
	HAL_StatusTypeDef status;
  	uint32_t prog_bit = 0;
	HAL_FLASH_Unlock();
	/* Check the parameters */
	assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));

	/* Process Locked */
	__HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

	if (status == HAL_OK)
	{
		pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

		/* Deactivate the data cache if they are activated to avoid data misbehavior */
		if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
		{
		/* Disable data cache  */
		__HAL_FLASH_DATA_CACHE_DISABLE();
		pFlash.CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
		}
		else
		{
		pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
		}
		
		for(Addresses = addr;Addresses < addr + len*4; Addresses += 4) {
			/* Program double-word (64-bit) at a specified address */
			Flash_ProgramWord(Addresses, pDat++);
				/* Barrier to ensure programming is performed in 2 steps, in right order
				(independently of compiler optimization behavior) */
			__ISB();
			/* Wait for last operation to be completed */
			while(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE));
		}
		prog_bit = FLASH_CR_PG;
		

		/* If the program operation is completed, disable the PG or FSTPG Bit */
		if (prog_bit != 0U)
		{
			CLEAR_BIT(FLASH->CR, prog_bit);
		}

		/* Flush the caches to be sure of the data consistency */
		FLASH_FlushCaches();

		}
		/* Process Unlocked */
	__HAL_UNLOCK(&pFlash);
	HAL_FLASH_Lock();
	/* return status */
	return status;
}
void VerifyParaList(int * pVal, const int * pValList, int ListNum, int DefaultId)
{
	int i;
	for(i=0;i<ListNum;i++){
		if(*pVal == pValList[i]) return;//found in list
	}
	*pVal = pValList[DefaultId];	//set to first
}
