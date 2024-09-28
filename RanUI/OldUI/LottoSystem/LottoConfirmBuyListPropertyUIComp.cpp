#include "stdafx.h"

#include "./LottoConfirmBuyListPropertyUIComp.h"
#include "./GLLottoSystemManUI.h"

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

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoConfirmBuyListPropertyUIComp::CLottoConfirmBuyListPropertyUIComp( CInnerInterface* pInterface
																	   , EngineDeviceMan* pEngineDevice )
																	   : CLottoBuyListPropertyUIComp( pInterface
																	   , pEngineDevice )
																	   , m_confirmState( ESTATE_INCONFIRM )
																	   , m_pConfirmRanking( NULL )
{
	memset( m_pNumImage, 0, sizeof(CBasicTextBox*)*LottoSystem::LOTTO_SLOT_NUM_CONT );
}

CLottoConfirmBuyListPropertyUIComp::~CLottoConfirmBuyListPropertyUIComp() { }

void CLottoConfirmBuyListPropertyUIComp::CreateSubControl()
{
	CLottoBuyListPropertyUIComp::CreateSubControl();

	m_pConfirmRanking = new CBasicTextBox( m_pEngineDevice );
	m_pConfirmRanking->CreateSub( this, "LOTTO_BUY_LIST_COMP_PROPERTY_REGION", UI_FLAG_YSIZE, EConfirmRanking );
	m_pConfirmRanking->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 20, _DEFAULT_FONT_BOLD_FLAG ) );
	m_pConfirmRanking->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
	m_pConfirmRanking->AddText( "", NS_UITEXTCOLOR::RED );
	m_pConfirmRanking->SetVisibleSingle( FALSE );
	RegisterControl( m_pConfirmRanking );

	// 번호가 맞는지 아닌지 확인하는 Image;
	{
		static std::string strNumTextId[ LottoSystem::LOTTO_SLOT_NUM_CONT ] = {
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE01",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE02",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE03",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE04",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE05",
			"LOTTO_BUY_LIST_COMP_PROPERTY_NUM_IMAGE06",
		};


		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{
			m_pNumImage[ i ] = new CSwapImage( m_pEngineDevice );
			m_pNumImage[ i ]->CreateSub ( this, strNumTextId[ i ].c_str(), UI_FLAG_DEFAULT );
			m_pNumImage[ i ]->SetImage( "LOTTO_ICON" );
			m_pNumImage[ i ]->SetVisibleSingle( FALSE );
			RegisterControl( m_pNumImage[ i ] );
		}
	}
}

void CLottoConfirmBuyListPropertyUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CLottoBuyListPropertyUIComp::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoConfirmBuyListPropertyUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CLottoBuyListPropertyUIComp::TranslateUIMessage( ControlID, dwMsg );

	switch( ControlID )
	{
	case EConfirmRanking:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			if( m_pConfirmRanking && ( m_state == ESTATE_ACTIVE ) )
			{
				if( m_buyInfo.ranking == 0 )
				{
					m_pConfirmRanking->SetOneLineText(
						ID2GAMEINTEXT("LOTTO_BUY_LIST_CONFIRM_WIN_FAIL_TEXT"),
						NS_UITEXTCOLOR::RED );
				}
				else
				{
					m_pConfirmRanking->SetOneLineText(
						sc::string::format( ID2GAMEINTEXT("LOTTO_BUY_LIST_CONFIRM_WIN_TEXT"), m_buyInfo.ranking ).c_str(),
						NS_UITEXTCOLOR::WHITE );
				}

				m_pTicketImage->SetVisibleSingle( FALSE );
				m_pBuyType->SetVisibleSingle( FALSE );
				for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
				{
					m_pNumText[ i ]->SetVisibleSingle( FALSE );
					m_pNumImage[ i ]->SetVisibleSingle( FALSE );
				}	
			}
		}
		else
		{
			if( m_pConfirmRanking )
				m_pConfirmRanking->SetOneLineText( "" );

			if( m_state == ESTATE_ACTIVE )
			{
				m_pTicketImage->SetVisibleSingle( TRUE );
				m_pBuyType->SetVisibleSingle( TRUE );
				for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
				{
					m_pNumText[ i ]->SetVisibleSingle( TRUE );
					m_pNumImage[ i ]->SetVisibleSingle( TRUE );
				}					
			}
		}
		break;
	}
}

void CLottoConfirmBuyListPropertyUIComp::SetPropertyState( CLottoBuyListPropertyUIComp::PROPERTY_STATE state )
{
	CLottoBuyListPropertyUIComp::SetPropertyState( state );
}

void CLottoConfirmBuyListPropertyUIComp::ConfirmStateOn()
{
	m_confirmState = ESTATE_CONFIRM;

	m_pConfirmRanking->SetVisibleSingle( TRUE );
	for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
	{
		if( m_pNumImage[ i ] )
		{
			if( m_buyInfo.drawingNumState[ i ] )
				m_pNumImage[ i ]->SetImage( "LOTTO_O" );
			else
				m_pNumImage[ i ]->SetImage( "LOTTO_X" );

			if( m_state == CLottoBuyListPropertyUIComp::ESTATE_ACTIVE )
				m_pNumImage[ i ]->SetVisibleSingle( TRUE );
			else
				m_pNumImage[ i ]->SetVisibleSingle( FALSE );
		}
	}
}

void CLottoConfirmBuyListPropertyUIComp::ConfirmStateOff()
{
	m_confirmState = ESTATE_INCONFIRM;

	m_pConfirmRanking->SetVisibleSingle( FALSE );
	for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		m_pNumImage[ i ]->SetVisibleSingle( FALSE );
}