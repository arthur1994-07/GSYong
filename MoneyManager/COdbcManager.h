#ifndef _COdbcManager_H_
#define _COdbcManager_H_

#pragma once

#include <set>

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <Windows.h>
//#include <sqlext.h> // ODBC library
//#include <odbcinst.h>
#include <vector>
#include <strstream>

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h> // Safe string function

//#include "../RanLogic/s_NetGlobal.h"

#include "./DatabaseTable.h"
//#include "GLCharData.h"
//#include "GLogicEx.h"

// Auto Link Library Files
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"Winmm.lib")
#pragma comment (lib,"strsafe.lib")

// Seconds of database response time
#ifndef DB_RESPONSE_TIME
#define DB_RESPONSE_TIME	30
#endif

#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2
#define DB_CHA_DUF			-3

#define DB_CONNECTION_COUNT 10

#ifndef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE	2048
#else
#undef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE 2048
#endif

#ifndef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#else
#undef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#endif

// string data size
#ifndef USR_ID_LENGTH
#define USR_ID_LENGTH				20
#endif

#ifndef USR_PASS_LENGTH
#define USR_PASS_LENGTH				USR_ID_LENGTH
#endif

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH				33
#endif

#ifndef CHAT_MSG_SIZE
#define CHAT_MSG_SIZE				100
#endif

#ifndef GLOBAL_CHAT_MSG_SIZE
#define GLOBAL_CHAT_MSG_SIZE		981
#endif

#ifndef DB_SVR_NAME_LENGTH
#define DB_SVR_NAME_LENGTH			50
#endif

#ifndef DB_NAME_LENGTH
#define DB_NAME_LENGTH				50
#endif

// #ifndef MAX_SERVERCHAR_NUM
// #define MAX_SERVERCHAR_NUM			2
// #endif

//#ifndef MAX_ONESERVERCHAR_NUM
//#define MAX_ONESERVERCHAR_NUM		16 // 한 서버에서 만들수 있는 캐릭터 갯수
//#endif

#define MAX_ONESERVERCHAR_NUM_GMTOOL	16

#ifndef SERVER_NAME_LENGTH
#define SERVER_NAME_LENGTH			50
#endif

#ifndef MAP_NAME_LENGTH
#define MAP_NAME_LENGTH				50
#endif

struct SMoneyInfo
{
	int		 nNumber;
	LONGLONG lnMoney1;
	LONGLONG lnMoney2;

	SMoneyInfo()
		: nNumber(0)
		, lnMoney1(0)
		, lnMoney2(0)
	{
	}
};


class COdbcManager
{
public:
	static COdbcManager* GetInstance();
	static void	ReleaseInstance();

private:
	COdbcManager(void);
	~COdbcManager(void);

	static COdbcManager* SelfInstance;

	// 창고돈을 중복으로 에디트 하지 않기 위해
    std::set<int> m_setUserNum;

protected:
	SQLHENV m_hEnvUser;  // Handle of sql (void*)	
	SQLHDBC m_hOdbcUser; // Handle of ODBC (void*)

	SQLHENV m_hEnvGame;  // Handle of sql (void*)
	SQLHDBC m_hOdbcGame; // Handle of ODBC (void*)

public:

	// Basic Function
	int			OpenDB(char* szOdbcName,
		char* szUsrID,
		char* szUsrPasswd,
		char* szDbName,
		SQLHENV hEnv,
		SQLHDBC hOdbc);

	void		CloseDB(SQLHENV hEnv, SQLHDBC hOdbc);

	SQLHSTMT    GetStmt     (SQLHDBC hOdbc);
	void        FreeStmt    (SQLHSTMT hStmt);
	int			ExecuteSQL	(SQLHDBC hOdbc, CString strSQL);
	int			Insert		(SQLHDBC hOdbc, CString strSQL);
	int			Update		(SQLHDBC hOdbc, CString strSQL);
	int			Delete		(SQLHDBC hOdbc, CString strSQL);

	///////////////////////////////////////////////////////////////////////////
	int         OpenUserDB(char* szOdbcName,
						   char* szUsrID,
						   char* szUsrPasswd);

	int         OpenGameDB( const char* szOdbcName, 
							const char* szUsrID, 
							const char* szUsrPasswd);
	
	void		CloseUserDB();
	void		CloseGameDB();

	///////////////////////////////////////////////////////////////////////////
	// DB 작업관련 함수
	int			GetChaSeq();
	BOOL		GetTempMoneyInfo( const int nSeqNum, int &nChaNum, LONGLONG &lnTempMoney, const int MONEY_VER );
	BOOL		GetGameMoneyInfo( const int nChaNum, int &nUserNum, LONGLONG &lnGameMoney );
	BOOL		GetTempSTOMoneyInfo( const int nUserNum, LONGLONG &lnStorageMoney, const int MONEY_VER );
	BOOL		GetSTOMoneyInfo( const int nUserNum, LONGLONG &lnStorageMoney );

	BOOL		UpdateTempMoneyInfo( const int nChaNum, LONGLONG lnGameMoney, const int MONEY_VER );
	BOOL		UpdateGameMoneyInfo( int nChaNum, LONGLONG lnGameMoney );
	BOOL		UpdateTempSTOMoneyInfo( const int nUserNum, LONGLONG lnStorageMoney, const int MONEY_VER );
	BOOL		UpdateSTOMoneyInfo( const int nUserNum, LONGLONG lnStorageMoney );

	BOOL		DecryptGameMoney( const int nSeqNum, int &nGameChaNum, int &nGameUserNum, LONGLONG &lnChaMoney, const int MONEY_VERSION );
	BOOL		DecryptSTOMoney( const int nGameUserNum, LONGLONG &lnStorageMoney, const int MONEY_VERSION );
	BOOL		EncryptGameMoney( const int nChaNum, LONGLONG lnChaMoney, const int MONEY_VERSION );
	BOOL		EncryptSTOMoney( const int nGameUserNum, LONGLONG lnStorageMoney, const int MONEY_VERSION );

	SMoneyInfo	UpdateGameMoney( const int nSeqNum, float fValue, BOOL bMultiply, const int MONEY_VERSION );
	SMoneyInfo	UpdateSTOMoney( const int nGameUserNum, float fValue, BOOL bMultiply, const int MONEY_VERSION );
	BOOL		CheckGameMoney( const int nSeqNum, SMoneyInfo &moneyInfo, const int MONEY_VERSION );
	BOOL		CheckSTOMoney( const int nGameUserNum, SMoneyInfo &moneyInfo, const int MONEY_VERSION );

	
	// Ver 100 => Ver 101 로 새로 암호화
	SMoneyInfo  NewEncryptGameMoney_Ver100to101( int nSeqNum );
	SMoneyInfo  NewEncryptSTOMoney_Ver100to101( int nUserNum );
	
	// Seq로 UserNum을 찾음
	int			GetSeqUserNum( const int nSeqNum, const int MONEY_VERSION );
	
	
};

#endif _COdbcManager_H_