#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CPieceMorphEdit 대화 상자입니다.

class CPieceMorphEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPieceMorphEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void ReSetListEff();

public:
	CPieceMorphEdit();
	virtual ~CPieceMorphEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_MORPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonInsert();
	CListBox m_listXFile;
	virtual BOOL OnSetActive();
};
