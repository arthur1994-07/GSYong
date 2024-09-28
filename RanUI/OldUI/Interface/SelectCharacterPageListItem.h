#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/UIOuterWindow.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class COuterInterface;
class CUIControl;
class CBasicTextBox;
struct SCHARINFO_LOBBY;
enum EMCHARCLASS;

class CSelectCharacterPageListItem : public CUIOuterWindow
{
    enum
    {
        SELECT_CHAR_FOCUS = NO_ID + 1,
    };

public:
    CSelectCharacterPageListItem ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CSelectCharacterPageListItem ();

public:
    void CreateSubControl ();

    CUIControl*	CreateControl( const char* szControl );
    CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

    void ResetData();

    void SetListItemInfo( SCHARINFO_LOBBY* CharInfo, BOOL bSelect );


public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
    CUIControl* m_pClassOn[GLCI_NUM_ETC];
    CUIControl* m_pClassOff[GLCI_NUM_ETC];

    CBasicTextBox* m_pLevelTextBox;
    CBasicTextBox* m_pClassTextBox;
    CBasicTextBox* m_pNameTextBox;

    CUIControl*	m_pItemSelect;
};