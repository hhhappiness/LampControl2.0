#ifndef _KET_H_
#define _KET_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#include "board_config.h"
#include "Key_Base.h"



#define KEY_DIV2_SHOT		3 //3
#define KEY_DIV2_LONG		(KEY_DIV2_SHOT + 0x40)
#define KEY_MULT_SHOT		1
#define KEY_MULT_LONG		(KEY_MULT_SHOT + 0x40)
#define KEY_DOWN_SHOT		4
#define KEY_DOWN_LONG		(KEY_DOWN_SHOT + 0x40)
#define KEY_ENTER_SHOT		2
#define KEY_ENTER_LONG		(KEY_ENTER_SHOT + 0x40)
#define KEY_UP_SHOT			5
#define KEY_UP_LONG			(KEY_UP_SHOT + 0x40)
#define KEY_POWER_SHOT		6
#define KEY_POWER_LONG		(KEY_POWER_SHOT + 0x40)
#define KEY_ENCODER         7

#define KEY_DIV2_RELEASE	(KEY_DIV2_SHOT + 0x20)
#define KEY_MULT_RELEASE 	(KEY_MULT_SHOT + 0x20)
#define KEY_UP_RELEASE		(KEY_UP_SHOT + 0x20)
#define KEY_DOWN_RELEASE 	(KEY_DOWN_SHOT + 0x20)
#define KEY_ENTER_RELEASE	(KEY_ENTER_SHOT + 0x20)  


#define KEY_DIV2_LONG_RELEASE	(KEY_DIV2_SHOT + KEY_LONG_RELEASE)
#define KEY_MULT_LONG_RELEASE 	(KEY_MULT_SHOT + KEY_LONG_RELEASE)
#define KEY_UP_LONG_RELEASE		(KEY_UP_SHOT + KEY_LONG_RELEASE)
#define KEY_DOWN_LONG_RELEASE 	(KEY_DOWN_SHOT + KEY_LONG_RELEASE)
#define KEY_ENTER_LONG_RELEASE	(KEY_ENTER_SHOT + KEY_LONG_RELEASE)  




#ifdef __cplusplus
 }
#endif 

#endif//_KET_H_

