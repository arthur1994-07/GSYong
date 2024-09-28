#pragma once

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	class ILottoSystemState;
	class CLottoSystemBasicState;
	class CLottoSystemWaitState;
	class CLottoSystemDrawingState;

	class CLottoDrawingMachine;

	// 하나의 로또 시스템을 표현한다;
	class CLottoSystem
	{
	public:
		CLottoSystem( LOTTO_SYSTEM_ID id );
		~CLottoSystem();

	public:
		void Initialize();
		void Destroy();

		bool Update( float fElapsedTime, const CTime& currentTime );
		bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		// DB를 Load해서 Server와 DB를 동기화한다;
		void LoadFromDB();

	public:
		// 상태 변경;
		bool ChangeState( EMLOTTO_STATE state );

		// 다음 턴으로 넘어감;
		void GoNextTurn();

		// Client와의 동기화;
		void SyncState( DWORD dwClientID, DWORD dwChaNum );
		void BroadcastSyncSystemValue();

	private:
		// 상태 변경 Broadcast;
		void BroadcastChangeState();

		// 다음 Turn으로 넘어감 Broadcast;
		void BroadcastGoNextTurn();

	public:
		/// DB에도 적용되는 로직;
		// 누적 금액 누적;
		void AccumulateMoney( LONGLONG deltaMoney );

		// 구매 리스트에 구매 로또 추가;
		bool AddBuyList( LOTTO_SYSTEM_BUY& buyLotto,
			LONGLONG accumulateMoney,
			LONGLONG charge );

		// 당첨 번호 갱신;
		void UpdateWinNum( std::vector<unsigned int>& winNum );
		void UpdatePreWinNum( std::vector<unsigned int>& winNum );

		// 당첨 번호 리스트;
		void GenerateWinNumList();

		// 랭킹 정보 갱신;
		void UpdateRankingInfo();

		// 추첨기 정보 갱신;
		void InitializeDrawingMachine();
		void UpdateDrawingMachine();

	public:
		/// 랭킹 정보 추가;
		void AddRankingInfo( const LOTTO_SYSTEM_WINNER_RANKING_INFO& rankingInfo );
		void ClearRankingInfo();

		// 이월 금액 계산;
		LONGLONG CalculateNextMoney();

	private:
		void ClearWinNumList();
		void AddTempWinNum( const LOTTO_WIN_NUM_INT& tempWinNum );
		LONGLONG MakeNumInt( std::vector<int>& numState, bool bMain = true );

	public:
		// 추첨 시간 구하기;
		void MakeDrawingDate();

	public:
		void SetWinType( const std::string& strWinTypes );

	public:
		// Getter;
		inline const EMLOTTO_MONEY_TYPE& GetMoneyType() const { return m_moneyType; }
		inline const int GetTotalNum() const { return m_nTotalNum; }
		inline const int GetDrawingTotalNum() const { return m_nDrawingTotalNum; }
		inline const int GetDrawingMainNum() const { return m_nDrawingMainNum; }
		inline const int GetDrawingBonusNum() const { return m_nDrawingBonusNum; }
		inline const LONGLONG GetLottoTicketMoney() const { return m_llLottoTicketMoney; }
		inline const int GetTicketFee() const { return m_ticketFee; }
		inline const unsigned int GetWinTypeCount() const { return m_vecWinType.size(); }
		inline const LOTTO_WIN_TYPE& GetWinType( unsigned int idx ) { return m_vecWinType.at( idx ); }
		inline const LOTTO_DRAWING_DATE& GetStartTime() const { return m_startTime; }
		inline const LOTTO_DRAWING_DATE& GetEndTime() const { return m_endTime; }
		inline const LOTTO_DRAWING_DATE& GetPreStartTime() const { return m_preStartTime; }
		inline const LOTTO_DRAWING_DATE& GetPreEndTime() const { return m_preEndTime; }
		inline const EMLOTTO_STATE GetLottoSystemState() const { return m_curState; }
		inline const LOTTO_SYSTEM_ID GetLottoSystemId() const { return m_id; }
		inline const LONGLONG GetAccumulateMoney() const { return m_accumulateMoney; }
		inline LOTTO_SYSTEM_LOTTO_NUM GetPreWinnerNum() const { return m_preWinnerNum; }
		inline LOTTO_SYSTEM_LOTTO_NUM GetWinnerNum() const { return m_winnerNum; }
		inline LOTTO_SYSTEM_WINNER_MAN_LIST GetWinnerManList() const { return m_winnerManList; }
		inline const unsigned int GetCurTurnNum() const { return m_turnNum; }
		inline std::vector<LOTTO_SYSTEM_WINNER_RANKING_INFO> GetLottoRankingInfoList() const { return m_rankingInfos; }
		inline CLottoDrawingMachine* GetDrawingMachine() { return m_pDrawingMachine; }
		inline std::vector<LOTTO_WIN_NUM_INT> GetWinNumList() { return m_winNumList; }
		inline bool GetInventLottoNumState() { return m_bInventLottoNum; }
		inline bool GetBuyLockState() { return m_bBuyLock; }
		inline bool GetChangeTurnLockState() { return m_bChangeTurnLock; }

		// Setter( 말 그대로 Setter );
		// Setter가 너무 많다;
		// DB와 Logic간의 동기화를 위해 큐를 만들어서 처리하는게 더 좋을듯;
		inline void SetMoneyType( EMLOTTO_MONEY_TYPE moneyType ) { m_moneyType = moneyType; }
		inline void SetTotalNum( int nTotalNum ) { m_nTotalNum = nTotalNum; }
		inline void SetDrawingTotalNum( int nDrawingTotalNum ) { m_nDrawingTotalNum = nDrawingTotalNum; }
		inline void SetDrawingMainNum( int nDrawingMainNum ) { m_nDrawingMainNum = nDrawingMainNum; }
		inline void SetDrawingBonusNum( int nDrawingBonusNum ) { m_nDrawingBonusNum = nDrawingBonusNum; }
		inline void SetLottoTicketMoney( LONGLONG llLottoTicketMoney ) { m_llLottoTicketMoney = llLottoTicketMoney; }
		inline void SetTicketFee( int ticketFee ) { m_ticketFee = ticketFee; }
		inline void SetCurTurnNum( unsigned int turnNum ) { m_turnNum = turnNum; }
		inline void SetAccumulateMoney( LONGLONG money ) { m_accumulateMoney = money; }
		inline void SetPreWinNum( LOTTO_SYSTEM_LOTTO_NUM winNum ) { m_preWinnerNum = winNum; }
		inline void SetCurWinNum( LOTTO_SYSTEM_LOTTO_NUM winNum ) { m_winnerNum = winNum; } 
		inline void SetWinnerLotto( const LOTTO_SYSTEM_WINNER_MAN& winLotto ) { m_winnerManList.winnerManList.push_back( winLotto ); }
		inline void SetInventLottoState( bool bInventLottoNum ) { m_bInventLottoNum = bInventLottoNum; }
		inline void SetBuyLockState( bool bBuyLock ) { m_bBuyLock = bBuyLock; }
		inline void SetChangeTurnLockState( bool bChangeTurnLock ) { m_bChangeTurnLock = bChangeTurnLock; }

	private:
		// Lotto System 현재 턴의 정보;
		EMLOTTO_MONEY_TYPE m_moneyType;
		int m_nTotalNum;
		int m_nDrawingTotalNum;
		int m_nDrawingMainNum;
		int m_nDrawingBonusNum;

		LONGLONG m_llLottoTicketMoney;
		int m_ticketFee;
		std::vector<LOTTO_WIN_TYPE> m_vecWinType;

		// 추첨 시간 정보;
		LOTTO_DRAWING_DATE m_startTime;
		LOTTO_DRAWING_DATE m_endTime;

		LOTTO_DRAWING_DATE m_preStartTime;
		LOTTO_DRAWING_DATE m_preEndTime;

		// 당첨 티켓 지급 로그 시간;
		float m_fGiveWinTicketTime;

		// 누적 당첨금 갱신 시간;
		float m_fUpdateAccumulateMoneyTime;

		// Buy Lock;
		bool m_bBuyLock;

		// Change Turn Lock;
		bool m_bChangeTurnLock;

		// 로또 시스템의 ID;
		LOTTO_SYSTEM_ID m_id;

		// 현재 상태;
		ILottoSystemState* m_pCurState;
		EMLOTTO_STATE m_curState;

		// 각 상태;
		CLottoSystemBasicState* m_pBasicState;
		CLottoSystemWaitState* m_pWaitState;
		CLottoSystemDrawingState* m_pDrawingState;

		// 로또 추첨기;
		CLottoDrawingMachine* m_pDrawingMachine;

		// 현재 로또번호를 임의로 조작했는가 여부;
		// ( 조작 가능 타입에서만 사용된다 );
		// 서버가 꺼지면 초기화된다;
		bool m_bInventLottoNum;

		// 당첨자 랭킹 별 정보 ( 당첨 시 마다 달라지는 유동 Data );
		std::vector<LOTTO_SYSTEM_WINNER_RANKING_INFO> m_rankingInfos;

		/// DB에서 가져 오는 Value;
		// 현재 누적 금액;
		LONGLONG m_accumulateMoney;

		// 현재 회차;
		unsigned int m_turnNum;

		// 이전 회차 당첨 번호;
		LOTTO_SYSTEM_LOTTO_NUM m_preWinnerNum;

		// 현재 당첨 번호;
		LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;

		// 현재 당첨자 리스트;
		LOTTO_SYSTEM_WINNER_MAN_LIST m_winnerManList;

		// 당첨번호 리스트;
		std::vector<LOTTO_WIN_NUM_INT> m_winNumList;
	};

}