#pragma once

#include "../../../../EngineLib/GUInterface/UIGroup.h"
#include "RnCalenderDefine.h"
//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBox;
class CInnerInterface;
class CSwapImage;

//-----------------------------------------------------------------------------------------------//
const int MaxEvent = 3;

class RnattendanceDayBox : public CUIGroup
{

public:
	RnattendanceDayBox(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnattendanceDayBox();

public:
	enum 
	{
		ATTENDANCE_DAY_OF_THE_WEEK = NO_ID + 1,
		ATTENDANCE_DAY_EVENT_1,
		ATTENDANCE_DAY_EVENT_2,
		ATTENDANCE_DAY_EVENT_3,
	};

public:
	void	         CreateSubControl ();           

public:
	virtual	void	 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void			 SetDayBackImage();
	void			 SetDayImage();
	void			 SetEventImage();

private:
	CInnerInterface* m_pInterface;
	std::vector<CSwapImage*> m_pDayEventImage;

};
