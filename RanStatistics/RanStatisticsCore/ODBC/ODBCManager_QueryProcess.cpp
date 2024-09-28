#include "stdafx.h"
#include "ODBCManager.h"
#include "GeoIP.h"
#include "Export2File.h"
#include <set>
#include <map>

#define MAX_USERID_LENGTH	64
#define MAX_CHANAME_LENGTH	64

using namespace std;

int COdbcManager::SQLPage01_NewUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;
	
	//UserDB
	ODBC_STMT* pConn = NULL;
	pConn = m_pUserDB->GetConnection();

	if (!pConn)
		return DB_ERROR;

	SQLCHAR		szDate[21]	= {0};  SQLINTEGER cbDate		= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt		= SQL_NTS;	

	TCHAR szQuery[1024] = {0};

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");


	_snprintf( szQuery,1024, 
				_T(
					"SELECT Convert(varchar(10), CreateDate, 21) AS CreateDate, Count(*) AS Cnt "	\
					"FROM GSUserInfo "																\
					"WHERE CreateDate BETWEEN '%s' AND '%s' "										\
					"GROUP BY Convert(varchar(10), CreateDate, 21) "								\
					"ORDER BY CreateDate ASC"
					), csFrom.GetString(), csTo.GetString() );


	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		PrintMsg(_T("ERROR: SQLPage01_NewUser"));

		CString tmp;
		PrintMsg( tmp = GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);
				
				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,	21, &cbDate );
				::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);				
			}
			else	//no more columns
			{				
				break;
			}

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-s\t%-d"), szDate, nCnt);
		}
	}
	m_pUserDB->FreeConnection(pConn);
	

	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );

	return DB_OK;
}


int COdbcManager::SQLPage01_NewChar( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	//UserDB
	ODBC_STMT* pConn = NULL;
	pConn= m_pGameDB->GetConnection();

	if (!pConn)
		return DB_ERROR;

	SQLCHAR		szDate[21]	= {0};  SQLINTEGER cbDate	= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt	= SQL_NTS;

	TCHAR szQuery[1024] = {0};

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	_snprintf( szQuery,1024, 
				_T(
					"SELECT Convert(varchar(10), ChaCreateDate, 21) AS CreateDate, Count(*) AS Cnt "	\
					"FROM ChaInfo "																		\
					"WHERE ChaCreateDate BETWEEN '%s' AND '%s' "										\
					"GROUP BY Convert(varchar(10), ChaCreateDate, 21) "									\
					"ORDER BY CreateDate ASC"
					), csFrom.GetString(), csTo.GetString() );


	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{		
		CString tmp;
		PrintMsg(_T("ERROR: SQLPage01_NewChar"));
		PrintMsg(tmp = GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConn->hStmt));
				m_pGameDB->FreeConnection(pConn);

				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,	21, &cbDate );
				::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);				
			}
			else	//no more columns
			{				
				break;
			}

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-s\t%-d"), szDate, nCnt);
		}
	}
	m_pGameDB->FreeConnection(pConn);

	
	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );

	return DB_OK;
}


//Number of distinct users who played the game in a month
int COdbcManager::SQLPage01_MonthlyUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	SQLCHAR		szDate[21]	= {0};  SQLINTEGER cbDate		= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt		= SQL_NTS;	

	TCHAR szQuery[1024] = {0};	
	TCHAR buffer[12] = {0,};


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	
	int nYearFrom = ctFrom.GetYear();
	int nYearTo = ctTo.GetYear();


	struct PAIRDATE {
		CString csFrom;
		CString csTo;
	};

	int cnt = nYearTo - nYearFrom + 1;
	int idx = 0;
	
	// Storage for 10-years Maximum
	PAIRDATE pd[20];	//1-Year per 1-PAIRDATE

	if ( nYearFrom < nYearTo )
	{
		// First Date
		//From
		//m_pdateFrom->GetWindowText( (pd[0].csFrom) );
		pd[0].csFrom = csFrom;
		
		//To
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom, 12, 31);
		pd[0].csTo.SetString( buffer );

		idx++;

		
		for ( int i=idx; i < cnt - 1; i++)
		{
			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 01, 01);
			pd[idx].csFrom.SetString( buffer );

			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 12, 31);
			pd[idx++].csTo.SetString( buffer );
		}
		

		// Last Date
		//From
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearTo, 01, 01);        
		pd[cnt-1].csFrom.SetString( buffer );

		//To
		pd[cnt-1].csTo = csTo;

        
	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].csFrom	= csFrom;
		pd[0].csTo		= csTo;
		//m_pdateFrom->GetWindowText(pd[0].csFrom);
		//m_pdateTo->GetWindowText(pd[0].csTo);
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}


	for ( int i=0; i<cnt; i++ )
	{
		//UserDB
		ODBC_STMT* pConn = NULL;
		pConn = m_pUserDB->GetConnection();

		if (!pConn)
			return DB_ERROR;


		_snprintf( szQuery,1024, 
					_T(
						"SELECT DATEPART(mm, LogDate) As LDate, COUNT(DISTINCT UserID) "	\
						"FROM GSLogGameTime "												\
						"WHERE LogDate BETWEEN '%s' AND '%s' AND GameTime > 0 "				\
						"GROUP BY DATEPART(mm, LogDate) "									\
						"ORDER BY DATEPART(mm, LogDate) "						
						), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

		if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
		{		
			PrintMsg(_T("ERROR: SQLPage01_NewChar"));
			PrintMsg(GetErrorString(pConn->hStmt));
			m_pUserDB->FreeConnection(pConn);

			return DB_ERROR;
		}
		else
		{
			PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

			while(true)
			{
				sReturn = ::SQLFetch(pConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					PrintMsg(szQuery);
					PrintMsg(GetErrorString(pConn->hStmt));
					m_pUserDB->FreeConnection(pConn);

					return DB_ERROR;
				}
				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,	21, &cbDate );
					::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);				
				}
				else	//no more columns
				{				
					break;
				}
				
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-d"), szDate, nCnt);
			}
		}

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));

		m_pUserDB->FreeConnection(pConn);

		
		PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );		
	}	

	return DB_OK;
}


