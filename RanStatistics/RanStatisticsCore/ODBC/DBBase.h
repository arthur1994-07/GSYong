#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Export2File.h"

// Seconds of database response time
#ifndef DB_RESPONSE_TIME
#define DB_RESPONSE_TIME	10
#endif
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0
#define DB_ROWCNT_ERROR		1

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2
#define DB_CHA_DUF			-3

#define DB_INSERT_ITEM		-2
#define DB_NOT_ITEM			-3

#define EXTREME_OK			1

#ifndef DB_PACKET_SIZE
#define DB_PACKET_SIZE		8192
#endif

#ifndef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE	1024
#endif

#ifndef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#endif

#ifndef DB_POOL_SIZE
#define DB_POOL_SIZE	5
#endif

#ifndef ODBC_ERROR_MESSAGE_LENGTH
#define ODBC_ERROR_MESSAGE_LENGTH 250
#endif

#define MAX_USERID_LENGTH	64
#define MAX_CHANAME_LENGTH	64


#define ADO_CONN_STR_USERDB	"Provider=SQLOLEDB.1;Password=RanUser@@;Persist Security Info=True;User ID=RanUser;Initial Catalog=RanUser;Data Source=59.23.229.216"
#define ADO_CONN_STR_GAMEDB	"Provider=SQLOLEDB.1;Password=RanGameS1@@;Persist Security Info=True;User ID=RanGameS1;Initial Catalog=RanGameS1;Data Source=59.23.229.216"
#define ADO_CONN_STR_LOGDB	"Provider=SQLOLEDB.1;Password=RanLog@@;Persist Security Info=True;User ID=RanLog;Initial Catalog=RanLog;Data Source=59.23.229.216"
#define ADO_CONN_STR_SHOPDB	"Provider=SQLOLEDB.1;Password=RanShop@@;Persist Security Info=True;User ID=RanShop;Initial Catalog=RanShop;Data Source=59.23.229.216"



class CDBManager
{
public:
	CDBManager(void) {};
	virtual ~CDBManager(void) {};

public:
	static void PrintMsg(const CString & strMsg)
	{
		CConsoleMessage::GetInstance()->PrintMsg( _T("%s"), strMsg.GetString() );
	};

	virtual int SQLPage01_NewUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )			=0;
	virtual int SQLPage01_NewChar( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )			=0;
	virtual int SQLPage01_MonthlyUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )		=0;
	virtual int SQLPage01_MonthlyShopIncome( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )	=0;
	
	virtual int SQLPage02_AnaysisGameTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )	=0;
	virtual int SQLPage02_ConcurrentUsers( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )	=0;
	virtual int SQLPage02_ChaLevel( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )			=0;
	virtual int SQLPage02_IP2Nation( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )			=0;
	
	virtual int SQLPage03_CC_Num( CTime &ctFrom, CTime &ctTo, BOOL bExceptKOR, CListBox *plbCCNum, CListBox *plbCCDetail, CListBox *plbUID )=0;
	
	virtual int SQLPage04_Daily_PlayTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult, CListBox *plbDetail )							=0;
};

