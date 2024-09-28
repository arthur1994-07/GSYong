#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;
class RnButton;

class CRebirthWindowButton : public CUIGroup
{
private:
    enum
    {
        BUTTON_BACK = NO_ID + 1,
    };

public:
    CRebirthWindowButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CRebirthWindowButton();

protected:
    CInnerInterface* m_pInterface;

private:
    RnButton* m_pButton;
    CBasicTextBox* m_pButtonText;

public:
    void CreateSubControl ();

    void SetText( std::string& strText );
    bool IsFlip();

public:
    virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};