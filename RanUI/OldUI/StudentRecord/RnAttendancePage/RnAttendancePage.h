#pragma once

#include "../../Util/UIPage.h"
//#include "../../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBox;
class CInnerInterface;
class RnattendanceCalender;
class RnattendanceDayInfo;
class GLGaeaClient;

//-----------------------------------------------------------------------------------------------//
class RnattendancePage : public CUIPage
{

public:
	RnattendancePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnattendancePage();

public:
	enum 
	{
		ATTENDANCE_CALENDER = PAGE_NEXT,
		ATTENDANCE_DAY_INFO,
	};

public:
	void CreateSubControl ();
	virtual void SetVisibleSingle( BOOL bVisible );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetDayBackImage();
	void SetDayImage();
	void SetEventImage();
	void ResetCalender();
	void SetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void ClearDayInfo();
	void SetSlotLScroolBarUpDate();

private:
	CInnerInterface* m_pInterface;
	RnattendanceCalender* m_pCalender;
	RnattendanceDayInfo* m_pDayInfo;
	GLGaeaClient* m_pGaeaClient;
};