int COdbcManager::SQLPage01_MonthlyShopIncome( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	SQLCHAR		szDate[21]	= {0};  SQLINTEGER cbDate		= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt		= SQL_NTS;

	TCHAR szQuery[1024] = {0};
	TCHAR buffer[12] = {0,};


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	
	int nYearFrom = ctFrom.GetYear();
	int nYearTo = ctTo.GetYear();

	struct PAIRDATE {
		CString csFrom;
		CString csTo;	
	};

	int cnt = nYearTo - nYearFrom + 1;
	int idx = 0;

	// Storage for 10-years Maximum
	PAIRDATE pd[10];	//1-Year per 1-PAIRDATE

	if ( nYearFrom < nYearTo )
	{
		// First Date
		//From
		//m_pdateFrom->GetWindowText( (pd[0].csFrom) );
		pd[0].csFrom = csFrom;

		//To
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom, 12, 31);
		pd[0].csTo.SetString( buffer );

		idx++;


		for ( int i=idx; i < cnt - 1; i++)
		{
			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 01, 01);
			pd[idx].csFrom.SetString( buffer );

			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 12, 31);
			pd[idx++].csTo.SetString( buffer );
		}


		// Last Date
		//From
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearTo, 01, 01);        
		pd[cnt-1].csFrom.SetString( buffer );

		//To
		//m_pdateTo->GetWindowText( (pd[cnt-1].csTo) );
		pd[cnt-1].csTo = csTo;


	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].csFrom	= csFrom;
		pd[0].csTo		= csTo;

		//m_pdateFrom->GetWindowText(pd[0].csFrom);
		//m_pdateTo->GetWindowText(pd[0].csTo);
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}


	for ( int i=0; i<cnt; i++ )
	{
		//UserDB
		ODBC_STMT* pConn = NULL;
		pConn = m_pShopDB->GetConnection();

		if (!pConn)
			return DB_ERROR;


		_snprintf( szQuery,1024, 
			_T(
				"SELECT DATEPART(mm, PurDate) AS Month, SUM(PurPrice) AS Price "	\
				"FROM ShopPurchase "												\
				"WHERE PurDate BETWEEN '%s' AND '%s' "								\
				"GROUP BY DATEPART(mm, PurDate)"									\
				"ORDER BY DATEPART(mm, PurDate)"
			), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

		if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
		{		
			PrintMsg(_T("ERROR: SQLPage01_NewChar"));
			PrintMsg(GetErrorString(pConn->hStmt));
			m_pShopDB->FreeConnection(pConn);

			return DB_ERROR;
		}
		else
		{
			PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

			while(true)
			{
				sReturn = ::SQLFetch(pConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					PrintMsg(szQuery);
					PrintMsg(GetErrorString(pConn->hStmt));
					m_pShopDB->FreeConnection(pConn);

					return DB_ERROR;
				}
				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,	21, &cbDate );
					::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);				
				}
				else	//no more columns
				{				
					break;
				}

				// $1 = 10 points.
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-d"), szDate, nCnt/10);
			}
		}

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));		

		m_pShopDB->FreeConnection(pConn);

		
		PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );
	}	

	return DB_OK;
}


