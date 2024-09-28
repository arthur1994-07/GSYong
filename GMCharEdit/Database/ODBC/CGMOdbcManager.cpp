#include "StdAfx.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../DbBase.h"
#include "./CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGMOdbcManager* CGMOdbcManager::SelfInstance = NULL;

CGMOdbcManager::CGMOdbcManager(void)
	: IDBManager(gmce::ODBC)
    , m_hEnvUser(NULL)
	, m_hOdbcUser(NULL)
	, m_hEnvGame(NULL)
	, m_hOdbcGame(NULL)
	, m_strUserIP(_T(""))
{
	//m_nDBType = gmce::ODBC;
}

CGMOdbcManager* CGMOdbcManager::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CGMOdbcManager();

	return SelfInstance;
}

void CGMOdbcManager::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

CGMOdbcManager::~CGMOdbcManager(void)
{
	CloseUserDB();
	CloseGameDB();
}

void CGMOdbcManager::CloseUserDB()
{
	CloseDB(m_hEnvUser, m_hOdbcUser);
}

void CGMOdbcManager::CloseGameDB()
{
	CloseDB(m_hEnvGame, m_hOdbcGame);
}

int CGMOdbcManager::OpenDB(char* szOdbcName, 
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

void CGMOdbcManager::CloseDB(SQLHENV hEnv, SQLHDBC hOdbc)
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

int CGMOdbcManager::Insert(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int	CGMOdbcManager::Update(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int CGMOdbcManager::Delete(SQLHDBC hOdbc, CString strSQL)
{
	return ExecuteSQL(hOdbc, strSQL);
}

int	CGMOdbcManager::ExecuteSQL(SQLHDBC hOdbc, CString strSQL)
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

void CGMOdbcManager::FreeStmt(SQLHSTMT hStmt)
{
    ::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = NULL;
}

SQLHSTMT CGMOdbcManager::GetStmt(SQLHDBC hOdbc)
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
int CGMOdbcManager::OpenUserDB(char* szOdbcName, 
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

int CGMOdbcManager::OpenGameDB( const char* szOdbcName, const char* szUsrID, const char* szUsrPasswd)
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

///////////////////////////////////////////////////////////////////////////////
// User Database
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 0 로그인 실패
// 10 특별사용자 (게임방 업체 사장, 기자 등등)
// 20 GM 3 급
// 21 GM 2 급
// 22 GM 1 급
// 30 Master
int CGMOdbcManager::GmLogin(CString strGMID, CString strGMPassword, CString strUserIP)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
    if (!hStmt) return DB_ERROR;

	SQLRETURN sReturn=0;

	std::strstream strTemp;
	strTemp << "{call gm_login(";
	strTemp << "'" << strGMID.GetString() << "',";
	strTemp << "'" << strGMPassword.GetString() << "',";
	strTemp << "'" << strUserIP.GetString() << "',?)}";
	strTemp << '\0';

	SQLINTEGER nReturn=0;
	SQLINTEGER nOutput=0, cbOutput=SQL_NTS;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	// Bind the parameter.
	sReturn = ::SQLBindParameter(hStmt, 
								1, 
								SQL_PARAM_OUTPUT, 
								SQL_C_SLONG, 
								SQL_INTEGER, 
								0, 
								0,
								&nOutput, 
								0, 
								&nLenBuffer);
	
	if (sReturn == SQL_ERROR || sReturn == SQL_INVALID_HANDLE)
	{		
		FreeStmt(hStmt); 
		return DB_ERROR;
	}

	sReturn = ::SQLExecute(hStmt);	
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{			
		nReturn = DB_ERROR;
	}
	else
	{
		nReturn = nOutput;
	}
	FreeStmt(hStmt); 
	return nReturn;
}

int CGMOdbcManager::GetServerInfo(std::vector<viewServerInfo> &v)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
    if (!hStmt) return DB_ERROR;
	
	std::strstream strTemp;
	strTemp << "SELECT SGNum, SvrNum, SGName, SvrType, OdbcName, OdbcUserID, OdbcPassword FROM viewServerList" << '\0';
	
	SQLRETURN	sReturn;

	sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp.str(),
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}

    SQLINTEGER nSGNum, cbSGNum = SQL_NTS;
	SQLINTEGER nSvrNum, cbSvrNum = SQL_NTS;
	SQLCHAR    szSGName[256]; SQLINTEGER cbSGName = SQL_NTS;
	SQLINTEGER nSvrType, cbSvrType = SQL_NTS;
	SQLCHAR    szOdbcName[256]; SQLINTEGER cbOdbcName = SQL_NTS;
	SQLCHAR	   szOdbcUserID[256]; SQLINTEGER cbOdbcUserID = SQL_NTS;
	SQLCHAR	   szOdbcPassword[256]; SQLINTEGER cbOdbcPassword = SQL_NTS;


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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nSGNum,         0, &cbSGNum);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nSvrNum,        0, &cbSvrNum);
			::SQLGetData(hStmt, 3, SQL_C_CHAR, szSGName,      256, &cbSGName);
			::SQLGetData(hStmt, 4, SQL_C_LONG, &nSvrType,       0, &cbSvrType);
			::SQLGetData(hStmt, 5, SQL_C_CHAR, szOdbcName,    256, &cbOdbcName);
			::SQLGetData(hStmt, 6, SQL_C_CHAR, szOdbcUserID,  256, &cbOdbcUserID);
			::SQLGetData(hStmt, 7, SQL_C_CHAR, szOdbcPassword,256, &cbOdbcPassword);

			viewServerInfo sTemp;
            sTemp.nSGNum        = nSGNum;
            sTemp.nSvrNum       = nSvrNum;
			sTemp.nSvrType	    = nSvrType;

			sTemp.strName.Format("(%d:%d)", nSGNum, nSvrNum);

			if (cbSGName != 0 && cbSGName != -1)
			{
				sTemp.strSGName.Format(CString(szSGName));
				sTemp.strName += CString(szSGName);
			}
            if (cbOdbcName != 0 && cbOdbcName != -1) 
				sTemp.strOdbcName.Format(CString(szOdbcName));
			if (cbOdbcUserID != 0 && cbOdbcUserID != -1) 
				sTemp.strOdbcUserID.Format(CString(szOdbcUserID));
			if (cbOdbcPassword != 0 && cbOdbcPassword != -1) 
				sTemp.strOdbcPassword.Format(CString(szOdbcPassword));
            
            v.push_back(sTemp);
		}
		else
		{
			break;			
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int CGMOdbcManager::GetUserNum(CString strUserID)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;
    CString     strSQL;
    
    strSQL = "SELECT UserNum From UserInfo WHERE UserID='" + strUserID + "'";
    
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.GetString(), 
							SQL_NTS);	

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		SQLINTEGER nUserNum;	
		SQLINTEGER cbUserNum   = SQL_NTS;

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
				::SQLGetData(hStmt, 1, SQL_C_LONG, &nUserNum,  0, &cbUserNum);
				FreeStmt(hStmt);
				return nUserNum;
			}
			else
			{
				FreeStmt(hStmt);
				return DB_ERROR;			
			}
		}
	}
	else
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}       
}

int	CGMOdbcManager::GetAllChar(std::vector<AllCharNum> &v)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;
    
	CString strSQL;
	strSQL = "SELECT ChaNum, UserNum, ChaName From ChaInfo Order By ChaNum";

	SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.GetString(),
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaNum, nUserNum;
	SQLINTEGER cbChaNum   = SQL_NTS, cbUserNum  = SQL_NTS;
    
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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaNum,       0, &cbChaNum);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 3, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 

            AllCharNum sTemp;
            sTemp.ChaNum        = nChaNum;
            sTemp.UserNum       = nUserNum;
            if (cbChaName != 0 && cbChaName != -1) sTemp.ChaName.Format(CString(szChaName));
            
            v.push_back(sTemp);
		}
		else
		{
			break;			
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int	CGMOdbcManager::GetAllInven(std::vector<AllCharNum> &v)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;
    
	CString strSQL;
	// strSQL = " SELECT UserInvenNum, UserNum From UserInven";
	strSQL = "SELECT A.ChaNum, A.UserNum, A.ChaName FROM ChaInfo AS A,";
    strSQL = strSQL + " (SELECT  UserNum, MIN(ChaNum) AS ChaNum FROM ChaInfo GROUP BY UserNum) AS B";
	strSQL = strSQL + " WHERE A.ChaNum=B.ChaNum";
    strSQL = strSQL + " ORDER BY A.UserNum";

	SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.GetString(),
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaNum, nUserNum;
	SQLINTEGER cbChaNum   = SQL_NTS, cbUserNum  = SQL_NTS;
    
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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaNum,       0, &cbChaNum);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 3, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 

            AllCharNum sTemp;
            sTemp.ChaNum        = nChaNum;
            sTemp.UserNum       = nUserNum;
            if (cbChaName != 0 && cbChaName != -1) sTemp.ChaName.Format(CString(szChaName));
            
            v.push_back(sTemp);
		}
		else
		{
			break;			
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int CGMOdbcManager::GetExchangeItemLog( const std::vector<int>& GenType, const std::vector<int>& ExchangFlag, std::vector<ExchangeItemLog>& vOut )
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt)
        return DB_ERROR;

    CString csQuery;

    std::string strGenTypeNumbers = "";
    std::string strExchangeFlagNumbers = "";

    for ( size_t i = 0; i < GenType.size(); i++ )
    {
        if ( i == 0 )
            strGenTypeNumbers += sc::string::format( "%1%", GenType[i] );
        else
            strGenTypeNumbers += sc::string::format( ",%1%", GenType[i] );
    }

    for ( size_t i = 0; i < ExchangFlag.size(); i++ )
    {
        if ( i == 0 )
            strExchangeFlagNumbers += sc::string::format( "%1%", ExchangFlag[i] );
        else
            strExchangeFlagNumbers += sc::string::format( ",%1%", ExchangFlag[i] );
    }

    if ( GenType.size() > 0 && ExchangFlag.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "		                					    \
            "WHERE MakeNum <> 0 AND MakeType IN(%s) AND ExchangeFlag IN(%s) "	\
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "     	\
            ), strGenTypeNumbers.c_str(), strExchangeFlagNumbers.c_str() );
    }
    else if ( GenType.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "						                	    \
            "WHERE MakeNum <> 0 AND MakeType IN(%s) "                           \
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ), strGenTypeNumbers.c_str() );
    }
    else if ( ExchangFlag.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "             							    \
            "WHERE MakeNum <> 0 AND ExchangeFlag IN(%s) "                    	\
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ), strExchangeFlagNumbers.c_str() );
    }
    else
    {
        csQuery.Empty();
        csQuery = CString(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "             							    \
            "WHERE MakeNum <> 0 "                    	                        \
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ) );
    }


    SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
        (SQLCHAR*) csQuery.GetString(),
        SQL_NTS);

    if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        FreeStmt(hStmt);

        return DB_ERROR;
    }

    LONGLONG nMakeNum;
    SQLINTEGER nCount;
    SQLINTEGER cbMakeNum   = SQL_NTS, cbCount  = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_SBIGINT, &nMakeNum, 0, &cbMakeNum);
            ::SQLGetData(hStmt, 2, SQL_C_LONG, &nCount, 0, &cbCount);

            ExchangeItemLog ExchangeItem;
            ExchangeItem.MakeNum = nMakeNum;
            ExchangeItem.Count = nCount;

            vOut.push_back(ExchangeItem);
        }
        else
        {
            break;			
        }
    }
    FreeStmt(hStmt);

    return DB_OK;
}

