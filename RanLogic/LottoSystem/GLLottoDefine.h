#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../Network/NetLogicDefine.h"

#include <boost/tokenizer.hpp>

namespace LottoSystem
{
	// Lotto System�� �� ���� ����;
	enum EMLOTTO_STATE
	{
		LOTTO_STATE_BASIC,						// �⺻ ���� ���� ����;
		LOTTO_STATE_WAIT,						// ��÷ ��� ����;
		LOTTO_STATE_DRAWING,					// ��÷ ����;
		LOTTO_STATE_GATHERING,					// ���� ����;
	};

	// �ζ��� ȭ�� Ÿ��;
	enum EMLOTTO_MONEY_TYPE
	{
		LOTTO_MONEY_TYPE_GAME_MONEY,			// ���� �Ӵ�;
		LOTTO_MONEY_TYPE_POINT					// ����Ʈ;
	};

	// ����;
	enum EMDAY_OF_THE_WEEK
	{
		DAY_OF_THE_WEEK_NONE,
		SUNDAY,									// �Ͽ���;
		MONDAY,									// ������;
		TUESDAY,								// ȭ����;
		WEDNESDAY,								// ������;
		THURSDAY,								// �����;
		FRIDAY,									// �ݿ���;
		SATURDAY,								// �����;
		DAY_OF_THE_WEEK_SIZE					// ������;
	};

	// �� ���� ���;
	enum EMGIVE_MONEY_TYPE
	{
		ACCUMULATE_MONEY,						// ���� �ݾ�;
		FIXED_MONEY								// ���� �ݾ�;
	};

	// �ζ� ���� Ÿ��;
	enum EMLOTTO_BUY_TYPE
	{
		NONE_BUY_TYPE,
		AUTO_BUY,								// �ڵ� ����;
		MANUAL_BUY								// ���� ����;
	};

	/// �ý��ۿ� ū ��ȭ�� ���涧 ������� �����Ѵ�;
	/// ������ ������ ������ �ƴ϶�� �ٲ��� ���� ������ ���δ�;
	// �ζ� �ý��� ID;
	typedef unsigned int LOTTO_SYSTEM_ID;
	const int LOTTO_NONE_ID = -1;
	const int LOTTO_MIN_ID = 0;
	const int LOTTO_MAX_ID = 1;

	// ���;
	const int MAX_LOTTO_SYSTEM = 2;				// �ִ� �ζ� �ý��� ����;
	const int MAX_DRAWING_NUM_LIST = 7;			// �ִ� �ζ� ��ȣ ����;
	const int MAX_RANKING_NUM = 5;				// �ִ� ��ŷ;

	/// Tokenizer
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
}