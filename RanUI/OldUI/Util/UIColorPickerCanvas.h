#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_CANVAS_SELECT_UPDATE = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CUIColorPickerCanvas : public CUIGroup
{

public:
    enum
    {
        COLORPICKERCANVAS_BASECOLOR = NO_ID + 1,
        COLORPICKERCANVAS_GRADIENT,
        COLORPICKERCANVAS_POINTER,
    };

public:
    static const DWORD dwDEFAULTCOLOR  = 0xFFFFFFFF;
    static const DWORD dwRESTRICTCOLOR = 0xFF808080;

public:
    CUIColorPickerCanvas(EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorPickerCanvas();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void         SetBaseColor  ( const DWORD dwColor );
    const DWORD  GetFinalColor () { return m_dwFinalColor; }
    const FLOAT  GetHSVLight( const DWORD dwColor );
    const FLOAT  GetHSVSaturation( const DWORD dwColor);

public:
    void         UpdateByColor( const DWORD dwColor );
    
public:
    void         SetRestrict ( const BOOL bRestrict );

private:
    void         UpdateSelectColor ();
    void         UpdatePointerPosition ();

private:
    CUIControl*  m_pImage_BaseColor;
    CUIControl*  m_pImage_Gradient;
    CUIControl*  m_pImage_Pointer;
    CUIControl*  m_pImage_Lock;

private:
    int          m_nPosX;
    int          m_nPosY;
    BOOL         m_bButtonDown;
private:
    DWORD        m_dwBaseColor;
    DWORD        m_dwFinalColor;

};
