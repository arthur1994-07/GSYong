#pragma once
#include "stdafx.h"
#include "Page01.h"
#include "Page02.h"
#include "Page03.h"
#include "Page04.h"

class CDBManager;
class CCfg;
class CRanStatisticsDlg;

// CMainPropertySheet

class CMainPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMainPropertySheet)

public:
	CMainPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMainPropertySheet();

	void	EnablePage (int nPage, BOOL bEnable);	
	void	SetMainDlg( CDialog *p );

	BOOL	StartODBCManager();
	BOOL	IsODBCReady() { return m_bODBCReady; }
	BOOL	IsExported();

	CDBManager *GetDBManager();

	CTime&	GetFrom();
	CTime&	GetTo();

	CPage01 m_Page01;
	CPage02 m_Page02;
	CPage03 m_Page03;
	CPage04 m_Page04;
	
	// Follwings are NOT used now.
	int m_nLastActive;								// Save the current page in TCN_SELCHANGING	
	CMap <int, int&, int, int&> m_DisabledPages;	// list of indexes of disabled pages

protected:

	CRanStatisticsDlg	*m_pDlgRanStatistics;

	CDBManager	*m_pDBManager;
	CCfg		*m_pCfg;


	BOOL	m_bODBCReady;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL OnInitDialog();
};


