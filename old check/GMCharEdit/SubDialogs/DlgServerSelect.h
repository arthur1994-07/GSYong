#pragma once

#include "afxcmn.h"

#include <vector>
#include "../Database/DatabaseTable.h"

// CDlgServerSelect 대화 상자입니다.

class CDlgServerSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgServerSelect)

public:
	CDlgServerSelect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgServerSelect();
	void InitListHead();
	void FillData();

	int m_nSelectedServer;
	std::vector<viewServerInfo> m_vServerList;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SERVER_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl m_ListServerSelect;
	virtual BOOL OnInitDialog();
};
