//#include "stm32f10x_it.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal_rtc_ex.h"
#include "bsp.h"
#include "key_config.h"
#include "RTC.h"
#include "rtc_int.h"
#include "Key_Base.h"
#include "encoder.h"
#include "key.h"
#include <stdio.h>

extern int encoder_val;
//����״̬��
typedef enum{
    KS_Idle,
    KS_Pressed1,
    KS_WaitDbClick,
    KS_Long_Pressed,
}KeyState_t;

#ifndef REPEAT_TURN_POINT1	
	#define REPEAT_TURN_POINT1 25
#endif

#ifndef REPEAT_TURN_POINT2	
	#define REPEAT_TURN_POINT2 40
#endif

//����ֵУ��
#if KeyPushTimes < 2
    #error KeyPushTimes must >= 2
#endif

#if KeyLongPushTimes < 5 || KeyLongPushTimes<(KeyPushTimes+KeyRunTimes)
    #error KeyLongPushTimes must >= 5 and KeyLongPushTimes must >=(KeyPushTimes+KeyRunTimes)
#endif

#if KeyRunTimes < 1
    #error KeyRunTimes must >= 1
#endif

#if KeyScanNum > 1	//�а���ɨ��
	int      KeyScanId =      0;
#else	//�ް���ɨ��
	#define KeyScanId 	0
#endif
s16 Encode;
s16 EncodeLast;






KEY_BUF KeyBuf;
#define KeyOutBuf KeyBuf.Key[3] //��������ֽ�
static u8   PressCount[KeyScanNum][KeyRowNum];
static u8   ReleaseCount[KeyScanNum][KeyRowNum];
static u8   KeyState[KeyScanNum][KeyRowNum];
KEY_BUF     KeyNum;
 u8   		KeyBufNum;    //�������ڴ��еİ�����
volatile u8 AnyKeyPressedFlag = 0;
u8   ShiftState=0;  //shift��״̬��0��KEY_SHIFT
//�ظ����Ƶ�Ӧ�ò㴦��
// u8   		KeyRepeatNum; //�ظ�����
//u16         KeyRepeatNumRead;    //�Ѷ��ظ�����
//u16         KeyRepeatNumReadSum; //�ۼ��Ѷ��ظ�����

u32 getKeyBuf()
{
    return KeyBuf.Word;  
}
//��ֵ���뻺����
//0=>1=>2=>3
 void InKeyBuf(u8 x){
    switch (KeyBufNum) {
    case 1:
        KeyBuf.Key[2]=x;
        KeyBufNum=2;
        break;
    case 2:
        KeyBuf.Key[1]=x;
        KeyBufNum=3;
        break;
    case 3:
        KeyBuf.Key[0]=x;
        KeyBufNum=4;
        break;
    case 4:
        KeyBuf.Word=KeyBuf.Word<<8; //������λ���뻺����, ����м�δ�����Ƴ�  ori = " >> "
		KeyBuf.Key[0]=x;
    default:
        KeyBuf.Key[3]=x;
        KeyBufNum=1;
        break;
    }
}

#if KeyScanNum > 1	//�а���ɨ��
  //ɨ����һ��
    inline static void ScanNextCol(){
    	KeyScanId++;
    	if(KeyScanId==KeyScanNum) KeyScanId = 0;
    };
#else   //û�оͶ���һ���պ�
    #define ScanNextCol()
#endif


//��ʼ������������
void InitKey(void)
{
	int i,j;
    for (j=0;j<KeyScanNum;j++) {
        for(i=0;i<KeyRowNum;i++) {
            PressCount[j][i]=0;//��հ�������������
            ReleaseCount[j][i]=0;
			KeyState[j][i]=0;
        }
    }
    ShiftState = 0;
    //�������������
    ClearKey();
}

//TIM4 50Hz for Keyboard
//Ƶ�������õ�RTC�жϣ���ʡһ��TIM

