#include "share.h"
#include "fonts.h"
#include "des.h"
#include "tea.h"

ShareFlashInfo_t const ShareInfo __attribute__((section ("SHARE_INFO")))={
	SHARE_ROM_MAGIC, //32 		MaginNum;
	&DEFAULT_ASCII_FONT,//Font_t * 	DefaultAsciiFont;
	xtea_setkey,//int (*setkey)(const u8 *in_key, u32 key_len)
	xeta_encrypt,//void (*encrypt)(u8 *dst, const u8 *src)
	xeta_decrypt,//void (*decrypt)(u8 *dst, const u8 *src)
};
