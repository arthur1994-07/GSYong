#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"
#include "InstanceRow.h"
#include "InstanceListBox.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
const int DWORD_MAX = 0xffffffffUL;

InstanceListBox::InstanceListBox( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIListBox( pEngineDevice )
, m_pInterface( pInterface )
{
}

InstanceListBox::~InstanceListBox() { }

void InstanceListBox::AddItem( const TCHAR* pText, ... )
{
	va_list va;
	va_start( va, pText );

	InstanceRow* pProperty = CreateProperty(pText);
	pProperty->SetInstanceId( va_arg(va, DWORD) );

	if( pProperty )
	{
		m_kControls.push_back( pProperty );
		++m_nTotalLine;

		UpdateVisibleLineToUnVisible();
		UpdateVisibleLine();
		UpdateVisibleLineToVisible();
		UpdateScrollThumb();
	}

	va_end( va );
}

InstanceRow* InstanceListBox::CreateProperty(const TCHAR* pText)
{
	InstanceRow* pProperty = new InstanceRow( m_pInterface, m_pEngineDevice );
	if( pProperty )
	{
		pProperty->CreateSub( this, m_strControl.c_str(), UI_FLAG_DEFAULT );
		pProperty->CreateSubControl();
		pProperty->SetText(pText);
		pProperty->SetVisibleSingle( FALSE );
		RegisterControl( pProperty );
		SetPosControl( pProperty );

		CreateDebugLineBox( pProperty );

		pProperty->InitDeviceObjects( m_pEngineDevice->GetDevice() );

	}

	return pProperty;
}

void InstanceListBox::SetItem( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	InstanceRow* pProperty =
		dynamic_cast<InstanceRow*>(GetControl( nIndex ) );
	pProperty->ClearText();
	pProperty->SetText(item);

	va_end(va);
}

void InstanceListBox::SetPropertyText ( DWORD dwInstanceID, const std::string& strText )
{
	for ( unsigned int i=0; i<m_kControls.size(); ++i )
	{
		InstanceRow* pInstanceRow =
			static_cast< InstanceRow* >( m_kControls.at( i ) );
		if ( pInstanceRow )
		{
			if ( pInstanceRow->GetInstanceId() == dwInstanceID )
				pInstanceRow->SetText( strText.c_str() );
		}
	}
}

DWORD InstanceListBox::GetInstanceID()
{
	int nselect = GetCurSel();
	InstanceRow* pInstanceRow = (InstanceRow*)GetControl(nselect);
	if (!pInstanceRow)
		return DWORD_MAX;
	else
		return pInstanceRow->GetInstanceId();
}
