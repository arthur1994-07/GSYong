#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CPieceMorphEdit ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PIECE_MORPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonInsert();
	CListBox m_listXFile;
	virtual BOOL OnSetActive();
};
