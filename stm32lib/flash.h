/**
 * @file flash.h
 * @brief STM32G4系列Flash用户数据存储头文件
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_flash.h"

/* Flash存储参数定义 */
#define FLASH_USER_START_ADDR   0x0801F000   /* Flash起始地址 */
#define FLASH_USER_END_ADDR     0x0801FFFF   /* Flash结束地址 */

/* 函数声明 */
u8 SecureApplication();
FlagStatus FLASH_GetReadOutProtectionStatus(void);
HAL_StatusTypeDef EraseUserDataFlash(u32 addr);

HAL_StatusTypeDef WriteUserDataToFlash(u32 addr, u32 *data, u32 len);
void SaveUserData(u32 addr, u32* data, u32 len);

/**
 * @brief Flash存储使用示例
 */
void FlashStorageExample(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H__ */
