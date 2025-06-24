#ifndef _GUI_SPEED_HPP_
#define _GUI_SPEED_HPP_

#include "GUI_Object.hpp"
#include "GUI_Display.hpp"
#include "AppParaCommon.h"
#include "Fonts.h"

namespace gui {
///רΪƵ����ʾ�����ڶ�����࣬��Ϊ���ֺŲ�һ�������Ҳ�����ʽ��һ�����ֵ�ؼ���һ��

class GUI_Speed : public GUI_Num
{

private:	
	GUI_Speed();
	GUI_Speed(const GUI_Speed &);  
    GUI_Speed & operator = (const GUI_Speed &);  

	GUI_DisplayBuf &DispBuf;
	int StepId;
	
	inline void SetTo_Freq();
	inline void SetTo_rpm();
	inline void SetTo_mpmin();
public:
	virtual int OnEditPage();
	void OnSetSpeedUnit();

	void Loop(int key);
    void _OnKeyLongUpDown(int key);

    static GUI_Speed & GetInstance()  
    {  
        static GUI_Speed instance;   //�ֲ���̬����  
        return instance;  
    } 	
	void SetDefaultWidth();

};

}

#endif //_GUI_SPEED_HPP_
