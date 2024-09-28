#pragma once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextBox;
class CBikeColorSlot;
class CBasicProgressBar;
class CUIEditBox;
class GLGaeaClient;

class CBikeColorChangeWindow : public CUIWindowEx
{
private:

	static const int nLIMIT_NUMBER_CHAR;
	static const int nMAX_COLOR;
	static const float fRGB_RATIO;

	enum
	{
		BIKE_COLOR_R_PROGRESS = ET_CONTROL_NEXT,
		BIKE_COLOR_G_PROGRESS,
		BIKE_COLOR_B_PROGRESS,

		BIKE_COLOR_R_EDITBOX,
		BIKE_COLOR_G_EDITBOX,
		BIKE_COLOR_B_EDITBOX,

		BIKE_COLOR_OK,
		BIKE_COLOR_CANCEL,

		COLOR_MAIN_SLOT_0,
		COLOR_MAIN_SLOT_1,
		COLOR_MAIN_SLOT_2,
		COLOR_MAIN_SLOT_3,
		COLOR_MAIN_SLOT_4,
		COLOR_MAIN_SLOT_5,
		COLOR_SUB_SLOT_0,
		COLOR_SUB_SLOT_1,
		COLOR_SUB_SLOT_2,
		COLOR_SUB_SLOT_3,
		COLOR_SUB_SLOT_4,
		COLOR_SUB_SLOT_5,

		BIKE_COLOR_SLOT = 6,
	};

public:
	CBikeColorChangeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBikeColorChangeWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	CBikeColorSlot* CreateColorSlot ( CString strKeyword, UIGUID ControlID );
	
private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, const char* szText, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicTextBox*	CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID );
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, int x );
	void UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal );
	BOOL UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal );
	void SetProgressBarEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal );

	void TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg );
	void TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg );

	BOOL IsSetColor();
	void SetDefaultColorUIControl();
	void SetColorUIControl();
	void InitUIControl( BOOL bMain, INT nIndex );
	void InitSelect();
	void ClearEditBox();
	void RegulateRGB();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBikeColorSlot* m_pColorMainSlot[ BIKE_COLOR_SLOT ];
	CBikeColorSlot* m_pColorSubSlot[ BIKE_COLOR_SLOT ];

	CBasicProgressBar * m_pRProgressBar;
	CBasicProgressBar * m_pGProgressBar;
	CBasicProgressBar *	m_pBProgressBar;

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

};