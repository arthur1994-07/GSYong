#include "../../pch.h"
#include "../DbDefineLog.h"
#include "./OdbcStmt.h"
#include "s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::AddLogPing( int nUserNum, int nRTT, const char* szIP, __time64_t tTime )
{
    sc::writeLogError("COdbcManager::AddLogPing. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[128] = {0};
	int nReturn = 0;
	CTime cTime(tTime);
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	_snprintf( szQuery, 128, "{call sp_InsertPing(%d,%d,'%s','%s',?)}", nUserNum, nRTT, szIP, strTime.GetString());	
	nReturn = m_pLogDB->ExecuteSpInt(szQuery);	
	return nReturn;
*/
}

int COdbcManager::AddLogServerState(
    const __time64_t& LogDate,
	int nSGNum,
	int nSvrNum,
	int nUserNum,
	int nUserMax)
{
    sc::writeLogError("COdbcManager::AddLogServerState. Use ADO");
    return sc::db::DB_ERROR;
/*
	//TCHAR szTemp[128] = {0};
	//_snprintf( szTemp, 128, "{call log_serverstate(%d,%d,%d,%d)}", nUserNum, nUserMax, nSvrNum, nSGNum ); 
    std::string Query(
        sc::string::format(
            "{call LogServerStateInsertNew(%1%, %2%, %3%, %4%, '%5%')}",
            nUserNum,
            nUserMax,
            nSvrNum,
            nSGNum,
            sc::time::DateTimeFormat(LogDate, true, true)));
	return m_pLogDB->ExecuteSp(Query);
*/
}

int COdbcManager::LogRandomItem(const LOG_RANDOM_OPTION& _sLOG)
{
    sc::writeLogError("LogRandomItem. Do not call this function. Plz use ADO");
    return sc::db::DB_ERROR;
    /*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_LogItemRandom_Insert(%d,%d,%d,%d,%d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", 
									_sLOG.m_Mid, _sLOG.m_Sid, _sLOG.m_ServerGroup, _sLOG.m_ServerNumber,
									_sLOG.m_FieldNumber, _sLOG.m_MakeType, _sLOG.m_MakeNumber, (short) _sLOG.m_OptionType1,
									_sLOG.m_OptionValue1, (short) _sLOG.m_OptionType2, _sLOG.m_OptionValue2,
									(short) _sLOG.m_OptionType3, _sLOG.m_OptionValue3, (short) _sLOG.m_OptionType4,
									_sLOG.m_OptionValue4); 

	return m_pLogDB->ExecuteSpInt(szTemp);
    */
}

int COdbcManager::LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG)
{
	sc::writeLogError("LogItemCostumeStat. Do not call this function. Plz use ADO");
	return sc::db::DB_ERROR;
}

int COdbcManager::LogPetAction( const LOG_PET_ACTION &_sLOG )
{
    sc::writeLogError("LogPetAction. Do not call this function. Plz use ADO");
    return sc::db::DB_ERROR;
    /*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_LogPetAction_Insert(%d,%d,%d,%d,%d,?)}", _sLOG.m_PetNum, _sLOG.m_ItemMID, 
													_sLOG.m_ItemSID, _sLOG.m_ActionType, _sLOG.m_PetFull); 

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
	return nReturn;
    */
}

int COdbcManager::LogVehicleAction( const LOG_VEHICLE_ACTION &_sLOG )
{
    sc::writeLogError("LogVehicleAction. Do not call this function. Plz use ADO");
    return sc::db::DB_ERROR;
    /*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_LogVehicleAction_Insert(%d,%d,%d,%d,%d,?)}", _sLOG.m_VehicleNum, _sLOG.m_ItemMID, 
		_sLOG.m_ItemSID, _sLOG.m_ActionType, _sLOG.m_VehicleFull); 

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
	return nReturn;
    */
}

