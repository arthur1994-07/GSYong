#pragma once

#include "../../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CPartyTenderNotifyButton : public CUIGroup
{
protected:
	enum
	{
		TENDER_NOTIFY_BUTTON = NO_ID + 1,
	};

	enum NOTIFY_TYPE
	{
		TENDER_NOTIFY_NONE = 0,
		TENDER_NOTIFY_DICE = 1,
		TENDER_NOTIFY_MASTERTRANSFER = 2,
	};

public:
	CPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~CPartyTenderNotifyButton();

public:
	void CreateSubControl();
	void ResizeNotify();

	void				ClickNotifyButton();
	virtual	void        Notify( bool bIsNew );

	virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void SetNotifyType( NOTIFY_TYPE emType ) { m_emNotify = emType;}

protected:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

private:
	CBasicButton*       m_pButtonActiveImg;
	CBasicTextBox*		m_pNotify_Text;
	std::string			m_strNotify;

	CUIControl*			m_pNotify_Head;
	CUIControl*			m_pNotify_Body;
	CUIControl*			m_pNotify_Tail;
	CUIControl*			m_pNotify_Arrow;

	NOTIFY_TYPE m_emNotify;
};

class MyPartyTenderNotifyButton : public IPartyTenderNotifyButton, private CPartyTenderNotifyButton
{
public:
	MyPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyPartyTenderNotifyButton()   {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual	void Notify( bool bNotify );
};