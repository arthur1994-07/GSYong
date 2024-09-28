#include "stdafx.h"

#include "./UIGroupHelperNonClose.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIGroupHelperNonClose::CUIGroupHelperNonClose( EngineDeviceMan* pEngineDevice
											   , CInnerInterface* pInterface )
											   : CUIGroupHelper( pEngineDevice )
											   , m_pInterface( pInterface )
											   , m_bMove( false )
											   , m_bSelectColor( false )
{

}

CUIGroupHelperNonClose::~CUIGroupHelperNonClose() { }

void CUIGroupHelperNonClose::CreateSub( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag, UIGUID WndID )
{
	CUIGroupHelper::CreateSub( pParent, szControlKeyword, wAlignFlag, WndID );

	// Non Close UI 등록;
	m_pInterface->AddNonCloseUI( GetWndID() );
}

void CUIGroupHelperNonClose::CreateLightGrayMove( const char* szControlKeyword )
{
	CreateLineBox( szControlKeyword, "NO_LINE_WHITE_BACKGROUND", EUIGroupHelperNonCloseBackground );

	m_bMove = true;
	m_bSelectColor = true;
}

void CUIGroupHelperNonClose::CreateNonBackground(bool bMove,bool bSelectColor)
{
	m_bMove = bMove;
	m_bSelectColor = bSelectColor;
}

void CUIGroupHelperNonClose::CreateLightGrayNonMove( const char* szControlKeyword )
{
	CreateLineBox( szControlKeyword, "NO_LINE_WHITE_BACKGROUND", EUIGroupHelperNonCloseBackground );

	m_bMove = false;
	m_bSelectColor = true;
}

void CUIGroupHelperNonClose::Update( int x
									, int y
									, BYTE LB
									, BYTE MB
									, BYTE RB
									, int nScroll
									, float fElapsedTime
									, BOOL bFirstControl )
{
	if( m_bMove )
		DragUpdate( x, y, m_vPos );

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIGroupHelperNonClose::TranslateUIMessage( UIGUID ControlID
												, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );

	if( m_bMove )
	{
		DragMessage( dwMsg );

		if( m_bSelectColor )
			DragMessageSelectColor( dwMsg );
	}
}

void CUIGroupHelperNonClose::DragMessageSelectColor( DWORD dwMsg )
{
	if( CHECK_MOUSE_IN( dwMsg ) )
	{
		if( dwMsg & UIMSG_LB_DOWN )
			SetDiffuse( D3DCOLOR_ARGB( 0xB4, 0xFF, 0xFF, 0xFF ) );
		else if( CHECK_LB_UP_LIKE( dwMsg ) )
			SetDiffuse( D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF ) );
	}
	//	컨트롤 영역 밖에서 이벤트 발생;
	else if( CHECK_LB_UP_LIKE( dwMsg ) )		
	{								
		SetDiffuse( D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF ) );
	}
}