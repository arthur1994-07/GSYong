#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../OuterInterface.h"

#include "./SelectCountryListComp.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCountryListComp::CSelectCountryListComp ( COuterInterface* pInterface
											   , EngineDeviceMan* pEngineDevice )
											   : CUIListBox ( pEngineDevice )
											   , m_pInterface ( pInterface )
{
}

CSelectCountryListComp::~CSelectCountryListComp () { }

void CSelectCountryListComp::AddItem ( const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CSelectCountryListProperty* pProperty = CreateProperty();
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

CSelectCountryListProperty* CSelectCountryListComp::GetProperty ( int nIndex )
{
	CSelectCountryListProperty* pProperty =
		dynamic_cast< CSelectCountryListProperty* >( GetControl( nIndex ) );

	return pProperty;
}

CSelectCountryListProperty* CSelectCountryListComp::CreateProperty ()
{
	CSelectCountryListProperty* pProperty =
		new CSelectCountryListProperty(
		m_pInterface, m_pEngineDevice );
	if ( pProperty )
	{
		pProperty->CreateSub( this, m_strControl.c_str(), UI_FLAG_DEFAULT );
		pProperty->CreateSubControl();
		pProperty->SetVisibleSingle( FALSE );
		RegisterControl( pProperty );
		SetPosControl( pProperty );

		CreateDebugLineBox( pProperty );

		pProperty->InitDeviceObjects( m_pEngineDevice->GetDevice() );
	}

	return pProperty;
}

void CSelectCountryListComp::SetItem ( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CSelectCountryListProperty* pProperty =
		dynamic_cast<CSelectCountryListProperty*>(
		GetControl( nIndex ) );

	if ( pProperty )
	{
		CSelectCountryListProperty::SPROPERTY_INFO sPropertyInfo =
			va_arg( va, CSelectCountryListProperty::SPROPERTY_INFO );
		pProperty->SetPropertyInfo( sPropertyInfo );
	}

	va_end(va);
}