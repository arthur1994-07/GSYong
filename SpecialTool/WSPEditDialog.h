#pragma once
#include "afxwin.h"


// CWSPEditDialog 대화 상자입니다.

class CWSPEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CWSPEditDialog)

public:
	CWSPEditDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWSPEditDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WSPEDIT };

	WORD m_ProductType;
	WORD m_wWs;
	WORD m_wWsp;
	WORD m_wWspPer;

	CListBox m_TypeList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeTypelist();
};
