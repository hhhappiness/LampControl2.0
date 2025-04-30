#ifndef _GUI_PAGE_HPP_
#define _GUI_PAGE_HPP_

#include "GUI_Object.hpp"
#include "GUI_Display.hpp"
#include "fonts.h"
#include "key.h"
namespace gui {
///页面基类
class GUI_Page : public GUI_DisplayBuf
{
public:
	///构造函数，指明最大控件数，以便有效利用RAM
	GUI_Page(int max_num,  const GUI_DisplayBuf & dispbuf=MainDispBuf);
	
	~GUI_Page()
	{

	}
	///页面控件列表
	GUI_ObjList ObjList;
	
	int FocusId;	///<当前控件
	//页面常用的变量
	u32 TKey;	///<调用GetKey()的超时变量
	int Key;	///<保存GetKey()的返回键码
	u32 TIdle;	///<空闲的超时变量
	
	///基类的Loop虚函数，放了个列循环，从未使用。
	virtual int Loop(){
		while(1);
	}
	
	///基类的OnIdle虚函数，空函数
	virtual void OnIdle(){}
	///基类的Init虚函数，空函数
	virtual void Init(){}
	///显示指定控件，调用对应控件Display函数
	void Show(int id)
	{
		if(ObjList[id]!=NULL){
			ObjList[id]->Display();
		}
	}
	///基类的Show函数，显示所有控件
	virtual void Show()//Show all
	{
		int id;
		for(id=0; id<ObjList.Num; id++){
			Show(id);
		}
	}
	///显示指定起止序号的控件
	inline void Show(int id_start, int id_end)
	{
		for(; id_start<=id_end; id_start++){
			Show(id_start);
		}
	}
	///让指定控件获得焦点
	void SetFocus(int id, bool draw=true);
	
	void FocusPrevious();
	void FocusNext();
	virtual void OnEnter();	///按Enter键的缺省操作
	virtual void OnValChange(){} ///变量型控件OnEnter最后执行的操作
	virtual void OnClick(int i){};//按钮控件按下时的操作
	void GetKeyAndKnob();
};

extern GUI_Page * pCurrPage;
extern GUI_Page PopupPage;
}//namespace gui {
#endif