int COdbcManager::SQLPage02_AnaysisGameTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	SQLCHAR		szDate[21]		= {0};	SQLINTEGER cbDate			= SQL_NTS;
	SQLINTEGER	cntTotalUsers	= 0;	SQLINTEGER cbTotalUsers		= SQL_NTS;
	SQLINTEGER	cntPlayedUsers	= 0;	SQLINTEGER cbPlayedUsers	= SQL_NTS;
	SQLINTEGER	cntTotalGameTime= 0;	SQLINTEGER cbTotalGameTime	= SQL_NTS;

	TCHAR szQuery[1024] = {0};
	TCHAR buffer[12] = {0,};


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	
	int nYearFrom = ctFrom.GetYear();
	int nYearTo = ctTo.GetYear();

	
	struct PAIRDATESTRING {
		CString csFrom;
		CString csTo;
	};


	int cntYear = nYearTo - nYearFrom + 1;
	int idx = 0;

	// Storage for 10-years Maximum
	PAIRDATESTRING pd[10];	//1-Year per 1-PAIRDATESTRING

	if ( nYearFrom < nYearTo )
	{
		// First Date
		//From
		pd[0].csFrom = csFrom;
		//m_pdateFrom->GetWindowText( (pd[0].csFrom) );

		//To
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom, 12, 31);
		pd[0].csTo.SetString( buffer );

		idx++;

		for ( int i=idx; i < cntYear - 1; i++)
		{
			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 01, 01);
			pd[idx].csFrom.SetString( buffer );

			_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearFrom + idx, 12, 31);
			pd[idx++].csTo.SetString( buffer );
		}


		// Last Date
		//From
		_stprintf(buffer, "%4d-%0.2d-%0.2d", nYearTo, 01, 01);        
		pd[cntYear-1].csFrom.SetString( buffer );

		//To
		//m_pdateTo->GetWindowText( (pd[cntYear-1].csTo) );
		pd[cntYear-1].csTo = csTo;


	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].csFrom	= csFrom;
		pd[0].csTo		= csTo;
		//m_pdateFrom->GetWindowText(pd[0].csFrom);
		//m_pdateTo->GetWindowText(pd[0].csTo);
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}	

	for ( int i=0; i<cntYear; i++ )
	{
		//UserDB
		ODBC_STMT* pConn = NULL;
		pConn = m_pUserDB->GetConnection();

		if (!pConn)
			return DB_ERROR;


		_snprintf( szQuery,1024, 
			_T(
				"SELECT DATEPART(mm, LogDate) As Date, MAX(UserNum) AS TotalUser, "				\
				"COUNT(DISTINCT UserID) AS PlayedUsers, SUM(GameTime) AS TotalGameTime "		\
				"FROM GSLogGameTime "															\
				"WHERE LogDate between '%s' and '%s' and GameTime > 0 "							\
				"GROUP By DATEPART(mm, LogDate) "												\
				"ORDER BY DATEPART(mm, LogDate) "												\
			), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

		if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
		{		
			PrintMsg(_T("ERROR: SQLPage01_NewChar"));
			PrintMsg(GetErrorString(pConn->hStmt));
			m_pUserDB->FreeConnection(pConn);

			return DB_ERROR;
		}
		else
		{
			PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

			while(true)
			{
				sReturn = ::SQLFetch(pConn->hStmt);
				if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
				{
					PrintMsg(szQuery);
					PrintMsg(GetErrorString(pConn->hStmt));
					m_pUserDB->FreeConnection(pConn);

					return DB_ERROR;
				}
				if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
				{	
					::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,	21, &cbDate );
					::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&cntTotalUsers,		 0, &cbTotalUsers);
					::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	&cntPlayedUsers,	 0, &cbPlayedUsers);
					::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,	&cntTotalGameTime,	 0, &cbTotalGameTime);
				}
				else	//no more columns
				{				
					break;
				}

				// $1 = 10 points.
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%0.5d\t%0.5d\t%0.5d\t%0.1f\t%0.1f"), 
														szDate, cntTotalUsers, cntPlayedUsers, 
														cntTotalGameTime/60, (float)cntTotalGameTime/60/cntTotalUsers,
														(float)cntTotalGameTime/60/cntPlayedUsers);
			}
		}

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));		

		m_pUserDB->FreeConnection(pConn);

		
		PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );
	}	

	return DB_OK;
}


