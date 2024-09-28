#include "stdafx.h"
#include "../RanLogic/s_NetGlobal.h"
#include "./COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COdbcManager* COdbcManager::SelfInstance = NULL;

COdbcManager::COdbcManager(void)
: m_hEnvUser(NULL)
, m_hOdbcUser(NULL)
, m_hEnvGame(NULL)
, m_hOdbcGame(NULL)
{
}

COdbcManager* COdbcManager::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new COdbcManager();

	return SelfInstance;
}

void COdbcManager::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

COdbcManager::~COdbcManager(void)
{
	CloseUserDB();
	CloseGameDB();
}

void COdbcManager::CloseUserDB()
{
	CloseDB(m_hEnvUser, m_hOdbcUser);
}

void COdbcManager::CloseGameDB()
{
	CloseDB(m_hEnvGame, m_hOdbcGame);
}

int COdbcManager::OpenDB(char* szOdbcName, 
						 char* szUsrID, 
						 char* szUsrPasswd,
						 char* szDbName,
						 SQLHENV hEnv,
						 SQLHDBC hOdbc)
{
	if (hOdbc != NULL)
		CloseDB(hEnv, hOdbc);

	SQLRETURN sReturn; // SQL return value (short)

	SQLHENV hEnvTemp;
	SQLHDBC hOdbcTemp;

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnvTemp);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		hEnv = hEnvTemp;
	}
	else
	{		
		return DB_ERROR;
	}

	// ODBC set environment
	// version
	sReturn = ::SQLSetEnvAttr(hEnvTemp, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvTemp);
		hEnv = NULL;
		return DB_ERROR;
	}

	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_DBC, hEnvTemp, &hOdbcTemp);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbcTemp);
		hOdbc = NULL;
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvTemp);
		hEnv = NULL;
		return DB_ERROR;
	}

	// ODBC connect
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(hOdbcTemp, 5, (void*) SQL_LOGIN_TIMEOUT, 0);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		hOdbc = hOdbcTemp;
	}
	else
	{	
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
		hEnv = NULL;
		return DB_ERROR;
	}

	sReturn = ::SQLConnect(hOdbcTemp, 
		(SQLCHAR*) szOdbcName,
		SQL_NTS,
		(SQLCHAR*) szUsrID,
		SQL_NTS,
		(SQLCHAR*) szUsrPasswd,
		SQL_NTS);

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnvTemp);
		hEnvTemp = NULL;
		return DB_ERROR;
	}
}

void COdbcManager::CloseDB(SQLHENV hEnv, SQLHDBC hOdbc)
{
	if (hOdbc != NULL)
	{
		::SQLDisconnect(hOdbc);
		::SQLFreeHandle(SQL_HANDLE_DBC, hOdbc);
		hOdbc = NULL;
	}

	if (hEnv != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
		hEnv = NULL;
	}
}

int COdbcManager::Insert(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int	COdbcManager::Update(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int COdbcManager::Delete(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int	COdbcManager::ExecuteSQL(SQLHDBC hOdbc, CString strSQL)
{
	SQLHSTMT	hStmt; // Handle of statement (void*)
	SQLRETURN	sReturn;

	// Prepare statement handle
	hStmt = GetStmt(hOdbc);
	if (hStmt == NULL) return DB_ERROR;

	sReturn = ::SQLExecDirect(hStmt, 
		(SQLCHAR*) strSQL.GetString(), 
		SQL_NTS);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA)
	{
		FreeStmt(hStmt);
		return DB_OK;
	}
	else
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}
}

void COdbcManager::FreeStmt(SQLHSTMT hStmt)
{
	::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	hStmt = NULL;
}

SQLHSTMT COdbcManager::GetStmt(SQLHDBC hOdbc)
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)

	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, hOdbc, &hStmt);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		return hStmt;		
	}
	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
int COdbcManager::OpenUserDB(char* szOdbcName, 
							 char* szUsrID, 
							 char* szUsrPasswd)
{
	if (m_hOdbcUser != NULL)
		CloseDB(m_hEnvUser, m_hOdbcUser);

	SQLRETURN sReturn; // SQL return value (short)

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnvUser);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		return DB_ERROR;
	}

	// ODBC set environment
	// version
	sReturn = ::SQLSetEnvAttr(m_hEnvUser, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvUser);
		m_hEnvUser = NULL;
		return DB_ERROR;
	}

	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnvUser, &m_hOdbcUser);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, m_hOdbcUser);
		m_hOdbcUser = NULL;
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvUser);
		m_hEnvUser = NULL;
		return DB_ERROR;
	}

	// ODBC connect
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(m_hOdbcUser, 5, (void*) SQL_LOGIN_TIMEOUT, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{	
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvUser);
		m_hEnvUser = NULL;
		return DB_ERROR;
	}

	sReturn = ::SQLConnect(m_hOdbcUser, 
		(SQLCHAR*) szOdbcName,
		SQL_NTS,
		(SQLCHAR*) szUsrID,
		SQL_NTS,
		(SQLCHAR*) szUsrPasswd,
		SQL_NTS);

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvUser);
		m_hEnvUser = NULL;
		return DB_ERROR;
	}
}