int CGMOdbcManager::GetExchanageItemHistory( LONGLONG InMakeNum, std::vector<ExchangeItemHistory>& vOut )
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt)
        return DB_ERROR;

    CString csQuery;

    std::string strQueryTemp;
    strQueryTemp = sc::string::format(_T(
        "SELECT CONVERT(BIGINT, ExchangeNum) AS ExchangeNum, NIDMain, NIDSub, SGNum, SvrNum, FldNum "   \
        ", MakeType, CONVERT(BIGINT, MakeNum) AS MakeNum, ItemAmount, ItemFromFlag, ItemFrom "		    \
        ", ItemToFlag, ItemTo, ExchangeFlag, ExchangeDate, CONVERT(BIGINT, TradePrice) AS TradePrice "	\
        "FROM LogItemExchange "	            						                                    \
        "WHERE MakeNum = %1% "                                       	                                \
        "ORDER BY ExchangeDate DESC "                                  	                                \
        ), InMakeNum );

    csQuery.Empty();
    csQuery.Format( _T("%s"), strQueryTemp.c_str() );

    SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
        (SQLCHAR*) csQuery.GetString(),
        SQL_NTS);

    if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        FreeStmt(hStmt);

        return DB_ERROR;
    }

    LONGLONG ExchangeNum; SQLINTEGER cbExchangeNum = SQL_NTS;
    SQLINTEGER NIDMain; SQLINTEGER cbNIDMain = SQL_NTS;
    SQLINTEGER NIDSub; SQLINTEGER cbNIDSub = SQL_NTS;
    SQLINTEGER SGNum; SQLINTEGER cbSGNum = SQL_NTS;
    SQLINTEGER SvrNum; SQLINTEGER cbSvrNum = SQL_NTS;
    SQLINTEGER FldNum; SQLINTEGER cbFldNum = SQL_NTS;
    SQLINTEGER MakeType; SQLINTEGER cbMakeType = SQL_NTS;
    LONGLONG MakeNum; SQLINTEGER cbMakeNum = SQL_NTS;
    SQLINTEGER ItemAmount; SQLINTEGER cbItemAmount = SQL_NTS;
    SQLINTEGER ItemFromFlag; SQLINTEGER cbItemFromFlag = SQL_NTS;
    SQLINTEGER ItemFrom; SQLINTEGER cbItemFrom = SQL_NTS;
    SQLINTEGER ItemToFlag; SQLINTEGER cbItemToFlag = SQL_NTS;
    SQLINTEGER ItemTo; SQLINTEGER cbItemTo = SQL_NTS;
    SQLINTEGER ExchangeFlag; SQLINTEGER cbExchangeFlag = SQL_NTS;
    TIMESTAMP_STRUCT ExchangeDate; SQLINTEGER cbExchangeDate = SQL_NTS;
    LONGLONG TradePrice; SQLINTEGER cbTradePrice = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_SBIGINT, &ExchangeNum, 0, &cbExchangeNum);
            ::SQLGetData(hStmt, 2, SQL_C_LONG, &NIDMain, 0, &cbNIDMain);
            ::SQLGetData(hStmt, 3, SQL_C_LONG, &NIDSub, 0, &cbNIDSub);
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &SGNum, 0, &cbSGNum);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &SvrNum, 0, &cbSvrNum);
            ::SQLGetData(hStmt, 6, SQL_C_LONG, &FldNum, 0, &cbFldNum);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &MakeType, 0, &cbMakeType);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &MakeNum, 0, &cbMakeNum);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &ItemAmount, 0, &cbItemAmount);
            ::SQLGetData(hStmt, 10, SQL_C_LONG, &ItemFromFlag, 0, &cbItemFromFlag);
            ::SQLGetData(hStmt, 11, SQL_C_LONG, &ItemFrom, 0, &cbItemFrom);
            ::SQLGetData(hStmt, 12, SQL_C_LONG, &ItemToFlag, 0, &cbItemToFlag);
            ::SQLGetData(hStmt, 13, SQL_C_LONG, &ItemTo, 0, &cbItemTo);
            ::SQLGetData(hStmt, 14, SQL_C_LONG, &ExchangeFlag, 0, &cbExchangeFlag);
            ::SQLGetData(hStmt, 15, SQL_C_TYPE_TIMESTAMP, &ExchangeDate, 0, &cbExchangeDate);
            ::SQLGetData(hStmt, 16, SQL_C_LONG, &TradePrice, 0, &cbTradePrice);

            ExchangeItemHistory sExchangeHistory;
            sExchangeHistory.ExchangeNum = ExchangeNum;
            sExchangeHistory.NIDMain = NIDMain;
            sExchangeHistory.NIDSub = NIDSub;
            sExchangeHistory.SGNum = SGNum;
            sExchangeHistory.SvrNum = SvrNum;
            sExchangeHistory.FldNum = FldNum;
            sExchangeHistory.MakeType = MakeType;
            sExchangeHistory.MakeNum = MakeNum;
            sExchangeHistory.ItemAmount = ItemAmount;
            sExchangeHistory.ItemFromFlag = ItemFromFlag;
            sExchangeHistory.ItemFrom = ItemFrom;
            sExchangeHistory.ItemToFlag = ItemToFlag;
            sExchangeHistory.ItemTo = ItemTo;
            sExchangeHistory.ExchangeFlag = ExchangeFlag;
            sc::time::AdjustTime(ExchangeDate);
            sExchangeHistory.ExchangeDate = sc::time::GetTime(ExchangeDate);
            sExchangeHistory.TradePrice = TradePrice;

            vOut.push_back(sExchangeHistory);
        }
        else
        {
            break;			
        }
    }
    FreeStmt(hStmt);

    return DB_OK;
}

int CGMOdbcManager::SearchCharacter(const std::string& Query, std::vector<CharInfo>& v)
{
    AfxMessageBox("CGMOdbcManager::SearchCharacter. Use ADO");
    return DB_ERROR;
}

int CGMOdbcManager::SearchCharacter(std::strstream& strSearch, std::vector<CharInfo> &v)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
							 (SQLCHAR*) strSearch.str(),
							 SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
        return DB_ERROR;
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaNum, nUserNum,  nChaTribe, nChaClass,    nSGNum;
    SQLINTEGER nChaLevel, nChaPower, nChaStrong,   nChaStrength, nChaSpirit, nChaOnline;
    SQLINTEGER nChaDex,   nChaIntel, nChaStRemain, nChaDeleted;
	//  nChaHP,       nChaMP;
	SQLINTEGER cbChaNum    = SQL_NTS, cbUserNum    = SQL_NTS, cbChaTribe = SQL_NTS, cbChaClass    = SQL_NTS, cbSGNum       = SQL_NTS;
    SQLINTEGER cbChaLevel  = SQL_NTS, cbChaPower   = SQL_NTS, cbChaStrong   = SQL_NTS, cbChaStrength = SQL_NTS, cbChaSpirit = SQL_NTS;
    SQLINTEGER cbChaDex    = SQL_NTS, cbChaIntel   = SQL_NTS, cbChaStRemain = SQL_NTS, cbChaHP       = SQL_NTS, cbChaMP     = SQL_NTS;
	SQLINTEGER cbChaOnline = SQL_NTS, cbChaDeleted = SQL_NTS;

    LONGLONG   llChaExp, llChaMoney;
    SQLINTEGER cbChaExp = SQL_NTS, cbChaMoney = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaNum,       0, &cbChaNum);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 3, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &nChaClass,      0, &cbChaClass);

            ::SQLGetData(hStmt, 6, SQL_C_LONG, &nSGNum,         0, &cbSGNum);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &nChaPower,      0, &cbChaPower);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
            ::SQLGetData(hStmt,10, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);

            ::SQLGetData(hStmt,11, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);
            ::SQLGetData(hStmt,12, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
            ::SQLGetData(hStmt,13, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(hStmt,14, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
            ::SQLGetData(hStmt,15, SQL_C_SBIGINT, &llChaMoney,  0, &cbChaMoney);

            ::SQLGetData(hStmt,16, SQL_C_SBIGINT, &llChaExp,    0, &cbChaExp);
			::SQLGetData(hStmt,17, SQL_C_LONG,    &nChaOnline,  0, &cbChaSpirit);
			::SQLGetData(hStmt,18, SQL_C_LONG,    &nChaDeleted, 0, &cbChaDeleted);

            CharInfo sTemp;
            sTemp.ChaNum        = nChaNum;
            sTemp.UserNum       = nUserNum;
            if (cbChaName != 0 && cbChaName != -1) 
            {	
				// sTemp.ChaName.Format(_T("%s"), szChaName);
				sTemp.ChaName = szChaName;
			}
            sTemp.ChaTribe      = nChaTribe;
            sTemp.ChaClass      = nChaClass;

            sTemp.SGNum         = nSGNum;
            sTemp.ChaLevel      = nChaLevel;
            sTemp.ChaPower      = nChaPower;
            sTemp.ChaStrong     = nChaStrong;
            sTemp.ChaStrength   = nChaStrength;

            sTemp.ChaSpirit     = nChaSpirit;
            sTemp.ChaDex        = nChaDex;
            sTemp.ChaIntel      = nChaIntel;
            sTemp.ChaStRemain   = nChaStRemain;
            sTemp.ChaMoney      = llChaMoney;

            sTemp.ChaExp        = llChaExp;

			sTemp.ChaOnline     = nChaOnline;
			sTemp.ChaDeleted    = nChaDeleted;			

            v.push_back(sTemp);


            /*SQL_BIGINT
            SQL_C_SBIGINT

            CString m_strMoney
            m_strMoney.Format("I64d" , money);*/

		}
		else
		{
			break;			
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int CGMOdbcManager::CreateNewCharacter(SCHARDATA2* pCharData2, int nSvrNum)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;    

	int nChaNum = 0;	
	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = (DWORD) nSvrNum;

	// SCHARDATA2 sCha;
	// sCha.operator =n;
	
	// 현재 생성한 캐릭터 갯수를 가져온다.
	std::strstream strTemp2;
	strTemp2 << "SELECT COUNT(*) FROM ChaInfo WHERE chadeleted=0 and UserNum=";
	strTemp2 << dwUserNum;
	strTemp2 << '\0';

	sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp2.str(), 
							SQL_NTS);
	strTemp2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}

	SQLINTEGER nChaMaxNum, cbChaMaxNum=SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaMaxNum, 0, &cbChaMaxNum);
			if (nChaMaxNum >= MAX_ONESERVERCHAR_NUM_GMTOOL )
			{
				FreeStmt(hStmt);
				return DB_CHA_MAX;
			}
			break;			
		}
	}
	FreeStmt(hStmt);

	// 새로운 캐릭터를 생성한다.
	// 민첩 ChaDex, wDex
	// 지력 ChaIntel, wInt
	// 체력 ChaStrong, wStr
	// 힘   ChaPower, wPow
	// 정신 ChaSpirit, wSpi
	// 근력 ChaStrength, wSta
	std::strstream strTemp;
	strTemp << "INSERT INTO ChaInfo(UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel,";
	strTemp << "ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, ";
	strTemp << "ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, ";
	strTemp << "ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, ";
	strTemp << "ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ,";
	strTemp << "ChaSkills, ChaSkillSlot, ChaPutOnItems, ChaInven ) Values ( ";
		
	strTemp << dwUserNum << ",";
	strTemp << dwSvrNum << ",";
	strTemp << "'" << pCharData2->m_szName << "',";
	strTemp << pCharData2->m_emTribe << ","; 
	strTemp << pCharData2->m_emClass << ",";

    strTemp << pCharData2->m_nBright << ",";
	strTemp << pCharData2->m_wLevel << ",";
	strTemp << pCharData2->GetInvenMoney() << ","; 
	strTemp << pCharData2->m_sStats.wDex << ","; // 민첩 ChaDex, wDex
	strTemp << pCharData2->m_sStats.wInt << ","; // 지력 ChaIntel, wInt

	strTemp << pCharData2->m_sStats.wStr << ","; // 체력 ChaStrong, wStr
	strTemp << pCharData2->m_sStats.wPow << ","; // 힘   ChaPower, wPow
	strTemp << pCharData2->m_sStats.wSpi << ","; // 정신 ChaSpirit, wSpi
	strTemp << pCharData2->m_sStats.wSta << ","; // 근력 ChaStrength, wSta
	strTemp << pCharData2->m_wStatsPoint << ",";

	strTemp << pCharData2->m_powerAttack << ",";
	strTemp << pCharData2->m_powerDefence << ",";
	strTemp << pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE] << ",";
	strTemp << pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE] << ",";
	strTemp << pCharData2->m_sExperience.lnNow << ",";

	strTemp << pCharData2->m_dwSkillPoint << ",";		
	strTemp << pCharData2->m_sHP.nNow << ",";
	strTemp << pCharData2->m_sMP.nNow << ",";
	strTemp << pCharData2->m_sSP.nNow << ",";
	strTemp << pCharData2->m_wPK << ",";

	strTemp << pCharData2->m_sStartMapID.dwID << ",";
	strTemp << pCharData2->m_dwStartGate << ",";
	strTemp << pCharData2->m_vStartPos.x << ",";
	strTemp << pCharData2->m_vStartPos.y << ",";
	strTemp << pCharData2->m_vStartPos.z << ",";

	strTemp << " '', '', '', '')";
	strTemp << '\0';

	SQLHSTMT hStmt2 = GetStmt(m_hOdbcGame);
    if (!hStmt2) return DB_ERROR;

	sReturn = ::SQLExecDirect(hStmt2,
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt2);
        return DB_ERROR;
	}
	
	if (sReturn == SQL_ERROR) 
	{
		FreeStmt(hStmt2);
		return DB_CHA_DUF;
	}
	/*
	sReturn = ::SQLExecDirect(hStmt2,
							(SQLCHAR*) "SELECT @@IDENTITY", 
							SQL_NTS);
	*/

	sReturn = ::SQLExecDirect(hStmt2,
							  (SQLCHAR*) "SELECT cast(SCOPE_IDENTITY() as int)", 
							  SQL_NTS);
	
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt2);
        return DB_ERROR;
	}

	SQLINTEGER nChaNewNum, cbChaNewNum=SQL_NTS;

	while(TRUE)
	{
		sReturn = ::SQLFetch(hStmt2);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            FreeStmt(hStmt2);
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
            ::SQLGetData(hStmt2, 1, SQL_C_LONG, &nChaNewNum, 0, &cbChaNewNum);
			pCharData2->m_CharDbNum = nChaNewNum;
			FreeStmt(hStmt2);
			break;
		}
	}
	
	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Character Skill 
	pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
	ByteStream.GetBuffer( pBuffer, dwSize);
	WriteImage("ChaInfo.ChaSkills", nChaNewNum, (char *) pBuffer, dwSize );

	// Character Skill Quikc Slot
	pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
	ByteStream.GetBuffer( pBuffer, dwSize);
	WriteImage("ChaInfo.ChaSkillSlot", nChaNewNum, (char *) pBuffer, dwSize );

	// Character Put on item
	pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);
	WriteImage("ChaInfo.ChaPutOnItems", nChaNewNum, (char *) pBuffer, dwSize);

	// Character SkillFact
	pCharData2->GETSKILLFACT_BYBUF(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);		
	WriteImage("ChaInfo.ChaSkillFact", nChaNewNum, (char *) pBuffer, dwSize);

	// Character Inventory
	ByteStream.ClearBuffer();
	pCharData2->GETINVENTORYE_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);		
	WriteImage("ChaInfo.ChaInven", nChaNewNum, (char *) pBuffer, dwSize);

	// User Inventory
	// 1. Check User Inven		
	// 2. If exist skip
	// 3. not exist, write iventory image
	BOOL bInven = CheckInven(dwUserNum);
	if (!bInven)
	{
		MakeUserInven(dwUserNum);
	}


	return nChaNewNum;	
}

