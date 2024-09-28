#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

// CGateListPage 대화 상자입니다.
class	CsheetWithTab;
class CGateListPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGateListPage)

public:
	CGateListPage();
	virtual ~CGateListPage();

// 대화 상자 데이터입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listboxGate;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
