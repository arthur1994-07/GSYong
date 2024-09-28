#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

// CGateListPage ��ȭ �����Դϴ�.
class	CsheetWithTab;
class CGateListPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGateListPage)

public:
	CGateListPage();
	virtual ~CGateListPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_GATE_LIST };

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;	

public:
	BOOL	UpdateTreeItem ();

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetPropertyData ( int CalledPage, DWORD dwGateID );
	void	CleanAllItem ();

protected:
	BOOL		m_bGateIDInit;
	int			m_CalledPage;	
	DWORD		m_dwGateID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listboxGate;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
