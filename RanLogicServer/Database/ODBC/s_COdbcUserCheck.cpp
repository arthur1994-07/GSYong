#include "../../pch.h"
#include <boost/lexical_cast.hpp>
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"

#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::DaumUserCheck(TCHAR* szDaumGID, const TCHAR* szUUID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum, int& Age)
{
    // #1 UUID�� �̿��ؼ� UserTemp ���� UserID, UserIP, LoginDate �� �����´�.
	SQLRETURN sReturn=0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pKorDB->GetConnection();
	if (!spConn || !szUUID || !szDaumGID)
        return sc::db::DB_ERROR;

    std::string Uuid(szUUID);
    sc::string::SqlInjectionCheck(Uuid);
    std::string Query(
        sc::string::format(
            "SELECT UserID, LoginDate, SSNHEAD FROM dbo.UserUUID WHERE UserUUID = '%1%'", Uuid));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pKorDB->FreeConnection(spConn);
        return sc::db::DB_ERROR;
	}

	SQLCHAR szKorUserID[KOR_MAX_GID_LENGTH+1] = {0}; SQLINTEGER cbKorUserID = SQL_NTS;
	TIMESTAMP_STRUCT sLoginDate; SQLINTEGER cbLoginDate = SQL_NTS;
    // �ֹε�Ϲ�ȣ ���ڸ�
    SQLCHAR szKorSsnHead[KOR_SSN_HEAD_LENGTH] = {0}; SQLINTEGER cbKorSsnHead = SQL_NTS;

	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if ((sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO))
        {			
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            return sc::db::DB_ERROR;
		}

		if ((sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO))
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szKorUserID, KOR_MAX_GID_LENGTH+1, &cbKorUserID);
			::SQLGetData(spConn->hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sLoginDate, 0, &cbLoginDate);
            ::SQLGetData(spConn->hStmt, 3, SQL_C_CHAR, szKorSsnHead, KOR_SSN_HEAD_LENGTH, &cbKorSsnHead);

			if ((cbKorUserID != 0) && (cbKorUserID != -1))
			{
				StringCchCopy(szDaumGID, KOR_MAX_GID_LENGTH+1, (const TCHAR*) szKorUserID);
				nRowCount++;
			}

            if ((cbKorSsnHead != 0) && (cbKorSsnHead != -1))
            {
                std::string SsnHead((const char*) szKorSsnHead);
                sc::string::trim(SsnHead);
                Age = sc::string::SsnHeadToAge(SsnHead);
                /*
                sc::writeLogInfo(
					sc::string::format("SSNHead:%1%, Age:%2%, GID:%3%", SsnHead, Age, szKorUserID));
                */
            }
            else
            {
                sc::writeLogError("SSNHEAD NULL");
                Age = 15;
            }

			// �����̾� ����Ⱓ
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if ((sLoginDate.year <= 1970) || (sLoginDate.year >= 2999))
			{
				sLoginDate.year = 1970;
				sLoginDate.month = 2;
				sLoginDate.day = 1;
				sLoginDate.hour = 1;
				sLoginDate.minute = 1;
				sLoginDate.second = 1;
			}					
		}
		else
		{
			break;
		}
	}
	//m_pKorDB->FreeConnection( spConn );
	//spConn = NULL;
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		// Check Code
//		sc::writeLogError( _T("DB Check:%s"), szTemp );
		//strTemp.freeze( false );

