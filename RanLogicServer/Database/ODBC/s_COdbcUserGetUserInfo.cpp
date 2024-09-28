#include "../../pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"

#include "../../../RanLogic/Msg/LoginMsg.h"

#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 대만/홍콩/중국
USER_INFO_BASIC COdbcManager::GetUserInfo(const TCHAR* szUsrId, const TCHAR* szPasswd)
{
	USER_INFO_BASIC uib;
	uib.UserDbNum = 0;
	
    std::string Query("SELECT UserNum, UserID, LastLoginDate FROM dbo.UserInfo WITH (NOLOCK) WHERE UserID = '%s' AND UserPass ='%s'");
    if (szUsrId)
    {
		::StringCchCopy(uib.szUserID, GSP_USERID, szUsrId);
        Query = sc::string::format(Query, szUsrId, szPasswd);
    }
    else
    {
        Query = sc::string::format(Query, "", szPasswd);
    }

	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return uib;

	SQLINTEGER nUserNum=0, cbUserNum=SQL_NTS;
	sReturn = ::SQLExecDirect(
        spConn->hStmt, 
		(SQLCHAR*) Query.c_str(), 
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	else
	{
		while(true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{				
				break;
			}

 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nUserNum, 0, &cbUserNum);
				uib.UserDbNum = nUserNum;
			}
			else
			{
				break;		
			}
		}

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

	return uib;
}

/**
* 사용자 정보를 가져온다
* Thailand
* \param szUserID 
* \param pMsg 
* \return 
*/
int COdbcManager::ThaiGetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	SQLRETURN	sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
	SQLINTEGER nUserThaiFlag  = 0, cbUserThaiFlag  = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
    std::string Query(
        sc::string::format(
            "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, UserThaiFlag FROM dbo.UserInfo WHERE UserID='%s'",
            szUserID));

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);
				::SQLGetData(spConn->hStmt, 7, SQL_C_LONG,           &nUserThaiFlag,  0, &cbUserThaiFlag);

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;
				pMsg->wThaiFlag      = (WORD) nUserThaiFlag; // Thailand 사용자 타입 플래그

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
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
    std::string Query2(
        sc::string::format(
		"Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());		
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	//m_pGameDB->FreeConnection(spConn);

    return sc::db::DB_OK;
}

/**
* 사용자 정보를 가져와서 Agent 로 전송
*/
int COdbcManager::GetUserInfo(
	const TCHAR* szUserID,
	NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
	//SQLINTEGER nUserContinent = 0, cbUserContinent = SQL_NTS;
	//SQLINTEGER nUserCountry	  = 0, cbUserCountry   = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	   
	
    std::string UserId(szUserID);
    sc::string::SqlInjectionCheck(UserId);

	TIMESTAMP_STRUCT sLastLoginTime;
    SQLINTEGER cbLastLoginTime = SQL_NTS;	
	
	std::string Query;
	SQLINTEGER nChinaGameTime = 0,		cbChinaGameTime = SQL_NTS;
	SQLINTEGER nChinaOfflineTime = 0,	cbChinaOfflineTime = SQL_NTS;
	SQLINTEGER nChinaUserAge = 0,		cbChinaUserAge = SQL_NTS;


#ifdef _RELEASED

	Query =
		sc::string::format(
		"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, LastLoginDate FROM UserInfo WHERE UserID='%s'",
		UserId.c_str());

//#elif defined( CH_PARAM )    
#else
	if (m_ServiceProvider == SP_CHINA)
	{
		// 중국 GameTime Check를 위해서 추가
		Query = 
			sc::string::format(
				"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, GameTime, OfflineTime, LastLoginDate, UserAge FROM dbo.UserInfo WHERE UserID='%s'",
				UserId.c_str());
	}
	else
	{
		Query = 
			sc::string::format(
				"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.UserInfo WHERE UserID='%s'",
				UserId.c_str());
	}
#endif

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
//#if defined( CH_PARAM )
		if (m_ServiceProvider == SP_CHINA)
		{
			while(true)
			{
				sReturn = ::SQLFetch(spConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					Print(Query.c_str());
					Print(GetErrorString(spConn->hStmt));
					//m_pUserDB->FreeConnection(spConn);

					//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					return sc::db::DB_ERROR;
				}
 				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
					::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
					::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
					::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
					::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
					::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

					::SQLGetData(spConn->hStmt, 7, SQL_C_LONG,           &nChinaGameTime, 0, &cbChinaGameTime);
					::SQLGetData(spConn->hStmt, 8, SQL_C_LONG,           &nChinaOfflineTime, 0, &cbChinaOfflineTime);
					::SQLGetData(spConn->hStmt, 9, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);
					::SQLGetData(spConn->hStmt, 10, SQL_C_LONG,           &nChinaUserAge, 0, &cbChinaUserAge);

					pMsg->nUserNum       = nUserNum;
					pMsg->nUserType      = nUserType;
					pMsg->uChaRemain     = (USHORT) nChaRemain;
					pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
					pMsg->tPremiumTime   = cTemp.GetTime();

					// 채팅블록시간				
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
					CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
								 sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
					pMsg->tChatBlockTime = cTemp2.GetTime();

					pMsg->nChinaGameTime	= nChinaGameTime;
					pMsg->nChinaOfflineTime	= nChinaOfflineTime;

					CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
								 sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
					pMsg->tLastLoginTime = cTemp3.GetTime();
					
					pMsg->nChinaUserAge = nChinaUserAge;
				}
				else
				{
					break;
				}
			}
		}
		else
		{ 
//#else
			while(true)
			{
				sReturn = ::SQLFetch(spConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					Print(Query.c_str());
					Print(GetErrorString(spConn->hStmt));
					//m_pUserDB->FreeConnection(spConn);
					//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					return sc::db::DB_ERROR;
				}
 				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
					::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
					::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
					::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
					::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
					::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);
	#if defined(_RELEASED)
					::SQLGetData(spConn->hStmt, 7, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);
	#endif

					pMsg->nUserNum       = nUserNum;
					pMsg->nUserType      = nUserType;
					pMsg->uChaRemain     = (USHORT) nChaRemain;
					pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
					pMsg->tPremiumTime = cTemp.GetTime();

					// 채팅블록시간				
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
					CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
								 sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
					pMsg->tChatBlockTime = cTemp2.GetTime();


	#if defined(_RELEASED)
					CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
						sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
					pMsg->tLastLoginTime = cTemp3.GetTime();
	#endif
				}
				else
				{
					break;
				}
			}
		}
