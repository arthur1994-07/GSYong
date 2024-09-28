#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CDlgLevelEdit ��ȭ �����Դϴ�.
class CDlgLevelEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgLevelEdit)

public:
	CsheetWithTab	*m_pSheetTab;

	BOOL			m_bINIT;

public:
	void UpdatePage ( BOOL bMainPage=FALSE );

	void UpdateKeyBoardMouse();

public:
	CDlgLevelEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgLevelEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LEVELEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
public:
	CComboBox m_comboPage;
	afx_msg void OnCbnSelchangeComboPage();
};
