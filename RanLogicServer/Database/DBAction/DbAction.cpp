#include "../../pch.h"
#include "DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"

namespace db
{

DbActionPtr const DbActionNull::Null;

//
//mjeon.dbman
//
DbAction::DbAction(DB_TYPE dbType, DWORD dwClient, USHORT uPort)
    : m_dbType(dbType)
    , m_ClientSlot(dwClient)
    , m_uPort(uPort)
    , m_pDbManager(NULL)
	, m_pSubDbManager(NULL)
{
	//
	// mjeon.dbman
	//
// 	if (!m_pDbManager)
// 	{
// 		m_pDbManager = COdbcManager::GetInstance();
// 	}
}

DbAction::~DbAction()
{
}

void DbAction::SetDbManager(db::IDbManager *pDbMan)
{
	m_pDbManager = pDbMan;
}

void DbAction::SetSubDbManager(db::IDbManager *pDbMan)
{	
	m_pSubDbManager = pDbMan;
}

int DbAction::Execute(NetServer* pServer)
{
	return 0;
}

} // namespace db