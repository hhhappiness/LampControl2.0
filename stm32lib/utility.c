
#include "stm32g4xx.h"
#include "utility.h"

/*******************************************************************************
 Function:
 	将32位无符号数转成10进制字符串，并在指定位置加上小数点
	len为输出字符串长度，radix是小数点位置，实际存贮空间需要len+(radix>0)+1
    例：
    U32ToDECStr(1234,s,6,1,0)的结果为"123.4  "
    U32ToDECStr(1234,s,6,2,1)的结果为" 12.34 "
Date:
 	2006-8-23
	2014-11-26 U16改U32，len由不包含小数位数改成包含，增加对齐
*******************************************************************************/
void U32ToDECStr(u32 x,char *s,u8 len,u8 radix,u8 align)
{
    s8 i;
    u8 bufsize;
	u8 dotpos;
	u8 left=0;
	//计算数值本身要占多少位
	bufsize = U32ToDigitalNum(x);
	//加上小数点的占位
	if(radix!=0) {//如果有小数点
		if(bufsize<=radix) bufsize=radix+2;//增加小数点和前面的0占的位置
		else bufsize+=1;//增加小数点占的位置
		dotpos = bufsize-radix-1;//小数点位置
	}else{
		dotpos = 0x80;//弄一远大于bufsize的值
	}
	if(len<bufsize+1) {//缓冲区不够大
		s[0] = 0; //返回空字符串
		return;
	}
	if(align ==0) 		left=0;					//左对齐
	else if(align ==1) 	left=(len-1-bufsize)/2;	//中对齐
	else				left = (len-1-bufsize);	//右对齐
	//整体移位
	bufsize+=left;
	dotpos +=left;
	//掐尾
	s[bufsize]=0;
	//输出数值和小数点
    for(i=bufsize-1;i>=left;i--)
    {
		if(i==dotpos)
			s[i]='.';
		else{
			s[i]=x%10+'0';
			x=x/10;
		}
    }
	//前面填空格
	for(;i>=0;i--)
		s[i]=' ';
}

/*******************************************************************************
 Function:
 	将32位无符号数转成16进制字符串
Date:
 	2010-02-03
*******************************************************************************/
void U32ToHEXStr(u32 x,char *s,u8 len,u8 align)
{
    s8 i;
	u8 tmp;
	u8 bufsize;
	u8 left=0;
	bufsize = U32ToHexNum(x);
	if(len<bufsize+1) {//缓冲区不够大
		s[0] = 0; //返回空字符串
		return;
	}
	if(align ==0) 		left=0;					//左对齐
	else if(align ==1) 	left=(len-1-bufsize)/2;	//中对齐
	else				left = (len-1-bufsize);	//右对齐
	//整体移位
	bufsize+=left;
	//掐尾
	s[bufsize]=0;
	for(i=bufsize-1;i>=left;i--)
    {
		tmp=x & 0x0f;
		if(tmp<10) {
			tmp += '0';
		}else{
			tmp += 'A'-10;
		}
		s[i]=tmp;
		x = x>>4;
	}
	//补前导空格
	for(;i>=0;i--){
		s[i]=' ';
	}	
}

/*******************************************************************************
 Function:
 	将32位无符号数转成8位16进制字符串
Date:
 	2010-02-03
*******************************************************************************/
void U32ToHEXStr8(u32 x,char *s)
{
    s8 i;
	u8 tmp;
	i=7;
	while(x!=0) {
		tmp=x & 0x0f;
		if(tmp<10) {
			tmp += '0';
		}else{
			tmp += 'A'-10;
		}
		s[i]=tmp;
		if((i--)==0) break;
		x = x>>4;
	}
	//补前导0
	for(;i>=0;i--){
		s[i]='0';
	}
}
/*******************************************************************************
 Function:
 	将16位无符号数转成4位16进制字符串
Date:
 	2010-02-03
*******************************************************************************/
void U16ToHEXStr(u16 x,char *s)
{
    s8 i;
	u8 tmp;
	i=3;
	while(x!=0) {
		tmp=x & 0x0f;
		if(tmp<10) 	tmp += '0';
		else 		tmp += 'A'-10;
		s[i]=tmp;
		if((i--)==0) break;
		x = x>>4;
	}
	//补前导0
	for(;i>=0;i--){
		s[i]='0';
	}
}


/*******************************************************************************
 Function:
 	将8位无符号数转成2位16进制字符串
Date:
 	2006-8-23
*******************************************************************************/
void U8ToHEXStr(u16 x,char *s)
{
    s8 i;
	u8 tmp;

	for(i=1;i>=0;i--){
		tmp=x & 0x0f;
		if(tmp<10) 	s[i] = '0'+tmp;
		else 		s[i] = 'A'+tmp-10;
		x = x>>4;
	}
}
/*******************************************************************************
 Function:
 	将8位2进制数各bit转成字符串，高位在前，低位在后
Date:
 	2006-8-23
*******************************************************************************/
void U8ToBINStr(u8 Key,char* s,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		if(Key & (1<<i))s[len-1-i]='1';
		else s[len-1-i]='0';
	}
	s[len]=0;
}

/**************************************************************
在int型数据基础上增减数值，不超过Max,Min的范围，不考虑int的溢出
***************************************************************/
//增加或减少
int StepS32(int Dat,int x,int Min ,int Max){
	int newval = Dat+x;
	if(newval>Max) {
		return Max;
	}else if(newval<Min){
		return Min;
	}else{
        return newval;
	}	
}

