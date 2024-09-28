#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/LottoSystem/ILottoSystemMan.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

#include "../Database/DbManager.h"

#include "./LottoSystem.h"

#include "./GLLottoDataTypeServer.h"

class GLAgentServer;

namespace LottoSystem
{

	/*-------------------------------------------------------------------
	## �ζ� �ý��� ���� �Ŵ��� Agent;
	1. GLAgentServer �� �����Ǵ� Manager;

	2. GLAgentServerLottoSystem.cpp ���Ͽ� GLAgentServer�� �̾�����;
	��� �������̽� �Լ��� ���ǵǾ� �ִ�;
	-------------------------------------------------------------------*/
	class GLLottoSystemManAgent : public ILottoSystemMan, public CSingletone<GLLottoSystemManAgent>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoSystemManAgent();
		~GLLottoSystemManAgent();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );

		// Agent ������ ���� ��Ŷ�� ó����;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Client���� ����ȭ;
		void SyncState( DWORD dwClientID, DWORD dwChaNum );

		// �ζ� �ý����� ���¸� �����Ѵ�;
		bool ChangeLottoSystemState( unsigned int lottoSystemId, EMLOTTO_STATE state );

		// Agent�� Ŭ���̾�Ʈ���� ���;
		void SendToClientFromAgent( DWORD dwDbNum, DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Ư�� �������� Client���� ���;
		void SendToServerGroupClientFromAgent ( DWORD dwDbNum, DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Agent�� Field���� ���;
		void SendToFieldFromAgent( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Ư�� �������� Field���� ���;
		void SendToServerGroupFieldFromAgent ( DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Agent ��ε� ĳ��Ʈ;
		void BroadcastFromAgent( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		/// DBAction;
		void SystemLoad( unsigned int systemId );	// �ζ� �ý��� DBData �ε�;
		void DrawingMachineNumSelectAction(
			unsigned int lottoNum,
			unsigned int systemId );				// ��÷�� �ε�;
		void CurTurnSelectAction(
			unsigned int systemId );				// ���� �� �˾ƺ���;
		void AccumulateMoneySelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// Ư�� ���� ���� ��÷�� �˾ƺ���;
		void WinNumSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// Ư�� ���� ��÷��ȣ �˾ƺ���;
		void BuyListSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// Ư�� ���� ���Ÿ���Ʈ �˾ƺ���;
		void BuyListNonConfirmSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int systemId,
			int confirmTicket );					// Ư�� ���� Ȯ�ε��� ���� ���Ÿ���Ʈ �˾ƺ���;
		void WinManListSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// Ư�� ���� ��÷����Ʈ ���� �˾ƺ���;
		void InventWinNumSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// ������ ���� �ּ��ߺ� ��÷��ȣ 10�� �˾ƺ���;

		void DrawingMachineNumSaveAction(
			unsigned int lottoNum,
			unsigned int accumulateCount,
			unsigned int addCount,
			unsigned int systemId,
			bool bUpdate = true );					// ��÷�� ���� ����;
		void SystemTurnSaveAction(
			unsigned int turnNum,
			unsigned int systemId );				// �� ����;

		void BuyListInsertAction(
			LONGLONG accumulateMoney,
			LONGLONG charge,
			int buyType,
			const std::string& drawingNum,
			LONGLONG drawingNumInt,
			int confirmTicket,
			DWORD dwUserNum,
			DWORD dwDbNum,
			DWORD dwServerGroup,
			char* szChaName,
			unsigned int turnNum,
			unsigned int systemId );				// ����;

		void WinManListUpdateAction(
			unsigned int ranking,
			LONGLONG winMoney,
			SNATIVEID sItemID,
			DWORD dwItemCount,
			unsigned int turnNum,
			unsigned int systemId );				// ��÷����Ʈ�� ��÷�� �� ������ ���� ( ��÷���� ��� ��÷�ڰ� ����� ���Ŀ� ���� �����ȴ� ) ;
		void WinManListInsertAction(
			unsigned int ranking,
			LONGLONG winNumInt,
			LONGLONG bonusNumInt,
			unsigned int systemId );				// ��÷����Ʈ �߰�;
		void AccumulateMoneyUpdateAction(
			LONGLONG accumulateMoney,
			unsigned int turnNum,
			unsigned int systemId );				// ���� ��÷�� ����;
		void WinNumUpdateAction(
			const std::string& winNum,
			LONGLONG winNumInt,
			LONGLONG bonusNumInt,
			unsigned int turnNum,
			unsigned int systemId );				// ��÷��ȣ ����;
		void BuyListNonConfirmUpdateAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			LONGLONG invenMoney,
			unsigned int systemId );				// ��Ȯ�� ���Ÿ���Ʈ Ȯ�� ( Ȯ������ �ٷ� �����ȴ� );

		void SystemNextTurnAction(
			unsigned int systemId );				// ���� ������ �̵�;

		void ConfirmWinManAction(
			unsigned int systemId );				// ��÷�� Ȯ��;

		void ChangeInventWinNumAction(
			unsigned int turnNum,
			unsigned int systemId );				// ��÷��ȣ ����;

		void WinMoneyUpdateAction(
			unsigned int turnNum,
			unsigned int systemId );				// ��÷�� ����;

		void SystemBuyLogAction(
			const LOTTO_SYSTEM_BUY_LOG& logInfo );	// ���� �α�;

		void SystemWinManLogAction(
			unsigned int turnNum,
			unsigned int systemId );				// ��÷ �α�;

		void SystemTurnInfoLogAction(
			const LOTTO_SYSTEM_TURN_LOG& logInfo );	// �� �� �α�;

	private:
		// �ζ� �ý��� �߰�;
		void AddLottoSystem( LOTTO_SYSTEM_ID id );

	public:
		// Lotto System;
		CLottoSystem* GetLottoSystem( LOTTO_SYSTEM_ID id );

	public:
		// Agent ����;
		inline void SetAgentServer( GLAgentServer* pServer ) { m_pAgentServer = pServer; }

		inline GLAgentServer* GetAgentServer() { return m_pAgentServer; }

	private:
		// �� �ζ� �ý���;
		// �ζ� �ý����� ���ÿ� �ϳ� �̻� ����� �� �ִ�;
		std::vector<CLottoSystem*> m_pLottoSystems;

		// Agent Server;
		GLAgentServer* m_pAgentServer;
	};

}