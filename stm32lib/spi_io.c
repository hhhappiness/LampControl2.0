#include "spi_io.h"

#include "stm32.h"
#include "bsp.h"
#include "board_config.h"


#define SPIIO_CS(x)	LCD_CSB_Pin_O(x) 
#define SPIIO_MOSI(x)	LCD_SDA_Pin_O(x)
#define SPIIO_SCLK(x)	LCD_SCL_Pin_O(x)
#define SPIIO_MISO	GPA_I(6)

#if 0
u8 SPIIO_RW8(u8 data)
{
	  u8  i = 0,tmp = 0;   
        SPIIO_SCLK = 0;
	  for (i=0;i<8;i++)   
	  {   
	    if ((data & 0x80) != 0) 
			SPIIO_MOSI = 1;  
	    else 
			SPIIO_MOSI = 0;   
	    __nop();__nop();__nop();__nop();__nop();
	//	__nop();__nop();__nop();__nop();__nop();
	    SPIIO_SCLK = 1;   
	    data = (data << 1);   
	    /*
	    tmp = tmp<<1;   
	    if (SPIIO_MISO)  
			tmp |= 0x01;
			*/
	    SPIIO_SCLK = 0;   
	   __nop();__nop();__nop();__nop();__nop();
	   //__nop();__nop();__nop();__nop();__nop();
	  }   
	  return tmp;  
}
#endif

void SPIIO_Init(void)
{
	 SPIIO_SCLK = 1;
	 SPIIO_MOSI = 1;  
}


u8 SPIIO_RW8(u8 data)
{
	  u8  i = 0,tmp = 0;   
        SPIIO_SCLK = 1;
	  for (i=0;i<8;i++)   
	  {   
	    if ((data & 0x80) != 0) 
			SPIIO_MOSI = 1;  
	    else 
			SPIIO_MOSI = 0;   
	    //__nop();__nop();__nop();__nop();__nop();
	    SPIIO_SCLK = 0;   
	    data = (data << 1);   
	    /*
	    tmp = tmp<<1;   
	    if (SPIIO_MISO)  
			tmp |= 0x01;
			*/
	   __nop();__nop();__nop();__nop();__nop();
	   SPIIO_SCLK = 1;   
	  }   
	  return tmp;  
}

