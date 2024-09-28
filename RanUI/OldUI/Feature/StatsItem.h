
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CBasicButton;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BUTTON_DOWN_STATSITEM = UIMSG_USER1;
const DWORD UIMSG_BUTTON_UP_STATSITEM   = UIMSG_USER2;
////////////////////////////////////////////////////////////////////

class CStatsItem : public CUIGroup
{

protected:
	enum
	{
		STATSINFO_BUTTON_DOWN = NO_ID + 1,
		STATSINFO_BUTTON_UP,
	};

public:
	enum EMSTATSITEMTYPE
	{
		EMSTATSITEMTYPE_NULL			  = 0x0000,
		EMSTATSITEMTYPE_CONTROLLER_BUTTON = 0x0001,
	};

public:
	static const DWORD dwPRESSDELAY_START = 600;
	static const DWORD dwPRESSDELAY		  = 80;

public:
	CStatsItem(EngineDeviceMan* pEngineDevice);
	virtual	~CStatsItem ();

public:
	void			CreateSubControl( const CString& strLabel );

public:
	void			SetBasic  ( const INT32 nBasic )					{ m_nStatsBasic = nBasic;			 }
	void			SetAdd    ( const INT32 nAdd   )					{ m_nStatsAdd   = nAdd;				 }
	void			SetMinMax ( const INT32 nMin, const INT32 nMax )	{ m_nMax = nMax; m_nMin = nMin;      }

	void			SetEnableDownButton ( const BOOL bEnable )			{ m_bButton_Down_Disable = !bEnable; }
	void			SetEnableUpButton   ( const BOOL bEnable )			{ m_bButton_Up_Disable   = !bEnable; }

	void			SetType ( const DWORD dwType );

public:
	INT32			GetMax ()											{ return m_nMax;	  }
	INT32			GetAdd ()											{ return m_nStatsAdd; }

public:
	void			IncStatsAdd();
	void			DecStatsAdd();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBox*	CreateStaticControl (const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );

private:
	CBasicTextBox*	m_pLabel;
	CBasicTextBox*	m_pStats;
	
	CBasicButton*	m_pButton_Down;
	CBasicButton*	m_pButton_Up;

	CUIControl*		m_pButton_Down_Disable;
	CUIControl*		m_pButton_Up_Disable;

	INT32			m_nStatsBasic;
	INT32			m_nStatsAdd;

	INT32			m_nMin;
	INT32			m_nMax;

	BOOL			m_bButton_Down_Disable;
	BOOL			m_bButton_Up_Disable;

	BOOL			m_bButton_Down_Press;
	BOOL			m_bButton_Up_Press;

	DWORD			m_dwButton_Down_PressTime;
	DWORD			m_dwButton_Up_PressTime;

	DWORD			m_dwType;
};
