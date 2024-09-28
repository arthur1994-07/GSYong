#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "./OdbcStmt.h"
#include "./s_COdbcEnv.h"
#include "./s_COdbcManager.h"
#include "../DBAction/DbExecuter.h"
#include "../DBAction/DbAction.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

COdbcManager::COdbcManager(NetServer* pServer, db::SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider, bool UseCustomPool)
    : db::IDbManager(pServer, ServerType, ServiceProvider, db::ODBC)
    , m_UseCustomPool(UseCustomPool)
    , m_pGameDB(NULL)
	, m_pUserDB(NULL)
	, m_pBoardDB(NULL)
	, m_pLogDB(NULL)
	, m_pShopDB(NULL)
	, m_pTerraDB(NULL)
	, m_pGspDB(NULL)
	, m_pThaiDB(NULL)
	, m_pKorDB(NULL)
	, m_pMalaysiaDB(NULL)
{
	COdbcEnv::GetInstance();
}

COdbcManager::~COdbcManager()
{
    WaitAllDbJobCompleted(10000);

	SAFE_DELETE(m_pGameDB);
	SAFE_DELETE(m_pUserDB);
	SAFE_DELETE(m_pBoardDB);
	SAFE_DELETE(m_pLogDB);
	SAFE_DELETE(m_pShopDB);
	SAFE_DELETE(m_pTerraDB);
	SAFE_DELETE(m_pGspDB);
	SAFE_DELETE(m_pThaiDB);
	SAFE_DELETE(m_pKorDB);
	SAFE_DELETE(m_pMalaysiaDB);
	COdbcEnv::GetInstance()->ReleaseInstance();
}

void COdbcManager::WaitAllDbJobCompleted(DWORD WaitLimitTime)
{
    DWORD StartTime = GetTickCount();
    while (!IsAllDbJobCompleted())
    {
        Sleep(100);
        if ((GetTickCount() - StartTime) > WaitLimitTime)
        {
            sc::writeLogError(
                sc::string::format(
                    "Can't completed DB job(connection) until %1% msec.", WaitLimitTime));
        }
    }

    IDbManager::WaitAllDbJobCompleted(WaitLimitTime);
}

bool COdbcManager::IsAllDbJobCompleted()
{
    if (!m_UseCustomPool)
        return true;

    if (m_pGameDB && !m_pGameDB->IsAllDbJobCompleted())
        return false;
    
    if (m_pUserDB && !m_pUserDB->IsAllDbJobCompleted())
        return false;

    if (m_pBoardDB && !m_pBoardDB->IsAllDbJobCompleted())
        return false;

    if (m_pLogDB && !m_pLogDB->IsAllDbJobCompleted())
        return false;

    if (m_pShopDB && !m_pShopDB->IsAllDbJobCompleted())
        return false;
    
    if (m_pTerraDB && !m_pTerraDB->IsAllDbJobCompleted())
        return false;
    
    if (m_pGspDB && !m_pGspDB->IsAllDbJobCompleted())
        return false;

    if (m_pThaiDB && !m_pThaiDB->IsAllDbJobCompleted())
        return false;

    if (m_pKorDB && !m_pKorDB->IsAllDbJobCompleted())
        return false;

    if (m_pMalaysiaDB && !m_pMalaysiaDB->IsAllDbJobCompleted())
        return false;

    return true;
}

bool COdbcManager::IsAlive()
{
    bool bReturn = true;
    if (m_pGameDB && sc::db::DB_ERROR==m_pGameDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Game Database"));
        bReturn = false;
    }

    if (m_pUserDB && sc::db::DB_ERROR==m_pUserDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to User Database"));
        bReturn = false;
    }

    if (m_pBoardDB && sc::db::DB_ERROR==m_pBoardDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Board Database"));
        bReturn = false;
    }

    if (m_pLogDB && sc::db::DB_ERROR==m_pLogDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Log Database"));
        bReturn = false;
    }

    if (m_pShopDB && sc::db::DB_ERROR==m_pShopDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Shop Database"));
        bReturn = false;
    }

    if (m_pTerraDB && sc::db::DB_ERROR==m_pTerraDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Terra Database"));
        bReturn = false;
    }

    if (m_pGspDB && sc::db::DB_ERROR==m_pGspDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to GSP Database"));
        bReturn = false;
    }

    if (m_pThaiDB && sc::db::DB_ERROR==m_pThaiDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Thai Database"));
        bReturn = false;
    }

    if (m_pKorDB && sc::db::DB_ERROR==m_pKorDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Korea Database"));
        bReturn = false;
    }

    if (m_pMalaysiaDB && sc::db::DB_ERROR==m_pMalaysiaDB->ExecuteSQL(std::string("SELECT @@VERSION")))
    {
        sc::writeLogError(std::string("Can't connect to Malaysia Database"));
        bReturn = false;
    }
    return bReturn;
}

