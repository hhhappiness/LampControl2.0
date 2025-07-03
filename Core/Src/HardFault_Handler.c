#include "Reset.h"
#include "ctrl.h"

///缺省硬件错误中断的替代处理函数，检测到某个条FaultResetCondition()为真后复位。
///FaultResetCondition()条件在Board_config.h中定义，例如按下某个按键或组合键
void HardFault_Handler(void)
{
	while(1){
		wdg();
		if(FaultResetCondition()){
			//Reset(); //用自己写的跳转函数不行，没法切换CPU的中断状态
			SoftReset();
		}
	}
}

void NMI_Handler(void)
{
	while(1){
		wdg();
		if(FaultResetCondition()){
			//Reset(); //用自己写的跳转函数不行，没法切换CPU的中断状态
			SoftReset();
		}
	}
}