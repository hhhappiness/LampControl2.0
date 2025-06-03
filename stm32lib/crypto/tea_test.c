#include "tea.h"
///tea���Դ���
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
		//ԭʼ����
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		tea_encrypt(encoded,src);//����
		tea_decrypt(decoded,encoded);//����
		for (j = 0; j<8; j++){	//У��
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}
#endif
///xtea���Դ���
void TestXTEA(void)
{

	unsigned char key[XTEA_KEY_SIZE]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
		
	xtea_setkey(key, TEA_KEY_SIZE);

	
	for (i = 0; i<256; i++){
		//ԭʼ����
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		xtea_encrypt(encoded,src);//����
		xtea_decrypt(decoded,encoded);//����
		for (j = 0; j<8; j++){	//У��
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}

///xtea���Դ���
void TestXETA(void)
{

	unsigned char key[XTEA_KEY_SIZE]={0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68, 0xEA, 0x7B, 0x91, 0xDD, 0x5A, 0x3F, 0x2E, 0x68};

	unsigned char src[8];
	unsigned char encoded[8];
	unsigned char decoded[8];

	int i, j;
		
	xtea_setkey(key, TEA_KEY_SIZE);

	
	for (i = 0; i<256; i++){
		//ԭʼ����
		for (j = 0; j<8; j++){
			src[j] = i + j;
		}
		xeta_encrypt(encoded,src);//����
		xeta_decrypt(decoded,encoded);//����
		for (j = 0; j<8; j++){	//У��
			if (src[j] != decoded[j]){
				while (1);
			}
		}


	}
}