int CGMOdbcManager::CharacterInfoBackup( int nChaNum )
{
    sc::writeLogError("CGMOdbcManager::CharacterInfoBackup. Use ADO");
    return DB_ERROR;
}

/* // NOT USED

int CGMOdbcManager::GetChaInfoFromChaName(CString strChaName, std::vector<CharInfo> &v)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;    
    
    std::strstream strSQL;
/ *
	strSQL << " SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline ";
    strSQL << " From ChaInfo WHERE ChaName LIKE '%" << strChaName << "%'";
    strSQL << '\0';
* /

	strSQL << " IF EXISTS(SELECT name FROM sysobjects WHERE name = N'ChaNameInfo' AND type = 'U') ";
	strSQL << " Begin " ;
	strSQL << " SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline ";	
	strSQL << "	From ChaInfo where ChaNum in (Select ChaNum From ChaNameInfo Where ChaName LIKE '%" << strChaName << "%') ";
	strSQL << " Order by UserNum Desc ";
	strSQL << " End ";
	strSQL << " Else ";
	strSQL << " Begin ";
	strSQL << " SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline ";
	strSQL << "	From ChaInfo Where ChaName LIKE '%" << strChaName << "%' ";
	strSQL << "	Order by UserNum desc ";
	strSQL << " End ";
	strSQL << '\0';
    
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.str(), 
							SQL_NTS);
	strSQL.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaNum, nUserNum,  nChaTribe, nChaClass,    nSGNum;
    SQLINTEGER nChaLevel, nChaPower, nChaStrong,   nChaStrength, nChaSpirit;
    SQLINTEGER nChaDex,   nChaIntel, nChaStRemain, nChaOnline;
	// nChaHP,       nChaMP;
	SQLINTEGER cbChaNum   = SQL_NTS, cbUserNum  = SQL_NTS, cbChaTribe = SQL_NTS, cbChaClass    = SQL_NTS, cbSGNum       = SQL_NTS;
    SQLINTEGER cbChaLevel = SQL_NTS, cbChaPower = SQL_NTS, cbChaStrong   = SQL_NTS, cbChaStrength = SQL_NTS, cbChaSpirit = SQL_NTS;
    SQLINTEGER cbChaDex   = SQL_NTS, cbChaIntel = SQL_NTS, cbChaStRemain = SQL_NTS, cbChaHP       = SQL_NTS, cbChaMP     = SQL_NTS;
	SQLINTEGER cbChaOnline = SQL_NTS;

    LONGLONG   llChaExp, llChaMoney;
    SQLINTEGER cbChaExp = SQL_NTS, cbChaMoney = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaNum,       0, &cbChaNum);
			::SQLGetData(hStmt, 2, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 3, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &nChaClass,      0, &cbChaClass);

            ::SQLGetData(hStmt, 6, SQL_C_LONG, &nSGNum,         0, &cbSGNum);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &nChaPower,      0, &cbChaPower);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
            ::SQLGetData(hStmt,10, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);

            ::SQLGetData(hStmt,11, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);
            ::SQLGetData(hStmt,12, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
            ::SQLGetData(hStmt,13, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(hStmt,14, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
            ::SQLGetData(hStmt,15, SQL_C_SBIGINT, &llChaMoney,     0, &cbChaMoney);

            ::SQLGetData(hStmt,16, SQL_C_SBIGINT, &llChaExp,    0, &cbChaExp);
			::SQLGetData(hStmt,17, SQL_C_LONG,    &nChaOnline,  0, &cbChaOnline);

            CharInfo sTemp;
            sTemp.ChaNum        = nChaNum;
            sTemp.UserNum       = nUserNum;
            if (cbChaName != 0 && cbChaName != -1) sTemp.ChaName.Format(CString(szChaName));
            sTemp.ChaTribe      = nChaTribe;
            sTemp.ChaClass      = nChaClass;

            sTemp.SGNum         = nSGNum;
            sTemp.ChaLevel      = nChaLevel;
            sTemp.ChaPower      = nChaPower;
            sTemp.ChaStrong     = nChaStrong;
            sTemp.ChaStrength   = nChaStrength;

            sTemp.ChaSpirit     = nChaSpirit;
            sTemp.ChaDex        = nChaDex;
            sTemp.ChaIntel      = nChaIntel;
            sTemp.ChaStRemain   = nChaStRemain;
            sTemp.ChaMoney      = llChaMoney;

            sTemp.ChaExp        = llChaExp;
			sTemp.ChaOnline     = nChaOnline;

            v.push_back(sTemp);


            / *SQL_BIGINT
            SQL_C_SBIGINT

            CString m_strMoney
            m_strMoney.Format("I64d" , money);* /

		}
		else
		{
			break;			
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int CGMOdbcManager::GetChaInfoFromUserNum(int nUserNum, std::vector<CharInfo> &v)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;    
    
    std::strstream strSQL;
	strSQL << " SELECT ChaNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline";
    strSQL << " From ChaInfo WHERE UserNum=" << nUserNum;
    strSQL << '\0';
    
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.str(), 
							SQL_NTS);
	strSQL.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nChaNum,  nChaTribe, nChaClass,    nSGNum;
    SQLINTEGER nChaLevel, nChaPower, nChaStrong,   nChaStrength, nChaSpirit;
    SQLINTEGER nChaDex,   nChaIntel, nChaStRemain, nChaOnline;
	// , nChaHP,       nChaMP;
	SQLINTEGER cbChaNum  = SQL_NTS, cbChaTribe = SQL_NTS, cbChaClass    = SQL_NTS, cbSGNum       = SQL_NTS;
    SQLINTEGER cbChaLevel = SQL_NTS, cbChaPower = SQL_NTS, cbChaStrong   = SQL_NTS, cbChaStrength = SQL_NTS, cbChaSpirit = SQL_NTS;
    SQLINTEGER cbChaDex   = SQL_NTS, cbChaIntel = SQL_NTS, cbChaStRemain = SQL_NTS, cbChaHP       = SQL_NTS, cbChaMP     = SQL_NTS;
	SQLINTEGER cbChaOnline = SQL_NTS;

    LONGLONG   llChaExp, llChaMoney;
    SQLINTEGER cbChaExp = SQL_NTS, cbChaMoney = SQL_NTS;

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
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nChaNum,        0, &cbChaNum);
            ::SQLGetData(hStmt, 2, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 
            ::SQLGetData(hStmt, 3, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nChaClass,      0, &cbChaClass);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &nSGNum,         0, &cbSGNum);

            ::SQLGetData(hStmt, 6, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &nChaPower,      0, &cbChaPower);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);
            ::SQLGetData(hStmt,10, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);

            ::SQLGetData(hStmt,11, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
            ::SQLGetData(hStmt,12, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(hStmt,13, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
            ::SQLGetData(hStmt,14, SQL_C_SBIGINT, &llChaMoney,     0, &cbChaMoney);
            ::SQLGetData(hStmt,15, SQL_C_SBIGINT, &llChaExp,       0, &cbChaExp);

			::SQLGetData(hStmt,16, SQL_C_LONG, &nChaOnline,   0, &cbChaOnline);
			

            CharInfo sTemp;
            sTemp.UserNum       = nUserNum;
            sTemp.ChaNum        = nChaNum;
            if (cbChaName != 0 && cbChaName != -1) sTemp.ChaName.Format(CString(szChaName));
            sTemp.ChaTribe      = nChaTribe;
            sTemp.ChaClass      = nChaClass;

            sTemp.SGNum         = nSGNum;
            sTemp.ChaLevel      = nChaLevel;
            sTemp.ChaPower      = nChaPower;
            sTemp.ChaStrong     = nChaStrong;
            sTemp.ChaStrength   = nChaStrength;

            sTemp.ChaSpirit     = nChaSpirit;
            sTemp.ChaDex        = nChaDex;
            sTemp.ChaIntel      = nChaIntel;
            sTemp.ChaStRemain   = nChaStRemain;
            sTemp.ChaMoney      = llChaMoney;

            sTemp.ChaExp        = llChaExp;
			sTemp.ChaOnline     = nChaOnline;

            v.push_back(sTemp);


            / *SQL_BIGINT
            SQL_C_SBIGINT

            CString m_strMoney
            m_strMoney.Format("I64d" , money);* /

		}
		else
		{
			break;		
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}

int CGMOdbcManager::GetChaInfoFromChaNum(int nChaNum, std::vector<CharInfo> &v)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;    
    
    std::strstream strSQL;
	strSQL << " SELECT UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline ";
    strSQL << " From ChaInfo WHERE ChaNum=" << nChaNum;
    strSQL << '\0';
    
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.str(), 
							SQL_NTS);
	strSQL.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];
    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nUserNum,  nChaTribe, nChaClass,    nSGNum;
    SQLINTEGER nChaLevel, nChaPower, nChaStrong,   nChaStrength, nChaSpirit;
    SQLINTEGER nChaDex,   nChaIntel, nChaStRemain, nChaOnline;
	// nChaHP,       nChaMP;
	SQLINTEGER cbUserNum  = SQL_NTS, cbChaTribe = SQL_NTS, cbChaClass    = SQL_NTS, cbSGNum       = SQL_NTS;
    SQLINTEGER cbChaLevel = SQL_NTS, cbChaPower = SQL_NTS, cbChaStrong   = SQL_NTS, cbChaStrength = SQL_NTS, cbChaSpirit = SQL_NTS;
    SQLINTEGER cbChaDex   = SQL_NTS, cbChaIntel = SQL_NTS, cbChaStRemain = SQL_NTS, cbChaHP       = SQL_NTS, cbChaMP     = SQL_NTS;
	SQLINTEGER cbChaOnline = SQL_NTS;

    LONGLONG   llChaExp, llChaMoney;
    SQLINTEGER cbChaExp = SQL_NTS, cbChaMoney = SQL_NTS;

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
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 2, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 
            ::SQLGetData(hStmt, 3, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nChaClass,      0, &cbChaClass);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &nSGNum,         0, &cbSGNum);

            ::SQLGetData(hStmt, 6, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &nChaPower,      0, &cbChaPower);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);
            ::SQLGetData(hStmt,10, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);

            ::SQLGetData(hStmt,11, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
            ::SQLGetData(hStmt,12, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(hStmt,13, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
            ::SQLGetData(hStmt,14, SQL_C_SBIGINT, &llChaMoney,     0, &cbChaMoney);
            ::SQLGetData(hStmt,15, SQL_C_SBIGINT, &llChaExp,       0, &cbChaExp);

			::SQLGetData(hStmt,16, SQL_C_LONG, &nChaOnline,     0, &cbChaOnline);

            CharInfo sTemp;
            
            sTemp.ChaNum        = nChaNum;

            sTemp.UserNum       = nUserNum;            
            if (cbChaName != 0 && cbChaName != -1) sTemp.ChaName.Format(CString(szChaName));
            sTemp.ChaTribe      = nChaTribe;
            sTemp.ChaClass      = nChaClass;
            sTemp.SGNum         = nSGNum;

            sTemp.ChaLevel      = nChaLevel;
            sTemp.ChaPower      = nChaPower;
            sTemp.ChaStrong     = nChaStrong;
            sTemp.ChaStrength   = nChaStrength;
            sTemp.ChaSpirit     = nChaSpirit;

            sTemp.ChaDex        = nChaDex;
            sTemp.ChaIntel      = nChaIntel;
            sTemp.ChaStRemain   = nChaStRemain;
            sTemp.ChaMoney      = llChaMoney;
            sTemp.ChaExp        = llChaExp;
			sTemp.ChaOnline     = nChaOnline;

            v.push_back(sTemp);


            / *SQL_BIGINT
            SQL_C_SBIGINT

            CString m_strMoney
            m_strMoney.Format("I64d" , money);* /

		}
		else
		{
			break;		
		}
    }
    FreeStmt(hStmt);
    return DB_OK;
}
*/




