#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_PICKER_UPDATE = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CUIColorPickerSlide;
class CUIColorPickerCanvas;
class CUIColorPickerColorInfo;
class CUIColorPickerColorTable;

class CUIColorPickerGroup : public CUIGroup
{

public:
    enum
    {
        COLORPICKERGROUP_CANVAS = NO_ID + 1,
        COLORPICKERGROUP_SLIDE,
        COLORPICKERGROUP_COLORINFO,
        COLORPICKERGROUP_COLORTABLE,
    };

public:
    static const DWORD dwDEFAULTCOLOR  = 0xFFFFFFFF;
    static const DWORD dwRESTRICTCOLOR = 0xFF808080;

public:
    CUIColorPickerGroup(EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorPickerGroup();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void         SetColor( const DWORD dwColor );
    const DWORD  GetColor();

public:
    void         SetRestrict( const BOOL bRestrict );

private:
    CUIColorPickerCanvas*       m_pCanvas;
    CUIColorPickerSlide*        m_pSlide;
    CUIColorPickerColorInfo*    m_pColorInfo;
    CUIColorPickerColorTable*   m_pColorTable;

private:
    BOOL                        m_bRestrict;

};