int COdbcManager::SQLPage02_ConcurrentUsers( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	SQLCHAR		szDate[21]		= {0};	SQLINTEGER cbDate			= SQL_NTS;
	SQLINTEGER	cntTotalCU	= 0;		SQLINTEGER cbTotalCU		= SQL_NTS;
	SQLINTEGER	cntAvgCU	= 0;		SQLINTEGER cbAvgCU			= SQL_NTS;	

	TCHAR szQuery[1024] = {0};
	TCHAR buffer[12] = {0,};
	
	CString csFrom, csTo;

	int nYearFrom = ctFrom.GetYear();
	int nYearTo = ctTo.GetYear();
	

	int MONTHDAYS[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	//1base

    	
	struct PAIRDATE {
		CTime ctFrom;
		CTime ctTo;	
	};

	int cntYear = nYearTo - nYearFrom + 1;
	int idx = 0;

	// Storage for 10-years Maximum
	PAIRDATE pd[10];	//1-Year per 1-PAIRDATE

	if ( nYearFrom < nYearTo )
	{
		// First Date
		//From
        pd[0].ctFrom = ctFrom;
		//m_pdateFrom->GetTime(pd[0].ctFrom);

		//To		
		pd[0].ctTo = CTime (nYearFrom, 12, 31, 23, 59, 59);

		idx++;


		for ( int i=idx; i < cntYear - 1; i++)
		{			
			pd[idx].ctFrom = CTime(nYearFrom + idx, 01, 01, 00, 00, 00);

			pd[idx++].ctTo = CTime(nYearFrom + idx, 12, 31, 23, 59, 59);
		}


		// Last Date
		//From		
		pd[cntYear-1].ctFrom = CTime (nYearTo, 01, 01, 00, 00, 00);

		//To		
		//m_pdateTo->GetTime( (pd[cntYear-1].ctTo) );
		pd[cntYear-1].ctTo = ctTo;
		


	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].ctFrom	= ctFrom;
		pd[0].ctTo		= ctTo;

		//m_pdateFrom->GetTime( (pd[0].ctFrom) );
		//m_pdateTo->GetTime( (pd[0].ctTo) );
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;	
	}


    CString csBaseWeekNum;
	int sumTotalCU = 0, sumAvgCu = 0, cntWeeks = 0;
	

	for ( int i=0; i<cntYear; i++ )
	{
		for ( int j=pd[i].ctFrom.GetMonth(); j <= pd[i].ctTo.GetMonth(); j++ )
		{			
			_stprintf(buffer, "%4d-%0.2d-%0.2d", pd[i].ctFrom.GetYear(), j, (i==0 && j==pd[i].ctFrom.GetMonth()) ? pd[i].ctFrom.GetDay() : 01);
			csFrom.SetString(buffer);

			_stprintf(buffer, "%4d-%0.2d-%0.2d", pd[i].ctTo.GetYear(), j, MONTHDAYS[j]);
			csTo.SetString(buffer);

			_stprintf(buffer, "%4d-%0.2d-%0.2d", pd[i].ctFrom.GetYear(), j, 01);
			csBaseWeekNum.SetString(buffer);


			//UserDB
			ODBC_STMT* pConn = NULL;
			pConn = m_pLogDB->GetConnection();

			if (!pConn)
				return DB_ERROR;


			_snprintf( szQuery,1024, 
				_T(
				"SELECT DATEPART(ww, LogDate) - DATEPART(ww, '%s') + 1 AS Date, AVG(UserNum) AS AVG, MAX(UserNum) AS MAX "	\
				"FROM LogServerState "																\
				"WHERE LogDate between '%s' and '%s' "												\
				"GROUP By DATEPART(ww, LogDate) "													\
				"ORDER BY DATEPART(ww, LogDate) "
				), csBaseWeekNum.GetString(), csFrom.GetString(), csTo.GetString() );


			sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

			if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
			{		
				PrintMsg(_T("ERROR: SQLPage01_NewChar"));
				CString tmp;
				PrintMsg(tmp=GetErrorString(pConn->hStmt));
				m_pLogDB->FreeConnection(pConn);

				return DB_ERROR;
			}
			else
			{
				PrintMsg( _T("OK: SQL Excuted Query successfuly.") );
				
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d/%0.2d ]"), nYearFrom + i, j);
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("----------------------") );

				while(true)
				{
					sReturn = ::SQLFetch(pConn->hStmt);
					if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
					{
						PrintMsg(szQuery);
						PrintMsg(GetErrorString(pConn->hStmt));
						m_pLogDB->FreeConnection(pConn);

						return DB_ERROR;
					}
					if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
					{	
						::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szDate,		21, &cbDate );
						::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&cntAvgCU,	 0, &cbAvgCU);
						::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	&cntTotalCU, 0, &cbTotalCU);						
					}
					else	//no more columns
					{				
						break;
					}

					// $1 = 10 points.
					CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-3d\t%-3d"), 
						szDate, cntAvgCU, cntTotalCU );

					sumTotalCU += cntTotalCU;
					sumAvgCu += cntAvgCU;
					cntWeeks++;
				}
			}

			if (cntWeeks != 0)
			{
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("----------------------") );
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Avg\t%-3d\t%-3d"), sumAvgCu/cntWeeks, sumTotalCU/cntWeeks);
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("----------------------") );
				sumAvgCu = 0;
				sumTotalCU = 0;
				cntWeeks = 0;

				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));
			}

			m_pLogDB->FreeConnection(pConn);

			PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );
		}	//for j
	}
	

	return DB_OK;
}



