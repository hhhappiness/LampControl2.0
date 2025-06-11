#ifndef _GUI_OBJECT_H_
#define _GUI_OBJECT_H_

#include "Fonts.h"
#include "string.h"
#include "stdio.h"
#include "utility.h"

namespace gui {

///用于坐标、宽度、高度的数据类型，便于移植
///对于128x64的屏用u8就行
typedef unsigned char Pos_t;
	
/**
	位图类，定义了一个指点宽高的位图。隐含设定每8个像素一个字节。
**/	
class Bitmap
{
public:
	///由已有数据区构造位图
	Bitmap(Pos_t w, Pos_t h, u8 * buf, u8 startbit=0);

	///从现有Bitmap构造，复制信息及缓冲区指针。注意复制来源的像素存贮区不能是动态分配的内存，否则有安全隐患。
	Bitmap(const Bitmap & bitmap);
	
	///创建指定大小位图，动态分配像素内存。慎重使用本方法，确保安全
	Bitmap(Pos_t w, Pos_t h);
	
	~Bitmap();
	
	Pos_t Width;	///<位图宽度
	Pos_t Height;	///<位图高度
	u8 StartBit;	///<像素顶部空几个bit。当位图显示的坐标y不是8的倍数，例如余3，如果位图顶部空5个像素就刚好可以拼上，简化绘图的函数。
	u8 Alloc;		///<是否动态分配像素存贮区。
	u8 * pPix;		///<像素存贮区指针，不能为空
};

///文字对齐方式枚举
typedef enum {
	AlignLeft, 	//左(缺省)
	AlignMid,	//中
	AlignRight	//右
}Align_t;

///控件是否选中的枚举
typedef enum {
	NotSelected,	//未选中
	Selected		//已选中
}SelectMode_t;

class GUI_Page;

///控件的基类
///限定：单Page系统，只有一个活动的Page，当前显示的控件都是绘在当前Page上。由此带带的好处是不用每个控件定义Parent，直接绘在pCurrPage上就行了。
class GUI_Object
{
public:
	GUI_Object(Pos_t w, Pos_t h,const Font_t* font=&DEFAULT_ASCII_FONT)
	: x(0), y(0), Width(w), Height(h)
	, Align(AlignLeft), SelectMode(NotSelected), ClearBeforeShow(true), Enable(true), Font(font)
	,OnChange(NULL), OnOk(NULL)
	{
	}
	Pos_t x;		///<控件左上角坐标x
	Pos_t y;		///<控件左上角坐标y
	Pos_t Width;	///<控件宽度
	Pos_t Height;	///<控件高度
	Align_t 	Align;///<对齐方式 		
	SelectMode_t SelectMode;///<是否选中
	bool ClearBeforeShow;	///<输出前先清显示区
	bool Enable;			///<是否可以获得焦点
	const Font_t *Font;			///<Ascii字体
	bool (*OnChange)(int new_val);///<每次变化时执行的操作
	bool (*OnOk)(int new_val);///<确认后执行的操作	
	GUI_Page * bakPage;		///<控件如果需要弹出页面时，用这个指针保存原来的页面指针。
	virtual void Display(){}	///<显示控件的值、图标的虚函数
	virtual bool OnEdit(){return false;}	///<编辑状态处理函数，返回值：控件对应的值是否发生改变
	///设置控件坐标
	void SetPos(Pos_t x, Pos_t y) {		
		this->x = x; this->y = y;
	}
	///设置控件选中状态
	inline void SetSelect(SelectMode_t t=Selected){
		SelectMode = t;
	}
	///清除控件所占区域
	inline void Clear();
	///更新控件显示区到Lcd
	void Update();
	///在控件区域显示一个字符串
	inline void DisplayStr(const char *str);
	//设置对齐方式
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
	// 构造函数，初始化GUI_NumText对象
	GUI_NumText(int *val, u8 digital_num, u8 decimal=0, const Font_t* font=&DEFAULT_ASCII_FONT)
	: GUI_Object(digital_num*font->WidthDefault+1,font->Height,font) //缺省按16x7的字体
	,  pVal(val), DigitalNum(digital_num), Decimal(decimal)
	{
		Enable = false;
	}

	int *pVal;	///<控件绑定的数值的指针，限制为int型。
	u8 DigitalNum;	///<占用字符位数，包含小数点和负号
	u8 Decimal; 	///<小数个数	
	