//		sc::writeLogError( _T("DaumUserCheck can't get info UserID from UserTemp") );
		
		return KOR_ROWCOUNT_ERROR;
	}

	// WEB �α��νð� ����
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ���� �ð��� �����´�.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (����ð� - WEB ���� �α����� �ð�) �� ����Ѵ�.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (����ð� - WEB ���� �α����� �ð�) �� ������ �������� �־�� �α����� �����Ѵ�.
	// PC �� ��� �α׾ƿ� ���� �ʰ� �ڸ��� ���� �������� ���...
	// �� ����� �տ� �÷��� �� ������ ������ �״�� ���� ���ɼ��� �ִ�.

	/*
	if ( (diffTime >= KOR_ALLOW_TIME_DIFF_MINUS) &&
		 (diffTime <= KOR_ALLOW_TIME_DIFF_PLUS) )
	*/
	if(1)
	{
		//mjeon.ReturnOfHero 
		std::string KorUserId((char*) szKorUserID);
        sc::string::replaceForDb(KorUserId);
        std::string Query(
            sc::string::format(
                "{call daum_user_verify_new('%1%','%2%',%3%,%4%,?)}", KorUserId, szUserIP, nSvrGrp, nSvrNum));

		int nReturn = m_pUserDB->ExecuteSpInt(Query);
		
		if (nReturn == 11 || nReturn == 12 || nReturn == 13 || nReturn == 40 || nReturn == 41 )
		{
			//mjeon.ReturnOfHero

			// SET @Seq = IDENT_CURRENT('RanUser') --SCOPE_IDENTITY()
			//SET @PurKey = 'AT_' + Convert( varchar(10), GETDATE(), 12) + '_' -- + Cast(@Seq AS varchar(10))

			SYSTEMTIME st;
			GetLocalTime(&st);
			CTime cTime(st);
			CString strTime = cTime.Format("%y%m%d_%H%M%S");		

			CString strPurKey;		
			strPurKey.Format("%s_%s_%3d", _T("RH"), strTime.GetString(), st.wMilliseconds);	

            Query = sc::string::format(
                "{call sp_InsertItemIntoBank('%1%', '%2%', %3%, %4%, ?) }",
				strPurKey.GetString(),
				szKorUserID,	
				3194,	// ProductKey of event item
				0		// Price is 0			
				);

			int ret = m_pShopDB->ExecuteSpInt(Query);

			if (ret != sc::db::DB_OK)
			{
				if (ret == 1)
				{
					// The item already exists in the ItemBank.
					sc::writeLogError(std::string("DB:DaumUserCheck: RH item alreay exists in the itembank."));
				}
				else
				{
					// sc::db::DB_ERROR: Some log
					sc::writeLogError(std::string("DB:DaumUserCheck: DB insertion failure."));
				}
			}
		}

		// ������ ��ȯ�̺�Ʈ ������ ������� ����ǹǷ� �ٽ� ������.
		if ( nReturn == 40 || nReturn == 41 )
		{
			nReturn -= 10;
		}


		return nReturn;
	}
	else
	{
		// Time is wrong
		return KOR_ALLOW_TIME_ERROR;
	}
}
	

/**
* Excite (Japan)
* \param szUserID �����ID
* \param szUsrIP IP
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return 
*/
int COdbcManager::ExciteUserCheck(const TCHAR* szUserID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum)
{
    std::string Query(
        sc::string::format(
            "{call excite_user_verify('%s','%s',%d,%d,?)}", szUserID, szUserIP, nSvrGrp, nSvrNum));
	return m_pUserDB->ExecuteSpInt(Query);
}

/**
* Gonzo (Japan)
* \param szUserID �����ID
* \param szUsrIP IP
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return 
*/
int COdbcManager::JapanUserCheck(const TCHAR* szUserID, const TCHAR* szUserIP,  int nSvrGrp, int nSvrNum, int nUserNum)
{
    std::string Query(
        sc::string::format(
            "{call Japan_user_verify('%s','%s',%d,%d,%d,?)}", szUserID, szUserIP, nSvrGrp, nSvrNum, nUserNum));
	return m_pUserDB->ExecuteSpInt(Query);
}

/**
 * Taiwan / HongKong 
 * �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
 * \param szUsrID ����� ID
 * \param szPasswd ����� �н�����
 * \param szUsrIP ������� IP Address
 * \param nSvrGrp �����׷�
 * \param nSvrNum ������ȣ
 * \return DB ������ �����
 */
