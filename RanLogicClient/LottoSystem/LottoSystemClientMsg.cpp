#include "pch.h"

#include "./LottoSystemClient.h"

#include "./GLLottoGlobalDataClient.h"

#include "./GLLottoSystemManClient.h"

#include "../../SigmaCore/Json/MinJson.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

namespace LottoSystem
{

	// 서버와의 데이터 동기화;
	void CLottoSystemClient::SyncStatusProc( GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC* pSyncMsg )
	{
		// 올바른 동기화 패킷;
		if( pSyncMsg->m_emAC == LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK )
		{
			GLLottoGlobalDataClient::Instance()->SyncStatus( pSyncMsg );

			// 추첨기 초기화;
			LOTTO_SYSTEM_CLIENT_VALUE systemValue =
				GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

			m_pDrawingMachine->Initialize(
				systemValue.nLottoTotal,
				systemValue.nDrawingTotal,
				systemValue.nDrawingMain,
				systemValue.nDrawingBonus );

			m_pDrawingMachine->InitializeNumCount();

			// 추첨 일시 갱신;
			UpdateDrawingDate();
		}
	}

	void CLottoSystemClient::NextDrawingDataProc( GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC* pNextDrawingDataMsg )
	{
		m_startTime = pNextDrawingDataMsg->m_startTime;
		m_endTime = pNextDrawingDataMsg->m_endTime;
	}

	void CLottoSystemClient::CurrentStateProc( GLMSG::NET_LOTTO_MESSAGE_CURRENT_STATE_AC* pCurStateMsg )
	{
		m_curState = pCurStateMsg->m_curState;

		// 상태 변경 갱신;
		AddEvent( EUPDATE_CURRENT_STATE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Lotto_ChangeState,
			"-n, -n", pCurStateMsg->m_lottoSystemId, (int) m_curState );
	}

	// 현재 턴을 얻어왔을 때의 Proc;
	void CLottoSystemClient::CurrentTurnProc( GLMSG::NET_LOTTO_MESSAGE_CURRENT_TURN_AC* pCurTurnMsg )
	{
		m_curTurnNum = pCurTurnMsg->m_turn;

		AddEvent( EUPDATE_TURN );
	}