//��հ���������
void ClearKey(void){
    DisableKeyTimInt(); 
    KeyBuf.Word=0;
    KeyBufNum =0;
   // KeyRepeatNum=0;
    EnableKeyTimInt();
}


//��ȡ��ǰ�������ļ���
int GetKey(){
    s16 tmp;
    DisableKeyTimInt();//��������ʱ���ж�

    //Ƶ���Ƹ����˲�ͬ���㷨��KeyRepeatNumһֱ�ۼƣ�����0��
    //KeyRepeatNumRead�����ϴζ���ֵ������KeyRepeatNumֵ�Ĵ�С����ɲ�ͬ��ֵ
    if (KeyBufNum==0) {
        tmp=KEY_NULL;
        if(encoder_buff_num > 0) { 
            encoder_val = GetEncoder(); //��ȡ������ֵ
            tmp = KEY_ENCODER; 
        }
    }else {
        tmp=KeyOutBuf;
        KeyBuf.Word=KeyBuf.Word<<8 ; //�����Ƴ�������

        KeyBufNum--;
    }

    EnableKeyTimInt();//��������ʱ���ж�
    return tmp;
}

#define InKey_Press()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+ShiftState+1); \
                       \
}





#define InKey_Release()  {                \
    AnyKeyPressedFlag = 1;              \
    InKeyBuf(i*KeyScanNum+KeyScanId+1+ShiftState+KEY_RELEASE); \
}
#define InKey_Long()  {                \
    AnyKeyPressedFlag = 1;              \
	InKeyBuf(i*KeyScanNum+KeyScanId+1+KEY_REPEATE); \
} 
#define InKey_LongRelease()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_LONG_RELEASE); \
} 

#define InKey_Repeate()  {                \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_REPEATE); \
} 

