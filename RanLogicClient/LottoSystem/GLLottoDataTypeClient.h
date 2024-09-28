#pragma once

#include "../../RanLogicServer/LottoSystem/GLLottoDataTypeServer.h"

// Log 보고싶으면 주석 제거;
#ifndef NDEBUG
//#define _EVENT_BOX_MESSAGE_LOG
#endif

namespace LottoSystem
{

	// 현재 처리되어야 하는 이벤트;
	// 큐를 이용해 외부에서 꺼내서 관련된 행동을 수행할 수 있도록 한다;
	enum EVENT_BOX
	{
		NONE,							// 이벤트가 발생되지 않음;
		EUPDATE_INITIALIZE_SYSTEM,		// 시스템 초기화;
		EUPDATE_TURN,					// 회차 갱신;
		EUPDATE_DRAWING_DATE,			// 추첨일 전체 갱신;
		EUPDATE_PROGRESS_TIME,			// 진행상태 시간 갱신;
		EUPDATE_CURRENT_STATE,			// 현재 상태 갱신;
		EUPDATE_CHANGE_STATE,			// 현재상태 변경 갱신;
		EUPDATE_DRAWING_NUM,			// 추첨 번호 갱신;
		EUPDATE_BUY_LIST,				// 구매 리스트 갱신;
		EUPDATE_BUY_LIST_COUNT,			// 구매 리스트 개수 갱신;
		EUPDATE_CONFIRM_BUY_LIST,		// 미확인 구매 리스트 갱신;
		EUPDATE_ACCUMULATE_MONEY,		// 누적 당첨금 갱신;
		EUPDATE_TURN_ACCUMULATE_MONEY,	// 특정 누적 당첨금 갱신;
		EUPDATE_WIN_NUM,				// 당첨 번호 갱신;
		EUPDATE_CONFIRM_TURN_WIN_NUM,	// 당첨 확인 당첨 번호 갱신;
		EUPDATE_TURN_WIN_NUM,			// 특정 당첨 번호 갱신;
		EUPDATE_WIN_MAN_LIST,			// 특정 당첨자 정보 갱신;

		EUPDATE_NUM_DSP,				// Num DSP 갱신;
		EUPDATE_NUM,					// Num 누적 횟수 얻기;
		EUPDATE_NUM_INIT,				// Num 초기화;
		EUPDATE_NUM_ADD,				// Num 추가;
		EUPDATE_NUM_DEL,				// Num 제거;

		EVIEW_RANDOM_TYPE,				// 랜덤 타입;
		EVIEW_INVENT_TYPE,				// 조작 타입;
		EVIEW_CORRECT_SET_WIN_NUM,		// 올바른 당첨번호 조작;
		EVIEW_INCORRECT_SET_WIN_NUM,	// 잘못된 당첨번호 조작;
		EVIEW_POST_MESSAGE,				// 우편 뷰;
		EVIEW_NUM_DSP,					// Num DSP;
		EVIEW_LOCK_BUY,					// Lock Buy;
		EVIEW_LOCK_CHANGE_TURN,			// Lock Change Turn;

		EUPDATE_HAVE_MONEY				// 소지금 갱신;
	};

	// 로또 구매 리스트;
	struct LOTTO_SYSTEM_BUY_LIST
	{
		std::vector<LOTTO_SYSTEM_BUY> buyList;	// 구매 리스트;

		LOTTO_SYSTEM_BUY_LIST() { }

		LOTTO_SYSTEM_BUY_LIST( const LOTTO_SYSTEM_BUY_LIST& me )
		{
			for( unsigned int i=0; i<me.buyList.size(); ++i )
				buyList.push_back( me.buyList.at( i ) );
		}
	};

}