#ifndef _SPI_MASTER_H_
#define _SPI_MASTER_H_

#ifdef __cplusplus
 extern "C" {
#endif 
void SPIMasterInit(void);
void SPIMasterReInit(uint16_t psc);
void Spi_WriteByte(unsigned char dat);

#ifdef __cplusplus
}
#endif 
#endif
