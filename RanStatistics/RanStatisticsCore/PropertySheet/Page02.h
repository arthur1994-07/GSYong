#pragma once
#include "afxwin.h"

class CMainPropertySheet;
class CDBManager;

// CPage02 ��ȭ �����Դϴ�.

class CPage02 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage02)

public:
	CPage02( CMainPropertySheet *p );
	virtual ~CPage02();

	BOOL IsODBCReady();
	BOOL IsExported();

	CTime& GetFrom();
	CTime& GetTo();

	CDBManager *GetDBManager();

	enum { IDD = IDD_PAGE02 };

protected:
	CMainPropertySheet *m_pPropertySheet;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChildActivate();
	afx_msg void OnBnClickedButtonProcess02();	

	CListBox m_lbMsg;
	CListBox m_lbAnalaysisGameTime;
	CListBox m_lbConcurrent;
	CListBox m_lbChaLevel;
	CListBox m_lbIP2Nation;
};
