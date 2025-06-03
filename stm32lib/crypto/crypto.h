#ifndef _TYPE_H_
#define _TYPE_H_
// 通用自定义类型、宏等
typedef signed char      s8;
typedef unsigned char	 u8;
typedef signed short	 s16;
typedef unsigned short   u16;
typedef signed int	     s32;
typedef unsigned int	 u32;

typedef unsigned int __le32;
#define le32_to_cpu(x) 	(x)
#define cpu_to_le32(x)	(x)

#define	EINVAL		22	/* Invalid argument */

/*
* ror32 - rotate a 32-bit value right
*
* @word: value to rotate
* @shift: bits to roll
*/
static __inline u32 ror32(u32 word, unsigned int shift)
{
	return (word >> shift) | (word << (32 - shift));
}
/*
* rol32 - rotate a 32-bit value left
*
* @word: value to rotate
* @shift: bits to roll
*/
static __inline u32 rol32(u32 word, unsigned int shift)
{
	return (word << shift) | (word >> (32 - shift));
}

#endif

