//
//mjeon.ado
// 2010.12.29
//

#include "stdafx.h"
#include "ADOClass.hpp"
#include "ADOManager.h"
#include "GeoIP.h"
#include "Export2File.h"
#include <set>
#include <map>

using namespace std;



int CADOManager::SQLPage01_NewUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csCreateDate;
	int		nCnt = 0;

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	csQuery.Empty();

	csQuery.Format(
		_T(
		"SELECT Convert(varchar(10), CreateDate, 21) AS CreateDate, Count(*) AS Cnt "	\
		"FROM GSUserInfo "																\
		"WHERE CreateDate BETWEEN '%s' AND '%s' "										\
		"GROUP BY Convert(varchar(10), CreateDate, 21) "								\
		"ORDER BY CreateDate ASC"
		), csFrom.GetString(), csTo.GetString() );


	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;
		
	do
	{
		ado.GetCollect(0, csCreateDate);
		ado.GetCollect(1, nCnt);

		m_pConsole->PrintTo(plbResult, _T(" %-s\t%-d"), csCreateDate.GetBuffer(), nCnt);	
	}
	while ( ado.Next() );
	

	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );

	return DB_OK;
}


int CADOManager::SQLPage01_NewChar( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csCreateDate;
	int		nCnt = 0;

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	csQuery.Empty();

	csQuery.Format(
		_T(
		"SELECT Convert(varchar(10), ChaCreateDate, 21) AS CreateDate, Count(*) AS Cnt "	\
		"FROM ChaInfo "																		\
		"WHERE ChaCreateDate BETWEEN '%s' AND '%s' "										\
		"GROUP BY Convert(varchar(10), ChaCreateDate, 21) "									\
		"ORDER BY CreateDate ASC"
		), csFrom.GetString(), csTo.GetString() );


	CjADO ado(ADO_CONN_STR_GAMEDB, PrintMsg);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;

	do
	{
		ado.GetCollect(0, csCreateDate);
		ado.GetCollect(1, nCnt);

		m_pConsole->PrintTo(plbResult, _T(" %-s\t%-d"), csCreateDate.GetBuffer(), nCnt);

	}
	while ( ado.Next() );


	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );

	return DB_OK;
}


int CADOManager::SQLPage01_MonthlyUser( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;
	CString csLogDate;
	int		nCnt = 0;

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	int nYearFrom = ctFrom.GetYear();
	int nYearTo = ctTo.GetYear();

	TCHAR buffer[12] = {0,};

	struct PAIRDATE {
		CString csFrom;
		CString csTo;
	};

	int cnt = nYearTo - nYearFrom + 1;
	int idx = 0;

	// Storage for 20-years Maximum
	PAIRDATE pd[20];	//1-Year per 1-PAIRDATE

	if ( nYearFrom < nYearTo )
	{
		// First Date
		//From		
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
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}

	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

	for ( int i=0; i<cnt; i++ )
	{
		csQuery.Empty();

		csQuery.Format(
			_T(
			"SELECT DATEPART(mm, LogDate) As LDate, COUNT(DISTINCT UserID) "	\
			"FROM GSLogGameTime "												\
			"WHERE LogDate BETWEEN '%s' AND '%s' AND GameTime > 0 "				\
			"GROUP BY DATEPART(mm, LogDate) "									\
			"ORDER BY DATEPART(mm, LogDate) "						
			), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		//CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

		if ( !ado.Execute(csQuery) )
			return DB_ERROR;

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

		do
		{
			ado.GetCollect(0, csLogDate);
			ado.GetCollect(1, nCnt);

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-d"), csLogDate.GetBuffer(), nCnt);
		}
		while ( ado.Next() );

				
		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));

		
		PrintMsg( _T("OK: Done a Query.") );
		PrintMsg( _T("") );
	}	

	return DB_OK;
}


