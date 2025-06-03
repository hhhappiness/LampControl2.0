#include "des.h"
///des测试代码
void TestDES(void)
{
#ifndef PREDEFINED_DESKEY	
	unsigned char key[8]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};
#endif	
	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
#ifndef PREDEFINED_DESKEY		
	des_setkey(key, DES_KEY_SIZE);
#endif
	
	for (i = 0; i<256; i++){
		//原始数据
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		des_encrypt(encoded,src);//加密
		des_decrypt(decoded,encoded);//解密
		for (j = 0; j<8; j++){	//校验
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}