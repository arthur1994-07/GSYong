#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class CBasicTextBox;

class CCreateCharacterSchool : public CUIGroup
{
	enum
	{
		SCHOOL_SM,
		SCHOOL_HA,
		SCHOOL_BH,
	};

public:
	CCreateCharacterSchool(EngineDeviceMan* pEngineDevice);
	virtual ~CCreateCharacterSchool();

	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

	void ResetData();
	void SetSchoolData( INT nSchool );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

private:
	CUIControl* m_pSM;
	CUIControl* m_pHA;
	CUIControl* m_pBH;

	CBasicTextBox* m_pNameTextBox;
	CBasicTextBox* m_pInfoTextBox;
};