int CADOManager::SQLPage01_MonthlyShopIncome( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;
	CString csLogDate;
	int		nCnt = 0;

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
		pd[cnt-1].csTo = csTo;


	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].csFrom	= csFrom;
		pd[0].csTo		= csTo;
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}

	CjADO ado(ADO_CONN_STR_SHOPDB, PrintMsg);

	for ( int i=0; i<cnt; i++ )
	{
		csQuery.Empty();

		csQuery.Format(
			_T(
			"SELECT DATEPART(mm, PurDate) AS Month, SUM(PurPrice) AS Price "	\
			"FROM ShopPurchase "												\
			"WHERE PurDate BETWEEN '%s' AND '%s' "								\
			"GROUP BY DATEPART(mm, PurDate)"									\
			"ORDER BY DATEPART(mm, PurDate)"
			), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		
		//CjADO ado(ADO_CONN_STR_SHOPDB, PrintMsg);

		if ( !ado.Execute(csQuery) )
			return DB_ERROR;

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

		do
		{
			ado.GetCollect(0, csLogDate);
			ado.GetCollect(1, nCnt);

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-d"), csLogDate.GetBuffer(), nCnt/10);
		}
		while ( ado.Next() );

		
		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));

		PrintMsg( _T("OK: Done a Query.") );
		PrintMsg( _T("") );
	}

	return DB_OK;
}


int CADOManager::SQLPage02_AnaysisGameTime( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csLogDate;
	UINT	cntTotalUsers	= 0;
	UINT	cntPlayedUsers	= 0;
	UINT	cntTotalGameTime= 0;

	
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
		pd[cntYear-1].csTo = csTo;


	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].csFrom	= csFrom;
		pd[0].csTo		= csTo;		
	}
	else	//nYearFrom > nYearTo
	{		
		return DB_ERROR;
	}

	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

	for ( int i=0; i<cntYear; i++ )
	{
		csQuery.Empty();

		csQuery.Format(
			_T(
			"SELECT DATEPART(mm, LogDate) As Date, MAX(UserNum) AS TotalUser, "				\
			"COUNT(DISTINCT UserID) AS PlayedUsers, SUM(GameTime) AS TotalGameTime "		\
			"FROM GSLogGameTime "															\
			"WHERE LogDate between '%s' and '%s' and GameTime > 0 "							\
			"GROUP By DATEPART(mm, LogDate) "												\
			"ORDER BY DATEPART(mm, LogDate) "												\
			), pd[i].csFrom.GetString(), pd[i].csTo.GetString() );


		//CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

		if ( !ado.Execute(csQuery) )
			return DB_ERROR;

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d ]"), nYearFrom + i);

		do
		{
			ado.GetCollect(0, csLogDate);
			ado.GetCollect(1, cntTotalUsers);
			ado.GetCollect(2, cntPlayedUsers);
			ado.GetCollect(3, cntTotalGameTime);

			// $1 = 10 points.
			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%0.5d\t%0.5d\t%0.5d\t%0.1f\t%0.1f"), 
				csLogDate.GetBuffer(), cntTotalUsers, cntPlayedUsers, 
				cntTotalGameTime/60, (float)cntTotalGameTime/60/cntTotalUsers,
				(float)cntTotalGameTime/60/cntPlayedUsers);
		}
		while ( ado.Next() );


		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));

		PrintMsg( _T("OK: Done a Query.") );
		PrintMsg( _T("") );
	}	

	return DB_OK;
}


int CADOManager::SQLPage02_ConcurrentUsers( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csLogDate;
	UINT cntAvgCU	= 0;
	UINT cntTotalCU = 0;

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
		pd[cntYear-1].ctTo = ctTo;
	}
	else if ( nYearFrom == nYearTo )
	{
		pd[0].ctFrom	= ctFrom;
		pd[0].ctTo		= ctTo;
	}
	else	//nYearFrom > nYearTo
	{
		return DB_ERROR;
	}


	CString csBaseWeekNum;
	int sumTotalCU = 0, sumAvgCu = 0, cntWeeks = 0;

	CjADO ado(ADO_CONN_STR_LOGDB, PrintMsg);

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

			csQuery.Empty();

			csQuery.Format(
				_T(
				"SELECT DATEPART(ww, LogDate) - DATEPART(ww, '%s') + 1 AS Date, AVG(UserNum) AS AVG, MAX(UserNum) AS MAX "	\
				"FROM LogServerState "																\
				"WHERE LogDate between '%s' and '%s' "												\
				"GROUP By DATEPART(ww, LogDate) "													\
				"ORDER BY DATEPART(ww, LogDate) "
				), csBaseWeekNum.GetString(), csFrom.GetString(), csTo.GetString() );


			//CjADO ado(ADO_CONN_STR_LOGDB, PrintMsg);

			if ( !ado.Execute(csQuery) )
				return DB_ERROR;

			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("[ %4d/%0.2d ]"), nYearFrom + i, j);
			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("----------------------") );

			do
			{
				ado.GetCollect(0, csLogDate);
				ado.GetCollect(1, cntAvgCU);
				ado.GetCollect(2, cntTotalCU);
				

				// $1 = 10 points.
				CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %02s\t%-3d\t%-3d"), 
					csLogDate.GetBuffer(), cntAvgCU, cntTotalCU );

				sumTotalCU += cntTotalCU;
				sumAvgCu += cntAvgCU;
				cntWeeks++;
			}
			while ( ado.Next() );
			

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


			CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" "));

			PrintMsg( _T("OK: Done a Query.") );
			PrintMsg( _T("") );
		}	//for j
	}

	return DB_OK;
}