	static char NumStrBuf[GUI_NUM_STR_LEN];	//用于将数字转成字符串的共用缓冲区
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
	///只构造了基类的参数，其它参数必须在使用前设置好
	GUI_Num(u8 digital_num, const Font_t* font=&DEFAULT_ASCII_FONT)
	: GUI_NumText(NULL, digital_num, 0, font) 
	, Step(1),TuningMode(FineTuning)
	{
		Enable = true;
		Align = AlignRight;
	}
#endif	
	///自动模式的枚举
	typedef enum{
		Mode_AutoMinus	=-2,	///<自动减小
		Mode_Minus		=-1,	///<手动减小
		Mode_None		= 0,	///<无自动
		Mode_Plus		= 1,	///<手动增加
		Mode_AutoPlus	= 2,	///<自动增加
	}Mode_t;
	
    int Min;    ///<数值的下限
    int Max;    ///<数值的上限
    u32 Step;		///<递增值，缺省为1
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


	//编码器变速调节
	
	///步进某个值step
	int OnStep(int step);
	///控件对应值乘以2
	inline int Mul2() { 
		return(OnStep(*pVal));
	}
	///控件对应值降以2
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
		
    int *pVal;       		//控件对应的变量
    u8 Num;        			//列表元素个数
    u8 Id;         			//当前元素ID
    const int *ValList; 	//数值列表
    const char **StrList;	//对应显示的字符串数组
	virtual void Display();
	virtual bool OnEdit();
	///根据当前值在列表里查找得到Id。
	void InitId();
 };


/**************************************************************
 Bool型控件，选中后按确认切换显示两个字符串，对应0/1两个值
***************************************************************/
class GUI_Bool : public GUI_Object
{
public:
	GUI_Bool(Pos_t w, Pos_t h,  int *val, const char ** const str_list)
	: GUI_Object(w,h)
	, pVal(val), StrList(str_list)
	{
	}
    s32 *pVal;   //控件关联的变量
    const char **StrList;
	virtual void Display();
	virtual bool OnEdit();
};

#define CHECKBOX_W	16
#define CHECKBOX_H	14
/**************************************************************
 CheckBox型控件，选中后按确认切换显示CheckBox图标两种状态，对应0/1两个值
***************************************************************/
class GUI_CheckBox : public GUI_Object
{
public:
	GUI_CheckBox(int *val);
    s32 *pVal;   //控件关联的变量
    
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


///Slider控件线条厚度
#define SLIDER_LINE_THICKNESS		3
///Slder控件滑块宽度
#define SLIDER_Width		4
///Slder控件滑块宽度
#define SLIDER_HEIGHT		9
//Todo：Min，Max，与数值控件的关联，暂时先画个线条占个位置
class GUI_Slider : public GUI_Object
{
public:
	GUI_Slider(Pos_t w, int *val);
	s32 *pVal;   //控件关联的变量

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
	
	s32 Val;   //控件关联的变量
	int Min;    ///<数值的下限
    int Max;    ///<数值的上限
	
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
用于组织页面GUI_Page里的控件列表。同一个页面的控件数量有限，在不同设置情况下控件有所不同。
用c++类库的list有点复杂，还是弄个数组来组织。构造时new生成一个GUI_Object的指针数组，数组大小为MaxNum。
限定GUI_Object都用new生成，这样删除控件对像的方法统一。
简单起见数组中的指针从0开始顺序排列，中间没有空档。
这里提供简单的添加删除成员函数，具体维护由各页面自行处理。
通常的思路是不变的控件在页面构造函数里添加，变化的控件在Init()里添加、删除。
**/
class GUI_ObjList {
	
public:
	GUI_ObjList(int max_num);
	~GUI_ObjList();
	
	int MaxNum;	///< 最大控件数量，用于预先分配空间指针数组的空间
	int Num;	///< 当前控件数量
	GUI_Object ** _ObjList;///<控件指针数组，动态创建

public:
	///delete控件列表指向的对像，清空列表
	void Clear();
	//delete从i到j的控制对像
	void Delete(int i,int j);
	
	void Delete(int i);
	
	void Set(GUI_Object *p, int i);
	
	void Append(GUI_Object *p);
	//操作符[]直接返回列表中的指针
	inline GUI_Object * operator [](int i) {
		return _ObjList[i];
	}
};

}//namespace gui {
#endif//_GUI_OBJECT_H_

