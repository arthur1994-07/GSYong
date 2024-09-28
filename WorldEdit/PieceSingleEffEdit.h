#pragma once
#include "afxwin.h"

class CsheetWithTab;

// CPieceSingleEffEdit 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_SINGLEEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