	void CLottoSystemClient::GiveMoneyPostProc( GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC* pPostMsg )
	{
		unsigned int turnNum = pPostMsg->m_turnNum;
		unsigned int ranking = pPostMsg->m_ranking;
		LONGLONG llWinMoney = pPostMsg->m_winMoney;

		std::string strWinMoney;
		switch( pPostMsg->m_moneyType )
		{
		case LottoSystem::LOTTO_MONEY_TYPE_GAME_MONEY:
			strWinMoney = sc::string::format( "%d",
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
				llWinMoney, 3, "," ) );
			break;
		case LottoSystem::LOTTO_MONEY_TYPE_POINT:
			strWinMoney = sc::string::format( "%d Point",
				NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
				llWinMoney, 3, "," ) );
			break;
		}

		json_spirit::Object inputObj;
		inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EVIEW_POST_MESSAGE ) ) );
		inputObj.push_back( json_spirit::Pair( "turnNum", static_cast<int>( turnNum ) ) );
		inputObj.push_back( json_spirit::Pair( "ranking", static_cast<int>( ranking ) ) );
		inputObj.push_back( json_spirit::Pair( "winMoney", strWinMoney ) );

		std::string inputString = json_spirit::write( inputObj );

		AddJsonEvent( inputString );
	}

	void CLottoSystemClient::ReqLottoToBuyProc( GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC* pBuyMsg )
	{
		switch( pBuyMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_OK:
			if( pBuyMsg->m_lottoBuy.lottoBuyType != NONE_BUY_TYPE )
			{
				m_buyList.buyList.push_back( pBuyMsg->m_lottoBuy );
			}

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Lotto_BuyListUpdate,
				"-n, -b", 
				pBuyMsg->m_lottoSystemId,
				false );
			//UpdateBuyList();
			break;
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_MONEY_FAIL:
			break;
		}	
	}

	void CLottoSystemClient::AccumulateMoneyProc( GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC* pAccumulateMsg )
	{
		switch( pAccumulateMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK:
			{
				// 누적 금액 갱신;
				unsigned int turnNum = pAccumulateMsg->m_turnNum;

				// 현재 회차;
				if( turnNum == m_curTurnNum )
				{
					m_accumulateMoney = pAccumulateMsg->m_accumulateMoney;

					// 이벤트 등록;
					AddEvent( EUPDATE_ACCUMULATE_MONEY );

					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Lotto_AccumulateMoney,
						"-n, -l, -b", 
						pAccumulateMsg->m_lottoSystemId,
						m_accumulateMoney,
						false );
				}
				// 그외 특정 회차;
				else
				{
					m_turnAccumulateMoney = pAccumulateMsg->m_accumulateMoney;

					// 이벤트 등록;
					AddEvent( EUPDATE_TURN_ACCUMULATE_MONEY );

					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Lotto_AccumulateMoney,
						"-n, -l, -b",
						pAccumulateMsg->m_lottoSystemId,
						m_turnAccumulateMoney,
						true );
				}
			}
			break;
		case LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC_FAIL:
			break;
		}
	}

	void CLottoSystemClient::BuyListProc( GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC* pBuyListMsg )
	{
		switch( pBuyListMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_OK:
			{
				unsigned int turnNum = pBuyListMsg->m_turnNum;
				if( turnNum == m_curTurnNum )
				{
					if( pBuyListMsg->m_lotto.lottoBuyType != NONE_BUY_TYPE )
					{
						m_buyList.buyList.push_back( pBuyListMsg->m_lotto );

						int idx = m_buyList.buyList.size()-1;

						json_spirit::Object inputObj;
						inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EUPDATE_BUY_LIST ) ) );
						inputObj.push_back( json_spirit::Pair( "index", idx ) );

						std::string inputString = json_spirit::write( inputObj );

						AddJsonEvent( inputString );
					}
					else
						AddEvent( EUPDATE_BUY_LIST );
				}
				else
				{
					// 다른 회차의 구매리스트;
					if( pBuyListMsg->m_lotto.lottoBuyType != NONE_BUY_TYPE )
						m_confirmBuyList.buyList.push_back( pBuyListMsg->m_lotto ); //확인하지 않은 구매리스트

					m_confirmTurnNum = turnNum;

					AddEvent( EUPDATE_CONFIRM_BUY_LIST );
				}
			}	
			break;
		case LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_FAIL:
			break;
		}
	}

	void CLottoSystemClient::BuyListCountProc( GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC* pBuyListMsg )
	{
		if( pBuyListMsg->m_turnNum == m_curTurnNum )
		{
			json_spirit::Object inputObj;
			inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EUPDATE_BUY_LIST_COUNT ) ) );
			inputObj.push_back( json_spirit::Pair( "count", static_cast<int>( pBuyListMsg->m_count ) ) );

			std::string inputString = json_spirit::write( inputObj );
			AddJsonEvent( inputString );
		}

		const DWORD dwBuyListCount = pBuyListMsg->m_count;
		if ( m_buyList.buyList.size() != dwBuyListCount )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Lotto System ] [ BuyList Count Error : %1% / %2% ]",
				m_buyList.buyList.size(),
				dwBuyListCount ) );

			DWORD dwCount = 0;
			std::vector<LOTTO_SYSTEM_BUY>::iterator iter = m_buyList.buyList.begin();
			for ( iter; iter != m_buyList.buyList.end(); )
			{
				if ( dwCount >= dwBuyListCount )
					iter = m_buyList.buyList.erase( iter );
				else
					++iter;

				dwCount++;
			}
		}
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Lotto_BuyListUpdate,
			"-n, -b", 
			pBuyListMsg->m_lottoSystemId,
			pBuyListMsg->m_turnNum == m_curTurnNum ? false : true );
	}

	// 전송된 WinNum 정보를 받았을 때의 Proc;
	void CLottoSystemClient::WinningNumProc( GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC* pWinningNumMsg )
	{
		switch( pWinningNumMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_WINNER_NUM_AC_OK:
			{
				unsigned int turnNum = pWinningNumMsg->m_turnNum;

				// 현재 회차;
				if( turnNum == m_curTurnNum )
				{
					LuaTable tbNum( GLWidgetScript::GetInstance().GetLuaState() );

					for( unsigned int i=0; i<LottoSystem::MAX_DRAWING_NUM_LIST; ++i )
					{
						m_winnerNum.lottoNum[ i ] = pWinningNumMsg->m_winnerNum.lottoNum[ i ];

						tbNum.set( i + 1, pWinningNumMsg->m_winnerNum.lottoNum[ i ] );
					}
					m_winnerNumCount = pWinningNumMsg->m_winnerNumCount;

					AddEvent( EUPDATE_WIN_NUM );

					GLWidgetCallFuncValue args[ 2 ];
					args[ 0 ].SetInt( pWinningNumMsg->m_lottoSystemId );
					args[ 1 ].SetTable( &tbNum );

					GLWidgetScript::GetInstance().LuaCallFunc<void>(
						NSWIDGET_SCRIPT::g_strFunc_Lotto_WinNumUpdate, args, 2 );
				}
				// 당첨 확인 회차;
				else if( turnNum == m_confirmTurnNum )
				{
					LuaTable tbNum( GLWidgetScript::GetInstance().GetLuaState() );

					for( unsigned int i = 0; i < LottoSystem::MAX_DRAWING_NUM_LIST; ++i )
					{
						m_confirmWinnerNum.lottoNum[ i ] = pWinningNumMsg->m_winnerNum.lottoNum[ i ];
						m_turnWinnerNum.lottoNum[ i ] = pWinningNumMsg->m_winnerNum.lottoNum[ i ];

						tbNum.set( i + 1, pWinningNumMsg->m_winnerNum.lottoNum[ i ] );
					}

					AddEvent( EUPDATE_CONFIRM_TURN_WIN_NUM );
					AddEvent( EUPDATE_TURN_WIN_NUM );

					GLWidgetCallFuncValue args[ 2 ];
					args[ 0 ].SetInt( pWinningNumMsg->m_lottoSystemId );
					args[ 1 ].SetTable( &tbNum );

					GLWidgetScript::GetInstance().LuaCallFunc<void>(
						NSWIDGET_SCRIPT::g_strFunc_Lotto_WinNumUpdate, args, 2 );
				}
				// 특정 회차;
				else
				{					
					LuaTable tbNum( GLWidgetScript::GetInstance().GetLuaState() );

					for( unsigned int i=0; i<LottoSystem::MAX_DRAWING_NUM_LIST; ++i )
					{
						m_turnWinnerNum.lottoNum[ i ] = pWinningNumMsg->m_winnerNum.lottoNum[ i ];

						tbNum.set( i + 1, pWinningNumMsg->m_winnerNum.lottoNum[ i ] );
					}

					AddEvent( EUPDATE_TURN_WIN_NUM );

					GLWidgetCallFuncValue args[ 2 ];
					args[ 0 ].SetInt( pWinningNumMsg->m_lottoSystemId );
					args[ 1 ].SetTable( &tbNum );

					GLWidgetScript::GetInstance().LuaCallFunc<void>(
						NSWIDGET_SCRIPT::g_strFunc_Lotto_WinNumUpdate, args, 2 );
				}
			}
			break;
		case LottoSystem::EMLOTTO_REQ_WINNER_NUM_AC_FAIL:
			break;
		}
	}

	void CLottoSystemClient::WinningManListProc( GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC* pWinningManListMsg )
	{
		switch( pWinningManListMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_WINNER_MAN_LIST_AC_OK:
			{
				LOTTO_SYSTEM_WINNER_MAN winnerMan =
					pWinningManListMsg->m_winnerMan;

				if( winnerMan.ranking != 0 )
				{
					m_winnerManList.winnerManList.push_back( winnerMan );
					m_winnerManList.winnerManNameList.push_back( winnerMan.szChaName );
				}

				AddEvent( EUPDATE_WIN_MAN_LIST );

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Lotto_WinManUpdate,
					"-dw", GetLottoSystemId() );
			}
			break;
		case LottoSystem::EMLOTTO_REQ_WINNER_MAN_LIST_AC_FAIL:
			break;
		}
	}

	void CLottoSystemClient::ChangeStateProc( GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_AB* pMsg )
	{
		m_curState = pMsg->m_curState;

		LOTTO_SYSTEM_CLIENT_VALUE systemValue = 
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		// 추첨 대기 상태가 되면 먼저 WinnerNum을 초기화한다;
		if( m_curState == LOTTO_STATE_WAIT )
		{
			AddEvent( EUPDATE_WIN_NUM );

			m_winnerNum.Clear();
		}

		// 상태 변경 갱신;
		AddEvent( EUPDATE_CHANGE_STATE );

		// 집계 상태로 변할 때 다음 추첨일 갱신;
		if( m_curState == LOTTO_STATE_GATHERING )
		{
			AddEvent( EUPDATE_DRAWING_DATE );
		}

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Lotto_ChangeState,
			"-n, -n", pMsg->m_lottoSystemId, (int) m_curState );
	}

	// 다음턴으로 넘어갔을 때의 Proc;
	void CLottoSystemClient::GoNextTurnProc( GLMSG::NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB* pGoNextTurnMsg )
	{
		m_curTurnNum = pGoNextTurnMsg->m_turn;
		m_accumulateMoney = 0;
		m_buyList.buyList.clear();

		if( m_curTurnNum > 0 && m_confirmBuyList.buyList.empty() )
		{
			m_confirmTurnNum = m_curTurnNum-1;
			ReqPreWinNum( m_confirmTurnNum );
			ReqPreBuyList( m_confirmTurnNum );
		}

		UpdateBuyList();

		// 턴이 갱신되면 여러가지가 같이 갱신된다;
		AddEvent( EUPDATE_TURN );
		AddEvent( EUPDATE_BUY_LIST );
		AddEvent( EUPDATE_ACCUMULATE_MONEY );
		AddEvent( EUPDATE_WIN_NUM );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Lotto_NextTurn,
			"-n", pGoNextTurnMsg->m_lottoSystemId );
	}

	void CLottoSystemClient::DrawingNumProc( GLMSG::NET_LOTTO_MESSAGE_DRAWING_NUM_AB* pDrawingNumMsg )
	{
		m_winnerNumCount = pDrawingNumMsg->m_drawingCount;
		unsigned int count = pDrawingNumMsg->m_drawingCount-1;
		if( count >= 0 && count < MAX_DRAWING_NUM_LIST )
			m_winnerNum.lottoNum[ count ] = pDrawingNumMsg->m_curDrawingNum;

		json_spirit::Object inputObj;
		inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EUPDATE_DRAWING_NUM ) ) );
		inputObj.push_back( json_spirit::Pair( "drawingNum", static_cast<int>( pDrawingNumMsg->m_curDrawingNum ) ) );

		std::string inputString = json_spirit::write( inputObj );
		AddJsonEvent( inputString );
	}

	void CLottoSystemClient::NumDSPProc( GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC* pDspMsg )
	{
		int num = static_cast<int>( pDspMsg->m_num );
		int drawingCount = static_cast<int>( pDspMsg->m_drawingCount );

		json_spirit::Object inputObj;
		inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EUPDATE_NUM_DSP ) ) );
		inputObj.push_back( json_spirit::Pair( "num", num ) );
		inputObj.push_back( json_spirit::Pair( "drawingCount", drawingCount ) );

		std::string inputString = json_spirit::write( inputObj );
		AddJsonEvent( inputString );
	}

	void CLottoSystemClient::NumProc( GLMSG::NET_LOTTO_MESSAGE_NUM_AC* pNumMsg )
	{
		int num = static_cast<int>( pNumMsg->m_num );
		int addCount = static_cast<int>( pNumMsg->m_addCount );

		json_spirit::Object inputObj;
		inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EUPDATE_NUM ) ) );
		inputObj.push_back( json_spirit::Pair( "num", num ) );
		inputObj.push_back( json_spirit::Pair( "addCount", addCount ) );

		std::string inputString = json_spirit::write( inputObj );
		AddJsonEvent( inputString );
	}

	void CLottoSystemClient::NumInitProc( GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_AC* pNumInitMsg )
	{
		AddEvent( EUPDATE_NUM_INIT );
	}

	void CLottoSystemClient::NumAddProc( GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_AC* pNumAddMsg )
	{
		AddEvent( EUPDATE_NUM_ADD );
	}

	void CLottoSystemClient::NumDelProc( GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_AC* pNumDelMsg )
	{
		AddEvent( EUPDATE_NUM_DEL );
	}

	void CLottoSystemClient::InventNumDSPProc( GLMSG::NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC* pDspMsg )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
			pDspMsg->m_lottoSystemId );

		std::string strText = sc::string::format("[ %d ] : ", pDspMsg->m_count );
		for( unsigned int i=0; i<sValue.nDrawingTotal; ++i )
		{
			if( i >= 0 && i < MAX_DRAWING_NUM_LIST )
			{
				strText += sc::string::format( "%d ",
					pDspMsg->m_winnerNum.lottoNum[ i ] );
			}
		}

		json_spirit::Object inputObj;
		inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EVIEW_NUM_DSP ) ) );
		inputObj.push_back( json_spirit::Pair( "context", strText ) );

		std::string inputString = json_spirit::write( inputObj );
		AddJsonEvent( inputString );
	}

	void CLottoSystemClient::LockBuyProc( GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC* pLockBuyMsg )
	{
		m_bLockBuyState = pLockBuyMsg->m_bBuyLock;

		AddEvent( EVIEW_LOCK_BUY );
	}

	void CLottoSystemClient::LockChangeTurnProc( GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC* pChangeTurnMsg )
	{
		m_bLockChangeTurn = pChangeTurnMsg->m_bChangeTurnLock;

		AddEvent( EVIEW_LOCK_CHANGE_TURN );
	}
}