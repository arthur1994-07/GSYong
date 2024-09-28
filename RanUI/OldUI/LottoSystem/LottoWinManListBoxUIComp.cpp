#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "./LottoWinManListBoxUIComp.h"
#include "./LottoWinManListPropertyUIComp.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLottoWinManListBoxUIComp::CLottoWinManListBoxUIComp( CInnerInterface* pInterface
													 , EngineDeviceMan* pEngineDevice )
													 : CUIListBox( pEngineDevice )
													 , m_pInterface( pInterface )
{
}

CLottoWinManListBoxUIComp::~CLottoWinManListBoxUIComp() { }

void CLottoWinManListBoxUIComp::AddItem( const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoWinManListPropertyUIComp* pProperty = CreateProperty();

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

CLottoWinManListPropertyUIComp* CLottoWinManListBoxUIComp::CreateProperty()
{
	CLottoWinManListPropertyUIComp* pProperty =
		new CLottoWinManListPropertyUIComp(
		m_pInterface, m_pEngineDevice );
	if( pProperty )
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

void CLottoWinManListBoxUIComp::SetItem( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoWinManListPropertyUIComp* pProperty =
		dynamic_cast<CLottoWinManListPropertyUIComp*>(
		GetControl( nIndex ) );

	if( pProperty )
	{
		LottoSystem::LOTTO_WINMAN_LIST_PROPERTY_MEMBER propertyInfo =
			va_arg( va, LottoSystem::LOTTO_WINMAN_LIST_PROPERTY_MEMBER );
		pProperty->SetPropertyInfo( propertyInfo );
	}

	va_end(va);
}