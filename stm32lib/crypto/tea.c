/* 
 * Cryptographic API.
 *
 * TEA, XTEA, and XETA crypto alogrithms
 *
 * The TEA and Xtended TEA algorithms were developed by David Wheeler 
 * and Roger Needham at the Computer Laboratory of Cambridge University.
 *
 * Due to the order of evaluation in XTEA many people have incorrectly
 * implemented it.  XETA (XTEA in the wrong order), exists for
 * compatibility with these implementations.
 *
 * Copyright (c) 2004 Aaron Grothe ajgrothe@yahoo.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "tea.h"

#define TEA_BLOCK_SIZE		8
#define TEA_ROUNDS		32
#define TEA_DELTA		0x9e3779b9

#define XTEA_BLOCK_SIZE		8
#define XTEA_ROUNDS		32
#define XTEA_DELTA		0x9e3779b9

#if 0
static u32 tea_KEY[4];
#endif
static u32 xtea_KEY[4];

#if 0
int tea_setkey(const u8 *in_key, unsigned int key_len)
{
	const __le32 *key = (const __le32 *)in_key;
	
	if (key_len != 16)
	{
//		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	tea_KEY[0] = le32_to_cpu(key[0]);
	tea_KEY[1] = le32_to_cpu(key[1]);
	tea_KEY[2] = le32_to_cpu(key[2]);
	tea_KEY[3] = le32_to_cpu(key[3]);

	return 0; 

}

void tea_encrypt(u8 *dst, const u8 *src)
{
	u32 y, z, n, sum = 0;
	u32 k0, k1, k2, k3;
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	k0 = tea_KEY[0];
	k1 = tea_KEY[1];
	k2 = tea_KEY[2];
	k3 = tea_KEY[3];

	n = TEA_ROUNDS;

	while (n-- > 0) {
		sum += TEA_DELTA;
		y += ((z << 4) + k0) ^ (z + sum) ^ ((z >> 5) + k1);
		z += ((y << 4) + k2) ^ (y + sum) ^ ((y >> 5) + k3);
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}

void tea_decrypt(u8 *dst, const u8 *src)
{
	u32 y, z, n, sum;
	u32 k0, k1, k2, k3;
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	k0 = tea_KEY[0];
	k1 = tea_KEY[1];
	k2 = tea_KEY[2];
	k3 = tea_KEY[3];

	sum = TEA_DELTA << 5;

	n = TEA_ROUNDS;

	while (n-- > 0) {
		z -= ((y << 4) + k2) ^ (y + sum) ^ ((y >> 5) + k3);
		y -= ((z << 4) + k0) ^ (z + sum) ^ ((z >> 5) + k1);
		sum -= TEA_DELTA;
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}
#endif
int xtea_setkey(const u8 *in_key, unsigned int key_len)
{
	const __le32 *key = (const __le32 *)in_key;
	
	if (key_len != 16)
	{
//		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	xtea_KEY[0] = le32_to_cpu(key[0]);
	xtea_KEY[1] = le32_to_cpu(key[1]);
	xtea_KEY[2] = le32_to_cpu(key[2]);
	xtea_KEY[3] = le32_to_cpu(key[3]);

	return 0; 

}

void xtea_encrypt(u8 *dst, const u8 *src)
{
	u32 y, z, sum = 0;
	u32 limit = (u32)(XTEA_DELTA * XTEA_ROUNDS);//有溢出
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	while (sum != limit) {
		y += ((z << 4 ^ z >> 5) + z) ^ (sum + xtea_KEY[sum&3]); 
		sum += XTEA_DELTA;
		z += ((y << 4 ^ y >> 5) + y) ^ (sum + xtea_KEY[sum>>11 &3]); 
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}

void xtea_decrypt(u8 *dst, const u8 *src)
{
	u32 y, z, sum;
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	sum = (u32)(XTEA_DELTA * XTEA_ROUNDS);//有溢出

	while (sum) {
		z -= ((y << 4 ^ y >> 5) + y) ^ (sum + xtea_KEY[sum>>11 & 3]);
		sum -= XTEA_DELTA;
		y -= ((z << 4 ^ z >> 5) + z) ^ (sum + xtea_KEY[sum & 3]);
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}

void xeta_encrypt(u8 *dst, const u8 *src)
{
	u32 y, z, sum = 0;
	u32 limit = (u32)(XTEA_DELTA * XTEA_ROUNDS);//有溢出
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	while (sum != limit) {
		y += (z << 4 ^ z >> 5) + (z ^ sum) + xtea_KEY[sum&3];
		sum += XTEA_DELTA;
		z += (y << 4 ^ y >> 5) + (y ^ sum) + xtea_KEY[sum>>11 &3];
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}

void xeta_decrypt(u8 *dst, const u8 *src)
{
	u32 y, z, sum;
	const __le32 *in = (const __le32 *)src;
	__le32 *out = (__le32 *)dst;

	y = le32_to_cpu(in[0]);
	z = le32_to_cpu(in[1]);

	sum = (u32)(XTEA_DELTA * XTEA_ROUNDS);//有溢出

	while (sum) {
		z -= (y << 4 ^ y >> 5) + (y ^ sum) + xtea_KEY[sum>>11 & 3];
		sum -= XTEA_DELTA;
		y -= (z << 4 ^ z >> 5) + (z ^ sum) + xtea_KEY[sum & 3];
	}
	
	out[0] = cpu_to_le32(y);
	out[1] = cpu_to_le32(z);
}

