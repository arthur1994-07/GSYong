#include "StdAfx.h"
#include "BikeColorSlot.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBikeColorSlot::CBikeColorSlot(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pSelect( NULL )
    , m_pColor( NULL )
    , m_vColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) )
{
}

CBikeColorSlot::~CBikeColorSlot ()
{
}

void CBikeColorSlot::CreateSubControl ()
{
	m_pColor = CreateControl( "BIKE_COLOR_USE" );
	m_pSelect = CreateControl( "BIKE_COLOR_SELECT" );
	m_pColor->SetUseRender( TRUE );
	m_pColor->SetDiffuse( D3DCOLOR_ARGB( 255, 255, 255, 0 )  );
}

CUIControl*	CBikeColorSlot::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

void CBikeColorSlot::SetColor( D3DCOLOR vColor )
{
	m_vColor = vColor;
	m_pColor->SetDiffuse( m_vColor );
}

void CBikeColorSlot::SetSelect( BOOL bSelect )
{
	m_pSelect->SetVisibleSingle( bSelect );
}


void CBikeColorSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
