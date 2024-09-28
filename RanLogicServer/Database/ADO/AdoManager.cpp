#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "./ADOManager.h"

#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

AdoManager::AdoManager(NetServer* pServer, SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider)
    : IDbManager(pServer, ServerType, ServiceProvider, ADO)
{
	//
	// in case of Multi-threading, CoInitialize&CoUninitialize MUST be called per a thread.
	//
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		sc::writeLogError(std::string("AdoManager::::AdoManager() - CoInitialized() Failure."));
	}
}

AdoManager::~AdoManager()
{
    WaitAllDbJobCompleted(10000);
	//
	// in case of Multi-threading, CoInitialize&CoUninitialize MUST be called per a thread.
	//
	::CoUninitialize();
}

void AdoManager::WaitAllDbJobCompleted(DWORD WaitLimitTime)
{
    IDbManager::WaitAllDbJobCompleted(WaitLimitTime);
}

bool AdoManager::SetConnectionString(
    DB_TYPE Type,
    const std::string& Ip,
    const std::string& DbName,
    const std::string& Id,
    const std::string& Password, 
	int AdoMaxPoolSize, 
	int DatabasePacketSize)
{
    bool bReturn = false;
    std::string DbConnString(
        sc::string::format(
            "Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s;Max Pool Size=%d;Packet Size=%d;Current Language=english;",
            Password.c_str(),
            Id.c_str(),
            DbName.c_str(),
            Ip.c_str(),
			AdoMaxPoolSize,
			DatabasePacketSize));
    switch (Type)
    {
    case LOG_DB:
        m_LogDBConnString = DbConnString;
        if (m_adoLog.Open(m_LogDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
        break;

    case GAME_DB:
        m_GameDBConnString = DbConnString;
        if (m_adoGame.Open(m_GameDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
        break;
    case USER_DB:
        m_UserDBConnString = DbConnString;
        if (m_adoUser.Open(m_UserDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
        break;
    case SHOP_DB:
        m_ShopDBConnString = DbConnString;
        if (m_adoShop.Open(m_ShopDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
	case TEXASHOLDEM_DB:
		m_TexasHoldemDBConnString = DbConnString;
		if (m_adoTexasHoldem.Open(m_TexasHoldemDBConnString))
		{
			bReturn = TRUE;
			CheckVersion(Type);
		}
        break;
	case AUTH_DB:
		m_GlobalAuthDBConnString = DbConnString;
		if (m_adoGlobalAuth.Open(m_GlobalAuthDBConnString))
		{
			bReturn = TRUE;
			CheckVersion(Type);
		}
        break;
    case WORLDBATTLE_RELATED_DB:
        m_WorldBattleDBConnString = DbConnString;
        if (m_adoWorldBattle.Open(m_WorldBattleDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
        break;
    case RANMOBILE_DB:
        m_RanMobileDBConnString = DbConnString;
        if (m_adoRanMobile.Open(m_RanMobileDBConnString))
        {
            bReturn = TRUE;
            CheckVersion(Type);
        }
        break;
    default:
        MIN_ASSERT(0 && "아직 지원되지 않습니다.");
        break;
    }
    return bReturn;
}

bool AdoManager::IsAlive()
{
    bool bReturn = true;
    
    if (!m_UserDBConnString.empty() && !m_adoUser.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect user database
		if ( FALSE == m_adoUser.Open( m_UserDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to User Database" ) );
			bReturn = false;
		}
    }

    if (!m_GameDBConnString.empty() && !m_adoGame.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect game database
		if ( FALSE == m_adoGame.Open( m_GameDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to Game Database" ) );
			bReturn = false;
		}
    }

    if (!m_LogDBConnString.empty() && !m_adoLog.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect log database
		if ( FALSE == m_adoLog.Open( m_LogDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to Log Database" ) );
			bReturn = false;
		}
    }

    if (!m_ShopDBConnString.empty() && !m_adoShop.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect shop database
		if ( FALSE == m_adoShop.Open( m_ShopDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to Shop Database" ) );
			bReturn = false;
		}
    }

	if (!m_TexasHoldemDBConnString.empty() && !m_adoTexasHoldem.Execute(std::string("SELECT @@VERSION")))
	{
		// try reconnect TexasHoldem database
		if ( FALSE == m_adoTexasHoldem.Open( m_TexasHoldemDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to TexasHoldem Database" ) );
			bReturn = false;
		}
	}

	if (!m_GlobalAuthDBConnString.empty() && !m_adoGlobalAuth.Execute(std::string("SELECT @@VERSION")))
	{
		// try reconnect TexasHoldem database
		if ( FALSE == m_adoGlobalAuth.Open( m_GlobalAuthDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to GlobalAuth Database" ) );
			bReturn = false;
		}
	}

	if (!m_WorldBattleDBConnString.empty() && !m_adoWorldBattle.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect game database
		if ( FALSE == m_adoWorldBattle.Open( m_WorldBattleDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to WorldBattle Database" ) );
			bReturn = false;
		}
    }

	if (!m_RanMobileDBConnString.empty() && !m_adoRanMobile.Execute(std::string("SELECT @@VERSION")))
    {
		// try reconnect game database
		if ( FALSE == m_adoRanMobile.Open( m_RanMobileDBConnString ) )
		{
			sc::writeLogError( std::string( "Attempt to reconnect, but could not connect to RanMobile Database" ) );
			bReturn = false;
		}
    }

    return bReturn;
}

bool AdoManager::Reconnect()
{
    bool bReturn = true;
    if (!m_UserDBConnString.empty() && !m_adoUser.Open(m_UserDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to User Database"));
        bReturn = false;
    }

    if (!m_GameDBConnString.empty() && !m_adoGame.Open(m_GameDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to Game Database"));
        bReturn = false;
    }

    if (!m_LogDBConnString.empty() && !m_adoLog.Open(m_LogDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to Log Database"));
        bReturn = false;
    }

    if (!m_ShopDBConnString.empty() && !m_adoShop.Open(m_ShopDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to Shop Database"));
        bReturn = false;
    }

	if (!m_TexasHoldemDBConnString.empty() && !m_adoTexasHoldem.Open(m_TexasHoldemDBConnString))
	{
		sc::writeLogError(std::string("Can't connect to TexasHoldem Database"));
		bReturn = false;
	}

	if (!m_GlobalAuthDBConnString.empty() && !m_adoGlobalAuth.Open(m_GlobalAuthDBConnString))
	{
		sc::writeLogError(std::string("Can't connect to GlobalAuth Database"));
		bReturn = false;
	}

    if (!m_WorldBattleDBConnString.empty() && !m_adoWorldBattle.Open(m_WorldBattleDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to WorldBattle Database"));
        bReturn = false;
    }

    if (!m_RanMobileDBConnString.empty() && !m_adoRanMobile.Open(m_RanMobileDBConnString))
    {
        sc::writeLogError(std::string("Can't connect to RanMobile Database"));
        bReturn = false;
    }

    return bReturn;
}

bool AdoManager::CheckHdd()
{
    // 나중에 구현
    return true;
}

bool AdoManager::CheckVersion(DB_TYPE Type)
{
    bool bReturn = false;
	std::string strDbType;
    std::string ConnectionString;
    std::string Query("SELECT @@VERSION AS VERSION");
    switch (Type)
    {
    case LOG_DB:
		strDbType = "ADO:Log DB runs on ";
        ConnectionString = m_LogDBConnString;
        break;
    case GAME_DB:
		strDbType = "ADO:Game DB runs on ";
        ConnectionString = m_GameDBConnString;
        break;
    case USER_DB:
		strDbType = "ADO:User DB runs on ";
        ConnectionString = m_UserDBConnString;
        break;
    case SHOP_DB:
		strDbType = "ADO:Shop DB runs on ";
        ConnectionString = m_ShopDBConnString;
        break;
	case TEXASHOLDEM_DB:
		strDbType = "ADO:TexasHoldem DB runs on ";
		ConnectionString = m_TexasHoldemDBConnString;
		break;
	case AUTH_DB:
		strDbType = "ADO:Global Auth DB runs on ";
		ConnectionString = m_GlobalAuthDBConnString;
		break;
    case WORLDBATTLE_RELATED_DB:
		strDbType = "ADO:WorldBattle DB runs on ";
        ConnectionString = m_WorldBattleDBConnString;
        break;
    case RANMOBILE_DB:
		strDbType = "ADO:RanMobile DB runs on ";
        ConnectionString = m_RanMobileDBConnString;
        break;
    default:
        MIN_ASSERT(0 && "아직 지원되지 않습니다.");
        bReturn = true;
        break;
    }

    if (bReturn)
        return false;

    sc::db::AdoExt Ado(ConnectionString);
    if (!Ado.Execute(Query))
        return false;

    std::string StrVersion;

    BEGIN_GETCOLLECT(Ado)
        Ado.GetCollect(0, StrVersion);
    END_GETCOLLECT(Ado)

    sc::writeLogInfo(strDbType + StrVersion);
    return true;
}

int AdoManager::ExecuteStoredProcedure(const std::string& SpName, sc::db::AdoExt& Ado)
{
    if (!Ado.Execute4Cmd(SpName, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(
                "AdoManager::ExecuteStoredProcedure %1%", SpName));
        return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_OK;
    }
}

int AdoManager::ExecuteStoredProcedureIntReturn(const std::string& SpName, sc::db::AdoExt& Ado, const std::string& OutPutParamName)
{    
    if (!Ado.Execute4Cmd(SpName, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(	
                "AdoManager::ExecuteStoredProcedureIntReturn %1%", SpName));
        return sc::db::DB_ERROR;
    }

    _variant_t varReturn;
    if (!Ado.GetParam(OutPutParamName.c_str(), varReturn))
    {
        sc::writeLogError(
            sc::string::format(
                "AdoManager::ExecuteStoredProcedureIntReturn %1% GetParam", SpName));
        return sc::db::DB_ERROR;
    }
    return varReturn.intVal;
}

LONGLONG AdoManager::ExecuteStoredProcedureLonglongReturn(const std::string& SpName, sc::db::AdoExt& Ado, const std::string& OutPutParamName)
{
    if (!Ado.Execute4Cmd(SpName, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(
                "AdoManager::ExecuteStoredProcedureLonglongReturn %1%", SpName));
        return sc::db::DB_ERROR;
    }

    _variant_t varReturn;
    if (!Ado.GetParam(OutPutParamName.c_str(), varReturn))
    {
        sc::writeLogError(
            sc::string::format(
                "AdoManager::ExecuteStoredProcedureLonglongReturn %1% GetParam", SpName));
        return sc::db::DB_ERROR;
    }
    return varReturn.llVal;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenGameDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenUserDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenBoardDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenLogDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int AdoManager::OdbcOpenShopDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenTerraDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenGspDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenThaiDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenKorDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

//! 나중에 삭제해야 함 ------------------------------------------------------
int	AdoManager::OdbcOpenMyDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
    MIN_ASSERT(0 && "Do not call this function");
    return sc::db::DB_ERROR;
}

} // namespace db