int CGMOdbcManager::GetCharacterInfo(int nChaNum, int nUserNumber, GLCHARLOGIC* pChaData2)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;    
    
    std::strstream strSQL;
	strSQL << " SELECT UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strSQL << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strSQL << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ";
	strSQL << " ChaHP, ChaMP, ChaStartMap, ChaStartGate, ChaPosX, ";
	strSQL << " ChaPosY, ChaPosZ, GuNum, ChaBright, ChaAttackP, ";
	strSQL << " ChaDefenseP, ChaFightA, ChaShootA, ChaSP, ChaPK, ";
	strSQL << " ChaSkillPoint, ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, ";	
	strSQL << " ChaSchool, ChaSex, ChaHair, ChaFace, ChaLiving, ChaSkillPoint, ";    
    strSQL << " ChaInvenLine, ChaReturnMap, ChaReturnPosX, ChaReturnPosY, ChaReturnPosZ, ";
    strSQL << " ChaCP ";
    strSQL << " From ChaInfo WHERE ChaNum=" << nChaNum;
    strSQL << '\0';
    
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strSQL.str(), 
							SQL_NTS);
	strSQL.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	
    SQLCHAR    szChaName[256];


    SQLINTEGER cbChaName = SQL_NTS;
    SQLINTEGER nUserNum, cbUserNum=SQL_NTS;
	SQLINTEGER nChaTribe, cbChaTribe=SQL_NTS;
	SQLINTEGER nChaClass, cbChaClass=SQL_NTS;
	SQLINTEGER nSGNum, cbSGNum=SQL_NTS;

    SQLINTEGER nChaLevel, cbChaLevel=SQL_NTS;
	SQLINTEGER nChaPower, cbChaPower=SQL_NTS; 
	SQLINTEGER nChaStrong, cbChaStrong=SQL_NTS;   
	SQLINTEGER nChaStrength, cbChaStrength=SQL_NTS; 
	SQLINTEGER nChaSpirit, cbChaSpirit=SQL_NTS;

	SQLINTEGER nChaDex, cbChaDex=SQL_NTS;
	SQLINTEGER nChaIntel, cbChaIntel=SQL_NTS;
	SQLINTEGER nChaStRemain, cbChaStRemain=SQL_NTS; 
	SQLINTEGER nChaHP, cbChaHP=SQL_NTS;
	SQLINTEGER nChaMP, cbChaMP=SQL_NTS;
	SQLINTEGER nChaCP, cbChaCP=SQL_NTS;
	
	SQLINTEGER nChaStartMap, cbChaStartMap=SQL_NTS;
	SQLINTEGER nChaStartGate, cbChaStartGate=SQL_NTS;


	SQLFLOAT fChaPosX; SQLINTEGER cbChaPosX=SQL_NTS;
	SQLFLOAT fChaPosY; SQLINTEGER cbChaPosY=SQL_NTS;
	SQLFLOAT fChaPosZ; SQLINTEGER cbChaPosZ=SQL_NTS;

	SQLINTEGER nGuNum, cbGuNum=SQL_NTS;
	SQLINTEGER nChaBright, cbChaBright=SQL_NTS;
	SQLINTEGER nChaAttackP, cbChaAttackP=SQL_NTS;
	SQLINTEGER nChaDefenseP, cbChaDefenseP=SQL_NTS;
	SQLINTEGER nChaFightA, cbChaFightA=SQL_NTS;

	SQLINTEGER nChaShootA, cbChaShootA=SQL_NTS;
	SQLINTEGER nChaSP, cbChaSP=SQL_NTS;
	SQLINTEGER nChaPK, cbChaPK=SQL_NTS;
	SQLINTEGER nChaSkillPoint, cbChaSkillPoint=SQL_NTS;
	SQLINTEGER nChaSaveMap, cbChaSaveMap=SQL_NTS;

	SQLFLOAT fChaSavePosX; SQLINTEGER cbChaSavePosX=SQL_NTS;
	SQLFLOAT fChaSavePosY; SQLINTEGER cbChaSavePosY=SQL_NTS;
	SQLFLOAT fChaSavePosZ; SQLINTEGER cbChaSavePosZ=SQL_NTS;

	SQLINTEGER nChaSchool, cbChaSchool=SQL_NTS;
	SQLINTEGER nChaFace, cbChaFace=SQL_NTS;
	SQLINTEGER nChaHair, cbChaHair=SQL_NTS;
	SQLINTEGER nChaSex, cbChaSex=SQL_NTS;
	SQLINTEGER nChaLiving, cbChaLiving=SQL_NTS;
    
	LONGLONG   llChaExp, llChaMoney;
	SQLINTEGER cbChaExp=SQL_NTS, cbChaMoney=SQL_NTS;
	SQLINTEGER nChaSkillPoints, cbChaSkillPoints=SQL_NTS;

    SQLINTEGER nChaReturnMap  = 0; SQLINTEGER cbChaReturnMap  = SQL_NTS;
	SQLFLOAT   fChaReturnPosX = 0; SQLINTEGER cbChaReturnPosX = SQL_NTS;
	SQLFLOAT   fChaReturnPosY = 0; SQLINTEGER cbChaReturnPosY = SQL_NTS;
	SQLFLOAT   fChaReturnPosZ = 0; SQLINTEGER cbChaReturnPosZ = SQL_NTS;

    SQLINTEGER nChaInvenLine = 0; SQLINTEGER cbChaInvenLine = SQL_NTS;

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
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
            ::SQLGetData(hStmt, 2, SQL_C_CHAR, szChaName   ,  256, &cbChaName); 
            ::SQLGetData(hStmt, 3, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(hStmt, 4, SQL_C_LONG, &nChaClass,      0, &cbChaClass);
            ::SQLGetData(hStmt, 5, SQL_C_LONG, &nSGNum,         0, &cbSGNum);

            ::SQLGetData(hStmt, 6, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
            ::SQLGetData(hStmt, 7, SQL_C_LONG, &nChaPower,      0, &cbChaPower);
            ::SQLGetData(hStmt, 8, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
            ::SQLGetData(hStmt, 9, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);
            ::SQLGetData(hStmt,10, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);

            ::SQLGetData(hStmt,11, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
            ::SQLGetData(hStmt,12, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(hStmt,13, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
            ::SQLGetData(hStmt,14, SQL_C_SBIGINT, &llChaMoney,  0, &cbChaMoney);
            ::SQLGetData(hStmt,15, SQL_C_SBIGINT, &llChaExp,    0, &cbChaExp);

			::SQLGetData(hStmt,16, SQL_C_LONG, &nChaHP,			0, &cbChaHP);
			::SQLGetData(hStmt,17, SQL_C_LONG, &nChaMP,			0, &cbChaMP);
			::SQLGetData(hStmt,18, SQL_C_LONG, &nChaStartMap,	0, &cbChaStartMap);
			::SQLGetData(hStmt,19, SQL_C_LONG, &nChaStartGate,  0, &cbChaStartGate);
			::SQLGetData(hStmt,20, SQL_C_DOUBLE, &fChaPosX,		0, &cbChaPosX);

			::SQLGetData(hStmt,21, SQL_C_DOUBLE, &fChaPosY,		0, &cbChaPosY);
			::SQLGetData(hStmt,22, SQL_C_DOUBLE, &fChaPosZ,		0, &cbChaPosZ);
			::SQLGetData(hStmt,23, SQL_C_LONG, &nGuNum,			0, &cbGuNum);
			::SQLGetData(hStmt,24, SQL_C_LONG, &nChaBright,     0, &cbChaBright);
			::SQLGetData(hStmt,25, SQL_C_LONG, &nChaAttackP,    0, &cbChaAttackP);

			::SQLGetData(hStmt,26, SQL_C_LONG, &nChaDefenseP,   0, &cbChaDefenseP);
			::SQLGetData(hStmt,27, SQL_C_LONG, &nChaFightA,     0, &cbChaFightA);
			::SQLGetData(hStmt,28, SQL_C_LONG, &nChaShootA,     0, &cbChaShootA);
			::SQLGetData(hStmt,29, SQL_C_LONG, &nChaSP,			0, &cbChaSP);
			::SQLGetData(hStmt,30, SQL_C_LONG, &nChaPK,			0, &cbChaPK);

			::SQLGetData(hStmt,31, SQL_C_LONG, &nChaSkillPoint, 0, &cbChaSkillPoint);
			::SQLGetData(hStmt,32, SQL_C_LONG, &nChaSaveMap,    0, &cbChaSaveMap);
			::SQLGetData(hStmt,33, SQL_C_DOUBLE, &fChaSavePosX,   0, &cbChaSavePosX);
			::SQLGetData(hStmt,34, SQL_C_DOUBLE, &fChaSavePosY,   0, &cbChaSavePosY);
			::SQLGetData(hStmt,35, SQL_C_DOUBLE, &fChaSavePosZ,   0, &cbChaSavePosZ);

			::SQLGetData(hStmt,36, SQL_C_LONG, &nChaSchool,			0, &cbChaSchool);
			::SQLGetData(hStmt,37, SQL_C_LONG, &nChaSex,			0, &cbChaSex);
			::SQLGetData(hStmt,38, SQL_C_LONG, &nChaHair,			0, &cbChaHair);
			::SQLGetData(hStmt,39, SQL_C_LONG, &nChaFace,			0, &cbChaFace);
			::SQLGetData(hStmt,40, SQL_C_LONG, &nChaLiving,			0, &cbChaLiving);
			::SQLGetData(hStmt,41, SQL_C_LONG, &nChaSkillPoints,    0, &cbChaSkillPoints);

            ::SQLGetData(hStmt,42, SQL_C_LONG,   &nChaInvenLine,  0, &cbChaInvenLine);
            ::SQLGetData(hStmt,43, SQL_C_LONG,   &nChaReturnMap,  0, &cbChaReturnMap);
			::SQLGetData(hStmt,44, SQL_C_DOUBLE, &fChaReturnPosX, 0, &cbChaReturnPosX);
			::SQLGetData(hStmt,45, SQL_C_DOUBLE, &fChaReturnPosY, 0, &cbChaReturnPosY);
			::SQLGetData(hStmt,46, SQL_C_DOUBLE, &fChaReturnPosZ, 0, &cbChaReturnPosZ);
			
			::SQLGetData(hStmt,47, SQL_C_LONG, &nChaCP,			0, &cbChaCP);
   
            pChaData2->m_CharDbNum = (DWORD) nChaNum;
			pChaData2->SetUserID( (DWORD) nUserNum );
            if (cbChaName != 0 && cbChaName != -1)
                pChaData2->SetName((const char*) szChaName);
            pChaData2->m_emTribe = EMTRIBE   (nChaTribe);
            pChaData2->m_emClass = EMCHARCLASS(nChaClass);
            // sTemp.SGNum         = nSGNum; // 서버 그룹
            pChaData2->m_wLevel = (WORD) nChaLevel;

            // wPow;	//	힘.
	        // wStr;	//	체력.
	        // wSpi;	//	정신.
	        // wDex;	//	민첩.
	        // wInt;	//	지력.
	        // wSta;	//	근력.		--(추가)
            pChaData2->m_sStats.wPow = (WORD) nChaPower;
            pChaData2->m_sStats.wStr = (WORD) nChaStrong;
            pChaData2->m_sStats.wSta = (WORD) nChaStrength;
            pChaData2->m_sStats.wSpi = (WORD) nChaSpirit;
            pChaData2->m_sStats.wDex = (WORD) nChaDex;
            pChaData2->m_sStats.wInt = (WORD) nChaIntel;

            pChaData2->m_wStatsPoint = (WORD) nChaStRemain;

			pChaData2->SetInvenMoney( llChaMoney );
            pChaData2->m_sExperience.lnNow  = llChaExp;

			pChaData2->m_sHP.nNow			= nChaHP;
			pChaData2->m_sMP.nNow			= nChaMP;
			pChaData2->m_sSP.nNow	 		= nChaSP;
			pChaData2->m_sStartMapID.dwID	= (DWORD) nChaStartMap;
			pChaData2->m_dwStartGate		= (DWORD) nChaStartGate;
			pChaData2->m_vStartPos.x		= (float) fChaPosX;

			pChaData2->m_vStartPos.y		= (float) fChaPosY;
			pChaData2->m_vStartPos.z		= (float) fChaPosZ,	
			//nGuNum;
			pChaData2->m_nBright			= nChaBright;

			pChaData2->m_powerAttack		= nChaAttackP;
			pChaData2->m_powerDefence	= nChaDefenseP;

			pChaData2->m_powerAttribute[SKILL::EMAPPLY_MELEE]				= nChaFightA;
			pChaData2->m_powerAttribute[SKILL::EMAPPLY_RANGE]				= nChaShootA;
//			pChaData2->m_powerAttribute[SKILL::EMAPPLY_MAGIC]				= nChaMagicA;		

			pChaData2->m_wPK				= (WORD) nChaPK;

			pChaData2->m_sSaveMapID.dwID = (DWORD) nChaSaveMap;
			pChaData2->m_vSavePos.x		 = (float) fChaSavePosX; 
			pChaData2->m_vSavePos.y		 = (float) fChaSavePosY; 
			pChaData2->m_vSavePos.z		 = (float) fChaSavePosZ; 

			pChaData2->m_wSchool		= (WORD) nChaSchool;
			pChaData2->m_wSex			= (WORD) nChaSex;
			pChaData2->m_wHair			= (WORD) nChaHair;
			pChaData2->m_wFace			= (WORD) nChaFace;
			pChaData2->m_nLiving		= nChaLiving;

			pChaData2->m_dwSkillPoint   = static_cast<DWORD> (nChaSkillPoints);

            pChaData2->m_sLastCallMapID.dwID = (DWORD) nChaReturnMap;
			pChaData2->m_vLastCallPos.x		= (float) fChaReturnPosX; 
			pChaData2->m_vLastCallPos.y		= (float) fChaReturnPosY; 
			pChaData2->m_vLastCallPos.z		= (float) fChaReturnPosZ;

            // 인벤토리 라인
            pChaData2->m_wINVENLINE = (WORD) nChaInvenLine;

			pChaData2->m_sCP.wNow			= (WORD) nChaCP;
           
		}
		else
		{
			break;		
		}
    }
    FreeStmt(hStmt);

	// 스킬		
	se::ByteStream ByteStream;
    int nRetCode;    		
	nRetCode = ReadImage("ChaInfo.ChaSkills", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	pChaData2->SETEXPSKILLS_BYBUF(ByteStream);

	// Skill Quick Slot
	nRetCode = ReadImage("ChaInfo.ChaSkillSlot", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	pChaData2->SETSKILL_QUICKSLOT(ByteStream);
    
	// 착용아이템
	nRetCode = ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	SETPUTONITEMS_BYBUF(pChaData2->m_PutOnItems,ByteStream);

	// Quest
	sReturn = ReadImage("ChaInfo.ChaQuest", nChaNum, ByteStream);
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
	pChaData2->SETQUESTPLAY(ByteStream, false);

	// 캐릭터 인벤토리
	nRetCode = ReadImage("ChaInfo.ChaInven", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	pChaData2->SETINVENTORY_BYBUF(ByteStream);
	
	nRetCode = ReadImage("ChaInfo.ChaCoolTime", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETITEMCOOLTIME_BYBUF(ByteStream);

	// 걸려있는 SkillFact
	nRetCode = ReadImage("ChaInfo.ChaSkillFact", nChaNum, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETSKILLFACT_BYBUF(ByteStream);

    return DB_OK;
}

int CGMOdbcManager::RestoreCharacterInfoBackup( int nChaNum )
{
    sc::writeLogError("CGMOdbcManager::RestoreCharacterInfoBackup. Use ADO");
    return DB_ERROR;
}

int	CGMOdbcManager::GetUserInven(GLCHARLOGIC* pChaData2)
{
	return ReadUserInven(pChaData2);
}

// 사용자 인벤토리 정보를 가져온다.
int	CGMOdbcManager::ReadUserInven(GLCHARLOGIC* pChaData2)
{
	// 유저인벤토리, money
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn; 

	int nUserNum = 0;	
	
    std::strstream strTemp;
	strTemp << "SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 FROM UserInven WHERE UserNum=" << pChaData2->GetUserID();
	strTemp << '\0';
	
	sReturn = ::SQLExecDirect(hStmt,
							  (SQLCHAR*) strTemp.str(), 
							  SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        FreeStmt(hStmt);
        return DB_ERROR;
    }

	LONGLONG   llUserMoney = 0;
    SQLINTEGER cbUserMoney = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate2; SQLINTEGER cbStorageDate2 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate3; SQLINTEGER cbStorageDate3 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate4; SQLINTEGER cbStorageDate4 = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_SBIGINT,          &llUserMoney, 0, &cbUserMoney);
            ::SQLGetData(hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sStorageDate2, 0, &cbStorageDate2);
            ::SQLGetData(hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sStorageDate3, 0, &cbStorageDate3);
            ::SQLGetData(hStmt, 4, SQL_C_TYPE_TIMESTAMP, &sStorageDate4, 0, &cbStorageDate4);

			pChaData2->SetStorageMoney( llUserMoney );
            // 인벤토리 사용만료기간
            CTime cTemp2(sStorageDate2.year, sStorageDate2.month,  sStorageDate2.day, 
                            sStorageDate2.hour, sStorageDate2.minute, sStorageDate2.second);
            pChaData2->m_tSTORAGE[0] = cTemp2.GetTime();

            CTime cTemp3(sStorageDate3.year, sStorageDate3.month,  sStorageDate3.day, 
                            sStorageDate3.hour, sStorageDate3.minute, sStorageDate3.second);
            pChaData2->m_tSTORAGE[1] = cTemp3.GetTime();

            CTime cTemp4(sStorageDate4.year, sStorageDate4.month,  sStorageDate4.day, 
                            sStorageDate4.hour, sStorageDate4.minute, sStorageDate4.second);
            pChaData2->m_tSTORAGE[2] = cTemp4.GetTime();
		}
		else
		{
			break;		
		}
        Sleep( 1 );
    }
    FreeStmt(hStmt);    

	// image
	se::ByteStream ByteStream;
	int nRetCode = ReadUserInven(pChaData2->GetUserID(), ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;

	pChaData2->SETSTORAGE_BYBUF(ByteStream);
	return DB_OK;
}

// 사용자 인벤토리에서 데이타를 읽어온다.
int	CGMOdbcManager::ReadUserInven(DWORD dwUserNum, se::ByteStream &ByteStream)
{	
	ByteStream.ClearBuffer ();
	
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	char		pBinary[DB_IMAGE_BUF_SIZE];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;
		
	std::strstream strTemp;
	strTemp << "SELECT UserInven.UserInven FROM UserInven where (UserNum=" <<  dwUserNum << ")";	
	strTemp << '\0';

	// Create a result
	sReturn = ::SQLExecDirect(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	lTotalSize = 0;
	// Retrieve and display each row of data.
	while ((sReturn = ::SQLFetch(hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::memset(pBinary, 0, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(hStmt, 1, SQL_C_BINARY, pBinary, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBinary, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;			
			
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}
	
	// Close the cursor.
	FreeStmt(hStmt);
	// ::CLogFile(hStmt);
	return DB_OK;
}
int	CGMOdbcManager::SaveCharacterBase(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum <= 0 || pCharData2->m_wLevel == 0)
		return DB_ERROR;
	
	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_CharDbNum;
	

	// 민첩 ChaDex, wDex
	// 지력 ChaIntel, wInt
	// 체력 ChaStrong, wStr
	// 힘   ChaPower, wPow
	// 정신 ChaSpirit, wSpi
	// 근력 ChaStrength, wSta

	// 캐릭터를 저장 한다.
	std::strstream strTemp;
	strTemp << "UPDATE ChaInfo SET ";
    
	strTemp << "  ChaBright="   << pCharData2->m_nBright;		
	strTemp << ", ChaLevel="    << (int) pCharData2->m_wLevel;
	strTemp << ", ChaMoney="    << pCharData2->GetInvenMoney();
	strTemp << ", ChaDex="      << (int) pCharData2->m_sStats.wDex;		// 민첩
	strTemp << ", ChaIntel="    << (int) pCharData2->m_sStats.wInt;		// 지력
	
	strTemp << ", ChaStrong="   << (int) pCharData2->m_sStats.wStr;		// 체력
	strTemp << ", ChaPower="    << (int) pCharData2->m_sStats.wPow;		// 힘
	strTemp << ", ChaSpirit="   << (int) pCharData2->m_sStats.wSpi;		// 정신
	strTemp << ", ChaStrength=" << (int) pCharData2->m_sStats.wSta;	// 근력
	strTemp << ", ChaStRemain=" << (int) pCharData2->m_wStatsPoint;
    
	strTemp << ", ChaAttackP="  << pCharData2->m_powerAttack;	
	strTemp << ", ChaDefenseP=" << pCharData2->m_powerDefence;
	strTemp << ", ChaFightA="   << (int) pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE];
	strTemp << ", ChaShootA="   << (int) pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE];
	strTemp << ", ChaExp="      << pCharData2->m_sExperience.lnNow;

	strTemp << ", ChaSkillPoint=" << (int) pCharData2->m_dwSkillPoint;	
	strTemp << ", ChaHP="         << pCharData2->m_sHP.nNow;
	strTemp << ", ChaMP="         << pCharData2->m_sMP.nNow;
	strTemp << ", ChaSP="         << pCharData2->m_sSP.nNow;
	strTemp << ", ChaPK="         << (int) pCharData2->m_wPK;

	strTemp << ", ChaStartMap="   << (int) pCharData2->m_sStartMapID.dwID;	
	strTemp << ", ChaStartGate="  << (int) pCharData2->m_dwStartGate;
	strTemp << ", ChaPosX=" << pCharData2->m_vStartPos.x;
	strTemp << ", ChaPosY=" << pCharData2->m_vStartPos.y;
	strTemp << ", ChaPosZ=" << pCharData2->m_vStartPos.z;
    
    strTemp << ", ChaSaveMap="  << (int) pCharData2->m_sSaveMapID.dwID;
    strTemp << ", ChaSavePosX=" << pCharData2->m_vSavePos.x;
    strTemp << ", ChaSavePosY=" << pCharData2->m_vSavePos.y;
    strTemp << ", ChaSavePosZ=" << pCharData2->m_vSavePos.z;
	strTemp << ", ChaFace=" << pCharData2->m_wFace;

	strTemp << ", ChaHair=" << pCharData2->m_wHair;
	strTemp << ", ChaSex=" << pCharData2->m_wSex;
	strTemp << ", ChaSchool=" << pCharData2->m_wSchool;
	strTemp << ", ChaLiving=" << pCharData2->m_nLiving;	

    strTemp << ", ChaReturnMap="  << (int) pCharData2->m_sLastCallMapID.dwID;
    strTemp << ", ChaReturnPosX=" << pCharData2->m_vLastCallPos.x;
    strTemp << ", ChaReturnPosY=" << pCharData2->m_vLastCallPos.y;
    strTemp << ", ChaReturnPosZ=" << pCharData2->m_vLastCallPos.z;

    strTemp << ", ChaInvenLine="  << (int) pCharData2->m_wINVENLINE; 
	strTemp << ", ChaCP="         << (int) pCharData2->m_sCP.dwData;
    ////////////////////////////////////////////////////////////

	strTemp << "  WHERE ChaNum=" << dwChaNum;
	strTemp << '\0';
	
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	else
	{
		FreeStmt(hStmt);
		return DB_OK;
	}
}

int CGMOdbcManager::SaveCharacterInven( LPVOID _pbuffer, bool bClone /*= false*/ )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL) return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	// Inventory
	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;

	ByteStream.ClearBuffer();
	pCharData2->GETINVENTORYE_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);		
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaInven", dwChaNum, (char *) pBuffer, dwSize);
	else
		return DB_ERROR;
}

int CGMOdbcManager::SaveCharacterSkill(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Skill 	
	pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaSkills", dwChaNum, (char *) pBuffer, dwSize );
	else
		return DB_ERROR;
}


int CGMOdbcManager::SaveCharacterQuest(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream( 40960 );
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Quest
	pCharData2->GETQUESTPLAY(ByteStream);	
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaQuest", dwChaNum, (char *) pBuffer, dwSize );
	else
		return DB_ERROR;
}

int CGMOdbcManager::SaveCharacterSkillQuickSlot(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Quest
	pCharData2->GETSKILL_QUICKSLOT(ByteStream);	
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaSkillSlot", dwChaNum, (char *) pBuffer, dwSize );
	else
		return DB_ERROR;
}

int CGMOdbcManager::SaveCharacterActionQuickSlot(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Quest
	pCharData2->GETACTION_QUICKSLOT(ByteStream);	
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaActionSlot", dwChaNum, (char *) pBuffer, dwSize );
	else
		return DB_ERROR;
}

int CGMOdbcManager::SaveCharacterUserInven(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	
    SaveStorageDate(pCharData2->GetUserID(), 1, pCharData2->m_tSTORAGE[0]);
    SaveStorageDate(pCharData2->GetUserID(), 2, pCharData2->m_tSTORAGE[1]);
    SaveStorageDate(pCharData2->GetUserID(), 3, pCharData2->m_tSTORAGE[2]);

    pCharData2->GETSTORAGE_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);		
	if (pBuffer != NULL)
	{
		return WriteUserInven(pCharData2->GetStorageMoney(), 
						      pCharData2->GetUserID() , 
						      (char *) pBuffer, 
						      dwSize);
	}
	else
	{
		return DB_ERROR;
	}
}

int CGMOdbcManager::SaveCharacterSkillFact(LPVOID _pbuffer)
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;

	pCharData2->GETSKILLFACT_BYBUF(ByteStream);	
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaSkillFact", dwChaNum, (char *) pBuffer, dwSize );
	else
		return DB_ERROR;

	return DB_OK;
}

int	CGMOdbcManager::SaveCharacterSlots ( LPVOID _pbuffer )
{
	return DB_ERROR;
}

int CGMOdbcManager::SaveStorageDate(int nUserNum, int nStorageNum, __time64_t tTime)
{
    if (nStorageNum < 1 || nStorageNum > 3)
    {
        return DB_ERROR;
    }

    CTime cTemp(tTime);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    std::strstream strTemp;
    strTemp << "Update UserInven Set ";
    switch (nStorageNum)
    {
    case 1:
        strTemp << "ChaStorage2='" << strTime.GetString() << "'";
        break;
    case 2:
        strTemp << "ChaStorage3='" << strTime.GetString() << "'";
        break;
    case 3:
        strTemp << "ChaStorage4='" << strTime.GetString() << "'";
        break;
    default:
        return DB_ERROR;
        break;
    }
    strTemp << " WHERE UserNum=" << nUserNum;
    strTemp << '\0';

	int nReturn = ExecuteSQL(m_hOdbcGame, CString(strTemp.str()));
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

int CGMOdbcManager::SaveCharacterPutOnItem( LPVOID _pbuffer, bool bClone /*= false*/ )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );
	if (pCharData2 == NULL)	return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	//if (dwChaNum < 0) return DB_ERROR;

	// Put on item
	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);
	if (pBuffer != NULL)
		return WriteImage("ChaInfo.ChaPutOnItems", dwChaNum, (char *) pBuffer, dwSize);
	else
		return DB_ERROR;
}	

// 캐릭터를 저장한다
int	CGMOdbcManager::SaveCharacter ( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	
	if (dwChaNum == 0 || pCharData2->m_wLevel == 0)
		return DB_ERROR;
	
	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_CharDbNum;
	

	// 민첩 ChaDex, wDex
	// 지력 ChaIntel, wInt
	// 체력 ChaStrong, wStr
	// 힘   ChaPower, wPow
	// 정신 ChaSpirit, wSpi
	// 근력 ChaStrength, wSta

	// 캐릭터를 저장 한다.
	std::strstream strTemp;
	strTemp << "UPDATE ChaInfo SET ";
	strTemp << "  ChaBright=" << pCharData2->m_nBright;		
	strTemp << ", ChaLevel=" << pCharData2->m_wLevel;
	strTemp << ", ChaMoney=" << pCharData2->GetInvenMoney();
	strTemp << ", ChaDex=" << pCharData2->m_sStats.wDex;		// 민첩
	strTemp << ", ChaIntel=" << pCharData2->m_sStats.wInt;		// 지력
	
	strTemp << ", ChaStrong=" << pCharData2->m_sStats.wStr;		// 체력
	strTemp << ", ChaPower=" << pCharData2->m_sStats.wPow;		// 힘
	strTemp << ", ChaSpirit=" << pCharData2->m_sStats.wSpi;		// 정신
	strTemp << ", ChaStrength=" << pCharData2->m_sStats.wSta;	// 근력
	strTemp << ", ChaStRemain=" << pCharData2->m_wStatsPoint;

	strTemp << ", ChaAttackP=" << pCharData2->m_powerAttack;	
	strTemp << ", ChaDefenseP=" << pCharData2->m_powerDefence;
	strTemp << ", ChaFightA=" << pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE];
	strTemp << ", ChaShootA=" << pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE];
	strTemp << ", ChaExp=" << pCharData2->m_sExperience.lnNow;

	strTemp << ", ChaSkillPoint=" << pCharData2->m_dwSkillPoint;	
	strTemp << ", ChaHP=" << pCharData2->m_sHP.nNow;
	strTemp << ", ChaMP=" << pCharData2->m_sMP.nNow;
	strTemp << ", ChaSP=" << pCharData2->m_sSP.nNow;
	strTemp << ", ChaPK=" << pCharData2->m_wPK;

	strTemp << ", ChaStartMap=" << (int) pCharData2->m_sStartMapID.dwID;	
	strTemp << ", ChaStartGate=" << (int) pCharData2->m_dwStartGate;
	strTemp << ", ChaPosX=" << pCharData2->m_vStartPos.x;
	strTemp << ", ChaPosY=" << pCharData2->m_vStartPos.y;
	strTemp << ", ChaPosZ=" << pCharData2->m_vStartPos.z;

    // Add : 2003-09-24 
    strTemp << ", ChaSaveMap="  << (int) pCharData2->m_sSaveMapID.dwID;
    strTemp << ", ChaSavePosX=" << pCharData2->m_vSavePos.x;
    strTemp << ", ChaSavePosY=" << pCharData2->m_vSavePos.y;
    strTemp << ", ChaSavePosZ=" << pCharData2->m_vSavePos.z;

	strTemp << ", ChaSchool=" << pCharData2->m_wSchool;
	strTemp << ", ChaFace=" << pCharData2->m_wFace;
	strTemp << ", ChaHair=" << pCharData2->m_wHair;
	strTemp << ", ChaLiving=" << pCharData2->m_nLiving;
	strTemp << ", ChaCP=" << pCharData2->m_sCP.dwData;
    ////////////////////////////////////////////////////////////

	strTemp << "  WHERE ChaNum=" << dwChaNum;
	strTemp << '\0';
	
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

    SQLRETURN	sReturn;
    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}
	else
	{
		FreeStmt(hStmt);
	}
	
	se::ByteStream ByteStream( 40960 );
	LPBYTE pBuffer = NULL;
	DWORD dwSize;

	// Skill 
	pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkills", dwChaNum, (char *) pBuffer, dwSize );
	pBuffer = NULL;

	// Character Quest information
	pCharData2->GETQUESTPLAY(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaQuest", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Skill Quick Slot	
	pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
	ByteStream.GetBuffer(pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkillSlot", dwChaNum, (char *) pBuffer, dwSize );
	pBuffer = NULL;

	// Put on item
	pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaPutOnItems", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// SkillFact
	pCharData2->GETSKILLFACT_BYBUF(ByteStream);
	ByteStream.GetBuffer(pBuffer, dwSize);		
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkillFact", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Inventory
	ByteStream.ClearBuffer();
	pCharData2->GETINVENTORYE_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);		
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaInven", dwChaNum, (char *) pBuffer, dwSize);

    // 사용자 인벤토리 저장
	if ( pCharData2->m_bServerStorage )
	{
		pCharData2->GETSTORAGE_BYBUF(ByteStream);
		ByteStream.GetBuffer( pBuffer, dwSize);		
		if (pBuffer != NULL)
		{
			WriteUserInven(	pCharData2->GetStorageMoney(), 
							pCharData2->GetUserID() , 
							(char *) pBuffer, 
							dwSize);
		}
	}
	
	return DB_OK;	
}

