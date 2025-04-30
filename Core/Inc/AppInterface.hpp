#ifndef _APP_INTERFACE_H_
#define _APP_INTERFACE_H_

#include "type.h"

///本模块定义在GUI中调用的应用程序接口，大多是参数表值的变化后执行的操作

void OnSysIdle(void);
void OnChangePulseWidth_Auto(void);
void OnChangePulseWidth_LED(void);
void OnChangeTrigMode(void);
//void OnChangeTrigMode(int new_val);
bool OnChangeSpeedUnit(void);

bool OnChangeContrast(int new_val);

bool OnChangeFreq(int new_val);
bool OnChangeRpm(int new_val);
bool OnChangeLineSpeed(int new_val);
bool OnChangePulseWidth_LED(int new_val);
bool OnChange_Delay_SinglePulse(int new_val);

bool OnOkPlateLen(int new_val);
bool OnOkImagePerPlate(int new_val);
bool OnOkPowerKey(int new_val);
bool OnOkWorkTime(int new_val);
bool OnOkPowerOffDly(int new_val);


void OnIdleMainPage(u32 &TReg);
void OnIdleMenuPage(u32 &TReg);
void OnIdleSubPage(u32 &TReg);
void OnIdleEditPage(u32 &TReg);


bool OnChange_Perimeter(int new_val);
bool OnChange_PrintLength(int new_val);

bool OnChangeMaxPower(int new_val);



#endif//_APP_INTERFACE_H_

