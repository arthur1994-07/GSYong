#pragma once

#include <string>
#include <vector>

#include "GLContrlBaseMsg.h"

#include "../LottoSystem/GLLottoDataType.h"

namespace LottoSystem
{
	// 클라이언트 동기화 패킷;
	enum EMLOTTO_REQ_SYNC_STATUS_AC
	{
		EMLOTTO_REQ_SYNC_STATUS_AC_FAIL,			// 일반 오류;
		EMLOTTO_REQ_SYNC_STATUS_AC_OK,				// 성공;
	};

	// 구매 요청;
	enum EMLOTTO_REQ_LOTTO_TO_BUY_AC
	{
		EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL,			// 일반 오류;
		EMLOTTO_REQ_LOTTO_TO_BUY_AC_OK,				// 성공;

		EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR,	// 구매 가능 상태가 아님;
		EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL,		// 레벨 제한;
		EMLOTTO_REQ_LOTTO_TO_BUY_AC_MONEY_FAIL		// 돈 제한;
	};

	// 누적 당첨금;
	enum EMLOTTO_REQ_ACCUMULATE_MONEY_AC
	{
		EMLOTTO_REQ_ACCUMULATE_MONEY_AC_FAIL,		// 일반 오류;
		EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK			// 성공;
	};

	// 구매 목록;
	enum EMLOTTO_REQ_BUY_LIST_AC
	{
		EMLOTTO_REQ_BUY_LIST_AC_FAIL,				// 일반 오류;
		EMLOTTO_REQ_BUY_LIST_AC_OK					// 성공;
	};

	// 당첨 번호;
	enum EMLOTTO_REQ_WINNER_NUM_AC
	{
		EMLOTTO_REQ_WINNER_NUM_AC_FAIL,				// 일반 오류;
		EMLOTTO_REQ_WINNER_NUM_AC_OK				// 성공;
	};

	// 전체 당첨자;
	enum EMLOTTO_REQ_WINNER_MAN_LIST_AC
	{
		EMLOTTO_REQ_WINNER_MAN_LIST_AC_FAIL,		// 일반 오류;
		EMLOTTO_REQ_WINNER_MAN_LIST_AC_OK			// 성공;
	};
}

namespace GLMSG
{

	// Lotto System 의 모든 패킷의 헤더;
	// 사용할 수 있는 패킷이 아니다;
	/*struct NET_LOTTO_MESSAGE_HEADER : public NET_MSG_GENERIC
	{
		LottoSystem::LOTTO_SYSTEM_ID m_lottoSystemId;

		NET_LOTTO_MESSAGE_HEADER( const DWORD nSize
			, LottoSystem::LOTTO_SYSTEM_ID id
			, const EMNET_MSG msg )
			: NET_MSG_GENERIC( msg, nSize )
			, m_lottoSystemId( id )
		{

		}
	};*/

	struct NET_LOTTO_MESSAGE_HEADER : public NET_MSG_GENERIC_RELAY_AGENT
	{
		LottoSystem::LOTTO_SYSTEM_ID m_lottoSystemId;

		NET_LOTTO_MESSAGE_HEADER ( const DWORD _nSize
			, LottoSystem::LOTTO_SYSTEM_ID _sLottoSystemID
			, const EMNET_MSG _emMsg )
			: m_lottoSystemId ( _sLottoSystemID )
		{
			nType = _emMsg;
			dwSize = _nSize;
		}
	};


