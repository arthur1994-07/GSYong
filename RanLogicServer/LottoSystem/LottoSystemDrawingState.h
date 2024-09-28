#pragma once

#include "./LottoSystemState.h"

namespace LottoSystem
{

	class CLottoSystemDrawingState : public CLottoSystemState
	{
	public:
		CLottoSystemDrawingState( LOTTO_SYSTEM_ID lottoSystemId, CLottoSystem* pLottoSystem );
		virtual ~CLottoSystemDrawingState();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float elapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		virtual void Update_ChangeState();

		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum );

	private:
		// 로또 구매 요청;
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	private:
		// 패킷 처리;
		virtual void ChangeStateMsgProc();

	private:
		// 로또 추첨 번호 Broadcast;
		void BroadcastDrawingNum();

		// 추첨 끝 ( 당첨자 확인 및 당첨금 지급 );
		void ExitDrawingState();

	private:
		float m_fAllDrawingTime;			// 전체 추첨 시간;
		float m_fEachOtherDrawingTime;		// 각각의 번호 추첨 시간;

		unsigned int m_curDrawingCount;		// 현재 추첨 번호의 개수;
	};

}