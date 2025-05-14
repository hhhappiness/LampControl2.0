
#include "stm32g4xx.h"
#include "utility.h"

/*******************************************************************************
 Function:
 	��32λ�޷�����ת��10�����ַ���������ָ��λ�ü���С����
	lenΪ����ַ������ȣ�radix��С����λ�ã�ʵ�ʴ����ռ���Ҫlen+(radix>0)+1
    ����
    U32ToDECStr(1234,s,6,1,0)�Ľ��Ϊ"123.4  "
    U32ToDECStr(1234,s,6,2,1)�Ľ��Ϊ" 12.34 "
Date:
 	2006-8-23
	2014-11-26 U16��U32��len�ɲ�����С��λ���ĳɰ��������Ӷ���
*******************************************************************************/
void U32ToDECStr(u32 x,char *s,u8 len,u8 radix,u8 align)
{
    s8 i;
    u8 bufsize;
	u8 dotpos;
	u8 left=0;
	//������ֵ����Ҫռ����λ
	bufsize = U32ToDigitalNum(x);
	//����С�����ռλ
	if(radix!=0) {//�����С����
		if(bufsize<=radix) bufsize=radix+2;//����С�����ǰ���0ռ��λ��
		else bufsize+=1;//����С����ռ��λ��
		dotpos = bufsize-radix-1;//С����λ��
	}else{
		dotpos = 0x80;//ŪһԶ����bufsize��ֵ
	}
	if(len<bufsize+1) {//������������
		s[0] = 0; //���ؿ��ַ���
		return;
	}
	if(align ==0) 		left=0;					//�����
	else if(align ==1) 	left=(len-1-bufsize)/2;	//�ж���
	else				left = (len-1-bufsize);	//�Ҷ���
	//������λ
	bufsize+=left;
	dotpos +=left;
	//��β
	s[bufsize]=0;
	//�����ֵ��С����
    for(i=bufsize-1;i>=left;i--)
    {
		if(i==dotpos)
			s[i]='.';
		else{
			s[i]=x%10+'0';
			x=x/10;
		}
    }
	//ǰ����ո�
	for(;i>=0;i--)
		s[i]=' ';
}

/*******************************************************************************
 Function:
 	��32λ�޷�����ת��16�����ַ���
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
	if(len<bufsize+1) {//������������
		s[0] = 0; //���ؿ��ַ���
		return;
	}
	if(align ==0) 		left=0;					//�����
	else if(align ==1) 	left=(len-1-bufsize)/2;	//�ж���
	else				left = (len-1-bufsize);	//�Ҷ���
	//������λ
	bufsize+=left;
	//��β
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
	//��ǰ���ո�
	for(;i>=0;i--){
		s[i]=' ';
	}	
}

/*******************************************************************************
 Function:
 	��32λ�޷�����ת��8λ16�����ַ���
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
	//��ǰ��0
	for(;i>=0;i--){
		s[i]='0';
	}
}
/*******************************************************************************
 Function:
 	��16λ�޷�����ת��4λ16�����ַ���
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
	//��ǰ��0
	for(;i>=0;i--){
		s[i]='0';
	}
}


/*******************************************************************************
 Function:
 	��8λ�޷�����ת��2λ16�����ַ���
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
 	��8λ2��������bitת���ַ�������λ��ǰ����λ�ں�
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
��int�����ݻ�����������ֵ��������Max,Min�ķ�Χ��������int�����
***************************************************************/
//���ӻ����
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

//���ӻ����
u16 StepU16(u16 Dat,int x,u16 Min,u16 Max){
	int newval = (int)Dat+x;//ת�����η�ֹ���
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

///����u32�������ۼӺ�
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
//���ַ���ת��float��: -12.3423 .3423 
//������-,.�����ֿ�ͷ�������ַ�����
//���������ֽ�����С��λ������6λ
float Str2Float(void *Str)
{
	int Sign=1;		//����λ,1��-1
	int Intiger=0;	//��������
	int Fraction=0;	//С������
	int FracNum=0;	//С������
	char *p = (char *)Str;
	int Stage=0;//0����ͷ, 1:�������� 2:С������
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

//���ַ�ת��Сд
__inline char _ToLowCase(const char ch)
{
	if(ch >='A' && ch <= 'Z') {
		return (ch+'a'-'A');
	}else{
		return ch;
	}
}

//�����ִ�Сд�Ƚ��ַ���
int StrCmpNoCase(const char *p1,const char *p2)
{
	char ch1,ch2;
	while(1) {
		ch1 = _ToLowCase(*p1++);
		ch2 = _ToLowCase(*p2++);
		if(ch1==ch2) {//��ͬ
			if(ch1==0) return 0;//һ�����
		}else{//�����
			return(ch1-ch2);
		}
	}
}

int U32ToStr(u32 x,char *s,u8 len)
{
	int Len1 = U32ToDigitalNum(x);
	int i;
	if(Len1+1>len) return 0;//����
	s+=Len1;
	*s=0;//������
	for(i=0;i<Len1;i++){
		s--;
		*s = x%10+'0';
		x=x/10;
	}
	return Len1;
}

///��2�����ֵİ汾��ת���ַ���������1.1����������ʹ��sprintf
///����ֵΪ�ַ�������
int VersonStr2(char *s,u8 len, u8 Major, u8 Minor)
{
	int Len1 = U32ToStr(Major,s,len);
	int Len2;
	if(Len1 ==0) return 0;
	len -= Len1;
	s += Len1;
	//��.
	if(2>len) return Len1;
	*s='.';
	s++;
	*s = 0;
	len --;
	//��Minor
	Len2 = U32ToStr(Minor,s,len);
	return (Len1+1+Len2);
}

///��3�����ֵİ汾��ת���ַ���������1.0.1����������ʹ��sprintf
///����ֵΪ�ַ�������
int VersonStr3(char *s,u8 len, u8 Major, u8 Mid, u8 Minor)
{
	return 0;
}