int COdbcManager::SQLPage02_ChaLevel( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;	

	//UserDB
	ODBC_STMT* pConn = NULL;
	pConn = m_pGameDB->GetConnection();

	if (!pConn)
		return DB_ERROR;

	SQLCHAR		szLv[21]	= {0};  SQLINTEGER cbLv	= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt	= SQL_NTS;	

	TCHAR szQuery[1024] = {0};

	int sum = 0;
	int lvPrevRange = 0; // Level Range starts from 0.

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");


	_snprintf( szQuery,1024, 
		_T(
		"SELECT CAST(ChaLevelGroup AS varchar(10)) AS ChaLevelGroupName, ChaUse "					\
		"FROM "																						\
		"(SELECT ChaLevelGroup, Sum(ChaUse) AS ChaUse From "										\
		"(SELECT ChaLevel, UserNum, ChaLevel AS ChaLevelGroup, 1-ChaDeleted As ChaUse From ChaInfo "\
		"WHERE ChaCreateDate BETWEEN '%s' and '%s' "												\
		"GROUP BY ChaLevel, ChaDeleted, UserNum) AS t "												\
		"GROUP BY ChaLevelGroup) as tt "															\
		"ORDER BY ChaLevelGroup "
		), csFrom.GetString(), csTo.GetString() );


	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{		
		PrintMsg(_T("ERROR: SQLPage01_NewChar"));
		PrintMsg(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConn->hStmt));
				m_pGameDB->FreeConnection(pConn);

				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szLv,	21, &cbLv );
				::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);				
			}
			else	//no more columns
			{				
				break;
			}
			
			int nLv = _ttoi((const TCHAR *)szLv);			

			if (nLv / 10 > lvPrevRange)
			{
				lvPrevRange = nLv / 10;

				//CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("---------") );
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Total\t%-d"), sum);
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("-------------") );
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );
				sum = 0;
			}

			sum += nCnt;

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-3s\t%-d"), szLv, nCnt);
		}
	}

	//CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("---------") );
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Total\t%-d"), sum);
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("-------------") );
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );

	m_pGameDB->FreeConnection(pConn);
	

	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );

	return DB_OK;
}


int COdbcManager::SQLPage02_IP2Nation( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	SQLRETURN	sReturn = 0;

	multiset<CString> ms;
	multiset<CString>::iterator ims;	
	set<CString> s;
	set<CString>::iterator is;	
	CString item;

	const char *country;
	unsigned int cntNoIP = 0;
	unsigned int cntTotalIP = 0;	

	//UserDB
	ODBC_STMT* pConn = NULL;
	pConn = m_pUserDB->GetConnection();

	if (!pConn)
		return DB_ERROR;

	SQLCHAR		szIP[16]	= {0};  SQLINTEGER cbIP		= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt	= SQL_NTS;	

	TCHAR szQuery[1024] = {0};
	

	GeoIP * gi = NULL;
	gi = GeoIP_open("GeoIP.dat", GEOIP_STANDARD | GEOIP_CHECK_CACHE);

	if (gi == NULL) {
		PrintMsg(_T("ERROR: GeoIP database could not be opened."));		
		return FALSE;
	}


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");


	_snprintf( szQuery,1024, 
		_T(
		"SELECT LastUserIP, COUNT(LastUserIP) AS Cnt "	\
		"FROM GSUserInfo "								\
		"WHERE LastLoginDate BETWEEN '%s' and '%s' "	\
		"GROUP BY LastUserIP "							\
		"ORDER BY LastUserIP "
		), csFrom.GetString(), csTo.GetString() );

	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		PrintMsg(_T("ERROR: SQLPage03_Ip2Nation"));
		PrintMsg(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szIP,	16, &cbIP );
				::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nCnt,	 0, &cbCnt);
			}
			else	//no more columns
			{				
				break;
			}

			if (_tcscmp((TCHAR *)szIP, _T("(null)")) == 0)
				continue;

			country = GeoIP_country_code3_by_addr(gi,(const char *)szIP);

			if (country == NULL)
			{
				cntNoIP++;
				continue;
			}

			cntTotalIP++;

			item.SetString(country);

			s.insert(item);
			ms.insert(item);

			//CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %2s  %3d"), country, nCnt);
		}
	}

	//Process IP addresses and country codes.
		
	for (is = s.begin(); is != s.end(); ++is)
	{
		CString item = *is;		
		
		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-4s\t%-4d"), item.GetString(), ms.count( item ) ); //country, nCnt);
	}

	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("-------------"));
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Total\t%-4d"), cntTotalIP);
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("No IP\t%-4d"), cntNoIP);

	GeoIP_delete(gi);

	m_pUserDB->FreeConnection(pConn);
	

	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );

	return DB_OK;
}



