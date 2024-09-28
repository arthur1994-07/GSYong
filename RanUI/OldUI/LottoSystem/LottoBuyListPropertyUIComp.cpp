#include "stdafx.h"

#include "./LottoBuyListPropertyUIComp.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

CLottoBuyListPropertyUIComp::CLottoBuyListPropertyUIComp(  CInnerInterface* pInterface
														 , EngineDeviceMan* pEngineDevice )
														 : CUIGroupHelper( pEngineDevice )
														 , m_pInterface( pInterface )
														 , m_pTicketImage( NULL )
														 , m_pBuyType( NULL )
														 , m_state( ESTATE_INACTIVE )
{
	memset( m_pNumText, 0, sizeof(CBasicTextBox*)*LottoSystem::LOTTO_SLOT_NUM_CONT );
}

CLottoBuyListPropertyUIComp::~CLottoBuyListPropertyUIComp() { }

void CLottoBuyListPropertyUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_BUY_LIST_COMP_PROPERTY_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
	}

	// Image;
	{
		CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
		pImage->CreateSub ( this, "LOTTO_BUY_LIST_COMP_PROPERTY_TICKET_IMAGE", UI_FLAG_DEFAULT );
		pImage->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
		RegisterControl( pImage );

		m_pTicketImage = new CSwapImage( m_pEngineDevice );
		m_pTicketImage->CreateSub ( this, "LOTTO_BUY_LIST_COMP_PROPERTY_TICKET_IMAGE", UI_FLAG_DEFAULT );
		m_pTicketImage->SetImage( "LOTTO_ICON" );
		m_pTicketImage->SetVisibleSingle( FALSE );
		RegisterControl( m_pTicketImage );
	}

	// Text Box;
	{
		m_pBuyType = new CBasicTextBox( m_pEngineDevice );
		m_pBuyType->CreateSub( this, "LOTTO_BUY_LIST_COMP_PROPERTY_BUY_TYPE", UI_FLAG_YSIZE );
		m_pBuyType->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
		m_pBuyType->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_LEFT );
		m_pBuyType->AddText( ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT", 0 ), NS_UITEXTCOLOR::YELLOW );
		m_pBuyType->SetVisibleSingle( FALSE );
		RegisterControl( m_pBuyType );

		static std::string strNumTextId[ LottoSystem::LOTTO_SLOT_NUM_CONT ] = {
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM01",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM02",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM03",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM04",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM05",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM06",
		};

		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{
			m_pNumText[ i ] = new CBasicTextBox( m_pEngineDevice );
			m_pNumText[ i ]->CreateSub( this, strNumTextId[ i ].c_str(), UI_FLAG_YSIZE );
			m_pNumText[ i ]->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			m_pNumText[ i ]->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
			m_pNumText[ i ]->AddText( "1", NS_UITEXTCOLOR::WHITE );
			m_pNumText[ i ]->SetVisibleSingle( FALSE );
			RegisterControl( m_pNumText[ i ] );
		}
	}
}

void CLottoBuyListPropertyUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoBuyListPropertyUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoBuyListPropertyUIComp::SetPropertyState( PROPERTY_STATE state )
{
	m_state = state;

	m_pTicketImage->SetVisibleSingle( FALSE );
	m_pBuyType->SetVisibleSingle( FALSE );
	for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		m_pNumText[ i ]->SetVisibleSingle( FALSE );

	switch( m_state )
	{
	case ESTATE_ACTIVE:
		m_pTicketImage->SetVisibleSingle( TRUE );
		m_pBuyType->SetVisibleSingle( TRUE );
		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
			m_pNumText[ i ]->SetVisibleSingle( TRUE );
		break;
	case ESTATE_INACTIVE:
		break;
	}
}

void CLottoBuyListPropertyUIComp::SetNumText( unsigned int idx, unsigned int num )
{
	if( idx >= 0 && idx < LottoSystem::LOTTO_SLOT_NUM_CONT )
	{
		CBasicTextBox* pTextBox = m_pNumText[ idx ];
		if( pTextBox )
		{
			pTextBox->SetOneLineText(
				sc::string::format( "%1%", num ).c_str(),
				NS_UITEXTCOLOR::WHITE );
		}
	}
}

void CLottoBuyListPropertyUIComp::SetBuyType( LottoSystem::EMLOTTO_BUY_TYPE buyType, unsigned int idx )
{
	std::string strIdx;
	if( idx > 0 )
	{
		unsigned int preIdx = idx/26;
		unsigned int postIdx = idx%26;

		if( preIdx > 0 )
			strIdx += ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT_IDX", preIdx-1 );
		if( postIdx >= 0 )
			strIdx += ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT_IDX", postIdx );
	}
	else
		strIdx = ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT_IDX", 0 );

	switch( buyType )
	{
	case LottoSystem::AUTO_BUY:
		m_pBuyType->SetOneLineText(
			sc::string::format( ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT", 0 ),
			strIdx ).c_str(),
			NS_UITEXTCOLOR::YELLOW );
		break;
	case LottoSystem::MANUAL_BUY:
		m_pBuyType->SetOneLineText(
			sc::string::format( ID2GAMEWORD( "LOTTO_BUY_TYPE_TEXT", 1 ),
			strIdx ).c_str(),
			NS_UITEXTCOLOR::YELLOW );
		break;
	}
}

void CLottoBuyListPropertyUIComp::SetBuyInfo( const LottoSystem::LOTTO_SYSTEM_BUY buyInfo, unsigned int idx )
{
	m_buyInfo = buyInfo;

	switch( buyInfo.lottoBuyType )
	{
	case LottoSystem::NONE_BUY_TYPE:
		SetPropertyState( ESTATE_INACTIVE );
		break;
	case LottoSystem::AUTO_BUY:
	case LottoSystem::MANUAL_BUY:
		SetPropertyState( ESTATE_ACTIVE );
		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{
			if( i >= 0 && i < LottoSystem::MAX_DRAWING_NUM_LIST )
				SetNumText( i, buyInfo.drawingNum[ i ] );
		}
		SetBuyType( buyInfo.lottoBuyType, idx );
		break;
	}
}