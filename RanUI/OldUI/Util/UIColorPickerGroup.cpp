#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "./UIColorPickerGroup.h"
#include "./UIColorPickerSlide.h"
#include "./UIColorPickerCanvas.h"
#include "./UIColorPickerColorInfo.h"
#include "./UIColorPickerColorTable.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerGroup::CUIColorPickerGroup(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pCanvas(NULL)
    , m_pSlide(NULL)
    , m_pColorInfo(NULL)
    , m_pColorTable(NULL)
    , m_bRestrict(FALSE)
{
}

CUIColorPickerGroup::~CUIColorPickerGroup ()
{
}

void CUIColorPickerGroup::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 라인박스
    {
        CBasicLineBoxSmart* pLineBox = NULL;

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_GROUP_LINBOX_CANVAS", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_GROUP_LINBOX_TEXINFO" );
        RegisterControl ( pLineBox );

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_GROUP_LINBOX_SLIDE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_GROUP_LINBOX_TEXINFO" );
        RegisterControl ( pLineBox );	

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_GROUP_LINBOX_COLORINFO_COLOR", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_GROUP_LINBOX_TEXINFO" );
        RegisterControl ( pLineBox );

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_GROUP_LINBOX_COLORTABLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_GROUP_LINBOX_TEXINFO" );
        RegisterControl ( pLineBox );
    }

    // 캔버스
    {
        CUIColorPickerCanvas* pControl = new CUIColorPickerCanvas(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_GROUP_CANVAS", UI_FLAG_DEFAULT, COLORPICKERGROUP_CANVAS );
        pControl->CreateSubControl();
        RegisterControl ( pControl );
        m_pCanvas = pControl;
    }

    // 슬라이드
    {
        CUIColorPickerSlide* pControl = new CUIColorPickerSlide(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_GROUP_SLIDE", UI_FLAG_DEFAULT, COLORPICKERGROUP_SLIDE );
        pControl->CreateSubControl();
        RegisterControl ( pControl );
        m_pSlide = pControl;
    }

    // 색상정보
    {
        CUIColorPickerColorInfo* pControl = new CUIColorPickerColorInfo(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_GROUP_COLORINFO", UI_FLAG_DEFAULT, COLORPICKERGROUP_COLORINFO );
        pControl->CreateSubControl();
        RegisterControl ( pControl );
        m_pColorInfo = pControl;
    }

    // 테이블
    {
        CUIColorPickerColorTable* pControl = new CUIColorPickerColorTable(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_GROUP_COLORTABLE", UI_FLAG_DEFAULT, COLORPICKERGROUP_COLORTABLE );
        pControl->CreateSubControl();
        RegisterControl ( pControl );
        m_pColorTable = pControl;
    }

    m_pCanvas->SetBaseColor ( m_pSlide->GetColor() );
    m_pColorInfo->SetColor  ( m_pCanvas->GetFinalColor() );
}

void CUIColorPickerGroup::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIColorPickerGroup::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

    switch ( ControlID )
    {
    case COLORPICKERGROUP_CANVAS:
        {
            if ( UIMSG_CANVAS_SELECT_UPDATE & dwMsg )
            {
                m_pCanvas->SetBaseColor ( m_pSlide->GetColor() );
                m_pColorInfo->SetColor  ( m_pCanvas->GetFinalColor() );

                AddMessageEx(UIMSG_PICKER_UPDATE);
            }
        }
        break;

    case COLORPICKERGROUP_SLIDE:
        {
            if ( UIMSG_SLIDE_SELECT_UPDATE & dwMsg )
            {
                m_pCanvas->SetBaseColor ( m_pSlide->GetColor() );
                m_pColorInfo->SetColor  ( m_pCanvas->GetFinalColor() );

                AddMessageEx(UIMSG_PICKER_UPDATE);
            }
        }
        break;

    case COLORPICKERGROUP_COLORINFO:
        {
            if ( UIMSG_COLORINFO_EDIT_UPDATE & dwMsg )
            {
                DWORD dwColor = m_pColorInfo->GetColor();
                 
                m_pSlide->UpdateByColor_2( dwColor );

                m_pCanvas->SetBaseColor( m_pSlide->GetColor() );
                m_pCanvas->UpdateByColor( dwColor );

                AddMessageEx(UIMSG_PICKER_UPDATE);
            }
        }
        break;

    case COLORPICKERGROUP_COLORTABLE:
        {
            if ( UIMSG_COLORTABLE_SELECT & dwMsg )
            {
                DWORD dwColor = m_pColorTable->GetColor();

                m_pSlide->UpdateByColor_2( dwColor );

                m_pCanvas->SetBaseColor( m_pSlide->GetColor() );
                m_pCanvas->UpdateByColor( dwColor );

                m_pColorInfo->SetColor  ( dwColor );

                AddMessageEx(UIMSG_PICKER_UPDATE);
            }
        }
        break;
    }
}

void CUIColorPickerGroup::SetColor( const DWORD dwColor )
{
    m_pSlide->UpdateByColor_2( dwColor );

    m_pCanvas->SetBaseColor( m_pSlide->GetColor() );
    m_pCanvas->UpdateByColor( dwColor );

    m_pColorInfo->SetColor( dwColor );
}

const DWORD CUIColorPickerGroup::GetColor()
{
    return m_pColorInfo->GetColor();
}

void CUIColorPickerGroup::SetRestrict( const BOOL bRestrict )
{
    m_bRestrict = bRestrict;
    m_pCanvas->SetRestrict(bRestrict);
    m_pColorTable->SetRestrict( bRestrict );
    m_pColorInfo->SetRestrict(bRestrict);

    if ( m_bRestrict )
    {
        m_pSlide->SetDiffuse( dwRESTRICTCOLOR );
        m_pSlide->SetNoMessage( TRUE );
        m_pSlide->SetNoUpdate( TRUE );
    }
    else
    {
        m_pSlide->SetDiffuse( dwDEFAULTCOLOR );
        m_pSlide->SetNoMessage( FALSE );
        m_pSlide->SetNoUpdate( FALSE );
    }
}
