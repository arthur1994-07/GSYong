#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../Network/NetLogicDefine.h"

#include <boost/tokenizer.hpp>

namespace LottoSystem
{
	// Lotto System의 각 게임 상태;
	enum EMLOTTO_STATE
	{
		LOTTO_STATE_BASIC,						// 기본 구입 가능 상태;
		LOTTO_STATE_WAIT,						// 추첨 대기 상태;
		LOTTO_STATE_DRAWING,					// 추첨 상태;
		LOTTO_STATE_GATHERING,					// 집계 상태;
	};

	// 로또의 화폐 타입;
	enum EMLOTTO_MONEY_TYPE
	{
		LOTTO_MONEY_TYPE_GAME_MONEY,			// 게임 머니;
		LOTTO_MONEY_TYPE_POINT					// 포인트;
	};

	// 요일;
	enum EMDAY_OF_THE_WEEK
	{
		DAY_OF_THE_WEEK_NONE,
		SUNDAY,									// 일요일;
		MONDAY,									// 월요일;
		TUESDAY,								// 화요일;
		WEDNESDAY,								// 수요일;
		THURSDAY,								// 목요일;
		FRIDAY,									// 금요일;
		SATURDAY,								// 토요일;
		DAY_OF_THE_WEEK_SIZE					// 사이즈;
	};

	// 돈 지급 방법;
	enum EMGIVE_MONEY_TYPE
	{
		ACCUMULATE_MONEY,						// 누적 금액;
		FIXED_MONEY								// 고정 금액;
	};

	// 로또 구매 타입;
	enum EMLOTTO_BUY_TYPE
	{
		NONE_BUY_TYPE,
		AUTO_BUY,								// 자동 구매;
		MANUAL_BUY								// 수동 구매;
	};

	/// 시스템에 큰 변화가 생길때 상수값을 변경한다;
	/// 리뉴얼 수준의 개편이 아니라면 바뀌지 않을 것으로 보인다;
	// 로또 시스템 ID;
	typedef unsigned int LOTTO_SYSTEM_ID;
	const int LOTTO_NONE_ID = -1;
	const int LOTTO_MIN_ID = 0;
	const int LOTTO_MAX_ID = 1;

	// 상수;
	const int MAX_LOTTO_SYSTEM = 2;				// 최대 로또 시스템 개수;
	const int MAX_DRAWING_NUM_LIST = 7;			// 최대 로또 번호 개수;
	const int MAX_RANKING_NUM = 5;				// 최대 랭킹;

	/// Tokenizer
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
}