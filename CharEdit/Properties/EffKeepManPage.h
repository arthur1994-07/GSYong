#pragma once
#include "afxwin.h"

class CsheetWithTabChar;

// CEffKeepManPage ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFKEEP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
