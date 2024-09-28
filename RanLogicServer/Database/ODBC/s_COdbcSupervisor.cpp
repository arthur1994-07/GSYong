#include "../../pch.h"
#include <boost/bind.hpp>
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Util/Lock.h"

#include "./OdbcStmt.h"
#include "./s_COdbcSupervisor.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

COdbcSupervisor::COdbcSupervisor(bool UseCustomPool)
	: m_bUseCustomPool(UseCustomPool)
  //, m_nPoolSize(sc::db::DB_POOL_SIZE)
  //, m_nDBTimeOut(sc::db::DB_RESPONSE_TIME)
{	
	Init();
}

COdbcSupervisor::COdbcSupervisor(
	const TCHAR* szOdbcName,
	const TCHAR* szUsrID,
	const TCHAR* szUsrPass,
	const TCHAR* szDBName,
    bool UseCustomPool
  /*int nPoolSize,*/
  /*int nDBTimeOut*/) 
	: m_bUseCustomPool(UseCustomPool)
  //, m_nPoolSize(sc::db::DB_POOL_SIZE)
  //, m_nDBTimeOut(sc::db::DB_RESPONSE_TIME)
{
    Init();
	SetDB(szOdbcName, szUsrID, szUsrPass, szDBName);
}

void COdbcSupervisor::Init()
{
    InitializeCriticalSectionAndSpinCount(&m_CriticalSection, sc::CRITICAL_SECTION_SPIN_COUNT);
    memset(m_szOdbcName, 0, (sc::db::DB_SVR_NAME_LENGTH+1) * sizeof(TCHAR));
    memset(m_szUsrID,    0, (USR_ID_LENGTH+1) * sizeof(TCHAR));
    memset(m_szUsrPass,  0, (USR_PASS_LENGTH+1) * sizeof(TCHAR));
    memset(m_szDBName,   0, (sc::db::DB_NAME_LENGTH+1) * sizeof(TCHAR));
}

COdbcSupervisor::~COdbcSupervisor(void)
{
    if (m_bUseCustomPool)
    {
        // todo 모든 작업이 끝나기 전까지 대기하고 있어야 한다. 2012-02-03 jgim
        //while (!IsAllDbJobCompleted())
        //    Sleep(1000);

        for (size_t i=0; i<m_CustomPool.size(); ++i)
        {
            db::ODBC_STMT* pStmt = m_CustomPool[i];
            delete pStmt;
        }
    }
	DeleteCriticalSection(&m_CriticalSection);
}

bool COdbcSupervisor::IsAllDbJobCompleted()
{
    if (!m_bUseCustomPool)
        return true;

    for (size_t i=0; i<m_CustomPool.size(); ++i)
    {
        db::ODBC_STMT* pStmt = m_CustomPool[i];
        if (pStmt->m_bUse)
            return true;
    }

    return false;
}

/*
void COdbcSupervisor::lockOn()
{
	EnterCriticalSection( &m_CriticalSection );
}
*/
/*
void COdbcSupervisor::lockOff() 
{
	LeaveCriticalSection( &m_CriticalSection );
}
*/
void COdbcSupervisor::SetDB(
	const TCHAR* szOdbcName,
	const TCHAR* szUsrID,
	const TCHAR* szUsrPass,
	const TCHAR* szDBName)
{
	SetOdbcName(szOdbcName);
	SetUserID(szUsrID);
	SetUserPass(szUsrPass);
	SetDBName(szDBName);
	//SetPoolSize( nPoolSize );
	//SetResponseTime( nDBTimeOut );
}
	
void COdbcSupervisor::SetOdbcName( const TCHAR* szName )
{
	::StringCchCopy( m_szOdbcName, sc::db::DB_SVR_NAME_LENGTH+1, szName );
}

void COdbcSupervisor::SetUserID( const TCHAR* szUsrID )
{
	::StringCchCopy( m_szUsrID, USR_ID_LENGTH+1, szUsrID );
}

void COdbcSupervisor::SetUserPass( const TCHAR* szUsrPasswd )
{
	::StringCchCopy( m_szUsrPass, USR_PASS_LENGTH+1, szUsrPasswd );
}

