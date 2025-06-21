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

/**
 * @brief 获取Flash读保护状态
 * @return FlagStatus 读保护状态 (SET: 有读保护, RESET: 无读保护)
 */
FlagStatus FLASH_GetReadOutProtectionStatus(void);

/**
 * @brief 擦除用户数据区Flash页
 * @param addr Flash地址
 * @return HAL_StatusTypeDef HAL状态
 */
HAL_StatusTypeDef EraseUserDataFlash(u32 addr);

/**
 * @brief 将用户数据写入Flash
 * @param addr 目标地址
 * @param data 数据指针
 * @param len 数据长度(字节)
 * @return HAL_StatusTypeDef HAL状态
 */
HAL_StatusTypeDef WriteUserDataToFlash(u32 addr, u32 *data, u32 len);

/**
 * @brief 保存用户数据到Flash
 * @param addr 自定义地址
 * @param data 用户数据指针
 * @param len 数据长度(字节)
 */
void SaveUserData(u32 addr, u32* data, u32 len);

/**
 * @brief Flash存储使用示例
 */
void FlashStorageExample(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H__ */
