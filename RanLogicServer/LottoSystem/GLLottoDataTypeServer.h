#pragma once

#include "../../RanLogic/LottoSystem/GLLottoDataType.h"

namespace LottoSystem
{

	// Log
	struct LOTTO_SYSTEM_TURN_LOG
	{
		EMLOTTO_MONEY_TYPE moneyType;
		unsigned int systemId;
		unsigned int turnNum;
		std::string drawingStartTime;
		std::string drawingEndTime;
		std::string winInfo;
		LONGLONG ticketMoney;
		int ticketMaxCount;
		LONGLONG accumulateMoney;
		//LONGLONG charge;
		int chargePct;
		std::string rankingAllMoney;
		std::string rankingAllCount;
		std::string rankingMoney;
		LONGLONG forwardMoney;

		LOTTO_SYSTEM_TURN_LOG()
			: moneyType( LOTTO_MONEY_TYPE_GAME_MONEY )
			, systemId( 0 )
			, turnNum( 0 )
			, ticketMoney( 0 )
			, ticketMaxCount( 0 )
			, accumulateMoney( 0 )
			//, charge( 0 )
			, chargePct( 0 )
			, forwardMoney( 0 )
		{

		}

	};

	// Buy Log
	struct LOTTO_SYSTEM_BUY_LOG
	{
		DWORD dwTicketID;
		unsigned int systemId;
		unsigned int turnNum;
		DWORD dwUserNum;
		DWORD chaNum;
		DWORD dwServerGroup;
		EMLOTTO_BUY_TYPE buyType;
		std::string drawingNum;

		LOTTO_SYSTEM_BUY_LOG()
			: dwTicketID( 0 )
			, systemId( 0 )
			, turnNum( 0 )
			, dwUserNum( 0 )
			, chaNum( 0 )
			, dwServerGroup( 0 )
			, buyType( NONE_BUY_TYPE )
		{

		}
	};

	// 당첨번호 경우의 수 데이터;
	struct LOTTO_WIN_NUM_INT
	{
		std::vector<LONGLONG> winNumInt;		// Ranking에 맞는 MainNum의 모든 경우의 수;
		std::vector<LONGLONG> bonusNumInt;		// Ranking에 맞는 BonusNum의 모든 경우의 수;
		unsigned int ranking;					// Ranking;

		LOTTO_WIN_NUM_INT()
			: ranking( 0 )
		{

		}
	};
}