int COdbcManager::UserCheck(
	const TCHAR* szUsrID, 
	const TCHAR* szPasswd, 
	const TCHAR* szUsrIP,
	const TCHAR* szRandomPasswd,
	int nRandomNum,
	int nSvrGrp,
	int nSvrNum)
{
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
    std::string strUserID(szUsrID);
    sc::string::trim(strUserID);
    sc::string::replaceForDb(strUserID);

    std::string strPasswd(szPasswd);
    sc::string::trim(strPasswd);
    sc::string::replaceForDb(strPasswd);

    std::string strRandomPasswd(szRandomPasswd);
    sc::string::trim(strRandomPasswd);
    sc::string::replaceForDb(strRandomPasswd);

    std::string RandomNum = boost::lexical_cast<std::string> (nRandomNum);

    std::string Query;

//mjeon.ReturnOfHero
#if 0
	//mjeon.ReturnOfHero
    Query = sc::string::format(
        "{call user_verify_new('%s','%s','%s',%d,%d,'%s','%s',?)}", 
        strUserID.c_str(),
        strPasswd.c_str(),
        szUsrIP,
        nSvrGrp,
        nSvrNum,
        strRandomPasswd.c_str(),
        RandomNum.c_str());

	int nReturn = m_pUserDB->ExecuteSpInt(Query);


	if ( nReturn == 11 || nReturn == 12 || nReturn == 13 || nReturn == 17 )
	{
		//mjeon.ReturnOfHero

		// SET @Seq = IDENT_CURRENT('RanUser') --SCOPE_IDENTITY()
		//SET @PurKey = 'AT_' + Convert( varchar(10), GETDATE(), 12) + '_' -- + Cast(@Seq AS varchar(10))
		
		SYSTEMTIME st;
		GetLocalTime(&st);
		CTime cTime(st);
		CString strTime = cTime.Format("%y%m%d_%H%M%S");		

		CString strPurKey;		
		strPurKey.Format("%s_%s_%3d", _T("RH"), strTime.GetString(), st.wMilliseconds);

        Query = sc::string::format(
            "{call sp_InsertItemIntoBank( '%s', '%s', %d, %d, ?) }",
			strPurKey.GetString(),
			strUserID.c_str(),
			3194,	// ProductKey of event item
			0		// Price is 0			
			);


		int ret = m_pLogDB->ExecuteSpInt(Query);

		if (ret != sc::db::DB_OK)
		{
			if ( ret == 1 )
			{
				// The item already exists in the ItemBank.
				sc::writeLogError(std::string("DB:DaumUserCheck: RH item alreay exists in the itembank."));
			}
			else
			{
				// sc::db::DB_ERROR: Some log
				sc::writeLogError(std::string("DB:DaumUserCheck: DB insertion failure."));
			}
		}
	}
#else
    Query = sc::string::format(
        "{call user_verify('%s','%s','%s',%d,%d,'%s','%s',?)}", 
		strUserID.c_str(),
		strPasswd.c_str(),
		szUsrIP,
		nSvrGrp,
		nSvrNum,
		strRandomPasswd.c_str(),
		RandomNum.c_str());


	int nReturn = m_pUserDB->ExecuteSpInt(Query);
#endif

	return nReturn;
}
;


//
//mjeon.indonesia
//
int COdbcManager::IdnUserCheck(int nUserNum, const std::string &strId, const std::string &strIP, int nSvrGrp, int nSvrNum)
{
	std::string strId4DB(strId);

	//
	// DB ���ڿ��� ���� ' -> ''
	//
    sc::string::trim(strId4DB);
    sc::string::replaceForDb(strId4DB);

    CString csQuery;
	csQuery.Format(_T("{call idn_user_verify(%d, '%s','%s',%d, %d,?)}"), 
					nUserNum, strId4DB.c_str(), strIP.c_str(), nSvrGrp, nSvrNum);

	int nReturn = m_pUserDB->ExecuteSpInt(csQuery.GetString());

	return nReturn;
}



