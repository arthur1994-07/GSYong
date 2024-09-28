#pragma once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Party/GLPartyDefine.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;

class CPartyLinkWindow : public CUIWindowEx
{
private:
	enum
	{
		PARTY_LINK_JOIN_BUTTON = ET_CONTROL_NEXT,
	};

public:
	CPartyLinkWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CPartyLinkWindow ();

	void	CreateSubControl ();
	void	SetPartyLink( int nLinkIndex, const GLPARTY_LINK& sPartyLink );

private:
	CBasicTextBox*	CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID );

public:
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual void SetVisibleSingle(BOOL visible);

private:
	CBasicButton*	m_pJoinButton;

	CBasicTextBox*	m_pSectionTextBox[MAXPARTY];
	CBasicTextBox*  m_pClassTextBox[MAXPARTY];
	CBasicTextBox*	m_pNameTextBox[MAXPARTY];

	GLPARTY_LINK	m_sPartyLink;
	int				m_nLinkIndex;

protected:
	GLGaeaClient*	m_pGaeaClient;

};