//#endif
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

    std::string Query2(
        sc::string::format(
            "Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );

	sReturn = ::SQLExecDirect(spConn->hStmt,
							(SQLCHAR*) Query2.c_str(), 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	//m_pGameDB->FreeConnection(spConn);
    return sc::db::DB_OK;
}


//
//mjeon.indonesia
//
int COdbcManager::IdnGetUserInfo(const std::string &strId, NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	SQLRETURN	sReturn = 0;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	   
	
    std::string UserId(strId);
    sc::string::SqlInjectionCheck(UserId);

	TIMESTAMP_STRUCT sLastLoginTime;
    SQLINTEGER cbLastLoginTime = SQL_NTS;	


	/*IdnUserInfo*/
    std::string Query(
        sc::string::format(
	        "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, LastLoginDate FROM dbo.UserInfo WHERE UserID='%s'",
		    UserId.c_str()));

    
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
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
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				
				return sc::db::DB_ERROR;
			}

 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);
				::SQLGetData(spConn->hStmt, 7, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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

                pMsg->tPremiumTime = cTemp.GetTime();

				// 채팅블록시간
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

				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();


				CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
					sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
				pMsg->tLastLoginTime = cTemp3.GetTime();
			}
			else
			{
				break;
			}
        }		
	}
	

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

    std::string Query2(
        sc::string::format(
            "Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );

	sReturn = ::SQLExecDirect(spConn->hStmt,
							(SQLCHAR*) Query2.c_str(), 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
        
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}
	
    return sc::db::DB_OK;
}




/**
* DAUM : 사용자 정보를 가져와서 Agent 로 전송
*/
int COdbcManager::DaumGetUserInfo(const TCHAR* szGID, DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
    SQLRETURN	sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum  = 0,  cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
    std::string Query(
        sc::string::format(
            "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.DaumUserInfo WHERE UserGID='%1%'",
            szGID));

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime = cTemp.GetTime();

				// 채팅블록시간
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

    // 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	
    std::string Query2(
        sc::string::format(
            "Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}
	
	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.	

	//m_pGameDB->FreeConnection(spConn);

	// Daum 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
    std::string Query3(
        sc::string::format(
            "SELECT CheckFlag FROM dbo.DaumUserInfo WHERE UserGID='%s'", szGID));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query3.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS ) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		Print(Query3.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(false);
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query3.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pUserDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}
		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag);
			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}		
	}

	//strTemp2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	//m_pUserDB->FreeConnection(spConn);
    return sc::db::DB_OK;
}