/**
* Thailand
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::ThaiUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum )
{
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
    std::string strUserID(szUsrID);
    sc::string::replaceForDb(strUserID);

    std::string strPasswd(szPasswd);
    sc::string::replaceForDb(strPasswd);

    std::string Query(
        sc::string::format(
	        "{call thai_user_verify('%s','%s','%s',%d,%d,?)}",
            strUserID.c_str(),
            strPasswd.c_str(),
            szUsrIP,
            nSvrGrp,
            nSvrNum));
	return m_pUserDB->ExecuteSpInt(Query);
}



/**
* GSP
* \n �α����Ϸ��� ����ڸ� üũ�Ѵ�.
* \param 
* \param 
* \param szUsrIP �α����Ϸ��� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return 
*/
int COdbcManager::GspUserCheck(TCHAR* szGspUserID, const TCHAR* szUUID, const TCHAR* szUserIP, int nSvrGrp, int nSvrNum)
{
	// #1 UUID�� �̿��ؼ� UserTemp ���� UserID, UserIP, LoginDate �� �����´�.
	SQLRETURN sReturn=0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGspDB->GetConnection();
	if (!spConn || !szUUID)
        return sc::db::DB_ERROR;
    
    std::string Uuid(szUUID);
    sc::string::SqlInjectionCheck(Uuid);

    std::string Query(
        sc::string::format(
            "SELECT UserID, UserIP, LoginDate FROM dbo.UserTemp WHERE GUID='%1%'", Uuid));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//m_pGspDB->FreeConnection(spConn);		
        return sc::db::DB_ERROR;
	}

	SQLCHAR szGspUserID2[GSP_USERID] = {0};
	SQLCHAR szGspUserIP[GSP_USERIP] = {0};
	TIMESTAMP_STRUCT sLoginDate; SQLINTEGER cbLoginDate = SQL_NTS;
	
	SQLINTEGER cbGspUserID = SQL_NTS;
	SQLINTEGER cbGspUserIP = SQL_NTS;

	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if ( (sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO) )
        {			
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pGspDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if ( (sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO) )
		{
			::SQLGetData( spConn->hStmt, 1, SQL_C_CHAR, szGspUserID2, GSP_USERID, &cbGspUserID );
			::SQLGetData( spConn->hStmt, 2, SQL_C_CHAR, szGspUserIP, GSP_USERIP, &cbGspUserIP );
			::SQLGetData( spConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sLoginDate, 0, &cbLoginDate );

			if ( (cbGspUserID != 0) && (cbGspUserID != -1) )
			{
				StringCchCopy(szGspUserID, GSP_USERID, (const TCHAR*) szGspUserID2);
				nRowCount++;
			}

			// �����̾� ����Ⱓ
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if ( (sLoginDate.year <= 1970) || (sLoginDate.year >= 2999) )
			{
				sLoginDate.year = 1970;
				sLoginDate.month = 2;
				sLoginDate.day = 1;
				sLoginDate.hour = 1;
				sLoginDate.minute = 1;
				sLoginDate.second = 1;
			}					
		}
		else
		{
			break;
		}		
	}
	//m_pGspDB->FreeConnection( spConn );
	//spConn = NULL;
	
	// Row Count �� 1 �� �ƴϸ� �����̴�.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		// Check Code
		sc::writeLogInfo(sc::string::format("DB Check:%s", Query.c_str()));
		sc::writeLogError(std::string("DB:GspUserCheck can't get info UserID from UserTemp"));		
		return sc::db::DB_ERROR;
	}

	// WEB �α��νð� ����
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ���� �ð��� �����´�.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (����ð� - WEB ���� �α����� �ð�) �� ����Ѵ�.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (����ð� - WEB ���� �α����� �ð�) �� ������ �������� �־�� �α����� �����Ѵ�.
	// PC �� ��� �α׾ƿ� ���� �ʰ� �ڸ��� ���� �������� ���...
	// �� ����� �տ� �÷��� �� ������ ������ �״�� ���� ���ɼ��� �ִ�.
	if ( (diffTime >= GSP_ALLOW_TIME_DIFF_MINUS) &&
		 (diffTime <= GSP_ALLOW_TIME_DIFF_PLUS) )
	{
		// Time is ok		
        std::string Query(
            sc::string::format(
                "{call gsp_user_verify('%s','%s',%d,%d,?)}",
                szGspUserID2,
                szUserIP,
                nSvrGrp,
                nSvrNum));
		return m_pUserDB->ExecuteSpInt(Query);
	}
	else
	{
		// Time is wrong
		return GSP_ALLOW_TIME_ERROR;
	}
}