	/// Agent 자기 자신에게 보내는 메시지;
	struct NET_LOTTO_MESSAGE_SYNC_AGENT_MYSELF : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_SYNC_AGENT_MYSELF ( )
			: NET_LOTTO_MESSAGE_HEADER( sizeof( NET_LOTTO_MESSAGE_SYNC_AGENT_MYSELF )
			, 0
			, NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_AGENT_MYSELF )
		{
		}
	};


	/// Agent Broadcast;
	// 상태 변경;
	struct NET_LOTTO_MESSAGE_CHANGE_STATE_AB : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_STATE m_curState;

		NET_LOTTO_MESSAGE_CHANGE_STATE_AB ( const LottoSystem::LOTTO_SYSTEM_ID _sLottoSystemID
			, const LottoSystem::EMLOTTO_STATE _emLottoState )
			: NET_LOTTO_MESSAGE_HEADER( sizeof( NET_LOTTO_MESSAGE_CHANGE_STATE_AB )
			, _sLottoSystemID
			, NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB )
			, m_curState( _emLottoState )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOALLCLIENT;
		}
	};

	// 회차 변경;
	struct NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_turn;

		NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turn )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB), id, NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB )
			, m_turn( turn )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOALLCLIENT;
		}
	};

	// 추첨 시 번호;
	struct NET_LOTTO_MESSAGE_DRAWING_NUM_AB : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_drawingCount;			// 몇번째 추첨 번호인가?;
		unsigned int m_curDrawingNum;			// 추첨된 번호;

		NET_LOTTO_MESSAGE_DRAWING_NUM_AB( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int drawingCount
			, unsigned int drawingNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_DRAWING_NUM_AB), id, NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB )
			, m_drawingCount( drawingCount )
			, m_curDrawingNum( drawingNum )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOALLCLIENT;
		}
	};

	/// Client -> Field -> Agent
	// 구매 요청;
	struct NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		LottoSystem::LOTTO_SYSTEM_BUY m_lottoBuy;

		NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF), id, NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_CF )
			, m_dwDbNum( dwDbNum )
		{

		}
	};

	// 구매 요청;
	struct NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		LottoSystem::LOTTO_SYSTEM_BUY m_lottoBuy;
		LONGLONG m_invenMoney;
		LONGLONG m_point;
		unsigned int m_level;

		NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, LONGLONG invenMoney
			, LONGLONG point
			, unsigned int level )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA )
			, m_dwDbNum( dwDbNum )
			, m_invenMoney( invenMoney )
			, m_point( point )
			, m_level( level )
		{

		}
	};

	// 확인안된 구매리스트 확인하기;
	struct NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;

		NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_CF )
			, m_dwDbNum( dwDbNum )
		{

		}
	};

	// 확인안된 구매리스트 확인하기;
	struct NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		LONGLONG m_invenMoney;
		LONGLONG m_point;

		NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, LONGLONG invenMoney
			, LONGLONG point )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA )
			, m_dwDbNum( dwDbNum )
			, m_invenMoney( invenMoney )
			, m_point( point )
		{

		}
	};

	/// Client -> Agent
	// 현재 상태 확인;
	/*struct NET_LOTTO_MESSAGE_CURRENT_STATE_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;

		NET_LOTTO_MESSAGE_CURRENT_STATE_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CURRENT_STATE_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA )
		{

		}
	};

	// 현재 회차 확인;
	struct NET_LOTTO_MESSAGE_CURRENT_TURN_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;

		NET_LOTTO_MESSAGE_CURRENT_TURN_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CURRENT_TURN_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA )
		{

		}
	};*/

	// 누적 당첨금;
	struct NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_turnNum;			// 0으로 입력할 경우 현재 턴;

		NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA )
			, m_turnNum( turnNum )
			, m_dwDbNum( dwDbNum )
		{

		}
	};

	// 캐릭터의 구매 목록;
	struct NET_LOTTO_MESSAGE_BUY_LIST_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		//int m_idx;						// 0이하의 수이면 모든 구매 목록 갱신 요청;
		unsigned int m_turnNum;			// 0으로 입력할 경우 현재 턴;

		NET_LOTTO_MESSAGE_BUY_LIST_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, DWORD dwDbNum )
			//, int idx = -1 )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_BUY_LIST_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA )
			, m_turnNum( turnNum )
			, m_dwDbNum( dwDbNum )
			//, m_idx( idx )
		{

		}
	};

	// 회차별 당첨 번호;
	struct NET_LOTTO_MESSAGE_WINNING_NUM_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_turnNum;			// 0으로 입력할 경우 현재 턴;

		NET_LOTTO_MESSAGE_WINNING_NUM_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_WINNING_NUM_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA )
			, m_turnNum( turnNum )
			, m_dwDbNum( dwDbNum )
		{

		}
	};

	// 회차별 전체 당첨자 목록;
	struct NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_turnNum;			// 0으로 입력할 경우 현재 턴;

		NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA )
			, m_turnNum( turnNum )
			, m_dwDbNum( dwDbNum )
		{

		}
	};

	/// Client -> Agent ( GM )
	// 상태 변경;
	struct NET_LOTTO_MESSAGE_CHANGE_STATE_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_CHANGE_STATE_CA( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CHANGE_STATE_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA )
		{

		}
	};

	// 사용자들이 현재까지 선택한 누적 번호의 수;
	struct NET_LOTTO_MESSAGE_NUM_DSP_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_num;

		NET_LOTTO_MESSAGE_NUM_DSP_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int num )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_DSP_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA )
			, m_dwDbNum( dwDbNum )
			, m_num( num )
		{

		}
	};


	// 로또 추첨 시의 각 번호가 번호당 몇개씩 들어 있는지 확인;
	struct NET_LOTTO_MESSAGE_NUM_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_num;

		NET_LOTTO_MESSAGE_NUM_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int num )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA )
			, m_dwDbNum( dwDbNum )
			, m_num( num )
		{

		}
	};


	// 로또의 각 번호를 하나가지고 있는 상태로 초기화;
	struct NET_LOTTO_MESSAGE_NUM_INIT_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;

		NET_LOTTO_MESSAGE_NUM_INIT_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_INIT_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA )
			, m_dwDbNum( dwDbNum )
		{

		}
	};


	// 특정 번호의 개수를 추가;
	struct NET_LOTTO_MESSAGE_NUM_ADD_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_num;
		unsigned int m_count;

		NET_LOTTO_MESSAGE_NUM_ADD_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int num
			, unsigned int count )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_ADD_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA )
			, m_dwDbNum( dwDbNum )
			, m_num( num )
			, m_count( count )
		{

		}
	};


	// 특정 번호의 개수를 삭제;
	struct NET_LOTTO_MESSAGE_NUM_DEL_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_num;
		unsigned int m_count;

		NET_LOTTO_MESSAGE_NUM_DEL_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int num
			, unsigned int count )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_DEL_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA )
			, m_dwDbNum( dwDbNum )
			, m_num( num )
			, m_count( count )
		{

		}
	};

	// 로또 번호 조작;
	struct NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;

		NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_SET_INVENT_WIN_NUM_CA )
		{

		}
	};

	// 로또 번호 조작 적용 초기화;
	struct NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_RESET_INVENT_WIN_NUM_CA )
		{

		}
	};

	// 로또 Lock & Unlock;
	struct NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		bool m_bBuyLock;

		NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, bool bBuyLock )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_CA )
			, m_bBuyLock( bBuyLock )
		{

		}
	};

	// 로또 턴 변경 Lock;
	struct NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA : public NET_LOTTO_MESSAGE_HEADER
	{
		bool m_bChangeTurnLock;

		NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA( LottoSystem::LOTTO_SYSTEM_ID id
			, bool bChangeTurnLock )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA), id, NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_CA )
			, m_bChangeTurnLock( bChangeTurnLock )
		{

		}
	};

	/// Agent -> Field
	struct NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_turnNum;

		LONGLONG m_winMoney;
		LottoSystem::EMLOTTO_MONEY_TYPE m_moneyType;
		unsigned int m_ranking;

		SNATIVEID m_sWinItem;
		DWORD m_dwWinItemCount;

		NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int turnNum
			, LONGLONG winMoney
			, unsigned int ranking )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF), id, NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_AF )
			, m_dwDbNum( dwDbNum )
			, m_turnNum( turnNum )
			, m_winMoney( winMoney )
			, m_ranking( ranking )
			, m_moneyType( LottoSystem::LOTTO_MONEY_TYPE_GAME_MONEY )
			, m_sWinItem( SNATIVEID( false ) )
			, m_dwWinItemCount( 0 )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOFIELD;

			dwChaNum = dwDbNum;
		}
	};

	struct NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC : public NET_LOTTO_MESSAGE_HEADER
	{
		DWORD m_dwDbNum;
		unsigned int m_turnNum;

		LONGLONG m_winMoney;
		LottoSystem::EMLOTTO_MONEY_TYPE m_moneyType;
		unsigned int m_ranking;

		NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum
			, unsigned int turnNum
			, LONGLONG winMoney
			, unsigned int ranking )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF), id, NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC )
			, m_dwDbNum( dwDbNum )
			, m_turnNum( turnNum )
			, m_winMoney( winMoney )
			, m_ranking( ranking )
			, m_moneyType( LottoSystem::LOTTO_MONEY_TYPE_GAME_MONEY )
		{
		}
	};

	/// Agent -> Field -> Client
	// 구매 요청;
	struct NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC m_emAC;
		DWORD m_dwDbNum;

		LottoSystem::EMLOTTO_MONEY_TYPE m_moneyType;
		LONGLONG m_lottoTicketMoney;

		LottoSystem::LOTTO_SYSTEM_BUY m_lottoBuy;

		NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF( LottoSystem::LOTTO_SYSTEM_ID id
			, DWORD dwDbNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF), id, NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_AF )
			, m_emAC( LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL )
			, m_dwDbNum( dwDbNum )
			, m_moneyType( LottoSystem::LOTTO_MONEY_TYPE_GAME_MONEY )
			, m_lottoTicketMoney( 0 )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOFIELD;

			dwChaNum = dwDbNum;
		}
	};

	// 구매 요청;
	struct NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC m_emAC;

		LottoSystem::LOTTO_SYSTEM_BUY m_lottoBuy;

		NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL )
		{

		}
	};


	/// Agent -> Client
	// 클라이언트 동기화 패킷 Global Value;
	struct NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC m_emAC;
		LottoSystem::LOTTO_GLOBAL_VALUE m_golbalValue;

		NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_GLOBAL_VALUE_AC )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 클라이언트 동기화 패킷 System Value;
	struct NET_LOTTO_MESSAGE_SYNC_STATUS_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC m_emAC;
		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE m_systemValue;				// Lotto System Value;

		NET_LOTTO_MESSAGE_SYNC_STATUS_AC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_SYNC_STATUS_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_FAIL )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 다음 추첨 일자 정보 패킷;
	struct NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::LOTTO_DRAWING_DATE m_startTime;	// 시작 시간;
		LottoSystem::LOTTO_DRAWING_DATE m_endTime;		// 종료 시간;

		NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, LottoSystem::LOTTO_DRAWING_DATE startTime
			, LottoSystem::LOTTO_DRAWING_DATE endTime )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC )
			, m_startTime( startTime )
			, m_endTime( endTime )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 현재 상태;
	struct NET_LOTTO_MESSAGE_CURRENT_STATE_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_STATE m_curState;

		NET_LOTTO_MESSAGE_CURRENT_STATE_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, LottoSystem::EMLOTTO_STATE curState )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CURRENT_STATE_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC )
			, m_curState( curState )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	struct NET_LOTTO_MESSAGE_CURRENT_TURN_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_turn;

		NET_LOTTO_MESSAGE_CURRENT_TURN_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turn )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_CURRENT_TURN_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC )
			, m_turn( turn )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 회차별 누적 당첨금;
	struct NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC m_emAC;
		unsigned int m_turnNum;
		LONGLONG m_accumulateMoney;

		NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, LONGLONG accumulateMoney )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC_FAIL )
			, m_turnNum( turnNum )
			, m_accumulateMoney( accumulateMoney )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 캐릭터의 구매 목록;
	// 하나씩 보내지만 개수 정보는 취급하지 않는다;
	// 패킷이 손실 될 경우 개수가 덜 표시되는 문제가 발생 할 가능성이 있다;
	struct NET_LOTTO_MESSAGE_BUY_LIST_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_BUY_LIST_AC m_emAC;
		unsigned int m_turnNum;
		LottoSystem::LOTTO_SYSTEM_BUY m_lotto;

		NET_LOTTO_MESSAGE_BUY_LIST_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_BUY_LIST_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_FAIL )
			, m_turnNum( turnNum )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	struct NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_turnNum;
		unsigned int m_count;

		NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum
			, unsigned int count )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC )
			, m_turnNum( turnNum )
			, m_count( count )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 회차별 당첨 번호;
	struct NET_LOTTO_MESSAGE_WINNING_NUM_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_WINNER_NUM_AC m_emAC;
		unsigned int m_turnNum;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;
		unsigned m_winnerNumCount;

		NET_LOTTO_MESSAGE_WINNING_NUM_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_WINNING_NUM_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_WINNER_NUM_AC_FAIL )
			, m_turnNum( turnNum )
			, m_winnerNumCount( 0 )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 회차별 전체 당첨자 목록;
	// 하나씩 보내지만 개수 정보는 취급하지 않는다;
	// 패킷이 손실 될 경우 개수가 덜 표시되는 문제가 발생 할 가능성이 있다;
	struct NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		LottoSystem::EMLOTTO_REQ_WINNER_MAN_LIST_AC m_emAC;
		unsigned int m_turnNum;
		LottoSystem::LOTTO_SYSTEM_WINNER_MAN m_winnerMan;

		NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int turnNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC )
			, m_emAC( LottoSystem::EMLOTTO_REQ_WINNER_MAN_LIST_AC_FAIL )
			, m_turnNum( turnNum )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	/// Agent -> Client ( GM )
	// 사용자들이 현재까지 선택한 누적 번호의 수;
	struct NET_LOTTO_MESSAGE_NUM_DSP_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_num;
		unsigned int m_drawingCount;

		NET_LOTTO_MESSAGE_NUM_DSP_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int num
			, unsigned int drawingCount )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_DSP_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC )
			, m_num( num )
			, m_drawingCount( drawingCount )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};


	// 로또 추첨 시의 각 번호가 번호당 몇개씩 들어 있는지 확인;
	struct NET_LOTTO_MESSAGE_NUM_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_num;
		unsigned int m_addCount;

		NET_LOTTO_MESSAGE_NUM_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int num
			, unsigned int addCount )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC )
			, m_num( num )
			, m_addCount( addCount )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};


	// 로또의 각 번호를 하나가지고 있는 상태로 초기화;
	struct NET_LOTTO_MESSAGE_NUM_INIT_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_NUM_INIT_AC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_INIT_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};


	// 특정 번호의 개수를 추가;
	struct NET_LOTTO_MESSAGE_NUM_ADD_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_NUM_ADD_AC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_ADD_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};


	// 특정 번호의 개수를 삭제;
	struct NET_LOTTO_MESSAGE_NUM_DEL_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		NET_LOTTO_MESSAGE_NUM_DEL_AC( LottoSystem::LOTTO_SYSTEM_ID id )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_NUM_DEL_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 조작 가능할 때 DSP;
	struct NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		unsigned int m_count;								// 중복한 개수;
		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM m_winnerNum;	// 번호;

		NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, unsigned int count
			, LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winnerNum )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_INVENT_NUM_DSP_AC )
			, m_count( count )
			, m_winnerNum( winnerNum )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 로또 Lock & Unlock;
	struct NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		bool m_bBuyLock;

		NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, bool bBuyLock )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_AC )
			, m_bBuyLock( bBuyLock )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};

	// 로또 턴 변경 Lock;
	struct NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC : public NET_LOTTO_MESSAGE_HEADER
	{
		bool m_bChangeTurnLock;

		NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC( LottoSystem::LOTTO_SYSTEM_ID id
			, bool bChangeTurnLock )
			: NET_LOTTO_MESSAGE_HEADER( sizeof(NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC), id, NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_AC )
			, m_bChangeTurnLock( bChangeTurnLock )
		{
			emRelayType = EMMSG_RELAY_TYPE_AGENTTOCLIENT;
		}
	};
}