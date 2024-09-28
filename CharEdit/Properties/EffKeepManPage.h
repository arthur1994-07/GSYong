#pragma once
#include "afxwin.h"

class CsheetWithTabChar;

// CEffKeepManPage 대화 상자입니다.

class CEffKeepManPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffKeepManPage)

protected:
	CsheetWithTabChar	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	void EffectListting ();

public:
	CEffKeepManPage();
	virtual ~CEffKeepManPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFKEEP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonEffState();
	afx_msg void OnBnClickedButtonEffDelete();
	afx_msg void OnBnClickedButtonEffInsert();
	CComboBox m_EffType;
	CListBox m_EffList;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnLbnDblclkListEff();
};
