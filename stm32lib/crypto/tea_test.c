#include "tea.h"
///tea测试代码
#if 0
void TestTEA(void)
{

	unsigned char key[TEA_KEY_SIZE]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
		
	tea_setkey(key, TEA_KEY_SIZE);

	
	for (i = 0; i<256; i++){
		//原始数据
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		tea_encrypt(encoded,src);//加密
		tea_decrypt(decoded,encoded);//解密
		for (j = 0; j<8; j++){	//校验
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}
#endif
///xtea测试代码
void TestXTEA(void)
{

	unsigned char key[XTEA_KEY_SIZE]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
		
	xtea_setkey(key, TEA_KEY_SIZE);

	
	for (i = 0; i<256; i++){
		//原始数据
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		xtea_encrypt(encoded,src);//加密
		xtea_decrypt(decoded,encoded);//解密
		for (j = 0; j<8; j++){	//校验
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}

///xtea测试代码
void TestXETA(void)
{

	unsigned char key[XTEA_KEY_SIZE]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
		
	xtea_setkey(key, TEA_KEY_SIZE);

	
	for (i = 0; i<256; i++){
		//原始数据
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		xeta_encrypt(encoded,src);//加密
		xeta_decrypt(decoded,encoded);//解密
		for (j = 0; j<8; j++){	//校验
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}
