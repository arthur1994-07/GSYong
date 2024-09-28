#pragma once

#include "./GLLottoDataTypeClient.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

#include "../EventBox/EventBox.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

namespace LottoSystem
{
	class CLottoDrawingMachine;

	// Client���� �� Lotto System�� �ǹ��ϴ� Class;
	class CLottoSystemClient : public EventBox::CEventBox
	{
	public:
		// ���� Flag;
		enum BUY_LOTTO_FLAG
		{
			BUY_LOTTO_FLAG_OK,
			BUY_LOTTO_FLAG_FAIL,				// �Ϲ� ����;
			BUY_LOTTO_FLAG_LOCK_FAIL,			// ���� ��� ���·� ���� ����;
			BUY_LOTTO_FLAG_LEVEL_FAIL,			// ���� ����;
			BUY_LOTTO_FLAG_MONEY_FAIL,			// ���� ��� ����;
			BUY_LOTTO_FLAG_DUPLICATE_FAIL,		// �ߺ� ��ȣ;
			BUY_LOTTO_FLAG_NUM_FAIL,			// ���� ���� ����;
			BUY_LOTTO_FLAG_ZERO_FAIL			// ���ڿ� 0�� ����;
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
		// �������� ���� ��Ŷ�� ���� ó��;
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
		// �ζ� ����;
		BUY_LOTTO_FLAG BuyLottoAuto();						// �ڵ� ���� ( GM �� );
		BUY_LOTTO_FLAG BuyLottoTicket(
			LOTTO_SYSTEM_LOTTO_NUM lottoNum,
			EMLOTTO_BUY_TYPE lottoBuyType,
			bool bBuy = true );								// �ζ� ���� or �ζ� ���� ���� Ȯ��;
		LOTTO_SYSTEM_LOTTO_NUM MakeAutoLottoNum();			// �ζ� ��ȣ ����;

		// ���� ����Ʈ ����;
		void UpdateBuyList();								// ��ü ����Ʈ ����;
		void UpdateConfirmBuyList();
		void ReqPreBuyList( unsigned int turnNum );

		// ���� �ݾ� ����;
		void UpdateAccumulateMoney();
		void ReqPreAccumulateMoney( unsigned int turnNum );

		// ��÷ �Ͻ� ����;
		void UpdateDrawingDate();

		// ���� ���� ����;
		void UpdateProgressTime( const CTime& currentTime );

		// ��÷ ��ȣ ���;
		void UpdateWinNum();
		void ReqPreWinNum( unsigned int turnNum );

		// ��÷�� ���;
		void UpdateWinManList();
		void ReqPreWinManList( unsigned int turnNum );

		// ���Ÿ���Ʈ Ȯ���ϱ�;
		void ConfirmBuyList();

	public:
		// �ζ� ��ȣ ������ ���� ���;
		static bool CompareLottoNum( unsigned int a, unsigned int b );

	public:
		// ���� ���� ����Ʈ;
		const LOTTO_SYSTEM_BUY GetBuyInfo( unsigned int idx ) const;

		// ���� ���� Ȯ������ ���� ���� ����Ʈ;
		const LOTTO_SYSTEM_BUY GetConfirmBuyInfo( unsigned int idx ) const;

		// 1���� ��������;
		const LOTTO_WIN_TYPE GetLottoWinType1st ( ) const;

	public:
		// ���԰�������
		inline const EMLOTTO_STATE GetLottoSystemState() const { return m_curState; }
		//��÷���۽ð�
		inline const LOTTO_DRAWING_DATE& GetStartTime() const { return m_startTime; }
		//��÷ �����ð�
		inline const LOTTO_DRAWING_DATE& GetEndTime() const { return m_endTime; }
		//��÷����Ǹ� �帥�ð�
		inline const LOTTO_DRAWING_DATE& GetCurProgressTime() const { return m_progressState; }
		// ��÷ ����������
		inline bool GetCurProgressState() const { return m_bProgressState; }
		//�ζ� �ý���ID
		inline const LOTTO_SYSTEM_ID GetLottoSystemId() const { return m_id; }
		//���� ����Ǵ� ȸ��
		inline const unsigned int GetCurTurnNum() const { return m_curTurnNum; }
		//���� �ݾ�
		inline const LONGLONG GetHaveMoney() const { return m_haveMoney; }
		
