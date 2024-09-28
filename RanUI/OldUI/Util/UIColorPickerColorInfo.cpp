#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"


#include "UIColorPickerColorInfo.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerColorInfo::CUIColorPickerColorInfo(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pColor(NULL)
    , m_dwColor(0)
    , m_pEditBox_R(0)
    , m_pEditBox_G(0)
    , m_pEditBox_B(0)
    , m_pText_R(0)
    , m_pText_G(0)
    , m_pText_B(0)
    , m_nR(0)
    , m_nG(0)
    , m_nB(0)
{
}

CUIColorPickerColorInfo::~CUIColorPickerColorInfo ()
{
}

void CUIColorPickerColorInfo::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 라인 박스
    {
        CBasicLineBoxSmart* pLineBox = NULL;

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_LINEBOX_R", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO" );
        RegisterControl ( pLineBox );

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_LINEBOX_G", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO" );
        RegisterControl ( pLineBox );

        pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_LINEBOX_B", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO" );
        RegisterControl ( pLineBox );
    }	

    // 텍스트박스
    {
        CBasicTextBox* pStaticText = NULL; 

        pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "COLORPICKER_COLORINFO_TEXT_R", UI_FLAG_DEFAULT );
        pStaticText->SetFont ( pFont );
        pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        pStaticText->SetText( "R" );
        RegisterControl ( pStaticText );
        m_pText_R = pStaticText;

        pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "COLORPICKER_COLORINFO_TEXT_G", UI_FLAG_DEFAULT );
        pStaticText->SetFont ( pFont );
        pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        pStaticText->SetText( "G" );
        RegisterControl ( pStaticText );
        m_pText_G = pStaticText;

        pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "COLORPICKER_COLORINFO_TEXT_B", UI_FLAG_DEFAULT );
        pStaticText->SetFont ( pFont );
        pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pStaticText->SetText( "B" );	
        RegisterControl ( pStaticText );
        m_pText_B = pStaticText;
    }

    //  색상 정보
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_COLORINFO_COLOR", UI_FLAG_DEFAULT );
        pControl->SetUseRender( TRUE );
        pControl->SetDiffuse( 0 );
        RegisterControl ( pControl );
        m_pColor = pControl;
    }

    // 에디트 박스
    {
        CUIEditBox* pEditBox = NULL;
        pEditBox = new CUIEditBox(m_pEngineDevice);
        pEditBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_R", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLORPICKERCOLORINFO_EDITBOX_R );
        pEditBox->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        pEditBox->SetFont ( pFont );
        pEditBox->SetLimitInput ( 3 );
        pEditBox->DoUSE_NUMBER( true );
        pEditBox->SetAlign( EDIT_RIGHT_ALIGN );
        pEditBox->SetTextColor( NS_UITEXTCOLOR::GOLD );
        RegisterControl ( pEditBox );
        m_pEditBox_R = pEditBox;

        pEditBox = new CUIEditBox(m_pEngineDevice);
        pEditBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_G", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLORPICKERCOLORINFO_EDITBOX_G );
        pEditBox->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        pEditBox->SetFont ( pFont );
        pEditBox->SetLimitInput ( 3 );
        pEditBox->DoUSE_NUMBER( true );
        pEditBox->SetAlign( EDIT_RIGHT_ALIGN );
        pEditBox->SetTextColor( NS_UITEXTCOLOR::GOLD );
        RegisterControl ( pEditBox );
        m_pEditBox_G = pEditBox;

        pEditBox = new CUIEditBox(m_pEngineDevice);
        pEditBox->CreateSub ( this, "COLORPICKER_COLORINFO_EDIT_B", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLORPICKERCOLORINFO_EDITBOX_B );
        pEditBox->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        pEditBox->SetFont ( pFont );
        pEditBox->SetLimitInput ( 3 );
        pEditBox->DoUSE_NUMBER( true );
        pEditBox->SetAlign( EDIT_RIGHT_ALIGN );
        pEditBox->SetTextColor( NS_UITEXTCOLOR::GOLD );
        RegisterControl ( pEditBox );
        m_pEditBox_B = pEditBox;
    }

    SetColor ( 0xFFFFFFFF );
}

