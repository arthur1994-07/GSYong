#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include <vector>

//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_SLIDE_SELECT_UPDATE = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CUIColorPickerSlide : public CUIGroup
{

public:
    enum
    {
        COLORPICKERSLIDE_SLIDE = NO_ID + 1,
        COLORPICKERSLIDE_ARROW,
    };

public:
    enum
    {
        COLOR_SECTION = 21,

        BASECOLORINDEX_RBG = 0,
        BASECOLORINDEX_BRG = 1,
        BASECOLORINDEX_BGR = 2,
        BASECOLORINDEX_GBR = 3,
        BASECOLORINDEX_GRB = 4,
        BASECOLORINDEX_RGB = 5,
    };

public:
    CUIColorPickerSlide(EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorPickerSlide();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void         UpdateByColor   ( const DWORD dwColor ); // 사용하지 않는다.
    void         UpdateByColor_2 ( const DWORD dwColor );
    const DWORD  GetColor () { return m_dwColor; }
    const DWORD  GetBestColor ( const DWORD dwColor );

private:
    void         InitColorArray ();
    void         UpdateSelectColor ();
    void         UpdateArrowPosition ();

private:
    const BOOL   CompareColorAmount ( const int nColor1, const int nColor2 );
    const int    GetPositionByIndex ( const int nIndex );

private:
    CUIControl*  m_pImage_Slide;
    CUIControl*  m_pImage_Arrow;

private:
    int          m_nPosY;
    int          m_nColorIndex;
    DWORD        m_dwColor;
    BOOL         m_bButtonDown;

private:
    std::vector<DWORD> m_dwColorArray;
    std::vector<int>   m_nBaseColorIndexArray;

};
