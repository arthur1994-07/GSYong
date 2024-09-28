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

		// ��÷�� �ʱ�ȭ;
		m_pDrawingMachine->Initialize(
			systemValue.nLottoTotal,
			systemValue.nDrawingTotal,
			systemValue.nDrawingMain,
			systemValue.nDrawingBonus );

		// Ŭ���̾�Ʈ ���� ������ AddCount �ʱ�ȭ ����;
		m_pDrawingMachine->InitializeNumCount();

		// ���Ÿ�� �ʱ�ȭ;
		m_buyList.buyList.clear();

		// ��÷Ȯ�� ��� ����;
		m_confirmBuyList.buyList.clear();

		// ��÷�� ��� �ʱ�ȭ;
		m_winnerManList.Clear();

		// �ý��� �ʱ�ȭ;
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

		// �ð������̹Ƿ� �� �����Ӹ��� ���������� ���Ž�Ų��;
		UpdateProgressTime( currentTime );

		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return false;

		// Ŭ���̾�Ʈ�� ĳ���� ������ ����;
		// �ݾ��� �������Ӹ��� ���������� ���Ž�Ų��;
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

		// ��� ���谡 Ȯ���ϱ� ������ static_cast�� �����Ѵ�;
		// dynamic_cast�� �� ���� ������ �ӵ��� �� �� �߽��ߴ�;
		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC:
			NextDrawingDataProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC:			// ���� ����;
			CurrentStateProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CURRENT_STATE_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC:			// ���� ��;
			CurrentTurnProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CURRENT_TURN_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC:
			GiveMoneyPostProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC:		// ���� ��û ���;
			ReqLottoToBuyProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC:		// ���� ��÷�� ��û ���;
			AccumulateMoneyProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC:				// ���� ����Ʈ ��û ���;
			BuyListProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC:
			BuyListCountProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC:				// ��÷ ��ȣ ��û ���;
			WinningNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC:		// ��÷�� ����Ʈ ��û ���;
			WinningManListProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC*>( pMsg ) );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB:			// ���� ���� ��Ŷ;
			ChangeStateProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_AB*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB:			// ȸ�� ���� ��Ŷ;
			GoNextTurnProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB:				// ��÷ ��ȣ ��Ŷ;
			DrawingNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_DRAWING_NUM_AB*>( pMsg ) );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC:					// ����ڵ��� ������� ������ ���� ��ȣ�� ��;
			NumDSPProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC:						// �ζ� ��÷ ���� �� ��ȣ�� ��ȣ�� ��� ��� �ִ��� Ȯ��;
			NumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC:				// �ζ��� �� ��ȣ�� �ϳ������� �ִ� ���·� �ʱ�ȭ;
			NumInitProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC:					// Ư�� ��ȣ�� ������ �߰�;
			NumAddProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_AC*>( pMsg ) );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC:					// Ư�� ��ȣ�� ������ ����;
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

	// �ڵ� ���� ( Test���̶�� ���� �ȴ� );
	// ���� �ڵ� ���� ����� �ʿ��� �� ����ص� �������;
	CLottoSystemClient::BUY_LOTTO_FLAG CLottoSystemClient::BuyLottoAuto()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return BUY_LOTTO_FLAG_FAIL;

		if( m_bLockBuyState )
			return BUY_LOTTO_FLAG_LOCK_FAIL;

		// ���� Ŭ���̾�Ʈ�� ĳ���� DB num�� ���Ѵ�;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		// ĳ���Ͱ� �ڰ��� �Ǵ��� Ȯ���Ѵ�;
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

		// �ζ� ��ȣ�� �����;
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
				// ��÷�Ⱑ ����� �������� ���� ��쿡�� ���Ը޽����� ������ �ʴ´�;
				m_pDrawingMachine->ClearDrawingMachine();
				return BUY_LOTTO_FLAG_FAIL;
			}

			msg.m_lottoBuy.drawingNumState[ i ] = false;
		}
		msg.m_lottoBuy.ranking = 0;

		// ��÷�� ����;
		m_pDrawingMachine->ClearDrawingMachine();

		// ��Ŷ�� ������;
		GLLottoSystemManClient::Instance()->SendToFieldServer( &msg );

		return BUY_LOTTO_FLAG_OK;
	}

	// ����;
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

		// ���� Ŭ���̾�Ʈ�� ĳ���� DB num�� ���Ѵ�;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		// ĳ���Ͱ� �ڰ��� �Ǵ��� Ȯ���Ѵ�;
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( !pChar )
			return BUY_LOTTO_FLAG_FAIL;

		// ���� �˻�;
		unsigned int level = pChar->GetLevel();
		if( systemValue.buyLevel > level )
			return BUY_LOTTO_FLAG_LEVEL_FAIL;

		// ������ �˻�;
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

		// �ζ� ��ȣ �˻�;
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

		// �ζ� ��ȣ�� ������;
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

			// ��Ŷ�� ������;
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

		// ��÷�� Ȯ��;
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

		// ��÷�� ����;
		m_pDrawingMachine->ClearDrawingMachine();

		return lottoNum;
	}

	// ���� ����Ʈ ����;
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

		// ���ο� ���� ����Ʈ�� �޾ƿ;� �ϱ� ������ ���� �ʱ�ȭ�Ѵ�;
		if( m_curTurnNum == turnNum )
			m_buyList.buyList.clear();
		else
			m_confirmBuyList.buyList.clear();

		// ���� Ŭ���̾�Ʈ�� ĳ���� DB num�� ���Ѵ�;
		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	// ���� �ݾ� ����;
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

		// ���� Ŭ���̾�Ʈ�� ĳ���� DB num�� ���Ѵ�;
		GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA msg( m_id, turnNum, dwDbNum );
		GLLottoSystemManClient::Instance()->SendToAgentServer( &msg );
	}

	// ��÷ �Ͻ� ���;
	void CLottoSystemClient::UpdateDrawingDate()
	{
		LOTTO_SYSTEM_CLIENT_VALUE systemValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !systemValue.bUse )
			return;

		// ��÷ �Ͻ� ����;
		AddEvent( EUPDATE_DRAWING_DATE );
	}

	// ���� ���� ����;
	void CLottoSystemClient::UpdateProgressTime( const CTime& currentTime )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( m_id );

		if( !sValue.bUse )
			return;

		// ��÷ ��¥���;
		if( currentTime.GetDay() == m_startTime.sTime.GetDay() )
		{
			m_bProgressState = true;

			m_progressState.emDrawingDayOfTheWeek =
				static_cast<EMDAY_OF_THE_WEEK>( currentTime.GetDayOfWeek() );
			m_progressState.nDrawingTime =
				currentTime.GetHour()*3600 +
				currentTime.GetMinute()*60 +
				currentTime.GetSecond();

			// ��÷ ���� �ð��� ������;
			if( m_endTime.nDrawingTime <= m_progressState.nDrawingTime )
			{
				m_bProgressState = false;
				return;
			}

			// �������;
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

			// ������� ����;
			AddEvent( EUPDATE_PROGRESS_TIME );
		}
	}

	// ��÷ ��ȣ ���;
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

	// ��÷�� ���;
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

		// ���ο� ��÷�� ����Ʈ�� �޾ƿ;� �ϱ� ������ ���� �ʱ�ȭ�Ѵ�;
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

	// Ư�� Index�� ���Ÿ�� ��������;
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
				// �ζ� ��÷ ��ȣ ���� �ؾ��ϸ�;
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
			/* ��÷ �Ǿ����� ������� ��� ��ȣ�� �´���;
			 * Ȯ���Ѵ�;
			 * Ȯ���ϴ� ��ġ�� �ָ��ؼ� ã�Ⱑ �����;
			 * �� �� ������ ��ġ�� ã�� �ʿ䰡 �ִ�; */
			LOTTO_SYSTEM_BUY buyInfo = m_confirmBuyList.buyList.at( i );

			// �ζ� ��÷ ��ȣ ���� �ؾ��ϸ�;
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

