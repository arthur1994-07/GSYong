#include "../../pch.h"
#include "./s_COdbcEnv.h"

#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../SigmaCore/DebugInclude.h"



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
	if (SelfInstance)
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
		sc::writeLogInfo(std::string("SQL_ATTR_CONNECTION_POOLING OK"));
	}
	else
	{
		sc::writeLogError(std::string("SQL_ATTR_CONNECTION_POOLING Failed"));

		return sc::db::DB_ERROR;
	}

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		sc::writeLogInfo(std::string("SQLAllocHandle OK"));
	}
	else
	{
		sc::writeLogError(std::string("SQLAllocHandle Failed"));

		return sc::db::DB_ERROR;
	}

	// ODBC set environment
	// version	
	sReturn = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = NULL;
		return sc::db::DB_ERROR;
	}

	return sc::db::DB_OK;
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