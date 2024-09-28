#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./DBAction/DbExecuter.h"
#include "./DBAction/DbAction.h"
#include "./DbManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

IDbManager::IDbManager(NetServer* pServer, SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider, EM_DB_CONNECTION_TYPE ConnectionType)
    : m_pServer(pServer)
    , m_ServiceProvider(ServiceProvider)
    , m_ConnectionType(ConnectionType)
{
    m_pExecuter = new DbExecuterMan(pServer, ServerType);
}

IDbManager::~IDbManager()
{
    SAFE_DELETE(m_pExecuter);
}

void IDbManager::AddJob(db::DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddJob(spJob, spJob->Type());
}

void IDbManager::AddGameJob(DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddGameDbJob(spJob);
}

void IDbManager::AddLogJob(DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddLogDbJob(spJob);
}

void IDbManager::AddUserJob(DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddUserDbJob(spJob);
}

void IDbManager::AddWebJob(DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddWebDbJob(spJob);
}

void IDbManager::AddShopJob(DbActionPtr spJob, IDbManager *pSubDbMan)
{
    spJob->SetDbManager(this);
	spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddShopDbJob(spJob);
}

void IDbManager::AddClubLogDbJob(DbActionPtr spJob, IDbManager* pSubDbMan)
{
    spJob->SetDbManager(this);
    spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddClubLogDbJob(spJob);
}

void IDbManager::AddAuthJob(DbActionPtr spJob, IDbManager* pSubDbMan)
{
    spJob->SetDbManager(this);
    spJob->SetSubDbManager(pSubDbMan);	//mjeon.ado

    m_pExecuter->AddAuthDbJob(spJob);
}

void IDbManager::ExecuteLastJob()
{
    m_pExecuter->ExecuteLastJob();
}

void IDbManager::WaitAllDbJobCompleted(DWORD WaitLimitTime)
{
    DWORD StartTime = GetTickCount();
    while (!m_pExecuter->IsAllDbJobCompleted())
    {
        Sleep(100);
        if ((GetTickCount() - StartTime) > WaitLimitTime)
        {
            sc::writeLogError(
                sc::string::format(
                "Can't completed DB job(Executer) until %1% msec.", WaitLimitTime));
        }
    }
}

} // namespace db