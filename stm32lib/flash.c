/**
 * @file flash.c
 * @brief STM32G4系列Flash用户数据存储示例
 */

#include "flash.h"
#include "utility.h"
#include "main.h"


/* 用户数据结构体示例 */
typedef struct {
    uint32_t parameter1;
    uint32_t parameter2;
    float    parameter3;
    uint8_t  parameter4[16];
    uint32_t crc;              /* 校验和 */
} UserData_t;


/**
 * @brief 启用Flash读保护
 * @param level: 读保护等级 (OB_RDP_LEVEL_0, OB_RDP_LEVEL_1, OB_RDP_LEVEL_2)
 * @return HAL状态
 * @note 设置Level 2保护后无法通过编程方式取消保护，请谨慎使用！
 */
HAL_StatusTypeDef FLASH_SetReadProtection(uint32_t level)
{
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_OBProgramInitTypeDef OBInit;
    
    /* 解锁Flash和选项字节 */
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }
    
    status = HAL_FLASH_OB_Unlock();
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return status;
    }
    
    /* 设置读保护级别 */
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel = level;
    
    /* 编程选项字节 */
    status = HAL_FLASHEx_OBProgram(&OBInit);
    if (status != HAL_OK) {
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        return status;
    }
    
    /* 启动加载选项字节 */
    status = HAL_FLASH_OB_Launch();
    
    /* 锁定选项字节和Flash */
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
    
    return status;
}

/**
 * @brief 获取Flash读保护状态
 * @return SET: 启用了读保护; RESET: 未启用读保护
 */
FlagStatus FLASH_GetReadOutProtectionStatus(void)
{
    uint32_t rdp_level = READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP);
    
    if (rdp_level == OB_RDP_LEVEL_0) {
        return RESET;  // 无读保护
    } else {
        return SET;    // 有读保护
    }
}

u8 SecureApplication(void)
{
    #if 0
    /* 检查是否已经启用了读保护 */
    if (FLASH_GetReadOutProtectionStatus() == RESET) {
        /* 没有读保护，启用Level 1保护 */
        if (HAL_FLASH_OB_Unlock() == HAL_OK) {
            //开始设置读保护
            FLASH_SetReadProtection(OB_RDP_LEVEL_1);
            //读保护已经设置，设备将重启
        } else {
            return 0;  //RESET
            //无法解锁选项字节
        }
    } else {
       //读保护已经启用
    }
    return 1;  //SET
#endif
    return 0;
    /* 应用程序继续执行... */
}

/**
 * @brief 计算数据的CRC校验和
 * @param data 数据指针
 * @param length 数据长度(字节)
 * @return CRC值
 */
static uint32_t CalculateCRC(const uint8_t *data, uint32_t length)
{
     // CRC-32查找表
    static const uint32_t crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    uint32_t crc = 0xFFFFFFFF;
    
    // 使用查表法计算CRC
    while (length--) {
        crc = (crc >> 8) ^ crc_table[(crc ^ *data++) & 0xFF];
    }
    
    return ~crc;
}

/**
 * @brief 擦除用户数据区Flash页
 * @return HAL状态
 */
HAL_StatusTypeDef EraseUserDataFlash(u32 addr)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t page_error = 0;
    FLASH_EraseInitTypeDef erase_init;
    
    /* 验证地址范围 */
    if (addr < FLASH_USER_START_ADDR || addr >= FLASH_USER_END_ADDR) {
        return HAL_ERROR;
    }
    
    /* 计算页号 (使用传入的地址) */
    uint32_t page_number = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    
    /* 配置擦除参数 */
    erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase_init.Banks       = FLASH_BANK_1; /* 根据实际地址选择Bank */
    erase_init.Page        = page_number;
    erase_init.NbPages     = 1; /* 擦除1页 */
    
    /* 解锁Flash */
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return status;
    }
    
    /* 清除所有Flash标志位 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    
    /* 执行擦除 */
    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    /* 锁定Flash，无论擦除是否成功 */
    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();
    
    /* 如果擦除失败则返回擦除错误状态，否则返回锁定状态 */
    return (status != HAL_OK) ? status : lock_status;
}


/**
 * @brief 将用户数据写入Flash
 * @param data 用户数据结构体指针
 * @return HAL状态
 */
HAL_StatusTypeDef WriteUserDataToFlash(u32 addr, u32 *data, u32 len)
{
    HAL_StatusTypeDef status;
    
    /* 先擦除Flash页 */
    status = EraseUserDataFlash(addr);
    if (status != HAL_OK)
    {
        return status;
    }
    
    /* 解锁Flash */
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return status;
    }
    
    /* 清除所有Flash标志位 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    
    /* 计算64位双字的数量 */
    uint32_t data_length_64 = (len + 7) / 8;
    uint64_t *source_addr = (uint64_t *)data;
    uint32_t address = addr;
    
    /* 使用快速内存访问写入数据 */
    for (uint32_t i = 0; i < data_length_64; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, source_addr[i]);
        if (status != HAL_OK)
        {
            return status; /* 如果编程失败，立即返回错误 */
        }
        address += 8; /* 移动到下一个64位位置 */
    }
    
    /* 始终锁定Flash，无论是否成功 */
    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();
    
    /* 如果编程过程中出错，返回编程错误；否则返回锁定状态 */
    return (status != HAL_OK) ? status : lock_status;
}



/**
 * @brief 保存用户数据到Flash
 * @param addr 自定义地址
 * @param data 用户数据指针
 * @param len 数据长度
 */
void SaveUserData(u32 addr,u32* data,u32 len)
{
    u32 user_data_crc,read_data_crc;
    u32 *read_data;
    /* 计算CRC ,改为累计和*/
    user_data_crc = SumU32(data, len/4 -1);//CalculateCRC((uint8_t*)data, len - sizeof(uint32_t));
    
    /* 写入Flash */
    if (WriteUserDataToFlash(addr,data,len) != HAL_OK)
    {
        /* 错误处理 */
        Error_Handler();
    }
    
    /* 读取数据进行校验 */
    /* 直接从Flash地址读取数据 */
    // memcpy(read_data, (void*)addr, len);
    
    // /* 验证CRC ,改为累计和*/
    // read_data_crc = SumU32(read_data, len/4 -1);//CalculateCRC((uint8_t*)data, len - sizeof(uint32_t));
    // if (user_data_crc != read_data_crc)
    // {
    //     /* CRC校验失败，处理错误 */
    //     Error_Handler();
    // }
}

