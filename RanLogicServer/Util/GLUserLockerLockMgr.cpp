#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Character/GLChar.h"

#include "./GLUserLockerLockMgr.h"
#include "../../SigmaCore/DebugInclude.h"


GLUserLockerLockMgr::GLUserLockerLockMgr( db::IDbManager* pDBMan )
: m_pAdoMan( pDBMan )
{
	open();
}

GLUserLockerLockMgr::~GLUserLockerLockMgr()
{
	close();
}

void GLUserLockerLockMgr::Lock( DWORD UserNum )
{
	std::string strQuery( sc::string::format( "INSERT INTO UserLock( UserNum ) VALUES( %1% );", UserNum ) );
	executeSQL( strQuery );
}

void GLUserLockerLockMgr::UnLock( DWORD UserNum )
{
	std::string strQuery( sc::string::format( "DELETE FROM UserLock WHERE UserNum = %1%;", UserNum ) );
	executeSQL( strQuery );
}

void GLUserLockerLockMgr::UnLock()
{
	std::string strQuery( "SELECT UserNum FROM UserLock" );
	SQLite::TablePtr pTable( m_UserLockerLock.QuerySQL2( strQuery.c_str() ) );
	if ( !pTable.m_pTable )
	{
		return;
	}

	SQLite::Table& rTable = pTable();
	int nRowCount = rTable.GetRowCount();
	if ( 0 == nRowCount )
	{
		return;
	}

	for ( int loop = 0; loop < nRowCount; ++loop )
	{
		DWORD UserNum = 0;
		int Option = 0;
		UserNum = rTable.GetValue< DWORD >( 0 );
		m_pAdoMan->UpdateUserLockerOption( UserNum, Option );
		rTable.GoNext();
	}

	std::string strQuery2( "DELETE FROM UserLock" );
	executeSQL( strQuery2 );
}

const std::string strCreateQuery = "\
								   CREATE TABLE IF NOT EXISTS UserLock( \
								   UserNum INTEGER NOT NULL, \
								   PRIMARY KEY( UserNum ) ); \
								   CREATE UNIQUE INDEX IF NOT EXISTS Idx_UserLock_UserNum ON UserLock ( UserNum );";

const std::string strDeleteQuery = "DELETE FROM UserLock WHERE UserNum = %1%;";

int GLUserLockerLockMgr::open()
{
	std::string strFile = sc::getAppPath();
	strFile.append( "\\userlocker_lock.db3" );
	int nResult = m_UserLockerLock.Open( strFile.c_str() );
	if ( SQLITE_OK != nResult )
	{
		std::string strMessage( 
			sc::string::format( "GLUserLockerLockMgr open Open critical error: %1%", m_UserLockerLock.GetLastError() ) );
		sc::ErrorMessageBox( strMessage, false );
		return eOpen;
	}

	nResult = executeSQL( strCreateQuery );
	if ( eSuccess != nResult )
	{
		return nResult;
	}

	return eSuccess;
}

void GLUserLockerLockMgr::close()
{
	if ( m_UserLockerLock.IsOpen() )
	{
		m_UserLockerLock.Close();
	}
}

int GLUserLockerLockMgr::executeSQL( const std::string& strQuery )
{
	if ( strQuery.empty() )
	{
		return eStringEmpty;
	}

	int nResult = SQLITE_OK;
	nResult = m_UserLockerLock.ExecuteSQL( strQuery );
	if ( SQLITE_OK != nResult )
	{
		std::string strMessage( 
			sc::string::format( 
			"GLUserLockerLockMgr execute error expression : %1% error : %2%", 
			strQuery.c_str(), m_UserLockerLock.GetLastError() ) );
		sc::writeLogError( strMessage );
		return eExecute;
	}

	return eSuccess;
}

