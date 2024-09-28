#pragma once

#include "resource.h"
#include "afxwin.h"

class CMainPropertySheet;
class CDBManager;

// CPage01 ��ȭ �����Դϴ�.

class CPage01 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage01)

public:
	CPage01( CMainPropertySheet *p );
	virtual ~CPage01();

	BOOL IsODBCReady();
	BOOL IsExported();

	CTime& GetFrom();
	CTime& GetTo();

	CDBManager *GetDBManager();
	
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PAGE01 };

protected:
	CMainPropertySheet *m_pPropertySheet;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChildActivate();
	afx_msg void OnBnClickedButtonProcess();
		
	CListBox m_lbMsg;
	CListBox m_lbNewUser;	
	CListBox m_lbNewChar;
	CListBox m_lbMonthlyUser;
	CListBox m_lbMonthlyShopIncome;
};
