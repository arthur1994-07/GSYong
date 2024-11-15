#pragma once

#include "../EngineLib/G-Logic/GLDefine.h"


// CItemDialog 대화 상자입니다.

class CItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemDialog)

public:
	CItemDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	SNATIVEID	m_nidITEM;

public:
	virtual BOOL OnInitDialog();
	CListBox m_cItemList;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonFind();
};
