
#pragma once

#include "DbAction.h"

namespace db
{
	class ItemPeriodExLogInsert : public db::DbAction
	{
	private:
		DWORD dwChaNum;
		WORD wItemMid;		
		WORD wItemSid;		
		WORD RemainNum;
		__time64_t tPreviousPeriod;
		__time64_t tCurrentPeriod;

	public:
		ItemPeriodExLogInsert(DWORD  chaNum
			, WORD itemMid
			, WORD itemSid	
			, WORD remainNum
			, __time64_t previousPeriod
			, __time64_t currentPeriod)
			:dwChaNum(chaNum)
			, wItemMid(itemMid)	
			, wItemSid(itemSid)	
			, RemainNum(remainNum)
			, tPreviousPeriod(previousPeriod)
			, tCurrentPeriod(currentPeriod) {}
		virtual ~ItemPeriodExLogInsert(void) {}
		virtual int Execute(NetServer* pServer) override;
	};

}