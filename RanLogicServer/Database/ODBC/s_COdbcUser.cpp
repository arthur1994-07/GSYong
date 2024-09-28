#include "../../pch.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

#include "../../../SigmaCore/DebugInclude.h"


__time64_t COdbcManager::GetChatBlockTime(int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLRETURN sReturn=0;

	TCHAR szTemp[128] = {0};
	
	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.DaumUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_JAPAN:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.JapanUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.GspUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.GSUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.GMUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.USUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	default:
		_snprintf( szTemp, 128, "SELECT ChatBlockDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;
	}

    TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    __time64_t tChatBlockDate = 0;

    sReturn = ::SQLExecDirect(spConn->hStmt,
							  (SQLCHAR*) szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(spConn->hStmt));
	
        return sc::db::DB_ERROR;
	}

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
            ::SQLGetData(spConn->hStmt, 1, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

            // 프리미엄 만료기간
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
			{
				sChatBlockDate.year = 1970;
				sChatBlockDate.month = 2;
				sChatBlockDate.day = 1;
				sChatBlockDate.hour = 1;
				sChatBlockDate.minute = 1;
				sChatBlockDate.second = 1;
			}
            CTime cTemp(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day, 
                        sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
            tChatBlockDate = cTemp.GetTime();
        }
		else
		{
			break;
		}		
	}
	
    return tChatBlockDate;
}

int COdbcManager::SetChatBlockTime(int nUserNum, __time64_t tBlockTime)
{
	if (nUserNum <= 0) return sc::db::DB_ERROR;
	
	CTime cTemp(tBlockTime);
	CString strTime = cTemp.Format(_T("%Y-%m-%d %H:%M:%S"));	

	TCHAR szTemp[256] = {0};
	
	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 256, "UPDATE DaumUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET ChatBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

	return nReturn;
}


///////////////////////////////////////////////////////////////////////////////
// 해당 사용자의 계정 생성 날짜를 가져온다.
__time64_t COdbcManager::GetCreateDate(const int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLRETURN sReturn=0;
	
	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.DaumUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_JAPAN:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.JapanUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_THAILAND:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.GspUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.GSUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.GMUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.USUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	default:
		_snprintf( szTemp, 128, "SELECT CreateDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;
	}

	TIMESTAMP_STRUCT sCreateDate; SQLINTEGER cbCreateDate = SQL_NTS;
	__time64_t tCreateDate = 0;

	sReturn = ::SQLExecDirect(spConn->hStmt,
		(SQLCHAR*) szTemp, 
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}

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
			::SQLGetData(spConn->hStmt, 1, SQL_C_TYPE_TIMESTAMP, &sCreateDate, 0, &cbCreateDate);

			// 계정 생성기간
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sCreateDate.year <= 1970 || sCreateDate.year >= 2999)
			{
				sCreateDate.year = 1970;
				sCreateDate.month = 2;
				sCreateDate.day = 1;
				sCreateDate.hour = 1;
				sCreateDate.minute = 1;
				sCreateDate.second = 1;
			}
			CTime cTemp(sCreateDate.year, sCreateDate.month,  sCreateDate.day, 
				sCreateDate.hour, sCreateDate.minute, sCreateDate.second);
			tCreateDate = cTemp.GetTime();
		}
		else
		{
			break;
		}		
	}

	return tCreateDate;
}

///////////////////////////////////////////////////////////////////////////////
// 해당 사용자의 프리미엄 기간을 가져온다.
__time64_t COdbcManager::GetPremiumTime(int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLRETURN sReturn=0;
	
	TCHAR szTemp[128] = {0};
    
	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.DaumUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.TerraUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.JapanUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.GspUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.GSUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.GMUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.USUserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "SELECT PremiumDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d", nUserNum );
		break;
	}

    TIMESTAMP_STRUCT sPremiumDate; SQLINTEGER cbPremiumDate = SQL_NTS;
    __time64_t tPremiumDate = 0;

    sReturn = ::SQLExecDirect(spConn->hStmt,
							(SQLCHAR*) szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(spConn->hStmt));
		
        return sc::db::DB_ERROR;
	}

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
            ::SQLGetData(spConn->hStmt, 1, SQL_C_TYPE_TIMESTAMP, &sPremiumDate, 0, &cbPremiumDate);

            // 프리미엄 만료기간
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
			{
				sPremiumDate.year = 1970;
				sPremiumDate.month = 2;
				sPremiumDate.day = 1;
				sPremiumDate.hour = 1;
				sPremiumDate.minute = 1;
				sPremiumDate.second = 1;
			}
            CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                        sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
            tPremiumDate = cTemp.GetTime();
        }
		else
		{
			break;
		}		
	}
	
    return tPremiumDate;
}

