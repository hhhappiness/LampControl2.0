#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_


#include "GUI_page.hpp"

namespace gui {
class GUI_NumPage : public GUI_Page
{

private:  
    GUI_NumPage();   //构造函数是私有的  

    GUI_NumPage(const GUI_NumPage &);  
    GUI_NumPage & operator = (const GUI_NumPage &);  

	GUI_Page * bakPage;

	enum{
		iValue,
		MaxObjNum
	};
	enum{
		MaxDigitalNum=8, ///<最大数字字符数，包含小数点.8位可以满足7个数字加一个小数点
		FontWidth=11,	///字体宽度11，与24x11的字体相符
	};

	GUI_Num * pNumObj;
	int Val;
	u32 InvertState;
	int CurrDigital;
	u32 InvertDigitalState;
	u32 Step;	///<步进值，以绑定GUI_Num的Step值为初值，随操作变化
	int StepId,MinStepId,MaxStepId;
	GUI_Num::Mode_t AutoMode;
	int HoldRepeatKey;
	int AutoTimes;
	int ReleaseTimes;
	u32 TAutoMode;	//自动模式的计时器
	u32 TQuit; 
	
	void ChangeStep(int delta);
	void OnStep(int step);
	inline void ShowStepMark();
	void StopScan();
	void StartAutoPlus(bool FromNone);
	void StartAutoMinus(bool FromNone);
	void AutoPlus();
	void AutoMinus();
	void InvertPlus();
	void InvertMinus();

	void ClearInv();
	void InvertDigital(u32 i);///<反显一个数字，顺序是从低到高(右到左)
	void MoveDigital(int dir);
//	void OnChangeTuningMode(bool changed=true);
public:	
    static GUI_NumPage & GetInstance()  
    {  
        static GUI_NumPage instance;   //局部静态变量  
        return instance;  
    } 	
	
	void Attach(GUI_Num *pN);
	void UnInit();
	virtual void Show();
	virtual int Loop();
};


}

#endif
