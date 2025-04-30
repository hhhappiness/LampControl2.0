#ifndef _GUI_NUM_PAGE_HPP_
#define _GUI_NUM_PAGE_HPP_


#include "GUI_page.hpp"

namespace gui {
class GUI_NumPage : public GUI_Page
{

private:  
    GUI_NumPage();   //���캯����˽�е�  

    GUI_NumPage(const GUI_NumPage &);  
    GUI_NumPage & operator = (const GUI_NumPage &);  

	GUI_Page * bakPage;

	enum{
		iValue,
		MaxObjNum
	};
	enum{
		MaxDigitalNum=8, ///<��������ַ���������С����.8λ��������7�����ּ�һ��С����
		FontWidth=11,	///������11����24x11���������
	};

	GUI_Num * pNumObj;
	int Val;
	u32 InvertState;
	int CurrDigital;
	u32 InvertDigitalState;
	u32 Step;	///<����ֵ���԰�GUI_Num��StepֵΪ��ֵ��������仯
	int StepId,MinStepId,MaxStepId;
	GUI_Num::Mode_t AutoMode;
	int HoldRepeatKey;
	int AutoTimes;
	int ReleaseTimes;
	u32 TAutoMode;	//�Զ�ģʽ�ļ�ʱ��
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
	void InvertDigital(u32 i);///<����һ�����֣�˳���Ǵӵ͵���(�ҵ���)
	void MoveDigital(int dir);
//	void OnChangeTuningMode(bool changed=true);
public:	
    static GUI_NumPage & GetInstance()  
    {  
        static GUI_NumPage instance;   //�ֲ���̬����  
        return instance;  
    } 	
	
	void Attach(GUI_Num *pN);
	void UnInit();
	virtual void Show();
	virtual int Loop();
};


}

#endif
