#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;

class CPointShopTooltip: public CUIGroup
{
private:
    enum
    {
        TOOLTIP_BACK = NO_ID + 1,
    };

public:
    CPointShopTooltip( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CPointShopTooltip();

protected:
    CInnerInterface* m_pInterface;

private:
    CBasicLineBoxSmart* m_pBack;

    CBasicTextBox* m_pUpTextBoxTitle;
    CBasicLineBoxSmart* m_pUpTextBoxLine;
    CBasicTextBox* m_pUpTextBoxText;

    CBasicTextBox* m_pDownTextBoxTitle;
    CBasicLineBoxSmart* m_pDownTextBoxLine;
    CBasicTextBox* m_pDownTextBoxText;

public:
    void CreateSubControl ();

public:
    void SetTextUpBoxTitle( std::string& strText, const D3DCOLOR& textColor );
    void SetTextUpBoxText( std::string& strText, const D3DCOLOR& textColor );

    void SetTextDownBoxTitle( std::string& strText, const D3DCOLOR& textColor );
    void SetTextDownBoxText( std::string& strText, const D3DCOLOR& textColor );

public:
    virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};