int CGMOdbcManager::CopyCharacterProductInfo( DWORD dwOriginalChaNum, DWORD dwNewChaNum )
{
    return DB_ERROR;
}

int CGMOdbcManager::ItemSearch( int mid, int sid, std::vector< ITEM_SEARCH >& result )
{
	return DB_ERROR;
}

int CGMOdbcManager::ItemSearch( CString& guid, std::vector< ITEM_SEARCH >& result )
{
	return DB_ERROR;
}

int CGMOdbcManager::ItemStatistic( std::vector< ITEM_STATISTIC >& result )
{
	return DB_ERROR;
}

int CGMOdbcManager::ItemSelect( int Owner, int InvenType, std::vector< SINVENITEM_SAVE >& vecItems )
{
	return DB_ERROR;
}

// 유저인벤토리가 있는지 체크한다.
// 없다면 새롭게 생성한다.
BOOL CGMOdbcManager::CheckInven(DWORD dwUserNum)
{
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return FALSE;

	RETCODE nRetCode = 0;	
	SQLRETURN  sReturn;
	SQLINTEGER nInvenNum;
	SQLINTEGER cbInvenNum = SQL_NTS;

	std::strstream strTemp;
	strTemp << "SELECT count(*) FROM UserInven where (UserNum=" <<  dwUserNum << ") ";
	strTemp << '\0';

    sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return FALSE;
	}
	
	while (1)
	{
		sReturn = ::SQLFetch(hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            FreeStmt(hStmt);
            return FALSE;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{				
			::SQLGetData(hStmt, 1, SQL_C_LONG, &nInvenNum, 0, &cbInvenNum);
		}
		else
		{
			break;
		}
	}	

	FreeStmt(hStmt);

	if (nInvenNum > 0)
	{
		return TRUE;
	}
	else // make new inven
	{
		return FALSE;
	}
}

