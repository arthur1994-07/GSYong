#ifndef _CODBCSUPERVISOR_H_
#define _CODBCSUPERVISOR_H_

#pragma once

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <Windows.h>
#include <strstream>
#include <stdio.h>

#include "../../../SigmaCore/Memory/CMemPooler.h"
#include "../../../SigmaCore/Database/Odbc/OdbcDefine.h"
#include "../../../SigmaCore/Database/DbDefine.h"

//#include "../../../RanLogic/s_NetGlobal.h"
//#include "../../../RanLogic/Util/s_CConsoleMessage.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"//
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"

#include "./s_COdbcEnv.h"

namespace db
{
    class ODBC_STMT;
}

//! \ingroup NetServerLib
//! \date 2003-07-12
//! \author jgkim
//! \todo
//!      ODBC Pooling 체크 http://www.tidytutorials.com/2009/10/sql-server-c-odbc-connection-pool.html
class COdbcSupervisor
{
public:
	COdbcSupervisor(bool UseCustomPool);
	COdbcSupervisor(
		const TCHAR* szOdbcName,
		const TCHAR* szUsrID,
		const TCHAR* szUsrPass,
		const TCHAR* szDBName,
        bool UseCustomPool
        /*int nPoolSize = sc::db::DB_POOL_SIZE, // 5 */
        /*int nDBTimeOut = sc::db::DB_RESPONSE_TIME*/ ); // 10
	~COdbcSupervisor();

protected:
	CRITICAL_SECTION m_CriticalSection;
    std::tr1::shared_ptr<db::ODBC_STMT> m_BaseConnection;

	//void lockOn();
	//void lockOff();

	TCHAR m_szOdbcName[sc::db::DB_SVR_NAME_LENGTH+1];
	TCHAR m_szUsrID[USR_ID_LENGTH+1];	
	TCHAR m_szUsrPass[USR_PASS_LENGTH+1];
	TCHAR m_szDBName[sc::db::DB_NAME_LENGTH+1];		
  //int m_nDBTimeOut;
  //int m_nPoolSize;
    bool m_bUseCustomPool; //! 직접 제작한 connection pool 을 사용할 것인가?

    std::vector<db::ODBC_STMT*> m_CustomPool;

	//sc::TCList<ODBC_STMT> m_Use;
	//sc::TCList<ODBC_STMT> m_UnUse;

	std::tr1::shared_ptr<db::ODBC_STMT>	CreateNewConnection();

private:
    void Init();

public:	
	void SetDB(
			const TCHAR* szOdbcName,
			const TCHAR* szUsrID,
			const TCHAR* szUsrPass,
			const TCHAR* szDBName);
	void SetOdbcName( const TCHAR* szName );
	void SetUserID( const TCHAR* szUsrID );
	void SetUserPass( const TCHAR* szUsrPasswd );
	void SetDBName( const TCHAR* szDBName );
	//void SetPoolSize( int nSize = sc::db::DB_POOL_SIZE );
    size_t GetPoolSize() const { return m_CustomPool.size(); }
	//void SetResponseTime( int nTime = sc::db::DB_RESPONSE_TIME );
	
	int	OpenDB();
	void CloseDB();

    std::tr1::shared_ptr<db::ODBC_STMT> GetConnection();	
    //! shared_ptr 로 수정되면서 필요없어졌음
    void FreeConnectionPool(db::ODBC_STMT* pStmt);

	//int	ExecuteSQL( CString strSQL );
	//int	ExecuteSQL( std::strstream& strSQL );
    int	ExecuteSQL(const std::string& Query);

	//int ExecuteSp( std::strstream& strSP );
    int ExecuteSp(const std::string& Query);
	//int ExecuteSpInt(std::strstream& strSP);
    int ExecuteSpInt(const std::string& Query);

	LONGLONG ExecuteSpLONGLONG( const TCHAR * szSP );

	int ReadImage(
			std::strstream& strTemp, 
			int nChaNum, 
			se::ByteStream &ByteStream );
	int	ReadImage(
			const TCHAR* objName, 
			int nChaNum, 
			se::ByteStream &ByteStream );
	int	ReadImagePostAttach(
			const TCHAR* objName, 
			LONGLONG llPostID, 
			se::ByteStream &ByteStream );
	int	ReadImagePet(
			const TCHAR* objName,
			int nChaNum,
			int nPetNum, 
			se::ByteStream &ByteStream );
	int	ReadImageVehicle(
		const TCHAR* objName, 
		int nVehicleNum,
		int nCharNum,
		se::ByteStream &ByteStream );
    int WriteImage(
			std::strstream& strTemp,
			int nChaNum, 
			BYTE* pData, 
			int nSize );
	int WriteImage(
			const TCHAR* strTemp,
			int nChaNum, 
			BYTE* pData, 
			int nSize );
    std::string GetErrorString(SQLHSTMT hStmt, SQLSMALLINT fHandleType = SQL_HANDLE_STMT);
	//void Print( CString strMsg );
    void Print(const std::string& Msg);

    bool IsAllDbJobCompleted();
};

#endif // _CODBCSUPERVISOR_H_
