#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../RanLogic/LottoSystem/ILottoSystemMan.h"

#include "./LottoSystemClient.h"

#include "../GLGaeaClient.h"

namespace LottoSystem
{

	// Lotto System의 Client 관리자;
	// Lotto System 들을 관리한다;
	// Lotto System 에서 사용하는 기능들을 제공한다;
	class GLLottoSystemManClient : public ILottoSystemMan, public CSingletone<GLLottoSystemManClient>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoSystemManClient();
		~GLLottoSystemManClient();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Agent Server와의 통신;
		void SendToAgentServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field Server와의 통신;
		void SendToFieldServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		const int GetConfirmBuyListCount();
		CLottoSystemClient* GetLottoSystemUsingTabIdx( unsigned int useIdx );

	public:
		// GM 명령어;
		bool GMCommandSetLottoSystemID( unsigned int lottoSystemId );
		bool GMCommandViewLottoSystemID();
		bool GMCommandChangeState();
		bool GMCommandNumDSP( unsigned int num = 0 );
		bool GMCommandNum( unsigned int num = 0 );
		bool GMCommandNumInit();
		bool GMCommandNumAdd( unsigned int num, unsigned int count );
		bool GMCommandNumDel( unsigned int num, unsigned int count );

		bool GMCommandViewDrawingType();
		bool GMCommandSetWinNum( const std::string& strWinNum );
		bool GMCommandResetWinNum();

		bool GMCommandCantBuy();
		bool GMCommandCanBuy();
		bool GMCommandChangeTurnOn();
		bool GMCommandChangeTurnOff();

		// GM 명령어 ( Develop Mode );
		bool GMCommandBuyTicket();
		bool GMCommandAccumulateMoney();
		bool GMCommandViewBuyList();
		bool GMCommandViewDrawingDate();
		bool GMCommandViewWinNum();
		bool GMCommandViewWinManList();

	public:
		// Gaea Client의 기능을 활용하기 위하여 세팅해준다;
		inline void SetGaeaClient( GLGaeaClient* pGaeaClient ) { m_pGaeaClient = pGaeaClient; }

		// Getter;
		// GM 명령어가 적용될 Lotto System의 ID;
		inline const unsigned int GetGmCommandLottoSystemId() const { return m_gmCommandLottoSystemId; }

		inline const unsigned int GetLottoSystemCount() const { return m_pLottoSystems.size(); }
		inline CLottoSystemClient* GetLottoSystem( LOTTO_SYSTEM_ID id )
		{
			if( id >= 0 && id < m_pLottoSystems.size() )
				return m_pLottoSystems.at( id );

			return NULL;
		}

		/// Client의 자체 로그를 남기기 위해 임시 사용;
		/// Log 이외의 기능으로는 사용하지 말자;
		/// UI쪽에서 뿌릴수도 있으나 이미 있는 코드를 활용했다;
		inline void ConsoleLog( const std::string& log ) { m_pGaeaClient->PrintConsoleText( log ); }

	private :
		CLottoSystemClient* FindSystem( unsigned int nID );

	public:
		// 로또 이전 회차 리스트
		LuaTable GetPrevTurnList( unsigned int nID );
		// 특정 회차 당첨 정보
		LuaTable GetPrevTurnWinnerList( unsigned int nID );
		// 현재 회차 누적금액
		LONGLONG GetAccumulateMoney( unsigned int nID );
		// 현재 진행중인 회차		
		int GetCurTurnNum( unsigned int nID );
		// 로또 시작시간
		LuaTable GetStartTime( unsigned int nID );
		// 최대 구매 개수
		int GetBuyCountMax( unsigned int nID );
		// Total Num;
		int GetTotalNum ( unsigned int nID );
		// 구매 갯수
		int GetBuyCount( unsigned int nID, bool bConfirm );
		// 추첨이 진행중인지
		bool GetLockBuyState( unsigned int nID );
		// 로또 구매 비용
		LONGLONG GetBuyCost( unsigned int nID );
		// 로또 오토 번호 
		LuaTable GetAutoNum( unsigned int nID );
		//현재 회차의 나의 구매리스트
		LuaTable GetBuyList( unsigned int nID, bool bConfirm );
		//로또 확인하지 않은 회차 당첨 번호 
		LuaTable GetConfirmNum( unsigned int nID );

	public:
		void ReqPreWinNum( unsigned int nID, unsigned int nTurnNum );
		void ReqPreWinManList( unsigned int nID, unsigned int nTurnNum );
		void ReqPreAccumulateMoney( unsigned int nID, unsigned int nTurnNum );
		CLottoSystemClient::BUY_LOTTO_FLAG ReqBuyLotto( unsigned int nID, LuaTable tbNum, bool bAuto );
		CLottoSystemClient::BUY_LOTTO_FLAG ReqBuyLottoList ( unsigned int nID, unsigned int nCount );
		void ReqPreBuyList( unsigned int nID, unsigned int nTurnNum );
		void ReqConfirmBuyList( unsigned int nID );

		int GetLottoSystemState ( unsigned int nID );
		int GetViewNumCharName ();

	private:
		// 각 Lotto System;
		std::vector<CLottoSystemClient*> m_pLottoSystems;

		// Gaea Client;
		GLGaeaClient* m_pGaeaClient;

		// GMCommand를 위한 Lotto System ID;
		unsigned int m_gmCommandLottoSystemId;
	};

}