void COdbcSupervisor::SetDBName( const TCHAR* szDBName )
{
	::StringCchCopy( m_szDBName, sc::db::DB_NAME_LENGTH+1, szDBName );
}
/*
void COdbcSupervisor::SetPoolSize( int nSize )
{
	m_nPoolSize = nSize;
}
*/
/*
void COdbcSupervisor::SetResponseTime( int nTime )
{
	m_nDBTimeOut = nTime;
}
*/
int COdbcSupervisor::OpenDB()
{
    // 연결 1개를 유지하기 위해서 의도적으로 free 하지 않았다.
	m_BaseConnection = CreateNewConnection();
	if (!m_BaseConnection)
		return sc::db::DB_ERROR;
	else	
		return sc::db::DB_OK;
}

std::tr1::shared_ptr<db::ODBC_STMT> COdbcSupervisor::CreateNewConnection()
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);

    if (m_bUseCustomPool)
    {
        for (size_t i=0; i<m_CustomPool.size(); ++i)
        {
            db::ODBC_STMT* pStmt = m_CustomPool[i];
            if (!pStmt->m_bUse)
            {
                std::tr1::shared_ptr<db::ODBC_STMT> spStmt(pStmt, boost::bind(&COdbcSupervisor::FreeConnectionPool, this, _1));
                // Prepare statement handle
                SQLRETURN sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, spStmt->hOdbc, (SQLHANDLE *) &spStmt->hStmt);
                if (sReturn == SQL_SUCCESS_WITH_INFO)
                {
                    Print(GetErrorString(spStmt->hStmt));
                    sc::writeLogError(sc::string::format("DB:SQLAllocHandle SQL_HANDLE_STMT Failed %hd", sReturn));                    
                    std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
                    return NullOdbc;
                }
                else
                {
                    spStmt->m_bUse = true;
                    return spStmt;
                }
            }
        }
    }

	SQLRETURN	sReturn = 0; // SQL return value (short)	
	SQLHSTMT	hStmt = NULL; // Handle of statement (void*)

    std::tr1::shared_ptr<db::ODBC_STMT> spOdbc;
    if (m_bUseCustomPool)
    {
        db::ODBC_STMT* pStmt = new db::ODBC_STMT;
        m_CustomPool.push_back(pStmt);
        std::tr1::shared_ptr<db::ODBC_STMT> spStmt(pStmt, boost::bind(&COdbcSupervisor::FreeConnectionPool, this, _1));
        spOdbc = spStmt;
        spOdbc->m_bUse = true;        
    }
    else
    {
        std::tr1::shared_ptr<db::ODBC_STMT> spStmt(new db::ODBC_STMT);
        spOdbc = spStmt;
    }

	if (!spOdbc)    
    {
        std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
        return NullOdbc;
    }

    // ODBC allocate connection memory
    sReturn = ::SQLAllocHandle(
				    SQL_HANDLE_DBC,
				    COdbcEnv::GetInstance()->GetHandle(),
				    (SQLHANDLE *) &spOdbc->hOdbc );
    if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
    {
	    sc::writeLogError(std::string("DB:SQLAllocHandle SQL_HANDLE_DBC Failed"));
	    //SAFE_DELETE( spOdbc );
	    COdbcEnv::GetInstance()->CreateHandle();
        std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
        return NullOdbc;
    }
	
    // Login time out 5 sec
    sReturn = ::SQLSetConnectAttr(
				    spOdbc->hOdbc,
				    5,
				    (void*) SQL_LOGIN_TIMEOUT,
				    0 );
    if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
    {
	    sc::writeLogError(std::string("DB:SQLSetConnectAttr SQL_LOGIN_TIMEOUT Failed"));
	    //::SQLFreeHandle( SQL_HANDLE_DBC, spOdbc->hOdbc );
	    //SAFE_DELETE( spOdbc );
        std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
        return NullOdbc;
    }

    sReturn = ::SQLConnect(
				    spOdbc->hOdbc, 
				    (SQLCHAR*) m_szOdbcName,
				    SQL_NTS,
				    (SQLCHAR*) m_szUsrID,
				    SQL_NTS,
				    (SQLCHAR*) m_szUsrPass,
				    SQL_NTS );

    if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
    {
	    Print(GetErrorString( (SQLHSTMT)spOdbc->hOdbc, SQL_HANDLE_DBC));		
	    sc::writeLogError(sc::string::format("DB:SQLConnect Failed %hd", sReturn));

	    //::SQLFreeHandle( SQL_HANDLE_DBC, spOdbc->hOdbc );
	    //SAFE_DELETE( spOdbc );
        std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
        return NullOdbc;
    }

	// Prepare statement handle
	sReturn = ::SQLAllocHandle(SQL_HANDLE_STMT, spOdbc->hOdbc, (SQLHANDLE *) &spOdbc->hStmt);
	if ( (sReturn != SQL_SUCCESS) && (sReturn == SQL_SUCCESS_WITH_INFO) )
	{
		Print(GetErrorString(spOdbc->hStmt));
		sc::writeLogError(sc::string::format("DB:SQLAllocHandle SQL_HANDLE_STMT Failed %hd", sReturn));
		//::SQLFreeHandle( SQL_HANDLE_DBC, spOdbc->hOdbc );
		//SAFE_DELETE( spOdbc );
        std::tr1::shared_ptr<db::ODBC_STMT> NullOdbc((db::ODBC_STMT*) 0);
        return NullOdbc;
	}

	return spOdbc;
}

