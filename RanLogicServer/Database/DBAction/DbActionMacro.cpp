#include "pch.h"

#include "../../../SigmaCore/Util/DateTime.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "./DbActionMacro.h"

#include "../DbManager.h"

namespace db
{
	int MacroLogInsert::Execute(NetServer* pServer)
	{
		if ( m_pSubDbManager->MacroLogInsert(m_dwChaDBNum, m_wState) == sc::db::DB_ERROR )
			return sc::db::DB_ERROR;

		return sc::db::DB_OK;
	}
}