#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../InnerInterface.h"

#include "./AttendanceDayList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceDayList::CAttendanceDayList(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_nProgress(0)
{
    memset(m_pButton, 0, sizeof(m_pButton));
}

CAttendanceDayList::~CAttendanceDayList ()
{
}

void CAttendanceDayList::CreateSubControl ()
{
    for ( UINT i=0; i<MAX_DAYS; ++i )
    {
        m_pButton[i] = CreateButton( ATTENDANCE_BUTTON+i, i );
    }
}

void CAttendanceDayList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendanceDayList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    if ( ATTENDANCE_BUTTON <= ControlID && ControlID < ATTENDANCE_BUTTON+MAX_DAYS )
    {
        UIGUID cButtonID = ControlID - ATTENDANCE_BUTTON;

        if ( CHECK_MOUSE_IN ( dwMsg ) && m_nProgress < (UINT)cButtonID )
        {
            m_pInterface->SHOW_COMMON_LINEINFO( 
                ID2GAMEWORD("ATTENDANCE_TOOLTIP_TEXT", 2), 
                NS_UITEXTCOLOR::WHITE );
        }
    }

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceDayList::SetProgress( const UINT nDays )
{
    if ( MAX_DAYS <= nDays )
    {
        return;
    }

    m_nProgress = nDays;

    for ( UINT i=0; i<m_nProgress; ++i )
    {
        m_pButton[i]->SetStyle( SDAYBUTTON::EMSTYLE_COMPLETE );
    }

    m_pButton[m_nProgress]->SetStyle( SDAYBUTTON::EMSTYLE_TODAY );

    for ( UINT i=m_nProgress+1; i<MAX_DAYS; ++i )
    {
        m_pButton[i]->SetStyle( SDAYBUTTON::EMSTYLE_DEFAULT );
    }
}

CAttendanceDayList::SDAYBUTTON* CAttendanceDayList::CreateButton ( const UIGUID cID, const UINT nDays )
{
    std::string strControl = sc::string::format("ATTENDANCE_PAGE_DAYLIST_BUTTON%1%", nDays );

    SDAYBUTTON* pButton = new SDAYBUTTON(m_pEngineDevice);
    pButton->CreateSub( this, strControl.c_str(), UI_FLAG_DEFAULT, cID );
    pButton->CreateSubControl( nDays );
    RegisterControl( pButton );

    return pButton;
}


//-----------------------------------------------------------------------------------------------//

void CAttendanceDayList::SDAYBUTTON::CreateSubControl( const UINT nDays )
{
    CD3DFontPar* pFont  = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, _DEFAULT_FONT_SHADOW_FLAG );
    std::string  strDay = sc::string::format( ID2GAMEWORD("ATTENDANCE_DAYLIST_TEXT", 0), nDays+1 ); 

    DAYS = nDays;

    LINEBOX[0] = new CBasicLineBoxSmart(m_pEngineDevice);
    LINEBOX[0]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_LINEBOX" );
    LINEBOX[0]->CreateSubControl( "ATTENDANCE_PAGE_DAYLIST_BUTTON_LINEBOX_TEXINFO_DEFAULT" );
    RegisterControl( LINEBOX[0] );

    LINEBOX[1] = new CBasicLineBoxSmart(m_pEngineDevice);
    LINEBOX[1]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_LINEBOX" );
    LINEBOX[1]->CreateSubControl( "ATTENDANCE_PAGE_DAYLIST_BUTTON_LINEBOX_TEXINFO_COMPLATE" );
    RegisterControl( LINEBOX[1] );

    TODAY[0] = new CUIControl(m_pEngineDevice);
    TODAY[0]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_IMAGE_TODAY_HEAD" );
    RegisterControl( TODAY[0] );

    TODAY[1] = new CUIControl(m_pEngineDevice);
    TODAY[1]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_IMAGE_TODAY_BODY" );
    RegisterControl( TODAY[1] );

    TODAY[2] = new CUIControl(m_pEngineDevice);
    TODAY[2]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_IMAGE_TODAY_TAIL" );
    RegisterControl( TODAY[2] );

    CHECK = new CUIControl(m_pEngineDevice);
    CHECK->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_CHECK" );
    RegisterControl( CHECK );

    TEXTBOX[0] = new CBasicTextBox(m_pEngineDevice);
    TEXTBOX[0]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_TEXTBOX_DEFAULT" );
    TEXTBOX[0]->SetFont( pFont );
    TEXTBOX[0]->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
    TEXTBOX[0]->SetText( strDay.c_str() );
    RegisterControl( TEXTBOX[0] );

    TEXTBOX[1] = new CBasicTextBox(m_pEngineDevice);
    TEXTBOX[1]->CreateSub( this, "ATTENDANCE_PAGE_DAYLIST_BUTTON_TEXTBOX_TODAY" );
    TEXTBOX[1]->SetFont( pFont );
    TEXTBOX[1]->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
    TEXTBOX[1]->SetText( strDay.c_str() );
    RegisterControl( TEXTBOX[1] );

    SetStyle ( EMSTYLE_DEFAULT );
}

void CAttendanceDayList::SDAYBUTTON::SetStyle( const EMSTYLE emStyle )
{
    STYLE = emStyle;

    // 모든 컨트롤을 끈다
    {
        LINEBOX[0]->SetVisibleSingle( FALSE );
        LINEBOX[1]->SetVisibleSingle( FALSE );

        TEXTBOX[0]->SetVisibleSingle( FALSE );
        TEXTBOX[1]->SetVisibleSingle( FALSE );

        TODAY[0]->SetVisibleSingle( FALSE );
        TODAY[1]->SetVisibleSingle( FALSE );
        TODAY[2]->SetVisibleSingle( FALSE );

        CHECK->SetVisibleSingle( FALSE );
    }

    switch ( emStyle )
    {
    case EMSTYLE_DEFAULT:
        {
            LINEBOX[0]->SetVisibleSingle( TRUE );

            TEXTBOX[0]->SetVisibleSingle( TRUE );
            TEXTBOX[0]->SetUseTextColor( 0, TRUE );
            TEXTBOX[0]->SetTextColor(0, NS_UITEXTCOLOR::ATTENDANCE_DAYLIST_DEFAULT );
        }
        break;

    case EMSTYLE_COMPLETE:
        {
            LINEBOX[1]->SetVisibleSingle( TRUE );
            CHECK->SetVisibleSingle( TRUE );

            TEXTBOX[0]->SetVisibleSingle( TRUE );
            TEXTBOX[0]->SetUseTextColor( 0, TRUE );
            TEXTBOX[0]->SetTextColor( 0, NS_UITEXTCOLOR::ATTENDANCE_DAYLIST_COMPLETE );
        }
        break;
    
    case EMSTYLE_TODAY:
        {
            TODAY[0]->SetVisibleSingle( TRUE );
            TODAY[1]->SetVisibleSingle( TRUE );
            TODAY[2]->SetVisibleSingle( TRUE );

            TEXTBOX[1]->SetVisibleSingle( TRUE );
            TEXTBOX[1]->SetUseTextColor( 0, TRUE );
            TEXTBOX[1]->SetTextColor( 0, NS_UITEXTCOLOR::ATTENDANCE_DAYLIST_TODAY );
        }
        break;
    };
}