void COdbcSupervisor::CloseDB()
{
}

std::tr1::shared_ptr<db::ODBC_STMT> COdbcSupervisor::GetConnection()
{    
    return CreateNewConnection();
}

void COdbcSupervisor::FreeConnectionPool(db::ODBC_STMT* pStmt)
{	
	if (pStmt)
    {
        sc::CriticalSectionOwnerLock OwnerLock(m_CriticalSection);
	    ::SQLCloseCursor(pStmt->hStmt);
	    ::SQLFreeHandle(SQL_HANDLE_STMT, pStmt->hStmt);
        pStmt->hStmt = NULL;
        pStmt->m_bUse = false;
	    //::SQLDisconnect(hConn->hOdbc);
	    //::SQLFreeHandle(SQL_HANDLE_DBC, hConn->hOdbc);
	    //SAFE_DELETE(hConn);
    }
}

/*
int	COdbcSupervisor::ExecuteSQL( std::strstream& strSQL )
{
	ODBC_STMT*	pConn = NULL;
    SQLRETURN	sReturn = 0;
	
	// Prepare statement handle
	pConn = GetConnection();
    if (pConn == NULL) return sc::db::DB_ERROR;
	
	sReturn = ::SQLExecDirect(
					pConn->hStmt, 
					(SQLCHAR*) strSQL.str(),
					SQL_NTS );
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{		
		FreeConnection(pConn);
		return sc::db::DB_OK;
	}
	else
	{
        std::string strError(sc::string::format("ERROR %1%", strSQL.str()));
		Print(strError);
		Print(GetErrorString(pConn->hStmt));
		FreeConnection(pConn);
		return sc::db::DB_ERROR;
	}
}
*/
/*
int	COdbcSupervisor::ExecuteSQL( CString strSQL )
{
	ODBC_STMT*	pConn = NULL;
    SQLRETURN	sReturn = 0;
	
	// Prepare statement handle
	pConn = GetConnection();
    if (!pConn) return sc::db::DB_ERROR;
	
	sReturn = ::SQLExecDirect(
					pConn->hStmt, 
					(SQLCHAR*) strSQL.GetString(), 
					SQL_NTS );
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{
		FreeConnection(pConn);
		return sc::db::DB_OK;
	}
	else
	{
		Print( strSQL.GetString() );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return sc::db::DB_ERROR;
	}
}
*/
/*
int COdbcSupervisor::ExecuteSp( std::strstream& strSP )
{
	ODBC_STMT* pConn = NULL;
	SQLRETURN sReturn = 0;
	
	pConn = GetConnection();
	if (!pConn)	return sc::db::DB_ERROR;
	
	::SQLPrepare( pConn->hStmt,(SQLCHAR*) strSP.str(), SQL_NTS );
	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
        Print( strSP.str() );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return sc::db::DB_ERROR;
	}
	else
	{
		FreeConnection(pConn);
		return sc::db::DB_OK;
	}
}
*/
/*
int COdbcSupervisor::ExecuteSpInt( std::strstream& strSP )
{
	ODBC_STMT* pConn = GetConnection();
	if (!pConn)	return sc::db::DB_ERROR;
		
	SQLRETURN  sReturn=0;
	SQLINTEGER nOutput=0;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare( pConn->hStmt, (SQLCHAR*) strSP.str(), SQL_NTS );
	
	// Bind the parameter.
	sReturn = ::SQLBindParameter(
					pConn->hStmt, 
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
		Print( strSP.str() );
		Print( _T("SQLBindParameter") );
		FreeConnection( pConn );
		return sc::db::DB_ERROR;
	}

	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print( strSP.str() );
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );
		return sc::db::DB_ERROR;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	else
	{
		FreeConnection( pConn );
	}
    return nOutput;
}
*/
int	COdbcSupervisor::ExecuteSQL(const std::string& Query)
{	
	SQLRETURN	sReturn = 0;
	// Prepare statement handle
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{
		//FreeConnection(spConn);
		return sc::db::DB_OK;
	}
	else
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
}

