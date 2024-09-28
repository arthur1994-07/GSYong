#pragma once

#include "./GLLottoDataTypeClient.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

#include "../EventBox/EventBox.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

namespace LottoSystem
{
	class CLottoDrawingMachine;

	// Client에서 각 Lotto System을 의미하는 Class;
	class CLottoSystemClient : public EventBox::CEventBox
	{
	public:
		// 구매 Flag;
		enum BUY_LOTTO_FLAG
		{
			BUY_LOTTO_FLAG_OK,
			BUY_LOTTO_FLAG_FAIL,				// 일반 실패;
			BUY_LOTTO_FLAG_LOCK_FAIL,			// 구매 잠금 상태로 인한 실패;
			BUY_LOTTO_FLAG_LEVEL_FAIL,			// 레벨 부족;
			BUY_LOTTO_FLAG_MONEY_FAIL,			// 구입 비용 없음;
			BUY_LOTTO_FLAG_DUPLICATE_FAIL,		// 중복 번호;
			BUY_LOTTO_FLAG_NUM_FAIL,			// 범위 밖의 숫자;
			BUY_LOTTO_FLAG_ZERO_FAIL			// 숫자에 0이 들어옴;
		};

	public:
		CLottoSystemClient( LOTTO_SYSTEM_ID id
			, GLGaeaClient* pGaeaClient );
		~CLottoSystemClient();

	public:
		void Initialize();
		void Destroy();

		bool Update( float fElapsedTime, const CTime& currentTime );
		bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// 서버에서 오는 패킷에 대한 처리;
		void SyncStatusProc( GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC* pSyncMsg );
		void NextDrawingDataProc( GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC* pNextDrawingDataMsg );
		void CurrentStateProc( GLMSG::NET_LOTTO_MESSAGE_CURRENT_STATE_AC* pCurStateMsg );
		void CurrentTurnProc( GLMSG::NET_LOTTO_MESSAGE_CURRENT_TURN_AC* pCurTurnMsg );
		void GiveMoneyPostProc( GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC* pPostMsg );
		void ReqLottoToBuyProc( GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC* pBuyMsg );
		void AccumulateMoneyProc( GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC* pAccumulateMsg );
		void BuyListProc( GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC* pBuyListMsg );
		void BuyListCountProc( GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC* pBuyListMsg );
		void WinningNumProc( GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC* pWinningNumMsg );
		void WinningManListProc( GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC* pWinningManListMsg );

		void ChangeStateProc( GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_AB* pMsg );
		void GoNextTurnProc( GLMSG::NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB* pGoNextTurnMsg );
		void DrawingNumProc( GLMSG::NET_LOTTO_MESSAGE_DRAWING_NUM_AB* pDrawingNumMsg );

		void NumDSPProc( GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC* pDspMsg );
		void NumProc( GLMSG::NET_LOTTO_MESSAGE_NUM_AC* pNumMsg );
		void NumInitProc( GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_AC* pNumInitMsg );
		void NumAddProc( GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_AC* pNumAddMsg );
		void NumDelProc( GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_AC* pNumDelMsg );
		void InventNumDSPProc( GLMSG::NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC* pDspMsg );
		void LockBuyProc( GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC* pLockBuyMsg );
		void LockChangeTurnProc( GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC* pChangeTurnMsg );

	public:
		// 로또 구매;
		BUY_LOTTO_FLAG BuyLottoAuto();						// 자동 구매 ( GM 용 );
		BUY_LOTTO_FLAG BuyLottoTicket(
			LOTTO_SYSTEM_LOTTO_NUM lottoNum,
			EMLOTTO_BUY_TYPE lottoBuyType,
			bool bBuy = true );								// 로또 구매 or 로또 구매 조건 확인;
		LOTTO_SYSTEM_LOTTO_NUM MakeAutoLottoNum();			// 로또 번호 생성;

		// 구매 리스트 갱신;
		void UpdateBuyList();								// 전체 리스트 갱신;
		void UpdateConfirmBuyList();
		void ReqPreBuyList( unsigned int turnNum );

		// 누적 금액 갱신;
		void UpdateAccumulateMoney();
		void ReqPreAccumulateMoney( unsigned int turnNum );

		// 추첨 일시 갱신;
		void UpdateDrawingDate();

		// 진행 상태 갱신;
		void UpdateProgressTime( const CTime& currentTime );

		// 당첨 번호 출력;
		void UpdateWinNum();
		void ReqPreWinNum( unsigned int turnNum );

		// 당첨자 출력;
		void UpdateWinManList();
		void ReqPreWinManList( unsigned int turnNum );

		// 구매리스트 확인하기;
		void ConfirmBuyList();

	public:
		// 로또 번호 정렬을 위해 사용;
		static bool CompareLottoNum( unsigned int a, unsigned int b );

	public:
		// 실제 구매 리스트;
		const LOTTO_SYSTEM_BUY GetBuyInfo( unsigned int idx ) const;

