#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "./LottoBasicBuyListBoxUIComp.h"
#include "./LottoBasicBuyListPropertyUIComp.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLottoBasicBuyListBoxUIComp::CLottoBasicBuyListBoxUIComp( CInnerInterface* pInterface
														 , EngineDeviceMan* pEngineDevice )
														 : CUIListBox( pEngineDevice )
														 , m_pInterface( pInterface )
{
}

CLottoBasicBuyListBoxUIComp::~CLottoBasicBuyListBoxUIComp() { }

void CLottoBasicBuyListBoxUIComp::AddItem( const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoBasicBuyListPropertyUIComp* pProperty = CreateProperty();

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

CLottoBasicBuyListPropertyUIComp* CLottoBasicBuyListBoxUIComp::CreateProperty()
{
	CLottoBasicBuyListPropertyUIComp* pProperty =
		new CLottoBasicBuyListPropertyUIComp(
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
		LottoSystem::LOTTO_SYSTEM_BUY buyInfo;
		pProperty->SetBuyInfo( buyInfo, m_nTotalLine );
	}

	return pProperty;
}

void CLottoBasicBuyListBoxUIComp::SetItem( int nIndex, const TCHAR* item, ... )
{
	va_list va;
	va_start( va, item );

	CLottoBasicBuyListPropertyUIComp* pProperty =
		dynamic_cast<CLottoBasicBuyListPropertyUIComp*>(
		GetControl( nIndex ) );

	if( pProperty )
	{
		LottoSystem::LOTTO_SYSTEM_BUY buyInfo =
			va_arg( va, LottoSystem::LOTTO_SYSTEM_BUY );
		pProperty->SetBuyInfo( buyInfo, nIndex );
	}

	va_end(va);
}