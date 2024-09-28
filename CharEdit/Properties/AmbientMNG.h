#pragma once
#include "afxcmn.h"


// CAmbientMNG 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_AMBIENT_MNG };
public:
	void UpdateList();

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
