#pragma once

#include "afxwin.h"

#include "../EngineLib/G-Logic/GLDefine.h"


// CNpcDialog 대화 상자입니다.

class CNpcDialog : public CDialog
{
	DECLARE_DYNAMIC(CNpcDialog)

public:
	CNpcDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNpcDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCDIALOG };

public:
	SNATIVEID	m_sNpcID;
	bool		m_bMob;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cNpcList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedOk();
};
