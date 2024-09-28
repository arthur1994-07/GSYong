// MainPropertySheet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "RanStatisticsDlg.h"
#include "MainPropertySheet.h"
#include "ODBCManager.h"
#include "ADOManager.h"
#include "CCfg.h"


// CMainPropertySheet

IMPLEMENT_DYNAMIC(CMainPropertySheet, CPropertySheet)

CMainPropertySheet::CMainPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
						:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
						,m_pDlgRanStatistics(NULL)
						,m_pDBManager(NULL)
						,m_pCfg(NULL)
						,m_Page01(CPage01(this))
						,m_Page02(CPage02(this))
						,m_Page03(CPage03(this))
						,m_Page04(CPage04(this))
{
	AddPage(&m_Page01);
	AddPage(&m_Page02);
	AddPage(&m_Page03);
	AddPage(&m_Page04);
}

CMainPropertySheet::CMainPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
						:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
						,m_pDlgRanStatistics(NULL)
						,m_pDBManager(NULL)
						,m_pCfg(NULL)
						,m_Page01(CPage01(this))
						,m_Page02(CPage02(this))
						,m_Page03(CPage03(this))
						,m_Page04(CPage04(this))
{
	AddPage(&m_Page01);
	AddPage(&m_Page02);
	AddPage(&m_Page03);
	AddPage(&m_Page04);
}

CMainPropertySheet::~CMainPropertySheet()
{
	if ( m_pCfg->useODBC() )
	{
		COdbcManager::ReleaseInstance();		
	}
	else
	{
		CADOManager::ReleaseInstance();		
	}

	//
	// Configuration
	//	
	CCfg::ReleaseInstance();
}


void CMainPropertySheet::SetMainDlg( CDialog *p )
{
	m_pDlgRanStatistics = (CRanStatisticsDlg *)p;
}

void CMainPropertySheet::EnablePage (int nPage, BOOL bEnable)
{
	// if we want to enable the page
	if (bEnable)
	{
		// remove the index from the map
		m_DisabledPages.RemoveKey (nPage);
		// take out " - Disabled" from tab label
		CTabCtrl* pTab = GetTabControl();
		ASSERT (pTab);
		TC_ITEM ti;

		TCHAR szText[100];

		ti.mask = TCIF_TEXT;
		ti.pszText = szText;
		ti.cchTextMax = 100;
		VERIFY (pTab->GetItem (nPage, &ti));
		TCHAR * pFound = strstr (szText, " - Disabled");
		
		if (pFound)
		{
			*pFound = '\0';
			VERIFY (pTab->SetItem (nPage, &ti));
		}
	}
	// if we want to disable it
	else
	{
		// add the index to the map
		m_DisabledPages.SetAt (nPage, nPage);
		// add " - Disabled" to tab label
		CTabCtrl* pTab = GetTabControl();
		ASSERT (pTab);
		TC_ITEM ti;
		TCHAR szText[100];
		ti.mask = TCIF_TEXT;
		ti.pszText = szText;
		ti.cchTextMax = 100;
		VERIFY (pTab->GetItem (nPage, &ti));
		_tcscat (szText, _T(" (Locked)"));
		VERIFY (pTab->SetItem (nPage, &ti));
	}
}


BEGIN_MESSAGE_MAP(CMainPropertySheet, CPropertySheet)
END_MESSAGE_MAP()


// CMainPropertySheet 메시지 처리기입니다.

BOOL CMainPropertySheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	//
	// Following steps use ListBox(UI).
	// Therefore, they MUST be placed after UI was created.
	//
	m_pCfg = CCfg::GetInstance();

	if ( m_pCfg->useODBC() )
	{
		m_pDBManager = COdbcManager::GetInstance();

		m_bODBCReady = StartODBCManager();

		if ( !m_bODBCReady )
		{
			CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR: DBManager couldn't be started."));
			CConsoleMessage::GetInstance()->PrintMsg(_T(" -> Check DB.cfg and restart RanStatistics."));

			return FALSE;
		}

		CConsoleMessage::GetInstance()->PrintMsg(_T("ODBC: DBManager is ready."));		
	}
	else
	{
		m_pDBManager = CADOManager::GetInstance();

		CConsoleMessage::GetInstance()->PrintMsg(_T("ADO: DBManager is ready."));		
	}	

	return bResult;
}


