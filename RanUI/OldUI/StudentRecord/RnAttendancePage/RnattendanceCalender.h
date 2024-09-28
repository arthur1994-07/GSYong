#pragma once

#include "../../../../EngineLib/GUInterface/UIGroup.h"
#include "RnCalenderDefine.h"
//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBox;
class CInnerInterface;
class RnattendanceDayBox;
class RnattendanceDateBox;
class CSwapImage;
class GLGaeaClient;
class RnButton;

//-----------------------------------------------------------------------------------------------//

class RnattendanceCalender : public CUIGroup
{

public:
	RnattendanceCalender(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnattendanceCalender();

public:

	enum EM_WEEKMINMAX
	{
		ATTENDANCE_WEEK_MAX = ATTENDDANCE_WEEK,
		ATTENDDANCE_MONTH_MAX= ATTENDDANCE_MONTH,
	};

	enum 
	{
		ATTENDANCE_DAY_BOX_START = NO_ID + 1,
		ATTENDANCE_WEEK_END = ATTENDANCE_DAY_BOX_START + ATTENDANCE_WEEK_MAX,

		ATTENDANCE_DATE_BOX_START = 100,
		ATTENDANCE_DATE_BOX_END = ATTENDANCE_DATE_BOX_START + ATTENDDANCE_MONTH_MAX,

		ATTENDANCE_DAY_LEFT_BUTTON,
		ATTENDANCE_DAY_RIGHT_BUTTON,
	};


public:
	void	CreateSubControl();           

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetDayBackImage();
	void SetDayImage();
	void SetEventImage();
	void ResetCalender();

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	std::vector<RnattendanceDayBox*> m_pDayBox;
	std::vector<RnattendanceDateBox*> m_pDateBox;
	CSwapImage* m_pMonthImage;
	CSwapImage* m_pYearImage[4];
	CSwapImage* m_pTotalDayImage[2];
	RnButton* m_pPastButton;
	RnButton* m_pFutureButton;

private:
	int m_nYear;
	int m_nMonth;
	int m_nYunbonus;
	int m_nDayCount;
	int m_nMonth_days[12];

public:
	int GetDayOfTheWeek(int year, int month, int date);
	void SetCalenderValue();
	int CheckYUN(int nYear);
	EM_DAY_OF_WEEK GetfirstDayofWeek();
	EM_DAY_OF_WEEK GetLastDayofWeek();
	int GetDayCount();
	EM_DAY_OF_WEEK GetLastDayofWeekBeforeMont();
	int GetDayCountBeforeMonth();

	void SetMonthImage( int nMonth );
	void SetYearImage( int nYear, bool bBlockAndWhite = false );
	void SetTotalDay( int nDay, bool bBlockAndWhite = false );
	void SetCalender();
};
