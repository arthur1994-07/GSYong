#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "./LottoConfirmBuyListBoxUIComp.h"
#include "./LottoConfirmBuyListPropertyUIComp.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLottoConfirmBuyListBoxUIComp::CLottoConfirmBuyListBoxUIComp( CInnerInterface* pInterface
															 , EngineDeviceMan* pEngineDevice )
															 : CUIListBox( pEngineDevice )
															 , m_pInterface( pInterface )
{
}

CLottoConfirmBuyListBoxUIComp::~CLottoConfirmBuyListBoxUIComp() { }

void CLottoConfirmBuyListBoxUIComp::AddItem( const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoConfirmBuyListPropertyUIComp* pProperty = CreateProperty();

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

CLottoConfirmBuyListPropertyUIComp* CLottoConfirmBuyListBoxUIComp::GetProperty( int nIndex )
{
	CLottoConfirmBuyListPropertyUIComp* pProperty =
		dynamic_cast<CLottoConfirmBuyListPropertyUIComp*>( GetControl( nIndex ) );

	return pProperty;
}

CLottoConfirmBuyListPropertyUIComp* CLottoConfirmBuyListBoxUIComp::CreateProperty()
{
	CLottoConfirmBuyListPropertyUIComp* pProperty =
		new CLottoConfirmBuyListPropertyUIComp(
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

void CLottoConfirmBuyListBoxUIComp::SetItem( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoConfirmBuyListPropertyUIComp* pProperty =
		dynamic_cast<CLottoConfirmBuyListPropertyUIComp*>(
		GetControl( nIndex ) );

	if( pProperty )
	{
		LottoSystem::LOTTO_SYSTEM_BUY buyInfo =
			va_arg( va, LottoSystem::LOTTO_SYSTEM_BUY );
		pProperty->SetBuyInfo( buyInfo, nIndex );
	}

	va_end(va);
}