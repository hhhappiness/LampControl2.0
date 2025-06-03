#include "des.h"
///des���Դ���
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
		//ԭʼ����
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		des_encrypt(encoded,src);//����
		des_decrypt(decoded,encoded);//����
		for (j = 0; j<8; j++){	//У��
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}