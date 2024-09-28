#pragma	once

#include "../EngineLib/GUInterface/BasicButton.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"

class CBasicButton;
class CBasicTextBox;

class CTextButtonUnity : public CUIGroup
{
private:
	enum
	{
		ASBASIC_BUTTON = NO_ID + 1,
		ASBASIC_BUTTON_IN,
		ASBASIC_BUTTON_CLICK,
	};

private:
	CString				m_strText;	
	D3DCOLOR			m_dwColor;

	BOOL				m_bRadio;
	bool				m_bFlip;
	BOOL				m_bDisAble;

	BOOL				m_bButtonInfo;

	WORD				m_KeyCode;
	WORD				m_KeyCodeEx;

	CUIControl*			m_pButton;
	CUIControl*			m_pButtonIn;
	CUIControl*			m_pButtonClick;
	CBasicTextBox*		m_pButtonText;
	CUIControl*			m_pButtonTextBack;

public:
	CTextButtonUnity (EngineDeviceMan* pEngineDevice);
	virtual	~CTextButtonUnity ();

public:
	void	CreateBaseButton ( const TCHAR* szButtonControl );
	void	CreateClickButton ( 
		const TCHAR* szButtonControl, 
		WORD wFlipMode = CBasicButton::CLICK_FLIP);
	void	CreateMouseInButton ( const TCHAR* szButtonControl );
	void	CreateTextBox ( 
		const TCHAR* szButtonControl, 
		const TCHAR* szText ,
		WORD nTextAlign = TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );

public:
	void	SetOneLineText( 
		const CString& strTemp, 
		const D3DCOLOR& TextColor = NULL );
	void	SetRadioButton( BOOL bRadio )	{ m_bRadio = bRadio; }
	void	ResetButton();

	void	SetDisable( BOOL bDisAble );
	BOOL	GetDisable()					{ return m_bDisAble; }

	void	SetFlip( bool bFlip );
	bool	IsFlip()						{ return m_bFlip; }

	void SetShortcutKey ( WORD nKeyCode, WORD nKeyCodeEx = NULL )
	{ 
        m_KeyCode = nKeyCode;
        m_KeyCodeEx = nKeyCodeEx;
    }

public:
	virtual void Update ( 
		int x, 
		int y, 
		BYTE LB, 
		BYTE MB, 
		BYTE RB, 
		int nScroll, 
		float fElapsedTime, 
		BOOL bFirstControl );

};