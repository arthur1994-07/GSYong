#pragma once
#include "../../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBoxEx;
class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class RnButton;

class RnAttendanceQuestSlot : public CUIGroup
{


public:
	RnAttendanceQuestSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~RnAttendanceQuestSlot();

public:
	enum 
	{
		ATTENDANCE_DAY_OF_THE_WEEK = NO_ID + 1,
		ATTENDANCE_DAY_EVENT,
		ATTENDANCE_ITEM_IMAGE,
		ATTENDANCE_REWARD_BUTTON,
		ATTENDANCD_SELECT_QUEST_BUTON,
	};

	void	CreateSubControl ();


protected:
	GLGaeaClient*	m_pGaeaClient;
	CInnerInterface*	m_pInterface;
	CBasicTextBoxEx*	m_pCourseTextBox;
	CBasicTextBoxEx*	m_pQuestNameTextBox;
	CBasicTextBoxEx*	m_pMoneyTextBox;
	CBasicTextBoxEx*	m_pExpTextBox;
	CBasicTextBoxEx*	m_pRewardTextBox;
	CItemImageEx*	m_pItemImage;
	DWORD m_dwQuestId;
	RnButton*	m_pRewardButton;
	RnButton*	m_pSelectButton;

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	void SetInfoText( const CString strCourse = "", const D3DCOLOR& CourseTextColor = NS_UITEXTCOLOR::WHITE,
					const CString strTitle = "", const D3DCOLOR& TitleTextColor = NS_UITEXTCOLOR::WHITE,
					const DWORD dwMoney = 0, const D3DCOLOR& MoneyTextColor = NS_UITEXTCOLOR::WHITE,
					const LONGLONG lnExp = 0, const D3DCOLOR& ExpTextColor = NS_UITEXTCOLOR::WHITE,
					const CString strRewardItem = "", const D3DCOLOR& RewardItemTextColor = NS_UITEXTCOLOR::WHITE,
					DWORD sItemID = UINT_MAX);
	void ClearSlot();
	void SetVisibleSingleReWardButton( bool bVisible );
	void SetVisibleSingleSelectButton( bool bVisible );
	void SetQuestId(DWORD dwQuestId);
};