/**
 * Terra
 * �α����Ϸ��� ����ڸ� üũ�Ѵ�.
 * \param szTerraLoginName 
 * \param szTerraDecodedTID 
 * \param szUsrIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \return 
 */
int COdbcManager::TerraUserCheck(
	TCHAR* szTerraLoginName,
	const TCHAR* szTerraDecodedTID,
    const TCHAR* szUsrIP,
	int nSvrGrp,
    int nSvrNum )
{
	// #1 Decoded TID �� �̿��ؼ� TempUser ���� TLoginName �� �����´�.
	SQLRETURN sReturn=0;
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pTerraDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	std::string Query;

	// ���� ���ڵ��� TID �� �̿��ؼ� TLoginName �� �����´�.
//#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_VIETNAM)
	{
		Query = sc::string::format("SELECT TLoginName FROM dbo.TempUser WHERE UTid = '%1%'", szTerraDecodedTID);
	}
	else
	{
//#else
	// ���������� �α��� ��� �������� ���� ����
		Query = sc::string::format("SELECT LoginID FROM dbo.TempUser WHERE TKeyUser = '%1%'", szTerraDecodedTID);
	}
//#endif

	sReturn = ::SQLExecDirect(spConn->hStmt,
							  (SQLCHAR*) Query.c_str(), 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pTerraDB->FreeConnection(spConn);
        return sc::db::DB_ERROR;
	}

	SQLCHAR szTLoginName[TERRA_TLOGIN_NAME+1] = {0};
	SQLINTEGER cbTLoginName = SQL_NTS;
	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {			
            Print(Query);
			Print(GetErrorString(spConn->hStmt));
            //m_pTerraDB->FreeConnection(spConn);
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szTLoginName, TERRA_TLOGIN_NAME+1, &cbTLoginName); 

			if (cbTLoginName != 0 && cbTLoginName != -1) // TLoginName
			{
				StringCchCopy(szTerraLoginName, TERRA_TLOGIN_NAME+1, (const char*) szTLoginName);
				nRowCount++;
			}
		}
		else
		{
			break;
		}		
	}
	//m_pTerraDB->FreeConnection(spConn);
	//spConn = NULL;
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		sc::writeLogError(std::string("DB:TerraUserCheck can't get info TLoginName from dbo.TempUser"));
		
		return sc::db::DB_ERROR;
	}
	
	// TLoginName �� "" �� ��Ÿ���� ����� ������ �־� ����ó���Ѵ�.
	if (strlen(szTerraLoginName) < 2)
        return sc::db::DB_ERROR;
    
    std::string Query2(
        sc::string::format(
	        "{call terra_user_verify('%s','X','X','X','%s',%d,%d,?)}", szTerraLoginName, szUsrIP, nSvrGrp, nSvrNum));

	return m_pUserDB->ExecuteSpInt(Query2);
}


