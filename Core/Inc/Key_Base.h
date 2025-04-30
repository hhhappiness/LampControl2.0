#ifndef _KEY_BASE_H_
#define _KEY_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif 
	
void InitKey(void);
int KeyInput(void);
int GetKey(void);
	
#define KEY_NULL            0
//�����롣���û��Shift��������31��������Shift��������15����������ť
#define KEY_1               1
#define KEY_2               2
#define KEY_3               3
#define KEY_4               4
#define KEY_5               5
#define KEY_6               6
#define KEY_7               7
#define KEY_8               8
#define KEY_9               9
#define KEY_10              10
#define KEY_11              11
#define KEY_12              12
#define KEY_13              13
#define KEY_14              14
#define KEY_15              15

//������־
#define KEY_PRESSED         0x00    //��������
#define KEY_SHIFT           0x10    //shift��
#define KEY_RELEASE         0x20    //�̰��ͷ�
#define KEY_REPEATE         0x40    //�ظ���
#define KEY_LONG_RELEASE    0x60    //�����ͷ�
#define KEY_DOUBLE          0x80    //˫��

//�������ܶ���
#define KEY_REPEATE_EN  0x01    //ʹ���ظ���(��Ӱ�쳤����)
#define KEY_DBCLICK_EN  0x02    //ʹ��˫��  
#define KEY_SHIFT_EN    0x04    //����Shift��

//��������±�־
extern volatile unsigned char AnyKeyPressedFlag ;

#ifdef __cplusplus
}
#endif 

#endif //_KEY_BASE_H_