int CADOManager::SQLPage02_ChaLevel( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csLv;
	UINT	nCnt = 0;
	
	int sum = 0;
	int lvPrevRange = 0; // Level Range starts from 0.

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	csQuery.Empty();

	csQuery.Format(
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


	CjADO ado(ADO_CONN_STR_GAMEDB, PrintMsg);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;	

	do
	{
		ado.GetCollect(0, csLv);
		ado.GetCollect(1, nCnt);
		

		int nLv = _ttoi((LPCTSTR)csLv.GetBuffer());

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

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-3s\t%-d"), csLv.GetBuffer(), nCnt);
	}
	while ( ado.Next() );


	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Total\t%-d"), sum);
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("-------------") );
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" ") );


	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );


	return DB_OK;
}


int CADOManager::SQLPage02_IP2Nation( CTime &ctFrom, CTime &ctTo, CListBox *plbResult )
{
	CString csQuery;

	CString csIP;
	UINT	nCnt = 0;

	multiset<CString> ms;
	multiset<CString>::iterator ims;	
	set<CString> s;
	set<CString>::iterator is;	
	CString item;

	const char *country;
	unsigned int cntNoIP = 0;
	unsigned int cntTotalIP = 0;	

	GeoIP * gi = NULL;
	gi = GeoIP_open("GeoIP.dat", GEOIP_STANDARD | GEOIP_CHECK_CACHE);

	if (gi == NULL) {
		PrintMsg(_T("ERROR: GeoIP database could not be opened."));		
		return FALSE;
	}


	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");

	csQuery.Empty();

	csQuery.Format(
		_T(
		"SELECT LastUserIP, COUNT(LastUserIP) AS Cnt "	\
		"FROM GSUserInfo "								\
		"WHERE LastLoginDate BETWEEN '%s' and '%s' "	\
		"GROUP BY LastUserIP "							\
		"ORDER BY LastUserIP "
		), csFrom.GetString(), csTo.GetString() );


	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;	

	do
	{
		ado.GetCollect(0, csIP);
		ado.GetCollect(1, nCnt);


		if ( csIP.Compare(_T("(null)")) == 0 )
			continue;

		country = GeoIP_country_code3_by_addr(gi,(const char *)csIP.GetBuffer() );

		if (country == NULL)
		{
			cntNoIP++;
			continue;
		}

		cntTotalIP++;

		item.SetString(country);

		s.insert(item);
		ms.insert(item);		
	}
	while ( ado.Next() );

	
	for (is = s.begin(); is != s.end(); ++is)
	{
		CString item = *is;		

		CConsoleMessage::GetInstance()->PrintTo(plbResult, _T(" %-4s\t%-4d"), item.GetString(), ms.count( item ) ); //country, nCnt);
	}

	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("-------------"));
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("Total\t%-4d"), cntTotalIP);
	CConsoleMessage::GetInstance()->PrintTo(plbResult, _T("No IP\t%-4d"), cntNoIP);

	GeoIP_delete(gi);

	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );

	return DB_OK;
}