/**
* GSP 사용자 정보를 가져온다.
* \param szUserID UserID varchar(50)
* \param pMsg
* \return 
*/
int COdbcManager::GspGetUserInfo(const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate ";
	//strTemp << "FROM GspUserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;

    std::string Query(
        sc::string::format(
            "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.GspUserInfo WHERE UserID='%s'", szUserID));
    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch( spConn->hStmt );
			if ( (sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO) )
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
 			if ((sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO))
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);
    return sc::db::DB_OK;
}

// 말레이지아
int COdbcManager::TerraGetUserInfo(
	const TCHAR* szTLoginName, 
	TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;

	SQLINTEGER nVTGameTime = 0, cbVTGameTime = SQL_NTS;
	TIMESTAMP_STRUCT sLastLoginTime; SQLINTEGER cbLastLoginTime = SQL_NTS;

	std::string Query;

  
//#if defined( VN_PARAM ) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		// 베트남 탐닉 방지 시스템 추가
		Query = sc::string::format(
					"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, GameTime, LastLoginDate FROM dbo.TerraUserInfo WHERE TLoginName='%s'", 
						szTLoginName);
	}
	else
	{
//#else
		Query = sc::string::format(
					"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.TerraUserInfo WHERE TLoginName='%s'",
						szTLoginName);
	}
//#endif
    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
//#if defined( VN_PARAM ) //vietnamtest%%%

		if (m_ServiceProvider == SP_VIETNAM)
		{
			while (true)
			{
				sReturn = ::SQLFetch(spConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					Print(Query.c_str());
					Print(GetErrorString(spConn->hStmt));
					//m_pUserDB->FreeConnection(spConn);
					return sc::db::DB_ERROR;
				}
				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
					::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
					::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
					::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
					::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
					::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

					::SQLGetData(spConn->hStmt, 7, SQL_C_LONG,           &nVTGameTime, 0, &cbVTGameTime);
					::SQLGetData(spConn->hStmt, 8, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);

					pMsg->nUserNum       = nUserNum;
					pMsg->nUserType      = nUserType;
					pMsg->uChaRemain     = (USHORT) nChaRemain;
					pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
					pMsg->tPremiumTime   = cTemp.GetTime();

					// 채팅블록시간				
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
					CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
						sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
					pMsg->tChatBlockTime = cTemp2.GetTime();

					pMsg->nVTGameTime	= nVTGameTime;

					CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
						sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
					pMsg->tLastLoginTime = cTemp3.GetTime();
				}
				else
				{
					break;
				}
			}
		}
		else
		{
//#else
			while(true)
			{
				sReturn = ::SQLFetch(spConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					Print(Query.c_str());
					Print(GetErrorString(spConn->hStmt));
					//m_pUserDB->FreeConnection(spConn);
					//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					return sc::db::DB_ERROR;
				}
 				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
					::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
					::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
					::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
					::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
					::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

					pMsg->nUserNum  = nUserNum;
					pMsg->nUserType = nUserType;
					pMsg->uChaRemain     = (USHORT) nChaRemain;
					pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
					pMsg->tPremiumTime   = cTemp.GetTime();

					// 채팅블록시간
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
					CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
								 sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
					pMsg->tChatBlockTime = cTemp2.GetTime();
				}
				else
				{
					break;
				}
			}
		}