		//����ȸ�� ���ų���
		inline const LOTTO_SYSTEM_BUY_LIST& GetBuyList() const { return m_buyList; }
		//����ȸ�� ���� ����
		inline const unsigned int GetBuyCount() const { return m_buyList.buyList.size(); }
		//����ȸ�� ���� �ݾ�
		inline const LONGLONG GetAccumulateMoney() const { return m_accumulateMoney; }
		//��÷ ��ȣ
		inline const LOTTO_SYSTEM_LOTTO_NUM GetWinnerNum() const { return m_winnerNum; }
		//����ȸ�� ��÷�ڼ�
		inline const int GetWinnerNumCount() const { return m_winnerNumCount; }

		//Ȯ������ ���� ȸ��
		inline const unsigned int GetConfirmTurnNum() const { return m_confirmTurnNum; }
		// Ȯ������ ���� ���Ÿ���Ʈ
		inline const LOTTO_SYSTEM_BUY_LIST GetConfirmBuyList() const { return m_confirmBuyList; }
		// Ȯ������ ���� ���Ÿ���Ʈ ��
		inline const unsigned int GetConfirmBuyCount() const { return m_confirmBuyList.buyList.size(); }
		// Ȯ������ ���� ����Ʈ ��÷��ȣ
		inline const LOTTO_SYSTEM_LOTTO_NUM GetConfirmWinnerNum() const { return m_confirmWinnerNum; }

		// ��÷�� ����Ʈ
		inline const LOTTO_SYSTEM_WINNER_MAN_LIST& GetWinnerManList() const { return m_winnerManList; }
		// �����ݾ�
		inline const LONGLONG GetTurnAccumulateMoney() const { return m_turnAccumulateMoney; }
		// ��÷�� ��ȣ
		inline const LOTTO_SYSTEM_LOTTO_NUM GetTurnWinnerNum() const { return m_turnWinnerNum; }

		//������ݻ���
		inline const bool GetLockBuyState() const { return m_bLockBuyState; }
		inline const bool GetLockChangeTurnState() const { return m_bLockChangeTurn; }

	private:
		GLGaeaClient* m_pGaeaClient;

		// Buy Lock;
		bool m_bLockBuyState;

		// Change Turn Lock;
		bool m_bLockChangeTurn;

		// �ζ� �ý����� ID;
		LOTTO_SYSTEM_ID m_id;

		// ��÷��;
		CLottoDrawingMachine* m_pDrawingMachine;

		// ���� ����;
		EMLOTTO_STATE m_curState;

		// ��÷ �ð�;
		LOTTO_DRAWING_DATE m_startTime;		// ���� �ð�;
		LOTTO_DRAWING_DATE m_endTime;		// ���� �ð�;

		// ���� ���� �ð�;
		LOTTO_DRAWING_DATE m_progressState;
		bool m_bProgressState;

		// ���� ĳ������ ������ ���� ( ĳ���� ������ �̿��� ������ );
		LONGLONG m_haveMoney;

		// ���� ȸ��;
		unsigned int m_curTurnNum;

		// ���� ȸ���� ���� ����;
		LOTTO_SYSTEM_BUY_LIST m_buyList;
		LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;
		int m_winnerNumCount;
		LONGLONG m_accumulateMoney;

		// ��÷ Ȯ�� ���� ���� ( Ȯ������ ���� ���Ÿ���Ʈ );
		unsigned int m_confirmTurnNum;
		LOTTO_SYSTEM_BUY_LIST m_confirmBuyList;
		LOTTO_SYSTEM_LOTTO_NUM m_confirmWinnerNum;

		// Ư�� ȸ���� ��÷�� ����;
		LOTTO_SYSTEM_WINNER_MAN_LIST m_winnerManList;
		LOTTO_SYSTEM_LOTTO_NUM m_turnWinnerNum;
		LONGLONG m_turnAccumulateMoney;
	};

}