BOOL CMainPropertySheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	NMHDR* pnmh = (NMHDR*)lParam;

	// tab is about to change
	if (TCN_SELCHANGING == pnmh->code)
	{
		// save the current page index		
		
		m_nLastActive = GetActiveIndex ();
	}
	else if (TCN_SELCHANGE == pnmh->code)	// tab has been changed
	{
		// get the current page index
	
		int nCurrentPage = GetActiveIndex ();		
		
		// if current page is in our map of disabled pages	
		if (m_DisabledPages.Lookup (nCurrentPage, nCurrentPage))
		{
			PostMessage (PSM_SETCURSEL, m_nLastActive); // activate the previous page			
		}		
	}

	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}



BOOL CMainPropertySheet::StartODBCManager()
{
	int nRetCode = DB_ERROR;

	CCfg *pcfg = m_pCfg;

	
	//-------------------------- Configuration DB --------------------------

	//Open UserDB
	nRetCode = COdbcManager::GetInstance()->OpenUserDB( pcfg->GetUserOdbcName(),
														pcfg->GetUserOdbcUser(),
														pcfg->GetUserOdbcPass(),
														pcfg->GetUserOdbcDatabase(),
														pcfg->GetUserOdbcPoolSize(),
														pcfg->GetUserOdbcResponseTime() );

	if ( nRetCode == DB_ERROR )
	{		
		CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR:UserDB could not been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
		
		return FALSE;
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: UserDB has been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
	}

    
	//Open GameDB
	nRetCode = COdbcManager::GetInstance()->OpenGameDB( pcfg->GetGameOdbcName(),
														pcfg->GetGameOdbcUser(),
														pcfg->GetGameOdbcPass(),
														pcfg->GetGameOdbcDatabase(),
														pcfg->GetGameOdbcPoolSize(),
														pcfg->GetGameOdbcResponseTime() );

	if ( nRetCode == DB_ERROR )
	{		
		CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR:GameDB could not been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );

		return FALSE;
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: GameDB has been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
	}


	//Open WebDB
	nRetCode = COdbcManager::GetInstance()->OpenWebDB( pcfg->GetWebOdbcName(),
		pcfg->GetWebOdbcUser(),
		pcfg->GetWebOdbcPass(),
		pcfg->GetWebOdbcDatabase(),
		pcfg->GetWebOdbcPoolSize(),
		pcfg->GetWebOdbcResponseTime() );

	if ( nRetCode == DB_ERROR )
	{		
		CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR:WebDB could not been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );

		return FALSE;
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: WebDB has been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
	}


	//Open LogDB
	nRetCode = COdbcManager::GetInstance()->OpenLogDB(  pcfg->GetLogOdbcName(),
														pcfg->GetLogOdbcUser(),
														pcfg->GetLogOdbcPass(),
														pcfg->GetLogOdbcDatabase(),
														pcfg->GetLogOdbcPoolSize(),
														pcfg->GetLogOdbcResponseTime() );

	if ( nRetCode == DB_ERROR )
	{		
		CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR:LogDB could not been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );

		return FALSE;
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: LogDB has been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
	}


	//Open ShopDB
	nRetCode = COdbcManager::GetInstance()->OpenShopDB( pcfg->GetShopOdbcName(),
														pcfg->GetShopOdbcUser(),
														pcfg->GetShopOdbcPass(),
														pcfg->GetShopOdbcDatabase(),
														pcfg->GetShopOdbcPoolSize(),
														pcfg->GetShopOdbcResponseTime() );
	
	if ( nRetCode == DB_ERROR )
	{		
		CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR:ShopDB could not been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );

		return FALSE;
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintMsg(_T("OK: ShopDB has been opened."));
		CConsoleMessage::GetInstance()->PrintMsg( _T(" ") );
	}


	return TRUE;
}

BOOL CMainPropertySheet::IsExported()
{
	return m_pDlgRanStatistics->IsExported();
}

CTime& CMainPropertySheet::GetFrom()
{
	return m_pDlgRanStatistics->GetFrom();
}

CTime& CMainPropertySheet::GetTo()
{
	return m_pDlgRanStatistics->GetTo();
}

CDBManager *CMainPropertySheet::GetDBManager()
{
	if (m_pDBManager == NULL)
	{
		CConsoleMessage::GetInstance()->PrintMsg( _T("ERROR: DBManager is NULL.") );
		return NULL;
	}

	if ( m_pCfg->useODBC() )
	{
		if ( !IsODBCReady() )
		{
			CConsoleMessage::GetInstance()->PrintMsg( _T("ERROR: ODBC is not Ready!") );

			return NULL;
		}
	}

	return m_pDBManager;
}