int COdbcSupervisor::ExecuteSp(const std::string& Query)
{	
	SQLRETURN sReturn = 0;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;


	//
	//mjeon.ToDo
	//SQLPrepare & SQLExecute are faster when they are used several times with same SQL stmt.
	//But in this case, there will not be any benefit of SQL speed because it's used only once.
	//
	//Anyway SQL-relative classes and functions must be re-modeled.
	//
	::SQLPrepare(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	sReturn = ::SQLExecute( spConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	else
	{
		//FreeConnection(spConn);
		return sc::db::DB_OK;
	}
}

int COdbcSupervisor::ExecuteSpInt(const std::string& Query)
{
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
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
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	sReturn = ::SQLExecute( spConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print(Query);
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	else
	{
		//FreeConnection( spConn );
	}
	return nOutput;
}


LONGLONG COdbcSupervisor::ExecuteSpLONGLONG( const TCHAR * szSP )
{
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	SQLRETURN  sReturn=0;
	SQLBIGINT nOutput=0;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare( spConn->hStmt, (SQLCHAR*)szSP, SQL_NTS );

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
		Print( szSP );
		Print( _T("SQLBindParameter") );
		//FreeConnection( spConn );
		return sc::db::DB_ERROR;
	}

	sReturn = ::SQLExecute( spConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print( szSP );
		Print( GetErrorString( spConn->hStmt ) );
		//FreeConnection( spConn );
		return sc::db::DB_ERROR;
	}
	else
	{
		//FreeConnection( spConn );
	}
	return nOutput;
}


int	COdbcSupervisor::ReadImagePet(
		const TCHAR* objName, 
		int nChaNum,
		int nPetNum, 
		se::ByteStream &ByteStream )
{
	if (objName == NULL)
        return sc::db::DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)		
	SQLCHAR		pBinary[sc::db::DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "SELECT %s FROM dbo.PetInfo where (PetNum=%d And PetChaNum=%d)", objName, nPetNum, nChaNum));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ( sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO )
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(spConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, sc::db::DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
				spConn->hStmt,
				1,
				SQL_C_BINARY,
				pBinary,
				sc::db::DB_IMAGE_BUF_SIZE,
				&lSize );
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

int	COdbcSupervisor::ReadImageVehicle(
    const TCHAR* objName, 
    int nVehicleNum,
    int nCharNum,
    se::ByteStream &ByteStream )
{
	if (objName == NULL)
        return sc::db::DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)		
	SQLCHAR		pBinary[sc::db::DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
	        "SELECT %s FROM dbo.VehicleInfo where (VehicleNum=%d) And (VehicleChaNum=%d)", objName, nVehicleNum, nCharNum));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(spConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, sc::db::DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
				spConn->hStmt,
				1,
				SQL_C_BINARY,
				pBinary,
				sc::db::DB_IMAGE_BUF_SIZE,
				&lSize );
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

int	COdbcSupervisor::ReadImagePostAttach(
    const TCHAR* objName, 
    LONGLONG llPostID, 
    se::ByteStream &ByteStream)
{
	if (objName == NULL)
        return sc::db::DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)		
	SQLCHAR		pBinary[sc::db::DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "SELECT %1% FROM dbo.PostInfo where (PostID=%2%)", objName, llPostID));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(spConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, sc::db::DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
				spConn->hStmt,
				1,
				SQL_C_BINARY,
				pBinary,
				sc::db::DB_IMAGE_BUF_SIZE,
				&lSize );
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

