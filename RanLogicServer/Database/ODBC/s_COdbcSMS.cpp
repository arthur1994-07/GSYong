#include "../../pch.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

#include "../../../SigmaCore/DebugInclude.h"

int COdbcManager::LogSMS(
	int nSendChaNum,	
	int nReceiveChaNum,
	const TCHAR* szReceivePhone,
	const TCHAR* szSmsMsg )
{
    sc::writeLogError("COdbcManager::LogSMS. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nSendChaNum < 0 ||
		nReceiveChaNum < 0 ||
		szReceivePhone == NULL ||
		szSmsMsg == NULL )
	{
		return sc::db::DB_ERROR;
	}
	else
	{
		TCHAR szTemp[256] = {0};
		_snprintf( szTemp, 256, "{call sp_LogSms_Insert(%d,'',%d,'%s','%s',?)}",
								nSendChaNum,
								nReceiveChaNum,
								szReceivePhone,
								szSmsMsg );
		
		int nReturn = m_pLogDB->ExecuteSpInt( szTemp );	
		return nReturn;
	}
*/
}

int COdbcManager::SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber)
{
    sc::writeLogError("COdbcManager::SetChaPhoneNumber. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nChaNum < 0 || szPhoneNumber == NULL)
		return sc::db::DB_ERROR;
		
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaFriendSms(%d,'%s',?)}", nChaNum, szPhoneNumber );
	
	int nReturn = m_pGameDB->ExecuteSpInt( szTemp );		
	return nReturn;
*/
}

/**
* 캐릭터의 친구정보를 가져온다.
* \param nChaNum 친구정보를 가져올 캐릭터 번호
* \param vFriend 친구정보를 담을 vector
*/
int	COdbcManager::GetChaFriendSMS(
	int nChaNum,
	std::vector<CHA_FRIEND_SMS> &vFriend )
{
	if (nChaNum < 0) return sc::db::DB_ERROR;

	SQLRETURN sReturn = 0;
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT ChaP, ChaS, ChaName, ChaFlag, ChaToPhone From viewChaFriendSms ";
	//strTemp << "WHERE ChaP=" << nChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT TOP %d ChaP, ChaS, ChaName, ChaFlag, ChaToPhone From dbo.viewChaFriendSms WHERE ChaP=%d",
					   MAX_FRIEND, nChaNum );
	
	sReturn = ::SQLExecDirect(
					spConn->hStmt,
					(SQLCHAR*)szTemp, 
					SQL_NTS );

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print( szTemp );
		Print( GetErrorString(spConn->hStmt) );
		//m_pGameDB->FreeConnection( spConn );
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}

	SQLCHAR szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaP = 0, cbChaP=SQL_NTS;
	SQLINTEGER nChaS = 0, cbChaS=SQL_NTS;
	SQLINTEGER nChaFlag = 0, cbChaFlag=SQL_NTS;
	SQLCHAR szPhoneNumber[SMS_RECEIVER] = {0}; SQLINTEGER cbPhoneNumber = SQL_NTS;
	
	while (true)
	{
		sReturn = ::SQLFetch( spConn->hStmt );
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print( szTemp );
			Print( GetErrorString(spConn->hStmt) );
            //m_pGameDB->FreeConnection( spConn );
			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return sc::db::DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			CHA_FRIEND_SMS sFriendSMS;

			// Bind data
			::SQLGetData( spConn->hStmt, 1, SQL_C_LONG,	&nChaP, 0, &cbChaP );
			::SQLGetData( spConn->hStmt, 2, SQL_C_LONG,	&nChaS, 0, &cbChaS );
			::SQLGetData( spConn->hStmt, 3, SQL_C_CHAR,  szChaName, CHR_ID_LENGTH, &cbChaName );
			::SQLGetData( spConn->hStmt, 4, SQL_C_LONG,	&nChaFlag, 0, &cbChaFlag );
			::SQLGetData( spConn->hStmt, 5, SQL_C_CHAR,  szPhoneNumber, SMS_RECEIVER, &cbPhoneNumber );

			 if (cbChaName != 0 && cbChaName != -1)
				::StringCchCopy( sFriendSMS.szCharName, CHR_ID_LENGTH+1, (const TCHAR*) szChaName );

			 if (cbPhoneNumber != 0 && cbPhoneNumber != -1)
				::StringCchCopy( sFriendSMS.szPhoneNumber, SMS_RECEIVER, (const TCHAR*) szPhoneNumber );

			 sFriendSMS.nChaS = nChaS;
			 sFriendSMS.nChaFlag = nChaFlag;

			 vFriend.push_back ( sFriendSMS );
		}
		else
		{
			break;
		}		
	}	  
	//m_pGameDB->FreeConnection(spConn);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return sc::db::DB_OK;    
}
