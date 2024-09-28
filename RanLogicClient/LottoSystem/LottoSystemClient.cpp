#include "pch.h"

#include "../GLGaeaClient.h"

#include "../Char/GLCharacter.h"

#include "../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "./GLLottoGlobalDataClient.h"

#include "./GLLottoSystemManClient.h"

#include "./LottoSystemClient.h"

namespace LottoSystem
{

	CLottoSystemClient::CLottoSystemClient( LOTTO_SYSTEM_ID id
		, GLGaeaClient* pGaeaClient )
		: m_id( id )
		, m_bLockBuyState( false )
		, m_bLockChangeTurn( false )
		, m_curTurnNum( 0 )
		, m_pGaeaClient( pGaeaClient )
		, m_curState( LOTTO_STATE_BASIC )
		, m_accumulateMoney( 0 )
		, m_winnerNumCount( 0 )
		, m_bProgressState( false )
		, m_confirmTurnNum( 0 )
	{
		m_pDrawingMachine = new CLottoDrawingMachine();
	}

	CLottoSystemClient::~CLottoSystemClient()
	{
		SAFE_DELETE ( m_pDrawingMachine );
	}

	void CLottoSystemClient::Initialize()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		// 추첨기 초기화;
		m_pDrawingMachine->Initialize(
			systemValue.nLottoTotal,
			systemValue.nDrawingTotal,
			systemValue.nDrawingMain,
			systemValue.nDrawingBonus );

		// 클라이언트 쪽은 무조건 AddCount 초기화 상태;
		m_pDrawingMachine->InitializeNumCount();

		// 구매목록 초기화;
		m_buyList.buyList.clear();

		// 당첨확인 목록 갱신;
		m_confirmBuyList.buyList.clear();

		// 당첨자 목록 초기화;
		m_winnerManList.Clear();