bool COdbcManager::Reconnect()
{
    bool bReturn = true;
    if (m_pGameDB &&
        sc::db::DB_ERROR == OdbcOpenGameDB(m_GameDbInfo.OdbcName.c_str(), m_GameDbInfo.UserID.c_str(), m_GameDbInfo.UserPasswd.c_str(), m_GameDbInfo.DbName.c_str(), m_UseCustomPool, m_GameDbInfo.PoolSize, m_GameDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Game Database"));
        bReturn = false;
    }

    if (m_pUserDB &&
        sc::db::DB_ERROR == OdbcOpenUserDB(m_UserDbInfo.OdbcName.c_str(), m_UserDbInfo.UserID.c_str(), m_UserDbInfo.UserPasswd.c_str(), m_UserDbInfo.DbName.c_str(), m_UseCustomPool, m_UserDbInfo.PoolSize, m_UserDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to User Database"));
        bReturn = false;
    }

    if (m_pBoardDB &&
        sc::db::DB_ERROR == OdbcOpenBoardDB(m_BoardDbInfo.OdbcName.c_str(), m_BoardDbInfo.UserID.c_str(), m_BoardDbInfo.UserPasswd.c_str(), m_BoardDbInfo.DbName.c_str(), m_UseCustomPool, m_BoardDbInfo.PoolSize, m_BoardDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Board Database"));
        bReturn = false;
    }

    if (m_pLogDB &&
        sc::db::DB_ERROR == OdbcOpenLogDB(m_BoardDbInfo.OdbcName.c_str(), m_BoardDbInfo.UserID.c_str(), m_BoardDbInfo.UserPasswd.c_str(), m_BoardDbInfo.DbName.c_str(), m_UseCustomPool, m_BoardDbInfo.PoolSize, m_BoardDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Log Database"));
        bReturn = false;
    }

    if (m_pShopDB &&
        sc::db::DB_ERROR == OdbcOpenShopDB(m_ShopDbInfo.OdbcName.c_str(), m_ShopDbInfo.UserID.c_str(), m_ShopDbInfo.UserPasswd.c_str(), m_ShopDbInfo.DbName.c_str(), m_UseCustomPool, m_ShopDbInfo.PoolSize, m_ShopDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Shop Database"));
        bReturn = false;
    }

    if (m_pTerraDB &&
        sc::db::DB_ERROR == OdbcOpenTerraDB(m_TerraDbInfo.OdbcName.c_str(), m_TerraDbInfo.UserID.c_str(), m_TerraDbInfo.UserPasswd.c_str(), m_TerraDbInfo.DbName.c_str(), m_UseCustomPool, m_TerraDbInfo.PoolSize, m_TerraDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Terra Database"));
        bReturn = false;
    }

    if (m_pGspDB && 
        sc::db::DB_ERROR == OdbcOpenGspDB(m_GspDbInfo.OdbcName.c_str(), m_GspDbInfo.UserID.c_str(), m_GspDbInfo.UserPasswd.c_str(), m_GspDbInfo.DbName.c_str(), m_UseCustomPool, m_GspDbInfo.PoolSize, m_GspDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to GSP Database"));
        bReturn = false;
    }

    if (m_pThaiDB && 
        sc::db::DB_ERROR == OdbcOpenThaiDB(m_ThaiDbInfo.OdbcName.c_str(), m_ThaiDbInfo.UserID.c_str(), m_ThaiDbInfo.UserPasswd.c_str(), m_ThaiDbInfo.DbName.c_str(), m_UseCustomPool, m_ThaiDbInfo.PoolSize, m_ThaiDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Thai Database"));
        bReturn = false;
    }

    if (m_pKorDB && 
        sc::db::DB_ERROR == OdbcOpenKorDB(m_KorDbInfo.OdbcName.c_str(), m_KorDbInfo.UserID.c_str(), m_KorDbInfo.UserPasswd.c_str(), m_KorDbInfo.DbName.c_str(), m_UseCustomPool, m_KorDbInfo.PoolSize, m_KorDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Korea Database"));
        bReturn = false;
    }

    if (m_pMalaysiaDB && 
        sc::db::DB_ERROR == OdbcOpenMyDB(m_MalaysiaDbInfo.OdbcName.c_str(), m_MalaysiaDbInfo.UserID.c_str(), m_MalaysiaDbInfo.UserPasswd.c_str(), m_MalaysiaDbInfo.DbName.c_str(), m_UseCustomPool, m_MalaysiaDbInfo.PoolSize, m_MalaysiaDbInfo.DbTimeOut))
    {
        sc::writeLogError(std::string("Can't connect to Malaysia Database"));
        bReturn = false;
    }
    return bReturn;
}

bool COdbcManager::CheckHdd()
{
    // 나중에 구현
//     if (m_pGameDB)
//     if (m_pUserDB)
//     if (m_pBoardDB)
//     if (m_pLogDB)
//     if (m_pShopDB)
//     if (m_pTerraDB)
//     if (m_pGspDB)
//     if (m_pThaiDB)
//     if (m_pKorDB)
//     if (m_pMalaysiaDB)
    return true;
}

int	COdbcManager::OdbcOpenGameDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pGameDB);
    m_GameDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pGameDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pGameDB->OpenDB();
}

int	COdbcManager::OdbcOpenUserDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pUserDB);
    m_UserDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pUserDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pUserDB->OpenDB();
}

int COdbcManager::OdbcOpenBoardDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pBoardDB);
    m_BoardDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pBoardDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pBoardDB->OpenDB();
}

int COdbcManager::OdbcOpenLogDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pLogDB);
    m_LogDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pLogDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pLogDB->OpenDB();
}

int COdbcManager::OdbcOpenShopDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pShopDB);
    m_ShopDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pShopDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pShopDB->OpenDB();
}

