#include "GUI_ListPage.hpp"
#include "ctrl.h"

namespace gui {

///���캯�����������MaxItemNum��GUI_List�ؼ���ָ����ͬ���б���ֵ��˳�����С�
GUI_ListPage::GUI_ListPage(GUI_List &L)
: GUI_Page(MaxItemNum, SecondDispBuf)
, Pop_y(0), Pop_h(LcmYPixel)
{
	//������У����¶���
	Pop_w = L.Width+8;
	Pop_x = (LcmXPixel - Pop_w)/2;
	//��ȡԭʼ�ؼ�����Ϣ
	CurrId = L.Id;
	ItemNum = L.Num;
	ValList = L.ValList;
	//��ǰ������ʾ�ļ���
	GetVisibleItem();
	//����GUI_Listʵ��
	for(int i=0;i<=EndId-StartId;i++){
		Val[i]=ValList[StartId+i];
		ObjList.Append( new  GUI_List(L.Width, L.Height, &Val[i], L.Num, L.ValList, L.StrList));
		ObjList[i]->SetPos(Pop_x+4, Pop_y+8+16*i);
	}
	//���ý��㣬���ػ�
	SetFocus(CurrId- StartId,false);
}

///���ֻ����3�����ǰ�����������ȷ����ǰ��ʾ�ļ��
void GUI_ListPage::GetVisibleItem()
{
	if(ItemNum <= MaxItemNum){
		///����3���ȫ�ԡ�
		StartId = 0; EndId = ItemNum-1; 
	}else{///����3��
		if(CurrId == 0){
			///���Id�ڵ�0���ʾ��0��ʼ
			StartId = 0; EndId = MaxItemNum -1;
		}else if(CurrId == ItemNum -1){
			///���Id�����һ���ʾ�����
			StartId = CurrId -2 ; EndId = CurrId;
		}else{
			///�����м䣬��ʾǰ���һ��
			StartId = CurrId -1 ; EndId = CurrId+1;
		}
	}	
}
	
///����Ƶ���һ��
void GUI_ListPage::NextItem(int dir)
{
	if(dir==1){//����
		if(CurrId == ItemNum-1) return;
		CurrId ++;
	}else if(dir==-1){//����
		if(CurrId == 0) return;
		CurrId --;		
	}else{//invalid
		return;
	}
	//�����趨��ǰ������ʾ�ļ���
	GetVisibleItem();
	//���ý���
	SetFocus(CurrId- StartId);
	//���¿ؼ���Ӧ��ֵ
	for(int i=0; i<ObjList.Num; i++){
		Val[i]=ValList[StartId+i];
		GUI_List * p = (GUI_List * )ObjList[i];
		p->InitId();		
	}
	//������ʾ
	GUI_Page::Show();
	Update(Pop_x, Pop_y, Pop_w, Pop_h);	
}

///��ʼ�������ݵ�ԭPageָ�룬�����˳�ʱ�ָ���ʾ
void GUI_ListPage::Init()
{
	bakPage = pCurrPage;
	pCurrPage = this;
}

///��ʾһ���������ʾ�б�ؼ�
void GUI_ListPage::Show()
{
	Fill8(Pop_x, Pop_y, Pop_w, Pop_h, 0);
	Rectangle(Pop_x+2, Pop_y+2, Pop_w-4, Pop_h-4);
	GUI_Page::Show();
	Update(Pop_x, Pop_y, Pop_w, Pop_h);
}

///����ѭ�������Ҽ��ƶ���꣬ȷ��ѡ���˳�
int GUI_ListPage::Loop()
{
	TKey = GetTimerCount();
	while(1){
		if(IsTimeOut_ms(TKey,100)){
			wdg();
			TKey = GetTimerCount();
			Key = GetKey();
			switch(Key){
			case KEY_UP_SHOT : 		NextItem(-1); break;
			case KEY_DOWN_SHOT : 	NextItem(1); break;			
			case KEY_ENTER_SHOT : 
				pCurrPage = bakPage;//�˳�ǰ�ָ�pCurrPageָ��
				pCurrPage->Update(Pop_x, Pop_y, Pop_w, Pop_h);//�ָ��ڸǲ��ֵ���ʾ
				return(StartId+FocusId);
			}

		}
	}
}

}//namespace gui {
