#include "pch.h"

#include "./LottoSystemWaitState.h"

#include "./GLLottoSystemManAgent.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	CLottoSystemWaitState::CLottoSystemWaitState( LOTTO_SYSTEM_ID lottoSystemId
		, CLottoSystem* pLottoSystem )
		: CLottoSystemState( lottoSystemId, pLottoSystem )
		, m_fAllWaitTime( 0.f ) { }
	CLottoSystemWaitState::~CLottoSystemWaitState() { }

	void CLottoSystemWaitState::Initialize()
	{
		CLottoSystemState::Initialize();
	}

	void CLottoSystemWaitState::Destroy()
	{
		CLottoSystemState::Destroy();
	}

	bool CLottoSystemWaitState::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !CLottoSystemState::Update( fElapsedTime, currentTime ) )
			return false;

		// 시스템의 Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		// 시스템의 대기 시간;
		float fWaitTime = static_cast<float>( systemValue.nDrawingReadyTime );

		m_fAllWaitTime += ( fElapsedTime );

		// 대기 시간이 지나가면;
		if( m_fAllWaitTime >= fWaitTime )
		{
			return GLLottoSystemManAgent::Instance()->
				ChangeLottoSystemState( m_lottoSystemId, LOTTO_STATE_DRAWING );
		}

		return true;
	}

	bool CLottoSystemWaitState::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !CLottoSystemState::OnMessage( pMsg, dwClientID, dwGaeaID ) )
			return false;

		return true;
	}

	void CLottoSystemWaitState::Update_ChangeState()
	{
		m_fAllWaitTime = 0.f;

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( m_lottoSystemId );
		if( pLottoSystem )
		{
			pLottoSystem->ClearRankingInfo();

			// 당첨자 리스트, 랭킹 정보, 당첨금 갱신;
			unsigned int turnNum = pLottoSystem->GetCurTurnNum();

			GLLottoSystemManAgent::Instance()->WinMoneyUpdateAction(
				turnNum, m_lottoSystemId );
		}
	}

	void CLottoSystemWaitState::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{

	}

	void CLottoSystemWaitState::SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		CLottoSystemState::SendReqLottoToBuyAC( pMsg, dwClientID, dwGaeaID );

		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA*>( pMsg );

		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		// 응답 패킷;
		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF(
			m_lottoSystemId, dwDbNum );

		// 대기 상태에서는 구매가 불가능 하다;
		lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR;

		// 응답;
		GLLottoSystemManAgent::Instance()->SendToFieldFromAgent( dwClientID, &lottoMsgAF );
	}

	void CLottoSystemWaitState::SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_CA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_CA*>( pMsg );

		DWORD dwDbNum = pLottoMsg->m_dwDbNum;
		unsigned int turnNum = pLottoMsg->m_turnNum;

		DWORD dwServerGroup = 0;
		GLAgentServer* pServer = GLLottoSystemManAgent::Instance()->GetAgentServer();
		if ( pServer )
		{
			if ( pServer->IsAgentClientSlot( dwClientID ) )
				dwServerGroup = pServer->GetServerGroup_ClientSlotID( dwClientID );
			else
				dwServerGroup = pServer->GetServerGroup();
		}

		// 응답 패킷;
		if( m_pLottoSystem->GetCurTurnNum() == turnNum )
		{
			GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
				m_lottoSystemId, turnNum );
			lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
			lottoMsgAC.m_winnerNum = m_pLottoSystem->GetWinnerNum();

			// 응답;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &lottoMsgAC );
		}
		else
		{
			// 다른 회차의 경우 DbAction을 한다;
			GLLottoSystemManAgent::Instance()->WinNumSelectAction(
				dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
		}
	}

	void CLottoSystemWaitState::ChangeStateMsgProc()
	{
		GLLottoSystemManAgent::Instance()->ChangeLottoSystemState( m_lottoSystemId, LOTTO_STATE_DRAWING );
	}

}