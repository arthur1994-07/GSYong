#pragma once
#include "../../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class CInnerInterface;
class RnattendanceDayBox;
class RnattendanceDateBox;
class RnAttendanceQuestSlot;
class CBasicScrollBarEx;
class GLGaeaClient;
class EngineDeviceMan;
class CBasicScrollBar;
class CBasicScrollBarNoTHUMB;

//-----------------------------------------------------------------------------------------------//
class RnattendanceDayInfo : public CUIGroup
{

public:
	RnattendanceDayInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnattendanceDayInfo();

public:
	enum 
	{
		ATTENDANCE_DAY_OF_THE_WEEK = NO_ID + 1,
		ATTENDANCE_DAY_EVENT,
	};

public:
	void CreateSubControl ();           

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetDayBackImage();
	void SetDayImage();
	void SetEventImage();
	void SetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void ClearDayInfo();
	void SetInfoLScroolBarUpDate(CBasicTextBoxEx* pBasicTextBoxEx);
	void SetSlotLScroolBarUpDate();

	void    UpdatePageData();
	void    UpdateSlot();
	void	ClearSlot();
private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	CBasicTextBoxEx* m_pDayText; //³¯Â¥ ex) 5ÀÏ - ¿À´Ã
	//CBasicTextBoxEx* m_pQuest;
	CBasicScrollBarEx*	m_pInfoListScrollBar;
	//CBasicScrollBarEx*	m_pInfoQuestScrollBar;
	CBasicScrollBarNoTHUMB*	m_pInfoQuestScrollBar;
	int m_nQuestListScrollPos;
	std::vector<RnAttendanceQuestSlot*> m_pQuestSlot;
	std::vector<CString> m_vecCstring;
};
