#pragma once

#include "resource.h"
#include "afxwin.h"

class CMainPropertySheet;
class CDBManager;

// CPage04 대화 상자입니다.

class CPage04 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage04)

public:
	CPage04( CMainPropertySheet *p );
	virtual ~CPage04();

	BOOL IsODBCReady();
	BOOL IsExported();

	CTime& GetFrom();
	CTime& GetTo();

	CDBManager *GetDBManager();

	enum { IDD = IDD_PAGE04 };

protected:
	CMainPropertySheet *m_pPropertySheet;	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChildActivate();
	afx_msg void OnBnClickedButtonProcess04();
	
	CListBox m_lbMsg;
	CListBox m_lbDailyPlayTime;
	CListBox m_lbDailyPlayTimeDetail;
};
