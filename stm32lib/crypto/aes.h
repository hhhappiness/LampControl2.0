#ifndef _AES_H_
#define _AES_H_

#include "crypto.h"

#ifdef __cplusplus
 extern "C" {
#endif 

void aes_gen_tabs (void);
int aes_setkey(const u8 *in_key, unsigned int key_len);
void aes_encrypt(u8 *out, const u8 *in);
void aes_decrypt(u8 *out, const u8 *in);
void TestAES(void);


#ifdef __cplusplus
 }
#endif 

#endif//_AES_H_
