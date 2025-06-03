#ifndef _TEA_H_
#define _TEA_H_

#include "crypto.h"

#ifdef __cplusplus
 extern "C" {
#endif 

#define TEA_KEY_SIZE		16
int tea_setkey(const u8 *in_key, unsigned int key_len);
void tea_encrypt(u8 *dst, const u8 *src);
void tea_decrypt(u8 *dst, const u8 *src);
void TestTEA(void);

#define XTEA_KEY_SIZE		16
int xtea_setkey(const u8 *in_key, unsigned int key_len);
void xtea_encrypt(u8 *dst, const u8 *src);
void xtea_decrypt(u8 *dst, const u8 *src);
void TestXTEA(void);

void xeta_encrypt(u8 *dst, const u8 *src);
void xeta_decrypt(u8 *dst, const u8 *src);
void TestXETA(void);

#ifdef __cplusplus
 }
#endif 

#endif//_TEA_H_
