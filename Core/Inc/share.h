#ifndef _SHARE_H_
#define _SHARE_H_

#include "type.h"
#include "fontbase.h"

#ifdef __cplusplus
 extern "C" {
#endif 
 
 #define SHARE_ROM_MAGIC	0x948b7a4b
typedef struct{
	u32 		MaginNum;
	const Font_t * 	DefaultAsciiFont;
	int (*setkey)(const u8 *in_key, u32 key_len);
	void (*encrypt)(u8 *dst, const u8 *src);
	void (*decrypt)(u8 *dst, const u8 *src);
	u32 Reserved[8-5];
}ShareFlashInfo_t;
 
extern const ShareFlashInfo_t ShareInfo;
#ifdef __cplusplus
 }
#endif 

#endif//_SHARE_H_