int	CGMOdbcManager::MakeUserInven(DWORD dwUserNum)
{
	CString strTemp;
	strTemp.Format("INSERT INTO UserInven (UserNum, UserMoney, UserInven) VALUES(%d,%d,%s)",
		            dwUserNum,
		            0,
		            "''");

	return Update(m_hOdbcGame, strTemp);
}


//
//mjeon.ado
// this method is implemented only for ADO.
//
int CGMOdbcManager::WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, GLCHARLOGIC *pCharData2 )
{
	return 0;
}

// 사용자 인벤토리에 데이타를 저장한다.
int	CGMOdbcManager::WriteUserInven(LONGLONG llMoney, // LONGLONG __int64, 저장할 돈의 양
							   DWORD dwUserNum, // 사용자번호
							   char* pData, // 데이타포인터
							   int nSize) // 사이즈

{
	// User Inventory
	// 1. Check User Inven		
	// 2. If exist skip
	// 3. not exist, write iventory image
	BOOL bInven = CheckInven(dwUserNum);
	if (!bInven)
	{
		MakeUserInven(dwUserNum);
	}

	CString strMoney;
	strMoney.Format("UPDATE UserInven SET UserMoney= %.I64d WHERE ( UserNum= %d )", llMoney, dwUserNum);	
	Update(m_hOdbcGame, strMoney);    

	// Update Image
	RETCODE nRetCode = 0;
	int nTemp = 0;	

	std::strstream strTemp;
	strTemp << "UPDATE UserInven SET UserInven = ? WHERE (UserNum=" <<  dwUserNum << ") ";
	strTemp << '\0';
	
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

	SQLRETURN	sReturn; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}	
	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(hStmt, 
								1, 
								SQL_PARAM_INPUT,
								SQL_C_BINARY, 
								SQL_LONGVARBINARY,
								nSize, 
								0, 
								(SQLPOINTER) 2, 
								0, 
								&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(hStmt);
	
	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(hStmt, pData, (SQLINTEGER) nSize);
		}
	}
    // Close the cursor.
	FreeStmt(hStmt);
	// ::CLogFile(hStmt);
	return DB_OK;
}

