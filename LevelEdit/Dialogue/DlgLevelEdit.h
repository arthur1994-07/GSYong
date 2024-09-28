#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CDlgLevelEdit 대화 상자입니다.
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
	CDlgLevelEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLevelEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LEVELEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
public:
	CComboBox m_comboPage;
	afx_msg void OnCbnSelchangeComboPage();
};
