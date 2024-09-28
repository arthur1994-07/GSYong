#pragma once

#include <vector>
#include <strstream>

#include "../Util/CGmCfg.h"
#include "../Util/CConsoleMessage.h"
#include "../Database/DatabaseTable.h"
#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "afxcmn.h"

// CClubSearch 폼 뷰입니다.

class CClubSearch : public CFormView
{
	DECLARE_DYNCREATE(CClubSearch)

protected:
	CClubSearch();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CClubSearch();

public:
	enum { IDD = IDD_CLUBSEARCH };
	BOOL m_bInit;

public:
	void InitListHead();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strClubName;
	CListCtrl m_ListClubSearch;
	afx_msg void OnBnClickedButtonClubSearch();
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


