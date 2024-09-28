#include "stdafx.h"

#include "./SelectCountryListProperty.h"

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

#include "../../../SigmaCore/DebugInclude.h"

CSelectCountryListProperty::CSelectCountryListProperty ( COuterInterface* pInterface
													   , EngineDeviceMan* pEngineDevice )
													   : CUIGroupHelper ( pEngineDevice )
													   , m_pInterface( pInterface )
													   , m_pCountryNameTextBox( NULL )
													   , m_state( ESTATE_INACTIVE )
{
}

CSelectCountryListProperty::~CSelectCountryListProperty() { }

void CSelectCountryListProperty::CreateSubControl ( )
{
	// Line Box;
	{
		CBasicLineBoxSmart* pLineBox;
		pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
		pLineBox->CreateSub(
			this,
			"SELECT_COUNTRY_LIST_PROPERTY_REGION",
			UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "NO_LINE_GRAY_BACKGROUND" );
		RegisterControl( pLineBox );
	}

	// Text Box;
	{
		m_pCountryNameTextBox = new CBasicTextBox( m_pEngineDevice );
		m_pCountryNameTextBox->CreateSub(
			this, "SELECT_COUNTRY_LIST_PROPERTY_TEXTBOX", UI_FLAG_YSIZE );
		m_pCountryNameTextBox->SetFont(
			DxFontMan::GetInstance().LoadDxFont (
			_DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pCountryNameTextBox->SetTextAlign(
			TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		m_pCountryNameTextBox->AddText(
			"",
			NS_UITEXTCOLOR::WHITE );
		m_pCountryNameTextBox->SetVisibleSingle( FALSE );

		RegisterControl( m_pCountryNameTextBox );
	}
}

void CSelectCountryListProperty::Update (
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

void CSelectCountryListProperty::TranslateUIMessage (
	UIGUID ControlID,
	DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CSelectCountryListProperty::SetPropertyState ( PROPERTY_STATE state )
{
	m_state = state;

	if ( m_pCountryNameTextBox )
		m_pCountryNameTextBox->SetVisibleSingle( FALSE );

	switch( m_state )
	{
	case ESTATE_ACTIVE:
		if ( m_pCountryNameTextBox )
			m_pCountryNameTextBox->SetVisibleSingle( TRUE );
		break;
	case ESTATE_INACTIVE:
		break;
	}
}

void CSelectCountryListProperty::SetPropertyInfo (
	const SPROPERTY_INFO& sPropertInfo )
{
	if ( m_pCountryNameTextBox )
	{
		m_pCountryNameTextBox->SetOneLineText(
			sc::string::format( "%1%", sPropertInfo.strCountryName ).c_str(),
			NS_UITEXTCOLOR::WHITE );

		m_sPropertInfo = sPropertInfo;

		SetPropertyState( ESTATE_ACTIVE );
	}
}