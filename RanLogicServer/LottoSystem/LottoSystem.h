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

	// �ϳ��� �ζ� �ý����� ǥ���Ѵ�;
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

		// DB�� Load�ؼ� Server�� DB�� ����ȭ�Ѵ�;
		void LoadFromDB();

	public:
		// ���� ����;
		bool ChangeState( EMLOTTO_STATE state );

		// ���� ������ �Ѿ;
		void GoNextTurn();

		// Client���� ����ȭ;
		void SyncState( DWORD dwClientID, DWORD dwChaNum );
		void BroadcastSyncSystemValue();

	private:
		// ���� ���� Broadcast;
		void BroadcastChangeState();

		// ���� Turn���� �Ѿ Broadcast;
		void BroadcastGoNextTurn();

	public:
		/// DB���� ����Ǵ� ����;
		// ���� �ݾ� ����;
		void AccumulateMoney( LONGLONG deltaMoney );

		// ���� ����Ʈ�� ���� �ζ� �߰�;
		bool AddBuyList( LOTTO_SYSTEM_BUY& buyLotto,
			LONGLONG accumulateMoney,
			LONGLONG charge );

		// ��÷ ��ȣ ����;
		void UpdateWinNum( std::vector<unsigned int>& winNum );
		void UpdatePreWinNum( std::vector<unsigned int>& winNum );

		// ��÷ ��ȣ ����Ʈ;
		void GenerateWinNumList();

		// ��ŷ ���� ����;
		void UpdateRankingInfo();

		// ��÷�� ���� ����;
		void InitializeDrawingMachine();
		void UpdateDrawingMachine();

	public:
		/// ��ŷ ���� �߰�;
		void AddRankingInfo( const LOTTO_SYSTEM_WINNER_RANKING_INFO& rankingInfo );
		void ClearRankingInfo();

		// �̿� �ݾ� ���;
		LONGLONG CalculateNextMoney();

	private:
		void ClearWinNumList();
		void AddTempWinNum( const LOTTO_WIN_NUM_INT& tempWinNum );
		LONGLONG MakeNumInt( std::vector<int>& numState, bool bMain = true );

	public:
		// ��÷ �ð� ���ϱ�;
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

		// Setter( �� �״�� Setter );
		// Setter�� �ʹ� ����;
		// DB�� Logic���� ����ȭ�� ���� ť�� ���� ó���ϴ°� �� ������;
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
		// Lotto System ���� ���� ����;
		EMLOTTO_MONEY_TYPE m_moneyType;
		int m_nTotalNum;
		int m_nDrawingTotalNum;
		int m_nDrawingMainNum;
		int m_nDrawingBonusNum;

		LONGLONG m_llLottoTicketMoney;
		int m_ticketFee;
		std::vector<LOTTO_WIN_TYPE> m_vecWinType;

		// ��÷ �ð� ����;
		LOTTO_DRAWING_DATE m_startTime;
		LOTTO_DRAWING_DATE m_endTime;

		LOTTO_DRAWING_DATE m_preStartTime;
		LOTTO_DRAWING_DATE m_preEndTime;

		// ��÷ Ƽ�� ���� �α� �ð�;
		float m_fGiveWinTicketTime;

		// ���� ��÷�� ���� �ð�;
		float m_fUpdateAccumulateMoneyTime;

		// Buy Lock;
		bool m_bBuyLock;

		// Change Turn Lock;
		bool m_bChangeTurnLock;

		// �ζ� �ý����� ID;
		LOTTO_SYSTEM_ID m_id;

		// ���� ����;
		ILottoSystemState* m_pCurState;
		EMLOTTO_STATE m_curState;

		// �� ����;
		CLottoSystemBasicState* m_pBasicState;
		CLottoSystemWaitState* m_pWaitState;
		CLottoSystemDrawingState* m_pDrawingState;

		// �ζ� ��÷��;
		CLottoDrawingMachine* m_pDrawingMachine;

		// ���� �ζǹ�ȣ�� ���Ƿ� �����ߴ°� ����;
		// ( ���� ���� Ÿ�Կ����� ���ȴ� );
		// ������ ������ �ʱ�ȭ�ȴ�;
		bool m_bInventLottoNum;

		// ��÷�� ��ŷ �� ���� ( ��÷ �� ���� �޶����� ���� Data );
		std::vector<LOTTO_SYSTEM_WINNER_RANKING_INFO> m_rankingInfos;

		/// DB���� ���� ���� Value;
		// ���� ���� �ݾ�;
		LONGLONG m_accumulateMoney;

		// ���� ȸ��;
		unsigned int m_turnNum;

		// ���� ȸ�� ��÷ ��ȣ;
		LOTTO_SYSTEM_LOTTO_NUM m_preWinnerNum;

		// ���� ��÷ ��ȣ;
		LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;

		// ���� ��÷�� ����Ʈ;
		LOTTO_SYSTEM_WINNER_MAN_LIST m_winnerManList;

		// ��÷��ȣ ����Ʈ;
		std::vector<LOTTO_WIN_NUM_INT> m_winNumList;
	};

}