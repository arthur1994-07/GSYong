#pragma once
#include "afxcmn.h"
#include "afxwin.h"


class CsheetWithTabBoardEdit;
// CBoardEditDlg 대화 상자입니다.

class CBoardEditDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CBoardEditDlg)

protected:
	CsheetWithTabBoardEdit	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabBoardEdit *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdateValueData( bool bSet = TRUE );

public:
	void ResetPage ();

public:
	CBoardEditDlg();
	virtual ~CBoardEditDlg();

	void UpdatePassengerDataList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BOARDPOS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckChartest();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedRadioPosition();
	afx_msg void OnBnClickedRadioRotation();
	afx_msg void OnBnClickedButtonGetbone();
	virtual BOOL OnInitDialog();
	CListCtrl m_list_Passenger;
	afx_msg void OnBnClickedButtonLoadtestchf();
	CListCtrl m_list_TestChar;
	CListBox m_list_TestAnim;
	afx_msg void OnNMDblclkListEditchar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListEditbone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTestanim();
	afx_msg void OnNMClickListEditchar(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedButtonLoadtestchf2();
	afx_msg void OnBnClickedButtonReset1();
	afx_msg void OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinValue1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinValue3(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMClickListEditbone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditBValue1();
	afx_msg void OnEnChangeEditBValue2();
	afx_msg void OnEnChangeEditBValue3();
};
