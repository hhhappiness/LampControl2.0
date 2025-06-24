#ifndef _GUI_SPEED_HPP_
#define _GUI_SPEED_HPP_

#include "GUI_Object.hpp"
#include "GUI_Display.hpp"
#include "AppParaCommon.h"
#include "Fonts.h"

namespace gui {
///专为频率显示、调节定义的类，因为它字号不一样，并且操作方式与一般的数值控件不一样

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
        static GUI_Speed instance;   //局部静态变量  
        return instance;  
    } 	
	void SetDefaultWidth();

};

}

#endif //_GUI_SPEED_HPP_
