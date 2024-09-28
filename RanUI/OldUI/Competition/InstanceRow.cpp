#include "stdafx.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/GUInterface/BasicTextBox.h"
#include "InstanceRow.h"

const int DWORD_MAX = 0xffffffffUL;

InstanceRow::InstanceRow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
:CUIGroup(pEngineDevice)
, m_pInstanceName(NULL)
, m_pWatingImage(NULL)
, m_dwInstanceId(DWORD_MAX)
{
}

InstanceRow::~InstanceRow()
{

}

void InstanceRow::CreateSubControl()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	m_pInstanceName = new CBasicTextBox(m_pEngineDevice);
	m_pInstanceName->CreateSub( this, "RNCOMPETITION_LINEBOX_LISTBOX_ROW", UI_FLAG_DEFAULT );
	m_pInstanceName->SetFont( pFont );
	m_pInstanceName->SetTextAlign( UI_FLAG_LEFT | UI_FLAG_CENTER_Y );
	//m_pInstanceName->SetUseTextColor( 0, TRUE );
	//m_pInstanceName->SetTextColor(0,NS_UITEXTCOLOR::ORANGE);
	RegisterControl( m_pInstanceName );

}

void InstanceRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void InstanceRow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void InstanceRow::SetText( const TCHAR* pText, DWORD dwColor )
{
	m_pInstanceName->SetText( pText, dwColor );
}

void InstanceRow::ClearText()
{
	m_pInstanceName->ClearText();
}

void InstanceRow::SetInstanceId( DWORD dwID )
{
	m_dwInstanceId = dwID;
}

DWORD InstanceRow::GetInstanceId()
{
	return m_dwInstanceId;
}