//#endif
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;


	// 극강부 생성가능 여부 체크 
    std::string Query2(
        sc::string::format("Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}		
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	//m_pGameDB->FreeConnection(spConn);
/*	
	// 말레이지아 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return sc::db::DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
	//std::strstream strTemp1;
	//strTemp1 << "SELECT CheckFlag FROM ";
	//strTemp1 << "TerraUserInfo WHERE TLoginName='" << szTLoginName << "'";
	//strTemp1 << std::ends;

	_snprintf( szTemp, 256, "SELECT CheckFlag FROM "
							"TerraUserInfo WHERE TLoginName='%s'",
							szTLoginName );

	sReturn = ::SQLExecDirect( pConn->hStmt, ( SQLCHAR*)szTemp, SQL_NTS );

	if( (sReturn != SQL_SUCCESS ) && (sReturn != SQL_SUCCESS_WITH_INFO ))
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection( false );

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}

	while( true )
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pUserDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return sc::db::DB_ERROR;
		}
		if( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag );

			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pUserDB->FreeConnection(pConn);
*/
    return sc::db::DB_OK;
}

/**
* Excite 사용자 정보를 가져온다.일본.
* \param szUserID UserID varchar(20)
* \param pMsg
* \return 
*/
int COdbcManager::ExciteGetUserInfo(const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
    std::string Query(
        sc::string::format(
	        "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.ExciteUserInfo WHERE UserID='%s'", 
            szUserID));

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);
	
	// 일본 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
    std::string Query2(
        sc::string::format(
            "SELECT CheckFlag FROM dbo.ExciteUserInfo WHERE UserID='%s'",
            szUserID));
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(false);
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pUserDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag );
			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}
	}
	//m_pUserDB->FreeConnection(spConn);
    return sc::db::DB_OK;
}