int COdbcManager::OpenGameDB( const char* szOdbcName, const char* szUsrID, const char* szUsrPasswd)
{
	if (m_hOdbcGame != NULL)
		CloseDB(m_hEnvGame, m_hOdbcGame);

	SQLRETURN sReturn; // SQL return value (short)

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnvGame);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		return DB_ERROR;
	}

	// ODBC set environment
	// version
	sReturn = ::SQLSetEnvAttr(m_hEnvGame, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvGame);
		m_hEnvGame = NULL;
		return DB_ERROR;
	}

	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnvGame, &m_hOdbcGame);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, m_hOdbcGame);
		m_hOdbcGame = NULL;
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvGame);
		m_hEnvGame = NULL;
		return DB_ERROR;
	}

	// ODBC connect
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(m_hOdbcGame, 5, (void*) SQL_LOGIN_TIMEOUT, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{	
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvGame);
		m_hEnvGame = NULL;
		return DB_ERROR;
	}

	sReturn = ::SQLConnect(m_hOdbcGame, 
		(SQLCHAR*) szOdbcName,
		SQL_NTS,
		(SQLCHAR*) szUsrID,
		SQL_NTS,
		(SQLCHAR*) szUsrPasswd,
		SQL_NTS);

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{	
		return DB_OK;
	}
	else
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvGame);
		m_hEnvGame = NULL;
		return DB_ERROR;
	}
}

int COdbcManager::GetChaSeq()
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	SQLRETURN sReturn=0;
	int		  nMaxSeq = 0;	

	TCHAR szTemp[128] = {0};

	_snprintf( szTemp, 128, "SELECT Top 1 Seq From ChaLastInfo ORDER BY Seq Desc" );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	SQLINTEGER nSeqNum = 0, cbSeqNum = SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{

			::SQLGetData(hStmt, 1, SQL_C_LONG, &nSeqNum,        0, &cbSeqNum);

			nMaxSeq = nSeqNum;
		}
		else
		{
			break;
		}
	}
	FreeStmt(hStmt);

	return nMaxSeq;
}

int COdbcManager::GetSeqUserNum( const int nSeqNum, const int MONEY_VERSION )
{
    std::set<int> setAllUserNum;

	int nChaNum;
	int nUserNum;
	LONGLONG lnTemp;

	if( DB_ERROR == GetTempMoneyInfo( nSeqNum, nChaNum, lnTemp, MONEY_VERSION ) ) -1;	
	if( nChaNum == 0 ) 	return -1;
	if( DB_ERROR == GetGameMoneyInfo( nChaNum, nUserNum, lnTemp ) ) -1;


	return nUserNum;
}

