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
	## 로또 시스템 관리 매니저 Agent;
	1. GLAgentServer 와 연동되는 Manager;

	2. GLAgentServerLottoSystem.cpp 파일에 GLAgentServer와 이어지는;
	모든 인터페이스 함수가 정의되어 있다;
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

		// Agent 서버로 오는 패킷을 처리함;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Client와의 동기화;
		void SyncState( DWORD dwClientID, DWORD dwChaNum );

		// 로또 시스템의 상태를 변경한다;
		bool ChangeLottoSystemState( unsigned int lottoSystemId, EMLOTTO_STATE state );

		// Agent와 클라이언트와의 통신;
		void SendToClientFromAgent( DWORD dwDbNum, DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// 특정 서버군의 Client로의 통신;
		void SendToServerGroupClientFromAgent ( DWORD dwDbNum, DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Agent와 Field와의 통신;
		void SendToFieldFromAgent( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// 특정 서버군의 Field로의 통신;
		void SendToServerGroupFieldFromAgent ( DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Agent 브로드 캐스트;
		void BroadcastFromAgent( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		/// DBAction;
		void SystemLoad( unsigned int systemId );	// 로또 시스템 DBData 로딩;
		void DrawingMachineNumSelectAction(
			unsigned int lottoNum,
			unsigned int systemId );				// 추첨기 로딩;
		void CurTurnSelectAction(
			unsigned int systemId );				// 현재 턴 알아보기;
		void AccumulateMoneySelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// 특정 턴의 누적 당첨금 알아보기;
		void WinNumSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// 특정 턴의 당첨번호 알아보기;
		void BuyListSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// 특정 턴의 구매리스트 알아보기;
		void BuyListNonConfirmSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int systemId,
			int confirmTicket );					// 특정 턴의 확인되지 않은 구매리스트 알아보기;
		void WinManListSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// 특정 턴의 당첨리스트 정보 알아보기;
		void InventWinNumSelectAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			unsigned int turnNum,
			unsigned int systemId );				// 조작을 위한 최소중복 추첨번호 10개 알아보기;

		void DrawingMachineNumSaveAction(
			unsigned int lottoNum,
			unsigned int accumulateCount,
			unsigned int addCount,
			unsigned int systemId,
			bool bUpdate = true );					// 추첨기 정보 저장;
		void SystemTurnSaveAction(
			unsigned int turnNum,
			unsigned int systemId );				// 턴 저장;

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
			unsigned int systemId );				// 구매;

		void WinManListUpdateAction(
			unsigned int ranking,
			LONGLONG winMoney,
			SNATIVEID sItemID,
			DWORD dwItemCount,
			unsigned int turnNum,
			unsigned int systemId );				// 당첨리스트의 당첨금 및 아이템 수정 ( 당첨금은 모든 당첨자가 조사된 이후에 따로 수정된다 ) ;
		void WinManListInsertAction(
			unsigned int ranking,
			LONGLONG winNumInt,
			LONGLONG bonusNumInt,
			unsigned int systemId );				// 당첨리스트 추가;
		void AccumulateMoneyUpdateAction(
			LONGLONG accumulateMoney,
			unsigned int turnNum,
			unsigned int systemId );				// 누적 당첨금 변경;
		void WinNumUpdateAction(
			const std::string& winNum,
			LONGLONG winNumInt,
			LONGLONG bonusNumInt,
			unsigned int turnNum,
			unsigned int systemId );				// 당첨번호 변경;
		void BuyListNonConfirmUpdateAction(
			DWORD dwDbNum,
			DWORD dwServerGroup,
			LONGLONG invenMoney,
			unsigned int systemId );				// 미확인 구매리스트 확인 ( 확인이후 바로 삭제된다 );

		void SystemNextTurnAction(
			unsigned int systemId );				// 다음 턴으로 이동;

		void ConfirmWinManAction(
			unsigned int systemId );				// 당첨자 확인;

		void ChangeInventWinNumAction(
			unsigned int turnNum,
			unsigned int systemId );				// 당첨번호 조작;

		void WinMoneyUpdateAction(
			unsigned int turnNum,
			unsigned int systemId );				// 당첨금 갱신;

		void SystemBuyLogAction(
			const LOTTO_SYSTEM_BUY_LOG& logInfo );	// 구매 로그;

		void SystemWinManLogAction(
			unsigned int turnNum,
			unsigned int systemId );				// 당첨 로그;

		void SystemTurnInfoLogAction(
			const LOTTO_SYSTEM_TURN_LOG& logInfo );	// 각 턴 로그;

	private:
		// 로또 시스템 추가;
		void AddLottoSystem( LOTTO_SYSTEM_ID id );

	public:
		// Lotto System;
		CLottoSystem* GetLottoSystem( LOTTO_SYSTEM_ID id );

	public:
		// Agent 서버;
		inline void SetAgentServer( GLAgentServer* pServer ) { m_pAgentServer = pServer; }

		inline GLAgentServer* GetAgentServer() { return m_pAgentServer; }

	private:
		// 각 로또 시스템;
		// 로또 시스템은 동시에 하나 이상 진행될 수 있다;
		std::vector<CLottoSystem*> m_pLottoSystems;

		// Agent Server;
		GLAgentServer* m_pAgentServer;
	};

}