#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class CBasicTextBox;
class CUIControl;

class CCreateCharacterSex : public CUIGroup
{

public:
	CCreateCharacterSex(EngineDeviceMan* pEngineDevice);
	virtual ~CCreateCharacterSex();

	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

	void ResetData();
	void SetSexualData( INT nSex );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

private:
	CUIControl* m_pMale;
	CUIControl* m_pFemale;

	CBasicTextBox* m_pNameTextBox;
};