		// 이전 턴의 확인하지 않은 구매 리스트;
		const LOTTO_SYSTEM_BUY GetConfirmBuyInfo( unsigned int idx ) const;

		// 1등의 보상정보;
		const LOTTO_WIN_TYPE GetLottoWinType1st ( ) const;

	public:
		// 구입가능한지
		inline const EMLOTTO_STATE GetLottoSystemState() const { return m_curState; }
		//추첨시작시간
		inline const LOTTO_DRAWING_DATE& GetStartTime() const { return m_startTime; }
		//추첨 끝난시간
		inline const LOTTO_DRAWING_DATE& GetEndTime() const { return m_endTime; }
		//추첨진행되며 흐른시간
		inline const LOTTO_DRAWING_DATE& GetCurProgressTime() const { return m_progressState; }
		// 추첨 진행중인지
		inline bool GetCurProgressState() const { return m_bProgressState; }
		//로또 시스템ID
		inline const LOTTO_SYSTEM_ID GetLottoSystemId() const { return m_id; }
		//현재 진행되는 회차
		inline const unsigned int GetCurTurnNum() const { return m_curTurnNum; }
		//소유 금액
		inline const LONGLONG GetHaveMoney() const { return m_haveMoney; }
		
		//현재회차 구매내역
		inline const LOTTO_SYSTEM_BUY_LIST& GetBuyList() const { return m_buyList; }
		//현재회차 구매 갯수
		inline const unsigned int GetBuyCount() const { return m_buyList.buyList.size(); }
		//현재회차 누적 금액
		inline const LONGLONG GetAccumulateMoney() const { return m_accumulateMoney; }
		//당첨 번호
		inline const LOTTO_SYSTEM_LOTTO_NUM GetWinnerNum() const { return m_winnerNum; }
		//현재회차 당첨자수
		inline const int GetWinnerNumCount() const { return m_winnerNumCount; }

		//확인하지 않은 회차
		inline const unsigned int GetConfirmTurnNum() const { return m_confirmTurnNum; }
		// 확인하지 않은 구매리스트
		inline const LOTTO_SYSTEM_BUY_LIST GetConfirmBuyList() const { return m_confirmBuyList; }
		// 확인하지 않은 구매리스트 수
		inline const unsigned int GetConfirmBuyCount() const { return m_confirmBuyList.buyList.size(); }
		// 확인하지 않은 리스트 당첨번호
		inline const LOTTO_SYSTEM_LOTTO_NUM GetConfirmWinnerNum() const { return m_confirmWinnerNum; }

		// 당첨자 리스트
		inline const LOTTO_SYSTEM_WINNER_MAN_LIST& GetWinnerManList() const { return m_winnerManList; }
		// 누적금액
		inline const LONGLONG GetTurnAccumulateMoney() const { return m_turnAccumulateMoney; }
		// 당첨자 번호
		inline const LOTTO_SYSTEM_LOTTO_NUM GetTurnWinnerNum() const { return m_turnWinnerNum; }

		//구매잠금상태
		inline const bool GetLockBuyState() const { return m_bLockBuyState; }
		inline const bool GetLockChangeTurnState() const { return m_bLockChangeTurn; }

	private:
		GLGaeaClient* m_pGaeaClient;

		// Buy Lock;
		bool m_bLockBuyState;

		// Change Turn Lock;
		bool m_bLockChangeTurn;

		// 로또 시스템의 ID;
		LOTTO_SYSTEM_ID m_id;

		// 추첨기;
		CLottoDrawingMachine* m_pDrawingMachine;

		// 현재 상태;
		EMLOTTO_STATE m_curState;

		// 추첨 시간;
		LOTTO_DRAWING_DATE m_startTime;		// 시작 시간;
		LOTTO_DRAWING_DATE m_endTime;		// 종료 시간;

		// 현재 진행 시간;
		LOTTO_DRAWING_DATE m_progressState;
		bool m_bProgressState;

		// 현재 캐릭터의 소지금 정보 ( 캐릭터 정보를 이용해 갱신함 );
		LONGLONG m_haveMoney;

		// 현재 회차;
		unsigned int m_curTurnNum;

		// 현재 회차의 구매 정보;
		LOTTO_SYSTEM_BUY_LIST m_buyList;
		LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;
		int m_winnerNumCount;
		LONGLONG m_accumulateMoney;

		// 당첨 확인 구매 정보 ( 확인하지 않은 구매리스트 );
		unsigned int m_confirmTurnNum;
		LOTTO_SYSTEM_BUY_LIST m_confirmBuyList;
		LOTTO_SYSTEM_LOTTO_NUM m_confirmWinnerNum;

		// 특정 회차의 당첨자 정보;
		LOTTO_SYSTEM_WINNER_MAN_LIST m_winnerManList;
		LOTTO_SYSTEM_LOTTO_NUM m_turnWinnerNum;
		LONGLONG m_turnAccumulateMoney;
	};

}