int COdbcManager::OdbcOpenTerraDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pTerraDB);
    m_TerraDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pTerraDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pTerraDB->OpenDB();
}

int	COdbcManager::OdbcOpenGspDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pGspDB);
    m_GspDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pGspDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pGspDB->OpenDB();
}

int	COdbcManager::OdbcOpenThaiDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pThaiDB);
    m_ThaiDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pThaiDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pThaiDB->OpenDB();
}

int	COdbcManager::OdbcOpenKorDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pKorDB);
    m_KorDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pKorDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pKorDB->OpenDB();
}

// 말레이시아 PC방 이벤트
int	COdbcManager::OdbcOpenMyDB(
    const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName,
    bool UseCustomPool, int nPoolSize, int nDBTimeOut)
{
	SAFE_DELETE(m_pMalaysiaDB);
    m_MalaysiaDbInfo.SetData(szOdbcName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nPoolSize);
    m_pMalaysiaDB = new COdbcSupervisor(szOdbcName, szUsrID, szUsrPasswd, szDbName, UseCustomPool);
	return m_pMalaysiaDB->OpenDB();
}	

void COdbcManager::CloseGspDB()
{
	SAFE_DELETE(m_pGspDB);
}

void COdbcManager::CloseGameDB()
{
	SAFE_DELETE(m_pGameDB);
}

void COdbcManager::CloseUserDB()
{
	SAFE_DELETE(m_pUserDB);
}

void COdbcManager::CloseBoardDB()
{
	SAFE_DELETE(m_pBoardDB);
}

void COdbcManager::CloseLogDB()
{
	SAFE_DELETE(m_pLogDB);
}

void COdbcManager::CloseShopDB()
{
	SAFE_DELETE(m_pShopDB);
}

void COdbcManager::CloseTerraDB()
{
	SAFE_DELETE(m_pTerraDB);
}

void COdbcManager::CloseThaiDB()
{
	SAFE_DELETE(m_pThaiDB);
}

void COdbcManager::CloseKorDB()
{
	SAFE_DELETE(m_pKorDB);
}

void COdbcManager::CloseMyDB()
{
	SAFE_DELETE(m_pMalaysiaDB);
}

void COdbcManager::Print(const std::string& strMsg)
{
    sc::writeLogError(sc::string::format("DB:%1%", strMsg));
}

