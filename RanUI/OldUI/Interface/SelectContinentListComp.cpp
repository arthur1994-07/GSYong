#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../OuterInterface.h"

#include "./SelectContinentListComp.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectContinentListComp::CSelectContinentListComp ( COuterInterface* pInterface
												   , EngineDeviceMan* pEngineDevice )
												   : CUIListBox ( pEngineDevice )
												   , m_pInterface ( pInterface )
{
}

CSelectContinentListComp::~CSelectContinentListComp () { }

void CSelectContinentListComp::AddItem ( const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CSelectContinentListProperty* pProperty = CreateProperty();
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

CSelectContinentListProperty* CSelectContinentListComp::GetProperty ( int nIndex )
{
	CSelectContinentListProperty* pProperty =
		dynamic_cast< CSelectContinentListProperty* >( GetControl( nIndex ) );

	return pProperty;
}

CSelectContinentListProperty* CSelectContinentListComp::CreateProperty ()
{
	CSelectContinentListProperty* pProperty =
		new CSelectContinentListProperty(
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

void CSelectContinentListComp::SetItem ( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CSelectContinentListProperty* pProperty =
		dynamic_cast<CSelectContinentListProperty*>(
		GetControl( nIndex ) );

	if ( pProperty )
	{
		CSelectContinentListProperty::SPROPERTY_INFO sPropertyInfo =
			va_arg( va, CSelectContinentListProperty::SPROPERTY_INFO );
		pProperty->SetPropertyInfo( sPropertyInfo );
	}

	va_end(va);
}