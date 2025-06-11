#include "GUI_ListPage.hpp"
#include "ctrl.h"

namespace gui {

///构造函数，创建最多MaxItemNum个GUI_List控件，指向相同的列表，但值是顺序排列。
GUI_ListPage::GUI_ListPage(GUI_List &L)
: GUI_Page(MaxItemNum, SecondDispBuf)
, Pop_y(0), Pop_h(LcmYPixel)
{
	//横向居中，上下顶格
	Pop_w = L.Width+8;
	Pop_x = (LcmXPixel - Pop_w)/2;
	//获取原始控件的信息
	CurrId = L.Id;
	ItemNum = L.Num;
	ValList = L.ValList;
	//当前可以显示哪几项
	GetVisibleItem();
	//创建GUI_List实例
	for(int i=0;i<=EndId-StartId;i++){
		Val[i]=ValList[StartId+i];
		ObjList.Append( new  GUI_List(L.Width, L.Height, &Val[i], L.Num, L.ValList, L.StrList));
		ObjList[i]->SetPos(Pop_x+4, Pop_y+8+16*i);
	}
	//设置焦点，不重绘
	SetFocus(CurrId- StartId,false);
}

///最多只能显3项，按当前焦点和总数量确定当前显示哪几项。
void GUI_ListPage::GetVisibleItem()
{
	if(ItemNum <= MaxItemNum){
		///不足3项就全显。
		StartId = 0; EndId = ItemNum-1; 
	}else{///超过3项
		if(CurrId == 0){
			///如果Id在第0项，显示从0开始
			StartId = 0; EndId = MaxItemNum -1;
		}else if(CurrId == ItemNum -1){
			///如果Id在最后一项，显示最后几项
			StartId = CurrId -2 ; EndId = CurrId;
		}else{
			///如在中间，显示前后各一项
			StartId = CurrId -1 ; EndId = CurrId+1;
		}
	}	
}
	
///光标移到下一项
void GUI_ListPage::NextItem(int dir)
{
	if(dir==1){//下移
		if(CurrId == ItemNum-1) return;
		CurrId ++;
	}else if(dir==-1){//上移
		if(CurrId == 0) return;
		CurrId --;		
	}else{//invalid
		return;
	}
	//重新设定当前可以显示哪几项
	GetVisibleItem();
	//设置焦点
	SetFocus(CurrId- StartId);
	//更新控件对应的值
	for(int i=0; i<ObjList.Num; i++){
		Val[i]=ValList[StartId+i];
		GUI_List * p = (GUI_List * )ObjList[i];
		p->InitId();		
	}
	//更新显示
	GUI_Page::Show();
	Update(Pop_x, Pop_y, Pop_w, Pop_h);	
}

///初始化，备份当原Page指针，用于退出时恢复显示
void GUI_ListPage::Init()
{
	bakPage = pCurrPage;
	pCurrPage = this;
}

///显示一个外框，再显示列表控件
void GUI_ListPage::Show()
{
	Fill8(Pop_x, Pop_y, Pop_w, Pop_h, 0);
	Rectangle(Pop_x+2, Pop_y+2, Pop_w-4, Pop_h-4);
	GUI_Page::Show();
	Update(Pop_x, Pop_y, Pop_w, Pop_h);
}

///按键循环，左右键移动光标，确定选中退出
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
				pCurrPage = bakPage;//退出前恢复pCurrPage指针
				pCurrPage->Update(Pop_x, Pop_y, Pop_w, Pop_h);//恢复遮盖部分的显示
				return(StartId+FocusId);
			}

		}
	}
}

}//namespace gui {
