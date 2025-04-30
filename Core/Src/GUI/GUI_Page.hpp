#ifndef _GUI_PAGE_HPP_
#define _GUI_PAGE_HPP_

#include "GUI_Object.hpp"
#include "GUI_Display.hpp"
#include "fonts.h"
#include "key.h"
namespace gui {
///ҳ�����
class GUI_Page : public GUI_DisplayBuf
{
public:
	///���캯����ָ�����ؼ������Ա���Ч����RAM
	GUI_Page(int max_num,  const GUI_DisplayBuf & dispbuf=MainDispBuf);
	
	~GUI_Page()
	{

	}
	///ҳ��ؼ��б�
	GUI_ObjList ObjList;
	
	int FocusId;	///<��ǰ�ؼ�
	//ҳ�泣�õı���
	u32 TKey;	///<����GetKey()�ĳ�ʱ����
	int Key;	///<����GetKey()�ķ��ؼ���
	u32 TIdle;	///<���еĳ�ʱ����
	
	///�����Loop�麯�������˸���ѭ������δʹ�á�
	virtual int Loop(){
		while(1);
	}
	
	///�����OnIdle�麯�����պ���
	virtual void OnIdle(){}
	///�����Init�麯�����պ���
	virtual void Init(){}
	///��ʾָ���ؼ������ö�Ӧ�ؼ�Display����
	void Show(int id)
	{
		if(ObjList[id]!=NULL){
			ObjList[id]->Display();
		}
	}
	///�����Show��������ʾ���пؼ�
	virtual void Show()//Show all
	{
		int id;
		for(id=0; id<ObjList.Num; id++){
			Show(id);
		}
	}
	///��ʾָ����ֹ��ŵĿؼ�
	inline void Show(int id_start, int id_end)
	{
		for(; id_start<=id_end; id_start++){
			Show(id_start);
		}
	}
	///��ָ���ؼ���ý���
	void SetFocus(int id, bool draw=true);
	
	void FocusPrevious();
	void FocusNext();
	virtual void OnEnter();	///��Enter����ȱʡ����
	virtual void OnValChange(){} ///�����Ϳؼ�OnEnter���ִ�еĲ���
	virtual void OnClick(int i){};//��ť�ؼ�����ʱ�Ĳ���
	void GetKeyAndKnob();
};

extern GUI_Page * pCurrPage;
extern GUI_Page PopupPage;
}//namespace gui {
#endif
