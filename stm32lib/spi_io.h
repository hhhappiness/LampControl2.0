#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#include "stm32.h"
void SPIIO_Init(void);
u8 SPIIO_RW8(u8 data);

#ifdef __cplusplus
}
#endif 
#endif