std::string COdbcManager::GetErrorString(SQLHSTMT hStmt, SQLSMALLINT fHandleType )
{
	SQLRETURN sReturn = 0;
	SQLCHAR SqlState[6] = {0};
	SQLCHAR Msg[sc::db::ODBC_ERROR_MESSAGE_LENGTH+1] = {0};
	SQLSMALLINT MsgLen = 0;
	SQLINTEGER nError = 0;
	int nDiag = 0;

    std::string strTemp;

	if (!hStmt)
        return strTemp;

	::SQLGetDiagField(fHandleType, hStmt, 0, SQL_DIAG_NUMBER, &nDiag, 0, &MsgLen);

	for (nDiag=1;;nDiag++)
	{
		sReturn = ::SQLGetDiagRec(fHandleType, 
								  hStmt, 
								  nDiag,
								  SqlState,
								  &nError,
								  Msg,
								  sc::db::ODBC_ERROR_MESSAGE_LENGTH, 
								  &MsgLen);
		if (sReturn == SQL_NO_DATA)
			break;
		else if ( sReturn == SQL_DATA_AT_EXEC ) 
			break;
		else
            strTemp = sc::string::format("%1%, NativeError:%2%, %3%", (LPCTSTR) SqlState, nError, (LPCTSTR) Msg);

	}
	return strTemp;
}

