#pragma once
#include "afxwin.h"


// CLandGatePage 대화 상자입니다.
class DxLandGate;

class CLandGatePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLandGatePage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

	BOOL			m_bDlgInit;
	DxLandGate*		m_pLandGate;

public:
	void ListtingLandGate ();
	void UpdateContrl ();
	void ViewContrls ( BOOL bView, BOOL bClear=FALSE );

public:
	CLandGatePage();
	virtual ~CLandGatePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LANDGATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonAdd();
	CListBox m_listLandGate;
	afx_msg void OnBnClickedCheckRend();
	afx_msg void OnBnClickedButtonClone();	
};
