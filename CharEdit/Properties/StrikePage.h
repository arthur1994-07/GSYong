#pragma once
#include "afxwin.h"


// CStrikePage 대화 상자입니다.
class CsheetWithTabAnim;

class CStrikePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CStrikePage)

protected:
	CsheetWithTabAnim	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	CString m_sAnimName;

public:
	void SetAnimName ( CString sName );
	void ListStrike ();
	void FrameMarking ( BOOL bReset=FALSE );

public:
	CStrikePage();
	virtual ~CStrikePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_STRIKE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_comboPiece;
	CComboBox m_comboEff;
	CListBox m_listStrike;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedOk();
};
