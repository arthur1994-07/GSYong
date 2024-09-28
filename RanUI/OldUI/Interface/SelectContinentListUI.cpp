#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"

#include "SelectCharacterPage.h"
#include "SelectCharacterLeftPage.h"
#include "SelectCountryUI.h"

#include "SelectCountryListUI.h"

#include "SelectContinentListComp.h"

#include "SelectContinentListUI.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectContinentListUI::CSelectContinentListUI ( GLGaeaClient* pGaeaClient
											   , COuterInterface* pInterface
											   , EngineDeviceMan* pEngineDevice )
											   : CUIOuterWindow ( pInterface
											   , pEngineDevice )
											   , m_pGaeaClient ( pGaeaClient )
											   , m_pContinentList ( NULL )
{
}

CSelectContinentListUI::~CSelectContinentListUI ()
{
}

void CSelectContinentListUI::CreateSubControl ()
{
	// Line Box;
	{
		CBasicLineBoxSmart* pLineBox;
		pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
		pLineBox->CreateSub(
			this,
			"SELECT_CONTINENT_LIST_REGION",
			UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "BLACK_LINE_DARKGRAY_BACKGROUND" );
		RegisterControl( pLineBox );
	}
	
	// List Box;
	{
		m_pContinentList = new CSelectContinentListComp(
			m_pInterface, m_pEngineDevice );
		m_pContinentList->CreateSub(
			this, "SELECT_CONTINENT_LIST_REGION", UI_FLAG_DEFAULT );
		m_pContinentList->Create(
			"SELECT_CONTINENT_LIST_PROPERTY_REGION",
			"YELLOW_LINE_GRAY_BACKGROUND",
			0/* Scroll */, 1, 1, 1 );

		RegisterControl( m_pContinentList );
	}
	//m_pContinentList->SetCurSel( 0 );
}

void CSelectContinentListUI::Update ( int x
							   , int y
							   , BYTE LB
							   , BYTE MB
							   , BYTE RB
							   , int nScroll
							   , float fElapsedTime
							   , BOOL bFirstControl )
{
	CUIOuterWindow::Update(
		x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectContinentListUI::TranslateUIMessage ( UIGUID ControlID
										   , DWORD dwMsg )
{
	CUIOuterWindow::TranslateUIMessage( ControlID, dwMsg );

}

void CSelectContinentListUI::SetVisibleSingle( BOOL bVisible )
{
	CSelectCharacterPage* pSelectCharacterPage =
		m_pInterface->GetSelectCharacterPage();
	if ( pSelectCharacterPage )
		pSelectCharacterPage->SetLock( !bVisible );

	CSelectCharacterLeftPage* pSelectCharacterLeftPage =
		m_pInterface->GetSelectCharacterLeftPage();
	if ( pSelectCharacterLeftPage )
		pSelectCharacterLeftPage->SetLock( !bVisible );

	CSelectCountryUI* pSelectCountryUI =
		m_pInterface->GetSelectCountryUI();
	if ( pSelectCountryUI )
		pSelectCountryUI->SetLock( !bVisible );

	m_pInterface->SetCharacterSelectLock( bVisible );

	CUIOuterWindow::SetVisibleSingle( bVisible );
}

void CSelectContinentListUI::UpdateList ()
{
	Country::GLCountryManClient* pCountryMan =
		Country::GLCountryManClient::Instance();

	if ( m_pContinentList )
	{
		m_pContinentList->ClearItem();

		if ( pCountryMan )
		{
			for ( DWORD i=0; i<pCountryMan->GetContinentCount(); ++i )
			{
				Country::GLContinent* pContinent = pCountryMan->GetContinentUsingIndex( i );
				if ( pContinent )
				{
					if( m_pContinentList->GetTotalLine() < static_cast<int>( i+1 ) )
						m_pContinentList->AddItem( "" );

					CSelectContinentListProperty::SPROPERTY_INFO sPropertyInfo;
					sPropertyInfo.dwContinentID = pContinent->GetContinent();
					sPropertyInfo.strContinentName = pContinent->GetContinentName();
						
					m_pContinentList->SetItem( i, "", sPropertyInfo );
				}
			}
		}
	}

	// Default ¼³Á¤;
	m_pContinentList->SetCurSel( 0 );

	CSelectCountryListUI* pCountryListUI =
		m_pInterface->GetSelectCountryListUI ();
	if ( pCountryListUI )
	{
		pCountryListUI->UpdateList( GetSelectedContinentID() );
	}
}

const Country::CONTINENT_ID CSelectContinentListUI::GetSelectedContinentID () const
{

	if ( m_pContinentList )
	{
		CSelectContinentListProperty* pProperty =
			m_pContinentList->GetProperty( m_pContinentList->GetCurSel() );

		if ( pProperty )
		{
			CSelectContinentListProperty::SPROPERTY_INFO sPropertyInfo = 
				pProperty->GetPropertyInfo();

			return sPropertyInfo.dwContinentID;
		}
	}

	return static_cast< Country::CONTINENT_ID >( 0 );
}