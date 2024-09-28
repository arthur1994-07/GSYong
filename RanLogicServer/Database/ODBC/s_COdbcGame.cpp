#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::GetCharacterInfo(
    int nUserNumber, int nChaNum, GLCHARAG_DATA* pChaData)
{
    sc::writeLogError("COdbcManager::GetCharacterInfo. Use ADO");
    return sc::db::DB_ERROR;
/*
    MIN_STATIC_ASSERT(GLCHARAG_DATA::VERSION <= 0x0002);

	assert(pChaData&&"(GLCHARAG_DATA*)의 값이 유효하지 않습니다.");
	SQLRETURN sReturn = 0;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	// 캐릭터 정보를 가져온다.
    std::string Query(
        sc::string::format(
            "EXEC sp_GetChaAgentInfo %1%, %2%", nChaNum, nUserNumber));
	
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query);		
		Print(GetErrorString(spConn->hStmt));
		
        return sc::db::DB_ERROR;
	}

	SQLINTEGER nUserNum = 0, cbUserNum=SQL_NTS;
	SQLINTEGER nSGNum   = 0, cbSGNum=SQL_NTS;
    SQLINTEGER nGuNum   = 0, cbGuNum=SQL_NTS; // Club 번호
	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLCHAR    szChaPhoneNumber[SMS_RECEIVER] = {0}; SQLINTEGER cbChaPhoneNumber = SQL_NTS;
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;

	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;
    
	SQLINTEGER nChaStartMap = 0, cbChaStartMap=SQL_NTS;
	SQLINTEGER nChaStartGate = 0, cbChaStartGate=SQL_NTS;

	SQLFLOAT   fChaPosX = 0; SQLINTEGER cbChaPosX=SQL_NTS;
	SQLFLOAT   fChaPosY = 0; SQLINTEGER cbChaPosY=SQL_NTS;
	SQLFLOAT   fChaPosZ = 0; SQLINTEGER cbChaPosZ=SQL_NTS;

	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;	

	SQLFLOAT fChaSavePosX = 0; SQLINTEGER cbChaSavePosX=SQL_NTS;
	SQLFLOAT fChaSavePosY = 0; SQLINTEGER cbChaSavePosY=SQL_NTS;
	SQLFLOAT fChaSavePosZ = 0; SQLINTEGER cbChaSavePosZ=SQL_NTS;

	SQLINTEGER nChaSchool = 0, cbChaSchool=SQL_NTS;
    TIMESTAMP_STRUCT sChaGuSecede; SQLINTEGER cbChaGuSecede = SQL_NTS; // 클럽탈퇴시간

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nUserNum,	0, &cbUserNum);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nSGNum,		0, &cbSGNum);
            ::SQLGetData(spConn->hStmt, 3, SQL_C_CHAR, szChaName,  CHR_ID_LENGTH+1, &cbChaName); 
            ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG, &nChaLevel, 0, &cbChaLevel);
			::SQLGetData(spConn->hStmt, 5, SQL_C_LONG, &nChaClass,   0, &cbChaClass);
			::SQLGetData(spConn->hStmt, 6, SQL_C_LONG, &nChaBright,  0, &cbChaBright);
            ::SQLGetData(spConn->hStmt, 7, SQL_C_LONG, &nChaStartMap,0, &cbChaStartMap);
			::SQLGetData(spConn->hStmt, 8, SQL_C_LONG,   &nChaStartGate,  0, &cbChaStartGate);
			::SQLGetData(spConn->hStmt, 9, SQL_C_DOUBLE, &fChaPosX,	0, &cbChaPosX);
			::SQLGetData(spConn->hStmt, 10, SQL_C_DOUBLE, &fChaPosY,	0, &cbChaPosY);

			::SQLGetData(spConn->hStmt, 11, SQL_C_DOUBLE, &fChaPosZ,	0, &cbChaPosZ);
			::SQLGetData(spConn->hStmt, 12, SQL_C_LONG,   &nChaSaveMap, 0, &cbChaSaveMap);
			::SQLGetData(spConn->hStmt, 13, SQL_C_DOUBLE, &fChaSavePosX, 0, &cbChaSavePosX);
			::SQLGetData(spConn->hStmt, 14, SQL_C_DOUBLE, &fChaSavePosY, 0, &cbChaSavePosY);
			::SQLGetData(spConn->hStmt, 15, SQL_C_DOUBLE, &fChaSavePosZ, 0, &cbChaSavePosZ);
			::SQLGetData(spConn->hStmt, 16, SQL_C_LONG, &nChaSchool, 0, &cbChaSchool);
            ::SQLGetData(spConn->hStmt, 17, SQL_C_LONG, &nGuNum,		0, &cbGuNum);
            ::SQLGetData(spConn->hStmt, 18, SQL_C_TYPE_TIMESTAMP, &sChaGuSecede, 0, &cbChaGuSecede);

			pChaData->m_UserDbNum = (DWORD) nUserNum;
			pChaData->m_dwServerID = (DWORD) nSGNum;

			if (cbChaName != 0 && cbChaName != -1) 				
				pChaData->SetChaName((const char*) szChaName);
            
            pChaData->m_wLevel = (WORD) nChaLevel;
			pChaData->m_emClass = EMCHARCLASS(nChaClass);
			pChaData->m_wLevel	= (WORD)nChaLevel;
			pChaData->m_nBright			= nChaBright;

			pChaData->m_sStartMapID.dwID	= (DWORD) nChaStartMap;

			pChaData->m_dwStartGate		= (DWORD) nChaStartGate;
			pChaData->m_vStartPos.x		= (float) fChaPosX;
			pChaData->m_vStartPos.y		= (float) fChaPosY;
			pChaData->m_vStartPos.z		= (float) fChaPosZ;

			pChaData->m_sSaveMapID.dwID = (DWORD) nChaSaveMap;
			pChaData->m_vSavePos.x		= (float) fChaSavePosX; 
			pChaData->m_vSavePos.y		= (float) fChaSavePosY; 
			pChaData->m_vSavePos.z		= (float) fChaSavePosZ;

			pChaData->m_wSchool         = (WORD) nChaSchool;
            pChaData->m_ClubDbNum         = (DWORD) nGuNum;

            // 클럽탈퇴시간
            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sChaGuSecede.year <= 1970 || sChaGuSecede.year >= 2999)
			{
				sChaGuSecede.year = 1970;
				sChaGuSecede.month = 2;
				sChaGuSecede.day = 1;
				sChaGuSecede.hour = 1;
				sChaGuSecede.minute = 1;
				sChaGuSecede.second = 1;
			}

            CTime cTemp(
                sChaGuSecede.year, sChaGuSecede.month,  sChaGuSecede.day, 
                sChaGuSecede.hour, sChaGuSecede.minute, sChaGuSecede.second);
            pChaData->m_tSECEDE = cTemp.GetTime();
		}
		else
		{
			break;
		}
	}
	
	return sc::db::DB_OK;
*/
}

