#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/UIOuterWindow.h"

class COuterInterface;
class CUIControl;
class CBasicLineBoxSmart;

class CLockSelectCharacterPageListItem : public CUIOuterWindow
{
    enum
    {
        LOCK_LINE_BOX = NO_ID + 1,
    };

public:
    CLockSelectCharacterPageListItem( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CLockSelectCharacterPageListItem ();

public:
    void CreateSubControl ();


public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
    CBasicLineBoxSmart* m_pLineBox;
    CUIControl* m_pLockImage;
};