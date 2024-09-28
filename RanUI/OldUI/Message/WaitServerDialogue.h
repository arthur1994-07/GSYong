#pragma	once

#include "../../../EngineLib/GUInterface/UIModal.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;

enum
{
	WAITSERVER_CLOSEGAME,
	WAITSERVER_TOLOBY,
	WAITSERVER_TOFRIEND,
	WAITSERVER_TO_WORLDBATTLE,
};

const DWORD UIMSG_MOUSEIN_OKBUTTON = UIMSG_USER1;

const float fDEFAULT_TIME_LEFT1 = 5.0f;

class CWaitServerDialogue : public CUIModal
{
	enum
	{
		WAITSERVER_OKBUTTON = NO_ID + 1,
	};	

	int		m_nACTION;
	float	m_fTIME_LEFT;

	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pMoveOKButton;

	CString m_strMessage;

public:
	CWaitServerDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CWaitServerDialogue ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer );	
	void	DoACTION_PLAY ();
	int		GetActionTYPE ()	{ return m_nACTION; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	CInnerInterface* m_pInterface;

};