//
// Different version of IP2Nation is designed for KorLive DB and used in Page03.
//

int COdbcManager::SQLPage03_CC_Num( CTime &ctFrom, CTime &ctTo, BOOL bExceptKOR, CListBox *plbCCNum, CListBox *plbCCDetail, CListBox *plbUID )
{
	SQLRETURN	sReturn = 0;

	set<CString> s;
	set<CString>::iterator is;

	typedef pair<CString, CString> IIPAIR;	//Id & Ip pair

	multimap<CString, IIPAIR> mm;
	multimap<CString, IIPAIR>::iterator imm;

	set<CString> suid;				//UserID Set
	set<CString>::iterator isuid;

/*
	multiset<CString> ms;
	multiset<CString>::iterator ims;
*/

	CString item;	//Country Code

	const char *country;
	unsigned int cntNoIP = 0;
	unsigned int cntTotalIP = 0;	

	//UserDB
	ODBC_STMT* pConn = NULL;
	pConn = m_pUserDB->GetConnection();

	if (!pConn)
		return DB_ERROR;

	SQLCHAR		szIP[16]	= {0};  SQLINTEGER cbIP		= SQL_NTS;
	SQLCHAR		szID[24]	= {0};  SQLINTEGER cbID		= SQL_NTS;
	SQLINTEGER	nCnt		= 0;	SQLINTEGER cbCnt	= SQL_NTS;	

	TCHAR szQuery[1024] = {0};


	GeoIP * gi = NULL;
	gi = GeoIP_open("GeoIP.dat", GEOIP_STANDARD | GEOIP_CHECK_CACHE);

	if (gi == NULL) {
		PrintMsg(_T("ERROR: GeoIP database could not be opened."));		
		return FALSE;
	}

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");
		

	_snprintf( szQuery,1024, 
		_T(
		"SELECT LogIpAddress, UserUID "										\
		"FROM DaumLogLogin "												\
		"WHERE LogDate BETWEEN '%s' AND '%s' AND LogIpAddress IS NOT NULL "	\
		"GROUP BY LogIpAddress, UserUID "									\
		"ORDER BY UserUID "
		), csFrom.GetString(), csTo.GetString() );


	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szQuery, SQL_NTS);


	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		PrintMsg(_T("ERROR: SQLPage03_CC_Num&Detail"));
		CString err = GetErrorString(pConn->hStmt);
		PrintMsg(err);
		m_pUserDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR,	szIP,	16, &cbIP );
				::SQLGetData(pConn->hStmt, 2, SQL_C_CHAR,	szID,	24, &cbID );
				::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	&nCnt,	 0, &cbCnt);
			}
			else	//no more columns
			{				
				break;
			}

			if (_tcscmp((TCHAR *)szIP, _T("(null)")) == 0)
				continue;

			country = GeoIP_country_code3_by_addr(gi,(const char *)szIP);

			if (country == NULL)
			{
				cntNoIP++;
				continue;
			}

			cntTotalIP++;

			item.SetString(country);
			
			s.insert(item);
			
			CString strID(szID); 
			CString strIP(szIP);            


			IIPAIR IDIP(strID, strIP);	//ID & IP;			

			mm.insert(make_pair(item, IDIP));

			//CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %2s  %3d"), country, nCnt);
		}
	}

	
	//1.Process IP addresses and country codes.

	for (is = s.begin(); is != s.end(); ++is)
	{
		CString item = *is;

		size_t cnt = mm.count( item );

		double rate = (double)(cnt * 100) / (double)cntTotalIP;

		CConsoleMessage::GetInstance()->PrintTo(plbCCNum, _T(" %-4s\t%-4d\t%-.2lf%%"), item.GetString(), mm.count( item ), rate ); //country, nCnt);


		for (imm = mm.lower_bound(item); imm != mm.upper_bound(item); ++imm)
		{
			IIPAIR ii = (IIPAIR)imm->second;

			if (bExceptKOR)
			{
				if ( !item.CompareNoCase(_T("KOR")) )
				{
					continue;
				}
			}

			suid.insert(ii.first);

			CConsoleMessage::GetInstance()->PrintTo(plbCCDetail, _T(" %-4s\t%-16s\t%-16s"), item.GetString(), ii.first, ii.second );
		}
	}

	for (isuid = suid.begin(); isuid != suid.end(); ++isuid)
	{
        CString uid = *isuid;

		CConsoleMessage::GetInstance()->PrintTo(plbUID, _T(" %-16s"), uid.GetString() );
	}

	CConsoleMessage::GetInstance()->PrintTo(plbCCNum, _T("-------------"));
	CConsoleMessage::GetInstance()->PrintTo(plbCCNum, _T("Total\t%-4d"), cntTotalIP);
	CConsoleMessage::GetInstance()->PrintTo(plbCCNum, _T("NoCode\t%-4d"), cntNoIP);


	GeoIP_delete(gi);

	m_pUserDB->FreeConnection(pConn);
	

	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );

	return DB_OK;
}