/**
* Japan(Gonzo) 사용자 정보를 가져온다.
* \param szUserID UserID varchar(16)
* \param pMsg
* \return 
*/
int COdbcManager::JapanGetUserInfo( const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS; 

    std::string Query(
        sc::string::format(
            "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.JapanUserInfo WHERE UserID='%s'",
			szUserID));

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);				
				return sc::db::DB_ERROR;
			}

 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	//m_pUserDB->FreeConnection(spConn);

	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
    std::string Query2(
        sc::string::format(
            "Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);		
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            //m_pGameDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}

	//m_pGameDB->FreeConnection(spConn);

    return sc::db::DB_OK;
}

int COdbcManager::GsGetUserInfo(
	const TCHAR* szUserID,
	GS_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	

    std::string Query(
        sc::string::format(
            "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.GSUserInfo WHERE UserID='%s'",
            szUserID));
    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query.c_str());
		        Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
                CTime cTemp(
                    sPremiumDate.year,
                    sPremiumDate.month,
                    sPremiumDate.day, 
                    sPremiumDate.hour,
                    sPremiumDate.minute,
                    sPremiumDate.second);
                pMsg->tPremiumTime = cTemp.GetTime();

				// 채팅블록시간				
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }		
	}	

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
    std::string Query2(
        sc::string::format(
            "Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			//// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}	
    return sc::db::DB_OK;
}

int COdbcManager::GmGetUserInfo(
								const TCHAR* szUserID,
								GM_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;

	SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
	SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
	SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
	SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;

	TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	

	std::string Query(
		sc::string::format(
		"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.GMUserInfo WHERE UserID='%s'",
		szUserID));
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				Print(Query.c_str());
				Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
				::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
				::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
				::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
				::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

				pMsg->nUserNum       = nUserNum;
				pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
				pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
				CTime cTemp(
					sPremiumDate.year,
					sPremiumDate.month,
					sPremiumDate.day, 
					sPremiumDate.hour,
					sPremiumDate.minute,
					sPremiumDate.second);
				pMsg->tPremiumTime = cTemp.GetTime();

				// 채팅블록시간				
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
					sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
		}		
	}	

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
		return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	std::string Query2(
		sc::string::format(
		"Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}	
	return sc::db::DB_OK;
}

int COdbcManager::UsGetUserInfo(
								const TCHAR* szUserID,
								US_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;

	SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
	SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
	SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
	SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;

	TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	

	std::string Query(
		sc::string::format(
		"SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM dbo.USUserInfo WHERE UserID='%s'",
		szUserID));
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pUserDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while (true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				Print(Query.c_str());
				Print(GetErrorString(spConn->hStmt));
				//m_pUserDB->FreeConnection(spConn);
				return sc::db::DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
				::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
				::SQLGetData(spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
				::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
				::SQLGetData(spConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(spConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

				pMsg->nUserNum       = nUserNum;
				pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
				pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

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
				CTime cTemp(
					sPremiumDate.year,
					sPremiumDate.month,
					sPremiumDate.day, 
					sPremiumDate.hour,
					sPremiumDate.minute,
					sPremiumDate.second);
				pMsg->tPremiumTime = cTemp.GetTime();

				// 채팅블록시간				
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
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
					sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
		}		
	}	

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	spConn = m_pGameDB->GetConnection(); 
	if (!spConn)
		return sc::db::DB_ERROR;

	SQLINTEGER bExtreme = 0, cbExtreme = SQL_NTS;
	SQLINTEGER nJumpingCharCnt = 0, cbJumpingCharCnt = SQL_NTS;
	SQLINTEGER bActor = 0, cbActor = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	// 극강부 리뉴얼에 따른 수정
	std::string Query2(
		sc::string::format(
		"Exec sp_ExtremeRenewal %d, %d, %d", nUserNum, GLCONST_CHAR::wLEVEL_FOR_EXTREME, GLCONST_CHAR::wLEVEL_FOR_ACTOR ) );
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query2.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		Print(Query2.c_str());
		Print(GetErrorString(spConn->hStmt));
		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query2.c_str());
			Print(GetErrorString(spConn->hStmt));
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG, &bExtreme, 0, &cbExtreme );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG, &nJumpingCharCnt, 0, &cbJumpingCharCnt);
			::SQLGetData( spConn->hStmt, 3, SQL_C_LONG, &bActor, 0, &cbActor );

			pMsg->bExtreme = bExtreme;
			pMsg->bJumping = nJumpingCharCnt;
			pMsg->bActor = bActor;
		}
		else
		{
			break;
		}
	}	
	return sc::db::DB_OK;
}

WORD COdbcManager::ThaiGetTodayPlayTime( int nUserNum )
{
    std::string Query(
        sc::string::format(
            "{call Thai_GetGameTime(%d,?)}", nUserNum));

	int nReturn = m_pUserDB->ExecuteSpInt(Query);	
	if (nReturn == sc::db::DB_ERROR)
		return 0;
	else
		return (WORD) nReturn;
}

int COdbcManager::GetUserTempInfo(int nUserNum)
{
	if (nUserNum <= 0)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "{call LogChaRemain_Get(%d,?)}", nUserNum));
	return m_pUserDB->ExecuteSpInt(Query);
}

int COdbcManager::SetUserTempInfo( int nUserNum, int nCreateChar )
{
	if (nUserNum <= 0)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "{call  LogChaRemain_Save( %d, %d, ? )}", nUserNum, nCreateChar));

	return m_pUserDB->ExecuteSpInt(Query);
}

int COdbcManager::WorldBattleUserCheck(
	const std::string& UserId,
	const std::string& UserPass,
	const std::string& UserIp,
	int ServerGroup,
	int ServerNumber)
{
	sc::writeLogError(std::string("Do not call COdbcManager::WorldBattleUserCheck use ADO function"));
	return sc::db::DB_ERROR;
}

int COdbcManager::WorldBattleGetUserInfo(const std::string& UserId, NET_LOGIN_FEEDBACK_DATA2& Msg)
{
    sc::writeLogError(std::string("Do not call COdbcManager::WorldBattleGetUserInfo use ADO function"));
    return sc::db::DB_ERROR;
}
