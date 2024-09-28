#include "../../pch.h"
#include <typeinfo>
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"
#include "../../Server/s_CFieldServer.h"
#include "DbExecuter.h"
#include "DbAction.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

DbExecuter::DbExecuter(NetServer* pServer)
    : EventThread(0)	//mjeon.tbb.concurrent_bounded_queue의 blocking pop()을 사용하므로
    , m_pServer(pServer)
	, m_nError( sc::db::DB_OK )
{
	start();
}

DbExecuter::~DbExecuter()
{   
	end();

	ExecuteJobLast();	//반드시 end()가 완료되고 난 후에 처리하라. 파괴자가 호출되는 스레드는 process()와 다른 스레드이다.
						
}

//자식에서 명시적인 구현이 필요하다.
void DbExecuter::start()
{
	startThread();
}
//자식에서 명시적인 구현이 필요하다.
void DbExecuter::end()
{
	endThread();
}

void DbExecuter::endHook()
{
#ifdef _USE_TBB

	m_cqJob.push(db::DbActionNull::Null);	//m_cqJob.pop()이 blocking 상태라면 이를 풀어준다.

#endif
}

void DbExecuter::AddJob(DbActionPtr spJob)
{
#ifdef _USE_TBB

	m_cqJob.push(spJob);	//mjeon.tbb

#else
	lockOn();
	m_vJob.push(spJob);	
	lockOff();
#endif
}

void DbExecuter::process()
{
    DbActionPtr pTemp;

#ifdef _USE_TBB
	m_cqJob.pop( pTemp );
#else
	//mjeon.tbb.concurrent_bounded_queue의 blocking pop()을 사용하므로
	//
	// TBB를 사용 안할 경우 무한루프에 빠진다.
	Sleep(1);
	lockOn();
	if ( !m_vJob.empty() )
	{
		pTemp = m_vJob.front();
		m_vJob.pop();
	}
    lockOff();
#endif

	if ( sc::db::DB_ERROR_OUTOFMEMORY == m_nError )
		return;

	if ( pTemp )
    {
		DWORD StartTime = GetTickCount();
		int nResult = pTemp->Execute( m_pServer );
		DWORD ElspTime = GetTickCount() - StartTime;
		if ( ElspTime > 500 )
		{
			sc::writeLogError(
				sc::string::format( "Class %1% Time %2%", typeid( *pTemp ).name(), ElspTime ) );
		}

		if ( sc::db::DB_ERROR_OUTOFMEMORY == nResult && sc::db::DB_ERROR_OUTOFMEMORY != m_nError )
		{
			m_nError = sc::db::DB_ERROR_OUTOFMEMORY;

			sc::writeLogError( 
				sc::string::format( "Class %1% error E_OUTOFMEMORY DbExecuter will stop", typeid( *pTemp ).name() ) );
			sc::writeLogError( std::string( "server will stop. because db E_OUTOFMEMORY error." ) );

			if ( m_pServer )
				m_pServer->ForceStop( SERVER_STOP_OUTOFMEMORY );
		}
    }
}

void DbExecuter::ExecuteJobLast()
{
#ifdef _USE_TBB

	DbActionPtr pTemp;

	while ( m_cqJob.try_pop( pTemp ) )
	{
		if ( sc::db::DB_ERROR_OUTOFMEMORY == m_nError )
			continue;
		if ( pTemp )
			pTemp->Execute( m_pServer );
	}

#else
	lockOn();
	if (!m_vJob.empty())
	{		
		while (!m_vJob.empty())
		{			
			if (DbActionPtr pTemp = m_vJob.front())
				pTemp->Execute(m_pServer);
			m_vJob.pop();
		}
	}
    lockOff();
#endif
}

DbExecuterMan::DbExecuterMan(NetServer* pServer, SERVER_TYPE Type)
    : m_pServer(pServer)
    , m_pGame(NULL)
    , m_pLog(NULL)
    , m_pUser(NULL)
    , m_pWeb(NULL)
	, m_pShop(NULL)
    , m_pClubLog(NULL)
	, m_pAuth(NULL)
{
    if (Type == AGENT_SERVER)
    {
        m_pUser = new DbExecuter(pServer);
        m_pGame = new DbExecuter(pServer);
        m_pLog  = new DbExecuter(pServer);
        m_pShop = new DbExecuter(pServer);
        m_pWeb  = new DbExecuter(pServer);
    }
    else if (Type == FIELD_SERVER)
    {
        m_pUser = new DbExecuter(pServer);
        m_pGame = new DbExecuter(pServer);
        m_pLog  = new DbExecuter(pServer);
        m_pShop = new DbExecuter(pServer);
    }
    else if (Type == SESSION_SERVER)
    {
        m_pUser = new DbExecuter(pServer);
        m_pGame = new DbExecuter(pServer);
    }
    else if (Type == CACHE_SERVER)
    {
        m_pUser    = new DbExecuter(pServer);
        m_pGame    = new DbExecuter(pServer);
        m_pLog     = new DbExecuter(pServer);        
        m_pWeb     = new DbExecuter(pServer);
        m_pShop    = new DbExecuter(pServer);
        m_pClubLog = new DbExecuter(pServer);
    }
	else if (Type == AUTH_SERVER)
	{
		m_pAuth = new DbExecuter(pServer);
	}
    else
    {
    }
}

