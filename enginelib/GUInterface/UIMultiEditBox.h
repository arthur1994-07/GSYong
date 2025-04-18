#pragma once

#include "UIGroup.h"

class CUIEditBox;
class CD3DFontPar;

class CUIMultiEditBox : public CUIGroup
{
	static const INT nDEFAULT_INDEX;

	CUIEditBox * m_pEditBox;
	INT m_nCurIndex;
	bool m_bNativeMode;

//public:
	//static const int nDEFAULT_LIMIT_LINE  = 256;
	//static const int nDEFAULT_LIMIT_TOTAL = INT_MAX;

public:
	void Init();
	void BeginEdit();	
	void EndEdit();
	void ClearEdit();

	void CreateEditBox(
        UIGUID WndID,
        char* szEditBox,
        char* szCarrat,
        BOOL bCarratUseRender,
        DWORD dwDiffuse,
        CD3DFontPar* pFont);

	void SetText( CString & strText );
	INT GetText( CString & strText );

	void GoLineUp();
	void GoLineDown();
	void SetLimitInput ( int nLimitInput );
	void SetLimitInput ( int nLine, int nLimitInput );

	INT GetEditLength();

private:
	void SetEditBox ( CUIEditBox* pEditBox ) 
    { 
        GASSERT( pEditBox );
        m_pEditBox = pEditBox;
    }
	BOOL FindIndexControl();

public:
	/*virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );*/
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	CUIMultiEditBox(EngineDeviceMan* pEngineDevice);
	virtual ~CUIMultiEditBox(void);

};
