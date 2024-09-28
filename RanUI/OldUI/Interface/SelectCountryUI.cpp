#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/Country/GLCountryManClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"

#include "SelectCountryUI.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCountryUI::CSelectCountryUI ( GLGaeaClient* pGaeaClient
								   , COuterInterface* pInterface
								   , EngineDeviceMan* pEngineDevice )
								   : CUIGroupHelper ( pEngineDevice )
								   , m_pGaeaClient ( pGaeaClient )
								   , m_pInterface ( pInterface )
								   , m_pSelectButton ( NULL )
								   , m_pSelectImage ( NULL )
								   , m_bSelectedCountry( false )
{
}

CSelectCountryUI::~CSelectCountryUI ()
{
}

void CSelectCountryUI::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox( m_pEngineDevice );
		pBasicLineBox->CreateSub(
			this,
			"BASIC_LINE_BOX_OUTER_FAT",
			UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat( "SELECT_COUNTRY_UI_REGION" );

		RegisterControl( pBasicLineBox );		
	}

	{
		const int nBUTTONSIZE = CBasicTextButton::SIZE40;

		m_pSelectButton = new CBasicTextButton( m_pEngineDevice );
		m_pSelectButton->CreateSub(
			this,
			"BASIC_TEXT_BUTTON40",
			UI_FLAG_XSIZE,
			SELECT_COUNTRY_BUTTON );
		m_pSelectButton->CreateBaseButton(
			"SELECT_COUNTRY_UI_SELECT_BUTTON",
			nBUTTONSIZE,
			CBasicButton::CLICK_FLIP,
			(char*)ID2GAMEWORD( "SELECT_COUNTRY_UI_TEXT", 0 ) );		

		RegisterControl( m_pSelectButton );
	}

	{
		m_pSelectImage = new CSwapImage( m_pEngineDevice );
		m_pSelectImage->CreateSub(
			this,
			"SELECT_COUNTRY_UI_SELECT_IMAGE",
			UI_FLAG_DEFAULT );
		m_pSelectImage->SetImage( "COUNTRY_UCT" );

		RegisterControl( m_pSelectImage );
	}
}

void CSelectCountryUI::Update ( int x
							  , int y
							  , BYTE LB
							  , BYTE MB
							  , BYTE RB
							  , int nScroll
							  , float fElapsedTime
							  , BOOL bFirstControl )
{
	CUIGroupHelper::Update(
		x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	Country::GLCountry* pCountry = m_pGaeaClient->GetClientCountry();
	if ( false == pCountry )
	{
		if ( m_sCountryInfo.ISNONCOUNTRY() )
		{
			m_pSelectImage->SetImage( "COUNTRY_UCT" );

			m_bSelectedCountry = false;

			m_sCountryInfo.RESET();
		}

		return;
	}

	// 현재 세팅된 값과 같으면 변경하지 않는다;
	if ( m_sCountryInfo == pCountry->GetCountryInfo() )
		return;

	// 현재 세팅된 값과 다를 경우 바꾼다;
	m_sCountryInfo = pCountry->GetCountryInfo();

	// 현재 세팅된 값과 다를 경우 이미지와 선택 상태를 변경한다;
	if ( m_pSelectImage )
	{
		m_pSelectImage->SetImage( pCountry->GetCountryImageID().c_str() );

		m_bSelectedCountry = true;
	}
}

void CSelectCountryUI::TranslateUIMessage ( UIGUID ControlID
										  , DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN( dwMsg ) )
	{
		AddMessageEx( UIMSG_MOUSEIN_SELECT_COUNTRYUI );
	}

	switch ( ControlID )
	{
	case SELECT_COUNTRY_BUTTON:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) ||
				CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				// 이미 국가를 선택한 상태라면;
				if ( m_bSelectedCountry )
				{
					m_pInterface->DoModal(
						ID2GAMEEXTEXT("SELECT_COUNTRY_ALREADY"),
						UI::MODAL_INFOMATION,
						UI::OK );
				}
				// 국가가 선택되지 않은 상태라면;
				else
				{
					m_pInterface->DoModal(
						ID2GAMEEXTEXT("SELECT_COUNTRY_NOTIFY"),
						UI::MODAL_INFOMATION,
						UI::OKCANCEL,
						OUTER_MODAL_SHOW_COUNTRY_SELECT_LIST_UI );
				}
			}
		}
		break;
	}

}

void CSelectCountryUI::SetVisibleSingle( BOOL bVisible )
{
	// UI가 닫힐 경우 국가정보를 초기화한다;
	if ( false == bVisible )
		m_sCountryInfo.RESET();

	CUIGroupHelper::SetVisibleSingle( bVisible );
}