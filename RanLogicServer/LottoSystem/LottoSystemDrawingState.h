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
		// �ζ� ���� ��û;
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	private:
		// ��Ŷ ó��;
		virtual void ChangeStateMsgProc();

	private:
		// �ζ� ��÷ ��ȣ Broadcast;
		void BroadcastDrawingNum();

		// ��÷ �� ( ��÷�� Ȯ�� �� ��÷�� ���� );
		void ExitDrawingState();

	private:
		float m_fAllDrawingTime;			// ��ü ��÷ �ð�;
		float m_fEachOtherDrawingTime;		// ������ ��ȣ ��÷ �ð�;

		unsigned int m_curDrawingCount;		// ���� ��÷ ��ȣ�� ����;
	};

}