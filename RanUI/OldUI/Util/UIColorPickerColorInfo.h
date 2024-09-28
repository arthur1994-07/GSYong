#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_COLORINFO_EDIT_UPDATE = UIMSG_USER1;
////////////////////////////////////////////////////////////////////.

class CUIEditBox;
class CBasicTextBox;

class CUIColorPickerColorInfo : public CUIGroup
{

public:
    enum
    {
        COLORPICKERCOLORINFO_NONE = NO_ID + 1,
        COLORPICKERCOLORINFO_EDITBOX_R,
        COLORPICKERCOLORINFO_EDITBOX_G,
        COLORPICKERCOLORINFO_EDITBOX_B,
    };

public:
    static const DWORD dwDEFAULTCOLOR  = 0xFFFFFFFF;
    static const DWORD dwRESTRICTCOLOR = 0xFF808080;

public:
    CUIColorPickerColorInfo(EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorPickerColorInfo();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    virtual void TranslateEditBoxMsg( CUIEditBox* pEditBox,	DWORD dwMsg );

public:
    void         SetRestrict ( const BOOL bRestrict );
    void         SetColor ( const DWORD dwColor );
    const DWORD  GetColor () { return m_dwColor; }

private:
    CUIControl*     m_pColor;
    CUIEditBox*     m_pEditBox_R;
    CUIEditBox*     m_pEditBox_G;
    CUIEditBox*     m_pEditBox_B;
    CBasicTextBox*  m_pText_R;
    CBasicTextBox*  m_pText_G;
    CBasicTextBox*  m_pText_B;

private:
    DWORD        m_dwColor;
    int          m_nR;
    int          m_nG;
    int          m_nB;

};