// 2차비번
int COdbcManager::Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass)
{
    sc::writeLogError("COdbcManager::Certification2ndPassword. Use ADO");
    return sc::db::DB_ERROR;
}

// 2차비번
int COdbcManager::Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass)
{
    sc::writeLogError("COdbcManager::Update2ndPassword. Use ADO");
    return sc::db::DB_ERROR;
}
/////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 해당 사용자의 캐릭터 정보를 가져온다.	
int COdbcManager::GetChaAllInfo(int nUsrNum)
{	
	return sc::db::DB_OK;
}

int COdbcManager::GetChaBAInfo(int UserNum, int SvrGrp, std::vector<int>& vChaNum)
{
    sc::writeLogError("COdbcManager::GetChaBAInfo. Use ADO");
    return sc::db::DB_ERROR;
}

//! 학교vs학교 파티대련결과저장
int	COdbcManager::LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost)
{
    sc::writeLogError("COdbcManager::LogPartyMatch. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call InsertPartyMatch(%d,%d,%u,%u)}", nSGNum, nSvrNum, wWin, wLost );
	int nReturn = m_pGameDB->ExecuteSp(szTemp);
	return nReturn;
*/
}

int COdbcManager::SetChaExp(int nChaNum, LONGLONG llExp)
{
    sc::writeLogError("COdbcManager::SetChaExp. Use ADO");
    return sc::db::DB_ERROR;
/*
    if (nChaNum < 1 || llExp < 0)
        return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaExp(%I64d,%d,?)}", llExp, nChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

    return nReturn;
*/
}

