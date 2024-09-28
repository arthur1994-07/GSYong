#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CPieceSingleEffEdit ��ȭ �����Դϴ�.

class CPieceSingleEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPieceSingleEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void ReSetListEff();
	void CameraJumpArrowEnable();

public:
	CPieceSingleEffEdit();
	virtual ~CPieceSingleEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PIECE_SINGLEEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	CListBox m_listSingleEff;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnLbnSelchangeListEff();
	afx_msg void OnBnClickedCheckPickPos();
	afx_msg void OnBnClickedButtonCurFind();
};