BOOL COdbcManager::GetTempMoneyInfo( const int nSeqNum, int &nChaNum, LONGLONG &lnTempMoney, const int MONEY_VER )
{

	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	_snprintf( szTemp, 256, "SELECT ChaNum, Chamoney From ChaLastInfo Where Seq = '%d'", nSeqNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// 캐릭터 로그아웃 할때의 ChaMoney
	SQLINTEGER nNumber = 0, cbChaNum = SQL_NTS;
	SQLCHAR    szTempMoney[CHAR_TEMPMONEY] = {0}; SQLINTEGER cbTempMoney = SQL_NTS;


	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nNumber,     0, &cbChaNum);
			::SQLGetData(hStmt, 2, SQL_C_CHAR, szTempMoney, CHAR_TEMPMONEY, &cbTempMoney);


			nChaNum = nNumber;
			if (cbTempMoney != 0 && cbTempMoney != -1) // TempMoney 값 복사
			{					
				lnTempMoney = DECRYPTMONEY((const char*) szTempMoney,MONEY_VER);					
			}
		}
		else
		{
			break;
		}		
	}

	FreeStmt(hStmt);

	return DB_OK; 
}
BOOL COdbcManager::GetGameMoneyInfo( const int nChaNum, int &nUserNum, LONGLONG &lnGameMoney )
{ 
	SQLRETURN sReturn=0;


	// Get GameTable Info
	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	_snprintf( szTemp, 256, "SELECT ChaMoney, UserNum From ChaInfo WHERE ChaNum = %d", nChaNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	SQLINTEGER nNumber = 0, cbUserNum = SQL_NTS;
	SQLINTEGER cbChaMoney=SQL_NTS; 
	LONGLONG   lnMoney;
	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(hStmt, 1, SQL_C_SBIGINT, &lnMoney,  0, &cbChaMoney);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nNumber,     0, &cbUserNum);

			nUserNum	= nNumber;
			lnGameMoney	= lnMoney;
		}
		else
		{
			break;
		}
	}

	FreeStmt(hStmt);
	

	return DB_OK; 
}
BOOL COdbcManager::GetTempSTOMoneyInfo( const int nUserNum, LONGLONG &lnStorageMoney, const int MONEY_VER )
{
	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	_snprintf( szTemp, 128, "SELECT UserMoney FROM UserLastInfo WHERE UserNum=%d", nUserNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// 캐릭터 로그아웃 할때의 ChaMoney
	SQLINTEGER nNumber = 0, cbChaNum = SQL_NTS;
	SQLCHAR    szTempMoney[CHAR_TEMPMONEY] = {0}; SQLINTEGER cbTempMoney = SQL_NTS;


	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			::SQLGetData(hStmt, 1, SQL_C_CHAR, szTempMoney, CHAR_TEMPMONEY, &cbTempMoney);

			if (cbTempMoney != 0 && cbTempMoney != -1) // TempMoney 값 복사
			{					
				lnStorageMoney = DECRYPTMONEY((const char*) szTempMoney,MONEY_VER);					
			}
		}
		else
		{
			break;
		}		
	}

	FreeStmt(hStmt);

	return DB_OK; 
}
BOOL COdbcManager::GetSTOMoneyInfo( const int nUserNum, LONGLONG &lnStorageMoney )
{ 
	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	_snprintf( szTemp, 128, "SELECT UserMoney FROM UserInven WHERE UserNum=%u", nUserNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// 캐릭터 로그아웃 할때의 ChaMoney
	SQLINTEGER cbMoney=SQL_NTS; 
	LONGLONG   lnMoney;


	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt);

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			::SQLGetData(hStmt, 1, SQL_C_SBIGINT, &lnMoney,  0, &cbMoney);

			lnStorageMoney = lnMoney;
		}
		else
		{
			break;
		}		
	}

	FreeStmt(hStmt);
	return DB_OK; 
}


BOOL COdbcManager::UpdateTempMoneyInfo( const int nChaNum, LONGLONG lnGameMoney, const int MONEY_VER )
{

	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	char szEncryptChar[CHAR_TEMPMONEY] = {0};
	StringCchCopyA(szEncryptChar, CHAR_TEMPMONEY, ENCRYPTMONEY(lnGameMoney,MONEY_VER).c_str());

	_snprintf( szTemp, 256, "UPDATE ChaLastInfo SET ChaMoney = '%s' WHERE ChaNum = %d", szEncryptChar, nChaNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}	

	FreeStmt(hStmt);

	return DB_OK;
}
BOOL COdbcManager::UpdateGameMoneyInfo( const int nChaNum, LONGLONG lnGameMoney )
{

	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;


	_snprintf( szTemp, 256, "UPDATE ChaInfo SET ChaMoney = %I64d WHERE ChaNum = %d", lnGameMoney, nChaNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}	

	FreeStmt(hStmt);

	return DB_OK;
}
BOOL COdbcManager::UpdateTempSTOMoneyInfo( const int nUserNum, LONGLONG lnStorageMoney, const int MONEY_VER )
{

	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	char szEncryptChar[CHAR_TEMPMONEY] = {0};
	StringCchCopyA(szEncryptChar, CHAR_TEMPMONEY, ENCRYPTMONEY(lnStorageMoney,MONEY_VER).c_str());

	_snprintf( szTemp, 256, "UPDATE UserLastInfo SET UserMoney = '%s' WHERE UserNum = %d", szEncryptChar, nUserNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}	

	FreeStmt(hStmt);
	return DB_OK;
}
BOOL COdbcManager::UpdateSTOMoneyInfo( const int nUserNum, LONGLONG lnStorageMoney )
{

	SQLRETURN sReturn=0;

	TCHAR szTemp[256] = {0};
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
	if (!hStmt) return DB_ERROR;

	_snprintf( szTemp, 256, "UPDATE UserInven SET UserMoney = %I64d WHERE UserNum = %d", lnStorageMoney, nUserNum );

	sReturn = ::SQLExecDirect(hStmt,
		(SQLCHAR*) szTemp,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}	

	FreeStmt(hStmt);
	return DB_OK;
}