/**
 * 해당 사용자의 프리미엄 기간을 세팅한다.
 */
int COdbcManager::SetPremiumTime(int nUserNum, __time64_t tPremiumTime)
{
	if (nUserNum <= 0) return sc::db::DB_ERROR;	

	CTime cTemp(tPremiumTime);
	CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");
	
	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:	
		_snprintf( szTemp, 256, "UPDATE DaumUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET PremiumDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

	return nReturn;
}

int COdbcManager::GetUserType(int nUserNum)
{
    sc::writeLogError("COdbcManager::GetUserType. Use ADO");
    return 0;
/*
	if (nUserNum <= 0)
		return 0;	

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call dbo.daum_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call dbo.Japan_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call dbo.terra_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call dbo.terra_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call dbo.user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call dbo.gsp_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call dbo.GS_user_gettype(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call dbo.user_gettype(%d, ?)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call dbo.user_gettype(%d, ?)}", nUserNum );
		break;
	}
	
	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);	

	return nReturn;
*/
}



int COdbcManager::GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo )
{
	SQLRETURN	sReturn = 0;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if ( NULL == spConn )
		return sc::db::DB_ERROR;

	LONGLONG wContinent = 0; SQLINTEGER cbContinent = SQL_NTS;
	LONGLONG wCountry = 0; SQLINTEGER cbCountry = SQL_NTS;

	std::string Query(
		sc::string::format(
		"Exec sp_SelectUserCountry %d", nUserNum ) );
	sReturn = ::SQLExecDirect( spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS );
	if ( ( sReturn != SQL_SUCCESS ) &&
		( sReturn != SQL_SUCCESS_WITH_INFO ) ) 
	{		
		Print( Query.c_str() );
		Print( GetErrorString(spConn->hStmt) );
		return sc::db::DB_ERROR;
	}

	while ( true )
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if ( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print( Query.c_str() );
			Print( GetErrorString( spConn->hStmt ) );
			return sc::db::DB_ERROR;
		}

		if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_SBIGINT, &wContinent, 0, &cbContinent );
			::SQLGetData( spConn->hStmt, 2, SQL_C_SBIGINT, &wCountry, 0, &cbCountry );
			sCountryInfo.wContinentID = wContinent;
			sCountryInfo.wCountryID = wCountry;
		}
		else
		{
			break;
		}
	}

	return sReturn;
}

/**
 * 해당사용자의 만들 수 있는 캐릭터 갯수를 가져온다.
 */