/**
* �۷ι� ����
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::GsUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum)
{
	
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
    std::string strUserID(szUsrID);
    sc::string::trim(strUserID);
    sc::string::replaceForDb(strUserID);

    std::string strPasswd(szPasswd);
    sc::string::trim(strPasswd);
    sc::string::replaceForDb(strPasswd);

    std::string Query(
        sc::string::format(
            "{call gs_user_verify('%s','%s','%s',%d,%d,?)}", 
            strUserID.c_str(),
            strPasswd.c_str(),
            szUsrIP,
            nSvrGrp,
            nSvrNum));

	return m_pUserDB->ExecuteSpInt(Query);
}

/**
* EU ����
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::GmUserCheck(
							  const TCHAR* szUsrID,
							  const TCHAR* szPasswd,
							  const TCHAR* szUsrIP,
							  int nSvrGrp,
							  int nSvrNum )
{

	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
	std::string strUserID( szUsrID );
	sc::string::trim( strUserID );
	sc::string::replaceForDb( strUserID );

	std::string strPasswd( szPasswd );
	sc::string::trim( strPasswd );
	sc::string::replaceForDb( strPasswd );

	std::string Query(
		sc::string::format(
		"{call gm_user_verify('%s','%s','%s',%d,%d,?)}", 
		strUserID.c_str(),
		strPasswd.c_str(),
		szUsrIP,
		nSvrGrp,
		nSvrNum ) );

	return m_pUserDB->ExecuteSpInt( Query );
}

/**
* US ����
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::UsUserCheck(
							  const TCHAR* szUsrID,
							  const TCHAR* szPasswd,
							  const TCHAR* szUsrIP,
							  int nSvrGrp,
							  int nSvrNum )
{

	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
	std::string strUserID( szUsrID );
	sc::string::trim( strUserID );
	sc::string::replaceForDb( strUserID );

	std::string strPasswd( szPasswd );
	sc::string::trim( strPasswd );
	sc::string::replaceForDb( strPasswd );

	std::string Query(
		sc::string::format(
		"{call us_user_verify('%s','%s','%s',%d,%d,?)}", 
		strUserID.c_str(),
		strPasswd.c_str(),
		szUsrIP,
		nSvrGrp,
		nSvrNum ) );

	return m_pUserDB->ExecuteSpInt( Query );
}

/**
* Daum
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szDaumGID Daum UserGID
* \param szDaumPass ����� �н�����
* \return 
*/
int COdbcManager::DaumUserPassCheck(const TCHAR* szDaumGID, const TCHAR* szDaumPasswd, int nCheckFlag)
{
    std::string Query(
        sc::string::format(
	        "{call daum_user_passcheck('%s','%s',%d,?)}", szDaumGID, szDaumPasswd, nCheckFlag));
	return m_pUserDB->ExecuteSpInt(Query);
}

/**
* Terra
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szTID Terra UserID
* \param szPasswd ����� �н�����
* \return 
*/
int COdbcManager::TerraUserPassCheck(const TCHAR* szTID, const TCHAR* szPasswd, int	nCheckFlag)
{
    std::string Query(
        sc::string::format(
	        "{call terra_user_passcheck('%s','%s',%d,?)}", szTID, szPasswd, nCheckFlag));
	return m_pUserDB->ExecuteSpInt(Query);
}


/**
* Excite
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szUID Excite UserID
* \param szPasswd ����� �н�����
* \return 
*/
int COdbcManager::ExciteUserPassCheck(const TCHAR* szUID, const TCHAR* szUserID, const TCHAR* szPasswd, int	nCheckFlag)
{
    std::string Query(
        sc::string::format(
            "{call excite_user_passcheck('%s','%s','%s',%d,?)}", szUID, szUserID,szPasswd, nCheckFlag));
	return m_pUserDB->ExecuteSpInt(Query);
}

//! Thailand
//! ������� PC�� Ŭ������ üũ�Ѵ�.	
//! \param szUsrIP ������� IP Address
//! \return DB ������ �����
int	COdbcManager::ThaiUserClassCheck(const TCHAR* szUsrIP)
{
	SQLRETURN	sReturn = 0;
	int			nOutput = 0;
	char		szClass[2] = {0};
	char		temp[2] = "A";
	char		temp1[2] = "B";
	char		temp2[2] = "C";

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pThaiDB->GetConnection();
//	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLCHAR szCC_Class[2] = {0}; SQLINTEGER cbCC_Class = SQL_NTS;
	
    std::string Query(
        sc::string::format(
            "SELECT CC_CLASS FROM dbo.TB_CC_IP WHERE CC_IP='%s'", szUsrIP));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
		//m_pThaiDB->FreeConnection(spConn);
//		m_pUserDB->FreeConnection(pConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query);
		        Print(GetErrorString(spConn->hStmt));
				// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
				//m_pThaiDB->FreeConnection(spConn);
				//m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR, szCC_Class, 3, &cbCC_Class);

				if( cbCC_Class != 0 && cbCC_Class != -1 )
				::StringCchCopy(szClass, 2, (const char*) szCC_Class);

				if( 0 == strcmp(szClass, temp) )
				{
					nOutput = 1;
				}
				else if( 0 == strcmp(szClass, temp1) )
				{
					nOutput = 2;
				}
				else if( 0 == strcmp(szClass, temp2) )
				{
					nOutput = 3;
				}
				else nOutput = 0;
			}
			else
			{
				break;
			}
		}
	}

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	//m_pThaiDB->FreeConnection(spConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}

