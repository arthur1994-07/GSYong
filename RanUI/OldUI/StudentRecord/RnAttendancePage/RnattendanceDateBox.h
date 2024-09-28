#pragma once
#include "../../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../../EngineLib/GUInterface/UIGroup.h"
#include "RnCalenderDefine.h"

//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBoxExLink;
class CInnerInterface;
class CSwapImage;
class GLGaeaClient;

//-----------------------------------------------------------------------------------------------//

class RnattendanceDateBox : public CUIGroup
{

public:
	RnattendanceDateBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnattendanceDateBox();

public:
	enum 
	{
		ATTENDANCE_DAY_BACK_IMAGE = NO_ID + 1,
		ATTENDANCE_DAY_MOUSE_UP_LINE,
		ATTENDANCE_DAY_EVENTTEXTBOX,
	};

public:
	void	         CreateSubControl ();           

public:
	virtual	void	 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetDateBackImage();
	void SetDateImage();
	void SetEventImage();
	void SetDateBackColor();
	void SetDays( const int nDays, bool bIsThisMonth = true, bool bIsAttendance = false );
	void SetEventText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE);
	void SetTodayInfo();

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	CSwapImage* m_pDateBackImage;
	CSwapImage* m_pDateImage[DAYNUM];
	CSwapImage* m_pAttendanceImage; // 출석, 결석
	CSwapImage* m_pDateMouseEventImage;
	CBasicLineBoxSmart* m_pDateBoxMouseOverLine;
	CBasicTextBoxExLink* m_pEventTextBox;

public:
	SDAYDATA m_sDayData;

};
