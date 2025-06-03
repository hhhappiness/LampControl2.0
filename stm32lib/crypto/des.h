#ifndef _DES_H_
#define _DES_H_

#include "crypto.h"
#ifdef __cplusplus
 extern "C" {
#endif 

#define DES_KEY_SIZE		8
#define DES_EXPKEY_WORDS	32
#define DES_BLOCK_SIZE		8
int des_setkey(const u8 *key, unsigned int key_len);
void des_encrypt(u8 *dst, const u8 *src);
void des_decrypt(u8 *dst, const u8 *src);

#ifdef DES3
#define DES3_EDE_KEY_SIZE	(3 * DES_KEY_SIZE)
#define DES3_EDE_EXPKEY_WORDS	(3 * DES_EXPKEY_WORDS)
#define DES3_EDE_BLOCK_SIZE	DES_BLOCK_SIZE

int des3_ede_setkey(const u8 *key, unsigned int key_len);
void des3_ede_encrypt(u8 *dst, const u8 *src);
void des3_ede_decrypt(u8 *dst, const u8 *src);

#endif
void TestDES(void);
#ifdef __cplusplus
 }
#endif 

#endif//_DES_H_