int	CGMOdbcManager::ReadImage	(const char* objName, int nChaNum, se::ByteStream &ByteStream)
{	
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	SQLCHAR		pBinary[DB_IMAGE_BUF_SIZE];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;
	
	hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

	std::strstream strTemp;
	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	// Create a result
	sReturn = ::SQLExecDirect(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}
	
	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ((sReturn = ::SQLFetch(hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::memset(pBinary, 0, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(hStmt, 1, SQL_C_BINARY, pBinary, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBinary, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	// Close the cursor.
	FreeStmt(hStmt);
	// ::CLogFile(hStmt);
	return DB_OK;
}

int	CGMOdbcManager::WriteImage(const char* objName, int nChaNum, char* pData, int nSize)
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT	hStmt; // Handle of statement (void*)
	// SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	if (objName == NULL || nChaNum <= 0 || pData == NULL || nSize <= 0)
	{
		return DB_ERROR;
	}

	hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) return DB_ERROR;

	std::strstream strTemp;
	strTemp << "UPDATE ChaInfo SET " << objName << "=? WHERE (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	// Prepare parameter
	sReturn = ::SQLPrepare(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(hStmt, 
								1, 
								SQL_PARAM_INPUT,
								SQL_C_BINARY, 
								SQL_LONGVARBINARY,
								nSize, 
								0, 
								(SQLPOINTER) 2, 
								0, 
								&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeStmt(hStmt);
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(hStmt);
	
	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(hStmt, pData, (SQLINTEGER) nSize);
		}
	}
    // Close the cursor.
	FreeStmt(hStmt);
	// ::CLogFile(hStmt);
	return DB_OK;
}

int CGMOdbcManager::WriteGMLog(int nGmNum, std::strstream& strCmd)
{
	std::strstream strTemp;
	strTemp << "INSERT INTO LogGmCmd (UserNum, GmCmd) VALUES(" << nGmNum << ",'";	
	strTemp << strCmd.str() << "')";
	strTemp << '\0';

	int nReturn = ExecuteSQL(m_hOdbcUser, CString(strTemp.str()));
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

/*
UserDB의 LogGmCmd Table에 GM이 현재 사용한 IP추가를 위해서 함수 추가
기존 단순 Insert에서 Adhoc을 줄이기 위해서 프로시저로 변경
*/
int CGMOdbcManager::WriteGMLog(int nGmNum, std::strstream& strCmd, CString strUserIP)
{
    SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
    if (!hStmt) return DB_ERROR;

	SQLRETURN sReturn=0;

	std::strstream strTemp;
	strTemp << "{call sp_gmEditLog(";
	strTemp << nGmNum << ", ";
	strTemp << "'" << strCmd.str() << "', ";
	strTemp << "'" << strUserIP.GetString() << "', ?)}";
	strTemp << '\0';

	SQLINTEGER nReturn=0;
	SQLINTEGER nOutput=0, cbOutput=SQL_NTS;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare(hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	// Bind the parameter.
	sReturn = ::SQLBindParameter(hStmt, 
								1, 
								SQL_PARAM_OUTPUT, 
								SQL_C_SLONG, 
								SQL_INTEGER, 
								0, 
								0,
								&nOutput, 
								0, 
								&nLenBuffer);
	
	if (sReturn == SQL_ERROR || sReturn == SQL_INVALID_HANDLE)
	{		
		FreeStmt(hStmt); 
		return DB_ERROR;
	}

	sReturn = ::SQLExecute(hStmt);	
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{			
		nReturn = DB_ERROR;
	}
	else
	{
		nReturn = nOutput;
	}
	FreeStmt(hStmt); 
	return nReturn;
}


int CGMOdbcManager::GetClubInfo(const char* szSearch, 
								std::vector<viewClubInfo> &v)
{
	return DB_OK;
/*
	SQLHSTMT hStmt = GetStmt(m_hOdbcUser);
    if (!hStmt) return DB_ERROR;
	
	std::strstream strTemp;
	strTemp << "SELECT GuNum, ChaNum, ChaName, GuName, GuNotice, GuRank, GuMoney, GuIncomeMoney, GuMarkVer ";
	strTemp << "From viewGuildInfo WHERE GuName like '%" << szSearch << "%";
	strTemp << '\0';
	
	SQLRETURN	sReturn;

	sReturn = ::SQLExecDirect(hStmt,
							(SQLCHAR*) strTemp.str(),
							SQL_NTS);	

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		FreeStmt(hStmt);
        return DB_ERROR;
	}

	LONGLONG   llUserMoney = 0;
    SQLINTEGER cbUserMoney = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate2; SQLINTEGER cbStorageDate2 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate3; SQLINTEGER cbStorageDate3 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate4; SQLINTEGER cbStorageDate4 = SQL_NTS;

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
            ::SQLGetData(hStmt, 1, SQL_C_SBIGINT,        &llUserMoney, 0, &cbUserMoney);
            ::SQLGetData(hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sStorageDate2, 0, &cbStorageDate2);
            ::SQLGetData(hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sStorageDate3, 0, &cbStorageDate3);
            ::SQLGetData(hStmt, 4, SQL_C_TYPE_TIMESTAMP, &sStorageDate4, 0, &cbStorageDate4);

			pChaData2->m_lnStorageMoney = llUserMoney;
            // 인벤토리 사용만료기간
            CTime cTemp2(sStorageDate2.year, sStorageDate2.month,  sStorageDate2.day, 
                            sStorageDate2.hour, sStorageDate2.minute, sStorageDate2.second);
            pChaData2->m_tSTORAGE[0] = cTemp2.GetTime();

            CTime cTemp3(sStorageDate3.year, sStorageDate3.month,  sStorageDate3.day, 
                            sStorageDate3.hour, sStorageDate3.minute, sStorageDate3.second);
            pChaData2->m_tSTORAGE[1] = cTemp3.GetTime();

            CTime cTemp4(sStorageDate4.year, sStorageDate4.month,  sStorageDate4.day, 
                            sStorageDate4.hour, sStorageDate4.minute, sStorageDate4.second);
            pChaData2->m_tSTORAGE[2] = cTemp4.GetTime();
		}
		else
		{
			break;		
		}
        Sleep( 1 );
    }
    FreeStmt(hStmt);

	strTemp.freeze( false );

	return DB_OK;
*/
}




//=================================== Vehicle ===================================

/*

int CGMOdbcManager::GetVehicle(
    GLVEHICLE* pVehicle,
    int nVehicleNum,
    int nCharNum)
{
	if (!pVehicle)
	{
		Print("ERROR:CGMOdbcManager::GetVehicle pVehicle==NULL");
		return DB_ERROR;
	}

	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn)
        return DB_ERROR;

	// 탈것의 기본정보를 가져온다.
    std::string Query(
        sc::string::format(
	        "Exec sp_SelectVehicle %d,%d",
            nVehicleNum, nCharNum));
	
	int nRowCount = 0;
	SQLRETURN sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query.c_str());
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		return DB_ERROR;
	}

	SQLINTEGER nOwnerNum = 0, cbOwnerNum = SQL_NTS;	
	SQLINTEGER nVehicleCardMID = 0, cbVehicleCardMID = SQL_NTS;
	SQLINTEGER nVehicleCardSID = 0, cbVehicleCardSID = SQL_NTS;
	SQLINTEGER nType = 0, cbType = SQL_NTS;
	SQLINTEGER nFull = 0, cbFull = SQL_NTS;
	SQLINTEGER nBooster = 0, cbBooster = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query.c_str());
			Print(GetErrorString(pConn->hStmt));
			m_pGameDB->FreeConnection(pConn);

			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nOwnerNum, 0, &cbOwnerNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nVehicleCardMID, 0, &cbVehicleCardMID);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nVehicleCardSID, 0, &cbVehicleCardSID);
			::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nType, 0, &cbType);
			::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nFull, 0, &cbFull);
			::SQLGetData(pConn->hStmt, 6, SQL_C_LONG, &nBooster, 0, &cbBooster);

			pVehicle->m_dwOwner				= (DWORD) nOwnerNum;
			pVehicle->m_sVehicleID.wMainID	= (WORD) nVehicleCardMID;
			pVehicle->m_sVehicleID.wSubID	= (WORD) nVehicleCardSID;
			pVehicle->m_emTYPE         = VEHICLE_TYPE(nType);			
			pVehicle->m_nFull		   = nFull;
			pVehicle->m_dwBoosterID	   = nBooster;
			nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	m_pGameDB->FreeConnection(pConn);	

		// Row Count 가 1 이 아니면 에러이다.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		sc::writeLogError(sc::string::format("DB:ERROR:GetVehicle RowCount = 0, VehicleID = %d, CharID = %d",
			nVehicleNum, nCharNum  ));
		
		return sc::db::DB_ROWCNT_ERROR;
	}

	// 탈것 의 인벤토리를 가져온다.
	if ( GetVehicleInven( pVehicle, nVehicleNum, nCharNum ) == DB_ERROR) return DB_ERROR;
	if ( GetVehicleColor( pVehicle, nVehicleNum, nCharNum ) == DB_ERROR) return DB_ERROR;	

	return DB_OK;
}


int CGMOdbcManager::GetVehicleInven( 
							    GLVEHICLE* pVehicle,
								int nVehicleNum,
								int nCharNum )
{
	if (pVehicle == NULL)
	{
		Print(_T("ERROR:CGMOdbcManager::GetVehicleInven pVehicle==NULL"));
		return DB_ERROR;
	}

	se::ByteStream ByteStream;
	int nReturn = 0;

	//  VehicleOnItem
	nReturn = m_pGameDB->ReadImageVehicle("VehicleInfo.VehiclePutOnItems", nVehicleNum, nCharNum,  ByteStream);

	if (nReturn == DB_ERROR)
	{
		return DB_ERROR;
	}
	else 
	{
		SETPUTONITEMS_BYBUF( pVehicle->m_PutOnItems, ByteStream );
		return DB_OK;
	}
}


int CGMOdbcManager::GetVehicleColor( 
								  GLVEHICLE* pVehicle,
								  int nVehicleNum,
								  int nCharNum )
{
	if (pVehicle == NULL)
	{
		Print(_T("ERROR:CGMOdbcManager::GetVehicleColor pVehicle==NULL"));
		return DB_ERROR;
	}

	se::ByteStream ByteStream;
	int nReturn = 0;

	//  VehicleOnItem
	nReturn = m_pGameDB->ReadImageVehicle("VehicleInfo.VehiclePartsColor", nVehicleNum, nCharNum,  ByteStream);

	if (nReturn == DB_ERROR)
	{
		return DB_ERROR;
	}
	else 
	{
		SETITEMSCOLOR_BYBUF( pVehicle->m_sColor, ByteStream );
		return DB_OK;
	}
}
*/

//================================================================================







int CGMOdbcManager::GetInt(std::string& strSearch)
{
	SQLRETURN	sReturn; // SQL return value (short)
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);
    if (!hStmt) 
	{
		return DB_ERROR;
	}
	else
	{
		sReturn = ::SQLExecDirect(hStmt,
								(SQLCHAR*) strSearch.c_str(),
								  SQL_NTS);	

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			SQLINTEGER nIntNum;	
			SQLINTEGER cbIntNum   = SQL_NTS;

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
					::SQLGetData(hStmt, 1, SQL_C_LONG, &nIntNum,  0, &cbIntNum);
					FreeStmt(hStmt);
					return nIntNum;
				}
				else
				{
					FreeStmt(hStmt);
					return DB_ERROR;			
				}
			}
		}
		else
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}
	}
}

