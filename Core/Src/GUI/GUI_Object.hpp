#ifndef _GUI_OBJECT_H_
#define _GUI_OBJECT_H_

#include "Fonts.h"
#include "string.h"
#include "stdio.h"
#include "utility.h"

namespace gui {

///�������ꡢ��ȡ��߶ȵ��������ͣ�������ֲ
///����128x64������u8����
typedef unsigned char Pos_t;
	
/**
	λͼ�࣬������һ��ָ���ߵ�λͼ�������趨ÿ8������һ���ֽڡ�
**/	
class Bitmap
{
public:
	///����������������λͼ
	Bitmap(Pos_t w, Pos_t h, u8 * buf, u8 startbit=0);

	///������Bitmap���죬������Ϣ��������ָ�롣ע�⸴����Դ�����ش����������Ƕ�̬������ڴ棬�����а�ȫ������
	Bitmap(const Bitmap & bitmap);
	
	///����ָ����Сλͼ����̬���������ڴ档����ʹ�ñ�������ȷ����ȫ
	Bitmap(Pos_t w, Pos_t h);
	
	~Bitmap();
	
	Pos_t Width;	///<λͼ���
	Pos_t Height;	///<λͼ�߶�
	u8 StartBit;	///<���ض����ռ���bit����λͼ��ʾ������y����8�ı�����������3�����λͼ������5�����ؾ͸պÿ���ƴ�ϣ��򻯻�ͼ�ĺ�����
	u8 Alloc;		///<�Ƿ�̬�������ش�������
	u8 * pPix;		///<���ش�����ָ�룬����Ϊ��
};

///���ֶ��뷽ʽö��
typedef enum {
	AlignLeft, 	//��(ȱʡ)
	AlignMid,	//��
	AlignRight	//��
}Align_t;

///�ؼ��Ƿ�ѡ�е�ö��
typedef enum {
	NotSelected,	//δѡ��
	Selected		//��ѡ��
}SelectMode_t;

class GUI_Page;

///�ؼ��Ļ���
///�޶�����Pageϵͳ��ֻ��һ�����Page����ǰ��ʾ�Ŀؼ����ǻ��ڵ�ǰPage�ϡ��ɴ˴����ĺô��ǲ���ÿ���ؼ�����Parent��ֱ�ӻ���pCurrPage�Ͼ����ˡ�
class GUI_Object
{
public:
	GUI_Object(Pos_t w, Pos_t h,const Font_t* font=&DEFAULT_ASCII_FONT)
	: x(0), y(0), Width(w), Height(h)
	, Align(AlignLeft), SelectMode(NotSelected), ClearBeforeShow(true), Enable(true), Font(font)
	,OnChange(NULL), OnOk(NULL)
	{
	}
	Pos_t x;		///<�ؼ����Ͻ�����x
	Pos_t y;		///<�ؼ����Ͻ�����y
	Pos_t Width;	///<�ؼ����
	Pos_t Height;	///<�ؼ��߶�
	Align_t 	Align;///<���뷽ʽ 		
	SelectMode_t SelectMode;///<�Ƿ�ѡ��
	bool ClearBeforeShow;	///<���ǰ������ʾ��
	bool Enable;			///<�Ƿ���Ի�ý���
	const Font_t *Font;			///<Ascii����
	bool (*OnChange)(int new_val);///<ÿ�α仯ʱִ�еĲ���
	bool (*OnOk)(int new_val);///<ȷ�Ϻ�ִ�еĲ���	
	GUI_Page * bakPage;		///<�ؼ������Ҫ����ҳ��ʱ�������ָ�뱣��ԭ����ҳ��ָ�롣
	virtual void Display(){}	///<��ʾ�ؼ���ֵ��ͼ����麯��
	virtual bool OnEdit(){return false;}	///<�༭״̬������������ֵ���ؼ���Ӧ��ֵ�Ƿ����ı�
	///���ÿؼ�����
	void SetPos(Pos_t x, Pos_t y) {		
		this->x = x; this->y = y;
	}
	///���ÿؼ�ѡ��״̬
	inline void SetSelect(SelectMode_t t=Selected){
		SelectMode = t;
	}
	///����ؼ���ռ����
	inline void Clear();
	///���¿ؼ���ʾ����Lcd
	void Update();
	///�ڿؼ�������ʾһ���ַ���
	inline void DisplayStr(const char *str);
	//���ö��뷽ʽ
	inline void SetAligh(Align_t al) {
		Align = al;
		}
};

class GUI_Text : public GUI_Object
{
public:
	GUI_Text(Pos_t w, Pos_t h, const char * str, const Font_t* font=&DEFAULT_ASCII_FONT) 
	: GUI_Object(w,h,font)
	, Str(str)
	{}
	const char * Str;
	virtual void Display();
};

#define GUI_NUM_STR_LEN		13
class GUI_NumText : public GUI_Object
{
public: 	
	// ���캯������ʼ��GUI_NumText����
	GUI_NumText(int *val, u8 digital_num, u8 decimal=0, const Font_t* font=&DEFAULT_ASCII_FONT)
	: GUI_Object(digital_num*font->WidthDefault+1,font->Height,font) //ȱʡ��16x7������
	,  pVal(val), DigitalNum(digital_num), Decimal(decimal)
	{
		Enable = false;
	}