LONGLONG COdbcManager::GetChaExp(int nChaNum)
{
    sc::writeLogError("COdbcManager::GetChaExp. Use ADO");
    return sc::db::DB_ERROR;
/*
    if (nChaNum < 1)
        return sc::db::DB_ERROR;

    SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT ChaExp From dbo.ChaInfo "
							"WHERE ChaNum=%d", nChaNum );
	
	sReturn = ::SQLExecDirect(spConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}

	LONGLONG   llChaExp = 0;
    SQLINTEGER cbChaExp = SQL_NTS;
	
	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(spConn->hStmt));
            
			return sc::db::DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data
            ::SQLGetData(spConn->hStmt, 1, SQL_C_SBIGINT, &llChaExp, 0, &cbChaExp);
		}
		else
		{
			break;
		}
	}

	return llChaExp;
*/
}

int COdbcManager::SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos)
{
    sc::writeLogError("COdbcManager::SetLastCallPos. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaLastCallPos(%u,%f,%f,%f,%d,?)}", dwMapID, vPos.x, vPos.y, vPos.z, nChaNum );
	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
    return nReturn;
*/
}

//! 캐릭터를 오프라인 상태로 만든다.
int COdbcManager::SetCharacterOffline(int nChaNum)
{
    sc::writeLogError("COdbcManager::SetCharacterOffline. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nChaNum < 0)
        return NET_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaOnline(%d,0)}", nChaNum );

	return m_pGameDB->ExecuteSp(szTemp);
*/
}

//! DB에서 캐릭터를 온라인 상태로 만든다.
int COdbcManager::SetCharacterOnline(int nChaNum)
{
    sc::writeLogError("COdbcManager::SetCharacterOnline. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nChaNum < 0)
        return NET_ERROR;
	
    std::string Query(
        sc::string::format(
            "{call UpdateChaOnline(%1%,1)}", nChaNum));
	return m_pGameDB->ExecuteSp(Query);
*/
}

int COdbcManager::SetAllCharacterOffline()
{
    sc::writeLogError("COdbcManager::SetAllCharacterOffline. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query("{call UpdateAllCharacterOffline}");
	return m_pGameDB->ExecuteSp(Query);
*/
}

int COdbcManager::SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable)
{
    sc::writeLogError("COdbcManager::SetChaPoint. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_SetChaPoint(%1%, %2%, %3%)}",
            ChaDbNum,
            PointRefundable,
            PointNotRefundable));
    return m_pGameDB->ExecuteSp(Query);
*/
}

int COdbcManager::TransDBDataToWorldBattle(SCHARDATA2* pCharData2, DWORD dwChaNum)
{
    sc::writeLogError("COdbcManager::TransDBDataToWorldBattle. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp )
{
    sc::writeLogError("COdbcManager::CalculateExpCompressor. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::GetItemTransferStatus( DWORD UserNum, int& Result )
{
	std::tr1::shared_ptr< db::ODBC_STMT > spConn = m_pGameDB->GetConnection();
	if ( !spConn )
		return sc::db::DB_ERROR;

	std::string Query( sc::string::format( "EXEC sp_GetItemTransferStatus %1%", UserNum ) );
	SQLRETURN sReturn = ::SQLExecDirect( spConn->hStmt, ( SQLCHAR* ) Query.c_str(), SQL_NTS );
	if ( sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO )
	{
		Print( Query );
		Print( GetErrorString( spConn->hStmt ) );
		return sc::db::DB_ERROR;
	}

	SQLINTEGER nResult = 0, cbResult=SQL_NTS;

	while ( true )
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if ( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print( Query );
			Print( GetErrorString( spConn->hStmt ) );
			return sc::db::DB_ERROR;
		}

		if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &nResult, 0, &cbResult );
			Result = nResult;
		}
		else
		{
			break;
		}
	}

	return sc::db::DB_OK;
}
