#pragma once
#include "afxwin.h"


// CWSPEditDialog ��ȭ �����Դϴ�.

class CWSPEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CWSPEditDialog)

public:
	CWSPEditDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CWSPEditDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WSPEDIT };

	WORD m_ProductType;
	WORD m_wWs;
	WORD m_wWsp;
	WORD m_wWspPer;

	CListBox m_TypeList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeTypelist();
};
