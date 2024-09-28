#pragma once

#include "../resource.h"
// CLevelLandGatePage 대화 상자입니다.
class DxLandGate;
class CsheetWithTab;

class CLevelLandGatePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelLandGatePage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

	BOOL			m_bDlgInit;
	DxLandGate*		m_pLandGate;

public:
	void ActiveLandGateEdit();
	void ListtingLandGate ();
	void UpdateContrl ();
	void ViewContrls ( BOOL bView, BOOL bClear=FALSE );
    void SetGateEffect();

public:
	CLevelLandGatePage();
	virtual ~CLevelLandGatePage();

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
	afx_msg void OnBnClickedCheckRend();
	afx_msg void OnBnClickedButtonClone();
    afx_msg void OnBnClickedButtonGateEff();
    afx_msg void OnBnClickedCheckGateEff();
    afx_msg void OnBnClickedButtonGateEffClear();
	afx_msg void OnBnClickedButtonAddGate();
	afx_msg void OnBnClickedButtonDeleteGate();	
	afx_msg void OnLbnSelchangeListGateTo();	
	
	CListBox m_listLandGate;
	CListBox m_listboxGateTo;
	DWORD m_nSelectGateTo;
};