/**
* Malaysia
* ������� PC�� Ŭ������ üũ�Ѵ�.
* \param szUsrIP ������� IP Address
* \return DB ������ �����
*/
int	COdbcManager::MyUserClassCheck(
			const TCHAR* szUsrIP)
{
	SQLRETURN	sReturn = 0;
	int			nOutput = 0;
	char		szClass[2] = {0};
	char		temp[2] = "A";
	char		temp1[2] = "B";
	char		temp2[2] = "C";
	char		temp3[2] = "D";

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pMalaysiaDB->GetConnection();
//	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!spConn)	return sc::db::DB_ERROR;

	SQLCHAR szCC_Class[2] = {0}; SQLINTEGER cbCC_Class = SQL_NTS;
	
    std::string Query(
        sc::string::format(
            "SELECT CC_CLASS FROM dbo.TB_CC_IP WHERE CC_IP='%s'", szUsrIP));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
		//m_pMalaysiaDB->FreeConnection(spConn);
		// m_pUserDB->FreeConnection(pConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query);
		        Print(GetErrorString(spConn->hStmt));
				// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
				//m_pMalaysiaDB->FreeConnection(spConn);
				// m_pUserDB->FreeConnection(pConn);

				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR, szCC_Class, 3, &cbCC_Class);

				if( cbCC_Class != 0 && cbCC_Class != -1 )
				::StringCchCopy(szClass, 2, (const char*) szCC_Class);

				if( 0 == strcmp(szClass, temp) )
				{
					nOutput = 1;
				}
				else if( 0 == strcmp(szClass, temp1) )
				{
					nOutput = 2;
				}
				else if( 0 == strcmp(szClass, temp2) )
				{
					nOutput = 3;
				}
				else if( 0 == strcmp(szClass, temp3) )
				{
					nOutput = 4;
				}
				else nOutput = 0;
			}
			else
			{
				break;
			}
		}
	}

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	//m_pMalaysiaDB->FreeConnection(spConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}


/**
* �ʸ���
* ������� PC�� Ŭ������ üũ�Ѵ�.
* \param szUsrIP ������� IP Address
* \return DB ������ �����
*/
int	COdbcManager::PhUserClassCheck(const TCHAR* szUsrIP)
{
	SQLRETURN	sReturn = 0;
	int			nOutput = 0;
	char		szClass[2] = {0};
	char		temp[2] = "A";
	char		temp1[2] = "B";
	char		temp2[2] = "C";
	char		temp3[2] = "D";

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	//	IP���ʽ�
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pTerraDB->GetConnection();
//	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLCHAR szCC_Class[2] = {0}; SQLINTEGER cbCC_Class = SQL_NTS;
	
    std::string Query(
        sc::string::format(
            "SELECT CC_CLASS FROM dbo.TB_CC_IP WHERE CC_IP='%s'", szUsrIP));

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
		// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
		//m_pTerraDB->FreeConnection(spConn);
//		m_pUserDB->FreeConnection(pConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(Query);
		        Print(GetErrorString(spConn->hStmt));
				// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
				//m_pTerraDB->FreeConnection(spConn);
//				m_pUserDB->FreeConnection(pConn);
				return sc::db::DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR, szCC_Class, 3, &cbCC_Class);

				if( cbCC_Class != 0 && cbCC_Class != -1 )
				::StringCchCopy(szClass, 2, (const char*) szCC_Class);

				if( 0 == strcmp(szClass, temp) )
				{
					nOutput = 1;
				}
				else if( 0 == strcmp(szClass, temp1) )
				{
					nOutput = 2;
				}
				else if( 0 == strcmp(szClass, temp2) )
				{
					nOutput = 3;
				}
				else if( 0 == strcmp(szClass, temp3) )
				{
					nOutput = 4;
				}
				else nOutput = 0;
			}
			else
			{
				break;
			}
		}
	}

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	//m_pTerraDB->FreeConnection(spConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}