//增加或减少
u16 StepU16(u16 Dat,int x,u16 Min,u16 Max){
	int newval = (int)Dat+x;//转成整形防止溢出
	if(newval>Max) {
		return Max;
	}else if(newval<Min){
		return Min;
	}else{
        return newval;
	}
}

u32 DecStr2U32(void *Str)
{
	u32 tmp=0;
	char *p = (char *)Str;
	while(*p>='0' && *p<='9') {
		tmp*=10;
		tmp+=*p - '0';
        p++;
	}
	return tmp;
}

u32 HexStr2U32(void *Str)
{
	u32 tmp=0;
	char *p = (char *)Str;
	while(1) {
		if(*p>='0' && *p<='9'){
			tmp*=16;
			tmp+=*p - '0';
		}else if(*p>='a' && *p<='f'){
			tmp*=16;
			tmp+=*p - 'a'+10;			
		}else if(*p>='A' && *p<='F'){
			tmp*=16;
			tmp+=*p - 'A'+10;				
		}else{
			break;
		}
		p++;

	}
	return tmp;
}

u32 Str2U32(void *Str)
{
	char *p = (char *)Str;
	if(*p=='0' && *(p+1)=='x'){
		return(HexStr2U32(p+2));
	}else{
		return(DecStr2U32(p));
	}
}

///计算u32型数据累加和
u32 SumU32(void *buf, int len)
{
	u32 *p = (u32 *)buf;
	u32 sum = 0;
	for(;len>0;len--){
		sum += *p++;
	}
	return sum;
}

int Str2S32(void *Str)
{
	int tmp=0;
	int sign = 1;
	char *p = (char *)Str;
	if(*p=='-') {
		sign = -1;
		p++;
	}
	while(*p>='0' && *p<='9') {
		tmp*=10;
		tmp+=*p - '0';
        p++;
	}
	if(sign==1) {
		return tmp;
	}else{
		return -tmp;
	}
}

#define MAX_FRAC_NUM 6
//将字符串转成float型: -12.3423 .3423 
//可以是-,.和数字开头，其它字符忽略
//遇到非数字结束。小数位不超过6位
float Str2Float(void *Str)
{
	int Sign=1;		//符号位,1或-1
	int Intiger=0;	//整数部分
	int Fraction=0;	//小数部分
	int FracNum=0;	//小数个数
	char *p = (char *)Str;
	int Stage=0;//0：开头, 1:整数部分 2:小数部分
	const float coff[MAX_FRAC_NUM+1]={1.0,0.1,0.01,0.001,0.0001,0.00001,0.000001};
	while(*p) {
		switch(Stage) {
		case 0: 
			if(*p>='0' && *p<='9'){
				Intiger=*p - '0';
				Stage = 1;
			}else if(*p=='-'){
				Stage = 1;
				Sign = -1;
			}else if(*p=='.'){
				Stage=2;
			}
			p++;
			break;
		case 1:
			if(*p>='0' && *p<='9'){
				Intiger*=10;
				Intiger+=*p - '0';
			}else if(*p=='.'){
				Stage=2;
			}else{
				goto EndOfStr2Float;
			}
			p++;
			break;
		case 2:
			if(*p>='0' && *p<='9'){
				Fraction*=10;
				Fraction+=*p - '0';
				FracNum++;
				if(FracNum==MAX_FRAC_NUM) {
					goto EndOfStr2Float;
				}
			}else{
				goto EndOfStr2Float;
			}
			p++;
			break;
		}
	}
EndOfStr2Float:
	return(Sign*(Intiger + Fraction*coff[FracNum]));
}

//将字符转成小写
__inline char _ToLowCase(const char ch)
{
	if(ch >='A' && ch <= 'Z') {
		return (ch+'a'-'A');
	}else{
		return ch;
	}
}

//不区分大小写比较字符串
int StrCmpNoCase(const char *p1,const char *p2)
{
	char ch1,ch2;
	while(1) {
		ch1 = _ToLowCase(*p1++);
		ch2 = _ToLowCase(*p2++);
		if(ch1==ch2) {//相同
			if(ch1==0) return 0;//一起结束
		}else{//不相等
			return(ch1-ch2);
		}
	}
}

int U32ToStr(u32 x,char *s,u8 len)
{
	int Len1 = U32ToDigitalNum(x);
	int i;
	if(Len1+1>len) return 0;//超长
	s+=Len1;
	*s=0;//结束符
	for(i=0;i<Len1;i++){
		s--;
		*s = x%10+'0';
		x=x/10;
	}
	return Len1;
}

///将2个数字的版本号转成字符串，例如1.1。尽量避免使用sprintf
///返回值为字符串长度
int VersonStr2(char *s,u8 len, u8 Major, u8 Minor)
{
	int Len1 = U32ToStr(Major,s,len);
	int Len2;
	if(Len1 ==0) return 0;
	len -= Len1;
	s += Len1;
	//加.
	if(2>len) return Len1;
	*s='.';
	s++;
	*s = 0;
	len --;
	//加Minor
	Len2 = U32ToStr(Minor,s,len);
	return (Len1+1+Len2);
}

///将3个数字的版本号转成字符串，例如1.0.1。尽量避免使用sprintf
///返回值为字符串长度
int VersonStr3(char *s,u8 len, u8 Major, u8 Mid, u8 Minor)
{
	return 0;
}