int	COdbcManager::DecryptGameMoney( const int nSeqNum, int &nGameChaNum, int &nGameUserNum, LONGLONG &lnChaMoney, const int MONEY_VERSION )
{

	int		 nChaNum = 0;
	int		 nUserNum = 0;
	LONGLONG lnTempMoney = 0;
	LONGLONG lnGameMoney = 0;


	// Get TempTable Info
	if( DB_ERROR == GetTempMoneyInfo( nSeqNum, nChaNum, lnTempMoney, MONEY_VERSION ) ) return DB_ERROR;	
	if( nChaNum == 0 ) 	return DB_ERROR;
	if( DB_ERROR == GetGameMoneyInfo( nChaNum, nUserNum, lnGameMoney ) ) return DB_ERROR;	

	if( lnGameMoney != lnTempMoney ) 
	{
		lnGameMoney = min(lnGameMoney,lnTempMoney);
	}

	nGameChaNum		= nChaNum;
	nGameUserNum	= nUserNum;
	lnChaMoney		= lnTempMoney;

	return DB_OK;

}

int	COdbcManager::DecryptSTOMoney( const int nGameUserNum, LONGLONG &lnStorageMoney, const int MONEY_VERSION )
{

	LONGLONG lnTempSTOMoney = 0;
	LONGLONG lnSTOMoney = 0;
	if( DB_ERROR == GetTempSTOMoneyInfo( nGameUserNum, lnTempSTOMoney, MONEY_VERSION ) ) return DB_ERROR;
	if( DB_ERROR == GetSTOMoneyInfo( nGameUserNum, lnSTOMoney ) ) return DB_ERROR;


	if( lnTempSTOMoney != lnSTOMoney ) 
	{
		lnSTOMoney = min(lnSTOMoney,lnTempSTOMoney);
	}

	lnStorageMoney = lnSTOMoney;

	return DB_OK;

}


BOOL COdbcManager::EncryptGameMoney( const int nChaNum, LONGLONG lnChaMoney, const int MONEY_VERSION )
{
	if( DB_ERROR == UpdateTempMoneyInfo( nChaNum, lnChaMoney, MONEY_VERSION) ) return DB_ERROR;
	if( DB_ERROR == UpdateGameMoneyInfo( nChaNum, lnChaMoney ) ) return DB_ERROR;


	return nChaNum;
}

BOOL COdbcManager::EncryptSTOMoney( const int nGameUserNum, LONGLONG lnStorageMoney, const int MONEY_VERSION )
{
	if( DB_ERROR == UpdateTempSTOMoneyInfo( nGameUserNum, lnStorageMoney, MONEY_VERSION ) ) return DB_ERROR;
	if( DB_ERROR == UpdateSTOMoneyInfo( nGameUserNum, lnStorageMoney ) ) return DB_ERROR;


	return nGameUserNum;
}



SMoneyInfo COdbcManager::UpdateGameMoney( const int nSeqNum, float fValue, BOOL bMultiply, const int MONEY_VERSION ) 
{
	SMoneyInfo returnValue;
	returnValue.nNumber = DB_ERROR;

	int nGameChaNum = 0;
	int nGameUserNum = 0;
	LONGLONG lnUserMoney = 0;
	if( DB_ERROR == DecryptGameMoney( nSeqNum, nGameChaNum, nGameUserNum, lnUserMoney, MONEY_VERSION ) ) return returnValue;

	returnValue.lnMoney1	= lnUserMoney;

	if( bMultiply )
		lnUserMoney *= fValue;
	else
		lnUserMoney += (LONGLONG)fValue;

	if( DB_ERROR == EncryptGameMoney( nGameChaNum, lnUserMoney, MONEY_VERSION ) ) return returnValue;
	
	returnValue.lnMoney2	= lnUserMoney;
	returnValue.nNumber = nGameChaNum;	

	return returnValue;
}

SMoneyInfo COdbcManager::UpdateSTOMoney( const int nGameUserNum, float fValue, BOOL bMultiply, const int MONEY_VERSION )
{
	SMoneyInfo returnValue;
	returnValue.nNumber = DB_ERROR;

	LONGLONG lnStorageMoney = 0;
	if( DB_ERROR == DecryptSTOMoney( nGameUserNum, lnStorageMoney, MONEY_VERSION ) ) return returnValue;

	returnValue.lnMoney1	= lnStorageMoney;

	if( bMultiply )
		lnStorageMoney *= fValue;
	else
		lnStorageMoney += (LONGLONG)fValue;


	if( DB_ERROR == EncryptSTOMoney( nGameUserNum, lnStorageMoney, MONEY_VERSION ) ) return returnValue;

	returnValue.lnMoney2	= lnStorageMoney;
	returnValue.nNumber			= nGameUserNum;	

	return returnValue;
}

