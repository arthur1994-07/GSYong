#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/*
int	COdbcManager::LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum)
{
    std::string Query(
        sc::string::format(
            "{call sp_lottery_check('%s','%s','%s',%d,?)}", 
            strLottery.GetString(),
            strIP.GetString(),
            strUserUID.GetString(),
            nUserNum));
	return m_pLogDB->ExecuteSpInt(Query);
}
*/
// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
/*
int COdbcManager::LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize)
{
    std::string Query(
        sc::string::format(
            "{call sp_LogPungPung('%s','%s',%d,%d)}", 
            strIP.GetString(),
            strUserUID.GetString(),
            nUserNum,
            nPrize));
	return m_pLogDB->ExecuteSp(Query);
}
*/
/*
int COdbcManager::LogPungPungCheck(CString strIP, int nUserNum)
{
    std::string Query(
        sc::string::format(
            "{call sp_PungPungCheck('%s',%d,?)}", 
            strIP.GetString(),
            nUserNum));
	return m_pLogDB->ExecuteSpInt(Query);
}
*/

int COdbcManager::LogUserAttend(int nUserNum, std::vector<USER_ATTEND_INFO>& vecUserAttend)
{
    sc::writeLogError("COdbcManager::LogUserAttend. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pShopDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "Exec AttendLog_GetList %d", nUserNum));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pShopDB->FreeConnection(spConn);	
		return sc::db::DB_ERROR;
	}
	
	TIMESTAMP_STRUCT sAttendTime;   SQLINTEGER cbAttendTime   = SQL_NTS;
	SQLINTEGER nCount = 0, cbCount=SQL_NTS;
	SQLINTEGER nAttendReward = 0, cbAttendReward=SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pShopDB->FreeConnection(spConn);			
            return sc::db::DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			USER_ATTEND_INFO sAttend;

			// Bind data
			::SQLGetData(spConn->hStmt, 1, SQL_C_TYPE_TIMESTAMP, &sAttendTime, 0, &cbAttendTime);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nCount, 0, &cbCount);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nAttendReward, 0, &cbAttendReward);
			
			// 유저 출석한 로그
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAttendTime.year <= 1970 || sAttendTime.year >= 2999)
			{
				sAttendTime.year = 1970;
				sAttendTime.month = 2;
				sAttendTime.day = 1;
				sAttendTime.hour = 1;
				sAttendTime.minute = 1;
				sAttendTime.second = 1;
			}
            CTime cTemp(sAttendTime.year, sAttendTime.month,  sAttendTime.day,
                        sAttendTime.hour, sAttendTime.minute, sAttendTime.second);

            sAttend.tAttendTime = cTemp.GetTime();
			sAttend.nComboAttend = nCount;
			sAttend.bAttendReward = (nAttendReward == 1) ? true : false;

			vecUserAttend.push_back ( sAttend );
		}
		else
		{
			break;
		}		
	}

	//m_pShopDB->FreeConnection(spConn);

	return sc::db::DB_OK;
*/
}

int COdbcManager::InsertUserAttend( int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount, int nAttendReward, int nItemMID, int nItemSID )
{
    sc::writeLogError("COdbcManager::InsertUserAttend. Use ADO");
    return sc::db::DB_ERROR;
/*
	if ( nUserNum < 0 )
	{
		Print(_T("COdbcManager::InsertUserAttend"));
		return sc::db::DB_ERROR;
	}

	CTime cTemp(tAttendTime);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    std::string Query(
        sc::string::format(
            "{call AttendLog_Create( %d,'%s','%s',%d,%d,%d,%d,?)}",
            nUserNum,
            strUserName.GetString(), 
            strTime.GetString(),
            nCount,
            nAttendReward,
            nItemMID,
            nItemSID));

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pShopDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLRETURN  sReturn=0;
	SQLINTEGER nOutput=0;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	// Bind the parameter.
	sReturn = ::SQLBindParameter(
		spConn->hStmt, 
		1,
		SQL_PARAM_OUTPUT,
		SQL_C_SLONG,
		SQL_INTEGER,
		0,
		0,
		&nOutput,
		0,
		&nLenBuffer );

	if ( sReturn == SQL_ERROR || sReturn == SQL_INVALID_HANDLE )
	{		
		Print(Query);
		Print("SQLBindParameter");
		//m_pShopDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	sReturn = ::SQLExecute( spConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pShopDB->FreeConnection(spConn);
		sReturn = sc::db::DB_ERROR;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	else
	{
		//m_pShopDB->FreeConnection( spConn );
	}

	if (nOutput != sc::db::DB_OK) 
	{        
		if (nOutput == sc::db::DB_ERROR)
		{
            Print(
                sc::string::format(
                    "DB:InsertUserAttend - Insert Log Attend %d %s %s %d %d %d %d",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(),
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));
		}
		else if (nOutput == sc::db::DB_INSERT_ITEM)
		{
			Print(
                sc::string::format(
                    "DB:InsertUserAttend - Insert Item %d %s %s %d %d %hd %hd",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(),
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));			
		}
		else if ( nOutput == sc::db::DB_NOT_ITEM )
		{
			Print(
                sc::string::format(
                    "DB:InsertUserAttend - No ShopItemMap %d %s %s %d %d %hd %hd",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(), 
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));
        }
	}	


	return sReturn;
*/
}


int COdbcManager::InsertShopItemMap( int nItemMID, int nItemSID )
{
    sc::writeLogError("COdbcManager::InsertShopItemMap. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call ShopItemMap_Exists(%d,%d,?)}",
		    nItemMID,
		    nItemSID));
	return m_pShopDB->ExecuteSpInt(Query);
*/
}
