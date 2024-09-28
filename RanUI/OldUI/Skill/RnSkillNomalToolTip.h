#pragma once
#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;

namespace
{
    static const float TOOTIP_VISIBLE_TIME = 0.5f;
}

class RnSkillNomalToolTip : public CUIGroup
{
private:
    enum
    {
        ID_TOOLTIP_BACKGROUND = NO_ID + 1,
        ID_TOOLTIP_TEXT,
    };
public:


    RnSkillNomalToolTip( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void CreateSubControl( const char* const szLineBoxInfo, const char* const szTextBoxInfo );
    virtual	void        SetVisibleSingle ( BOOL bVisible );
    int	                AddString( int nIndex, const CString& strText, const D3DCOLOR& TextColor );
    void                SetText( const CString& strText, const D3DCOLOR& TextColor );
    int	                AddText( const CString& strText, const D3DCOLOR& TextColor );
    void                ClearText();
protected:
    virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    CBasicLineBoxSmart* m_pToolTipBackground;
    CBasicTextBox*      m_pToolTipTextBox;
    float               m_fVisibleTime;

};
