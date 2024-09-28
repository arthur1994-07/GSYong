#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <vector>
#include <sstream>
#include <hash_map>

#include "ODBCSupervisor.h"
#include "CLock.h"
#include "../../Enginelib/ServiceProviderDefine.h"

class COdbcManager : public CDBManager, public CLock
{
public:	
	static COdbcManager* GetInstance();
	static void ReleaseInstance();


private:
	COdbcManager(void);
	~COdbcManager(void);
	static COdbcManager* SelfInstance;

public:
	void SetServiceProvider(EMSERVICE_PROVIDER ServiceProvider);

	int	OpenGameDB(
			TCHAR* szOdbcName, 
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseGameDB();

	int	OpenWebDB(
		TCHAR* szOdbcName, 
		TCHAR* szUsrID, 
		TCHAR* szUsrPasswd,
		TCHAR* szDbName,
		int nPoolSize = DB_POOL_SIZE,
		int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseWebDB();

	int	OpenUserDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseUserDB();

	int	OpenBoardDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseBoardDB();

	int	OpenLogDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseLogDB();

	int OpenShopDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseShopDB();
	
	int	OpenTerraDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseTerraDB();

	int	OpenGspDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseGspDB();

	int	OpenThaiDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseThaiDB();

	int	OpenKorDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseKorDB();

	// 말레이시아 PC방 이벤트
	int	OpenMyDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseMyDB();

	CString	GetErrorString(SQLHSTMT hStmt);
	void CheckConnection();


	//
	// In the CDBManager.h
	//
	/*
	void PrintMsg(const CString & strMsg);	
	
	//int	Command( TCHAR* szCmdStr );


	void SetDateControl( CDateTimeCtrl *pdateFrom, CDateTimeCtrl *pdateTo )
	{
		m_pdateFrom = pdateFrom;
		m_pdateTo   = pdateTo;
	}

	void SetExportControl( CButton *pcbExport )
	{
		m_pcbExport = pcbExport;
	}

	BOOL GetCheckExport()
	{
		if (m_pcbExport)
			return m_pcbExport->GetCheck();
		else
			return FALSE;		
	}

	void SetExceptKOR( CButton *pcbExceptKOR )
	{
		m_pcbExceptKOR = pcbExceptKOR;
	}
		
	BOOL GetCheckExceptKOR()
	{
		if (m_pcbExceptKOR)
			return m_pcbExceptKOR->GetCheck();
		else
			return FALSE;		
	}
	*/

	//
	// pure virtual functions
	//
	int SQLPage01_NewUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_NewChar( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_MonthlyUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage01_MonthlyShopIncome( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	
	int SQLPage02_AnaysisGameTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_ConcurrentUsers( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_ChaLevel( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );
	int SQLPage02_IP2Nation( CTime &ctFrom, CTime &ctTo, CListBox *plbResult );

	int SQLPage03_CC_Num( CTime &ctFrom, CTime &ctTo, BOOL bExceptKOR, CListBox *plbCCNum, CListBox *plbCCDetail, CListBox *plbUID );

	int SQLPage04_Daily_PlayTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult, CListBox *plbDetail );

protected:

	COdbcSupervisor* m_pGameDB;
	COdbcSupervisor* m_pWebDB;
	COdbcSupervisor* m_pUserDB;
	COdbcSupervisor* m_pBoardDB;
	COdbcSupervisor* m_pLogDB;
	COdbcSupervisor* m_pShopDB;
	

	COdbcSupervisor* m_pTerraDB;
	COdbcSupervisor* m_pGspDB;
	COdbcSupervisor* m_pThaiDB;
	COdbcSupervisor* m_pKorDB;
	COdbcSupervisor* m_pMalaysiaDB;			 // 말레이시아 PC방 이벤트

	EMSERVICE_PROVIDER m_ServiceProvider; ///< 서비스 제공업자

	//
	// In the CDBManager.h
	//
	/*
	CDateTimeCtrl	*m_pdateFrom;
	CDateTimeCtrl	*m_pdateTo;
	CButton			*m_pcbExport;
	CButton			*m_pcbExceptKOR;
	*/
};
