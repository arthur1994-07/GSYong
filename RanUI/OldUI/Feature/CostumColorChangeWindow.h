#pragma once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextBox;
class CCostumColorSlot;
class CBasicProgressBar;
class CUIEditBox;
class GLGaeaClient;

class CCostumColorChangeWindow : public CUIWindowEx
{
private:

	static const int nLIMIT_NUMBER_CHAR;
	static const int nMAX_COLOR;
	static const float fRGB_RATIO;

	enum
	{
		COSTUM_COLOR_R_PROGRESS = ET_CONTROL_NEXT,
		COSTUM_COLOR_G_PROGRESS,
		COSTUM_COLOR_B_PROGRESS,

		COSTUM_COLOR_R_EDITBOX,
		COSTUM_COLOR_G_EDITBOX,
		COSTUM_COLOR_B_EDITBOX,

		COSTUM_COLOR_OK,
		COSTUM_COLOR_CANCEL,

		COSTUM_COLOR_RANDOM,
		COSTUM_COLOR_RESET,

		COLOR_MAIN_SLOT_0,

		COLOR_SUB_SLOT_0,

		COSTUM_COLOR_SLOT = 1,
	};

public:
	CCostumColorChangeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCostumColorChangeWindow ();

private:
    GLGaeaClient* m_pGaeaClient;
    CCostumColorSlot* m_pColorMainSlot[ COSTUM_COLOR_SLOT ];
    CCostumColorSlot* m_pColorSubSlot[ COSTUM_COLOR_SLOT ];

    CBasicProgressBar * m_pRProgressBar;
    CBasicProgressBar * m_pGProgressBar;
    CBasicProgressBar *	m_pBProgressBar;

    CUIControl*			m_pRProgressBarRestrict;
    CUIControl*			m_pGProgressBarRestrict;
    CUIControl*			m_pBProgressBarRestrict;

    CUIEditBox * m_pREditBox;
    CUIEditBox * m_pGEditBox;
    CUIEditBox * m_pBEditBox;

    INT m_nR;
    INT m_nG;
    INT m_nB;

    INT m_nRBack;
    INT m_nGBack;
    INT m_nBBack;

    INT m_nRegR;
    INT m_nRegG;
    INT m_nRegB;

    BOOL m_bOKButton;

    INT m_nMainSelect;
    INT m_nSubSelect;

private:
    bool m_bRestrict;
    float m_fRestrictMax;
    float m_fRestrictMin;

public:
	void	CreateSubControl ();
	CCostumColorSlot* CreateColorSlot ( CString strKeyword, UIGUID ControlID );

private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, const char* szText, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicTextBox*	CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID );
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, int x );
	void UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal );
	BOOL UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal );
	void SetProgressBarEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal, bool bReset = false );

	void TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg );
	void TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg );

	BOOL IsSetColor();
	void SetDefaultColorUIControl();
	void SetColorUIControl();
	void InitUIControl( BOOL bMain, INT nIndex );
	void InitSelect();
	void ClearEditBox();
	void RegulateRGB();

	bool CheckRestrict( float& fPercent );
	bool CheckChangeOK();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void SetRestrict( bool bRestrict );
	bool IsRestrict()	{ return m_bRestrict; }

	void ShowRestrictBar();
};