int COdbcManager::GetUserChaRemain(int nUserNum)
{
	if (nUserNum <= 0)
		return 0;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call dbo.daum_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call dbo.Japan_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call dbo.terra_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call dbo.terra_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call dbo.user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "{call dbo.gsp_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call dbo.GS_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call dbo.user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "{call dbo.GM_user_cha_remain(%d, ?)}", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "{call dbo.US_user_cha_remain(%d, ?)}", nUserNum );
		break;

	default:
		_snprintf( szTemp, 128, "{call dbo.user_cha_remain(%d, ?)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	
    // 캐릭터생성갯수 에러
    if (nReturn < 0 || nReturn > MAX_CHAR_LENGTH)
    {
        nReturn = 0;
    }

	return nReturn;
}


/**
 * Test 서버에서 만들 수 있는 캐릭터 갯수
 */
int	COdbcManager::GetUserChaTestRemain(int nUserNum)
{
	if (nUserNum <= 0)
		return 0;
	
	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call daum_user_cha_test_remain(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_user_cha_test_remain(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call terra_user_cha_test_remain(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call terra_user_cha_test_remain(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "{call gsp_user_cha_test_remain(%d, ?)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call user_cha_test_remain(%d, ?)}", nUserNum );
		break;
	}

	int nReturn=0;
	
	nReturn = m_pUserDB->ExecuteSpInt(szTemp);	

    // 캐릭터생성갯수 에러
    if (nReturn < 0 || nReturn > MAX_CHAR_LENGTH)
    {
        nReturn = 0;
    }

	return nReturn;
}


///////////////////////////////////////////////////////////////////////////
// 해당사용자를 Logout 한다
int	COdbcManager::UserLogout(
	const TCHAR* szUsrID, 
	const TCHAR* szUsrIP, 
	int nUsrNum, 
	int nGameTime,
	int nChaNum,
	int nSvrGrp,
	int nSvrNum, 
	int nTotalGameTime,
	int nOfflineTime )
{	
	if (nGameTime == 0)	nGameTime = 1;	

	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:
		_snprintf( szTemp, 256, "{call dbo.daum_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_CHINA:
		_snprintf( szTemp, 256, "{call dbo.user_logout('%s',%d,%d,%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum, nTotalGameTime, nOfflineTime );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "{call dbo.Japan_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 256, "{call dbo.terra_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 256, "{call dbo.terra_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "{call dbo.user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 256, "{call dbo.gsp_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_GS:
		_snprintf( szTemp, 256, "{call dbo.GS_user_logout('%s',%d,%d,%d,%d,%d,'%s')}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum, szUsrIP );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "{call dbo.user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );		
		break;

	case SP_VIETNAM:
		_snprintf( szTemp, 256, "{call dbo.terra_user_logout('%s',%d,%d,%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum, nTotalGameTime, nOfflineTime );
		break;

	case SP_EU:
		_snprintf( szTemp, 256, "{call dbo.GM_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	case SP_US:
		_snprintf( szTemp, 256, "{call dbo.US_user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );
		break;

	default:  
		_snprintf( szTemp, 256, "{call dbo.user_logout('%s',%d,%d,%d,%d,%d)}", szUsrID, nUsrNum, nGameTime, nChaNum, nSvrGrp, nSvrNum );		
		break;
	}

/*
#if defined(TH_PARAM) || defined(_RELEASED)	|| defined(MY_PARAM) || defined(MYE_PARAM) 
	{
		// 말레이시아/태국에 남은 유저수를 저장한다.
		NET_CHA_BBA_INFO_DH msg;
		msg.nmg.nType = NET_MSG_CHA_BAINFO_AC;
		GetChaBAInfo(nUsrNum, nSvrGrp, &msg);
		SetUserTempInfo(nUsrNum, msg.m_ChaServerTotalNum );
	}
#endif
*/
	int nReturn = m_pUserDB->ExecuteSp(szTemp);

	return nReturn;
}

int COdbcManager::UserLogoutSimple(const TCHAR* szUsrID)
{
	if (szUsrID == NULL)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call dbo.Daum_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call dbo.Japan_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "{call dbo.Terra_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 128, "{call dbo.Terra_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_THAILAND:
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "{call dbo.Gsp_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "{call dbo.GS_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple('%s')}", szUsrID );
		break;

    case SP_WORLD_BATTLE:
        sc::writeLogError(std::string("Do not call this function COdbcManager::UserLogoutSimple. use ADO functions."));
        break;

	case SP_EU:
		_snprintf( szTemp, 128, "{call dbo.GM_UserLogoutSimple('%s')}", szUsrID );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "{call dbo.US_UserLogoutSimple('%s')}", szUsrID );
		break;

	default:
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple('%s')}", szUsrID );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSp(szTemp);
	return nReturn;
}

/**
 * 사용자 번호를 이용해서 사용자를 로그아웃 시킨다
 */
int COdbcManager::UserLogoutSimple2(int nUserNum)
{	
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call dbo.Daum_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call dbo.Japan_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "{call dbo.Terra_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call dbo.Terra_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call dbo.Gsp_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call dbo.GS_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "{call dbo.GM_UserLogoutSimple2(%d)}", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "{call dbo.US_UserLogoutSimple2(%d)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call dbo.UserLogoutSimple2(%d)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSp(szTemp);	

	return nReturn;
}


/**
* \n 해당서버의 사용자가 있을때만 로그아웃 시킨다
* \param szUsrID 사용자 ID
* \param nSvrGrp 서버 그룹
* \return 
*/
int	COdbcManager::UserLogoutSimple3( const TCHAR* szUsrID, int nSvrGrp )
{
	if (szUsrID == NULL)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:
		_snprintf( szTemp, 128, "{call Daum_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "{call Terra_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 128, "{call Terra_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_THAILAND:
		_snprintf( szTemp, 128, "{call UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "{call Gsp_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "{call GS_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "{call GM_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "{call US_UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;

	default:
		_snprintf( szTemp, 128, "{call UserLogoutSimple3('%s',%d)}", szUsrID, nSvrGrp );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSp(szTemp);	

	return nReturn;
}


// 해당 게임서버의 전체 유저를 로그아웃 시킨다.
int	COdbcManager::UserLogoutSvr(int nSvrGrp, 
								int nSvrNum)
{
	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:
		_snprintf( szTemp, 256, "UPDATE DaumUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_THAILAND:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d AND SvrNum=%d", nSvrGrp, nSvrNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

    return nReturn;
}


int COdbcManager::UserUpdateCha( int nUsrNum, const TCHAR* szChaName )
{
	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call Daum_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call Gsp_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "{call GS_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "{call GM_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "{call US_UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;

	default:
		_snprintf( szTemp, 128, "{call dbo.UpdateChaName(%d,'%s')}", nUsrNum, szChaName );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSp(szTemp);	

	return nReturn;
}


/**
* 해당 id 사용자의 사용자 번호를 가져온다.
*/
int COdbcManager::GetUserNum( const TCHAR* szUsrId )
{
	if (szUsrId == NULL)
		return sc::db::DB_ERROR;

	SQLRETURN	sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();

	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER nUserNum=0, cbUserNum=SQL_NTS;	

	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.DaumUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.JapanUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.TerraUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.TerraUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.UserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.GspUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.GSUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.UserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.GMUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.USUserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;

	default:
		_snprintf( szTemp, 256, "SELECT UserNum FROM dbo.UserInfo WITH (NOLOCK)  WHERE UserUID='%s'", szUsrId );
		break;
	}

	sReturn = ::SQLExecDirect(spConn->hStmt, 
							  (SQLCHAR*) szTemp, 
							  SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(spConn->hStmt));
		
		return sc::db::DB_ERROR;
	}
	else
	{
		while(true)
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
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nUserNum, 0, &cbUserNum);				
			}
			else
			{
				break;
			}
		}
	}
	
	return nUserNum;
}


/*
int COdbcManager::AddBlockIPList( TCHAR* szIP, TCHAR* szReason )
{	
	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "INSERT INTO BlockAddress (BlockAddress, BlockReason) VALUES('%s', '%s')", szIP, szReason ); 
	return m_pUserDB->ExecuteSQL(szTemp);
}
*/


/**
 * 캐릭터 생성, 갯수 감소
 */
int	COdbcManager::UserChaNumDecrease(int nUserNum)
{
	if (nUserNum <= 0) return sc::db::DB_ERROR;	

	std::strstream strTemp;
	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:	
		_snprintf( szTemp, 128, "{call Daum_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call Gsp_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call GS_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "{call GM_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "{call US_UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call UpdateChaNumDecrease(%d, ?)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);	

	return nReturn;
}


/*
 * TEST 서버 캐릭터 생성갯수 감소
 */
int	COdbcManager::UserTestChaNumDecrease(int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call Daum_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call Terra_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call Terra_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call Gsp_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call GS_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "{call GM_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "{call US_UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call UpdateTestChaNumDecrease(%d, ?)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);	

	return nReturn;
}


/**
 * 캐릭터 삭제, 캐릭터생성 갯수 증가
 */
int COdbcManager::UserChaNumIncrease(int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 128, "{call Daum_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 128, "{call Terra_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "{call Gsp_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "{call GS_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "{call GM_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "{call US_UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;

	default:
		_snprintf( szTemp, 128, "{call UpdateChaNumIncrease(%d, ?)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);	

	return nReturn;
}


/**
 * TEST 서버 캐릭터 삭제, 갯수 증가
 */
int COdbcManager::UserTestChaNumIncrease(int nUserNum)
{
	if (nUserNum <= 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:
		_snprintf( szTemp, 128, "{call Daum_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "{call Japan_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_PHILIPPINES:
		_snprintf( szTemp, 128, "{call Terra_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 128, "{call Terra_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 128, "{call UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 128, "{call Gsp_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 128, "{call GS_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "{call UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 128, "{call GM_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 128, "{call US_UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;

	default:		
		_snprintf( szTemp, 128, "{call UpdateTestChaNumIncrease(%d, ?)}", nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);

	return nReturn;
}


/**
* 해당사용자를 블록시킨다.
* \param nUserNum 사용자번호
* \param nHour 시간
* \return 
*/
int COdbcManager::UserBlockDate(
	int nUserNum,
	__time64_t tBlockTime )
{
    if ( nUserNum < 0 || 
		 tBlockTime == 0 )
    {
        Print(_T("UserBlockDate:nUserNum or nHour is incorrect"));
        return sc::db::DB_ERROR;
    }

	CTime cTemp( tBlockTime );
	CString strTime = cTemp.Format(_T("%Y-%m-%d %H:%M:%S"));

	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 256, "UPDATE DaumUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

	return nReturn;
}


/**
 * 해당사용자를 블록시킨다.
 * 입력
 * nUserNum : 사용자번호
 * nDay : 블럭일수 1 - 15 까지
 */
int COdbcManager::UserBlock(
	int nUserNum,
	int nDay )
{
    if (nUserNum < 0 || nDay < 0 || nDay > 15)
    {
        Print(_T("UserBlock:nUserNum or nDay is incorrect"));
        return sc::db::DB_ERROR;
    } // if (nUserNum < 0 || nDay < 0 || nDay > 15)	
	
	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 256, "UPDATE dbo.DaumUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserBlock=1,UserBlockDate=getdate()+%d WHERE UserNum=%d", nDay, nUserNum );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

	return nReturn;
}


/**
 * 해당 게임서버의 전체 유저를 로그아웃 시킨다.
 */
int	COdbcManager::AllServerUserLogout(int nSvrGrp)
{	
	TCHAR szTemp[256] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:		
		_snprintf( szTemp, 256, "UPDATE dbo.DaumUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_JAPAN:		
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 256, "UPDATE dbo.JapanUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_MALAYSIA:		
		_snprintf( szTemp, 256, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_THAILAND:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_GLOBAL:		
		_snprintf( szTemp, 256, "UPDATE dbo.GspUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_GS:		
		_snprintf( szTemp, 256, "UPDATE dbo.GSUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_EU:		
		_snprintf( szTemp, 256, "UPDATE dbo.GMUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	case SP_US:		
		_snprintf( szTemp, 256, "UPDATE dbo.USUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;

	default:		
		_snprintf( szTemp, 256, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1 AND SGNum=%d", nSvrGrp );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

    return nReturn;
}

int	COdbcManager::AllUserLogout()
{	
	TCHAR szTemp[128] = {0};

	switch (m_ServiceProvider)
	{
	case SP_KOREA:	
		_snprintf( szTemp, 128, "UPDATE dbo.DaumUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_JAPAN:
		// 일본 Excite에서 Gonzo로 서비스사 변경에 따른 변경
		_snprintf( szTemp, 128, "UPDATE dbo.JapanUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_PHILIPPINES:		
		_snprintf( szTemp, 128, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_MALAYSIA:
		_snprintf( szTemp, 128, "UPDATE dbo.TerraUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_THAILAND:
		_snprintf( szTemp, 128, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_GLOBAL:
		_snprintf( szTemp, 128, "UPDATE dbo.GspUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_GS:
		_snprintf( szTemp, 128, "UPDATE dbo.GSUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_INDONESIA:
		_snprintf( szTemp, 128, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_EU:
		_snprintf( szTemp, 128, "UPDATE dbo.GMUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	case SP_US:
		_snprintf( szTemp, 128, "UPDATE dbo.USUserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;

	default:		
		_snprintf( szTemp, 128, "UPDATE dbo.UserInfo WITH (UPDLOCK) SET UserLoginState=0 WHERE UserLoginState=1" );
		break;
	}

	int nReturn = m_pUserDB->ExecuteSQL(szTemp);	

    return nReturn;
}

int COdbcManager::UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName)
{
    std::string ItemNameTemp(ItemName);
    sc::string::replaceForDb(ItemNameTemp);
    char szItemName[100] = {0};
    size_t NameLen = ItemNameTemp.length();
    StringCchCopy(szItemName, 100, ItemNameTemp.c_str());

    std::string Query(
        sc::string::format(
            "{call dbo.LogItemDataInsertUpdate(%1%, %2%, '%3%')}",
            ItemMid,
            ItemSid,
            szItemName));
    return m_pUserDB->ExecuteSp(Query);
}
