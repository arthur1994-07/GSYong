#pragma once

#include "../Util/UIWindowEx.h"
#include "BasicChat.h"
#include "../../InnerInterface.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_CHAT_NORMAL			= UIMSG_USER1;
const DWORD UIMSG_CHAT_PRIVATE			= UIMSG_USER2;
const DWORD UIMSG_CHAT_PARTY			= UIMSG_USER3;
const DWORD UIMSG_CHAT_CLUB				= UIMSG_USER4;
const DWORD UIMSG_CHAT_ALLIANCE			= UIMSG_USER5;
const DWORD UIMSG_CHAT_SYSTEM			= UIMSG_USER6;
const DWORD UIMSG_CHAT_AREA				= UIMSG_USER7;
const DWORD UIMSG_CHAT_PARTY_RECRUIT	= UIMSG_USER8;

class	CInnerInterface;
class	CBasicButton;
class	CBasicTextBox;

class	CChatOptionWindow : public CUIWindowEx
{
private:
	enum
	{
		CHAT_NORMAL_STATIC = ET_CONTROL_NEXT,
		CHAT_CHANNEL_STATIC,
		CHAT_NORMAL_CHECK,
		CHAT_PRIVATE_CHECK,
		CHAT_PARTY_CHECK,
		CHAT_CLUB_CHECK,
		CHAT_ALLIANCE_CHECK,
		CHAT_SYSTEM_CHECK,
		CHAT_TOALL_CHECK,
		CHAT_AREA_CHECK,
		CHAT_PARTY_RECRUIT_CHECK,

		CHAT_NORMAL_TEXTBOX,
		CHAT_PRIVATE_TEXTBOX,
		CHAT_PARTY_TEXTBOX,
		CHAT_CLUB_TEXTBOX,
		CHAT_ALLIANCE_TEXTBOX,
		CHAT_SYSTEM_TEXTBOX,
		CHAT_TOALL_TEXTBOX,
		CHAT_AREA_TEXTBOX,
		CHAT_PARTY_RECRUIT_TEXTBOX,
		CHAT_OK_BUTTON,
		CHAT_CANCEL_BUTTON	
	};

public:
	CChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CChatOptionWindow();

	void	CreateSubControl();

	DWORD	m_dwChatFlag;

private:
    CBasicButton*	CreateButton( const CString& strKeyword, const CString& strFlipKeyword, const UIGUID& cID );
	CBasicTextBox*	CreateTextBox( const CString& strKeyword, const CString& strText, const UIGUID& cID );

public:
	virtual void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage( UIGUID cID, DWORD dwMsg );
	virtual void	SetVisibleSingle( BOOL bVisible );

	void	InitFlipButton();
	void	UpdateChatFlag();

private:
	CBasicButton*	m_pNormalButton;
	CBasicButton*	m_pPrivateButton;
	CBasicButton*	m_pPartyButton;
	CBasicButton*	m_pClubButton;
	CBasicButton*	m_pAllianceButton;
	CBasicButton*	m_pSystemButton;
	CBasicButton*	m_pToAllButton;
	CBasicButton*	m_pAreaButton;
	CBasicButton*	m_pPartyRecruitButton;

	CBasicTextBox*	m_pNormalTextBox;
	CBasicTextBox*	m_pPrivateTextBox;
	CBasicTextBox*	m_pPartyTextBox;
	CBasicTextBox*	m_pClubTextBox;
	CBasicTextBox*	m_pAllianceTextBox;
	CBasicTextBox*	m_pSystemTextBox;
	CBasicTextBox*	m_pToAllTextBox;
	CBasicTextBox*	m_pAreaTextBox;
	CBasicTextBox*	m_pPartyRecruitTextBox;

};

class MyChatOptionWindow : public IChatOptionWindow, private CChatOptionWindow
{
public:
    MyChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyChatOptionWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
};