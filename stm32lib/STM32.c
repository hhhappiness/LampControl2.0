#include "stm32.h"

//set flash latency depend on AHBClk
void SetFlashLatency(){
    if(AHBClk<=24000000) {
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS | FLASH_ACR_LATENCY_0;
    }else if(AHBClk<=48000000){
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS | FLASH_ACR_LATENCY_1;
    }else{
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS | FLASH_ACR_LATENCY_2;
    }
}

//switch clock to HSI
__INLINE void ClockSwith_HSI(void){
    if((RCC->CFGR & RCC_CFGR_SW)!=RCC_CFGR_SW_HSI) {//�����ǰ����HSI
        if(!(RCC->CR & (RCC_CR_HSION|RCC_CR_HSIRDY))) {//if HSI not on
            RCC->CR |= RCC_CR_HSION;      //��HSI
            while(!(RCC->CR & RCC_CR_HSIRDY));    //ȷ��HSI�ȶ�
        }
        RCC->CFGR &= ~RCC_CFGR_SW;    //�л���HSI
        while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SW_HSI<<2));//ȷ���л�
    }
}

/*
__INLINE void ClockSwitch_HSE2HSI()
{
	ClockSwith_HSI();
	//SetFlashLatency();
	//reconfig AHB,APB1,APB2
	//Dont't Need,Because HSE use  1:1:1
	//disable HSE
	RCC->CR &= ~RCC_CR_HSEON;
	
}
*/


void ClockSwitch_72M2HSI(void)
{
	ClockSwith_HSI();
	SetFlashLatency();
	//reconfig AHB,APB1,APB2
	//Dont't Need,Because HSE use  1:1:1
	//disable HSE
	RCC->CR &= ~RCC_CR_PLLON;
	RCC->CR &= ~RCC_CR_HSEON;	
}


//#define ClockSwitch_HSI272M() SetSysClockTo72()





//��ʱ����HSIת��PLL
__INLINE void ClockSwith_HSI2HSEPLL(){
    RCC->CR &= ~RCC_CR_PLLON;//��PLL���ܵ���PLL��֮ǰ���뽫ʱ���е�HSI
    RCC->CFGR = RCC_CFGR_PLLSRC //sourceΪHSE
            | RCC_CFGR_PLLMULLx 
            | RCC_CFGR_HPRE_DIV 
            | RCC_CFGR_PPRE2_DIV
            | RCC_CFGR_PPRE1_DIV;

    RCC->CR |= RCC_CR_PLLON;//set pll on
    while(!(RCC->CR & RCC_CR_PLLRDY));/*!< Wait till PLL is ready */
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL; //swith clock to PLL
    while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SW_PLL<<2));//ȷ���л�
}

__INLINE void ClockSwith_HSI2HSIPLL(){
    RCC->CR &= ~RCC_CR_PLLON;//��PLL���ܵ���PLL��֮ǰ���뽫ʱ���е�HSI
    RCC->CFGR = 0 //sourceΪHSI/2
            | RCC_CFGR_PLLMULLx 
            | RCC_CFGR_HPRE_DIV 
            | RCC_CFGR_PPRE2_DIV
            | RCC_CFGR_PPRE1_DIV;

    RCC->CR |= RCC_CR_PLLON;//set pll on
    while(!(RCC->CR & RCC_CR_PLLRDY));/*!< Wait till PLL is ready */
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL; //swith clock to PLL
    while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SW_PLL<<2));//ȷ���л�
}

//ʹ��HSE+PLL����ʱ��
void ClockInit_HSE_PLL ()
{
    ClockSwith_HSI();
    RCC->CIR = 0x00000000;//���ж�
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);  //��HSE
    while(!(RCC->CR & RCC_CR_HSERDY));    //ȷ��HSE�ȶ�
    SetFlashLatency();
    
    ClockSwith_HSI2HSEPLL();
}

//ʹ��HSE+PLL����ʱ��
void ClockInit_HSEBypass_PLL ()
{
    ClockSwith_HSI();
    RCC->CIR = 0x00000000;//���ж�
    RCC->CR |= (RCC_CR_HSEON | RCC_CR_HSEBYP);  //��HSE,Bypass Mode
    while(!(RCC->CR & RCC_CR_HSERDY));    //ȷ��HSE�ȶ�
    SetFlashLatency();
    
    ClockSwith_HSI2HSEPLL();
}

