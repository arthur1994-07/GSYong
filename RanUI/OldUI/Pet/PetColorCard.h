#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicProgressBar;
class CBasicTextBox;
class CBasicTextButton;
class CUIEditBox;
class GLGaeaClient;

class CPetColorCard : public CUIWindowEx
{
	static const int nLIMIT_NUMBER_CHAR;
	static const int nMAX_COLOR;
	static const float fRGB_RATIO;

	enum
	{
		PETCOLORCARD_R_PROGRESS = ET_CONTROL_NEXT,
		PETCOLORCARD_G_PROGRESS,
		PETCOLORCARD_B_PROGRESS,

		PETCOLORCARD_R_EDITBOX,
		PETCOLORCARD_G_EDITBOX,
		PETCOLORCARD_B_EDITBOX,

		PETCOLORCARD_OK,
		PETCOLORCARD_CANCEL,
	};

public:
	CPetColorCard(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPetColorCard();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, int x );
	void UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal );
	BOOL UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal );

	void TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg );
	void TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg );

	void InitUIControl();
	void ClearEditBox();
	void RegulateRGB();

private:
	CBasicProgressBar * m_pRProgressBar;
	CBasicProgressBar * m_pGProgressBar;
	CBasicProgressBar *	m_pBProgressBar;

	CUIEditBox * m_pREditBox;
	CUIEditBox * m_pGEditBox;
	CUIEditBox * m_pBEditBox;

	CUIControl * m_RGBControl;

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

};