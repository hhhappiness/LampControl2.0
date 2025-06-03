#include "aes.h"
///aes测试代码

void TestAES(void)
{

	unsigned char key[16] = { 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0x22, 0x8f, 0x0d, 0xed, 0xaf, 0x56, 0x10, 0xf6 };

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
	aes_gen_tabs();
	aes_setkey(key, 16);

	
	for (i = 0; i<256; i++){
		//原始数据
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		aes_encrypt(encoded,src);//加密
		aes_decrypt(decoded,encoded);//解密
		for (j = 0; j<8; j++){	//校验
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}
