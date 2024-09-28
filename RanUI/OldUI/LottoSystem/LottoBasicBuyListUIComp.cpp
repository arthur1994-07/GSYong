#include "stdafx.h"

#include "./LottoBasicBuyListUIComp.h"
#include "./LottoBasicBuyListBoxUIComp.h"
#include "./LottoWinNumUIComp.h"

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

CLottoBasicBuyListUIComp::CLottoBasicBuyListUIComp( CInnerInterface* pInterface
												   , EngineDeviceMan* pEngineDevice )
												   : CLottoBuyListUIComp( pInterface
												   , pEngineDevice )
{
}

CLottoBasicBuyListUIComp::~CLottoBasicBuyListUIComp() { }

void CLottoBasicBuyListUIComp::CreateSubControl()
{
	CLottoBuyListUIComp::CreateSubControl();

	// List Box;
	{
		m_pListBox = new CLottoBasicBuyListBoxUIComp( m_pInterface, m_pEngineDevice );
		m_pListBox->CreateSub( this, "LOTTO_BUY_LIST_COMP_BUY_LIST_REGION", UI_FLAG_DEFAULT );
		m_pListBox->Create( "LOTTO_BUY_LIST_COMP_PROPERTY_REGION",
			0/*"YELLOW_LINE_GRAY_BACKGROUND"*/,
			"LOTTO_BUY_LIST_COMP_LISTBOX_SCROOL", 1, 6, 1 );
		RegisterControl( m_pListBox );
	}

	m_pListBox->SetCurSel( 0 );
}

void CLottoBasicBuyListUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CLottoBuyListUIComp::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoBasicBuyListUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CLottoBuyListUIComp::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoBasicBuyListUIComp::SetVisibleSingle( BOOL bVisible )
{
	CLottoBuyListUIComp::SetVisibleSingle( bVisible );

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemUsingTabIdx(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( bVisible && m_pListBox )
	{
		// 세팅;
		for( unsigned int i=0; i<sValue.buyMultiNum; ++i )
		{
			// 설정된 최대 구매리스트 개수 만큼 추가;
			if( m_pListBox->GetTotalLine() < static_cast<int>( i+1 ) )
				m_pListBox->AddItem( "" );			// 기본 형식이 있으므로 값을 넣을 필요 없음;

			// 구매 정보 가져오기;
			LottoSystem::LOTTO_SYSTEM_BUY buyInfo;
			m_pListBox->SetItem( i, "", buyInfo );
		}

		// 구매 개수 세팅;
		m_pBuyListTextBox->SetOneLineText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MY_BUY_LIST_TEXT" ),
			pLottoSystem->GetBuyCount(), sValue.buyMultiNum ).c_str(),
			NS_UITEXTCOLOR::WHITE );
	}
}

void CLottoBasicBuyListUIComp::UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem
											 , int idx )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pListBox )
	{
		// 세팅;
		for( unsigned int i=0; i<sValue.buyMultiNum; ++i )
		{
			// 설정된 최대 구매리스트 개수 만큼 추가;
			if( m_pListBox->GetTotalLine() < static_cast<int>( i+1 ) )
				m_pListBox->AddItem( "" );			// 기본 형식이 있으므로 값을 넣을 필요 없음;

			// 구매 정보 가져오기;
			LottoSystem::LOTTO_SYSTEM_BUY buyInfo;
			if( i == idx )
			{
				buyInfo = pLottoSystem->GetBuyInfo( i );
				m_pListBox->SetItem( i, "", buyInfo );
			}

			if( idx == -1 )
				m_pListBox->SetItem( i, "", buyInfo );
		}
	}
}

void CLottoBasicBuyListUIComp::UpdateBuyListCount( const LottoSystem::CLottoSystemClient* pLottoSystem
												  , int count )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pListBox )
	{
		// 구매 개수 세팅;
		m_pBuyListTextBox->SetOneLineText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_MY_BUY_LIST_TEXT" ),
			count, sValue.buyMultiNum ).c_str(),
			NS_UITEXTCOLOR::WHITE );
		
	}
}

void CLottoBasicBuyListUIComp::UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pWinNum )
	{
		// 당첨 번호를 세팅한다;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum;
		int winNumCount = 0;
		if( pLottoSystem->GetLottoSystemState() == LottoSystem::LOTTO_STATE_BASIC ||
			pLottoSystem->GetLottoSystemState() == LottoSystem::LOTTO_STATE_GATHERING )
			winNum = pLottoSystem->GetWinnerNum();

		if( pLottoSystem->GetLottoSystemState() == LottoSystem::LOTTO_STATE_DRAWING )
		{
			winNum = pLottoSystem->GetWinnerNum();
			winNumCount = pLottoSystem->GetWinnerNumCount();
		}

		if( sValue.bUseSort && ( winNumCount == 0 ) )
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
			if( winNumCount != 0 )
			{
				if( i < static_cast<unsigned int>( winNumCount ) )
					m_pWinNum->SetNumImage( i, winNum.lottoNum[ i ] );
				else
					m_pWinNum->SetNumImage( i, 0 );
			}
			else
			{
				if( i >= 0 && i < LottoSystem::MAX_DRAWING_NUM_LIST )
					m_pWinNum->SetNumImage( i, winNum.lottoNum[ i ] );
			}
		}
	}
}

void CLottoBasicBuyListUIComp::UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	unsigned int curTurnNum = pLottoSystem->GetCurTurnNum();

	// 대기 상태, 추첨 상태일때만 이번 회차의 당첨번호를 출력한다;
	switch( pLottoSystem->GetLottoSystemState() )
	{
	case LottoSystem::LOTTO_STATE_WAIT:
	case LottoSystem::LOTTO_STATE_DRAWING:
		curTurnNum += 1;
		break;
	}

	if( m_pWinNumTextBox )
	{
		m_pWinNumTextBox->SetOneLineText(
			sc::string::format( ID2GAMEINTEXT( "LOTTO_WIN_NUM_TEXT" ),
			curTurnNum ).c_str(),
			NS_UITEXTCOLOR::WHITE );
	}
}