#pragma once

#include "afxwin.h"

#include "../EngineLib/G-Logic/GLDefine.h"


// CNpcDialog ��ȭ �����Դϴ�.

class CNpcDialog : public CDialog
{
	DECLARE_DYNAMIC(CNpcDialog)

public:
	CNpcDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNpcDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NPCDIALOG };

public:
	SNATIVEID	m_sNpcID;
	bool		m_bMob;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cNpcList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedOk();
};
