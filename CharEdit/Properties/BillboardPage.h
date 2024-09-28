#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;

// CBillboardPage 대화 상자입니다.

class CBillboardPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBillboardPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }

	void SetSpinControl( int nOffset1, int nOffset2, int nOffset3, int nOffset4 );

public:
	CBillboardPage();
	virtual ~CBillboardPage();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BILLBOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedOk_Save();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOffsetX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOffsetY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnEnChangeEditOffsetX();
	afx_msg void OnEnChangeEditOffsetY();
	CComboBox m_comboTexWidth;
	CComboBox m_comboTexHeight;
	afx_msg void OnCbnSelchangeComboTexWidth();
	afx_msg void OnCbnSelchangeComboTexHeight();
	afx_msg void OnBnClickedButtonDocument();
};