int CADOManager::SQLPage03_CC_Num( CTime &ctFrom, CTime &ctTo, BOOL bExceptKOR, CListBox *plbCCNum, CListBox *plbCCDetail, CListBox *plbUID )
{
	CString csQuery;

	CString csIP;
	CString csID;
	UINT	nCnt;

	set<CString> s;
	set<CString>::iterator is;

	typedef pair<CString, CString> IIPAIR;	//Id & Ip pair

	multimap<CString, IIPAIR> mm;
	multimap<CString, IIPAIR>::iterator imm;

	set<CString> suid;				//UserID Set
	set<CString>::iterator isuid;


	CString item;	//Country Code

	const char *country;
	unsigned int cntNoIP = 0;
	unsigned int cntTotalIP = 0;	


	GeoIP * gi = NULL;
	gi = GeoIP_open("GeoIP.dat", GEOIP_STANDARD | GEOIP_CHECK_CACHE);

	if (gi == NULL) {
		PrintMsg(_T("ERROR: GeoIP database could not be opened."));		
		return FALSE;
	}

	CString csFrom	= ctFrom.Format("%Y-%m-%d %H:%M:%S");
	CString csTo	= ctTo.Format("%Y-%m-%d %H:%M:%S");
		

	csQuery.Format(
		_T(
		"SELECT LogIpAddress, UserUID "										\
		"FROM DaumLogLogin "												\
		"WHERE LogDate BETWEEN '%s' AND '%s' AND LogIpAddress IS NOT NULL "	\
		"GROUP BY LogIpAddress, UserUID "									\
		"ORDER BY UserUID "
		), csFrom.GetString(), csTo.GetString() );


	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;	

	do
	{
		ado.GetCollect(0, csIP);
		ado.GetCollect(1, csID);
		ado.GetCollect(2, nCnt);


		if ( csIP.Compare(_T("(null)")) == 0 )
			continue;

		country = GeoIP_country_code3_by_addr(gi,(const char *)csIP.GetBuffer());

		if (country == NULL)
		{
			cntNoIP++;
			continue;
		}

		cntTotalIP++;

		item.SetString(country);

		s.insert(item);

		IIPAIR IDIP(csID, csIP);	//ID & IP;			

		mm.insert(make_pair(item, IDIP));		
	}
	while ( ado.Next() );



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

	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );


	return DB_OK;
}


int CADOManager::SQLPage04_Daily_PlayTime( CTime &ctFrom2, CTime &ctTo2, CListBox *plbResult, CListBox *plbDetail )
{
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
	//ODBC_STMT* pConnU = NULL;


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

	
	
	//CConsoleMessage *pConsole = CConsoleMessage::GetInstance();


	int			nGameTime = 0;
	int			nChaLevel = 0;

	CString		csUserID;
	CString		csChaName;
	CString		csIP;
	CString		csCountryWeb;


	//
	// 1.Let's get the ChaNum and it's daily total GameTime.
	//

	CString csQuery;

	csQuery.Format(		
		_T(
		"SELECT SUM(GameTime) as GameTime "														\
		"FROM (SELECT * FROM GSLogGameTime WHERE LogDate BETWEEN '%s' AND '%s') as TODAYDATA "	\
		"GROUP BY TODAYDATA.ChaNum "															\
		"ORDER BY GameTime DESC "
		), csFrom.GetString(), csTo.GetString() );


	CjADO ado(ADO_CONN_STR_USERDB, PrintMsg);
	
	if ( !ado.Execute(csQuery) )
		return DB_ERROR;	

	//while ( !ado.GetEOF() )
	do
	{
		//ado.GetCollect(_T("GameTime"), nGameTime);
		ado.GetCollect(0, nGameTime);

		int idx = nGameTime / 30;

		if (idx > GT_MORE)
			idx = GT_MORE;

		GameTimeDistribution[idx]++;

		//ado.MoveNext();
	}
	while ( ado.Next() );


	for (int i=GT_30; i<GT_HOW_MANY; i++)
	{
		m_pConsole->PrintTo(plbResult, _T("  %-s\t%-d"), strGT[i].c_str(), GameTimeDistribution[i]);		
	}
	

	
	//
	// 2. Get Detail
	//

	csQuery.Empty();
	
	csQuery.Format(
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


	if ( !ado.Execute(csQuery) )
		return DB_ERROR;

	do
	{
		ado.GetCollect(0,	nGameTime);
		ado.GetCollect(1,	csUserID);
		ado.GetCollect(2,	csChaName);
		ado.GetCollect(3,	nChaLevel);
		ado.GetCollect(4,	csIP);
		ado.GetCollect(5,	csCountryWeb);
		
		country = GeoIP_country_code3_by_addr( gi,(const char *) csIP.GetBuffer() );

		std::string CountryIP;

		if (country != NULL)
			CountryIP = country;
		else
			CountryIP = "";

		m_pConsole->PrintTo(plbDetail, _T(" %-4d\t%-24s\t%+24s\t%-3d\t%-24s\t%-s"), nGameTime, csUserID.GetBuffer(), csChaName.GetBuffer(), nChaLevel, csCountryWeb.GetBuffer(), CountryIP.c_str() );		
	}
	while ( ado.Next() );
	

	GeoIP_delete(gi);

	PrintMsg( _T("OK: Done a Query.") );
	PrintMsg( _T("") );

	return DB_OK;
}
