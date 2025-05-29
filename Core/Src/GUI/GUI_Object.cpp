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
	

///����������������λͼ
Bitmap::Bitmap(Pos_t w, Pos_t h, u8 * buf, u8 startbit)
: Width(w), Height(h), StartBit(startbit), Alloc(0), pPix(buf)
{}

///������Bitmap���죬������Ϣ��������ָ�롣
Bitmap::Bitmap(const Bitmap & bitmap)
: Width(bitmap.Width), Height(bitmap.Height), StartBit(bitmap.StartBit), Alloc(bitmap.Alloc), pPix(bitmap.pPix)
{
	if(bitmap.Alloc) while(1);//���ڰ�ȫ��������ֹ��Դλͼ�Ƕ�̬�����
}

///����ָ����Сλͼ����̬���������ڴ�
Bitmap::Bitmap(Pos_t w, Pos_t h)
: Width(w), Height(h), StartBit(0)
{
	pPix = new u8[w*((h+7)/8)];
	Alloc = 1;
}

Bitmap::~Bitmap(){
	if(Alloc) delete [] pPix;
}
	

///����ؼ���ʾ��
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

///�ڿؼ���ʾ�������뷽ʽ��ʾ�ַ���������߶Ⱥ�y��8�ı��������������������ؿհ�
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
	
	//Ĩ���������������м��
	pCurrPage->LineHWhite(x,y,Width);
	pCurrPage->LineHWhite(x,y+Height-1,Width);
}

///���¿ؼ���ʾ��Lcd
void  GUI_Object::Update()
{
	pCurrPage->Update(x,y,Width,Height);
}

char GUI_NumText::NumStrBuf[GUI_NUM_STR_LEN];	//���ڽ�����ת���ַ����Ĺ��û�����

///�ı��ؼ���ʾ
void GUI_Text::Display()
{
	if(ClearBeforeShow) Clear();
	DisplayStr(Str);
}

///��ֵ�ؼ���ʾ
void GUI_NumText::Display(){
	if(Decimal == 16){//16����
		ToHexStr();
	}else{
		ToDecStr();
	}
	if(ClearBeforeShow) Clear();
	DisplayStr(NumStrBuf);
}


