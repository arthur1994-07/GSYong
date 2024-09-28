#include "../../pch.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"
#include "../DBAction/DbAction.h"

#include "../../../RanLogic/Post/GLPostData.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../SigmaCore/DebugInclude.h"

//
//mjeon.sns
//

//
// FACEBOOK
//
int COdbcManager::SNSAuthedFB(SFACEBOOK &AuthFB)
{
    sc::writeLogError("COdbcManager::SNSAuthedFB. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[512] = {0};
	_snprintf( szQuery, 512, "{call sp_SNSAuthedFB(%d,'%s','%s',?)}", AuthFB.ChaNum, AuthFB.SKey, AuthFB.UID );
	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);	
	return nReturn;
*/
}


int COdbcManager::SNSUnAuthedFB(int nChaNum)
{
    sc::writeLogError("COdbcManager::SNSUnAuthedFB. Use ADO");
    return sc::db::DB_ERROR;
/*
    TCHAR szQuery[128] = {0};
	_snprintf( szQuery, 128, "{call sp_SNSUnAuthedFB(%d,?)}", nChaNum );
	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	return nReturn;	
*/
}


int COdbcManager::SNSGetUidFB(int nTargetChaNum, std::string &FBUID)
{
	if ( nTargetChaNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	int nRowCount = 0;

	SQLRETURN sReturn = 0;


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	TCHAR szQuery[128] = {0};

	_snprintf( szQuery, 128, "SELECT FBUID FROM dbo.SNSFacebookInfo WHERE ChaNum = %d", nTargetChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	
	
	SQLCHAR		UID[SNS_MAX_UID_LENGTH]={0,};		SQLINTEGER	cbUID = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data
			
			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR,	UID,	SNS_MAX_UID_LENGTH,	&cbUID);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);

	if (nRowCount > 1 || nRowCount <= 0)		// It MUST be 0 or 1.
		return sc::db::DB_ERROR;
	
	if (cbUID != 0 && cbUID != -1)
		FBUID = std::string((const char *)UID);


	return sc::db::DB_OK;
}




//
// TWITTER
//
int COdbcManager::SNSAuthedTW(STWITTER &AuthTW)
{
    sc::writeLogError("COdbcManager::SNSAuthedTW. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[1024] = {0};
	_snprintf( szQuery, 1024, "{call sp_SNSAuthedTW(%d,'%s','%s','%s',?)}", AuthTW.ChaNum, AuthTW.AToken, AuthTW.ATokenS, AuthTW.UID );
	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	return nReturn;
*/
}


int COdbcManager::SNSUnAuthedTW(int nChaNum)
{
    sc::writeLogError("COdbcManager::SNSUnAuthedTW. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[128] = {0};
	_snprintf( szQuery, 128, "{call sp_SNSUnAuthedTW(%d,?)}", nChaNum );
	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	return nReturn;	
*/
}


int COdbcManager::SNSGetUidTW(int nTargetChaNum, std::string &TWUID)
{
	if ( nTargetChaNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	int nRowCount = 0;

	SQLRETURN sReturn = 0;


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	TCHAR szQuery[128] = {0};

	_snprintf( szQuery, 128, "SELECT TWUID FROM dbo.SNSTwitterInfo WHERE ChaNum = %d", nTargetChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}


	SQLCHAR		UID[SNS_MAX_UID_LENGTH]={0,};		SQLINTEGER	cbUID = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR,	UID,	SNS_MAX_UID_LENGTH,	&cbUID);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);

	if (nRowCount > 1 || nRowCount <= 0)		// It MUST be 0 or 1.
		return sc::db::DB_ERROR;

	if (cbUID != 0 && cbUID != -1)
		TWUID = std::string((const char *)UID);


	return sc::db::DB_OK;
}




//
// SNS Common
//

int COdbcManager::SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW)
{
	int nRetFB = sc::db::DB_OK;
	int nRetTW = sc::db::DB_OK;

	nRetFB = SNSGetInfoFB(AuthFB);
	nRetTW = SNSGetInfoTW(AuthTW);

	//
	// At least, one of two queries MUST be succeed.
	//
	return ((nRetFB == sc::db::DB_OK || nRetTW == sc::db::DB_OK) ? sc::db::DB_OK : sc::db::DB_ERROR);
}



