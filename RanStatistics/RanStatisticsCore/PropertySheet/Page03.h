#pragma once
#include "afxwin.h"

class CMainPropertySheet;
class CDBManager;

// CPage03 대화 상자입니다.

class CPage03 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage03)

public:
	CPage03( CMainPropertySheet *p );
	virtual ~CPage03();

	BOOL IsODBCReady();
	BOOL IsExported();
	BOOL IsExceptKOR();

	CTime& GetFrom();
	CTime& GetTo();

	CDBManager *GetDBManager();

	enum { IDD = IDD_PAGE03 };

protected:
	CMainPropertySheet *m_pPropertySheet;	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChildActivate();
	afx_msg void OnBnClickedButtonProcess03();
		
	CListBox m_lbMsg;
	CListBox m_lbCCNum;
	CListBox m_lbCCDetail;
	CButton m_cbExceptKOR;
	CListBox m_lbUID;
	BOOL m_bExceptKOR;
};