int COdbcManager::InstantDungeonLogTypeInsert( const db::EM_INSTANT_LOG_TYPE& nLogType, const char* strParamName1/*=0*/, const char* strParamName2/*=0*/, const char* strParamName3/*=0*/, const char* strParamName4/*=0*/, const char* strParamName5/*=0*/, const char* strParamName6/*=0*/, const char* strParamName7/*=0*/, const char* strParamName8/*=0*/, const char* strParamName9/*=0*/, const char* strParamName10/*=0*/, const char* strParamName11/*=0*/, const char* strParamName12/*=0*/, const char* strParamName13/*=0*/, const char* strParamName14/*=0*/, const char* strParamName15/*=0*/, const char* strParamName16/*=0*/, const char* strParamName17/*=0*/, const char* strParamName18/*=0*/, const char* strParamName19/*=0*/, const char* strParamName20/*=0 */ )
{
    sc::writeLogError("COdbcManager::InstantDungeonLogTypeInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::InstantDungeonLogInsert( const db::EM_INSTANT_LOG_TYPE& nLogType, const char* strParam1/*=0*/, const char* strParam2/*=0*/, const char* strParam3/*=0*/, const char* strParam4/*=0*/, const char* strParam5/*=0*/, const char* strParam6/*=0*/, const char* strParam7/*=0*/, const char* strParam8/*=0*/, const char* strParam9/*=0*/, const char* strParam10/*=0*/, const char* strParam11/*=0*/, const char* strParam12/*=0*/, const char* strParam13/*=0*/, const char* strParam14/*=0*/, const char* strParam15/*=0*/, const char* strParam16/*=0*/, const char* strParam17/*=0*/, const char* strParam18/*=0*/, const char* strParam19/*=0*/, const char* strParam20/*=0 */ )
{
    sc::writeLogError("COdbcManager::InstantDungeonLogTypeInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck /*= false*/  )
{
    sc::writeLogError("COdbcManager::ConsignmentsSaleGet. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum, const int& ServerGroup )
{
    sc::writeLogError("COdbcManager::ConsignmentsSaleSelectChaNum. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum )
{
    sc::writeLogError("COdbcManager::ConsignmentsUpdateTurnNum. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem )
{
    sc::writeLogError("COdbcManager::ConsignmentsSaleInsert. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConsignmentsSaleDelete( sc::MGUID& Guid )
{
    sc::writeLogError("COdbcManager::ConsignmentsSaleDelete. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 )
{
    sc::writeLogError("COdbcManager::ConfignmentSaleSlotExpansionDateSelect. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::ConfignmentSaleSlotExpansionDateSet( DWORD dwChaDbNum, __time64_t tExpansionDate )
{
    sc::writeLogError("COdbcManager::ConfignmentSaleSlotExpansionDateSet. Use ADO");
    return sc::db::DB_ERROR;
}

//////////////////////////////////////////////////////////////////////////
//
// TOURNAMENT BETTING SYSTEM
//
int COdbcManager::TournamentBettingSelect( DWORD ClientDBid, DWORD BettingStatus )
{
	sc::writeLogError("COdbcManager::TournamentBettingSelect. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::TournamentBettingInsert( DWORD ChaNum,LONGLONG BettingCount, DWORD BettingGameSciptid,DWORD BettingGroup)
{
	sc::writeLogError("COdbcManager::TournamentBettingInsert. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::TournamentBettingUpdate( DWORD ChaNum,DWORD BettingGameSciptid,DWORD BettingStatus)
{
	sc::writeLogError("COdbcManager::TournamentBettingUpdate. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::TournamentBettingDelete( DWORD ChaNum)
{
	sc::writeLogError("COdbcManager::TournamentBettingDelete. Use ADO");
	return sc::db::DB_ERROR;
}


//////////////////////////////////////////////////////////////////////////
//
// MATCHING SYSTEM
//

int COdbcManager::MatchingOnload( std::string Matching_Content_Type,
						   std::string Betting_Script,
						   std::string Room_Admin_Script,
						   std::string Matching_Script,
						   std::string Lobby_Field_Manager )
{
	sc::writeLogError("COdbcManager::MatchingOnload. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingGroup( DWORD GroupID,
						  DWORD GameScriptID,
						  bool  IsDeleted)
{
	sc::writeLogError("COdbcManager::MatchingGroup. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingGroupUser( DWORD GroupID,
							  DWORD GameScriptID,
							  DWORD UserDBNum,
							  bool  IsDeleted)
{
	sc::writeLogError("COdbcManager::MatchingGroupUser. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingQueueIn( DWORD ClientID,
							DWORD GameScriptID,
							bool  IsDeleted)
{
	sc::writeLogError("COdbcManager::MatchingQueueIn. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceIn( DWORD GroupID,
							   DWORD GameScriptID,
							   DWORD  MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingInstanceIn. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceJoin( DWORD GroupID,
								 DWORD MapID,
								 DWORD GameScriptID,
								 DWORD  MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingInstanceJoin. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceOnStart( DWORD GroupReadySize,
									DWORD GameScriptID,
									DWORD MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingInstanceOnStart. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceOut( DWORD GroupID,
								DWORD GameScriptID,
								DWORD MatchingScriptID) 
{
	sc::writeLogError("COdbcManager::MatchingInstanceOut. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceResualt( DWORD Faction,
									bool IsLose,
									DWORD MapID,
									DWORD GameScriptID,
									DWORD MatchingScriptID) 
{
	sc::writeLogError("COdbcManager::MatchingInstanceResualt. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingRoomDestroy( DWORD MapID,
								DWORD GameScriptID,
								DWORD MatchingScriptID) 
{
	sc::writeLogError("COdbcManager::MatchingRoomDestroy. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingRoomReq( DWORD MapID,
							DWORD GameScriptID,
							DWORD MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingRoomReq. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingFinalWinner( DWORD GroupID,
								DWORD GameScriptID,
								DWORD MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingFinalWinner. Use ADO");
	return sc::db::DB_ERROR;
}
int COdbcManager::MatchingInstanceTournament( int   tournamentPosition,
									   std::string GroupName,
									   int   GroupIndexPosition,
									   int   GroupSize,
									   int   GroupMaxSize,
									   int   GroupHeadLevel,
									   DWORD GroupID,
									   DWORD GameScriptID,
									   DWORD MatchingScriptID)
{
	sc::writeLogError("COdbcManager::MatchingInstanceTournament. Use ADO");
	return sc::db::DB_ERROR;
}

int COdbcManager::MatchingJoinCountLoad (
	MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList )
{
	sc::writeLogError( "COdbcManager::MatchingJoinCountLoad. Use ADO" );

	return sc::db::DB_ERROR;
}

int COdbcManager::MatchingJoinCountReset ( )
{
	sc::writeLogError( "COdbcManager::MatchingJoinCountReset. Use ADO" );

	return sc::db::DB_ERROR;
}

int COdbcManager::MatchingJoinCountUpdate (
	const DWORD _dwCharDBNum,
	const MatchingSystem::SJOIN_COUNT _sJoinCount )
{
	sc::writeLogError( "COdbcManager::MatchingJoinCountUpdate. Use ADO" );

	return sc::db::DB_ERROR;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
 * modified : 
 */
int COdbcManager::LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                                             const SNATIVEID& ItemId, /* 당첨 Item ID */
                                             INT32 ReqCount,          /* 목표 오픈 수량 */
                                             DWORD ChaNum,            /* 목표 캐릭터 번호 */
                                             DWORD GMNum,             /* 등록 GM Number */
                                             INT32 OpenCount,         /* 실제 오픈 수량 */
                                             DWORD Winner             /* 당첨자 */
                                            )
{
	sc::writeLogError( "COdbcManager::LogRandomboxChanceComplete. Use ADO" );

	return sc::db::DB_ERROR;
}

int COdbcManager::RanMobileRequest( DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass )
{
	sc::writeLogError( "COdbcManager::RanMobileRequest. Use ADO" );

	return sc::db::DB_ERROR;
}