	int *pVal;	///<�ؼ��󶨵���ֵ��ָ�룬����Ϊint�͡�
	u8 DigitalNum;	///<ռ���ַ�λ��������С����͸���
	u8 Decimal; 	///<С������	
	
	static char NumStrBuf[GUI_NUM_STR_LEN];	//���ڽ�����ת���ַ����Ĺ��û�����
	inline void ToDecStr() {
		U32ToDECStr(*pVal, NumStrBuf, DigitalNum+1, Decimal, Align);
	}
	inline void ToHexStr() {
		U32ToHEXStr(*pVal, NumStrBuf, DigitalNum+1, Align);
	}	
	virtual void Display();
};


class GUI_Num : public GUI_NumText
{
public:
	GUI_Num(int *val, int min, int max, u8 digital_num, u8 decimal=0, const Font_t* font=&DEFAULT_ASCII_FONT) 
	: GUI_NumText(val, digital_num, decimal, font) 
	, Min(min), Max(max), Step(1), StepList(DefaultStupList), KeepList(DefaultKeepList), StepNum(sizeof(DefaultStupList)/4)//,TuningMode(FineTuning)
	{
		Enable = true;
		Align = AlignRight;
	}
#if 0	
	///ֻ�����˻���Ĳ�������������������ʹ��ǰ���ú�
	GUI_Num(u8 digital_num, const Font_t* font=&DEFAULT_ASCII_FONT)
	: GUI_NumText(NULL, digital_num, 0, font) 
	, Step(1),TuningMode(FineTuning)
	{
		Enable = true;
		Align = AlignRight;
	}
#endif	
	///�Զ�ģʽ��ö��
	typedef enum{
		Mode_AutoMinus	=-2,	///<�Զ���С
		Mode_Minus		=-1,	///<�ֶ���С
		Mode_None		= 0,	///<���Զ�
		Mode_Plus		= 1,	///<�ֶ�����
		Mode_AutoPlus	= 2,	///<�Զ�����
	}Mode_t;
	
    int Min;    ///<��ֵ������
    int Max;    ///<��ֵ������
    u32 Step;		///<����ֵ��ȱʡΪ1
	const u32 * StepList;
	const u32 * KeepList;
	u32 StepNum;
	enum{
		DefaultStepNum = 3
	};
	static const u32 DefaultStupList[DefaultStepNum];
	static const u32 DefaultKeepList[DefaultStepNum];
	
	inline void SetStepList(const u32 * list=DefaultStupList, const u32 * keep =DefaultKeepList, const u32 num=DefaultStepNum){
		StepList = list;
		KeepList = keep;
		StepNum = num;
	}
	virtual int OnEditPage();
	virtual bool OnEdit();


	//���������ٵ���
	
	///����ĳ��ֵstep
	int OnStep(int step);
	///�ؼ���Ӧֵ����2
	inline int Mul2() { 
		return(OnStep(*pVal));
	}
	///�ؼ���Ӧֵ����2
	inline int Div2()
	{
		return(OnStep(-(*pVal/2)));
	}
	
	void SetDefaultWidth(){
		Width = DigitalNum*Font->WidthDefault+1;
		Height = Font->Height;
	}
};

class GUI_List : public GUI_Object
{
public:	
	GUI_List(Pos_t w, Pos_t h, int *pval, u8 num, const int *val_list, const char ** const str_list) 
	: GUI_Object(w,h)
	, pVal(pval), Num(num), ValList(val_list), StrList(str_list)
	{
		InitId();
	}
		
