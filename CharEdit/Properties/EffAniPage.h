#pragma once
#include "afxwin.h"

class CsheetWithTabAnim;

// CEffAniPage ��ȭ �����Դϴ�.

class CEffAniPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffAniPage)

protected:
	CsheetWithTabAnim	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	CString m_sAnimName;

public:
	void SetAnimName ( CString sName );

protected:
	void FrameMarking ( BOOL bReset=FALSE );
	void EffectListting ();

public:
	CEffAniPage();
	virtual ~CEffAniPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFANI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonState();
	afx_msg void OnBnClickedButtonDel();
	virtual BOOL OnInitDialog();
	CComboBox m_EffType;
	CListBox m_EffList;
	virtual BOOL OnSetActive();
};