		// 시스템 초기화;
		AddEvent( EUPDATE_INITIALIZE_SYSTEM );
	}

	void CLottoSystemClient::Destroy()
	{
		SAFE_DELETE( m_pDrawingMachine );
	}

	bool CLottoSystemClient::Update( float fElapsedTime, const CTime& currentTime )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return true;

		// 시간정보이므로 매 프레임마다 지속적으로 갱신시킨다;
		UpdateProgressTime( currentTime );

		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return false;

		// 클라이언트의 캐릭터 소지금 갱신;
		// 금액은 매프레임마다 지속적으로 갱신시킨다;
		switch( systemValue.moneyType )
		{
		case LOTTO_MONEY_TYPE_GAME_MONEY:
			m_haveMoney = pChar->GetInvenMoney();
			break;
		case LOTTO_MONEY_TYPE_POINT:
			m_haveMoney = pChar->PointShopPoint();
			break;
		}

		AddEvent( EUPDATE_HAVE_MONEY );

		return true;
	}

	bool CLottoSystemClient::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// Sync;
		if( pMsg->Type() == NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC )
			SyncStatusProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC*>( pMsg ) );

		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return true;

		// 상속 관계가 확실하기 때문에 static_cast로 구현한다;
		// dynamic_cast로 할 수도 있지만 속도를 좀 더 중시했다;
		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC:
			NextDrawingDataProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC:			// 현재 상태;
			CurrentStateProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CURRENT_STATE_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC:			// 현재 턴;
			CurrentTurnProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CURRENT_TURN_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC:
			GiveMoneyPostProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC:		// 구매 요청 결과;
			ReqLottoToBuyProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC:		// 누적 당첨금 요청 결과;
			AccumulateMoneyProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC:				// 구매 리스트 요청 결과;
			BuyListProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC:
			BuyListCountProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC:				// 당첨 번호 요청 결과;
			WinningNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC:		// 당첨자 리스트 요청 결과;
			WinningManListProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC*>( pMsg ) );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB:			// 상태 변경 패킷;
			ChangeStateProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_AB*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB:			// 회차 변경 패킷;
			GoNextTurnProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB:				// 추첨 번호 패킷;
			DrawingNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_DRAWING_NUM_AB*>( pMsg ) );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC:					// 사용자들이 현재까지 선택한 누적 번호의 수;
			NumDSPProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC:						// 로또 추첨 시의 각 번호가 번호당 몇개씩 들어 있는지 확인;
			NumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC:				// 로또의 각 번호를 하나가지고 있는 상태로 초기화;
			NumInitProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC:					// 특정 번호의 개수를 추가;
			NumAddProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC:					// 특정 번호의 개수를 삭제;
			NumDelProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_INVENT_NUM_DSP_AC:
			InventNumDSPProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_AC:
			LockBuyProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_AC:
			LockChangeTurnProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC*>( pMsg ) );
			break;
		}

		return true;
	}

	// 자동 구매 ( Test용이라고 보면 된다 );
	// 물론 자동 구매 기능이 필요할 때 사용해도 상관없다;
	CLottoSystemClient::BUY_LOTTO_FLAG CLottoSystemClient::BuyLottoAuto()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return BUY_LOTTO_FLAG_FAIL;

		if( m_bLockBuyState )
			return BUY_LOTTO_FLAG_LOCK_FAIL;

		// 현재 클라이언트의 캐릭터 DB num을 구한다;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		// 캐릭터가 자격이 되는지 확인한다;
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return BUY_LOTTO_FLAG_FAIL;

		unsigned int level = pChar->GetLevel();
		if( systemValue.buyLevel > level )
			return BUY_LOTTO_FLAG_LEVEL_FAIL;

		LONGLONG money = 0;
		switch( systemValue.moneyType )
		{
		case LOTTO_MONEY_TYPE_GAME_MONEY:
			money = pChar->GetInvenMoney();
			break;
		case LOTTO_MONEY_TYPE_POINT:
			money = pChar->PointShopPoint();
			break;
		}

		if( systemValue.buyInvenMoney > money )
			return BUY_LOTTO_FLAG_MONEY_FAIL;

		if( systemValue.lottoTicketMoney > money )
			return BUY_LOTTO_FLAG_MONEY_FAIL;

		if ( NULL == m_pDrawingMachine )
			return BUY_LOTTO_FLAG_FAIL;

		// 로또 번호를 만든다;
		if ( false == m_pDrawingMachine->IsInitialize() )
		{
			m_pDrawingMachine->Initialize(
				systemValue.nLottoTotal,
				systemValue.nDrawingTotal,
				systemValue.nDrawingMain,
				systemValue.nDrawingBonus );
			m_pDrawingMachine->InitializeNumCount();
		}

		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF msg(
			m_id, dwDbNum );
		msg.m_lottoBuy.lottoBuyType = AUTO_BUY;
		msg.m_lottoBuy.dwDbNum = dwDbNum;
		StringCchCopy( msg.m_lottoBuy.szChaName, CHAR_SZNAME, pChar->GetName() );
		for( unsigned int i=0; i<systemValue.nDrawingMain; ++i )
		{
			msg.m_lottoBuy.drawingNum[ i ] = m_pDrawingMachine->StartDrawingMachine();
			if ( 0 == msg.m_lottoBuy.drawingNum[ i ] )
			{
				// 추첨기가 제대로 동작하지 않을 경우에는 구입메시지를 보내지 않는다;
				m_pDrawingMachine->ClearDrawingMachine();
				return BUY_LOTTO_FLAG_FAIL;
			}

			msg.m_lottoBuy.drawingNumState[ i ] = false;
		}
		msg.m_lottoBuy.ranking = 0;

		// 추첨기 리셋;
		m_pDrawingMachine->ClearDrawingMachine();

		// 패킷을 보낸다;
		GLLottoSystemManClient::Instance()->SendToFieldServer( &msg );

		return BUY_LOTTO_FLAG_OK;
	}

	// 구매;
	CLottoSystemClient::BUY_LOTTO_FLAG CLottoSystemClient::BuyLottoTicket(
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM lottoNum,
		EMLOTTO_BUY_TYPE lottoBuyType,
		bool bBuy )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return BUY_LOTTO_FLAG_FAIL;

		if( m_bLockBuyState )
			return BUY_LOTTO_FLAG_LOCK_FAIL;

		// 현재 클라이언트의 캐릭터 DB num을 구한다;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		// 캐릭터가 자격이 되는지 확인한다;
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return BUY_LOTTO_FLAG_FAIL;

		// 레벨 검사;
		unsigned int level = pChar->GetLevel();
		if( systemValue.buyLevel > level )
			return BUY_LOTTO_FLAG_LEVEL_FAIL;

		// 소지금 검사;
		LONGLONG money = 0;
		switch( systemValue.moneyType )
		{
		case LOTTO_MONEY_TYPE_GAME_MONEY:
			money = pChar->GetInvenMoney();
			break;
		case LOTTO_MONEY_TYPE_POINT:
			money = pChar->PointShopPoint();
			break;
		}

		if( systemValue.buyInvenMoney > money )
			return BUY_LOTTO_FLAG_MONEY_FAIL;

		if( systemValue.lottoTicketMoney > money )
			return BUY_LOTTO_FLAG_MONEY_FAIL;

		// 로또 번호 검사;
		unsigned int num = 0;
		for( unsigned int i=0; i<systemValue.nDrawingMain; ++i )
		{
			if( lottoNum.lottoNum[ i ] == 0 )
				return BUY_LOTTO_FLAG_ZERO_FAIL;

			if( lottoNum.lottoNum[ i ] < 0 ||
				lottoNum.lottoNum[ i ] > systemValue.nLottoTotal )
				return BUY_LOTTO_FLAG_NUM_FAIL;

			for( unsigned int j=0; j<systemValue.nDrawingMain; ++j )
			{
				if( i != j &&
					lottoNum.lottoNum[ i ] == lottoNum.lottoNum[ j ] )
					return BUY_LOTTO_FLAG_DUPLICATE_FAIL;
			}
		}

		// 로또 번호를 보낸다;
		if( bBuy )
		{
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF msg(
				m_id, dwDbNum );

			msg.m_lottoBuy.lottoBuyType = lottoBuyType;
			msg.m_lottoBuy.dwDbNum = dwDbNum;
			StringCchCopy( msg.m_lottoBuy.szChaName, CHAR_SZNAME, pChar->GetName() );
			for( unsigned int i=0; i<systemValue.nDrawingMain; ++i )
			{
				msg.m_lottoBuy.drawingNum[ i ] = lottoNum.lottoNum[ i ];
				msg.m_lottoBuy.drawingNumState[ i ] = false;
			}
			msg.m_lottoBuy.ranking = 0;

			// 패킷을 보낸다;
			GLLottoSystemManClient::Instance()->SendToFieldServer( &msg );
		}

		return BUY_LOTTO_FLAG_OK;
	}

	LOTTO_SYSTEM_LOTTO_NUM CLottoSystemClient::MakeAutoLottoNum()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return LOTTO_SYSTEM_LOTTO_NUM();

		// 추첨기 확인;
		if( !m_pDrawingMachine )
			return LOTTO_SYSTEM_LOTTO_NUM();

		if ( false == m_pDrawingMachine->IsInitialize() )
		{
			m_pDrawingMachine->Initialize(
				systemValue.nLottoTotal,
				systemValue.nDrawingTotal,
				systemValue.nDrawingMain,
				systemValue.nDrawingBonus );
			m_pDrawingMachine->InitializeNumCount();
		}

		LOTTO_SYSTEM_LOTTO_NUM lottoNum;
		for( unsigned int i=0; i<systemValue.nDrawingMain; ++i )
			lottoNum.lottoNum[ i ] = m_pDrawingMachine->StartDrawingMachine();

		// 추첨기 리셋;
		m_pDrawingMachine->ClearDrawingMachine();

		return lottoNum;
	}

	// 구매 리스트 갱신;
	void CLottoSystemClient::UpdateBuyList()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		ReqPreBuyList( m_curTurnNum );
	}

	void CLottoSystemClient::UpdateConfirmBuyList()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if ( false == systemValue.bUse )
			return;

		ReqPreBuyList( m_confirmTurnNum );
	}

	void CLottoSystemClient::ReqPreBuyList( unsigned int turnNum )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		// 새로운 구매 리스트를 받아와야 하기 때문에 먼저 초기화한다;
		if( m_curTurnNum == turnNum )
			m_buyList.buyList.clear();
		else
			m_confirmBuyList.buyList.clear();

		// 현재 클라이언트의 캐릭터 DB num을 구한다;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	// 누적 금액 갱신;
	void CLottoSystemClient::UpdateAccumulateMoney()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		ReqPreAccumulateMoney( m_curTurnNum );
	}

	void CLottoSystemClient::ReqPreAccumulateMoney( unsigned int turnNum )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		// 현재 클라이언트의 캐릭터 DB num을 구한다;
		GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	// 추첨 일시 출력;
	void CLottoSystemClient::UpdateDrawingDate()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		// 추첨 일시 갱신;
		AddEvent( EUPDATE_DRAWING_DATE );
	}

	// 진행 상태 갱신;
	void CLottoSystemClient::UpdateProgressTime( const CTime& currentTime )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !sValue.bUse )
			return;

		// 추첨 날짜라면;
		if( currentTime.GetDay() == m_startTime.sTime.GetDay() )
		{
			m_bProgressState = true;

			m_progressState.emDrawingDayOfTheWeek =
				static_cast<EMDAY_OF_THE_WEEK>( currentTime.GetDayOfWeek() );
			m_progressState.nDrawingTime =
				currentTime.GetHour()*3600 +
				currentTime.GetMinute()*60 +
				currentTime.GetSecond();

			// 추첨 종료 시간이 지나면;
			if( m_endTime.nDrawingTime <= m_progressState.nDrawingTime )
			{
				m_bProgressState = false;
				return;
			}

			// 진행상태;
			switch( m_curState )
			{
			case LOTTO_STATE_GATHERING:
			case LOTTO_STATE_BASIC:
			case LOTTO_STATE_WAIT:
				if( static_cast<int>( m_startTime.nDrawingTime )
					- static_cast<int>( m_progressState.nDrawingTime ) < 0 )
				{
					m_progressState.nDrawingTime = 0;
					return;
				}

				m_progressState.nDrawingTime =
					m_startTime.nDrawingTime - m_progressState.nDrawingTime;
				break;
			case LOTTO_STATE_DRAWING:
				if( static_cast<int>( m_progressState.nDrawingTime )
					- static_cast<int>( m_startTime.nDrawingTime ) < 0 )
				{
					m_progressState.nDrawingTime = 0;
					return;
				}

				m_progressState.nDrawingTime =
					m_progressState.nDrawingTime - m_startTime.nDrawingTime;
				break;
			}

			// 진행상태 갱신;
			AddEvent( EUPDATE_PROGRESS_TIME );
		}
	}

	// 당첨 번호 출력;
	void CLottoSystemClient::UpdateWinNum()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		ReqPreWinNum( m_curTurnNum );
	}

	void CLottoSystemClient::ReqPreWinNum( unsigned int turnNum )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	// 당첨자 출력;
	void CLottoSystemClient::UpdateWinManList()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		ReqPreWinManList( m_curTurnNum );
	}

	void CLottoSystemClient::ReqPreWinManList( unsigned int turnNum )
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		// 새로운 당첨자 리스트를 받아와야 하기 때문에 먼저 초기화한다;
		m_winnerManList.Clear();

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	void CLottoSystemClient::ConfirmBuyList()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		m_confirmBuyList.buyList.clear();
		m_confirmWinnerNum.Clear();

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF msg( m_id, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToFieldServer( &msg );
	}

	bool CLottoSystemClient::CompareLottoNum( unsigned int a, unsigned int b )
	{
		return a < b;
	}

	// 특정 Index의 구매목록 가져오기;
	const LOTTO_SYSTEM_BUY CLottoSystemClient::GetBuyInfo( unsigned int idx ) const
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return LOTTO_SYSTEM_BUY();

		for( unsigned int i=0; i<m_buyList.buyList.size(); ++i )
		{
			LOTTO_SYSTEM_BUY buyInfo = m_buyList.buyList.at( i );
			if( i == idx )
			{
				// 로또 추첨 번호 정렬 해야하면;
				if( systemValue.bUseSort )
				{
					std::vector<unsigned int> vTempDrawingNum;
					for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
					{
						if( j >= 0 && j < systemValue.nDrawingMain )
							vTempDrawingNum.push_back( buyInfo.drawingNum[ j ] );
					}

					sort( vTempDrawingNum.begin(), vTempDrawingNum.end(), CompareLottoNum );

					for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
					{
						if( j >= 0 && j < systemValue.nDrawingMain )
							buyInfo.drawingNum[ j ] = vTempDrawingNum.at( j );
					}
				}

				return buyInfo;
			}
		}

		return LOTTO_SYSTEM_BUY();
	}

	const LOTTO_SYSTEM_BUY CLottoSystemClient::GetConfirmBuyInfo( unsigned int idx ) const
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return LOTTO_SYSTEM_BUY();

		for( unsigned int i=0; i<m_confirmBuyList.buyList.size(); ++i )
		{
			/* 당첨 되었는지 몇등인지 어느 번호가 맞는지;
			 * 확인한다;
			 * 확인하는 위치가 애매해서 찾기가 힘들다;
			 * 좀 더 적절한 위치를 찾을 필요가 있다; */
			LOTTO_SYSTEM_BUY buyInfo = m_confirmBuyList.buyList.at( i );

			// 로또 추첨 번호 정렬 해야하면;
			if( systemValue.bUseSort )
			{
				std::vector<unsigned int> vTempDrawingNum;
				for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
				{
					if( j >= 0 && j < systemValue.nDrawingMain )
						vTempDrawingNum.push_back( buyInfo.drawingNum[ j ] );
				}

				sort( vTempDrawingNum.begin(), vTempDrawingNum.end(), CompareLottoNum );

				for( unsigned int j=0; j<LottoSystem::MAX_DRAWING_NUM_LIST; ++j )
				{
					if( j >= 0 && j < systemValue.nDrawingMain )
						buyInfo.drawingNum[ j ] = vTempDrawingNum.at( j );
				}
			}

			int mainNumCount = 0;
			int bonusNumCount = 0;
			m_pDrawingMachine->ConfirmWinningNum(
				buyInfo.drawingNum,
				m_confirmWinnerNum.lottoNum,
				buyInfo.drawingNumState,
				mainNumCount,
				bonusNumCount );

			if( i == idx )
				return buyInfo;
		}

		return LOTTO_SYSTEM_BUY();
	}

	const LOTTO_WIN_TYPE CLottoSystemClient::GetLottoWinType1st ( ) const
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if ( !systemValue.bUse )
			return LOTTO_WIN_TYPE();

		return systemValue.sLottoWinType1st;
	}

}

