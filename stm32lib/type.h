#ifndef _TYPE_H_
#define _TYPE_H_

//IAR的stdint.h里以下类型是条件编译的，编译器内部隐含定定义了，导致在eclipse报没定义
#ifndef __INT8_T_TYPE__
  typedef signed char   int8_t;
  typedef unsigned char uint8_t;
#endif /* __INT8_T_TYPE__ */

#ifndef __INT16_T_TYPE__
  typedef signed short   int16_t;
  typedef unsigned short uint16_t;
#endif /* __INT16_T_TYPE__ */

#ifndef __INT32_T_TYPE__
  typedef signed int   int32_t;
  typedef unsigned int uint32_t;
#endif /* __INT32_T_TYPE__ */
  
// 通用自定义类型、宏等
typedef signed char      	s8;
typedef unsigned char	 	u8;
typedef signed short	 	s16;
typedef unsigned short   	u16;
typedef signed int	    s32;
typedef unsigned int	 	u32;
//typedef signed __int64 	 s64;
//typedef unsigned __int64 u64;



typedef unsigned char	BOOL;	//C++中定义了bool,c里没有。定义成大写的为了跟C++的bool区分
#define TRUE			1
#define	FALSE			0

#ifndef NULL
	#define NULL            0
#endif

/*this struct indicate a rectangular area on the LCD screen*/
typedef struct
{
	u16 x1;
	u16 y1;
	u16 x2;
	u16 y2;
}RECT;

typedef struct
{
	u16 x;
	u16 y;
	u16 w;
	u16 h;
}Rect16_t;

typedef struct
{
	u8 x;
	u8 y;
	u8 w;
	u8 h;
}Rect8_t;

//将x,y,w,h转换成{x1,y1,x2,y2}用于RECT的初始化
#define RECT2(x,y,w,h) {x,y,x+w-1,y+h-1}
//分解RECT的无素，用于部分函数的参数调用
#define RECT_ITEM(R) (R).x1,(R).y1,(R).x2,(R).y2  

/*this struct indicate a pixel position on the LCD screen*/
typedef struct
{
	unsigned short x;
	unsigned short y;	
}POINT;

//#pragma anon_unions
typedef union{
    struct {
        u8 Year,Mon,Date,Day,Hour,Min,Sec;//Year:00-99,Date:0-7
    };
    s32 space[2];
}RTC_TIME;

//typedef struct { s64   x, y; } comp;
typedef struct { s16 x, y; } comp16;
typedef struct { s32 x, y; } comp32;
#define PI 3.141592653589793
#define _AT_SECTION(sec) __attribute__ ((section (#sec)))

///四个字符转成int型
#define FOURCHAR2INT(c1,c2,c3,c4) ((u32)(c1)<<24 | (u32)(c2)<<16 | (u32)(c3) << 8 | (u32)(c4))

//判断数组大小
#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) 	(sizeof(x)/sizeof(x[0]))
#endif	

#endif
