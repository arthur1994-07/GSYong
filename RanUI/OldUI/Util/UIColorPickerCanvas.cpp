#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "UIColorPickerCanvas.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerCanvas::CUIColorPickerCanvas(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pImage_BaseColor(NULL)
    , m_pImage_Gradient(NULL)
    , m_pImage_Pointer(NULL)
    , m_pImage_Lock(NULL)
    , m_nPosX(0)
    , m_nPosY(0)
    , m_bButtonDown(FALSE)
    , m_dwBaseColor(0)
    , m_dwFinalColor(0)
{
}

CUIColorPickerCanvas::~CUIColorPickerCanvas ()
{
}

void CUIColorPickerCanvas::CreateSubControl ()
{
    // 색
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_CANVAS_COLOR", UI_FLAG_DEFAULT, COLORPICKERCANVAS_BASECOLOR );
        pControl->SetUseRender( TRUE );
        pControl->SetDiffuse( 0 );
        RegisterControl ( pControl );
        m_pImage_BaseColor = pControl;
    }

    // 그라디언트
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_CANVAS_GRADIENT", UI_FLAG_DEFAULT, COLORPICKERCANVAS_GRADIENT );
        RegisterControl ( pControl );
        m_pImage_Gradient = pControl;
    }

    // 포인터
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_CANVAS_POINTER", UI_FLAG_DEFAULT, COLORPICKERCANVAS_POINTER );
        RegisterControl ( pControl );
        m_pImage_Pointer = pControl;
    }

    // 잠금
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_CANVAS_LOCK", UI_FLAG_DEFAULT );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl ( pControl );
        m_pImage_Lock = pControl;
    }

    UpdatePointerPosition ();
    UpdateSelectColor ();
}

void CUIColorPickerCanvas::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_bButtonDown )
    {   
        const UIRECT& rcBaseColorGlobalPos = m_pImage_BaseColor->GetGlobalPos();

        FLOAT fPointerPosX = x - rcBaseColorGlobalPos.left;
        FLOAT fPointerPosY = y - rcBaseColorGlobalPos.top;

        if ( fPointerPosX < 0.0f )
        {
            fPointerPosX = 0.0f;
        }
        else if ( rcBaseColorGlobalPos.sizeX < fPointerPosX )
        {
            fPointerPosX = rcBaseColorGlobalPos.sizeX;
        }

        if ( fPointerPosY < 0.0f )
        {
            fPointerPosY = 0.0f;
        }
        else if ( rcBaseColorGlobalPos.sizeY < fPointerPosY )
        {
            fPointerPosY = rcBaseColorGlobalPos.sizeY;
        }

        if ( m_nPosX == fPointerPosX && m_nPosY == fPointerPosY )
        {
            return;
        }

        m_nPosX = (int)fPointerPosX;
        m_nPosY = (int)fPointerPosY;

        UpdatePointerPosition ();
        UpdateSelectColor ();

        AddMessageEx ( UIMSG_CANVAS_SELECT_UPDATE );
    }
}

void CUIColorPickerCanvas::UpdateByColor( const DWORD dwColor )
{
    if ( !m_pImage_BaseColor )
    {
        return;
    }

    const UIRECT& rcBaseColorGlobalPos = m_pImage_BaseColor->GetGlobalPos();

    // 새터레이션
    float fSat   = GetHSVSaturation(dwColor);
    float fLight = GetHSVLight(dwColor);

    m_nPosX = (int)(fSat * rcBaseColorGlobalPos.sizeX);
    m_nPosY = (int)((1.0f-fLight) * rcBaseColorGlobalPos.sizeY);

    UpdatePointerPosition ();
    UpdateSelectColor ();
}

