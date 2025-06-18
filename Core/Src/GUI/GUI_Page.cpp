#include "GUI_Page.hpp"
#include <typeinfo>
#include "GUI_Speed.hpp"
#include "rtc_int.h"

using namespace std;

namespace gui {

///当前Page的指针
GUI_Page * pCurrPage;
	

GUI_Page::GUI_Page(int max_num,  const GUI_DisplayBuf & dispbuf) 
: GUI_DisplayBuf(dispbuf),ObjList(max_num) ,FocusId(-1)
{

}
	

void GUI_Page::SetFocus(int id, bool draw){
	GUI_Object *p;
	if (FocusId!=id) {
		if(FocusId>=0 && FocusId<ObjList.Num){
			p = ObjList[FocusId];
			p->SetSelect(NotSelected);
			if(draw) {
				p->Display();
				//Update();//个别控件是跨行的
				Update(p->x,p->y,p->Width,p->Height);
			}
		}
		FocusId=id;
		if(FocusId>=0 && FocusId<ObjList.Num){
			p = ObjList[FocusId];
			if(p->Enable){//是否使能
				p->SetSelect(Selected);
				if(draw) {
					p->Display();
					//Update();//个别控件是跨行的
					Update(p->x,p->y,p->Width,p->Height);
				}
			}
		}
	}
}
	
int GUI_Page::FocusPrevious()
{
	if(FocusId <0 || ObjList.Num==0) return 0; //没有焦点或没有控件，返回0
	int NextFocusId = FocusId;
	if(NextFocusId ==0) return 0;//返回0则在第一个焦点，此时再按上键，可以退出菜单
	//必须保证有一个控件能获得焦点，否则会陷入死循环。
	while(1) {
		if(NextFocusId ==0){
			NextFocusId = ObjList.Num-1;
		}else{
			NextFocusId = NextFocusId-1;
		}
		if(ObjList[NextFocusId]->Enable) break;//当前控件是否能获得焦点
	};
	SetFocus(NextFocusId);
	return 1;
}

void GUI_Page::FocusNext()
{
	if(FocusId <0 || ObjList.Num==0) return;
	int NextFocusId = FocusId;
	//必须保证有一个控件能获得焦点，否则会陷入死循环。
	while(1) {
		if(NextFocusId ==ObjList.Num-1){
			NextFocusId = 0;
		}else{
			NextFocusId = NextFocusId+1;
		}
		if(ObjList[NextFocusId]->Enable) break;//当前控件是否能获得焦点
	};	

	SetFocus(NextFocusId);
}

///按Enter键的缺省操作
int GUI_Page::OnEnter()
{

	if(FocusId <0 || ObjList.Num == 0) return;
	if(typeid(*ObjList[FocusId]) == typeid(GUI_Num)){
		GUI_Num *p = (GUI_Num *)ObjList[FocusId];
		p->OnEdit();
		OnValChange();
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_NumText)){
		GUI_NumText *p = (GUI_NumText *)ObjList[FocusId];
		return  p->OnChosen(); //确认选中文本后执行的操作
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_Speed)){
		GUI_Speed *p = (GUI_Speed *)ObjList[FocusId];
		PwrKeyDisableFlag = 1;
		p->OnEdit();	
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_List)){
		GUI_List * p = (GUI_List *) ObjList[FocusId];
		if(p->OnEdit()){//调用List的选择界面，如果值有变化，调用OnValChange();
			OnValChange();
		}
		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_TextButton)){
		OnClick(FocusId);
		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_IconButton)){
		OnClick(FocusId);
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_CheckBox)){
		GUI_CheckBox * p = (GUI_CheckBox *) ObjList[FocusId];
		p->OnEdit();//切换控件的值和图标
		OnValChange();//根据值的变化做相应动作		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_Bool)){
		GUI_Bool * p = (GUI_Bool *) ObjList[FocusId];
		p->OnEdit();//切换控件的值和图标
		OnValChange();//根据值的变化做相应动作
	}	
	
}

void GUI_Page::GetKeyAndKnob(){
	TKey = GetTimerCount();
	Key = GetKey();

}


}//namespace gui {

