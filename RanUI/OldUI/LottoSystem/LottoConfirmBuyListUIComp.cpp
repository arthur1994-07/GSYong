#include "stdafx.h"

#include "./LottoConfirmBuyListUIComp.h"
#include "./LottoConfirmBuyListBoxUIComp.h"
#include "./LottoConfirmBuyListPropertyUIComp.h"
#include "./LottoWinNumUIComp.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoConfirmBuyListUIComp::CLottoConfirmBuyListUIComp( CInnerInterface* pInterface
													   , EngineDeviceMan* pEngineDevice )
													   : CLottoBuyListUIComp( pInterface
													   , pEngineDevice )
													   , m_pListBox( NULL )
													   , m_bClickConfrim( false )
{
}

CLottoConfirmBuyListUIComp::~CLottoConfirmBuyListUIComp() { }

void CLottoConfirmBuyListUIComp::CreateSubControl()
{
	CLottoBuyListUIComp::CreateSubControl();

	// List Box;
	{
		m_pListBox = new CLottoConfirmBuyListBoxUIComp( m_pInterface, m_pEngineDevice );
		m_pListBox->CreateSub( this, "LOTTO_BUY_LIST_COMP_BUY_LIST_REGION", UI_FLAG_DEFAULT );
		m_pListBox->Create( "LOTTO_BUY_LIST_COMP_PROPERTY_REGION",
			0/*"YELLOW_LINE_GRAY_BACKGROUND"*/,
			"LOTTO_BUY_LIST_COMP_LISTBOX_SCROOL", 1, 6, 1 );
		RegisterControl( m_pListBox );
	}

	m_pListBox->SetCurSel( 0 );
}

void CLottoConfirmBuyListUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CLottoBuyListUIComp::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoConfirmBuyListUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CLottoBuyListUIComp::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoConfirmBuyListUIComp::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
											   , int idx)
{
	if( !pLottoSystem )
		return;

	// 먼저 확인하기 상태를 끈다;
	ConfirmStateOff();

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pListBox )
	{
		m_pListBox->ClearItem();
		for( unsigned int i=0; i<pLottoSystem->GetConfirmBuyCount(); ++i )
		{
			// 설정된 최대 구매리스트 개수 만큼 추가;
			if( m_pListBox->GetTotalLine() < static_cast<int>( i+1 ) )
				m_pListBox->AddItem( "" );			// 기본 형식이 있으므로 값을 넣을 필요 없음;

			// 이전 회의 구매 정보 가져오기;
			LottoSystem::LOTTO_SYSTEM_BUY buyInfo = pLottoSystem->GetConfirmBuyInfo( i );
			m_pListBox->SetItem( i, "", buyInfo );

			// 이전 회의 구매 개수 세팅;
			m_pBuyListTextBox->SetOneLineText(
				sc::string::format( ID2GAMEINTEXT( "LOTTO_CONFIRM_BUY_LIST_TEXT" ),
				pLottoSystem->GetConfirmBuyCount() ).c_str(),
				NS_UITEXTCOLOR::WHITE );
		}

		// 만약 미확인 구매리스트가 없다면;
		if( pLottoSystem->GetConfirmBuyCount() == 0 )
		{
			// 당첨확인 버튼 비활성화;
			GLLottoSystemManUI::Instance()->InactiveConfirmButtonMainUI();
		}
		// 구매리스트가 존재한다면;
		else
		{
			// 당첨확인 버튼 활성화;
			GLLottoSystemManUI::Instance()->ActiveConfirmButtonMainUI();
		}
	}
}

void CLottoConfirmBuyListUIComp::UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	static LottoSystem::LOTTO_GLOBAL_VALUE gValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoGlobalValue();

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pWinNum )
	{
		// 아직 확인하지 않은 턴의 당첨번호를 세팅한다;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum;
		winNum = pLottoSystem->GetConfirmWinnerNum();

		if( sValue.bUseSort )
		{
			std::vector<unsigned int> vTempDrawingNum;
			for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
			{
				if( j >= 0 && j < sValue.nDrawingMain )
					vTempDrawingNum.push_back( winNum.lottoNum[ j ] );
			}

			sort( vTempDrawingNum.begin(), vTempDrawingNum.end(), LottoSystem::CLottoSystemClient::CompareLottoNum );

			for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
			{
				if( j >= 0 && j < sValue.nDrawingMain )
					winNum.lottoNum[ j ] = vTempDrawingNum.at( j );
			}
		}

		for( unsigned int i=0; i<sValue.nDrawingTotal; ++i )
		{
			if( i >= 0 && i < LottoSystem::MAX_DRAWING_NUM_LIST )
				m_pWinNum->SetNumImage( i, winNum.lottoNum[ i ] );
		}
	}
}

void CLottoConfirmBuyListUIComp::UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	unsigned int confirmTurnNum = pLottoSystem->GetConfirmTurnNum();

	if( m_pWinNumTextBox )
	{
		m_pWinNumTextBox->SetOneLineText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_WIN_NUM_TEXT" ),
			confirmTurnNum+1 ).c_str(),
			NS_UITEXTCOLOR::WHITE );
	}
}

void CLottoConfirmBuyListUIComp::ConfirmRanking()
{
	if( m_pListBox )
	{
		if( !m_bClickConfrim )
		{
			for( int i=0; i<m_pListBox->GetTotalLine(); ++i )
			{
				CLottoConfirmBuyListPropertyUIComp* pProperty =
					m_pListBox->GetProperty( i );

				if( pProperty )
				{
					pProperty->ConfirmStateOn();
				}
			}

			LottoSystem::CLottoSystemClient* pLottoSystem =
				LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
				GLLottoSystemManUI::Instance()->GetCurTabIndex() );

			if( pLottoSystem )
				pLottoSystem->ConfirmBuyList();

			GLLottoSystemManUI::Instance()->InactiveConfirmButtonMainUI();

			m_bClickConfrim = true;
		}
	}
}

void CLottoConfirmBuyListUIComp::ConfirmStateOff()
{
	if( m_pListBox )
	{
		if( m_bClickConfrim )
		{
			for( int i=0; i<m_pListBox->GetTotalLine(); ++i )
			{
				CLottoConfirmBuyListPropertyUIComp* pProperty =
					m_pListBox->GetProperty( i );

				if( pProperty )
				{
					pProperty->ConfirmStateOff();
				}
			}

			m_bClickConfrim = false;
		}
	}
}

void CLottoConfirmBuyListUIComp::CloseUI()
{
	ConfirmStateOff();
}