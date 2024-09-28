#include "stdafx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../OuterInterface.h"

#include "./SelectCountryListUI.h"

#include "./SelectContinentListProperty.h"

#include "../../../SigmaCore/DebugInclude.h"

CSelectContinentListProperty::CSelectContinentListProperty ( COuterInterface* pInterface
														   , EngineDeviceMan* pEngineDevice )
														   : CUIGroupHelper ( pEngineDevice )
														   , m_pInterface( pInterface )
														   , m_pContinentNameTextBox( NULL )
														   , m_state( ESTATE_INACTIVE )
{
}

CSelectContinentListProperty::~CSelectContinentListProperty() { }

void CSelectContinentListProperty::CreateSubControl ( )
{
	// Line Box;
	{
		CBasicLineBoxSmart* pLineBox;
		pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
		pLineBox->CreateSub(
			this,
			"SELECT_CONTINENT_LIST_PROPERTY_REGION",
			UI_FLAG_DEFAULT,
			EPROPERTY_CLIECK_EVENT );
		pLineBox->CreateSubControl( "NO_LINE_GRAY_BACKGROUND" );
		RegisterControl( pLineBox );
	}

	// Text Box;
	{
		m_pContinentNameTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pContinentNameTextBox->CreateSub(
			this, "SELECT_CONTINENT_LIST_PROPERTY_TEXTBOX", UI_FLAG_YSIZE );
		m_pContinentNameTextBox->SetFont(
			DxFontMan::GetInstance().LoadDxFont (
			_DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pContinentNameTextBox->SetTextAlign(
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		m_pContinentNameTextBox->AddText(
			"",
			NS_UITEXTCOLOR::WHITE );
		m_pContinentNameTextBox->SetVisibleSingle( FALSE );

		RegisterControl( m_pContinentNameTextBox );
	}
}

void CSelectContinentListProperty::Update (
	int x,
	int y,
	BYTE LB,
	BYTE MB,
	BYTE RB,
	int nScroll,
	float fElapsedTime,
	BOOL bFirstControl )
{
	CUIGroupHelper::Update (
		x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectContinentListProperty::TranslateUIMessage (
	UIGUID ControlID,
	DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case EPROPERTY_CLIECK_EVENT:
		if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			CSelectCountryListUI* pCountryListUI =
				m_pInterface->GetSelectCountryListUI();

			if ( pCountryListUI )
			{
				pCountryListUI->UpdateList ( m_sPropertInfo.dwContinentID );
			}
		}
		break;
	}
}

void CSelectContinentListProperty::SetPropertyState ( PROPERTY_STATE state )
{
	m_state = state;

	if ( m_pContinentNameTextBox )
		m_pContinentNameTextBox->SetVisibleSingle( FALSE );

	switch( m_state )
	{
	case ESTATE_ACTIVE:
		if ( m_pContinentNameTextBox )
			m_pContinentNameTextBox->SetVisibleSingle( TRUE );
		break;
	case ESTATE_INACTIVE:
		break;
	}
}

void CSelectContinentListProperty::SetPropertyInfo (
	const SPROPERTY_INFO& sPropertInfo )
{
	if ( m_pContinentNameTextBox )
	{
		m_pContinentNameTextBox->SetOneLineText(
			sc::string::format( "%1%", sPropertInfo.strContinentName ).c_str(),
			NS_UITEXTCOLOR::WHITE );

		m_sPropertInfo = sPropertInfo;

		SetPropertyState( ESTATE_ACTIVE );
	}
}