void CUIColorPickerCanvas::UpdateSelectColor ()
{
    const UIRECT& rcBaseLocalPos    = m_pImage_BaseColor->GetLocalPos();
    const UIRECT& rcParentGlobalPos = GetGlobalPos();

    FLOAT fRatioX = 1.0f - m_nPosX/rcBaseLocalPos.sizeX;
    FLOAT fRatioY = 1.0f - m_nPosY/rcBaseLocalPos.sizeY;

    int nR = (int)((m_dwBaseColor&0xFF0000)>>16);
    int nG = (int)((m_dwBaseColor&0xFF00)>>8);
    int nB = (int)(m_dwBaseColor&0xFF);

    nR = nR + (int)( (255.0f - (float)nR) * fRatioX);
    nG = nG + (int)( (255.0f - (float)nG) * fRatioX);
    nB = nB + (int)( (255.0f - (float)nB) * fRatioX);

    nR = nR > 255 ? 255 : nR;
    nG = nG > 255 ? 255 : nG;
    nB = nB > 255 ? 255 : nB;

    nR = (int)((float)nR * fRatioY);
    nG = (int)((float)nG * fRatioY);
    nB = (int)((float)nB * fRatioY);

    nR = nR > 255 ? 255 : nR;
    nG = nG > 255 ? 255 : nG;
    nB = nB > 255 ? 255 : nB;

    m_dwFinalColor = 0xFF000000 | (nR<<16) | (nG<<8) | nB;
}

void CUIColorPickerCanvas::UpdatePointerPosition ()
{
    if ( !m_pImage_Pointer || !m_pImage_BaseColor )
    {
        return;
    }

    const UIRECT& rcPointerLocalPos = m_pImage_Pointer->GetLocalPos();
    const UIRECT& rcParentGlobalPos = GetGlobalPos();

    FLOAT fPointerPosX = (FLOAT)m_nPosX;
    FLOAT fPointerPosY = (FLOAT)m_nPosY;

    fPointerPosX -= rcPointerLocalPos.sizeX*0.5f;
    fPointerPosY -= rcPointerLocalPos.sizeY*0.5f;

    m_pImage_Pointer->SetLocalPos  ( D3DXVECTOR2(fPointerPosX, fPointerPosY) );
    m_pImage_Pointer->SetGlobalPos ( D3DXVECTOR2(rcParentGlobalPos.left + fPointerPosX, rcParentGlobalPos.top + fPointerPosY) );
}

void CUIColorPickerCanvas::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case COLORPICKERCANVAS_BASECOLOR:
        {            
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&(UIMSG_LB_DOWN) )
                {
                    m_bButtonDown = TRUE;
                }
            }

            if( !(dwMsg&(UIMSG_LB_DOWN|UIMSG_LB_PRESEED|UIMSG_LB_DRAG)) )
            {
                m_bButtonDown = FALSE;
            }
        }
        break;
    };

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CUIColorPickerCanvas::SetBaseColor ( const DWORD dwColor )
{
    if ( m_pImage_BaseColor )
    {
        m_pImage_BaseColor->SetDiffuse( dwColor );
    }

    m_dwBaseColor = dwColor;

    UpdateSelectColor ();
}

const FLOAT CUIColorPickerCanvas::GetHSVLight( const DWORD dwColor )
{
    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    float fR = (float)nR/255.0f;
    float fG = (float)nG/255.0f;
    float fB = (float)nB/255.0f;

    return max(max(fR, fG), fB);
}

const FLOAT CUIColorPickerCanvas::GetHSVSaturation( const DWORD dwColor )
{
    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    float fR = (float)nR/255.0f;
    float fG = (float)nG/255.0f;
    float fB = (float)nB/255.0f;

    float fmax = max(max(fR, fG), fB);
    float fmin = min(min(fR, fG), fB);

    if (fmax == fmin)
        return 0.0f;

    return 1.0f - (fmin/fmax);
}

void CUIColorPickerCanvas::SetRestrict ( const BOOL bRestrict )
{
    if ( bRestrict )
    {
        m_pImage_Lock->SetVisibleSingle( TRUE );

        m_pImage_BaseColor->SetDiffuse( dwRESTRICTCOLOR );
        m_pImage_Gradient->SetDiffuse( dwRESTRICTCOLOR );
        m_pImage_Pointer->SetDiffuse( dwRESTRICTCOLOR );

        SetNoMessage( TRUE );
        SetNoUpdate( TRUE );
    }
    else
    {
        m_pImage_Lock->SetVisibleSingle( FALSE );

        m_pImage_BaseColor->SetDiffuse( dwDEFAULTCOLOR );
        m_pImage_Gradient->SetDiffuse( dwDEFAULTCOLOR );
        m_pImage_Pointer->SetDiffuse( dwDEFAULTCOLOR );

        SetNoMessage( FALSE );
        SetNoUpdate( FALSE );
    }
}