__INLINE void ClockSwith_HSI2HSE(){
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE; 
      /*!< Wait till PLL is used as system clock source */
    while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SW_HSE<<2));
}

//ʹ��HSE����ʱ��
void ClockInit_HSE ()
{
    ClockSwith_HSI();
    RCC->CIR = 0x00000000;//���ж�
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);  //��HSE
    while(!(RCC->CR & RCC_CR_HSERDY));    //ȷ��HSE�ȶ�
    SetFlashLatency();
    
    ClockSwith_HSI2HSE();
}

void ClockInit_HSI ()
{
	ClockSwith_HSI();
	SetFlashLatency();
}


int ClockSwitch_HSI272M(void)
{
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;

	if((RCC->CFGR & RCC_CFGR_SWS) == (RCC_CFGR_SW_PLL<<2))
		return -1;

	#if 1
  
	  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
	  /* Enable HSE */    
	  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	 
	  /* Wait till HSE is ready and if Time out is reached exit */
	  do
	  {
	    HSEStatus = RCC->CR & RCC_CR_HSERDY;
	    StartUpCounter++;  
	  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	  {
	    HSEStatus = (uint32_t)0x01;
	  }
	  else
	  {
	    HSEStatus = (uint32_t)0x00;
	  }  

	  if (HSEStatus == (uint32_t)0x01)
	  {
	    /* Enable Prefetch Buffer */
	    FLASH->ACR |= FLASH_ACR_PRFTBE;

	    /* Flash 2 wait state */
	    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
	    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

	 
	    /* HCLK = SYSCLK */
	    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
	      
	    /* PCLK2 = HCLK */
	    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
	    
	    /* PCLK1 = HCLK */
	    //RCC->CFGR &= ~(0x7 << 8);
	    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV;//RCC_CFGR_PPRE1_DIV2;

	    /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
	    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
	                                        RCC_CFGR_PLLMULL));
	    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

	    /* Enable PLL */
	    RCC->CR |= RCC_CR_PLLON;

	    /* Wait till PLL is ready */
	    while((RCC->CR & RCC_CR_PLLRDY) == 0)
	    {
	    }
	    
	    /* Select PLL as system clock source */
	    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

	    /* Wait till PLL is used as system clock source */
	    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
	    {
	    }
	  }
	  else
	  { /* If HSE fails to start-up, the application will have wrong clock 
	         configuration. User can add here some code to deal with this error */
	  }
	  #endif

	  //ClockSwith_HSI2PLL();

	  return  0;
}

void Delay_100ms(int i)
{
    for(;i>0;i--) {
        Delay_ms(100);
    }
}

///��ʼ���жϣ�����⺯�����NVIC_Init(), ʡ������ṹ�壬����ֱ��ʹ��(���������¿�������)
void InitINT(uint8_t NVIC_IRQChannel, uint8_t Priority, uint8_t SubPriority)
{
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;

	/* Check the parameters */
	assert_param(IS_NVIC_PREEMPTION_PRIORITY(Priority));  
	assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
    
    /* Compute the Corresponding IRQ Priority --------------------------------*/    
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (uint32_t)Priority << tmppre;
    tmppriority |=  SubPriority & tmpsub;
    tmppriority = tmppriority << 0x04;
        
    NVIC->IP[NVIC_IRQChannel] = tmppriority;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
	EnableINT(NVIC_IRQChannel);
}

///��ʼ���ж����ȼ������ݲ�ʹ��
void InitINTDisabled(uint8_t NVIC_IRQChannel, uint8_t Priority, uint8_t SubPriority)
{
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;

	/* Check the parameters */
	assert_param(IS_NVIC_PREEMPTION_PRIORITY(Priority));  
	assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
	
    /* Disable the Selected IRQ Channels --------------------------------------*/
	DisableINT(NVIC_IRQChannel);
	
    /* Compute the Corresponding IRQ Priority --------------------------------*/    
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (uint32_t)Priority << tmppre;
    tmppriority |=  SubPriority & tmpsub;
    tmppriority = tmppriority << 0x04;
        
    NVIC->IP[NVIC_IRQChannel] = tmppriority;
}