int COdbcManager::LogAction(const LOG_ACTION& LogData)
{
    sc::writeLogError("LogAction. Do not call this function. Plz use ADO");
    return sc::db::DB_ERROR;
	/*
	sp_LogAction_Insert
	_sLOG.m_nCHARID
	_sLOG.m_nTYPE 
	_sLOG.m_nTARID 
	_sLOG.m_nTARTYPE 
	_sLOG.m_nEXP 
	_sLOG.m_nBRIGHT_POINT 
	_sLOG.m_nLIFE_POINT 
	_sLOG.m_nMONEY 
	*/
    /*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_LogAction_Insert(%d,%d,%d,%d,%I64d,%d,%d,%d)}", LogData.m_nCHARID,
													LogData.m_ActionType, LogData.m_TargetId, LogData.m_TargetType, 
													LogData.m_nEXP, LogData.m_nBRIGHT_POINT, LogData.m_nLIFE_POINT,
													LogData.m_nMONEY );

	int nReturn = m_pLogDB->ExecuteSp(szTemp);
	return nReturn;
    */
}

int COdbcManager::LogAppeal(
	int nSGNum,
	int nChaNum,
	CString strMsg)
{
    /*
    if ((strMsg.GetLength() > CHAT_MSG_SIZE) || (nSGNum < 0) || (nChaNum < 0))
    {
        return sc::db::DB_ERROR;
    }

	strMsg.Trim(_T(" ")); // 앞뒤 공백제거
	strMsg.Replace("'", "''"); // ' -> ''

    TCHAR szTemp[128] = {0};
	_snprintf(
        szTemp,
        128,
        "INSERT INTO LogAppeal (SGNum, ChaNum, ApNote) VALUES (%d, %d)",
													 nSGNum, nChaNum);

	return m_pLogDB->ExecuteSQL(szTemp);
    */
    return sc::db::DB_ERROR;
}

int COdbcManager::LogHackProgram(
	int nSGNum, 
	int nSvrNum, 
	int nUserNum, 
	int nChaNum,
	int nHackProgramNum,
	const TCHAR* szComment)
{
    sc::writeLogError("COdbcManager::LogHackProgram. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (szComment == NULL)
        return sc::db::DB_ERROR;
	
	CString strComment(szComment);
	strComment.Trim(_T(" ")); // 앞뒤 공백제거
	strComment.Replace("'", "''"); // ' -> ''	

	TCHAR szTemp[512] = {0};
	_snprintf( szTemp, 512, "{call InsertLogHackProgram(%d,%d,%d,%d,%d,'%s',?)}", 
												nSGNum, nSvrNum, nUserNum, nChaNum, nHackProgramNum,
												strComment.GetString());

	return m_pLogDB->ExecuteSpInt(szTemp);
*/
}

int COdbcManager::LogChaDeleted(
	int nSGNum,
	int nSvrNum,
	int nUserNum,
	int nChaNum )
{
	/*
	std::strstream strTemp;
	strTemp << "Insert Into LogChaDelete (SGNum, SvrNum, nUserNum, nChaNum) Values (";
	strTemp << nSGNum << ",";
	strTemp << nSvrNum << ",";
	strTemp << nUserNum << ",";
	strTemp << nChaNum << ")";
	strTemp << std::ends;

	return m_pLogDB->ExecuteSQL(strTemp);

	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	*/
	return sc::db::DB_OK;
}

/*
int COdbcManager::LogServerState(int nSGNum, int nSvrNum, int nUserNum, int nUserMax)
{
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pLogDB->GetConnection();
	if (!pConn)	return sc::db::DB_ERROR;

	std::strstream strTemp;
	strTemp << "{call log_serverstate(";
	strTemp << nUserNum << ",";
	strTemp << nUserMax << ",";
	strTemp << nSvrNum << ",";	
	strTemp << nSGNum << ")}";
	strTemp << std::ends;

	::SQLPrepare(pConn->hStmt,(SQLCHAR*) strTemp.str(), SQL_NTS);

	sReturn = ::SQLExecute(pConn->hStmt);	
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(strTemp.str());		
		Print(GetErrorString(pConn->hStmt));
		m_pLogDB->FreeConnection(pConn);

		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}
	else
	{
		m_pLogDB->FreeConnection(pConn);

		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_OK;
	}
}
*/

int COdbcManager::CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)
{
    sc::writeLogError("COdbcManager::CTFPlayLog. Use ADO");
    return sc::db::DB_ERROR;
/*
	CString csQuery;
	csQuery.Format(_T(
		"{call sp_CTFPlayLog(%d, %d, %d, %d)}"), (int)nChaNum, (int)wSchool, nTotalPoint, nRewardPoint);
	int nReturn = m_pLogDB->ExecuteSpInt(csQuery.GetString());	
	return nReturn;
*/
}

