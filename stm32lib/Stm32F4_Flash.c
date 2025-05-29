#include "stm32.h"
#include "stm32f4_flash.h"
#include "stm32g4xx_hal_flash.h"
#include "stm32g4xx_hal_flash_ex.h"

#define FLASH_TIMEOUT_VALUE       ((uint32_t)5000)
/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t Flash_GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;  
  }
#ifdef FLASH_SECTOR_23  
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;  
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;  
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;  
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;  
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;  
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;  
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;  
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;  
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;  
  } 
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;  
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_23;  
  }
#endif
  return sector;
}

int Flash_EraseSectorAtAddress(uint32_t start_addr, uint32_t size)
{
	uint32_t FirstSector, NbOfSectors;
	uint32_t SectorError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	/* Get the 1st sector to erase */
	FirstSector = Flash_GetSector(start_addr);
	/* Get the number of sector to erase from 1st sector*/
	NbOfSectors = Flash_GetSector(start_addr+size-1) - FirstSector + 1;

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;
	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{ 
	  /*
		FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
	  */
		//Error_Handler();
		return -1;
	}
	return HAL_OK;
}

HAL_StatusTypeDef Flash_Program_SingleWord(uint32_t Address, uint32_t Data)
{
	HAL_StatusTypeDef status = HAL_ERROR;

	/* Process Locked */
//  __HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

	if(status == HAL_OK)
	{
		/*Program word (32-bit) at a specified address.*/
		FLASH->CR |= FLASH_CR_PG;

		*(__IO uint32_t*)Address = Data;

		/* Wait for last operation to be completed */
		status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

		/* If the program operation is completed, disable the PG Bit */
		FLASH->CR &= (~FLASH_CR_PG);
	}

	/* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);

	return status;
}

HAL_StatusTypeDef Flash_Program_SingleByte(uint32_t Address, uint8_t Data)
{
	HAL_StatusTypeDef status = HAL_ERROR;

	/* Process Locked */
//  __HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

	if(status == HAL_OK)
	{
		/*Program word (32-bit) at a specified address.*/
		FLASH->CR |= FLASH_CR_PG;

		*(__IO uint8_t*)Address = Data;

		/* Wait for last operation to be completed */
		status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

		/* If the program operation is completed, disable the PG Bit */
		FLASH->CR &= (~FLASH_CR_PG);
	}

	/* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);

	return status;
}


HAL_StatusTypeDef Flash_Program_Words(uint32_t Address, uint32_t *Data, int DataLen)
{
	HAL_StatusTypeDef status = HAL_ERROR;
//	if((DataLen & 0x3) != 0) return HAL_ERROR;
	/* Process Locked */
//  __HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if(status != HAL_OK) return status;	
	FLASH->CR |= FLASH_CR_PG;
	while(DataLen>0)//当长度不是的整数倍时，会小于0。
	{
		*(__IO uint32_t*)Address = *Data++;
		Address+=4;
		DataLen-= 4;
		/* Wait for last operation to be completed */
		status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);	
	}
	FLASH->CR &= (~FLASH_CR_PG);
	/* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);

	return status;	
}

HAL_StatusTypeDef Flash_Program_Bytes(uint32_t Address, uint8_t *Data, int DataLen)
{
	HAL_StatusTypeDef status = HAL_ERROR;
//	if((DataLen & 0x3) != 0) return HAL_ERROR;
	/* Process Locked */
//  __HAL_LOCK(&pFlash);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if(status != HAL_OK) return status;	
	FLASH->CR |= FLASH_CR_PG;
	while(DataLen>0)//当长度不是的整数倍时，会小于0。
	{
		*(__IO uint8_t*)Address = *Data++;
		Address++;
		DataLen--;
		/* Wait for last operation to be completed */
		status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);	
	}
	FLASH->CR &= (~FLASH_CR_PG);
	/* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);

	return status;	
}

#include "type.h"

void TestFlash()
{
	uint32_t i,j;
	uint32_t TestAddr = 0x08060000;
	uint32_t TestSize = 0x200;
	uint32_t Addr;
	uint32_t Buf[16];
	HAL_FLASH_Unlock();
	Flash_EraseSectorAtAddress(TestAddr, TestSize);
							   
	Addr = 	TestAddr;					   
	for(i=0; i<TestSize/4; )
	{
		for(j=0; j< ARRAY_SIZE(Buf); j++){
			Buf[j]=i++;
		}
		if (Flash_Program_Words(Addr, Buf, sizeof(Buf)) == HAL_OK)
		{
			Addr = Addr + sizeof(Buf);
		}
		else
		{ 
		/* Error occurred while writing data in Flash memory. 
		 User can add here some code to deal with this error */
		/*
		  FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
		*/
		//Error_Handler();
			while(1);
		}		
	}
	HAL_FLASH_Lock(); 
}