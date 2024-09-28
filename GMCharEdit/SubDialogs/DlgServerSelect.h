#pragma once

#include "afxcmn.h"

#include <vector>
#include "../Database/DatabaseTable.h"

// CDlgServerSelect ��ȭ �����Դϴ�.

class CDlgServerSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgServerSelect)

public:
	CDlgServerSelect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgServerSelect();
	void InitListHead();
	void FillData();

	int m_nSelectedServer;
	std::vector<viewServerInfo> m_vServerList;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SERVER_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl m_ListServerSelect;
	virtual BOOL OnInitDialog();
};