#define InKey_DbClick()  {                \
    AnyKeyPressedFlag = 1;              \
    InKeyBuf(i*KeyScanNum+KeyScanId+1 +KEY_DOUBLE); \
}
#define StoreInp 
static inline int ScanKeyPress()
{
    
    return -1;
}
//���������������֮��Ĺ�ϵ
//          HHHLHLLLLLLLLLHLHHHHHHHHH
//Press     000101234567899AAA0000000 //����+1, δ��ʱ������0, �ȶ�ʱ���߲���
//Release   0000000000000010120000000 //������0, �����ȶ����������, ����ȷ���ͷ�
//                x          x  
int KeyInput_1(void)
{
    int i=0;
    u8 PressCnt,ReleaseCnt,State,KeyMode;
    int InP;
    int j=0;
    for(;j<KeyRowNum;j++) {
        InP=KeyI(j);
        if (InP ==0 ){
            i=j;
            break; //�а�������
        }
    }
    State = KeyState[KeyScanId][i];
    if(IsKeyPressed || State !=KS_Idle) {//�а�������||״̬���ǿ���
        PressCnt = PressCount[KeyScanId][i];       //ȡѹ������
        ReleaseCnt  = ReleaseCount[KeyScanId][i];
        KeyMode = KeyModeCtrl[KeyScanId][i];
        switch (State) {
        case KS_Idle://����ʱ������
            //ע1��������һ�����ͷ�ʱ��ſ��Կ�ʼ״̬������ֹĳ�������������߰�������ʱ��Knob�����״̬��, �����ͷź�����²�ʼ��
            if (IsKeyPressed) {
                if (ReleaseCnt >= KeyPushTimes) { //ע1
                    PressCnt++; //ѹ����������
                    if (PressCnt >= KeyPushTimes){//ȷ��ѹ��
						State = KS_Pressed1;
                        if (KeyMode & (KEY_DBCLICK_EN |KEY_SHIFT_EN)) {
                            //˫����Shiftʱ�����水�¼�
                        }else{
                            break;//��һ�β����水�¼����Ƚ����Ƿ񳤰��жϣ����޼����������򱣴�̰�����
                        }
                        KeyPressLed(i);  //����LED
						ReleaseCnt = 0;
                    }
                }
            }
            break;
        case KS_Pressed1:
            if (IsKeyPressed) {
                PressCnt++; //ѹ���������ӣ�
                ReleaseCnt=0; //������ܳ��ֵĶ����ͷ�
                if(PressCnt == KeyLongPushTimes){
                    State = KS_Long_Pressed;
                    if (KeyMode & KEY_SHIFT_EN) {//�����shift��������״̬��Ҳ�ǰ���
                        ShiftState = KEY_SHIFT;
                    }
                    InKey_Repeate();
                }
            }else{
                ReleaseCnt++;
                if(PressCnt>=2){
                    InKey_Press(); //����ѹ�������ڵ���2������Ϊ���µ�û���������水��
                    //printf("pressed \r\n");
                    PressCnt=0;
                }
                if (ReleaseCnt>=KeyPushTimes){//ȷ���ͷ�
                    //ʹ��˫�����ң�����ѹ��ʱ��С��DbKeyPressTime
                    if((KeyModeCtrl[KeyScanId][i] & KEY_DBCLICK_EN) && PressCnt < DbKeyPressTime){
                        State = KS_WaitDbClick;//Release1,���ȴ���2�ΰ���
                    }else{//�����ͷ�
                        State = KS_Idle;
                        InKey_Release();
                    }
                    KeyReleaseLed(i);
                }
            }
            break;
        case KS_WaitDbClick:
            if (IsKeyPressed) {
                PressCnt++; //ѹ����������
                if (PressCnt >= KeyPushTimes) {
                    if (ReleaseCnt < DbKeyReleaseTime) {
                        State = KS_Idle;
                        InKey_DbClick();
                    }
                    ReleaseCnt = 0;
                }
            }else{
                ReleaseCnt++;
                if (ReleaseCnt > DbKeyReleaseTime) {//ȷ���ǵ���
                    State = KS_Idle;
                    InKey_Release();
                }
                PressCnt = 0;
            }
            break;
        case KS_Long_Pressed:
            if (IsKeyPressed) {
                if(KeyModeCtrl[KeyScanId][i] & KEY_REPEATE_EN){//��������ظ���
                    PressCnt++; //ѹ���������ӣ��ͷű��ֲ���
                    if (PressCnt == KeyLongPushTimes+KeyRunTimes) {
                        InKey_Repeate();
                        PressCnt = KeyLongPushTimes;	//���¼������˵�KeyLongPushTimes��׼����һ�γ���
                    }
                }
                ReleaseCnt = 0;//������ܵ�ë��
            }else{
                ReleaseCnt++;
                if (ReleaseCnt>=KeyPushTimes){
                    State = KS_Idle;
                    if (KeyMode & KEY_SHIFT_EN) {//�����shift������״̬
                        ShiftState = 0;
                    }
                    InKey_LongRelease();
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        }

        
    }
    else if(State ==KS_Idle){
        if(ReleaseCnt < KeyPushTimes) ReleaseCnt++; //ע1
        //����а��£����������ֱ��ͷţ������
        PressCnt = 0;
    }
    PressCount[KeyScanId][i] = PressCnt;
    ReleaseCount[KeyScanId][i]=ReleaseCnt;
    KeyState[KeyScanId][i] = State;
	ScanNextCol();
	return 0;
}


int KeyInput(void)
{
    int i=0;
    u8 PressCnt,ReleaseCnt,State,KeyMode;
    int InP;
    for(i=0;i<KeyRowNum;i++) {//ÿ������ֻ��һ��ɨ����
		InP=KeyI(i);  //��������ƽ
        PressCnt = PressCount[KeyScanId][i];       //ȡѹ������
        ReleaseCnt  = ReleaseCount[KeyScanId][i];
        State = KeyState[KeyScanId][i];
        KeyMode = KeyModeCtrl[KeyScanId][i];
        switch (State) {
        case KS_Idle://����ʱ������
            //ע1��������һ�����ͷ�ʱ��ſ��Կ�ʼ״̬������ֹĳ�������������߰�������ʱ��Knob�����״̬��, �����ͷź�����²�ʼ��
            if (IsKeyPressed){
                if (ReleaseCnt >= KeyPushTimes) { //ע1
                    PressCnt++; //ѹ����������
                    if (PressCnt >= KeyPushTimes){//ȷ��ѹ��
						State = KS_Pressed1;
                        if (KeyMode & (KEY_DBCLICK_EN |KEY_SHIFT_EN)) {
                            //˫����Shiftʱ�����水�¼�
                        }else{    //����һ���ж��ǳ������Ƕ̰�
                            break;
                        }
                        KeyPressLed(i);  //����LED
						ReleaseCnt = 0;
                    }
                }
            }else{
                if(ReleaseCnt < KeyPushTimes) ReleaseCnt++; //ע1
                //����а��£����������ֱ��ͷţ������
                PressCnt = 0;
            }
            break;
        case KS_Pressed1:
            if (IsKeyPressed) {
                PressCnt++; //ѹ���������ӣ�
                ReleaseCnt=0; //������ܳ��ֵĶ����ͷ�
                if(PressCnt == KeyLongPushTimes){
                    State = KS_Long_Pressed;
                    if (KeyMode & KEY_SHIFT_EN) {//�����shift��������״̬��Ҳ�ǰ���
                        ShiftState = KEY_SHIFT;
                    }
                    InKey_Repeate();
                }
            }else{
                ReleaseCnt++;
                if(PressCnt>=2){
                    InKey_Press(); //����ѹ�������ڵ���2������Ϊ���µ�û���������水��
                    PressCnt=0;
                }
                if (ReleaseCnt>=KeyPushTimes){//ȷ���ͷ�
                    //ʹ��˫�����ң�����ѹ��ʱ��С��DbKeyPressTime
                    if((KeyModeCtrl[KeyScanId][i] & KEY_DBCLICK_EN) && PressCnt < DbKeyPressTime){
                        State = KS_WaitDbClick;//Release1,���ȴ���2�ΰ���
                    }else{//�����ͷ�
                        State = KS_Idle;
                        InKey_Release();
                    }
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        case KS_WaitDbClick:
            if (IsKeyPressed) {
                PressCnt++; //ѹ����������
                if (PressCnt >= KeyPushTimes) {
                    if (ReleaseCnt < DbKeyReleaseTime) {
                        State = KS_Idle;
                        InKey_DbClick();
                    }
                    ReleaseCnt = 0;
                }
            }else{
                ReleaseCnt++;
                if (ReleaseCnt > DbKeyReleaseTime) {//ȷ���ǵ���
                    State = KS_Idle;
                    InKey_Release();
                }
                PressCnt = 0;
            }
            break;
        case KS_Long_Pressed:
            if (IsKeyPressed) {
                if(KeyModeCtrl[KeyScanId][i] & KEY_REPEATE_EN){//��������ظ���
                    PressCnt++; //ѹ���������ӣ��ͷű��ֲ���
                    if (PressCnt == KeyLongPushTimes+KeyRunTimes) {
                        InKey_Repeate();
                        PressCnt = KeyLongPushTimes;	//���¼������˵�KeyLongPushTimes��׼����һ�γ���
                    }
                }
                ReleaseCnt = 0;//������ܵ�ë��
            }else{
                ReleaseCnt++;
                if (ReleaseCnt>=KeyPushTimes){
                    State = KS_Idle;
                    if (KeyMode & KEY_SHIFT_EN) {//�����shift������״̬
                        ShiftState = 0;
                    }
                    InKey_LongRelease();
                    KeyReleaseLed(i);
                    PressCnt = 0;
                }
            }
            break;
        }

        PressCount[KeyScanId][i] = PressCnt;
        ReleaseCount[KeyScanId][i]=ReleaseCnt;
        KeyState[KeyScanId][i] = State;
    }

	ScanNextCol();
	return 0;
}