int	COdbcSupervisor::ReadImage(const TCHAR* objName, int nChaNum, se::ByteStream &ByteStream)
{
	if (objName == NULL)
        return sc::db::DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)		
	SQLCHAR		pBinary[sc::db::DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	
	
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
    if (!spConn)
        return sc::db::DB_ERROR;	
	
    std::string Query(
        sc::string::format(
            "SELECT %s FROM dbo.ChaInfo where (ChaNum=%d)", objName, nChaNum));

	// Create a result
	sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	
	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(spConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, sc::db::DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
							spConn->hStmt,
							1,
							SQL_C_BINARY,
							pBinary,
							sc::db::DB_IMAGE_BUF_SIZE,
							&lSize );
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	//FreeConnection(spConn);
	return sc::db::DB_OK;
}


int	COdbcSupervisor::ReadImage(std::strstream& strTemp,  int nChaNum, se::ByteStream &ByteStream)
{
	SQLRETURN	sReturn = 0; // SQL return value (short)		
	SQLCHAR		pBinary[sc::db::DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;
	
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
    if (!spConn)
        return sc::db::DB_ERROR;
	
	// Create a result
	sReturn = ::SQLExecDirect(
					spConn->hStmt,
					(SQLCHAR*) strTemp.str(),
					SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	
	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ((sReturn = ::SQLFetch(spConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::memset(pBinary, 0, sc::db::DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(spConn->hStmt, 1, SQL_C_BINARY, pBinary, sc::db::DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > sc::db::DB_IMAGE_BUF_SIZE) lSize = sc::db::DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBinary, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < sc::db::DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

int COdbcSupervisor::WriteImage(const TCHAR* strTemp, int nChaNum, BYTE* pData, int nSize)
{
	SQLRETURN	sReturn = 0; // SQL return value (short)
	// SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	// Prepare parameter
	sReturn = ::SQLPrepare(spConn->hStmt, (SQLCHAR*)strTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(spConn->hStmt, 
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
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(spConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(spConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(spConn->hStmt, pData, (SQLINTEGER) nSize);
		}
	}
	// Close the cursor.
	// ::SQLCloseCursor(hStmt);
	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

int COdbcSupervisor::WriteImage(
		std::strstream& strTemp, 
		int nChaNum, 
		BYTE* pData, 
		int nSize )
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	// SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = GetConnection();
    if (!spConn)
        return sc::db::DB_ERROR;
	
	// Prepare parameter
	sReturn = ::SQLPrepare(spConn->hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(GetErrorString(spConn->hStmt));
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(spConn->hStmt, 
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
		//FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(spConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
    {
		Print(GetErrorString(spConn->hStmt));
        //FreeConnection(spConn);
        return sc::db::DB_ERROR;
	}
	
	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(spConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(spConn->hStmt, pData, (SQLINTEGER) nSize);
		}
	}
    // Close the cursor.
	// ::SQLCloseCursor(hStmt);
	//FreeConnection(spConn);
	return sc::db::DB_OK;
}

void COdbcSupervisor::Print(const std::string& strMsg)
{
	sc::writeLogError(sc::string::format("DB:%1%", strMsg));
}

/*
void COdbcSupervisor::Print( const TCHAR * szMsg )
{
	sc::writeLogError(sc::string::format("DB:%s", szMsg));
}
*/
std::string COdbcSupervisor::GetErrorString(SQLHSTMT hStmt, SQLSMALLINT fHandleType)
{
	SQLRETURN sReturn = 0;
	SQLCHAR SqlState[6] = {0};
	SQLCHAR Msg[sc::db::ODBC_ERROR_MESSAGE_LENGTH+1] = {0};
	SQLSMALLINT MsgLen = 0;
	SQLINTEGER nError = 0;
	int nDiag = 0;

    std::string strTemp;

	if (!hStmt)
        return strTemp;

	::SQLGetDiagField(fHandleType, hStmt, 0, SQL_DIAG_NUMBER, &nDiag, 0, &MsgLen);

	for (nDiag=1;;nDiag++)
	{
		sReturn = ::SQLGetDiagRec(fHandleType, 
								  hStmt, 
								  nDiag,
								  SqlState,
								  &nError,
								  Msg,
								  sc::db::ODBC_ERROR_MESSAGE_LENGTH, 
								  &MsgLen);
		if (sReturn == SQL_NO_DATA)
			break;
		else if ( sReturn == SQL_DATA_AT_EXEC ) 
			break;
		else
            strTemp = sc::string::format("%1%, NativeError:%2%, %3%", (LPCTSTR) SqlState, nError, Msg);
	}
	return strTemp;
}