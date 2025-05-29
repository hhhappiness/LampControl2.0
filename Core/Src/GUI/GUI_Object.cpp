#include "GUI_Object.hpp"

#include "GUI_Page.hpp"
#include "Icons.hpp"
#include "Key.h"
#include "GUI_ListPage.hpp"
#include "GUI_NumPage.hpp"
#include "rtc_int.h"
#include "ByteConstSymbol.h"

extern void Delay_100ms(int i);
namespace gui {
	

///由已有数据区构造位图
Bitmap::Bitmap(Pos_t w, Pos_t h, u8 * buf, u8 startbit)
: Width(w), Height(h), StartBit(startbit), Alloc(0), pPix(buf)
{}

///从现有Bitmap构造，复制信息及缓冲区指针。
Bitmap::Bitmap(const Bitmap & bitmap)
: Width(bitmap.Width), Height(bitmap.Height), StartBit(bitmap.StartBit), Alloc(bitmap.Alloc), pPix(bitmap.pPix)
{
	if(bitmap.Alloc) while(1);//存在安全隐患，禁止来源位图是动态分配的
}

///创建指定大小位图，动态分配像素内存
Bitmap::Bitmap(Pos_t w, Pos_t h)
: Width(w), Height(h), StartBit(0)
{
	pPix = new u8[w*((h+7)/8)];
	Alloc = 1;
}

Bitmap::~Bitmap(){
	if(Alloc) delete [] pPix;
}
	

///清除控件显示区
inline void GUI_Object::Clear()
{
	pCurrPage->Fill8(x,y,Width,Height,0x00);
	
}

/*
inline void GUI_Object::SetAligh(Align_t al)
{
	if((al != AlignLeft)||(al != AlignMid) ||(al != AlignRight))
		return ;
	Align = al;
}
*/

///在控件显示区按对齐方式显示字符串，假设高度和y是8的倍数，顶底至少两个像素空白
inline void GUI_Object::DisplayStr(const char *str)
{
	
	if(Font!= pCurrPage->pFontASCII){
		const Font_t *bak;
		bak = pCurrPage->pFontASCII;
		pCurrPage->pFontASCII = Font;
		pCurrPage->DispStr8Align(x,y,Width, Height, Align, str,(SelectMode == Selected) ? 0xFF : 0);
		pCurrPage->pFontASCII = bak;
	}else{
		pCurrPage->DispStr8Align(x,y,Width, Height, Align, str,(SelectMode == Selected) ? 0xFF : 0);
	}
	
	//抹掉上下两行留出行间距
	pCurrPage->LineHWhite(x,y,Width);
	pCurrPage->LineHWhite(x,y+Height-1,Width);
}

///更新控件显示到Lcd
void  GUI_Object::Update()
{
	pCurrPage->Update(x,y,Width,Height);
}

char GUI_NumText::NumStrBuf[GUI_NUM_STR_LEN];	//用于将数字转成字符串的共用缓冲区

///文本控件显示
void GUI_Text::Display()
{
	if(ClearBeforeShow) Clear();
	DisplayStr(Str);
}

///数值控件显示
void GUI_NumText::Display(){
	if(Decimal == 16){//16进制
		ToHexStr();
	}else{
		ToDecStr();
	}
	if(ClearBeforeShow) Clear();
	DisplayStr(NumStrBuf);
}


///根据当前值在列表里查找得到Id。
void GUI_List::InitId(){
	int i;
#if 0	//初始化时的值可能与列表中的值可能不符，选最接近的一个
	if (ValList[0] < ValList[1]) {//从小到大排列
		for (i=Num-1;i>=0;i--) {//按值从大到小匹配
			if (*pVal>=ValList[i]){
				Id=i;
				*pVal=ValList[i];
				break;//跳出循环
			}
		}
	}else {
		for (i=Num-1;i>=0;i--) {//按值从小到大匹配
			if (*pVal<=ValList[i]){
				Id=i;
				*pVal=ValList[i];
				break;//跳出循环
			}
		}
	}
#else	//初值必须与列表相符，不符就选中第一个值
	if (ValList[0] < ValList[1]) {//从小到大排列
		for (i=Num-1;i>=0;i--) {//按值从大到小匹配
			if (*pVal==ValList[i]){
				Id=i;
				break;//跳出循环
			}
		}
	}	
#endif		
	if (i<0) {//没有找到匹配
		Id=0;
		*pVal=ValList[0];
	}		
}

void GUI_List::Display()
{
	if(ClearBeforeShow) Clear();
	DisplayStr(StrList[Id]);	
}


void GUI_Bool::Display()
{
	if(ClearBeforeShow) Clear();
	DisplayStr(StrList[*pVal]);	
}

void GUI_Icon::Display()
{
	pCurrPage->DispBmp(x,y,*pIcon,(SelectMode == Selected) ? 0xFF : 0);
}

const u32 GUI_Num::DefaultStupList[3]={1,5,10};
const u32 GUI_Num::DefaultKeepList[3]={1,1,2};

///调用缺省编辑界面操作，子类可以重载
 int GUI_Num::OnEditPage()
{
	GUI_NumPage & NumPage = GUI_NumPage::GetInstance();
	NumPage.Attach(this);
	NumPage.Show();
	
	return NumPage.Loop();	
}
bool GUI_Num::OnEdit()
{
	int BakVal = *pVal;
	int Ret = OnEditPage();	
	PwrKeyDisableFlag = 0;
	if(OnChange == NULL){//确认后才生效的
		if(Ret != BakVal){//值发生变化
			if(OnOk !=NULL){
				//由OnOk来校验值的有效性并赋值或撤销，这里不用处理
				if(!OnOk(Ret)){
					//nothing to do 
				}else{
					//nothing to do 
				}
			}else{
				*pVal = Ret;//在这里赋值
			}	
		}		
	}else{//值已经校验过，生效或撤销，这里不用处理
		//nothing to do 
	}
	Display();
	Update();
	Delay_100ms(3);
	return true;
}

int GUI_Num::OnStep(int step){
	int bak=*pVal;//备份旧值
	int Ret = StepS32(*pVal,step,Min,Max);
	if(OnChange != NULL){ ///如果要立即生效
		if(!OnChange(Ret)){
			return bak;//返回旧值，OnUpdate必须保持pVal的值不变
		}else{
			return Ret;//返回新值，OnUpdate已经更新pVal的值
		}
	}else{
		*pVal = Ret;//直接复制，这里pVal指向的不是原始绑定值，是在GUI_NumPage里建的临时变量
		return Ret;///确认退出编辑再生效
	}
}

void GUI_Progress ::Display(){
	Clear();
	pCurrPage->Rectangle(x,y,Width,Height);
	int x0;
	int w = Width -4;
	int End = x + Width - 2;
	int Range = Max - Min;
	x0= x+2 + w - ((Val-Min)*w + Range/2)/Range;
	pCurrPage->Fill8(x0,y,  End - x0,8, XXXXXX_X);
	pCurrPage->Fill8(x0,y+8,End - x0,8, X_XXXXXX);
	
}

#define _LIST_MAX_NUM 3

bool GUI_List::OnEdit()
{
	GUI_ListPage * p = new GUI_ListPage(*this);
	p->Init();
	p->Show();
	int Ret = p->Loop();
	if(Ret != Id){
		Id = Ret;
		*pVal = ValList[Id];
		Display();
		Update();
		return true;
	}
	return false;

}

/**
GUI_CheckBox相关
**/
//CheckBox两个状态的图标像素。第二个图标带个√
const u8 GUI_CheckBox::CheckBox[2][CHECKBOX_W*(CHECKBOX_H+7/8)]={
{
0x00,0xFC,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xFC,0x00,
0x00,0x3F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x3F,0x00,
},
{
0x00,0xFC,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x84,0xC4,0x64,0x34,0x14,0xFC,0x00,
0x00,0x3F,0x21,0x23,0x26,0x2C,0x2C,0x26,0x23,0x21,0x20,0x20,0x20,0x20,0x3F,0x00,
},
};

///CheckBox两个状态的图标
const Bitmap GUI_CheckBox::Icon_CheckBox[2]={
	Bitmap(CHECKBOX_W, CHECKBOX_H, (u8 *)CheckBox[0], 1),
	Bitmap(CHECKBOX_W, CHECKBOX_H, (u8 *)CheckBox[1], 1),	
};

GUI_CheckBox::GUI_CheckBox(int *val)
: GUI_Object(CHECKBOX_W,CHECKBOX_H)	//大小固定为图标的大小
, pVal(val)
{
}

///显示图标
void GUI_CheckBox::Display()
{
	if(*pVal==0){
		pCurrPage->DispBmp(x,y+1,Icon_CheckBox[0],(SelectMode == Selected) ? 0xFF : 0);
	}else{
		pCurrPage->DispBmp(x,y+1,Icon_CheckBox[1],(SelectMode == Selected) ? 0xFF : 0);
	}
}

///按回车直接切换状态
bool GUI_CheckBox::OnEdit()
{
	int NewVal =  (*pVal==0) ? 1 : 0;
	bool Changed;
	if(OnOk){
		if(OnOk(NewVal)){
			Changed = true;
		}else{
			Changed = false;
		}
	}else{
		*pVal = NewVal;
		Changed = true;
	}
	Display();
	Update();
	
	return Changed;
}

///按回车直接切换状态
bool GUI_Bool::OnEdit()
{
	int NewVal =  (*pVal==0) ? 1 : 0;
	bool Changed;
	if(OnOk){
		if(OnOk(NewVal)){
			Changed = true;
		}else{
			Changed = false;
		}
	}else{
		*pVal = NewVal;
		Changed = true;
	}
	Display();
	Update();
	
	return Changed;
}


GUI_Slider::GUI_Slider(Pos_t w, int *val)
: GUI_Object(w,16)//高度暂时固定为16
, pVal(val)
{
}
void GUI_Slider::Display()
{
	u8 yMid = y+Height/2-1;
	pCurrPage->LineH(x,yMid-1,Width);
	pCurrPage->LineH(x,yMid  ,Width);
	pCurrPage->LineH(x,yMid+1,Width);
}
bool GUI_Slider::OnEdit()
{
	return true;
}

///按指定max_num构造一个空的控件列表
GUI_ObjList::GUI_ObjList(int max_num) : MaxNum(max_num) ,Num(0), _ObjList(NULL)
{
	if(MaxNum>0){
		//分配列表所用空间
		_ObjList = new GUI_Object *[MaxNum];
		//所有指针指向空
		for(int i=0;i<MaxNum;i++){
			_ObjList[i] = NULL;
		}		
	}
}

GUI_ObjList::~GUI_ObjList()
{
	//删除已有控件，
	Clear();
	//释放列表所占空间
	if(_ObjList!=NULL){
		delete [] _ObjList;
	}		
}


///delete控件列表指向的对像，清空列表
void GUI_ObjList::Clear(){
	for(int i=0;i<MaxNum;i++){
		Delete(i);
	}
	Num = 0;
}	
//delete从i到j的列表
void GUI_ObjList::Delete(int i,int j)
{
	for(;i<j;i++){
		Delete(i);
	}
}

void GUI_ObjList::Delete(int i){
	if(_ObjList[i] != NULL){
		delete _ObjList[i];	//目前假设所有控件都是new创建的
		_ObjList[i] = NULL;
		Num --;
	}		
}

///设置控件到第i项。如果之前未占用，直接设置。如果被占用，先delete原来的
void GUI_ObjList::Set(GUI_Object *p, int i){
	if(_ObjList[i]==NULL) {
		Num++;
	}else{
		delete _ObjList[i];
	}
	_ObjList[i] = p;
}

///添加新控件指针到未尾
void GUI_ObjList::Append(GUI_Object *p){
	if(Num<MaxNum){
		_ObjList[Num] = p;
		Num++;
	}
}

}//namespace gui {

