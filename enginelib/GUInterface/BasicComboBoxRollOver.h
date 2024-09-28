#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"
#include "../../Enginelib/GUInterface/UITextControl.h"
#include "UITextControl.h"

////////////////////////////////////////////////////////////////////
//	����� �޽��� ����
const DWORD UI_MSG_COMBOBOX_ROLLOVER_SCROLL = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicLineBox;

class	CBasicComboBoxRollOver : public CUIGroup
{
protected:
static	const int nSTARTLINE;
static	const int nLIMITLINE;
static	const D3DCOLOR	dwMOUSEOVERCOLOR;
static	const int nNOT_SELECTED;

protected:
	enum
	{
		COMBOBOX_ROLLOVER_TEXT_BOX = 1,
		BASIC_COMBOBOX_ROLLOVER_SCROLLBAR
	};

public:
	CBasicComboBoxRollOver (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicComboBoxRollOver ();

public:
	virtual void CreateBaseComboBoxRollOver( char* szComboBoxControl, bool bSelectInit = TRUE );
    virtual void CreateWhiteComboBoxRollOver( char* szComboBoxControl, bool bSelectInit = TRUE );

protected:
	virtual void CreateComboBoxRollOverImage( bool bSelectInit );
    virtual void CreateWhiteComboBoxRollOverImage( bool bSelectInit );
	virtual void CreateScrollBar ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetScrollPercent ( float fPercent );

	int AddText(const CString& str, bool bVisibleScroll = FALSE, bool bOneLine = FALSE, const D3DCOLOR& TextColor = NS_UITEXTCOLOR::WHITE );
	void ClearText();

	void SetSelectIndex ( int nIndex );
	int GetSelectIndex ()					{ return m_nSelectIndex; }

	// ���ϰ� ���� �״�� �������. ������ �ް� �װ� ����ϸ� ������ �� �ִ�.
	CString	GetSelectText ( int nIndex );

	void SetTextData( int nIndex, DWORD dwData );
	DWORD GetTextData( int nIndex );

	void SetVisibleThumb( bool bVisible );

	void SetTextColor ( int nIndex, const D3DCOLOR cColor );

    int GetCount();

	const bool IsValidIndex( DWORD dwIndex ) const;

public:
	inline const bool IsEmpty() const { return m_dwCurMaxItem ? false : true; }

protected:
	int	m_nSelectIndex;

	CBasicLineBox* m_pLineBox;
	CBasicTextBoxEx* m_pTextBox;
	CBasicScrollBarEx* m_pScrollBar;

	DWORD m_dwCurMaxItem;
};
