#pragma once
#include "afxwin.h"
#include "../resource.h"

// CGroupPropPage 대화 상자입니다.
struct EFF_PROPGROUP;
class CsheetWithTab;

class CGroupPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGroupPropPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	EFF_PROPGROUP*			m_pGroupProp;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	void SetProperty ( EFF_PROPGROUP* pGroupProp );

public:
	CGroupPropPage();
	virtual ~CGroupPropPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GROUPPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonok();
	afx_msg void OnBnClickedCheckNeverdie();
	afx_msg void OnBnClickedRadioAverage();
	afx_msg void OnBnClickedRadioSlowfast();
	afx_msg void OnBnClickedRadioFastslow();
};
