#include "SubPage.hpp"
#include "AppInterface.hpp"

namespace gui {
	
SubPage::SubPage(int max_num) 
: GUI_Page(max_num) 
{
}

int SubPage::Loop()
{
	TKey = GetTimerCount();
	TIdle = GetTimerCount();
	while(1){
		if(IsTimeOut_ms(TKey,100)){
			Key = GetKey();
			if(Key != KEY_NULL){
				switch(Key){
					case KEY_MODE_RELEASE: 	OnClose(0);	return 0;
//					case KEY_DIV2_SHOT : 	return 1;
//					case KEY_MULT_SHOT : 	return -1;
					case KEY_UP_SHOT : 		FocusPrevious(); break;
					case KEY_DOWN_SHOT : 	FocusNext(); break;
					case KEY_ENTER_RELEASE : 
						OnEnter();
						break;
					
				}
			}
		}
		OnIdle();
	}
	
}

void SubPage::OnIdle()
{
	//调用应用接口的空闲操作
	OnIdleSubPage(TIdle);
}

}//namespace gui {
