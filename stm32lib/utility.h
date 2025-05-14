#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "type.h"

#ifdef __cplusplus
 extern "C" {
#endif 
void U8ToBINStr(u8 Key,char * s,u8 len);
void U32ToDECStr(u32 x,char *s,u8 len,u8 radix,u8 align);
void U32ToHEXStr(u32 x,char *s,u8 len,u8 align);
void U32ToHEXStr8(u32 x,char *s);
void U16ToHEXStr(u16 x,char *s);
void U8ToHEXStr(u16 x,char *s);
int StepS32(int Dat,int x,int Min ,int Max);
u16 StepU16(u16 Dat,int x,u16 Min,u16 Max);
u32 SumU32(void *buf, int len);
u16 SumU16(void *buf, int len);	 
u32 Str2U32(void *Str);
u32 DecStr2U32(void *Str);
u32 HexStr2U32(void *Str);
int Str2S32(void *Str);
float Str2Float(void *src);
int StrCmpNoCase(const char *p1,const char *p2);
int VersonStr2(char *s,u8 len, u8 Major, u8 Minor);
int VersonStr3(char *s,u8 len, u8 Major, u8 Mid, u8 Minor);	 
__inline u32 U32ToDigitalNum(u32 x)
{
	if(x<10)				return 1;
	else if(x<100)			return 2;
	else if(x<1000)			return 3;
	else if(x<10000)		return 4;
	else if(x<100000)		return 5;
	else if(x<1000000)		return 6;
	else if(x<10000000)		return 7;
	else if(x<100000000)	return 8;
	else if(x<1000000000)	return 9;
	else					return 10;	
}	 

__inline u32 U32ToHexNum(u32 x)
{
	if(x<0x10)				return 1;
	else if(x<0x100)		return 2;
	else if(x<0x1000)		return 3;
	else if(x<0x10000)		return 4;
	else if(x<0x100000)		return 5;
	else if(x<0x1000000)	return 6;
	else if(x<0x10000000)	return 7;
	else					return 8;	
}	
#ifdef __cplusplus
 }
#endif 
 
#endif
 
