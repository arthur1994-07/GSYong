#pragma once

#include "../EngineLib/G-Logic/GLDefine.h"


// CItemDialog ��ȭ �����Դϴ�.

class CItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemDialog)

public:
	CItemDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CItemDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	SNATIVEID	m_nidITEM;

public:
	virtual BOOL OnInitDialog();
	CListBox m_cItemList;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonFind();
};
