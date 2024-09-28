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

#include "SelectCountryListComp.h"

#include "SelectCountryListUI.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCountryListUI::CSelectCountryListUI ( GLGaeaClient* pGaeaClient
										   , COuterInterface* pInterface
										   , EngineDeviceMan* pEngineDevice )
										   : CUIOuterWindow ( pInterface
										   , pEngineDevice )
										   , m_pGaeaClient ( pGaeaClient )
{
}

CSelectCountryListUI::~CSelectCountryListUI ()
{
}

void CSelectCountryListUI::CreateSubControl ()
{
	// Line Box;
	{
		CBasicLineBoxSmart* pLineBox;
		pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
		pLineBox->CreateSub(
			this,
			"SELECT_COUNTRY_LIST_REGION",
			UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "BLACK_LINE_DARKGRAY_BACKGROUND" );
		RegisterControl( pLineBox );
	}

	// List Box;
	{
		m_pCountryList = new CSelectCountryListComp(
			m_pInterface, m_pEngineDevice );
		m_pCountryList->CreateSub(
			this, "SELECT_COUNTRY_LIST_REGION", UI_FLAG_DEFAULT );
		m_pCountryList->Create(
			"SELECT_COUNTRY_LIST_PROPERTY_REGION",
			"YELLOW_LINE_GRAY_BACKGROUND",
			"SELECT_COUNTRY_LIST_SCROOL", 1, 1, 1 );

		RegisterControl( m_pCountryList );
	}

	const int nBUTTONSIZE = CBasicTextButton::SIZE22;

	{
		m_pButtonOK = new CBasicTextButton( m_pEngineDevice );
		m_pButtonOK->CreateSub(
			this,
			"BASIC_TEXT_BUTTON22",
			UI_FLAG_XSIZE,
			SELECT_COUNTRY_LIST_OK_BUTTON );
		m_pButtonOK->CreateBaseButton(
			"SELECT_COUNTRY_LIST_OK_BUTTON",
			nBUTTONSIZE,
			CBasicButton::CLICK_FLIP,
			(char*)ID2GAMEWORD( "SELECT_COUNTRY_LIST_COMP", 0 ) );		

		RegisterControl( m_pButtonOK );
	}

	{
		m_pButtonCancel = new CBasicTextButton( m_pEngineDevice );
		m_pButtonCancel->CreateSub(
			this,
			"BASIC_TEXT_BUTTON22",
			UI_FLAG_XSIZE,
			SELECT_COUNTRY_LIST_CANCEL_BUTTON );
		m_pButtonCancel->CreateBaseButton(
			"SELECT_COUNTRY_LIST_CANCEL_BUTTON",
			nBUTTONSIZE,
			CBasicButton::CLICK_FLIP,
			(char*)ID2GAMEWORD( "SELECT_COUNTRY_LIST_COMP", 1 ) );		

		RegisterControl( m_pButtonCancel );
	}
}

void CSelectCountryListUI::Update ( int x
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

	if ( Country::UNKNOWN_COUNTRY == GetSelectedCountryID() )
	{
		if ( m_pButtonOK->IsEnable() )
			m_pButtonOK->SetEnable( false );
	}
	else
	{
		if ( false == m_pButtonOK->IsEnable() )
			m_pButtonOK->SetEnable( true );
	}
}

void CSelectCountryListUI::TranslateUIMessage ( UIGUID ControlID
												 , DWORD dwMsg )
{
	CUIOuterWindow::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_COUNTRY_LIST_OK_BUTTON:
		{
			if ( false == m_pButtonOK->IsEnable() )
				break;

			if ( CHECK_KEYFOCUSED( dwMsg ) ||
				CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->DoModal(
					sc::string::format( ID2GAMEEXTEXT("SELECT_COUNTRY_CONFIRM"), GetSelectedCountryName() ),
					UI::MODAL_INFOMATION,
					UI::OKCANCEL,
					OUTER_MODAL_SELECTED_COUNTRY );
			}
		}
		break;

	case SELECT_COUNTRY_LIST_CANCEL_BUTTON:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) ||
				CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( SELECT_CONTINENT_LIST_UI );
				m_pInterface->UiHideGroup( SELECT_COUNTRY_LIST_UI );
			}
		}
		break;
	}
}

void CSelectCountryListUI::SetVisibleSingle( BOOL bVisible )
{
	CUIOuterWindow::SetVisibleSingle( bVisible );
}

void CSelectCountryListUI::UpdateList ( const Country::CONTINENT_ID dwCurContinentID )
{
	Country::GLCountryManClient* pCountryMan =
		Country::GLCountryManClient::Instance();

	if ( m_pCountryList )
	{
		m_pCountryList->ClearItem();

		if ( pCountryMan )
		{
			Country::GLContinent* pContinent =
				pCountryMan->GetContinent( dwCurContinentID );
			if ( NULL == pContinent )
				return;
			
			for ( DWORD i=0; i<pContinent->GetCountryCount(); ++i )
			{
				Country::GLCountry* pCountry = pContinent->GetCountryUsingIndex( i );
				if ( pCountry )
				{
					if( m_pCountryList->GetTotalLine() < static_cast<int>( i+1 ) )
						m_pCountryList->AddItem( "" );

					CSelectCountryListProperty::SPROPERTY_INFO sPropertyInfo;
					sPropertyInfo.dwCountryID = pCountry->GetCountry();
					sPropertyInfo.strCountryName = pCountry->GetCountryName();

					m_pCountryList->SetItem( i, "", sPropertyInfo );
				}
			}
		}
	}

	m_pCountryList->SetCurSel( 0 );
}

const Country::COUNTRY_ID CSelectCountryListUI::GetSelectedCountryID () const
{

	if ( m_pCountryList )
	{
		CSelectCountryListProperty* pProperty =
			m_pCountryList->GetProperty( m_pCountryList->GetCurSel() );

		if ( pProperty )
		{
			CSelectCountryListProperty::SPROPERTY_INFO sPropertyInfo = 
				pProperty->GetPropertyInfo();

			return sPropertyInfo.dwCountryID;
		}
	}

	return Country::UNKNOWN_COUNTRY;
}

const std::string& CSelectCountryListUI::GetSelectedCountryName () const
{
	if ( m_pCountryList )
	{
		CSelectCountryListProperty* pProperty =
			m_pCountryList->GetProperty( m_pCountryList->GetCurSel() );

		if ( pProperty )
		{
			CSelectCountryListProperty::SPROPERTY_INFO sPropertyInfo = 
				pProperty->GetPropertyInfo();

			return sPropertyInfo.strCountryName;
		}
	}

	return std::string();
}