//
//mjeon.Page04
//
int COdbcManager::SQLPage04_Daily_PlayTime( CTime &ctFrom2, CTime &ctTo2, CListBox *plbResult, CListBox *plbDetail )
{
	SQLRETURN	sReturn = 0;

	SQLCHAR		ChaName[MAX_CHANAME_LENGTH]	= {0};	SQLINTEGER cbChaName	= SQL_NTS;
	SQLCHAR		UserID[MAX_USERID_LENGTH]	= {0};	SQLINTEGER cbUserID		= SQL_NTS;
	SQLCHAR		IP[16]						= {0};  SQLINTEGER cbIP			= SQL_NTS;
	SQLCHAR		CountryWeb[64]				= {0};  SQLINTEGER cbCountryWeb	= SQL_NTS;	

	SQLINTEGER	ChaLevel		= 0;	SQLINTEGER cbChaLevel		= SQL_NTS;
	SQLINTEGER	GameTime		= 0;	SQLINTEGER cbGameTime		= SQL_NTS;	
	

	std::map<int, int> m;				//ChaNum Set
	std::map<int, int>::iterator im;


	enum {GT_30 = 0, GT_60, GT_90, GT_120, GT_150, GT_180, GT_210, GT_240, GT_270, GT_300, GT_330, GT_360, GT_390, GT_420, GT_450, GT_480, GT_510, GT_540, GT_570, GT_600, GT_630, GT_660, GT_690, GT_720, GT_MORE, GT_HOW_MANY};// GT_IDX;

	std::string strGT[GT_HOW_MANY];

	strGT[GT_30]	= "<30";
	strGT[GT_60]	= "<60";
	strGT[GT_90]	= "<90";
	strGT[GT_120]	= "<120";
	strGT[GT_150]	= "<150";
	strGT[GT_180]	= "<180";
	strGT[GT_210]	= "<210";
	strGT[GT_240]	= "<240";
	strGT[GT_270]	= "<270";
	strGT[GT_300]	= "<300";
	strGT[GT_330]	= "<330";
	strGT[GT_360]	= "<360";
	strGT[GT_390]	= "<390";
	strGT[GT_420]	= "<420";
	strGT[GT_450]	= "<450";
	strGT[GT_480]	= "<480";
	strGT[GT_510]	= "<510";
	strGT[GT_540]	= "<540";
	strGT[GT_570]	= "<570";
	strGT[GT_600]	= "<600";
	strGT[GT_630]	= "<630";
	strGT[GT_660]	= "<660";
	strGT[GT_690]	= "<690";
	strGT[GT_720]	= "<720";
	strGT[GT_MORE]	= ">=720";

	
	int GameTimeDistribution[GT_HOW_MANY];

	ZeroMemory(GameTimeDistribution, sizeof(GameTimeDistribution));

	//UserDB
	ODBC_STMT* pConnU = NULL;
	
	
	GeoIP * gi = NULL;
	gi = GeoIP_open("GeoIP.dat", GEOIP_STANDARD | GEOIP_CHECK_CACHE);

	if (gi == NULL) {
		PrintMsg(_T("ERROR: GeoIP database could not be opened."));		
		return FALSE;
	}
	
	const char *country = NULL;

	TCHAR szQuery[4096] = {0};


	CTime ctOrigin = ctFrom2;

	CTime ctFrom(ctOrigin.GetYear(), ctOrigin.GetMonth(), 
		ctOrigin.GetDay(), 0, 0, 0);

	CTime ctTo(ctOrigin.GetYear(), ctOrigin.GetMonth(), 
		ctOrigin.GetDay(), 23, 59, 59);


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	//
	// 1.Let's get the ChaNum and it's daily total GameTime.
	//


	//UserDB
	pConnU = NULL;
	pConnU = m_pUserDB->GetConnection();
	if (!pConnU)
		return DB_ERROR;


	_snprintf( szQuery,4096, 
		_T(
		"SELECT SUM(GameTime) as GameTime "												\
		"FROM (SELECT * FROM GSLogGameTime WHERE LogDate BETWEEN '%s' AND '%s') as TODAYDATA "	\
		"GROUP BY TODAYDATA.ChaNum "															\
		"ORDER BY GameTime DESC "
		), csFrom.GetString(), csTo.GetString() );


	sReturn = ::SQLExecDirect(pConnU->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		PrintMsg(_T("ERROR: SQLPage04_Daily_PlayTime"));

		CString tmp;
		PrintMsg( tmp = GetErrorString(pConnU->hStmt));
		
		m_pUserDB->FreeConnection(pConnU);
		return DB_ERROR;		
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConnU->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConnU->hStmt));
				
				m_pUserDB->FreeConnection(pConnU);
				return DB_ERROR;				
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				//::SQLGetData(pConnU->hStmt, 1, SQL_C_LONG,	&ChaNum,	0, &cbChaNum);
				::SQLGetData(pConnU->hStmt, 1, SQL_C_LONG,	&GameTime,	0, &cbGameTime );
			}
			else	//no more columns
			{
				break;
			}

			//m.insert(make_pair(ChaNum, GameTime));
			
			int idx = GameTime % 30;

			if (idx > GT_MORE)
				idx = GT_MORE;

			GameTimeDistribution[idx]++;
		}
	}

	m_pUserDB->FreeConnection(pConnU);


	

	for (int i=GT_30; i<GT_HOW_MANY; i++)
	{
		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("  %-s\t%-d"), strGT[i].c_str(), GameTimeDistribution[i]);		
	}


	//
	// 2. Get Detail
	//

	//UserDB
	pConnU = NULL;
	pConnU = m_pUserDB->GetConnection();
	if (!pConnU)
		return DB_ERROR;

	_snprintf( szQuery,4096, 
		_T(		
		"SELECT SUM(GameTime) AS GTime, UserID "													\
		",(SELECT ChaName FROM RanGameS1.dbo.ChaInfo WHERE ChaNum = GT.ChaNum) AS ChaName "			\
		",(SELECT ChaLevel FROM RanGameS1.dbo.ChaInfo WHERE ChaNum = GT.ChaNum) AS ChaLevel "		\
		",(SELECT TOP 1 LastUserIP FROM GSUserInfo WHERE UserNum = GT.UserNum) AS IP "				\
		",(SELECT UserCountry FROM RanWorldWeb.dbo.UserInfo WHERE UserNum = GT.UserNum) AS Country "\
		"FROM GSLogGameTime AS GT "																	\
		"WHERE LogDate BETWEEN '%s' AND '%s' AND ChaNum <> 0 "	\
		"GROUP BY ChaNum, UserNum, UserID "															\
		"ORDER BY GTime desc "
		), csFrom.GetString(), csTo.GetString() );

	sReturn = ::SQLExecDirect(pConnU->hStmt, (SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
		PrintMsg(_T("ERROR: SQLPage04_Daily_PlayTime_Detail #000"));

		CString tmp;
		PrintMsg( tmp = GetErrorString(pConnU->hStmt));
		m_pUserDB->FreeConnection(pConnU);

		return DB_ERROR;
	}
	else
	{
		PrintMsg( _T("OK: SQL Excuted Query successfuly.") );

		while(true)
		{
			sReturn = ::SQLFetch(pConnU->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				PrintMsg(szQuery);
				PrintMsg(GetErrorString(pConnU->hStmt));
				m_pUserDB->FreeConnection(pConnU);

				return DB_ERROR;
			}


			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				::SQLGetData(pConnU->hStmt, 1, SQL_C_LONG,	&GameTime,	0,					&cbGameTime );
				::SQLGetData(pConnU->hStmt, 2, SQL_C_CHAR,	UserID,		MAX_USERID_LENGTH,	&cbUserID );
				::SQLGetData(pConnU->hStmt, 3, SQL_C_CHAR,	ChaName,	MAX_CHANAME_LENGTH, &cbChaName);
				::SQLGetData(pConnU->hStmt, 4, SQL_C_LONG,	&ChaLevel,	0,					&cbChaLevel );
				::SQLGetData(pConnU->hStmt, 5, SQL_C_CHAR,	IP,			16,					&cbIP );
				::SQLGetData(pConnU->hStmt, 6, SQL_C_CHAR,	CountryWeb,	64,					&cbCountryWeb );				
			}
			else	//no more columns
			{
				break;
			}

			country = GeoIP_country_code3_by_addr(gi,(const char *)IP);		

			std::string CountryIP;

			if (country != NULL)
                CountryIP = country;
			else
				CountryIP = "";

			CConsoleMessage::GetInstance()->PrintTo(plbDetail, _T(" %-4d\t%-24s\t%+24s\t%-3d\t%-24s\t%-s"), GameTime, UserID, ChaName, ChaLevel, CountryWeb, CountryIP.c_str() );
		}
	}	

	
	PrintMsg( _T("OK: SQL Fetching has finished successfuly.") );


	GeoIP_delete(gi);

	m_pUserDB->FreeConnection(pConnU);

	return DB_OK;
}



