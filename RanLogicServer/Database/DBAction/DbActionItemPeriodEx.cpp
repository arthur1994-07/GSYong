#include "pch.h"

#include "../../../SigmaCore/Util/DateTime.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "./DbActionItemPeriodEx.h"

#include "../DbManager.h"

namespace db
{
	int ItemPeriodExLogInsert::Execute(NetServer* pServer)
	{
		if ( m_pSubDbManager->ItemPeriodExLogInsert(dwChaNum, wItemMid, wItemSid, RemainNum, tPreviousPeriod, tCurrentPeriod) == sc::db::DB_ERROR )
			return sc::db::DB_ERROR;

		return sc::db::DB_OK;
	}
}