#pragma once
#include "afxcmn.h"


// CAmbientMNG ��ȭ �����Դϴ�.

class CAmbientMNG : public CPropertyPage
{
	DECLARE_DYNAMIC(CAmbientMNG)

protected:
	CsheetWithTabChar	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	CAmbientMNG();
	virtual ~CAmbientMNG();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_AMBIENT_MNG };
public:
	void UpdateList();

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSave();
	CListCtrl m_PieceList;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedAddambientButton();
	afx_msg void OnBnClickedButtonSave2();
	afx_msg void OnBnClickedAmbientDefault();
};