LONGLONG CGMOdbcManager::GetBigInt(std::string& strSearch)
{
	SQLRETURN	sReturn; // SQL return value (short)	
	SQLHSTMT hStmt = GetStmt(m_hOdbcGame);	
    if (!hStmt) 
	{
		return DB_ERROR;
	}
	else
	{
		sReturn = ::SQLExecDirect(hStmt,
								(SQLCHAR*) strSearch.c_str(),
								  SQL_NTS);	

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			LONGLONG llNum;
			SQLINTEGER cbllNum   = SQL_NTS;

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
					::SQLGetData(hStmt, 1, SQL_C_SBIGINT, &llNum, 0, &cbllNum);
					FreeStmt(hStmt);
					return llNum;
				}
				else
				{
					FreeStmt(hStmt);
					return DB_ERROR;			
				}
			}
		}
		else
		{
			FreeStmt(hStmt);
			return DB_ERROR;
		}
	}
}

int CGMOdbcManager::GetActiveChaNum()
{
	std::string strTemp;
	strTemp = "SELECT Count(*) FROM ChaInfo WHERE ChaDeleted=0";

    return GetInt(strTemp);
}

int CGMOdbcManager::GetDeletedChaNum()
{
	std::string strTemp;
	strTemp = "SELECT Count(*) FROM ChaInfo WHERE ChaDeleted=1";

	return GetInt(strTemp);
}

int CGMOdbcManager::GetUserInvenNum()
{
	std::string strTemp;
	strTemp = "SELECT Count(*) FROM UserInven";

	return GetInt(strTemp);
}

int CGMOdbcManager::GetClubNum()
{
	std::string strTemp;
	strTemp = "SELECT Count(*) FROM dbo.GuildInfo";

	return GetInt(strTemp);
}

LONGLONG CGMOdbcManager::GetSumChaMoney()
{
	std::string strTemp;
	strTemp = "SELECT Sum(ChaMoney) FROM ChaInfo WHERE ChaDeleted=0";

	return GetBigInt(strTemp);
}

LONGLONG CGMOdbcManager::GetSumUserInvenMoney()
{
	std::string strTemp;
	strTemp = "SELECT Sum(UserMoney) FROM UserInven";

	return GetBigInt(strTemp);
}

LONGLONG CGMOdbcManager::GetSumClubInvenMoney()
{
	std::string strTemp;
	strTemp = "SELECT Sum(GuMoney) FROM dbo.GuildInfo";

	return GetBigInt(strTemp);
}	

void CGMOdbcManager::SetLocalIP()
{
    CString ip = "";
    char szHostName[256];
    PHOSTENT pHostInfo; 

    if( gethostname(szHostName,sizeof(szHostName)) ==0)
    {
        if((pHostInfo = gethostbyname(szHostName)) != NULL)
        {
            ip = inet_ntoa(*(struct in_addr *)*pHostInfo->h_addr_list);
        }
    }

    m_strUserIP = ip;
}

CString CGMOdbcManager::GetLocalIP()
{
	return m_strUserIP;
}

int CGMOdbcManager::GetCharacterSkill( int nChaNum, GLCHARLOGIC* pChaData2 )
{
    sc::ErrorMessageBox(std::string("ODBC not support this function."), false);
    return  DB_ERROR;
}

int CGMOdbcManager::WriteLogRandomItem( const LOG_RANDOM_OPTION &_sLOG )
{
	return 0;
}
