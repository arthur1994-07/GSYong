#pragma once
#include "afxwin.h"

#include "../../EngineLib/G-Logic/GLDefine.h"

// CDlgNPC ��ȭ �����Դϴ�.

class CDlgNPC : public CDialog
{
	DECLARE_DYNAMIC(CDlgNPC)

public:
	bool		m_bMob;
	SNATIVEID	m_nidNPC;

public:
	CDlgNPC(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgNPC();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_NPC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    void SelectItem( SNATIVEID sID );
	CListBox m_listNPC;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonFind();
};