void CUIColorPickerColorInfo::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    const int nColorMin = 0;
    const int nColorMax = 255; 
    
    BOOL bChange = FALSE;

    if ( m_pEditBox_R->IsBegin() )
    {
        CString strR = m_pEditBox_R->GetEditString ();
        int     nR   = atoi(strR.GetString());

        if ( 1 < strR.GetLength() && nR == 0 )
        {
            m_pEditBox_R->SetEditString ( "0" );
        }

        if ( nColorMax < nR )
        {
            m_pEditBox_R->SetEditString ( sc::string::format("%1%", nColorMax).c_str() );
            nR = nColorMax;
        }
        else if ( nR < nColorMin )
        {
            m_pEditBox_R->SetEditString ( sc::string::format("%1%", nColorMin).c_str() );
            nR = nColorMin;
        }

        if ( m_nR != nR )
        {
            m_nR = nR;
            bChange = TRUE;
        }
    }

    if ( m_pEditBox_G->IsBegin() )
    {
        CString strG = m_pEditBox_G->GetEditString ();
        int     nG   = atoi(strG.GetString());

        if ( 1 < strG.GetLength() && nG == 0 )
        {
            m_pEditBox_G->SetEditString ( "0" );
        }

        if ( nColorMax < nG )
        {
            m_pEditBox_G->SetEditString ( sc::string::format("%1%", nColorMax).c_str() );
            nG = nColorMax;
        }
        else if ( nG < nColorMin )
        {
            m_pEditBox_G->SetEditString ( sc::string::format("%1%", nColorMin).c_str() );
            nG = nColorMin;
        }

        if ( m_nG != nG )
        {
            m_nG = nG;
            bChange = TRUE;
        }
    }

    if ( m_pEditBox_B->IsBegin() )
    {
        CString strB = m_pEditBox_B->GetEditString ();
        int     nB   = atoi(strB.GetString());

        if ( 1 < strB.GetLength() && nB == 0 )
        {
            m_pEditBox_B->SetEditString ( "0" );
        }

        if ( nColorMax < nB )
        {
            m_pEditBox_B->SetEditString ( sc::string::format("%1%", nColorMax).c_str() );
            nB = nColorMax;
        }
        else if ( nB < nColorMin )
        {
            m_pEditBox_B->SetEditString ( sc::string::format("%1%", nColorMin).c_str() );
            nB = nColorMin;
        }

        if ( m_nB != nB )
        {
            m_nB = nB;
            bChange = TRUE;
        }
    }

    if ( bChange )
    {
        SetColor ( D3DCOLOR_ARGB (0xFF, m_nR, m_nG, m_nB) );
        AddMessageEx ( UIMSG_COLORINFO_EDIT_UPDATE );
    }
}

void CUIColorPickerColorInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case COLORPICKERCOLORINFO_EDITBOX_R	 : TranslateEditBoxMsg( m_pEditBox_R, dwMsg );	break;
    case COLORPICKERCOLORINFO_EDITBOX_G	 : TranslateEditBoxMsg( m_pEditBox_G, dwMsg );	break;
    case COLORPICKERCOLORINFO_EDITBOX_B	 : TranslateEditBoxMsg( m_pEditBox_B, dwMsg );	break;
    };

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CUIColorPickerColorInfo::TranslateEditBoxMsg( CUIEditBox* pEditBox, DWORD dwMsg )
{
    if( !pEditBox ) return;

    CString strEdit = m_pEditBox_R->GetEditString ();

    if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
    {
        pEditBox->EndEdit();
        pEditBox->BeginEdit();
    }
    else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
    {
        pEditBox->EndEdit();
    }
}

void CUIColorPickerColorInfo::SetColor ( const DWORD dwColor )
{
    if ( m_pColor )
    {
        m_pColor->SetDiffuse( dwColor );
    }

    m_nR = (int)((dwColor&0xFF0000)>>16);
    m_nG = (int)((dwColor&0xFF00)>>8);
    m_nB = (int)(dwColor&0xFF);

    std::string strR = sc::string::format("%1%", m_nR);
    std::string strG = sc::string::format("%1%", m_nG);
    std::string strB = sc::string::format("%1%", m_nB);

    m_pEditBox_R->SetEditString ( strR.c_str() );
    m_pEditBox_R->UpdateRenderText();
    m_pEditBox_R->UpdatePosition();

    m_pEditBox_G->SetEditString	( strG.c_str() );
    m_pEditBox_G->UpdateRenderText();
    m_pEditBox_G->UpdatePosition();

    m_pEditBox_B->SetEditString	( strB.c_str() );
    m_pEditBox_B->UpdateRenderText();
    m_pEditBox_B->UpdatePosition();

    m_dwColor = dwColor;
}

void CUIColorPickerColorInfo::SetRestrict ( const BOOL bRestrict )
{
    if ( bRestrict )
    {
        m_pText_R->SetTextColor(0, dwRESTRICTCOLOR );
        m_pText_G->SetTextColor(0, dwRESTRICTCOLOR );
        m_pText_B->SetTextColor(0, dwRESTRICTCOLOR );

        m_pEditBox_R->SetTextColor( dwRESTRICTCOLOR );
        m_pEditBox_G->SetTextColor( dwRESTRICTCOLOR );
        m_pEditBox_B->SetTextColor( dwRESTRICTCOLOR );

        SetNoUpdate( TRUE );
        SetNoMessage( TRUE );
    }
    else
    {
        m_pText_R->SetTextColor(0, dwDEFAULTCOLOR );
        m_pText_G->SetTextColor(0, dwDEFAULTCOLOR );
        m_pText_B->SetTextColor(0, dwDEFAULTCOLOR );

        m_pEditBox_R->SetTextColor( NS_UITEXTCOLOR::GOLD );
        m_pEditBox_G->SetTextColor( NS_UITEXTCOLOR::GOLD );
        m_pEditBox_B->SetTextColor( NS_UITEXTCOLOR::GOLD );

        SetNoUpdate( FALSE );
        SetNoMessage( FALSE );
    }
}