BOOL COdbcManager::CheckGameMoney( const int nSeqNum, SMoneyInfo &moneyInfo, const int MONEY_VERSION )
{

	int		 nChaNum = 0;
	int		 nUserNum = 0;
	LONGLONG lnTempMoney = 0;
	LONGLONG lnGameMoney = 0;


	// Get TempTable Info
	if( DB_ERROR == GetTempMoneyInfo( nSeqNum, nChaNum, lnTempMoney, MONEY_VERSION ) ) return DB_ERROR;	
	if( nChaNum == 0 ) 	return DB_ERROR;
	if( DB_ERROR == GetGameMoneyInfo( nChaNum, nUserNum, lnGameMoney ) ) return DB_ERROR;	

	if( lnGameMoney != lnTempMoney ) 
	{
		moneyInfo.nNumber = nChaNum;
		moneyInfo.lnMoney1 = lnGameMoney;
		moneyInfo.lnMoney2 = lnTempMoney;
		return FALSE;
	}


	return TRUE;
}

BOOL COdbcManager::CheckSTOMoney( const int nGameUserNum, SMoneyInfo &moneyInfo, const int MONEY_VERSION )
{

	int		 nChaNum = 0;
	int		 nUserNum = 0;
	LONGLONG lnTempSTOMoney = 0;
	LONGLONG lnSTOMoney = 0;

	if( DB_ERROR == GetTempSTOMoneyInfo( nGameUserNum, lnTempSTOMoney, MONEY_VERSION ) ) return DB_ERROR;
	if( DB_ERROR == GetSTOMoneyInfo( nGameUserNum, lnSTOMoney ) ) return DB_ERROR;


	if( lnTempSTOMoney != lnSTOMoney ) 
	{
		moneyInfo.nNumber = nGameUserNum;
		moneyInfo.lnMoney1 = lnSTOMoney;
		moneyInfo.lnMoney2 = lnTempSTOMoney;
		return FALSE;
	}


	return TRUE;
}

SMoneyInfo COdbcManager::NewEncryptGameMoney_Ver100to101( const int nSeqNum )
{
	int nGameChaNum = 0;
	int nGameUserNum = 0;
	LONGLONG lnGameMoney = 0;

	// Get TempTable Info
	const int OLD_VER = 101;
	const int NEW_VER = 100;

	SMoneyInfo moneyInfo;
	moneyInfo.nNumber = DB_ERROR;

	if( DB_ERROR == DecryptGameMoney( nSeqNum, nGameChaNum, nGameUserNum, lnGameMoney, OLD_VER ) ) return moneyInfo;
	moneyInfo.lnMoney1 = lnGameMoney;

	if( DB_ERROR == EncryptGameMoney( nGameChaNum, lnGameMoney, NEW_VER ) ) return moneyInfo;
	if( DB_ERROR == DecryptGameMoney( nSeqNum, nGameChaNum, nGameUserNum, lnGameMoney, NEW_VER ) ) return moneyInfo;
	
	moneyInfo.nNumber = nGameChaNum;
	moneyInfo.lnMoney2 = lnGameMoney;
	return moneyInfo;
}


SMoneyInfo COdbcManager::NewEncryptSTOMoney_Ver100to101( const int nUserNum )
{
	LONGLONG lnSTOMoney = 0;

	// Get TempTable Info
	const int OLD_VER = 100;
	const int NEW_VER = 101;

	SMoneyInfo moneyInfo;
	moneyInfo.nNumber = DB_ERROR;

	if( DB_ERROR == DecryptSTOMoney( nUserNum, lnSTOMoney, OLD_VER ) ) return moneyInfo;
	moneyInfo.lnMoney1 = lnSTOMoney;

	if( DB_ERROR == EncryptSTOMoney( nUserNum, lnSTOMoney, NEW_VER ) ) return moneyInfo;
	if( DB_ERROR == DecryptSTOMoney( nUserNum, lnSTOMoney, NEW_VER ) ) return moneyInfo;

	moneyInfo.nNumber = nUserNum;
	moneyInfo.lnMoney2 = lnSTOMoney;
	return moneyInfo;
}