    int *pVal;       		//�ؼ���Ӧ�ı���
    u8 Num;        			//�б�Ԫ�ظ���
    u8 Id;         			//��ǰԪ��ID
    const int *ValList; 	//��ֵ�б�
    const char **StrList;	//��Ӧ��ʾ���ַ�������
	virtual void Display();
	virtual bool OnEdit();
	///���ݵ�ǰֵ���б�����ҵõ�Id��
	void InitId();
 };


/**************************************************************
 Bool�Ϳؼ���ѡ�к�ȷ���л���ʾ�����ַ�������Ӧ0/1����ֵ
***************************************************************/
class GUI_Bool : public GUI_Object
{
public:
	GUI_Bool(Pos_t w, Pos_t h,  int *val, const char ** const str_list)
	: GUI_Object(w,h)
	, pVal(val), StrList(str_list)
	{
	}
    s32 *pVal;   //�ؼ������ı���
    const char **StrList;
	virtual void Display();
	virtual bool OnEdit();
};

#define CHECKBOX_W	16
#define CHECKBOX_H	14
/**************************************************************
 CheckBox�Ϳؼ���ѡ�к�ȷ���л���ʾCheckBoxͼ������״̬����Ӧ0/1����ֵ
***************************************************************/
class GUI_CheckBox : public GUI_Object
{
public:
	GUI_CheckBox(int *val);
    s32 *pVal;   //�ؼ������ı���
    
	static const u8 CheckBox[2][CHECKBOX_W*(CHECKBOX_H+7/8)];
	static const Bitmap Icon_CheckBox[2];

	virtual void Display();
	virtual bool OnEdit();
};

class GUI_TextButton : public GUI_Text
{
public:
	GUI_TextButton(Pos_t w, Pos_t h, const char *str)
	: GUI_Text(w,h,str)
	{
		Align = AlignMid;
	}

	void SetAligh(Align_t al);
};

class GUI_Icon : public GUI_Object
{
public:
	GUI_Icon(Pos_t w, Pos_t h, Bitmap *icon)
	: GUI_Object(w,h)
	, pIcon(icon)
	{}
    Bitmap * pIcon;
	virtual void Display();
};

class GUI_IconButton : public GUI_Icon
{
public:
	GUI_IconButton(Pos_t w, Pos_t h, Bitmap *icon)
	: GUI_Icon(w,h,icon)
	{}
};


///Slider�ؼ��������
#define SLIDER_LINE_THICKNESS		3
///Slder�ؼ�������
#define SLIDER_Width		4
///Slder�ؼ�������
#define SLIDER_HEIGHT		9
//Todo��Min��Max������ֵ�ؼ��Ĺ�������ʱ�Ȼ�������ռ��λ��
class GUI_Slider : public GUI_Object
{
public:
	GUI_Slider(Pos_t w, int *val);
	s32 *pVal;   //�ؼ������ı���

	virtual void Display();
	virtual bool OnEdit();
};

#define GUI_PROGRES_HEIGHT 12
class GUI_Progress : public GUI_Object
{
public:
	GUI_Progress(Pos_t w, Pos_t h=GUI_PROGRES_HEIGHT)
	: GUI_Object(w, h)
	{
		
	}
	
	s32 Val;   //�ؼ������ı���
	int Min;    ///<��ֵ������
    int Max;    ///<��ֵ������
	
	void SetRange(int min_val, int max_val){
		Min = min_val; Max = max_val;
	}
	void SetValue(int val, bool redraw=true){
		Val = val;
		if(redraw){
			Display();
			Update();
		}
	}
	virtual void Display();
};

/**
������֯ҳ��GUI_Page��Ŀؼ��б�ͬһ��ҳ��Ŀؼ��������ޣ��ڲ�ͬ��������¿ؼ�������ͬ��
��c++����list�е㸴�ӣ�����Ū����������֯������ʱnew����һ��GUI_Object��ָ�����飬�����СΪMaxNum��
�޶�GUI_Object����new���ɣ�����ɾ���ؼ�����ķ���ͳһ��
����������е�ָ���0��ʼ˳�����У��м�û�пյ���
�����ṩ�򵥵����ɾ����Ա����������ά���ɸ�ҳ�����д���
ͨ����˼·�ǲ���Ŀؼ���ҳ�湹�캯������ӣ��仯�Ŀؼ���Init()����ӡ�ɾ����
**/
class GUI_ObjList {
	
public:
	GUI_ObjList(int max_num);
	~GUI_ObjList();
	
	int MaxNum;	///< ���ؼ�����������Ԥ�ȷ���ռ�ָ������Ŀռ�
	int Num;	///< ��ǰ�ؼ�����
	GUI_Object ** _ObjList;///<�ؼ�ָ�����飬��̬����

public:
	///delete�ؼ��б�ָ��Ķ�������б�
	void Clear();
	//delete��i��j�Ŀ��ƶ���
	void Delete(int i,int j);
	
	void Delete(int i);
	
	void Set(GUI_Object *p, int i);
	
	void Append(GUI_Object *p);
	//������[]ֱ�ӷ����б��е�ָ��
	inline GUI_Object * operator [](int i) {
		return _ObjList[i];
	}
};

}//namespace gui {
#endif//_GUI_OBJECT_H_

