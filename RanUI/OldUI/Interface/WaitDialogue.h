#pragma	once

#include "../../../EngineLib/GUInterface/UIModal.h"

class	COuterInterface;
class	CBasicTextBox;
class	CBasicTextButton;

enum
{
	WAITSERVER_DUMMY,
	WAITSERVER_DEL_CHARACTER,
};

const DWORD UIMSG_MOUSEIN_OKBUTTON1 = UIMSG_USER1;

const	float	fDEFAULT_TIME_LEFT = 5.0f;

class CWaitDialogue : public CUIModal
{
	enum
	{
		WAITSERVER_OKBUTTON = NO_ID + 1,
	};	

	int		m_nACTION;
	bool	m_bDO_ACTION;
	float	m_fTIME_LEFT;

	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pMoveOKButton;

	CString m_strMessage;
	UIGUID	m_PrevPageID;

public:
	CWaitDialogue ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CWaitDialogue ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer );	

	void	DoACTION ()							{ m_bDO_ACTION = true; }

	void	SetPrevPageID ( UIGUID ControlID )	{ m_PrevPageID = ControlID; }
	UIGUID	GetPrevPageID ()					{ return m_PrevPageID; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	COuterInterface* m_pInterface;

};