int COdbcManager::GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool)
{
	sc::writeLogError("COdbcManager::GetCTFInfo. Use ADO");
	return sc::db::DB_ERROR;
}


int COdbcManager::LogGuidance(const LOG_GUIDANCE& sLog)
{
    sc::writeLogError("COdbcManager::LogGuidance. Use ADO");
    return sc::db::DB_ERROR;
/*	
	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "{call sp_InsertLogGuidance(%hd,%hd,%d,%d,%d,%d,%d,%d,%d,%hd,%hd,%hd,'%s',?)}", 
								sLog.wMapMID, sLog.wMapSID, sLog.dwClubID, sLog.nCertify, sLog.dwAllianceID, 
								sLog.dwAlliance[0], sLog.dwAlliance[1], sLog.dwCharID, sLog.dwClass, 
								sLog.wLevel, sLog.wKill, sLog.wDeath, sLog.strTime.GetString());
	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
	return sc::db::DB_OK;
*/
}

int COdbcManager::LogCDM(const LOG_CDM& sLog)
{
    sc::writeLogError("COdbcManager::LogCDM. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "{call sp_InsertLogCDM(%hd,%hd,%d,%d,%d,%hd,%hd,%hd,%hd,%hd,'%s',?)}", 
								sLog.wMapMID, sLog.wMapSID, sLog.dwClubID, sLog.dwCharID, sLog.dwClass, 
								sLog.wLevel, sLog.wKill, sLog.wDeath, sLog.wClubKill, sLog.wClubDeath, sLog.strTime.GetString());
	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
	return sc::db::DB_OK;
*/
}

int COdbcManager::LogCDMReuslt(const LOG_CDM_RESULT& sLog)
{
    sc::writeLogError("COdbcManager::LogCDMReuslt. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "{call sp_InsertResultLogCDM(%hd,%hd,%hd,%d,'%s',%d,%d,%hd,%hd,%hd,%hd,%hd,'%s',?)}", 
		sLog.wMapMID, sLog.wMapSID, sLog.wClubRank, sLog.dwClubID, sLog.strClubName,
		sLog.dwCharID, sLog.dwClass, sLog.wLevel, sLog.wKill, sLog.wDeath, sLog.wClubKill,
		sLog.wClubDeath, sLog.strTime );
	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
	return sc::db::DB_OK;
*/
}

int COdbcManager::ExecuteSqlLogDb(const std::string& SqlStr)
{
    if (m_pLogDB)
        return m_pLogDB->ExecuteSQL(SqlStr);
    else
        return sc::db::DB_ERROR;
}

int COdbcManager::LogMiniGameOddEvenInsert(
    DWORD UserDbNum,
    DWORD CharDbNum,        
    BYTE  CurrentRound,
    BYTE  WinLost,
    __int64 BattingMoney,
    __int64 DividendMoney,
    __int64 ChaMoney)
{
    MIN_ASSERT(0 && "LogMiniGameOddEvenInsert. Do not call this function. use ADO function.");
    return sc::db::DB_ERROR;
}

// 인증 서버 로그 기록
int COdbcManager::AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate )
{
	sc::writeLogError("COdbcManager::AddAuthServerState. Use ADO");
    return sc::db::DB_ERROR;
}

// 인증 절차 수행
int COdbcManager::ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID )
{
	sc::writeLogError("COdbcManager::ProcessCertificationForAuth. Use ADO");
    return sc::db::DB_ERROR;
}

// 이벤트 정보 기록
int COdbcManager::AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo )
{
	sc::writeLogError("COdbcManager::AddAuthEventInfo. Use ADO");
    return sc::db::DB_ERROR;
}

// 인증 서버 로그 기록
int COdbcManager::AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort )
{
	sc::writeLogError("COdbcManager::AddAuthServerState. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::AddAuthServerConnectionStateInit()
{
	sc::writeLogError("COdbcManager::AddAuthServerConnectionStateInit. Use ADO");
    return sc::db::DB_ERROR;
}