///���ݵ�ǰֵ���б�����ҵõ�Id��
void GUI_List::InitId(){
	int i;
#if 0	//��ʼ��ʱ��ֵ�������б��е�ֵ���ܲ�����ѡ��ӽ���һ��
	if (ValList[0] < ValList[1]) {//��С��������
		for (i=Num-1;i>=0;i--) {//��ֵ�Ӵ�Сƥ��
			if (*pVal>=ValList[i]){
				Id=i;
				*pVal=ValList[i];
				break;//����ѭ��
			}
		}
	}else {
		for (i=Num-1;i>=0;i--) {//��ֵ��С����ƥ��
			if (*pVal<=ValList[i]){
				Id=i;
				*pVal=ValList[i];
				break;//����ѭ��
			}
		}
	}
#else	//��ֵ�������б������������ѡ�е�һ��ֵ
	if (ValList[0] < ValList[1]) {//��С��������
		for (i=Num-1;i>=0;i--) {//��ֵ�Ӵ�Сƥ��
			if (*pVal==ValList[i]){
				Id=i;
				break;//����ѭ��
			}
		}
	}	
#endif		
	if (i<0) {//û���ҵ�ƥ��
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

///����ȱʡ�༭��������������������
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
	if(OnChange == NULL){//ȷ�Ϻ����Ч��
		if(Ret != BakVal){//ֵ�����仯
			if(OnOk !=NULL){
				//��OnOk��У��ֵ����Ч�Բ���ֵ���������ﲻ�ô���
				if(!OnOk(Ret)){
					//nothing to do 
				}else{
					//nothing to do 
				}
			}else{
				*pVal = Ret;//�����︳ֵ
			}	
		}		
	}else{//ֵ�Ѿ�У�������Ч���������ﲻ�ô���
		//nothing to do 
	}
	Display();
	Update();
	Delay_100ms(3);
	return true;
}

int GUI_Num::OnStep(int step){
	int bak=*pVal;//���ݾ�ֵ
	int Ret = StepS32(*pVal,step,Min,Max);
	if(OnChange != NULL){ ///���Ҫ������Ч
		if(!OnChange(Ret)){
			return bak;//���ؾ�ֵ��OnUpdate���뱣��pVal��ֵ����
		}else{
			return Ret;//������ֵ��OnUpdate�Ѿ�����pVal��ֵ
		}
	}else{
		*pVal = Ret;//ֱ�Ӹ��ƣ�����pValָ��Ĳ���ԭʼ��ֵ������GUI_NumPage�ｨ����ʱ����
		return Ret;///ȷ���˳��༭����Ч
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
GUI_CheckBox���
**/
//CheckBox����״̬��ͼ�����ء��ڶ���ͼ�������
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

///CheckBox����״̬��ͼ��
const Bitmap GUI_CheckBox::Icon_CheckBox[2]={
	Bitmap(CHECKBOX_W, CHECKBOX_H, (u8 *)CheckBox[0], 1),
	Bitmap(CHECKBOX_W, CHECKBOX_H, (u8 *)CheckBox[1], 1),	
};

GUI_CheckBox::GUI_CheckBox(int *val)
: GUI_Object(CHECKBOX_W,CHECKBOX_H)	//��С�̶�Ϊͼ��Ĵ�С
, pVal(val)
{
}

///��ʾͼ��
void GUI_CheckBox::Display()
{
	if(*pVal==0){
		pCurrPage->DispBmp(x,y+1,Icon_CheckBox[0],(SelectMode == Selected) ? 0xFF : 0);
	}else{
		pCurrPage->DispBmp(x,y+1,Icon_CheckBox[1],(SelectMode == Selected) ? 0xFF : 0);
	}
}

///���س�ֱ���л�״̬
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

///���س�ֱ���л�״̬
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
: GUI_Object(w,16)//�߶���ʱ�̶�Ϊ16
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

///��ָ��max_num����һ���յĿؼ��б�
GUI_ObjList::GUI_ObjList(int max_num) : MaxNum(max_num) ,Num(0), _ObjList(NULL)
{
	if(MaxNum>0){
		//�����б����ÿռ�
		_ObjList = new GUI_Object *[MaxNum];
		//����ָ��ָ���
		for(int i=0;i<MaxNum;i++){
			_ObjList[i] = NULL;
		}		
	}
}

GUI_ObjList::~GUI_ObjList()
{
	//ɾ�����пؼ���
	Clear();
	//�ͷ��б���ռ�ռ�
	if(_ObjList!=NULL){
		delete [] _ObjList;
	}		
}


///delete�ؼ��б�ָ��Ķ�������б�
void GUI_ObjList::Clear(){
	for(int i=0;i<MaxNum;i++){
		Delete(i);
	}
	Num = 0;
}	
//delete��i��j���б�
void GUI_ObjList::Delete(int i,int j)
{
	for(;i<j;i++){
		Delete(i);
	}
}

void GUI_ObjList::Delete(int i){
	if(_ObjList[i] != NULL){
		delete _ObjList[i];	//Ŀǰ�������пؼ�����new������
		_ObjList[i] = NULL;
		Num --;
	}		
}

///���ÿؼ�����i����֮ǰδռ�ã�ֱ�����á������ռ�ã���deleteԭ����
void GUI_ObjList::Set(GUI_Object *p, int i){
	if(_ObjList[i]==NULL) {
		Num++;
	}else{
		delete _ObjList[i];
	}
	_ObjList[i] = p;
}

///����¿ؼ�ָ�뵽δβ
void GUI_ObjList::Append(GUI_Object *p){
	if(Num<MaxNum){
		_ObjList[Num] = p;
		Num++;
	}
}

}//namespace gui {

