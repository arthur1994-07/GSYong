#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

class CBasicTextBox;

class CCreateCharacterClass : public CUIGroup
{
    enum
    {
        EXTREME,
        SWORDMAN,
        ARCHER,
        BRAWLER,
        SHAMAN,
        SCIENTIST,
        ASSASSIN,
		TRICKER,
        ETC,
    };

public:
    CCreateCharacterClass(EngineDeviceMan* pEngineDevice);
    virtual ~CCreateCharacterClass();

    void CreateSubControl ();

    CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
    CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

    void ResetData();
    void SetClassData( INT nSex, INT nClass );

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

private:

    CUIControl* m_pClass[GLCI_NUM_ETC];

    CBasicTextBox* m_pNameTextBox;
    CBasicTextBox* m_pInfoTextBox;
};