int COdbcManager::SNSGetInfoFB(SFACEBOOK &AuthFB) // int nChaNum, char *pAToken, char *pUID)
{
	if ( AuthFB.ChaNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	int nRowCount = 0;

	SQLRETURN sReturn = 0;


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	TCHAR szQuery[128] = {0};

	_snprintf( szQuery, 128, "SELECT FBSKey, FBUID FROM dbo.SNSFacebookInfo WHERE ChaNum = %d", AuthFB.ChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}


	SQLCHAR		SKey[SNS_MAX_SKEY_LENGTH]={0,};		SQLINTEGER	cbSKey = SQL_NTS;
	SQLCHAR		UID[SNS_MAX_UID_LENGTH]={0,};		SQLINTEGER	cbUID = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR,	SKey,	SNS_MAX_SKEY_LENGTH,&cbSKey);
			::SQLGetData(spConn->hStmt, 2, SQL_C_CHAR,	UID,	SNS_MAX_UID_LENGTH,	&cbUID);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);

	if (nRowCount > 1 || nRowCount < 0)		// It MUST be 0 or 1.
		return sc::db::DB_ERROR;

	if (cbSKey != 0 && cbSKey != -1)
		::StringCchCopy(AuthFB.SKey, SNS_MAX_SKEY_LENGTH, (const char*) SKey);
	if (cbUID != 0 && cbUID != -1)
		::StringCchCopy(AuthFB.UID, SNS_MAX_UID_LENGTH, (const char*) UID);


	return sc::db::DB_OK;
}




int COdbcManager::SNSGetInfoTW(STWITTER &AuthTW) // int nChaNum, char *pAToken, char *pUID)
{
	if ( AuthTW.ChaNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	int nRowCount = 0;

	SQLRETURN sReturn = 0;


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	TCHAR szQuery[128] = {0};

	_snprintf( szQuery, 128, "SELECT TWToken, TWTokenS, TWUID FROM dbo.SNSTwitterInfo WHERE ChaNum = %d", AuthTW.ChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}


	SQLCHAR		AToken[SNS_MAX_TOKEN_LENGTH]={0,};	SQLINTEGER	cbAToken = SQL_NTS;
	SQLCHAR		ATokenS[SNS_MAX_TOKEN_LENGTH]={0,};	SQLINTEGER	cbATokenS = SQL_NTS;
	SQLCHAR		UID[SNS_MAX_UID_LENGTH]={0,};		SQLINTEGER	cbUID = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR,	AToken,		SNS_MAX_TOKEN_LENGTH,&cbAToken);
			::SQLGetData(spConn->hStmt, 2, SQL_C_CHAR,	ATokenS,	SNS_MAX_TOKEN_LENGTH,&cbATokenS);
			::SQLGetData(spConn->hStmt, 3, SQL_C_CHAR,	UID,		SNS_MAX_UID_LENGTH,	&cbUID);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);

	if (nRowCount > 1 || nRowCount < 0)		// It MUST be 0 or 1.
		return sc::db::DB_ERROR;

	if (cbAToken != 0 && cbAToken != -1)
		::StringCchCopy(AuthTW.AToken, SNS_MAX_TOKEN_LENGTH, (const char*) AToken);
	if (cbATokenS != 0 && cbATokenS != -1)
		::StringCchCopy(AuthTW.ATokenS, SNS_MAX_TOKEN_LENGTH, (const char*) ATokenS);
	if (cbUID != 0 && cbUID != -1)
		::StringCchCopy(AuthTW.UID, SNS_MAX_UID_LENGTH, (const char*) UID);


	return sc::db::DB_OK;
}
