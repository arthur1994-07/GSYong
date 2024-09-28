#include "stdafx.h"
#include "ODBCEnv.h"
#include "ConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COdbcEnv* COdbcEnv::SelfInstance = NULL;

COdbcEnv::COdbcEnv(void) 
	: m_hEnv(NULL)
{
	CloseHandle();

	//
	//mjeon
	//CreateHandle sets up the SQL env.
	//
	CreateHandle();	
}

COdbcEnv::~COdbcEnv(void)
{
	CloseHandle();
}

COdbcEnv* COdbcEnv::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new COdbcEnv();
	return SelfInstance;
}

void COdbcEnv::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

int COdbcEnv::CreateHandle()
{
	CloseHandle();

	SQLRETURN sReturn; // SQL return value (short);


	//
	//mjeon
	//ODBC connection pooling was first introduced with ODBC version 3.0.	
	//	
	/*Connection pooling is enabled by calling SQLSetEnvAttr to set the SQL_ATTR_CONNECTION_POOLING attribute 
	to SQL_CP_ONE_PER_DRIVER or SQL_CP_ONE_PER_HENV. 
	This call must be made before the application allocates the shared environment
	for which connection pooling is to be enabled.
	The environment handle in the call to SQLSetEnvAttr is set to null,
	which makes SQL_ATTR_CONNECTION_POOLING a process-level attribute.
	After connection pooling is enabled, the application then allocates an implicit shared environment 
	by calling SQLAllocHandle with the InputHandle argument set to SQL_HANDLE_ENV.*/


	// Connection pooling
	sReturn = ::SQLSetEnvAttr(
					NULL,
					SQL_ATTR_CONNECTION_POOLING,
					(void*) SQL_CP_ONE_PER_HENV,
					0 );
	// sReturn = ::SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (void*) SQL_CP_ONE_PER_DRIVER, 0);	
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: SQL Connection Pooling has been set.") );
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg( _T("ERROR: SQL Connection Pooling could not be set.") );
		return DB_ERROR;
	}

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: SQL Environment handle has been allocated."));
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg( _T("ERROR: SQL Environment handle could not been allocated.") );
		return DB_ERROR;
	}

	// ODBC set environment
	// version	
	sReturn = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = NULL;
		return DB_ERROR;
	}

	return DB_OK;
}

void COdbcEnv::CloseHandle()
{
	if (m_hEnv != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = NULL;
	}
}

SQLHENV COdbcEnv::GetHandle()
{
	return m_hEnv;
}