DbExecuterMan::~DbExecuterMan()
{
    SAFE_DELETE(m_pGame);
    SAFE_DELETE(m_pLog);
    SAFE_DELETE(m_pUser);
    SAFE_DELETE(m_pWeb);
    SAFE_DELETE(m_pShop);
    SAFE_DELETE(m_pClubLog);
}

void DbExecuterMan::AddJob(DbActionPtr spJob, DB_TYPE Type)
{
    switch (Type)
    {
    case GAME_DB:
        AddGameDbJob(spJob);
        break;
    case USER_DB:
        AddUserDbJob(spJob);
        break;
    case LOG_DB:
        AddLogDbJob(spJob);
        break;
    case WEB_DB:
        AddWebDbJob(spJob);
        break;
    case SHOP_DB:
        AddShopDbJob(spJob);
        break;
    case CLUB_LOG_DB:
        AddClubLogDbJob(spJob);
        break;
    default:
        sc::writeLogError(
            sc::string::format(
                "DbExecuteMan AddJob unknown type db %1%", Type));
        break;
    };
}

void DbExecuterMan::AddGameDbJob(DbActionPtr spJob)
{
    if (spJob && m_pGame)
    {
        m_pGame->AddJob(spJob);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pGame is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pGame is null");
    }
}

void DbExecuterMan::AddUserDbJob(DbActionPtr spJob)
{
    if (spJob && m_pUser)
    {
        m_pUser->AddJob(spJob);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pUser is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pUser is null");
    }
}

void DbExecuterMan::AddLogDbJob(DbActionPtr spJob)
{
    if (spJob && m_pLog)
    {
        m_pLog->AddJob(spJob);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pLog is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pLog is null");
    }
}

void DbExecuterMan::AddWebDbJob(DbActionPtr spJob)
{
    if (spJob && m_pWeb)
    {
        m_pWeb->AddJob(spJob);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pWeb is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pWeb is null");
    }
}

void DbExecuterMan::AddShopDbJob(DbActionPtr spJob)
{
    if (spJob && m_pShop)
    {
        m_pShop->AddJob(spJob);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pShop is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pShop is null");
    }
}

void DbExecuterMan::AddClubLogDbJob(DbActionPtr spJob)
{
    if (spJob && m_pClubLog)
    {
        m_pClubLog->AddJob(spJob);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(sc::string::format("DbExecuteMan spJob or m_pClubLog is null %1%", typeid(*spJob).name()));
    }
    else
    {
        sc::writeLogError("DbExecuteMan spJob or m_pClubLog is null");
    }
}

void DbExecuterMan::AddAuthDbJob(DbActionPtr spJob)
{
    if (spJob)
    {
        m_pAuth->AddJob(spJob);
        return;
    }
    else
    {
        sc::writeLogError("DbExecuteMan AddAuthDbJob m_pClubLog is null");
    }
}

void DbExecuterMan::ExecuteLastJob()
{
    if (m_pGame)
    {
        m_pGame->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob Game"));
    }
    if (m_pLog)
    {
        m_pLog->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob Log"));
    }
    if (m_pUser)
    {
        m_pUser->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob User"));
    }
    if (m_pWeb)
    {
        m_pWeb->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob Web"));
    }
    if (m_pShop)
    {
        m_pShop->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob Shop"));
    }
    if (m_pClubLog)
    {
        // Club log 는 작업을 하지 않는다.
    }
	if (m_pAuth)
	{
		// 인증서버는 ExecuteLastJob를 호출하지 않으므로 아무런 작업을 하지 않음
		m_pAuth->ExecuteJobLast();
        sc::writeLogInfo(std::string("DbExecuteMan ExecuteLastJob Auth"));
	}
}

bool DbExecuterMan::IsAllDbJobCompleted()
{
    if (m_pGame && !m_pGame->IsJobEmpty())
        return false;

    if (m_pLog && !m_pLog->IsJobEmpty())
        return false;

    if (m_pUser && !m_pUser->IsJobEmpty())
        return false;

    if (m_pWeb && !m_pWeb->IsJobEmpty())
        return false;

    if (m_pShop && !m_pShop->IsJobEmpty())
        return false;

    if (m_pAuth && !m_pAuth->IsJobEmpty())
        return false;

    // club lob 는 작업을 하지 않는다.
    // m_pClubLog

    return true;
}

} // namespace db