#include "GUI_Page.hpp"
#include <typeinfo>
#include "GUI_Speed.hpp"
#include "rtc_int.h"

using namespace std;

namespace gui {

///��ǰPage��ָ��
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
				//Update();//����ؼ��ǿ��е�
				Update(p->x,p->y,p->Width,p->Height);
			}
		}
		FocusId=id;
		if(FocusId>=0 && FocusId<ObjList.Num){
			p = ObjList[FocusId];
			if(p->Enable){//�Ƿ�ʹ��
				p->SetSelect(Selected);
				if(draw) {
					p->Display();
					//Update();//����ؼ��ǿ��е�
					Update(p->x,p->y,p->Width,p->Height);
				}
			}
		}
	}
}
	
int GUI_Page::FocusPrevious()
{
	if(FocusId <0 || ObjList.Num==0) return 0; //û�н����û�пؼ�������0
	int NextFocusId = FocusId;
	if(NextFocusId ==0) return 0;//����0���ڵ�һ�����㣬��ʱ�ٰ��ϼ��������˳��˵�
	//���뱣֤��һ���ؼ��ܻ�ý��㣬�����������ѭ����
	while(1) {
		if(NextFocusId ==0){
			NextFocusId = ObjList.Num-1;
		}else{
			NextFocusId = NextFocusId-1;
		}
		if(ObjList[NextFocusId]->Enable) break;//��ǰ�ؼ��Ƿ��ܻ�ý���
	};
	SetFocus(NextFocusId);
	return 1;
}

void GUI_Page::FocusNext()
{
	if(FocusId <0 || ObjList.Num==0) return;
	int NextFocusId = FocusId;
	//���뱣֤��һ���ؼ��ܻ�ý��㣬�����������ѭ����
	while(1) {
		if(NextFocusId ==ObjList.Num-1){
			NextFocusId = 0;
		}else{
			NextFocusId = NextFocusId+1;
		}
		if(ObjList[NextFocusId]->Enable) break;//��ǰ�ؼ��Ƿ��ܻ�ý���
	};	

	SetFocus(NextFocusId);
}

///��Enter����ȱʡ����
int GUI_Page::OnEnter()
{

	if(FocusId <0 || ObjList.Num == 0) return;
	if(typeid(*ObjList[FocusId]) == typeid(GUI_Num)){
		GUI_Num *p = (GUI_Num *)ObjList[FocusId];
		p->OnEdit();
		OnValChange();
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_NumText)){
		GUI_NumText *p = (GUI_NumText *)ObjList[FocusId];
		return  p->OnChosen(); //ȷ��ѡ���ı���ִ�еĲ���
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_Speed)){
		GUI_Speed *p = (GUI_Speed *)ObjList[FocusId];
		PwrKeyDisableFlag = 1;
		p->OnEdit();	
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_List)){
		GUI_List * p = (GUI_List *) ObjList[FocusId];
		if(p->OnEdit()){//����List��ѡ����棬���ֵ�б仯������OnValChange();
			OnValChange();
		}
		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_TextButton)){
		OnClick(FocusId);
		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_IconButton)){
		OnClick(FocusId);
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_CheckBox)){
		GUI_CheckBox * p = (GUI_CheckBox *) ObjList[FocusId];
		p->OnEdit();//�л��ؼ���ֵ��ͼ��
		OnValChange();//����ֵ�ı仯����Ӧ����		
	}else if(typeid(*ObjList[FocusId]) == typeid(GUI_Bool)){
		GUI_Bool * p = (GUI_Bool *) ObjList[FocusId];
		p->OnEdit();//�л��ؼ���ֵ��ͼ��
		OnValChange();//����ֵ�ı仯����Ӧ����
	}	
	
}

void GUI_Page::GetKeyAndKnob(){
	TKey = GetTimerCount();
	Key = GetKey();

}


}//namespace gui {

